#include "StdAfx.h"
#include "cl_Topo_DBMS.h"
#include "cl_DTablePhys.h"
#include "fn_HashFunc.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_DTablePhys.cpp"

#define HASH_FILL_PERCENT                 (60) //

#define DEFAULT_START_NUMBER       (100000)
#define MINIMUM_START_NUMBER       (   100)
#define MAXIMUM_START_NUMBER      (1000000)

#define DEFAULT_GROW_MODE_IDX     (0)  // GROW_EVEN

DTablePhysC::DTablePhysC(void) :
//	m_bIncFlag(FALSE),
	m_bClearUserInitializations(TRUE),
	m_ClearFlag(FALSE),
	m_bTableHasUnicFields(FALSE),
	m_bTableIsDynamic(FALSE),
	m_bTableIsTemporary(FALSE),
	m_FieldsInfoArr(NULL),
	m_HashInfoArr(NULL),
	m_RecordArr(NULL),
	m_ValueBuff(NULL),
	m_Add_UserDataPointers(NULL),
	m_Change_UserDataPointers(NULL),
	m_HashGrowMode(DEFAULT_GROW_MODE_IDX),
	m_HashStartVolume(DEFAULT_START_NUMBER),
	m_KeyFieldsNum(0),
	m_SaveAfterValue(0),
	m_SaveAfterCnt(0)   {

	m_InfoPtr = static_cast<SDTableInfo *>(this);
}

DTablePhysC::~DTablePhysC()   {
	// this->Close();
}

void DTablePhysC::SetStartSize(UINT32 iStartSize)   {

	m_HashBlockStartUserSize=iStartSize;

	if(m_HashBlockStartUserSize == 0x0L)                   m_HashBlockStartUserSize=DEFAULT_START_NUMBER;
	if(m_HashBlockStartUserSize <  MINIMUM_START_NUMBER)   m_HashBlockStartUserSize=MINIMUM_START_NUMBER;
	if(m_HashBlockStartUserSize >  MAXIMUM_START_NUMBER)   m_HashBlockStartUserSize=MAXIMUM_START_NUMBER;

	m_HashStartVolume = (m_HashBlockStartUserSize * 100) / HASH_FILL_PERCENT;
}

#define GROW_EVEN 0
#define GROW_DOWN 1
#define GROW_UP   2

static _TCHAR * g_aszGrowModes[] = {
	_T("EVEN"),
	_T("DOWN"),
	_T("UP"),
	NULL
};
static CHAR * g_aszGrowModesComments[] = {
	"next block will be the same volume",
	"next block will be two times smaller",
	"next block will be two times larger",
	NULL
};

void DTablePhysC::SetGrowModeS(_TCHAR * pModeStr)   {

	for(int i=0; g_aszGrowModes[i]; i++)   {
		if(_tcscmp(pModeStr,g_aszGrowModes[i])==0)   {
			SetGrowModeI(i);
			return;
		}
	}
	SetGrowModeI(DEFAULT_GROW_MODE_IDX);
}

BOOL DTablePhysC::ChkDirExistence(CONST _TCHAR * pDirName)   {
	return(GetFileAttributes(pDirName)!=INVALID_FILE_ATTRIBUTES);
}

HRESULT DTablePhysC::SetDynamicInfo(CONST _TCHAR * pszDynamicPath)   {
	m_bTableIsDynamic = TRUE;

	m_Path.assign(pszDynamicPath);
	if(!m_Path.empty() && *(m_Path.rbegin())!='\\')   m_Path+='\\';
	if(MkDir(m_Path.c_str())!=S_OK)   return(LogFail(_F,0,_T("Working folder creation error. DataTable <%s>"),m_Path.c_str()));
	m_Path+=m_TopoPtr->m_Name;

	return(S_OK);
}

HRESULT DTablePhysC::MkDir(CONST _TCHAR * pDirName)   {
	bool bOk=false;
	int  j=0;

	while(1)   {

		if(!g_bItIsCDROM)   {
			if(!CreateDirectory(pDirName,NULL))   {
				if(GetLastError()==0x000000B7)   {
					bOk=true;
				}
			} else   {
				bOk=true;
			}
		} else   {
			bOk=true;
		}

		if(bOk && ChkDirExistence(pDirName))   return(S_OK);

		if(j++>=9)   return(LogFail(_F,0,_T("Folder <%s> was not created"),pDirName));

		Sleep(100);
	}
}

#define SIZE_OF_FIRST_HASH_BLOCK  (SIZE_OF_RECORD_OFFSET * ((m_HashBlockStartUserSize * 100) / HASH_FILL_PERCENT))

void DTablePhysC::ReCountHashInfoArray(void)   {
	SHashBlockInfo * pHInfo = m_HashInfoArr;
	for(UINT32 i=0; i<m_HashBlocksNum; i++, pHInfo++)   {

		WORD GrowMode=m_HashGrowMode;

		if(i>=HASH_BLOCKS_GROW_UP_START)   GrowMode=GROW_UP;

		if(i==0 || GrowMode==GROW_EVEN)   {
			pHInfo->uiHashBlockSize = m_HashBlockStartUserSize;
		} else   {
			if(GrowMode==GROW_DOWN)   {
				if((pHInfo->uiHashBlockSize = m_HashInfoArr[i-1].uiHashBlockSize / 2) < MINIMUM_START_NUMBER)   {
					pHInfo->uiHashBlockSize = MINIMUM_START_NUMBER;
				}
			} else   { // GrowMode==GROW_UP
				if((pHInfo->uiHashBlockSize=m_HashInfoArr[i-1].uiHashBlockSize * 2) > MAXIMUM_START_NUMBER)   {
					pHInfo->uiHashBlockSize = MAXIMUM_START_NUMBER;
				}
			}
		}

		pHInfo->uiHashBlockVolume              = (pHInfo->uiHashBlockSize * 100) / HASH_FILL_PERCENT;
		pHInfo->uiHashBlockVolumeInBytes       = SIZE_OF_RECORD_OFFSET * pHInfo->uiHashBlockVolume;
		pHInfo->uiHashBlockGlobalLimit         = ((i)?(m_HashInfoArr[i-1].uiHashBlockGlobalLimit):(0))+pHInfo->uiHashBlockSize;
		pHInfo->uiOffsetOfStartOfHashBlock     = ((i)?(m_HashInfoArr[i-1].uiOffsetOfEndOfHashBlock):(SIZE_OF_TABLE_INFO));
		pHInfo->uiOffsetOfEndOfHashBlock       = pHInfo->uiOffsetOfStartOfHashBlock + pHInfo->uiHashBlockVolumeInBytes;
	}
}

HRESULT DTablePhysC::SetDTFilesPathsAndNames(VOID)   {

	if(m_bTableIsDynamic)   { // Path from parents

		// Yet defined. See SetDynamicInfo()
		if(MkDir(m_Path.c_str())!=S_OK)   return(LogFail(_F,0,_T("Working folder creation error. DataTable <%s>"),m_Path.c_str()));

	} else   {

		DObjC * ParentArr[100], * ParentPtr=m_TopoPtr;
		int     ParentNum=0;

		m_Path.clear();

		do   {
			ParentArr[ParentNum++]=ParentPtr;
			ParentPtr=ParentPtr->m_ParentPtr;
		} while(ParentPtr);

		for(int i=ParentNum-1; i>=0; i--)   {
			m_Path+=ParentArr[i]->m_Name;
			if(!m_Path.empty() && *(m_Path.rbegin())!='\\')   m_Path+='\\';

			if(MkDir(m_Path.c_str())!=S_OK)   return(LogFail(_F,0,_T("Working folder creation error. DataTable <%s>"),m_Path.c_str()));
		}

	}

	fS.SetPathAndName(&m_Path,F_NAME_STAT);
	fH.SetPathAndName(&m_Path,F_NAME_HASH);
	fR.SetPathAndName(&m_Path,F_NAME_RECORDS);
	fV.SetPathAndName(&m_Path,F_NAME_VALUES);

	return(S_OK);
}

HRESULT DTablePhysC::Switch(void)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	// LogMess("DataTable SWITCH : %s",m_Path.c_str());

	HRESULT HR;
	UINT32  iReferenceCounter = m_TopoPtr->m_ReferenceCounter;

	m_bClearUserInitializations=FALSE;

	if(iReferenceCounter)   {
		m_TopoPtr->m_ReferenceCounter = 1;
		Close();
	}

	if((HR=SetDTFilesPathsAndNames())==S_OK)   {
		if(iReferenceCounter)   {
			HR=Open();
			m_TopoPtr->m_ReferenceCounter = iReferenceCounter;
		}
	}

	m_bClearUserInitializations=TRUE;

	return(HR);
}

void DTablePhysC::ClearUserSetings(void)   {
	if(m_bClearUserInitializations)   {

		memset(m_Find_ppArrayOfPointersToData,0,sizeof(m_Find_ppArrayOfPointersToData));
		memset(m_SpanFind_UserSetPointers,    0,sizeof(m_SpanFind_UserSetPointers));
		memset(m_Scan_ppArrayOfPointersToData,       0,sizeof(m_Scan_ppArrayOfPointersToData));
		memset(m_Scan_RecordIdx,              0,sizeof(m_Scan_RecordIdx));

		//for(int i=0; i<FIND_LIM; i++)   {
		//	m_Find_RecordFieldIdx[i]        = 0;
		//	m_Find_RecordNextOffs[i]        = ZERO_NEXT_RECORD_IDX;
		//	m_Find_RecordFoundIdx[i]        = 0xFFFFFFFF;
		//	m_Find_AnswerNoMore[i]          = TRUE;
		//	m_Find_KeyValuePtr[i]           = NULL;
		//	m_Find_ppArrayOfPointersToData[i] = NULL;
		//}

		//for(i=0; i<SPAN_FIND_LIM; i++)   {
		//	m_SpanFind_RecordFieldIdx[i]   = 0;
		//	m_SpanFind_LowerKeyValuePtr[i] = NULL;
		//	m_SpanFind_UpperKeyValuePtr[i] = NULL;
		//	m_SpanFind_UserSetPointers[i]  = NULL;
		//}

		m_Find_CurrId             = 0;
		m_Add_UserDataPointers    = NULL;
		m_Change_UserDataPointers = NULL;
	}
}

inline UINT32 GetCrc32(UINT32 uiGetCrc32, WORD wValue)   {
	BYTE bValue;

	bValue = (BYTE)wValue;
	uiGetCrc32  = (((uiGetCrc32) >> 8) ^ Crc32Tbl[((uiGetCrc32) ^ bValue) & 0xFF]);
	bValue = (BYTE)(wValue>>8);
	uiGetCrc32  = (((uiGetCrc32) >> 8) ^ Crc32Tbl[((uiGetCrc32) ^ bValue) & 0xFF]);
	return(uiGetCrc32);
}

VOID DTablePhysC::SetInternalInfo(VOID)   {
	UINT iCntOfFieldsWithoutNextPtr  = 0;

	m_TableCheckSum = GetCrc32(0,m_HashBlockStartUserSize);
	m_FieldsNum     = m_TopoPtr->m_ChildrenNum;

	for(UINT32 i=0; i<m_FieldsNum; i++)   {
		DFieldTopoC * pF=m_TopoPtr->FindChild(i);

		if(pF)   {

			if(pF->m_IndexedFlag)   {
				pF->m_NoFindFlag = FALSE;
				pF->m_IsUnicFlag = FALSE;
				pF->m_IsDataFlag = FALSE;
			}

			if(pF->m_NoFindFlag || pF->m_IsUnicFlag || pF->m_IsDataFlag)   iCntOfFieldsWithoutNextPtr++;

			                        m_TableCheckSum = GetCrc32(m_TableCheckSum,pF->m_FType);
			                        m_TableCheckSum = GetCrc32(m_TableCheckSum,pF->m_FSize);
			                        m_TableCheckSum = GetCrc32(m_TableCheckSum,iCntOfFieldsWithoutNextPtr);
			if(pF->m_NoFindFlag)    m_TableCheckSum = GetCrc32(m_TableCheckSum,i);
			if(pF->m_IsUnicFlag)    m_TableCheckSum = GetCrc32(m_TableCheckSum,i);
			if(pF->m_IsDataFlag)    m_TableCheckSum = GetCrc32(m_TableCheckSum,i);
			if(pF->m_IndexedFlag)   m_TableCheckSum = GetCrc32(m_TableCheckSum,i);

		} else   {
			throw LogFail(_F,ERROR_INVALID_ACCESS,_T("DataField <%ld> was not found. DataTable <%s>"),i,m_Path.c_str());
		}
	}

	m_RecordSize = 1 + m_FieldsNum * (SIZE_OF_RECORD_UNIT) - iCntOfFieldsWithoutNextPtr * (SIZE_OF_NEXT_OFFSET);

}

HRESULT DTablePhysC::Open(VOID)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	// LogMess("DataTable OPEN : %s",m_Path.c_str());

	HRESULT HR=S_OK;

	if(m_TopoPtr->m_ReferenceCounter==0)   {

		try   {

//			UINT   iClearState = 0;
//			UINT   iOpenState = 0;
//			bool   bClearTableFlag         = FALSE;
//			bool   bRestoreBackUpFilesFlag = FALSE;
//			BOOL   bFlSizeOk; 
//			BOOL   bFlInfoOk; 
//			UINT32 uiTableCheckSum = 0;
//			UINT32 uiNoNextFieldsNum  = 0;

			ClearUserSetings();

			SDTableInfo    oInfoFromDisk;
			memset((void *)&oInfoFromDisk,0,sizeof(SDTableInfo));
			memset((void *) m_InfoPtr,    0,sizeof(SDTableInfo));

			SetInternalInfo();

			if(m_bTableIsTemporary)   {
				fH.m_FFullPath+=_T("_tmp");
				fR.m_FFullPath+=_T("_tmp");
				fV.m_FFullPath+=_T("_tmp");
				fS.m_FFullPath+=_T("_tmp");
			}

			BOOL c1, b0, b1, b2;
			UINT iCheckStep = 0;

			while(1)   {

				if(fH.Open()!=S_OK)   throw LogFail(_F,0,_T("Hash_File Opening Error. DataTable <%s>"),m_Path.c_str());
				if(fR.Open()!=S_OK)   throw LogFail(_F,0,_T("Records_File Opening Error. DataTable <%s>"),m_Path.c_str());
				if(fV.Open()!=S_OK)   throw LogFail(_F,0,_T("Value_File Opening Error. DataTable <%s>"),m_Path.c_str());

				if(fH.GetSize()<(SIZE_OF_TABLE_INFO + SIZE_OF_FIRST_HASH_BLOCK))   {
					c1 = TRUE;
					break;
				} else   {
					if(fH.ReadAt(0,&oInfoFromDisk,sizeof(SDTableInfo))!=S_OK)   throw LogFail(_F,0,_T("HashFile Reading Error. DataTable <%s>"),m_Path.c_str());
					b0 = (oInfoFromDisk.m_DbmsVersion!=DBMS_VERSION);
					b1 = (oInfoFromDisk.m_TableCheckSum && oInfoFromDisk.m_TableCheckSum != m_TableCheckSum);
					b2 = (m_RecordSize == 0 || oInfoFromDisk.m_RecordsNum != fR.GetSize() / m_RecordSize);	
					c1 = FALSE;
				}
			
				if(b0 || b1 || b2)   {
					if(iCheckStep==0 && BakFiles_Exist(m_Path.c_str()))   {
						if(b0)   LogMess(_T("WRONG VERSION of DBMS. DataTable <%s> is damaged. Restoring from BACK_UP files ..."),m_Path.c_str());
						if(b1)   LogMess(_T("CHECK_SUM Error. DataTable <%s> is damaged. Restoring from BACK_UP files ..."),m_Path.c_str());
						if(b2)   LogMess(_T("WRONG_NUMBER_OF_RECORDS (%ld instead of %ld). DataTable <%s> is damaged. Restoring from BACK_UP files ..."),fR.GetSize()/m_RecordSize,oInfoFromDisk.m_RecordsNum,m_Path.c_str());
						fH.Close();
						fR.Close();
						fV.Close();
						BakFiles_Restore(m_Path.c_str());
					} else   {
						if(b0)   LogMess(_T("Possibly NEW VERSION of DBMS. Clearing DataTable <%s> ..."),m_Path.c_str());
						if(b1)   LogMess(_T("CHECK_SUM Error. DataTable <%s> is damaged. Clearing ..."),m_Path.c_str());
						if(b2)   LogMess(_T("WRONG_NUMBER_OF_RECORDS (%ld instead of %ld). DataTable <%s> is damaged. Clearing ..."),fR.GetSize()/m_RecordSize,oInfoFromDisk.m_RecordsNum,m_Path.c_str());
						c1 = TRUE;
						break;
					}
				} else   {
					break;
				}

				iCheckStep++;

			} // while(iCheckStep)

			if(c1)   {
				fH.Close();
				fR.Close();
				fV.Close();

				DeleteFiles(m_Path.c_str(),_T("\\DT_*.dta"));

				if(fH.Open()!=S_OK)   throw LogFail(_F,0,_T("Hash_File Opening Error N2. DataTable <%s>"),m_Path.c_str());
				if(fR.Open()!=S_OK)   throw LogFail(_F,0,_T("Records_File Opening Error N2. DataTable <%s>"),m_Path.c_str());
				if(fV.Open()!=S_OK)   throw LogFail(_F,0,_T("Value_File Opening Error N2. DataTable <%s>"),m_Path.c_str());

				UINT32 iTmpTableCheckSum = m_TableCheckSum; 
				memset((void *)m_InfoPtr,0,sizeof(SDTableInfo));
				m_TableCheckSum = iTmpTableCheckSum;
				m_DbmsVersion=DBMS_VERSION;
				m_HashBlocksNum=1;

				if(fH.Fill(SIZE_OF_TABLE_INFO + SIZE_OF_FIRST_HASH_BLOCK,0xFF)!=S_OK)   throw LogFail(_F,0,_T("HashFile InitialFilling Error. DataTable <%s>"),m_Path.c_str());
				if(fH.WriteAt(0,m_InfoPtr,sizeof(SDTableInfo))!=S_OK)      throw LogFail(_F,0,_T("HashFile Writing Error. DataTable <%s>"),m_Path.c_str());

			} else   {
				memcpy((void *)m_InfoPtr, (void *)&oInfoFromDisk, sizeof(SDTableInfo));
			}

			fR.SetPosition(0,FILE_END);
			m_RecordNewPtr  = fR.GetSize();

			fV.SetPosition(0,FILE_END);
			m_ValueNewPtr=fV.GetSize();

			m_ValueBuffLen       = STR_BUFF_INI_SIZE;
			m_KeyFieldsNum       = 0;
			

			m_FieldsInfoArr = new SFieldInfo[ m_FieldsNum ]; //--if((m_FieldsInfoArr = (SFieldInfo *)     new SFieldInfo[m_FieldsNum])==NULL)                             { throw LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("NoMemoryFor<FieldsInfoArr>")); }
			if((m_HashInfoArr   = (SHashBlockInfo *) calloc(1,m_HashBlocksNum * sizeof(SHashBlockInfo)   ))==NULL)   { throw LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("NoMemoryFor<HashInfoArr>"));   }
			if((m_RecordArr     = (SRecordField *)   calloc(1,m_FieldsNum     * sizeof(SRecordField) + 1 ))==NULL)   { throw LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("NoMemoryFor<RecordArr>"));     }
			if((m_ValueBuff     = (BYTE *)           calloc(1,m_ValueBuffLen  * sizeof(char)             ))==NULL)   { throw LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("NoMemoryFor<ValueBuff>"));     }

			ReCountHashInfoArray(); // Prepare correct hash arrays
			UINT  iCntOfFieldsWithoutNextPtr  = 0;
			SFieldInfo * pFInfo = m_FieldsInfoArr;
			for(UINT32 i=0; i<m_FieldsNum; i++,pFInfo++)   {
				DFieldTopoC * pF=m_TopoPtr->FindChild(i);
				if(pF)   {
					pFInfo->iIndex                = i;
					pFInfo->eType                 = pF->m_FType;
					pFInfo->iSize                 = pF->m_FSize;
					pFInfo->iRecordFieldBegOffset = i * (SIZE_OF_RECORD_UNIT) - iCntOfFieldsWithoutNextPtr * (SIZE_OF_NEXT_OFFSET);
					pFInfo->bNoFindFlag           = pF->m_NoFindFlag;
					pFInfo->bIsUnicFlag           = pF->m_IsUnicFlag;
					pFInfo->bIsDataFlag           = pF->m_IsDataFlag || (pFInfo->bNoFindFlag && (pFInfo->eType==TYPE_INT32 || pFInfo->eType==TYPE_UINT32));
					pFInfo->bIndexedFlag          = pF->m_IndexedFlag;
					pFInfo->bHasNextFlag          = TRUE;
					pFInfo->wValueMiddleSize      = STR_BUFF_INI_SIZE;
					pFInfo->pRecordFieldPtr       = (SRecordField *)(((UINT32 *)(m_RecordArr + i)) - iCntOfFieldsWithoutNextPtr);
					pFInfo->pszFieldName          = pF->m_Name.c_str();
					pFInfo->pszTypeName           = pF->GetTypeName();

					if(pFInfo->bIsUnicFlag)   m_bTableHasUnicFields=TRUE;

					if(pF->m_NoFindFlag || pF->m_IsUnicFlag || pF->m_IsDataFlag)   {
						pFInfo->bHasNextFlag=FALSE;
						iCntOfFieldsWithoutNextPtr++;
					}
					if(!(pF->m_NoFindFlag || pF->m_IsDataFlag))   {
						m_KeyFieldsNum++;
					}
					if(pFInfo->bIndexedFlag)   {
						_TCHAR szName[64];
						_stprintf(szName,_T("DT_zFld_%2.2ld_idx.dta%s"),i,m_bTableIsTemporary?_T("_tmp"):_T(""));
						pFInfo->oIndexFile.SetPathAndName(&m_Path,szName);
						pFInfo->oIndexTree.FileOpenAndLoad(&pFInfo->oIndexFile);
					}
				}
			}

			m_TopoPtr->m_ReferenceCounter=1;

			// fS.Delete();
			//fS.RenameBack();

		} catch(HRESULT Catch_HR)   {
			Close();
			HR=Catch_HR;
		}
	} else   {
		m_TopoPtr->m_ReferenceCounter++;
	}

	return(HR);
}

HRESULT DTablePhysC::TryClear(VOID)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	// LogMess("DataTable TRY_CLEAR : %s",m_Path.c_str());

	if(m_TopoPtr->m_ReferenceCounter)   return(S_FALSE);

	return(Clear());
}

HRESULT DTablePhysC::SaveBackup(BOOL bSaveAnyTable /*= FALSE*/)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	// LogMess("DataTable SaveBackup : %s",m_Path.c_str());

	UINT32 iReferenceCounter = m_TopoPtr->m_ReferenceCounter;
	UINT32 iDeletedRecordsNum;

	m_ClearFlag                 = FALSE;
	m_bClearUserInitializations = FALSE;

	if(bSaveAnyTable)   {
		if(m_SaveAfterValue==0)   {
			m_SaveAfterValue=0xFFFFFFFF;
			m_SaveAfterCnt  =0xFFFFFFFF;
		}
	}

	if(iReferenceCounter)   {
		m_TopoPtr->m_ReferenceCounter = 1;
		iDeletedRecordsNum = m_DeletedRecordsNum;
		m_DeletedRecordsNum = 0;
		if(Close()!=S_OK)   return(LogFail(_F,0,_T("DataTable <%s> SaveBackup::Close() error"),m_Path.c_str()));
	}

	if(m_SaveAfterValue==0xFFFFFFFF)   {
		m_SaveAfterValue=0;
		m_SaveAfterCnt  =0;
	}

	if(SetDTFilesPathsAndNames()!=S_OK)   return(LogFail(_F,0,_T("DataTable <%s> SaveBackup::Init() error"),m_Path.c_str()));

	if(iReferenceCounter!=0)   {
		if(Open()!=S_OK)   return(LogFail(_F,0,_T("DataTable <%s> SaveBackup::Open() error"),m_Path.c_str()));
		m_DeletedRecordsNum = iDeletedRecordsNum;
		m_TopoPtr->m_ReferenceCounter = iReferenceCounter;
	}

	m_bClearUserInitializations = TRUE;

	return(S_OK);
}

HRESULT DTablePhysC::Clear(BOOL bClearUserInitializations /* = TRUE */)   {
	VISIT_CRIT_SECT(m_oCriticalSection);

	// LogMess("DataTable CLEAR : %s",m_Path.c_str());

	UINT32 iReferenceCounter = m_TopoPtr->m_ReferenceCounter;

	m_ClearFlag = TRUE;
	m_bClearUserInitializations = bClearUserInitializations;

	if(iReferenceCounter)   {
		m_TopoPtr->m_ReferenceCounter = 1;
		if(Close()!=S_OK)   return(LogFail(_F,0,_T("DataTable <%s> Clear::Close() error"),m_Path.c_str()));
	}

	m_ClearFlag = FALSE;

	SetDTFilesPathsAndNames();

	m_RecordsNum        = 0;
	m_DeletedRecordsNum = 0;

	DeleteFiles(m_Path.c_str(),_T("\\DT_*.dta"));
	DeleteFiles(m_Path.c_str(),_T("\\DT_*.txt"));
	m_SaveAfterCnt = 0;

	if(!g_bItIsCDROM)   {
		SetFileAttributes(m_Path.c_str(),FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_ARCHIVE);
		RemoveDirectory(  m_Path.c_str());
	}

	if(SetDTFilesPathsAndNames()!=S_OK)   return(LogFail(_F,0,_T("DataTable <%s> Clear::Init() error"),m_Path.c_str()));

	if(iReferenceCounter!=0)   {
		if(Open()!=S_OK)   return(LogFail(_F,0,_T("DataTable <%s> Clear::Open() error"),m_Path.c_str()));
		m_TopoPtr->m_ReferenceCounter = iReferenceCounter;
	}

	m_bClearUserInitializations = TRUE;

	return(S_OK);
}


HRESULT DTablePhysC::Close(BOOL bClearAfterClose)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	// LogMess("DataTable CLOSE : %s",m_Path.c_str());

	bool bDoShrinkFlag = !m_ClearFlag && (m_DeletedRecordsNum && m_RecordsNum>100 && (m_DeletedRecordsNum*100/m_RecordsNum)>SHRINK_PERCENT);

	if(m_TopoPtr->m_ReferenceCounter==1)   {

		ClearUserSetings();

		/*if(!bDoShrinkFlag && !m_ClearFlag && fS.Open()==S_OK)   {
			fS.Clear();

			fS.fprintf("\r\n");
			fS.fprintf("   ----------------------------------------------------------------------------\r\n");
			fS.fprintf("   DataTable:   \"%S\"   (INFORMATION and STATISTICS)\r\n",m_TopoPtr->m_Name.c_str());
			fS.fprintf("   ----------------------------------------------------------------------------\r\n\r\n");

			fS.fprintf("      DATA_FIELDS (total: %ld, ",m_FieldsNum);
			if(m_FieldsNum==m_KeyFieldsNum)   {
				fS.fprintf("all are key-fields)");
			} else   {
				fS.fprintf("key-fields: %ld)",m_KeyFieldsNum);
			}
			fS.fprintf(":\r\n\r\n");

			SFieldInfo * pFInfo = m_FieldsInfoArr;
			for(UINT32 i=0; i<m_FieldsNum; i++,pFInfo++)   {

				static char * M0 = "";
				static char * M1 = "- UNIQUE VALUES (key-field)";
				static char * M2 = "- SEARCH IS NOT AVAILABLE";
				static char * M3 = "- DATA (search is not available)";
				static char * M4 = "- COMMON KEY-FIELD";
				static char * M5 = ", INDEXED";
				char        * pszMod;
				char        * pszMod2;
				if(pFInfo->bIsUnicFlag)   {
					pszMod=M1;
				} else if(pFInfo->bNoFindFlag)   {
					pszMod=M2;
				}else if(pFInfo->bIsDataFlag)   {
					pszMod=M3;
				} else   {
					pszMod=M4;
				}

				pszMod2=(pFInfo->bIndexedFlag)?M5:M0;

				fS.fprintf("   %5d  %-20S: %-8S%s%s\r\n", i, pFInfo->pszFieldName, pFInfo->pszTypeName, pszMod,pszMod2);

			}

			fS.fprintf("\r\n   -----------------------------------------------\r\n\r\n");

			fS.fprintf("   Number of data_table records : %lu\r\n", m_RecordsNum);
			fS.fprintf("   Number of deleted records    : %lu\r\n", m_DeletedRecordsNum);
			fS.fprintf("   Number of data_table elenents: %lu\r\n", m_RecordsNum*m_FieldsNum);
			fS.fprintf("\r\n\r\n");

			fS.fprintf("   Real data volume : %ld,%3.3ld,%3.3ld bytes\r\n",UINT32(m_DataGlobalLen)/(1000000),(UINT32(m_DataGlobalLen)%(1000000))/1000,(UINT32(m_DataGlobalLen)%(1000)));
			UINT32 DataDiskLen=fH.GetSize()+fR.GetSize()+fV.GetSize();
			fS.fprintf("   Disk data volume : %ld,%3.3ld,%3.3ld bytes\r\n",UINT32(DataDiskLen)/(1000000),(UINT32(DataDiskLen)%(1000000))/1000,(UINT32(DataDiskLen)%(1000)));
			if(DataDiskLen>m_DataGlobalLen)   {
				if(m_DataGlobalLen)   fS.fprintf("   Outgoings        : %.1f%%\r\n",float(DataDiskLen)/m_DataGlobalLen*100.-100.);
			} else   {
				if(DataDiskLen)       fS.fprintf("   Economies        : %.1f%%\r\n",float(m_DataGlobalLen)/DataDiskLen*100.-100.);
			}

			fS.fprintf("\r\n   -----------------------------------------------\r\n\r\n");

			//m_HashCollNumN=m_HashCollNum1+m_HashCollNum2+m_HashCollNumM;
			//m_HashCollFldN=m_HashCollFld1+m_HashCollFld2+m_HashCollFldM;

			fS.fprintf("   Number  of HASH-blocks  : %ld, mode of block-adding: GROW_%S (%s)\r\n",m_HashBlocksNum,g_aszGrowModes[m_HashGrowMode],g_aszGrowModesComments[m_HashGrowMode]);
			fS.fprintf("   Volumes of HASH-blocks  : ",m_HashBlockStartUserSize);
			for(UINT32 i=0; i<m_HashBlocksNum; i++)   fS.fprintf("%s%ld",i?", ":"",m_HashInfoArr[i].uiHashBlockSize);
			fS.fprintf(".\r\n");
			fS.fprintf("   Sum HASH-blocks volume  : %ld\r\n",m_HashInfoArr[m_HashBlocksNum-1].uiHashBlockGlobalLimit);
			fS.fprintf("   Sum number of HASH-keys : %ld (ReTry:%ld, ReRead:%ld, ReValue:%ld)\r\n",m_HashElemsNum,m_ReTryNum,m_ReReadNum,m_ReValueNum);

			//if(m_HashCollNum1) fS.fprintf("Число коллизий   1  : %ld (%5.1f%% быстрых )\r\n",m_HashCollNum1,(float)m_HashCollFld1/m_HashCollNum1*100.);
			//if(m_HashCollNum2) fS.fprintf("Число коллизий   2  : %ld (%5.1f%% быстрых )\r\n",m_HashCollNum2,(float)m_HashCollFld2/m_HashCollNum2*100.);
			//if(m_HashCollNumM) fS.fprintf("Число коллизий >=3  : %ld (%5.1f%% быстрых )\r\n",m_HashCollNumM,(float)m_HashCollFldM/m_HashCollNumM*100.);
			//if(m_HashCollNumN) fS.fprintf("Число коллизий общее: %ld (%5.1f%% быстрых )\r\n",m_HashCollNumN,(float)m_HashCollFldN/m_HashCollNumN*100.);

			if(!m_sTableIniStructure.empty())   {
				fS.fprintf("\r\n   -----------------------------------------------\r\n\r\n\r\n");
				fS.fprintf("   Table original structure from INI-file\r\n");
				fS.fprintf("   -----------------------------------------------\r\n\r\n");
				fS.fprintf("%s",m_sTableIniStructure.c_str());	
			}

			fS.fprintf("\r\n   ============================================================================\r\n");

			fS.Close();
			if(m_bTableIsTemporary)   fS.RenameTmp();
		}*/

		if(fH.WriteAt(0,m_InfoPtr,sizeof(SDTableInfo))!=S_OK)  return(LogFail(_F,0,_T("Internal Info was not written for DataTable <%s>"),m_Path.c_str()));

		fH.Close();
		fR.Close();
		fV.Close();

		if(m_bTableIsTemporary)   {
			fH.RenameTmp();
			fR.RenameTmp();
			fV.RenameTmp();
		}

		if( m_FieldsInfoArr )
		{
			for ( UINT32 i = 0; i < m_FieldsNum; i++ )
			{
				if ( m_FieldsInfoArr[ i ].bIndexedFlag )
				{
					m_FieldsInfoArr[ i ].oIndexTree.FileSaveAndClose();
					if( m_bTableIsTemporary ) 
					{
						m_FieldsInfoArr[ i ].oIndexFile.RenameTmp();
					}
				}
			}
		}

		if(m_ValueBuff)      { free(m_ValueBuff);        m_ValueBuff=NULL; }
		if(m_RecordArr)      { free(m_RecordArr);        m_RecordArr=NULL; }
		if(m_HashInfoArr)    { free(m_HashInfoArr);      m_HashInfoArr=NULL; }
		if(m_FieldsInfoArr)  { delete[] m_FieldsInfoArr; m_FieldsInfoArr=NULL; }

		m_TopoPtr->m_ReferenceCounter=0;

		if(bClearAfterClose)   {
			bDoShrinkFlag = FALSE;
			Clear();
		}

		if(bDoShrinkFlag)   Shrink();

		if(!m_ClearFlag && m_SaveAfterValue && m_SaveAfterCnt)   {
			BakFiles_Save(m_Path.c_str());
			m_SaveAfterCnt=0;
		}

	} else   {
		if(m_TopoPtr->m_ReferenceCounter==0)   {
			LogMess(_T("The extra try to close DataTable <%s> (Reference_Counter is zero already)"),m_Path.c_str());
		} else   {
			m_TopoPtr->m_ReferenceCounter--;
		}
	}

	return(S_OK);
}

HRESULT DTablePhysC::SetUserValue(UINT iValueIdx, UINT32 uiValue)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())          return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(iValueIdx>=USER_VALUES_LIM)    return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Index of UserValue <%ld> is out of range. DataTable <%s>"),iValueIdx,m_Path.c_str()));

	m_UserValues[iValueIdx]=uiValue;
	return(S_OK);
}

UINT32  DTablePhysC::GetUserValue(UINT iValueIdx)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())          return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(iValueIdx>=USER_VALUES_LIM)    return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Index of UserValue <%ld> is out of range. DataTable <%s>"),iValueIdx,m_Path.c_str()));

	return(m_UserValues[iValueIdx]);
}




