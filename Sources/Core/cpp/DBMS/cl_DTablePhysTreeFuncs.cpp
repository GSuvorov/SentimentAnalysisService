#include "StdAfx.h"
#include "cl_Topo_DBMS.h"
#include "cl_DTablePhys.h"
#include "fn_HashFunc.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_DTablePhysTreeFuncs.cpp"


UINT32 DTablePhysC::NodeCheckInterval(UINT32 iLinkToHashRecord)   {	

	NodeGetValueViaHash(iLinkToHashRecord);

	UINT32 iRet    = 0x0000;
	INT    iTmpVal;

	switch(m_SpanFind_FieldInfoPtr->eType)   {

		case TYPE_INT32:   {

			if(m_SpanFind_CurrLowerKeyValuePtr==NULL)   {
				iRet |= DEF_LoInEq;
			} else   {
				if(*((INT32*)m_SpanFind_CurrLowerKeyValuePtr) <INT32(m_oNodeValue_UINT32))   iRet |= DEF_LoIn;
				if(*((INT32*)m_SpanFind_CurrLowerKeyValuePtr)==INT32(m_oNodeValue_UINT32))   iRet |= DEF_LoEq;
			}

			if(m_SpanFind_CurrUpperKeyValuePtr==NULL)   {
				iRet |= DEF_UpInEq;
			} else   {
				if(*((INT32*)m_SpanFind_CurrUpperKeyValuePtr) >INT32(m_oNodeValue_UINT32))   iRet |= DEF_UpIn;
				if(*((INT32*)m_SpanFind_CurrUpperKeyValuePtr)==INT32(m_oNodeValue_UINT32))   iRet |= DEF_UpEq;
			}
			//LoInOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || *((INT32*)m_SpanFind_CurrLowerKeyValuePtr)<=INT32(m_oNodeValue_UINT32));
			//UpInOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || *((INT32*)m_SpanFind_CurrUpperKeyValuePtr)>=INT32(m_oNodeValue_UINT32));
			//LoEqOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || *((INT32*)m_SpanFind_CurrLowerKeyValuePtr)==INT32(m_oNodeValue_UINT32));
			//UpEqOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || *((INT32*)m_SpanFind_CurrUpperKeyValuePtr)==INT32(m_oNodeValue_UINT32));

//			printf("%ld->%1.1ld%1.1ld%1.1ld%1.1ld   ",INT32(m_oNodeValue_UINT32),iRet & DEF_LoIn,iRet & DEF_LoEq,iRet & DEF_UpIn,iRet & DEF_UpEq);
		}
		break;

		case TYPE_UINT32:   {
			if(m_SpanFind_CurrLowerKeyValuePtr==NULL)   {
				iRet |= DEF_LoInEq;
			} else   {
				if(*((UINT32*)m_SpanFind_CurrLowerKeyValuePtr) <UINT32(m_oNodeValue_UINT32))   iRet |= DEF_LoIn;
				if(*((UINT32*)m_SpanFind_CurrLowerKeyValuePtr)==UINT32(m_oNodeValue_UINT32))   iRet |= DEF_LoEq;
			}

			if(m_SpanFind_CurrUpperKeyValuePtr==NULL)   {
				iRet |= DEF_UpInEq;
			} else   {
				if(*((UINT32*)m_SpanFind_CurrUpperKeyValuePtr) >UINT32(m_oNodeValue_UINT32))   iRet |= DEF_UpIn;
				if(*((UINT32*)m_SpanFind_CurrUpperKeyValuePtr)==UINT32(m_oNodeValue_UINT32))   iRet |= DEF_UpEq;
			}
			//LoInOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || *((UINT32*)m_SpanFind_CurrLowerKeyValuePtr)<=UINT32(m_oNodeValue_UINT32));
			//UpInOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || *((UINT32*)m_SpanFind_CurrUpperKeyValuePtr)>=UINT32(m_oNodeValue_UINT32));
			//LoEqOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || *((UINT32*)m_SpanFind_CurrLowerKeyValuePtr)==UINT32(m_oNodeValue_UINT32));
			//UpEqOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || *((UINT32*)m_SpanFind_CurrUpperKeyValuePtr)==UINT32(m_oNodeValue_UINT32));
		}
		break;

		case TYPE_WSTRING:   {
			if(m_SpanFind_CurrLowerKeyValuePtr==NULL)   {
				iRet |= DEF_LoInEq;
			} else   {
				iTmpVal = wcscmp(((std::wstring *)m_SpanFind_CurrLowerKeyValuePtr)->c_str(),m_oNodeValue_WSTRING.c_str());
				if(iTmpVal<0)   {
					iRet |= DEF_LoIn;
				} else if(iTmpVal==0)   {
					iRet |= DEF_LoEq;
				}
			}
			if(m_SpanFind_CurrUpperKeyValuePtr==NULL)   {
				iRet |= DEF_UpInEq;
			} else   {
				iTmpVal = wcscmp(((std::wstring *)m_SpanFind_CurrUpperKeyValuePtr)->c_str(),m_oNodeValue_WSTRING.c_str());
				if(iTmpVal>0)   {
					iRet |= DEF_UpIn;
				} else if(iTmpVal==0)   {
					iRet |= DEF_UpEq;
				}
			}

			//LoInOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || wcscmp(((std::wstring *)m_SpanFind_CurrLowerKeyValuePtr)->c_str(),m_oNodeValue_WSTRING.c_str())<=0);
			//UpInOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || wcscmp(((std::wstring *)m_SpanFind_CurrUpperKeyValuePtr)->c_str(),m_oNodeValue_WSTRING.c_str())>=0);
			//LoEqOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || wcscmp(((std::wstring *)m_SpanFind_CurrLowerKeyValuePtr)->c_str(),m_oNodeValue_WSTRING.c_str())==0);
			//UpEqOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || wcscmp(((std::wstring *)m_SpanFind_CurrUpperKeyValuePtr)->c_str(),m_oNodeValue_WSTRING.c_str())==0);
			
		}
		break;

		case TYPE_STRING:   {
			if(m_SpanFind_CurrLowerKeyValuePtr==NULL)   {
				iRet |= DEF_LoInEq;
			} else   {
				iTmpVal = strcmp(((std::string *)m_SpanFind_CurrLowerKeyValuePtr)->c_str(),m_oNodeValue_STRING.c_str());
				if(iTmpVal<0)   {
					iRet |= DEF_LoIn;
				} else if(iTmpVal==0)   {
					iRet |= DEF_LoEq;
				}
			}
			if(m_SpanFind_CurrUpperKeyValuePtr==NULL)   {
				iRet |= DEF_UpInEq;
			} else   {
				iTmpVal = strcmp(((std::string *)m_SpanFind_CurrUpperKeyValuePtr)->c_str(),m_oNodeValue_STRING.c_str());
				if(iTmpVal>0)   {
					iRet |= DEF_UpIn;
				} else if(iTmpVal==0)   {
					iRet |= DEF_UpEq;
				}
			}
			//LoInOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || strcmp(((std::string *)m_SpanFind_CurrLowerKeyValuePtr)->c_str(),m_oNodeValue_STRING.c_str())<=0);
			//UpInOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || strcmp(((std::string *)m_SpanFind_CurrUpperKeyValuePtr)->c_str(),m_oNodeValue_STRING.c_str())>=0);
			//LoEqOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || strcmp(((std::string *)m_SpanFind_CurrLowerKeyValuePtr)->c_str(),m_oNodeValue_STRING.c_str())==0);
			//UpEqOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || strcmp(((std::string *)m_SpanFind_CurrUpperKeyValuePtr)->c_str(),m_oNodeValue_STRING.c_str())==0);
		}
		break;

		case TYPE_BYTE_10:   {
			if(m_SpanFind_CurrLowerKeyValuePtr==NULL)   {
				iRet |= DEF_LoInEq;
			} else   {
				iTmpVal = memcmp(m_SpanFind_CurrLowerKeyValuePtr,m_oNodeValue_BYTE_ARR,10);
				if(iTmpVal<0)   {
					iRet |= DEF_LoIn;
				} else if(iTmpVal==0)   {
					iRet |= DEF_LoEq;
				}
			}
			if(m_SpanFind_CurrUpperKeyValuePtr==NULL)   {
				iRet |= DEF_UpInEq;
			} else   {
				iTmpVal = memcmp(m_SpanFind_CurrUpperKeyValuePtr,m_oNodeValue_BYTE_ARR,10);
				if(iTmpVal>0)   {
					iRet |= DEF_UpIn;
				} else if(iTmpVal==0)   {
					iRet |= DEF_UpEq;
				}
			}
			//LoInOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || memcmp(m_SpanFind_CurrLowerKeyValuePtr,m_oNodeValue_BYTE_ARR,10)<=0);
			//UpInOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || memcmp(m_SpanFind_CurrUpperKeyValuePtr,m_oNodeValue_BYTE_ARR,10)>=0);
			//LoEqOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || memcmp(m_SpanFind_CurrLowerKeyValuePtr,m_oNodeValue_BYTE_ARR,10)==0);
			//UpEqOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || memcmp(m_SpanFind_CurrUpperKeyValuePtr,m_oNodeValue_BYTE_ARR,10)==0);
		}
		break;

		case TYPE_VECTOR:   {

			size_t iLen1;
			size_t iLen2;

			if(m_SpanFind_CurrLowerKeyValuePtr==NULL)   {
				iRet |= DEF_LoInEq;
			} else   {
				iLen1 = ((std::vector<BYTE> *)m_SpanFind_CurrLowerKeyValuePtr)->size();
				iLen2 = m_oNodeValue_VECTOR.size(); 
				if((iTmpVal=memcmp(&((std::vector<BYTE> *)m_SpanFind_CurrLowerKeyValuePtr)->front(),&m_oNodeValue_VECTOR.front(),min(iLen1,iLen2)))!=0)   {
					if(iTmpVal<0)   {
						iRet |= DEF_LoIn;
					} else if(iTmpVal==0)   {
						iRet |= DEF_LoEq;
					}
				} else   {
					if(iLen1<iLen2)   {
						iRet |= DEF_UpIn;
					} else if(iLen1==iLen2)   {
						iRet |= DEF_UpEq;
					}
				}
			}
			if(m_SpanFind_CurrUpperKeyValuePtr==NULL)   {
				iRet |= DEF_UpInEq;
			} else   {
				iLen1 = ((std::vector<BYTE> *)m_SpanFind_CurrUpperKeyValuePtr)->size();
				iLen2 = m_oNodeValue_VECTOR.size(); 
				if((iTmpVal=memcmp(&((std::vector<BYTE> *)m_SpanFind_CurrUpperKeyValuePtr)->front(),&m_oNodeValue_VECTOR.front(),min(iLen1,iLen2)))!=0)   {
					if(iTmpVal>0)   {
						iRet |= DEF_UpIn;
					} else if(iTmpVal==0)   {
						iRet |= DEF_UpEq;
					}
				} else   {
					if(iLen1>iLen2)   {
						iRet |= DEF_UpIn;
					} else if(iLen1==iLen2)   {
						iRet |= DEF_UpEq;
					}
				}
			}
		}
		break;

		default:
			LogFail(_F,ERROR_ACCESS_DENIED,_T("Unknown DATA_TYPE(%ld) for field N %ld. DataTable <%s>"),m_SpanFind_FieldInfoPtr->eType,m_SpanFind_FieldInfoPtr->iIndex,m_Path.c_str());
		break;
	}

	return(iRet);
}


BOOL DTablePhysC::NodeEstimateAndSaveInfo(UINT32 iLinkToHashRecord, BOOL bNeedCheck /*= TRUE*/)   {	
	UINT32 iCmp;

	if (bNeedCheck) iCmp = NodeCheckInterval(iLinkToHashRecord);

	if(!bNeedCheck || (iCmp & DEF_LoInEq) && (iCmp & DEF_UpInEq))    {
		do  
		{
			HRESULT HR = fR.ReadAt( m_iCurrFoundRecordIdx * m_RecordSize, m_RecordArr, m_RecordSize );
			if ( HR != S_OK )   {
				LogFail(_F,HR,_T("Record reading error. DataTable <%s>"),m_Path.c_str());
				//---return(LogFail(_F,HR,_T("Record reading error. DataTable <%s>"),m_Path.c_str()));
				return(FALSE);
			}

			if(((BYTE *)m_RecordArr)[m_RecordSize-1] == 0x00)   { // Extra byte 
				if(m_SpanFind_CurrIntersectPointer==NULL || m_SpanFind_CurrIntersectPointer->find(m_iCurrFoundRecordIdx)!=m_SpanFind_CurrIntersectPointer->end())   {
					m_SpanFind_UserSetPointers[m_SpanFind_FindId]->insert(m_iCurrFoundRecordIdx);
				}
			}

			if(m_SpanFind_FieldInfoPtr->bHasNextFlag)   {
				if((m_iCurrFoundRecordIdx = m_SpanFind_FieldInfoPtr->pRecordFieldPtr->NextRecordOffset)==ZERO_NEXT_RECORD_IDX)   {
					break;
				} else   {
					m_iCurrFoundRecordIdx /= m_RecordSize; 
				}
			} else   {
				break;
			}

		} 
		while(1);

		return(TRUE);
	} else   {
		return(FALSE);
	}
}

BOOL DTablePhysC::NodeValueL(VOID)   {

	switch(m_SpanFind_FieldInfoPtr->eType)   {

		case TYPE_STRING:
			return(strcmp((char *)m_pCurrNodeValuePtr,((std::string *)m_pNodeValue_PTR)->c_str())<0);
		break;

		case TYPE_WSTRING:
			return(wcscmp((wchar_t *)m_pCurrNodeValuePtr,((std::wstring *)m_pNodeValue_PTR)->c_str())<0);
		break;

		case TYPE_VECTOR:   {
			INT    iCmp;
			size_t iLen1 = ((std::vector<BYTE> *)m_pCurrNodeValuePtr)->size();
			size_t iLen2 = ((std::vector<BYTE> *)m_pNodeValue_PTR)->size(); 
			if((iCmp=memcmp(m_pCurrNodeValuePtr,&((std::vector<BYTE> *)m_pNodeValue_PTR)->front(),min(iLen1,iLen2)))!=0)   {
				return(iCmp<0);
			} else   {
				return(iLen1<iLen2);
			}
		}
		break;

		case TYPE_BYTE_10:
			return(memcmp(m_pCurrNodeValuePtr,m_pNodeValue_PTR,10)<0);
		break;

		case TYPE_INT32:
//			printf(" (%ld<%ld) ",*((INT32 *)m_pCurrNodeValuePtr),*((INT32 *)m_pNodeValue_PTR));
			return(*((INT32 *)m_pCurrNodeValuePtr)<*((INT32 *)m_pNodeValue_PTR));
		break;

		case TYPE_UINT32:
			return(*((UINT32 *)m_pCurrNodeValuePtr)<*((UINT32 *)m_pNodeValue_PTR));
		break;

		default:
			LogFail(_F,ERROR_ACCESS_DENIED,_T("Unknown DATA_TYPE(%ld). DataTable <%s>"),m_SpanFind_FieldInfoPtr->eType,m_Path.c_str());
			return(FALSE);
		break;
	}
}


HRESULT DTablePhysC::NodeGetValueViaHash(UINT32 iHashIdx)   {
//	VISIT_CRIT_SECT(m_oCriticalSection);

	m_iCurrFoundRecordIdx = -1;

	UINT32 iRecordIdx;

	if(fH.ReadAt(iHashIdx,&iRecordIdx,sizeof(UINT32))==S_OK)   {
		m_iCurrFoundRecordIdx = (iRecordIdx & ~UINT32(HASH_IDX_HIGH_BITS_MASK)); 
// printf("(%lX-%lX) ",iHashIdx,m_iCurrFoundRecordIdx);
		if(iRecordIdx == ZERO_HASH_RECORD)   { // Empty slot (No key in this block)
			return(LogFail(_F,0,_T("Indexing: Empty HASH-value. DataTable <%s>"),m_Path.c_str()));
		} else   {
			return(NodeGetValue(m_iCurrFoundRecordIdx));
		}
	} else   {
		return(LogFail(_F,0,_T("Indexing: HASH-value has not been read. DataTable <%s>"),m_Path.c_str()));
	}

	return(S_FALSE);
}

HRESULT DTablePhysC::NodeGetValue(UINT32 iRecordIdx)   {
//	VISIT_CRIT_SECT(m_oCriticalSection);

	HRESULT HR;

	if(iRecordIdx>=m_RecordsNum)   return(S_FALSE);

	UINT32 nValueOffset;

	if((HR=fR.ReadAt(iRecordIdx*m_RecordSize+m_SpanFind_FieldInfoPtr->iRecordFieldBegOffset,&nValueOffset,sizeof(UINT32)))!=S_OK)   {
		return(LogFail(_F,HR,_T("Record reading error. DataTable <%s>"),m_Path.c_str()));
	}

	try   {
		switch(m_SpanFind_FieldInfoPtr->eType)   {
			case TYPE_INT32:
			case TYPE_UINT32:
				m_oNodeValue_UINT32=(UINT32)nValueOffset;
				m_pNodeValue_PTR = (BYTE *)&m_oNodeValue_UINT32;
			break;

			case TYPE_BYTE_10:
				if(fV.ReadAt(nValueOffset,(LPVOID)m_oNodeValue_BYTE_ARR,10)!=S_OK)   return(LogFail(_F,0,_T("BYTE[10] rading error. DataTable <%s>"),m_Path.c_str()));
				m_pNodeValue_PTR = (BYTE *)&m_oNodeValue_BYTE_ARR;
			break;

			case TYPE_STRING:
			case TYPE_VECTOR:
			case TYPE_WSTRING:   {

				#define VALUE_BUFF_LENGTH_RESERV 10U

				UINT16 uiValueMiddleSize = m_SpanFind_FieldInfoPtr->wValueMiddleSize;

				if(fV.ReadAt(nValueOffset-2,m_ValueBuff,uiValueMiddleSize)!=S_OK)   return(LogFail(_F,0,_T("Read FIRST Value Block Error. DataTable <%s>"),m_Path.c_str()));
				WORD iValueLen=*((WORD *)m_ValueBuff);
				if(iValueLen>uiValueMiddleSize)   {
					if(iValueLen+VALUE_BUFF_LENGTH_RESERV>m_ValueBuffLen)   {
						m_ValueBuffLen = iValueLen + VALUE_BUFF_LENGTH_RESERV;						
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

				m_SpanFind_FieldInfoPtr->wValueMiddleSize = uiValueMiddleSize;

				switch(m_SpanFind_FieldInfoPtr->eType)   {

					case TYPE_STRING:
						m_oNodeValue_STRING.assign((char *)m_ValueBuff+2);
						m_pNodeValue_PTR = (BYTE *)&m_oNodeValue_STRING;
					break;

					case TYPE_WSTRING:
						m_oNodeValue_WSTRING.assign((wchar_t *)m_ValueBuff+1);
						m_pNodeValue_PTR = (BYTE *)&m_oNodeValue_WSTRING;
					break;

					case TYPE_VECTOR:
						iValueLen-=2;
						m_oNodeValue_VECTOR.resize(iValueLen);
						memcpy(&m_oNodeValue_VECTOR.front(),m_ValueBuff+2,iValueLen);
						m_pNodeValue_PTR = (BYTE *)&m_oNodeValue_VECTOR;
					break;
				}
			}
			break;

			default: return(LogFail(_F,ERROR_INVALID_BLOCK,_T("Unknown field type. DataTable <%s>"),m_Path.c_str())); break;
		}
	} catch(...)   {
		return(LogFail(_F,ERROR_INVALID_ACCESS,_T("Error during assignement internal data for field <%s>. DataTable <%s>"),m_SpanFind_FieldInfoPtr->pszFieldName,m_Path.c_str()));
	}

	return(S_OK);
}




//BOOL DTablePhysC::NodeAdd(UINT32 * piNodeIdx, SFieldInfo * pFieldInfo, BYTE * pValuePtr, WORD iValueLen)   {
//	INT32            iNodeIdx, iNodeIdx1, iNodeIdx2;
//	CIndexTree     * pIndexTree = &pFieldInfo->oIndexTree;
//
//	pIndexTree->bTreeGrowFlag = FALSE;
//
//	if(*piNodeIdx==-1)   {
//
//		iNodeIdx   = pIndexTree->AddNewNode();
//		*piNodeIdx = iNodeIdx;
//
//		pIndexTree->pTree[iNodeIdx].uiLinkToTableRecord = m_RecordsNum;
//		pIndexTree->pTree[iNodeIdx].uiLinkToRight       = 
//		pIndexTree->pTree[iNodeIdx].uiLinkToLeft        = -1;
//		pIndexTree->pTree[iNodeIdx].siBalans            = 0; 
//		pIndexTree->bTreeGrowFlag                       = TRUE;
////		pIndexTree->pTree[iNodeIdx].siCount             = 1;
//
//		return(TRUE);
//
//	} else   {
//
//		iNodeIdx = *piNodeIdx;
//
//		NodeGetValue(pIndexTree->pTree[iNodeIdx].uiLinkToTableRecord,pFieldInfo);
//
//		if(NodeValueLE(pFieldInfo,pValuePtr,iValueLen))   {
//
//			UINT32 uiValue = pIndexTree->pTree[iNodeIdx].uiLinkToLeft;
//			if(NodeAdd(&uiValue,pFieldInfo,pValuePtr,iValueLen))   {
//				pIndexTree->pTree[iNodeIdx].uiLinkToLeft = uiValue;
//			}
//
//			if(pIndexTree->bTreeGrowFlag==TRUE)   {
//
//				switch(pIndexTree->pTree[iNodeIdx].siBalans)   {
//					case 1 :  
//						pIndexTree->pTree[iNodeIdx].siBalans = 0; 
//						pIndexTree->bTreeGrowFlag = FALSE; 
//					break;
//
//					case 0 : 
//						pIndexTree->pTree[iNodeIdx].siBalans = -1; 
//					break;
//
//					case -1:
//						iNodeIdx1 = pIndexTree->pTree[iNodeIdx].uiLinkToLeft;
//						if(pIndexTree->pTree[iNodeIdx1].siBalans==-1)   {
//							pIndexTree->pTree[iNodeIdx ].uiLinkToLeft = pIndexTree->pTree[iNodeIdx1].uiLinkToRight;
//							pIndexTree->pTree[iNodeIdx1].uiLinkToRight = iNodeIdx;
//							pIndexTree->pTree[iNodeIdx ].siBalans = 0;
//							*piNodeIdx = iNodeIdx1;
//						} else {
//							iNodeIdx2 = pIndexTree->pTree[iNodeIdx1].uiLinkToRight;
//							pIndexTree->pTree[iNodeIdx1].uiLinkToRight = pIndexTree->pTree[iNodeIdx2].uiLinkToLeft;
//							pIndexTree->pTree[iNodeIdx2].uiLinkToLeft  = iNodeIdx1;
//							pIndexTree->pTree[iNodeIdx ].uiLinkToLeft  = pIndexTree->pTree[iNodeIdx2].uiLinkToRight;
//							pIndexTree->pTree[iNodeIdx2].uiLinkToRight = iNodeIdx;
//
//							if(pIndexTree->pTree[iNodeIdx2].siBalans==-1)   {
//								pIndexTree->pTree[iNodeIdx ].siBalans = 1;
//							} else   {
//								pIndexTree->pTree[iNodeIdx ].siBalans = 0;
//							}
//							if(pIndexTree->pTree[iNodeIdx2].siBalans==1)   {
//								pIndexTree->pTree[iNodeIdx1].siBalans = -1;
//							} else   {
//								pIndexTree->pTree[iNodeIdx1].siBalans = 0;
//							}
//							*piNodeIdx = iNodeIdx2;
//						}
//						pIndexTree->pTree[*piNodeIdx].siBalans = 0;
//						pIndexTree->bTreeGrowFlag = FALSE;
//						return(TRUE);
//					break;
//				}
//			}
//		} else   {
//			if(!NodeValueLE(pFieldInfo,pValuePtr,iValueLen))   {
//
//				UINT32 uiValue = pIndexTree->pTree[iNodeIdx].uiLinkToRight;
//				if(NodeAdd(&uiValue,pFieldInfo,pValuePtr,iValueLen))   {
//					pIndexTree->pTree[iNodeIdx].uiLinkToRight = uiValue;
//				}
//
//				if(pIndexTree->bTreeGrowFlag==TRUE)   {
//
//					switch(pIndexTree->pTree[iNodeIdx].siBalans)   {
//						case -1:  
//							pIndexTree->pTree[iNodeIdx].siBalans = 0; 
//							pIndexTree->bTreeGrowFlag = FALSE; 
//						break;
//
//						case  0: 
//							pIndexTree->pTree[iNodeIdx].siBalans = 1; 
//						break;
//
//						case  1:
//							iNodeIdx1 = pIndexTree->pTree[iNodeIdx].uiLinkToRight;
//							if(pIndexTree->pTree[iNodeIdx1].siBalans==1)   {
//								pIndexTree->pTree[iNodeIdx ].uiLinkToRight = pIndexTree->pTree[iNodeIdx1].uiLinkToLeft;
//								pIndexTree->pTree[iNodeIdx1].uiLinkToLeft = iNodeIdx;
//								pIndexTree->pTree[iNodeIdx ].siBalans = 0;
//								*piNodeIdx = iNodeIdx1;
//							} else   {
//								iNodeIdx2 = pIndexTree->pTree[iNodeIdx1].uiLinkToLeft;
//								pIndexTree->pTree[iNodeIdx1].uiLinkToLeft  = pIndexTree->pTree[iNodeIdx2].uiLinkToRight;
//								pIndexTree->pTree[iNodeIdx2].uiLinkToRight = iNodeIdx1;
//								pIndexTree->pTree[iNodeIdx ].uiLinkToRight = pIndexTree->pTree[iNodeIdx2].uiLinkToLeft;
//								pIndexTree->pTree[iNodeIdx2].uiLinkToLeft  = iNodeIdx;
//								if(pIndexTree->pTree[iNodeIdx2].siBalans==1)   {
//									pIndexTree->pTree[iNodeIdx ].siBalans = -1;
//								} else   {
//									pIndexTree->pTree[iNodeIdx ].siBalans = 0;
//								}
//								if(pIndexTree->pTree[iNodeIdx2].siBalans==-1)   {
//									pIndexTree->pTree[iNodeIdx1].siBalans = 1;
//								} else   {
//									pIndexTree->pTree[iNodeIdx1].siBalans = 0;
//								}
//								*piNodeIdx = iNodeIdx2;
//							}
//							pIndexTree->pTree[*piNodeIdx].siBalans = 0;
//							pIndexTree->bTreeGrowFlag = FALSE; 
//							return(TRUE);
//						break;
//					}
//				}
//			}
//			//else   {
//			//	pIndexTree->pTree[iNodeIdx].siCount++;
//			//}
//		}
//	}
//
//	return(FALSE);
//}

//VOID DTablePhysC::NodeFind(UINT32 iNodeIdx)   {	
//
//
//	if(iNodeIdx!=-1)   {
//
//		NodeGetValue(m_SpanFind_TreeData[iNodeIdx].uiLinkToTableRecord,m_SpanFind_FieldInfoPtr);
//
//		bool LoInOk;
//		bool UpInOk;
//
//		switch(m_SpanFind_FieldInfoPtr->eType)   {
//
//			case TYPE_INT32:   {
//				LoInOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || *((INT32*)m_SpanFind_CurrLowerKeyValuePtr)<=INT32(m_oNodeValue_UINT32));
//				UpInOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || *((INT32*)m_SpanFind_CurrUpperKeyValuePtr)>=INT32(m_oNodeValue_UINT32));
//			}
//			break;
//
//			case TYPE_UINT32:   {
//				LoInOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || *((UINT32*)m_SpanFind_CurrLowerKeyValuePtr)<=UINT32(m_oNodeValue_UINT32));
//				UpInOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || *((UINT32*)m_SpanFind_CurrUpperKeyValuePtr)>=UINT32(m_oNodeValue_UINT32));
//			}
//			break;
//
//			case TYPE_WSTRING:   {
//				LoInOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || wcscmp(((std::wstring *)m_SpanFind_CurrLowerKeyValuePtr)->c_str(),m_oNodeValue_WSTRING.c_str())<=0);
//				UpInOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || wcscmp(((std::wstring *)m_SpanFind_CurrUpperKeyValuePtr)->c_str(),m_oNodeValue_WSTRING.c_str())>=0);
//			}
//			break;
//
//			case TYPE_STRING:   {
//				LoInOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || strcmp(((std::string *)m_SpanFind_CurrLowerKeyValuePtr)->c_str(),m_oNodeValue_STRING.c_str())<=0);
//				UpInOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || strcmp(((std::string *)m_SpanFind_CurrUpperKeyValuePtr)->c_str(),m_oNodeValue_STRING.c_str())>=0);
//			}
//			break;
//
//			case TYPE_BYTE_10:   {
//				LoInOk = (m_SpanFind_CurrLowerKeyValuePtr==NULL || memcmp(m_SpanFind_CurrLowerKeyValuePtr,m_oNodeValue_BYTE_ARR,10)<=0);
//				UpInOk = (m_SpanFind_CurrUpperKeyValuePtr==NULL || memcmp(m_SpanFind_CurrUpperKeyValuePtr,m_oNodeValue_BYTE_ARR,10)>=0);
//			}
//			break;
//
//			case TYPE_VECTOR:   {
//				INT    iCmp;
//				size_t iLen1;
//				size_t iLen2; 
//
//				if(m_SpanFind_CurrLowerKeyValuePtr==NULL)   {
//					LoInOk=TRUE;
//				} else   {
//					iLen1 = ((std::vector<BYTE> *)m_SpanFind_CurrLowerKeyValuePtr)->size();
//					iLen2 = m_oNodeValue_VECTOR.size(); 
//					if((iCmp=memcmp(&((std::vector<BYTE> *)m_SpanFind_CurrLowerKeyValuePtr)->front(),&m_oNodeValue_VECTOR.front(),min(iLen1,iLen1)))!=0)   {
//						LoInOk=(iCmp<=0);
//					} else   {
//						LoInOk=(iLen1<=iLen2);
//					}
//				}
//
//				if(m_SpanFind_CurrUpperKeyValuePtr==NULL)   {
//					UpInOk=TRUE;
//				} else   {
//					iLen1 = ((std::vector<BYTE> *)m_SpanFind_CurrUpperKeyValuePtr)->size();
//					iLen2 = m_oNodeValue_VECTOR.size(); 
//					if((iCmp=memcmp(&((std::vector<BYTE> *)m_SpanFind_CurrUpperKeyValuePtr)->front(),&m_oNodeValue_VECTOR.front(),min(iLen1,iLen1)))!=0)   {
//						UpInOk=(iCmp>=0);
//					} else   {
//						UpInOk=(iLen1>=iLen2);
//					}
//				}
//			}
//			break;
//
//
//			default:
//				LoInOk=UpInOk=FALSE;
//				LogFail(_F,ERROR_ACCESS_DENIED,_T("Unknown DATA_TYPE(%ld) for field N %ld. DataTable <%s>"),m_SpanFind_FieldInfoPtr->eType,m_SpanFind_FieldInfoPtr->iIndex,m_Path.c_str());
//			break;
//		}
//
//		if(LoInOk)    {
//			NodeFind(m_SpanFind_TreeData[iNodeIdx].uiLinkToLeft);
//			if(UpInOk)   {
//				if(m_SpanFind_CurrIntersectPointer==NULL ||
//					m_SpanFind_CurrIntersectPointer->find(m_SpanFind_TreeData[iNodeIdx].uiLinkToTableRecord)!=m_SpanFind_CurrIntersectPointer->end())   {
//
//					m_SpanFind_UserSetPointers[m_SpanFind_FindId]->insert(m_SpanFind_TreeData[iNodeIdx].uiLinkToTableRecord);
//				}
//			}
//		}
//		if(UpInOk)    {
//			NodeFind(m_SpanFind_TreeData[iNodeIdx].uiLinkToRight);
//		}
//
//	}
//}

//BOOL DTablePhysC::NodeValueLE(SFieldInfo * pFieldInfo, BYTE * pValuePtr, WORD iValueLen)   {
//
//	switch(pFieldInfo->eType)   {
//
//		case TYPE_STRING:
//			return(strcmp((char *)pValuePtr,((std::string *)m_pNodeValue_PTR)->c_str())<=0);
//		break;
//
//		case TYPE_WSTRING:
//			return(wcscmp((wchar_t *)pValuePtr,((std::wstring *)m_pNodeValue_PTR)->c_str())<=0);
//		break;
//
//		case TYPE_VECTOR:   {
//			INT    iCmp;
//			size_t iLen1 = iValueLen;
//			size_t iLen2 = ((std::vector<BYTE> *)m_pNodeValue_PTR)->size(); 
//			if((iCmp=memcmp(pValuePtr,&((std::vector<BYTE> *)m_pNodeValue_PTR)->front(),min(iLen1,iLen1)))!=0)   {
//				return(iCmp<=0);
//			} else   {
//				return(iLen1<=iLen2);
//			}
//		}
//		break;
//
//		case TYPE_BYTE_10:
//			return(memcmp(pValuePtr,m_pNodeValue_PTR,10)<=0);
//		break;
//
//		case TYPE_INT32:
//			return(*((INT32 *)pValuePtr)<=*((INT32 *)m_pNodeValue_PTR));
//		break;
//
//		case TYPE_UINT32:
//			return(*((UINT32 *)pValuePtr)<=*((UINT32 *)m_pNodeValue_PTR));
//		break;
//
//		default:
//			LogFail(_F,ERROR_ACCESS_DENIED,_T("Unknown DATA_TYPE(%ld). DataTable <%s>"),pFieldInfo->eType,m_Path.c_str());
//			return(FALSE);
//		break;
//	}
//}

