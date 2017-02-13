#include "StdAfx.h"
#include "cl_Topo_DBMS.h"
#include "cl_DTablePhys.h"
#include "fn_HashFunc.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_DTablePhysFind.cpp"

inline UINT32 _GetCrc32(SFieldInfo * pFieldInfo, BYTE * pBuff, WORD nBuffLen)   {
	UINT32  Crc32 = 0;

	Crc32 = Crc32Tbl[pFieldInfo->iIndex & 0xFF];
//	Crc32 = (((Crc32) >> 8) ^ Crc32Tbl[((Crc32) ^ pFieldInfo->iIndex) & 0xFF]);
	for(unsigned int i=0; i<nBuffLen; i++)     Crc32 = (((Crc32) >> 8) ^ Crc32Tbl[((Crc32) ^ *pBuff++) & 0xFF]);
	Crc32 = (((Crc32) >> 8) ^ Crc32Tbl[((Crc32) ^ nBuffLen) & 0xFF]);
	Crc32 = (((Crc32) >> 8) ^ Crc32Tbl[((Crc32) ^ pFieldInfo->iIndex) & 0xFF]);

	return(Crc32);
}

HRESULT DTablePhysC::SaveNewValue(SFieldInfo * pFieldInfo, BYTE * pValuePtr, WORD nValueLen)   {

	static BYTE aZeroBuff[16];

	if(pValuePtr==NULL)   pValuePtr=aZeroBuff+sizeof(aZeroBuff)/2;

	try   {

		pFieldInfo->pRecordFieldPtr->NextRecordOffset=ZERO_NEXT_RECORD_IDX; // No NEXT
	
		switch(pFieldInfo->eType)   {

			case TYPE_STRING:
			case TYPE_VECTOR:
			case TYPE_WSTRING:
				{
				pFieldInfo->pRecordFieldPtr->StrValueOffsetOrNumValue=m_ValueNewPtr+2;
				WORD * pValuePtr2 = (WORD *)pValuePtr;
				WORD SavedWORD = *(--pValuePtr2);
				*pValuePtr2 = nValueLen+2;
				if(fV.WriteAtEnd(pValuePtr2,nValueLen+2)!=S_OK)   return(E_FAIL);
				*pValuePtr2 = SavedWORD;
				m_ValueNewPtr+=nValueLen+2;
			}
			break;

			case TYPE_BYTE_10:   {
				pFieldInfo->pRecordFieldPtr->StrValueOffsetOrNumValue=m_ValueNewPtr;
				if(fV.WriteAtEnd(pValuePtr,nValueLen)!=S_OK)   return(E_FAIL);
				m_ValueNewPtr+=nValueLen;
			}
			break;

			default:
				pFieldInfo->pRecordFieldPtr->StrValueOffsetOrNumValue=*((UINT32 *)pValuePtr);
			break;
		}
		m_DataGlobalLen+=nValueLen;

	} catch(...)   {
		return(LogFail(_F,ERROR_INVALID_ACCESS,_T("Error during assignement user data to field <%ld> . DataTable <%s>"),pFieldInfo->iIndex,m_Path.c_str()));
	}

	return(S_OK);
}

#define CHECK_HASH_BLOCK_SIZE 16

HRESULT DTablePhysC::FindKeyAndAdd(SFieldInfo * pFieldInfo, BYTE * pValuePtr, WORD nValueLen, BOOL bAddFlag /*= FALSE*/)   {
	HRESULT HR;

	// Not Key work

	if(pFieldInfo->bIsDataFlag)   { // Not Hash work (bAddFlag - Guaranteed)
		if((HR=SaveNewValue(pFieldInfo,pValuePtr,nValueLen))==S_OK)   {
			return(S_OK);
		} else   {
			return(LogFail(_F,HR,_T("New value was not added to DataTable <%s>"),m_Path.c_str()));
		}
	}

	// Key work

	UINT32           HashElemOffset, RecIdxArr[CHECK_HASH_BLOCK_SIZE];
	SHashBlockInfo * HInfoPtr   = m_HashInfoArr;
	UINT32           HashIdx=_GetCrc32(pFieldInfo,pValuePtr,nValueLen); // , HashReCnt;
	UINT32           HashIdxHighBits = HashIdx & HASH_IDX_HIGH_BITS_MASK;

	for(UINT32 i=0; i<m_HashBlocksNum; i++,HInfoPtr++)   {

		// HashReCnt=0;

		HashElemOffset = HInfoPtr->uiOffsetOfStartOfHashBlock + SIZE_OF_RECORD_OFFSET * (HashIdx % HInfoPtr->uiHashBlockVolume);
		if(fH.SetPosition(HashElemOffset)!=S_OK)   {
			LogFail(_F,0,_T("Position (%ld) in HASH-table was not found <%s>."),HashElemOffset,m_Path.c_str());
			return(E_FAIL);
		}

		int iTryNum;
		if((iTryNum = INT(HInfoPtr->uiOffsetOfEndOfHashBlock - HashElemOffset) / SIZE_OF_RECORD_OFFSET) > CHECK_HASH_BLOCK_SIZE)   iTryNum = CHECK_HASH_BLOCK_SIZE;

		do   { // First hashing and posible rehashing

			if(fH.Read(&RecIdxArr, iTryNum * SIZE_OF_RECORD_OFFSET)!=S_OK)   {
				LogFail(_F,0,_T("Value from HASH-table was not read <%s>."),m_Path.c_str());
				return(E_FAIL);
			}
			
			int iTryCnt = 0;
			for(iTryCnt=0; iTryCnt<iTryNum; iTryCnt++)   {

				// Is this slot empty?
				if(RecIdxArr[iTryCnt] == ZERO_HASH_RECORD)   { // Empty slot (No key in this block)

					if(bAddFlag)   {

						if(i+1==m_HashBlocksNum)   { // We have <Last block> && <Add Flag>

							// Add procedure (new key/key value)

							// Check row number
							if(m_RecordsNum & HASH_IDX_HIGH_BITS_MASK)   {
								LogFail(_F,0,_T("Row with number %ld exceeds defined limit <%s>."),m_RecordsNum,m_Path.c_str());
								return(E_FAIL);
							}

							// Save value
							if((HR=SaveNewValue(pFieldInfo,pValuePtr,nValueLen))!=S_OK)   {
								LogFail(_F,HR,_T("New value was not saved <%s>."),m_Path.c_str());
								return(E_FAIL);
							}

							// Save blocks of records' pointers
							RecIdxArr[iTryCnt]= m_RecordsNum | HashIdxHighBits; // Pointer to unit of record
							if(fH.WriteAtBackOffset(&RecIdxArr,iTryNum * SIZE_OF_RECORD_OFFSET)!=S_OK)   {
								LogFail(_F,0,_T("blocks of records' pointers was not written <%s>."),m_Path.c_str());
								return(E_FAIL);
							}

							if(pFieldInfo->bIndexedFlag)   {
								m_iCurrNodeHashIdx  = fH.GetPosition() - ( (iTryNum - iTryCnt) * SIZE_OF_RECORD_OFFSET);
								m_iCurrNodeTableIdx = m_RecordsNum;
//printf("[%lX-%lX] ",m_iCurrNodeHashIdx,RecIdxArr[iTryCnt] & ~UINT32(HASH_IDX_HIGH_BITS_MASK));
							}

							// New HASH-bock
							if(++m_HashElemsNum == HInfoPtr->uiHashBlockGlobalLimit)   {
								m_HashBlocksNum++;
								SHashBlockInfo* temp;
								if( (temp = (SHashBlockInfo*) realloc( m_HashInfoArr, m_HashBlocksNum * sizeof(SHashBlockInfo)) ) == NULL )
								{
									return(LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("NoReMemoryForHashInfo")));
								}
								else
								{
									m_HashInfoArr = temp;
								}
								ReCountHashInfoArray();
								fH.Fill(m_HashInfoArr[m_HashBlocksNum-1].uiHashBlockVolumeInBytes,0xFF);
							}

							return(S_OK); // We added new elem

						}
					}

					// Find procedure || (Add && not last hash block)

					break; // Do not check next Hash Incriment (Go To Next Hash Block)

				} else   { // Not empty RPTR_IDX

					if((RecIdxArr[iTryCnt] & HASH_IDX_HIGH_BITS_MASK) == HashIdxHighBits)   {

						UINT32 RecIdx_iTryCnt = RecIdxArr[iTryCnt] & ~UINT32(HASH_IDX_HIGH_BITS_MASK);

						if(RecIdx_iTryCnt < m_RecordsNum)   { // Ptr to existing record

							// Getting RECORD_UNIT
							if(fR.ReadAt((RecIdx_iTryCnt*m_RecordSize)+pFieldInfo->iRecordFieldBegOffset,pFieldInfo->pRecordFieldPtr,sizeof(UINT32))!=S_OK)   {
								LogFail(_F,0,_T("Row Field was not read <%s>."),m_Path.c_str());
								return(E_FAIL);
							}

							bool bItIsAKey;

							// Getting NAME_PTR
							if(pFieldInfo->iSize<=sizeof(UINT32))   {

								bItIsAKey = (UINT32)(pFieldInfo->pRecordFieldPtr->StrValueOffsetOrNumValue) == *((UINT32 *)pValuePtr);

							} else   {
								if(nValueLen>m_ValueBuffLen)   {
									m_ValueBuffLen = nValueLen + 2;
									BYTE* temp;
									if( (temp = (BYTE*) realloc( m_ValueBuff, m_ValueBuffLen) ) == NULL )
									{
										return(LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("NoMemoryForValueBuffer")));
									}
									else
									{
										m_ValueBuff = temp;
									}
								}
								if(fV.ReadAt(pFieldInfo->pRecordFieldPtr->StrValueOffsetOrNumValue,m_ValueBuff,nValueLen)!=S_OK)   {
									LogFail(_F,0,_T("Value of std::string or std::vector value was not read <%s>."),m_Path.c_str());
									return(E_FAIL);
								}
								bItIsAKey = !memcmp(m_ValueBuff,pValuePtr,nValueLen);
							}

							if(bItIsAKey)   { // Same Name

								if(bAddFlag)   {

									// Add procedure (existing key/key value)

									if(pFieldInfo->bIsUnicFlag)   {
										LogFail(_F,0,_T("Unsuccessfull try to save duplicated value for UNIC DataField"));
										return(S_FALSE);
									}

									// Check row number
									if(m_RecordsNum & HASH_IDX_HIGH_BITS_MASK)   {
										LogFail(_F,0,_T("Row with number %ld exceeds defined limit <%s>."),m_RecordsNum,m_Path.c_str());
										return(E_FAIL);
									}

									if(pFieldInfo->bHasNextFlag)   pFieldInfo->pRecordFieldPtr->NextRecordOffset = RecIdx_iTryCnt * m_RecordSize;
									RecIdxArr[iTryCnt]= m_RecordsNum | HashIdxHighBits;
									if(fH.WriteAtBackOffset(&RecIdxArr, iTryNum * SIZE_OF_RECORD_OFFSET)!=S_OK)   {
										LogFail(_F,0,_T("blocks of records' pointers was not written <%s>."),m_Path.c_str());
										return(E_FAIL);
									}

									m_DataGlobalLen+=nValueLen;
									return(S_OK);

								} else   { // It was only find procedure
									m_Find_RecordFoundIdx[m_Find_CurrId] = RecIdx_iTryCnt;
									m_Find_RecordNextOffs[m_Find_CurrId] = RecIdx_iTryCnt * m_RecordSize;
									return(S_OK); // We found it ! (Just found)
								}

							} else   { // else - Negativ <memcmp> or

								if(bAddFlag)   {
									m_ReValueNum++;
//									printf("%lX = %lX\n",RecIdxArr[iTryCnt],HashIdxHighBits);
								}
							}
						} else   { // else - Ptr to not yet existing record ( surely diferent key but same hash idx)
							// static int C=0;
							// printf(" <NO_RECORD %ld>",++C);
						}
					} else   { // Not desired field idx

						if(bAddFlag)   m_ReTryNum++;

						//if(bAddFlag)   {
						//   if(HashReCnt==0)   {
						//      m_HashCollFld1++;
						//   } else if(HashReCnt==1)   {
						//      m_HashCollFld2++;
						//   } else   {
						//      m_HashCollFldM++;
						//   }
						//}
					}
				} // End of <Not empty RPTR_IDX>

				//// All cases of rehashing
				//if(bAddFlag)   {
				//   if(HashReCnt==0)   {
				//      m_HashCollNum1++;
				//   } else if(HashReCnt==1)   {
				//      m_HashCollNum2++;
				//   } else   {
				//      m_HashCollNumM++;
				//   }
				//}
				// HashReCnt++;
			} // for(int iTryCnt=0; iTryCnt<iTryNum; iTryCnt++)   {

			if(iTryCnt<iTryNum)   {
				break;
			} else   {

				if(bAddFlag)   m_ReReadNum++;

				if((HashElemOffset += (SIZE_OF_RECORD_OFFSET * iTryNum)) >= HInfoPtr->uiOffsetOfEndOfHashBlock)   {
					HashElemOffset = HInfoPtr->uiOffsetOfStartOfHashBlock;
					if(fH.SetPosition(HashElemOffset)!=S_OK)   {
						LogFail(_F,0,_T("Position (%ld) in HASH-table was not found <%s>."),HashElemOffset,m_Path.c_str());
						return(E_FAIL);
					}
				}
				if((iTryNum = INT(HInfoPtr->uiOffsetOfEndOfHashBlock - HashElemOffset) / SIZE_OF_RECORD_OFFSET) > CHECK_HASH_BLOCK_SIZE)   iTryNum = CHECK_HASH_BLOCK_SIZE;
			}

		} while(1); // End of hashing/rehashing circle
	}

	if(!bAddFlag)   m_Find_RecordFoundIdx[m_Find_CurrId] = 0xFFFFFFFF;

	return(S_FALSE);
}

HRESULT DTablePhysC::DeleteRecord(UINT32 nRecordIdx)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	HRESULT HR=S_OK;
	BYTE    bChkByte;

	if(nRecordIdx>=m_RecordsNum)   return(S_FALSE);

	if((HR=fR.ReadAt((nRecordIdx+1)*m_RecordSize-1,&bChkByte,1))!=S_OK)   {
		return(LogFail(_F,HR,_T("CTRL byte reading error. DataTable <%s>"),m_Path.c_str()));
	}

	if(bChkByte == 0)   {
		m_DeletedRecordsNum++;
		bChkByte = 0x01;
		if((HR=fR.WriteAt((nRecordIdx+1)*m_RecordSize-1,&bChkByte,1))!=S_OK)   {
			return(LogFail(_F,HR,_T("CTRL byte writing error. DataTable <%s>"),m_Path.c_str()));
		}
	}

	return(HR);
}

HRESULT DTablePhysC::Shrink(VOID)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(m_DeletedRecordsNum == 0)   return(S_FALSE);
	if(m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("SHRINK: Operation can't be performed because DataTable <%s> is still open"),m_Path.c_str()));

	LogMess(_T("SHRINK of DataTable <%s>. Number of deleted records is grater then %ld%%"),m_Path.c_str(),m_DeletedRecordsNum*100/m_RecordsNum);

	HRESULT HR=S_OK;

	if(Open()!=S_OK)   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("SHRINK: Open old table error. DataTable <%s> "),m_Path.c_str()));

	#pragma warning( disable : 4291)
	DTablePhysC * poDuplTable = new DTablePhysC();
	if(poDuplTable == NULL)   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("SHRINK: Making of new instance of table error. DataTable <%s> "),m_Path.c_str()));
	#pragma warning( default : 4291)

	poDuplTable->m_TopoPtr                = m_TopoPtr;
	poDuplTable->m_FieldsNum              = m_FieldsNum;
	poDuplTable->m_HashBlockStartUserSize = m_HashBlockStartUserSize;
	poDuplTable->m_bTableIsDynamic        = m_bTableIsDynamic;
	poDuplTable->m_bTableIsTemporary      = TRUE;
	poDuplTable->m_Path                   = m_Path;
	poDuplTable->SetDTFilesPathsAndNames();

	poDuplTable->m_TopoPtr->m_ReferenceCounter = 0; // this & poDuplTable share the same TopoPtr so we reclear OpenFlag
	if(poDuplTable->Open()!=S_OK)   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("SHRINK: Open new table error. DataTable <%s> "),poDuplTable->m_Path.c_str()));

	memcpy(poDuplTable->m_UserValues,m_UserValues,sizeof(m_UserValues));

	VOID ** ppvDataAtt = AllocRecordDataStorage();
	if(ppvDataAtt)   {

		poDuplTable->AddInit(ppvDataAtt);

		UINT32 iCnt=0;
		ScanInit(0,ppvDataAtt);
		while(Scan()==S_OK)   {
//			printf("%ld %s\n",((std::vector<BYTE> *)ppvDataAtt[6])->size(),(BYTE *)&((std::vector<BYTE> *)ppvDataAtt[6])->front());
			poDuplTable->Add();
			iCnt++;
		}
		FreeRecordDataStorage(ppvDataAtt);

		m_DeletedRecordsNum = 0;
		m_ClearFlag         = TRUE; // Do not save statistics
		if(Close(TRUE)!=S_OK)   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("SHRINK: Close old table error. DataTable <%s> "),m_Path.c_str()));
		m_ClearFlag         = FALSE;

		poDuplTable->m_TopoPtr->m_ReferenceCounter = 1; // this & poDuplTable share the same TopoPtr so we reset OpenFlag

		if(poDuplTable->Close()!=S_OK)   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("SHRINK: Close new table error. DataTable <%s> "),m_Path.c_str()));
		
		delete poDuplTable;

	} else   {
		return(LogFail(_F,ERROR_OUTOFMEMORY,_T("SHRINK: Memory for data exchange was not allocated. DataTable <%s> "),m_Path.c_str()));
	}

	return(HR);
}

HRESULT DTablePhysC::AddInit(VOID * ppArrayOfPointersToData[])   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	SFieldInfo * pFieldInfo = m_FieldsInfoArr;

	m_Add_UserDataPointers=ppArrayOfPointersToData;

	// Add extra protection
	for(unsigned int i=0; i<m_FieldsNum; i++, pFieldInfo++)   {
		if(m_Add_UserDataPointers[i]==NULL && !pFieldInfo->bIsDataFlag)   return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("NullFieldDataPointer")));
	}

	return(S_OK);
}

HRESULT DTablePhysC::ChangeInit(VOID * ppArrayOfPointersToData[])   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	if(ppArrayOfPointersToData==NULL)   return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("ppArrayOfPointersToData==NULL. DataTable <%s>"),m_Path.c_str()));
	m_Change_UserDataPointers = ppArrayOfPointersToData;
	return(S_OK);
}


VOID **  DTablePhysC::AllocRecordDataStorage(VOID)   {

	VOID ** ppvppArrayOfPointersToData = (VOID **) calloc(m_FieldsNum,sizeof(VOID *));

	if(ppvppArrayOfPointersToData)   {
		SFieldInfo * pFieldInfo = m_FieldsInfoArr;
		for(unsigned int i=0; i<m_FieldsNum; i++, pFieldInfo++)   {
			switch(pFieldInfo->eType)   {

				#pragma warning( disable : 4291 )

				case TYPE_STRING:
					ppvppArrayOfPointersToData[i] = new std::string;
				break;

				case TYPE_VECTOR:
					ppvppArrayOfPointersToData[i] = new std::vector<BYTE>;
				break;

				case TYPE_WSTRING:
					ppvppArrayOfPointersToData[i] = new std::wstring;
				break;

				#pragma warning( default : 4291 )

				case TYPE_BYTE_10:
					ppvppArrayOfPointersToData[i] = new BYTE[10];
				break;

				default:
					ppvppArrayOfPointersToData[i] = new UINT32;
				break;
			}

			if(ppvppArrayOfPointersToData[i] == NULL)   {
				FreeRecordDataStorage(ppvppArrayOfPointersToData);
				LogFail(_F,ERROR_OUTOFMEMORY,_T("No memory for RowDataArray[%ld] element. DataTable <%s>"),i,m_Path.c_str());
				return(NULL);
			}
		}
		return(ppvppArrayOfPointersToData);
	} else   {
		LogFail(_F,ERROR_OUTOFMEMORY,_T("No memory for RowDataArray. DataTable <%s>"),m_Path.c_str());
		return(NULL);
	}
}

HRESULT  DTablePhysC::FreeRecordDataStorage(VOID ** ppvppArrayOfPointersToData)   {
	if(ppvppArrayOfPointersToData)   {
		SFieldInfo * pFieldInfo = m_FieldsInfoArr;
		for(unsigned int i=0; i<m_FieldsNum; i++, pFieldInfo++)   {
			switch(pFieldInfo->eType)   {
				case TYPE_STRING:
					delete (std::string *)ppvppArrayOfPointersToData[i];
				break;

				case TYPE_VECTOR:
					delete (std::vector<BYTE> *)ppvppArrayOfPointersToData[i];
				break;

				case TYPE_WSTRING:
					delete (std::wstring *)ppvppArrayOfPointersToData[i];
				break;

				case TYPE_BYTE_10:
					delete (BYTE *)ppvppArrayOfPointersToData[i];
				break;

				default:
					delete (UINT32 *)ppvppArrayOfPointersToData[i];
				break;
			}
		}
		free(ppvppArrayOfPointersToData);
		return(S_OK);
	} else   {
		return(S_FALSE);
	}
}

HRESULT DTablePhysC::Add(VOID)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	HRESULT         HR;
	BYTE         *  pValuePtr;
	WORD            iValueLen;
	SFieldInfo   *  pFieldInfo;

	static const BYTE achEmptyFieldDataBuff[10] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

	if(m_Add_UserDataPointers==NULL)   return(LogFail(_F,ERROR_NOT_READY,_T("Method AddInit() was not called. DataTable <%s>"),m_Path.c_str()));

//	LogMess(_T("!!! ADDING in DataTable <%s>"),m_Path.c_str());

	try   {
		if(m_bTableHasUnicFields)   {
			pFieldInfo = m_FieldsInfoArr;
			for(unsigned int i=0; i<m_FieldsNum; i++, pFieldInfo++)   {

				if(pFieldInfo->bIsUnicFlag)   { // UNIC

					switch(pFieldInfo->eType)   {
						case TYPE_STRING:
							pValuePtr=(BYTE *)((std::string *)m_Add_UserDataPointers[i])->c_str();
							iValueLen=WORD(((std::string *)m_Add_UserDataPointers[i])->size())+1;
							if(iValueLen>=STR_BUFF_MAX_SIZE)   return(LogFail(_F,ERROR_BAD_LENGTH,_T("Too long std::string passed to save. DataTable <%s>"),m_Path.c_str()));
						break;

						case TYPE_VECTOR:
							pValuePtr=(BYTE *)&((std::vector<BYTE> *)m_Add_UserDataPointers[i])->front();
							iValueLen=WORD(((std::vector<BYTE> *)m_Add_UserDataPointers[i])->size());
							if(iValueLen>=STR_BUFF_MAX_SIZE)   return(LogFail(_F,ERROR_BAD_LENGTH,_T("Too long std::vector passed to save. DataTable <%s>"),m_Path.c_str()));
						break;

						case TYPE_WSTRING:
							pValuePtr=(BYTE *)((std::wstring *)m_Add_UserDataPointers[i])->c_str();
							iValueLen=(WORD(((std::wstring *)m_Add_UserDataPointers[i])->size())+1)<<1;
							if(iValueLen>=STR_BUFF_MAX_SIZE)   return(LogFail(_F,ERROR_BAD_LENGTH,_T("Too long std::wstring passed to save. DataTable <%s>"),m_Path.c_str()));
						break;

						default:
							pValuePtr=(BYTE *)m_Add_UserDataPointers[i];
							if(pValuePtr==NULL)   pValuePtr=(BYTE *)achEmptyFieldDataBuff;
							iValueLen=pFieldInfo->iSize;
						break;
					}

					m_iCurrNodeHashIdx = -1;

					if(FindKeyAndAdd(pFieldInfo,pValuePtr,iValueLen,FALSE)==S_OK)   {
						// Duplicated value for UNIC DataField
						return(S_FALSE);
					}
				}
			}
		}

		pFieldInfo = m_FieldsInfoArr;
		for(unsigned int i=0; i<m_FieldsNum; i++, pFieldInfo++)   {
			switch(pFieldInfo->eType)   {
				case TYPE_STRING:
					pValuePtr=(BYTE *)((std::string *)m_Add_UserDataPointers[i])->c_str();
					iValueLen=WORD(((std::string *)m_Add_UserDataPointers[i])->size())+1;
					if(iValueLen>=STR_BUFF_MAX_SIZE)   return(LogFail(_F,ERROR_BAD_LENGTH,_T("Too long std::string passed to save. DataTable <%s>"),m_Path.c_str()));
				break;

				case TYPE_VECTOR:
					pValuePtr=(BYTE *)&((std::vector<BYTE> *)m_Add_UserDataPointers[i])->front();
					iValueLen=WORD(((std::vector<BYTE> *)m_Add_UserDataPointers[i])->size());
					if(iValueLen>=STR_BUFF_MAX_SIZE)   return(LogFail(_F,ERROR_BAD_LENGTH,_T("Too long std::vector passed to save. DataTable <%s>"),m_Path.c_str()));
				break;

				case TYPE_WSTRING:
					pValuePtr=(BYTE *)((std::wstring *)m_Add_UserDataPointers[i])->c_str();
					iValueLen=(WORD(((std::wstring *)m_Add_UserDataPointers[i])->size())+1)<<1;
					if(iValueLen>=STR_BUFF_MAX_SIZE)   return(LogFail(_F,ERROR_BAD_LENGTH,_T("Too long std::wstring passed to save. DataTable <%s>"),m_Path.c_str()));
				break;

				default:
					pValuePtr=(BYTE *)m_Add_UserDataPointers[i];
					if(pValuePtr==NULL)   pValuePtr=(BYTE *)achEmptyFieldDataBuff;
					iValueLen=pFieldInfo->iSize;
				break;
			}

			m_iCurrNodeHashIdx = -1;

			if((HR=FindKeyAndAdd(pFieldInfo,pValuePtr,iValueLen,TRUE))!=S_OK)   return(HR);

			if(pFieldInfo->bIndexedFlag && m_iCurrNodeHashIdx!=-1)   {
				m_SpanFind_FieldInfoPtr = pFieldInfo;
				m_pCurrNodeValuePtr   = pValuePtr;
//				m_iCurrNodeValueLen   = iValueLen;
//				m_iCurrNodeHashIdx    = // See above
//				if(pFieldInfo->eType==TYPE_STRING)   printf("\n%s ",pValuePtr);
//				printf("%ld ",m_iCurrNodeHashIdx);
				NodeAdd(m_iCurrNodeHashIdx);
			}

		}
	} catch(...)   {
		return(LogFail(_F,ERROR_INVALID_ACCESS,_T("Error during obtaining table-field <%ld> data info. DataTable <%s>"),pFieldInfo->iIndex,m_Path.c_str()));
	}

	((BYTE *)m_RecordArr)[m_RecordSize-1] = 0x00; // Extra byte
	if(fR.WriteAtEnd(m_RecordArr,m_RecordSize)!=S_OK)   return(LogFail(_F,0,_T("New record was not written. DataTable <%s>"),m_Path.c_str()));
	m_RecordsNum++;
	m_RecordNewPtr+=m_RecordSize;

	if(m_SaveAfterValue && ++m_SaveAfterCnt>=m_SaveAfterValue)   {
		SaveBackup();
	}

	return(S_OK);
}

BOOL DTablePhysC::GetFreeFindId(WORD * pnFindId)   {
	for(int i=0; i<FIND_LIM; i++)   {
		if(m_Find_ppArrayOfPointersToData[i]==NULL)   {
			*pnFindId=i;
			return(TRUE);
		}
	}

	return(FALSE);
}

HRESULT DTablePhysC::FindInitEx(_TCHAR   * pKeyName,  VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD * pnFindId)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())     return(LogFail(_F,ERROR_ACCESS_DENIED,    _T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(pnFindId==NULL)           return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Pointer to <nFindId> is NULL (DataTable <%s>)"),m_Path.c_str()));
	if(!GetFreeFindId(pnFindId)) return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("No free <FindId> (DataTable <%s>)"),m_Path.c_str()));

	return(FindInit(pKeyName,pKeyValue,ppArrayOfPointersToData,*pnFindId));
}

HRESULT DTablePhysC::FindInitEx(UINT32   nKeyIndex, VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD * pnFindId)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())     return(LogFail(_F,ERROR_ACCESS_DENIED,    _T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(pnFindId==NULL)           return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Pointer to <nFindId> is NULL (DataTable <%s>)"),m_Path.c_str()));
	if(!GetFreeFindId(pnFindId)) return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("No free <FindId> (DataTable <%s>)"),m_Path.c_str()));

	return(FindInit(nKeyIndex,pKeyValue,ppArrayOfPointersToData,*pnFindId));
}

HRESULT DTablePhysC::FindInit(_TCHAR * pKeyName, VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD nFindId)  {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	DFieldTopoC * pDField=m_TopoPtr->FindChild(pKeyName);

	if(pDField)   {
		return(FindInit(pDField->m_IndexInParentMap,pKeyValue,ppArrayOfPointersToData,nFindId));
	} else   {
		return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Key-name <%s> was not found (DataTable <%s>)"),pKeyName,m_Path.c_str()));
	}
}

HRESULT DTablePhysC::FindInit(UINT32 nKeyIndex, VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD nFindId)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	if(nFindId>=FIND_LIM)                        return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Find ID <%d> is out of range. DataTable <%s>"),nFindId,m_Path.c_str()));
	if(nKeyIndex>=m_FieldsNum)                   return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Index of DataField  is out of range. DataTable <%s>"),m_Path.c_str()));
	if(m_FieldsInfoArr[nKeyIndex].bIsDataFlag)   return(LogFail(_F,ERROR_NOT_SUPPORTED,_T("The specified filed N %ld has <IS_DATA> modifier. DataTable <%s>"),nKeyIndex,m_Path.c_str()));
	if(m_FieldsInfoArr[nKeyIndex].bNoFindFlag)   return(LogFail(_F,ERROR_NOT_SUPPORTED,_T("The specified filed N %ld has <NO_FIND> modifier. DataTable <%s>"),nKeyIndex,m_Path.c_str()));
	if(pKeyValue==NULL)                          return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Parameter <KeyValuePointer> is NULL. DataTable <%s>"),m_Path.c_str()));
//	if(ppArrayOfPointersToData==NULL)              return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Parameter <ppArrayOfPointersToData> is NULL. DataTable <%s>"),m_Path.c_str()));
	if(m_Find_ppArrayOfPointersToData[nFindId])    return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Find ID <%ld> is already busy. DataTable <%s>"),nFindId,m_Path.c_str()));

	m_Find_RecordFieldIdx[nFindId]        = nKeyIndex;
	m_Find_RecordNextOffs[nFindId]        = ZERO_NEXT_RECORD_IDX;
	m_Find_RecordFoundIdx[nFindId]        = 0xFFFFFFFF;
	m_Find_AnswerNoMore[nFindId]          = TRUE;
	m_Find_KeyValuePtr[nFindId]           = pKeyValue;
	m_Find_ppArrayOfPointersToData[nFindId] = ppArrayOfPointersToData?ppArrayOfPointersToData:DUMMY_USER_ARRAY_OF_POINTERS_TO_DATA;

	return(S_OK);
}

HRESULT DTablePhysC::FindCancel(WORD nFindId)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,    _T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(nFindId>=FIND_LIM)      return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Find ID <%d> is out of range. DataTable <%s>"),nFindId,m_Path.c_str()));

	m_Find_RecordFieldIdx[nFindId]        = 0;
	m_Find_RecordNextOffs[nFindId]        = ZERO_NEXT_RECORD_IDX;
	m_Find_RecordFoundIdx[nFindId]        = 0xFFFFFFFF;
	m_Find_AnswerNoMore[nFindId]          = TRUE;
	m_Find_KeyValuePtr[nFindId]           = NULL;
	m_Find_ppArrayOfPointersToData[nFindId] = NULL;

	return(S_OK);
}

HRESULT DTablePhysC::FindStart(WORD nFindId)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	BYTE         *  pValuePtr;
	WORD            iValueLen;
	SFieldInfo * pFieldInfo = m_FieldsInfoArr + m_Find_RecordFieldIdx[nFindId];

	if(nFindId>=FIND_LIM)    return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Find ID <%d> is out of range. DataTable <%s>"),nFindId,m_Path.c_str()));
	if(m_Find_ppArrayOfPointersToData[nFindId]==NULL)   return(LogFail(_F,ERROR_NOT_READY,_T("Method <FindInit()> was not called for DataTable <%s>"),m_Path.c_str()));
	// || m_Find_KeyValuePtr[nFindId]==NULL

	try   {
		switch(pFieldInfo->eType)   {
			case TYPE_STRING:
				pValuePtr=(BYTE *)((std::string *)m_Find_KeyValuePtr[nFindId])->c_str();
				iValueLen=WORD(((std::string *)m_Find_KeyValuePtr[nFindId])->size())+1;
			break;

			case TYPE_VECTOR:
				pValuePtr=(BYTE *)&((std::vector<BYTE> *)m_Find_KeyValuePtr[nFindId])->front();
				iValueLen=WORD(((std::vector<BYTE> *)m_Find_KeyValuePtr[nFindId])->size());
			break;

			case TYPE_WSTRING:
				pValuePtr=(BYTE *)((std::wstring *)m_Find_KeyValuePtr[nFindId])->c_str();
				iValueLen=(WORD(((std::wstring *)m_Find_KeyValuePtr[nFindId])->size())+1)<<1;
			break;

			default:
				pValuePtr=(BYTE *)m_Find_KeyValuePtr[nFindId];
				iValueLen=pFieldInfo->iSize;
			break;
		}
	} catch(...)   {
		return(LogFail(_F,ERROR_INVALID_ACCESS,_T("Error during obtaining data-field <%ld> info. DataTable <%s>"),pFieldInfo->iIndex,m_Path.c_str()));
	}


	m_Find_AnswerNoMore[nFindId]=FALSE;
	m_Find_CurrId=nFindId;
	return(FindKeyAndAdd(pFieldInfo,pValuePtr,iValueLen));
}

HRESULT DTablePhysC::FindStop(WORD nFindId)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())  return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	if(nFindId>=FIND_LIM)    return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Find ID <%d> is out of range. DataTable <%s>"),nFindId,m_Path.c_str()));

	m_Find_AnswerNoMore[nFindId]   = TRUE;
	m_Find_RecordNextOffs[nFindId] = ZERO_NEXT_RECORD_IDX;
//	m_Find_RecordFoundIdx[nFindId] = 0xFFFFFFFF;

	return(S_OK);
}

HRESULT DTablePhysC::FindOne(WORD nFindId)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	HRESULT HR;

	if(nFindId>=FIND_LIM)    return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Find ID <%d> is out of range. DataTable <%s>"),nFindId,m_Path.c_str()));

	FindStop(nFindId);
	HR=Find(nFindId);
	FindStop(nFindId);

	return(HR);
}

HRESULT DTablePhysC::FindOneAndChange(WORD nFindId)  {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	HRESULT HR;

	if(nFindId>=FIND_LIM)    return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Find ID <%d> is out of range. DataTable <%s>"),nFindId,m_Path.c_str()));

	FindStop(nFindId);
	HR=FindAndChange(nFindId);
	FindStop(nFindId);

	return(HR);
}

/*
HRESULT DTablePhysC::FindOneAndInc(WORD nFindId)  {
	VISIT_CRIT_SECT(m_oCriticalSection);
	HRESULT HR;

	m_bIncFlag=TRUE;
	HR=FindOneAndChange(nFindId);
	m_bIncFlag=FALSE;

	return(HR);
}
*/

HRESULT DTablePhysC::Find(WORD nFindId)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	HRESULT HR;

	if(nFindId>=FIND_LIM)    return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Find ID <%d> is out of range. DataTable <%s>"),nFindId,m_Path.c_str()));

	while(1)   {
		if(m_Find_AnswerNoMore[nFindId])   {
			if((HR=FindStart(nFindId))!=S_OK)   {
				m_Find_AnswerNoMore[nFindId]=TRUE;
				return(HR);
			}
		} else   {
			if(m_Find_RecordNextOffs[nFindId]==ZERO_NEXT_RECORD_IDX)   {
				m_Find_AnswerNoMore[nFindId]=TRUE;
				return(S_FALSE);
			}
		}

		m_Find_RecordFoundIdx[nFindId] = m_Find_RecordNextOffs[nFindId]/m_RecordSize;

		if((HR=fR.ReadAt(m_Find_RecordNextOffs[nFindId],m_RecordArr,m_RecordSize))!=S_OK)   {
			return(LogFail(_F,HR,_T("Row reading error. DataTable <%s>"),m_Path.c_str()));
		}

		if(((BYTE *)m_RecordArr)[m_RecordSize-1] == 0x00)   { // Extra byte
			if((HR=GetRecordData(m_Find_ppArrayOfPointersToData[nFindId],nFindId,TRUE))!=S_OK)   {
				return(LogFail(_F,HR,_T("Data reding error. DataTable <%s>"),m_Path.c_str()));
			} else   {
				return(S_OK);
			}
		} else   {
			SFieldInfo * pFieldInfo = m_FieldsInfoArr+m_Find_RecordFieldIdx[nFindId];
			if(pFieldInfo->bIsUnicFlag)   {
				m_Find_RecordNextOffs[nFindId]=ZERO_NEXT_RECORD_IDX;
			} else   {
				m_Find_RecordNextOffs[nFindId]=pFieldInfo->pRecordFieldPtr->NextRecordOffset;
			}
		}
	}
}

HRESULT DTablePhysC::FindAndChange(WORD nFindId)  {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	HRESULT HR;

	if(nFindId>=FIND_LIM)    return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Find ID <%d> is out of range. DataTable <%s>"),nFindId,m_Path.c_str()));
	if(m_Change_UserDataPointers==NULL)   return(LogFail(_F,ERROR_NOT_READY,_T("Method ChangeInit() was not called. DataTAble <%s>"),m_Path.c_str()));

	while(1)   {
		if(m_Find_AnswerNoMore[nFindId])   {
			if((HR=FindStart(nFindId))!=S_OK)   {
				m_Find_AnswerNoMore[nFindId]=TRUE;
				return(HR);
			}
		} else   {
			if(m_Find_RecordNextOffs[nFindId]==ZERO_NEXT_RECORD_IDX)   {
				m_Find_RecordFoundIdx[nFindId] = 0xFFFFFFFF;
				m_Find_AnswerNoMore[nFindId]=TRUE;
				return(S_FALSE);
			}
		}

		m_Find_RecordFoundIdx[nFindId] = m_Find_RecordNextOffs[nFindId]/m_RecordSize;

		if((HR=fR.ReadAt(m_Find_RecordNextOffs[nFindId],m_RecordArr,m_RecordSize))!=S_OK)   {
			return(LogFail(_F,HR,_T("Row reading error. DataTable <%s>"),m_Path.c_str()));
		}

		if(((BYTE *)m_RecordArr)[m_RecordSize-1] == 0x00)   { // Extra byte
			if((HR=SetRecordData(m_Change_UserDataPointers,nFindId,TRUE))!=S_OK)   {
				return(LogFail(_F,HR,_T("Error during changing record data. DataTable <%s>"),m_Path.c_str()));
			} else   {
				return(S_OK);
			}
		} else   {
			SFieldInfo * pFieldInfo = m_FieldsInfoArr+m_Find_RecordFieldIdx[nFindId];
			if(pFieldInfo->bIsUnicFlag)   {
				m_Find_RecordNextOffs[nFindId]=ZERO_NEXT_RECORD_IDX;
			} else   {
				m_Find_RecordNextOffs[nFindId]=pFieldInfo->pRecordFieldPtr->NextRecordOffset;
			}
		}
	}
}


HRESULT DTablePhysC::GetRecordData(VOID * pDataPtrArr[], WORD nId, BOOL bFindMode)   {

	if(pDataPtrArr==DUMMY_USER_ARRAY_OF_POINTERS_TO_DATA)   {
		if(bFindMode)   {
			SFieldInfo * pFieldInfo = m_FieldsInfoArr + m_Find_RecordFieldIdx[nId];
			if(pFieldInfo->bIsUnicFlag)   {
				m_Find_RecordNextOffs[nId]=ZERO_NEXT_RECORD_IDX;
			} else   {
				m_Find_RecordNextOffs[nId]=pFieldInfo->pRecordFieldPtr->NextRecordOffset;
			}
		}
		return(S_OK);
	}

	UINT32        nValueOffset   = 0;
	SFieldInfo * pFieldInfo = m_FieldsInfoArr;

	for(unsigned int i=0; i<m_FieldsNum; i++,pFieldInfo++)   {

		if(bFindMode && i==m_Find_RecordFieldIdx[nId])   {
			if(pFieldInfo->bIsUnicFlag)   {
				m_Find_RecordNextOffs[nId]=ZERO_NEXT_RECORD_IDX;
			} else   {
				m_Find_RecordNextOffs[nId]=pFieldInfo->pRecordFieldPtr->NextRecordOffset;
			}
		}

		if(pDataPtrArr[i]==NULL) continue;

		if(bFindMode)   {

			if(pDataPtrArr[i] == m_Find_KeyValuePtr[nId])   continue;

			if(i==m_Find_RecordFieldIdx[nId])   {
				try   {
					switch(pFieldInfo->eType)   {
						case TYPE_INT32:
						case TYPE_UINT32:  *((UINT32 *)pDataPtrArr[i])=*((UINT32 *)m_Find_KeyValuePtr[nId]);                    break;
						case TYPE_BYTE_10: memcpy(pDataPtrArr[i],m_Find_KeyValuePtr[nId],10);                                    break;
						case TYPE_STRING:  ((std::string *)pDataPtrArr[ i])->assign(*((std::string * )m_Find_KeyValuePtr[nId])); break;
						case TYPE_WSTRING: ((std::wstring *)pDataPtrArr[i])->assign(*((std::wstring *)m_Find_KeyValuePtr[nId])); break;
						case TYPE_VECTOR:  {
							size_t iValueLen = ((std::vector<BYTE> *)m_Find_KeyValuePtr[nId])->size();
							((std::vector<BYTE> *)pDataPtrArr[i])->resize(iValueLen);
							memcpy(&((std::vector<BYTE> *)pDataPtrArr[i])->front(),&((std::vector<BYTE> *)m_Find_KeyValuePtr[nId])->front(),iValueLen);
						}
						break;
						default: return(LogFail(_F,ERROR_INVALID_BLOCK,_T("Unknown field type. DataTable <%s>"),m_Path.c_str())); break;
					}
				} catch(...)   {
					return(LogFail(_F,ERROR_INVALID_ACCESS,_T("Error during assignement field <%ld> data. DataTable <%s>"),i,m_Path.c_str()));
				}
				continue;
			}
		}

		nValueOffset=pFieldInfo->pRecordFieldPtr->StrValueOffsetOrNumValue;

		try   {
			switch(pFieldInfo->eType)   {
				case TYPE_INT32:
				case TYPE_UINT32:  
					*((UINT32 *)pDataPtrArr[i])=(UINT32)nValueOffset;
				break;

				case TYPE_BYTE_10:
					if(fV.ReadAt(nValueOffset,(LPVOID)pDataPtrArr[i],10)!=S_OK)   return(LogFail(_F,0,_T("BYTE[10] rading error. DataTable <%s>"),m_Path.c_str()));
				break;

				case TYPE_STRING:
				case TYPE_VECTOR:
				case TYPE_WSTRING:   {

					#define VALUE_BUFF_LENGTH_RESERV 10U

					UINT16 uiValueMiddleSize = pFieldInfo->wValueMiddleSize;

					if(fV.ReadAt(nValueOffset-2,m_ValueBuff,uiValueMiddleSize)!=S_OK)   return(LogFail(_F,0,_T("Read FIRST Value Block Error. DataTable <%s>"),m_Path.c_str()));
					WORD iValueLen=*((WORD *)m_ValueBuff);
					if(iValueLen>uiValueMiddleSize)   {
						if(iValueLen+VALUE_BUFF_LENGTH_RESERV>m_ValueBuffLen)   {
							m_ValueBuffLen=iValueLen + VALUE_BUFF_LENGTH_RESERV;														
							BYTE* temp;
							if( (temp = (BYTE*) realloc( m_ValueBuff, m_ValueBuffLen) ) == NULL )
							{
								return(LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("No memory for ValueBuffer[%ld]. DataTable <%s>"),m_ValueBuffLen,m_Path.c_str()));
							}
							else
							{
								m_ValueBuff = temp;
							}

						}
						if(fV.ReadAt(nValueOffset-2+uiValueMiddleSize,m_ValueBuff+uiValueMiddleSize,iValueLen-uiValueMiddleSize)!=S_OK)   return(LogFail(_F,0,_T("Read FULL Value Block Error (std::vector or std::string). DataTable <%s>"),m_Path.c_str()));
						// printf("*:%ld",pFieldInfo->iIndex);
						uiValueMiddleSize=iValueLen+VALUE_BUFF_LENGTH_RESERV;
					} else   {
						if(uiValueMiddleSize>iValueLen+VALUE_BUFF_LENGTH_RESERV)   uiValueMiddleSize--;
					}

					pFieldInfo->wValueMiddleSize = uiValueMiddleSize;

					if(pFieldInfo->eType==TYPE_STRING)   {
						((std::string *)      pDataPtrArr[i])->assign((char *   )m_ValueBuff+2);
					} else if(pFieldInfo->eType==TYPE_WSTRING)   {
						((std::wstring *)     pDataPtrArr[i])->assign((wchar_t *)m_ValueBuff+1);
					} else   {
						iValueLen-=2;
						((std::vector<BYTE> *)pDataPtrArr[i])->resize(iValueLen);
						memcpy(&((std::vector<BYTE> *)pDataPtrArr[i])->front(),m_ValueBuff+2,iValueLen);
					}
				}
				break;

				default: return(LogFail(_F,ERROR_INVALID_BLOCK,_T("Unknown field type. DataTable <%s>"),m_Path.c_str())); break;
			}
		} catch(...)   {
			return(LogFail(_F,ERROR_INVALID_ACCESS,_T("Error during assignement field <%ld> data. DataTable <%s>"),i,m_Path.c_str()));
		}
	}

	return(S_OK);
}

HRESULT DTablePhysC::SetRecordData(VOID * pDataPtrArr[], WORD nId, BOOL bFindMode)   {
	BOOL          bWriteRecordFlag=FALSE;
	UINT32        nValueOffset   = 0;
	SFieldInfo * pFieldInfo = m_FieldsInfoArr;

	for(unsigned int i=0; i<m_FieldsNum; i++,pFieldInfo++)   {

		if(!pFieldInfo->bIsDataFlag || pDataPtrArr[i]==NULL)   continue;

		nValueOffset=pFieldInfo->pRecordFieldPtr->StrValueOffsetOrNumValue;

		if(bFindMode)   {
			if(i==m_Find_RecordFieldIdx[nId])   {
				if(pFieldInfo->bIsUnicFlag)   {
					m_Find_RecordNextOffs[nId]=ZERO_NEXT_RECORD_IDX;
				} else   {
					m_Find_RecordNextOffs[nId]=pFieldInfo->pRecordFieldPtr->NextRecordOffset; //
				}
			}
		}

		switch(pFieldInfo->eType)   {
			case TYPE_INT32:
			case TYPE_UINT32:
				try   {
					// if(m_bIncFlag)   { ...
					pFieldInfo->pRecordFieldPtr->StrValueOffsetOrNumValue=*((UINT32 *)pDataPtrArr[i]);
					bWriteRecordFlag=TRUE;
				} catch(...)   {
					return(LogFail(_F,ERROR_INVALID_ACCESS,_T("Error during assignement data to <Int> Value. DataTable <%s>"),m_Path.c_str()));
				}
			break;

			case TYPE_BYTE_10:
				if(fV.WriteAt(nValueOffset,(LPVOID)pDataPtrArr[i],10)!=S_OK)   return(LogFail(_F,0,_T("Error during writing data to <Byte10Arr> Value. DataTable <%s>"),m_Path.c_str()));
			break;

			case TYPE_STRING:
			case TYPE_VECTOR:
			case TYPE_WSTRING:
				return(LogFail(_F,ERROR_INVALID_BLOCK,_T("Types <std::string> and <std::vector> do not support change operation. DataTable <%s>"),m_Path.c_str())); break;
			break;

			default: return(LogFail(_F,ERROR_INVALID_BLOCK,_T("Unknown field type. DataTable <%s>"),m_Path.c_str())); break;
		}
	}

	if(bWriteRecordFlag)   {
		UINT32 RecPos = (bFindMode)?(m_Find_RecordNextOffs[nId]):(m_Scan_RecordIdx[nId]*m_RecordSize);
		((BYTE *)m_RecordArr)[m_RecordSize-1] = 0x00; // Extra byte
		if(fR.WriteAt(RecPos,m_RecordArr,m_RecordSize)!=S_OK)   return(LogFail(_F,0,_T("Error During Writing Data")));
	}

	return(S_OK);
}

UINT32 DTablePhysC::GetFoundIdx(WORD nFindId)   {
	VISIT_CRIT_SECT(m_oCriticalSection);

	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(nFindId>=FIND_LIM)      return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Find ID <%d> is out of range. DataTable <%s>"),nFindId,m_Path.c_str()));

	return(m_Find_RecordFoundIdx[nFindId]);
}

UINT32 DTablePhysC::GetRecordsNumber(VOID)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	return(m_RecordsNum);
}

UINT32 DTablePhysC::GetDeletedRecordsNumber(VOID)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	return(m_DeletedRecordsNum);
};
