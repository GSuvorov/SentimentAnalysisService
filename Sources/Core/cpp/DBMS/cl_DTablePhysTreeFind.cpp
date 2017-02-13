#include "StdAfx.h"
#include "cl_Topo_DBMS.h"
#include "cl_DTablePhys.h"
#include "fn_HashFunc.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_DTablePhysTreeFind.cpp"

HRESULT DTablePhysC::SpanFindInit(
	_TCHAR             * pKeyName,
	VOID               * pLowerKeyValue,
	VOID               * pUpperKeyValue,
	std::set< UINT32 > * pSetOfRecordsIndexes,
	WORD                 nFindId /* = 0*/,
	std::set< UINT32 > * pSetToIntersectWith /*= NULL*/)   {

	VISIT_CRIT_SECT(m_oCriticalSection);

	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	DFieldTopoC * pDField=m_TopoPtr->FindChild(pKeyName);

	if(pDField)   {
		return(SpanFindInit(pDField->m_IndexInParentMap,pLowerKeyValue,pUpperKeyValue,pSetOfRecordsIndexes,nFindId,pSetToIntersectWith));
	} else   {
		return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Key-name <%s> was not found (DataTable <%s>)"),pKeyName,m_Path.c_str()));
	}
}

HRESULT DTablePhysC::SpanFindInit(
	UINT32               nKeyIndex,
	VOID               * pLowerKeyValue,
	VOID               * pUpperKeyValue,
	std::set< UINT32 > * pSetOfRecordsIndexes,
	WORD                 nFindId /*= 0*/,
	std::set< UINT32 > * pSetToIntersectWith /*= NULL*/)   {

	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));

	if(nFindId>=SPAN_FIND_LIM)                        return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("SpanFind ID <%d> is out of range. DataTable <%s>"),nFindId,m_Path.c_str()));
	if(nKeyIndex>=m_FieldsNum)                        return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Index of DataField  is out of range. DataTable <%s>"),m_Path.c_str()));
	if(!m_FieldsInfoArr[nKeyIndex].bIndexedFlag)      return(LogFail(_F,ERROR_NOT_SUPPORTED,    _T("The specified filed N %ld has no <IS_INDEXED> modifier. DataTable <%s>"),nKeyIndex,m_Path.c_str()));
	if(pLowerKeyValue==NULL && pUpperKeyValue==NULL)  return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Range/Span parameters are both NULL. DataTable <%s>"),m_Path.c_str()));
	if(pSetOfRecordsIndexes==NULL)                    return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Parameter <pSetOfRecordsIndexes> is NULL. DataTable <%s>"),m_Path.c_str()));
	if(m_SpanFind_UserSetPointers[nFindId])           return(LogFail(_F,ERROR_ACCESS_DENIED,    _T("SpanFind ID <%ld> is already busy. DataTable <%s>"),nFindId,m_Path.c_str()));

	m_SpanFind_RecordFieldIdx[nFindId]   = nKeyIndex;
	m_SpanFind_LowerKeyValuePtr[nFindId] = pLowerKeyValue;
	m_SpanFind_UpperKeyValuePtr[nFindId] = pUpperKeyValue;
	m_SpanFind_UserSetPointers[nFindId]  = pSetOfRecordsIndexes;
	m_SpanFind_IntersectPointers[nFindId] = pSetToIntersectWith;

	return(S_OK);
}

BOOL DTablePhysC::GetFreeSpanFindId(WORD * pnFindId)   {
	for(int i=0; i<SPAN_FIND_LIM; i++)   {
		if(m_SpanFind_UserSetPointers[i]==NULL)   {
			*pnFindId=i;
			return(TRUE);
		}
	}
	return(FALSE);
}


HRESULT DTablePhysC::SpanFindInitEx(
	_TCHAR             * pKeyName,
	VOID               * pLowerKeyValue,
	VOID               * pUpperKeyValue,
	std::set< UINT32 > * pSetOfRecordsIndexes,
	WORD               * pnFindId,
	std::set< UINT32 > * pSetToIntersectWith /*= NULL*/)   {

	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())     return(LogFail(_F,ERROR_ACCESS_DENIED,    _T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(pnFindId==NULL)           return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Parameter <SpanFindId> is NULL (DataTable <%s>)"),m_Path.c_str()));
	if(!GetFreeSpanFindId(pnFindId)) return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("No free <SpanFindId> (DataTable <%s>)"),m_Path.c_str()));

	return(SpanFindInit(pKeyName,pLowerKeyValue,pUpperKeyValue,pSetOfRecordsIndexes,*pnFindId,pSetToIntersectWith));

}

HRESULT DTablePhysC::SpanFindInitEx(
	UINT32               nKeyIndex,
	VOID               * pLowerKeyValue,
	VOID               * pUpperKeyValue,
	std::set< UINT32 > * pSetOfRecordsIndexes,
	WORD               * pnFindId,
	std::set< UINT32 > * pSetToIntersectWith /*= NULL*/)   {

	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())     return(LogFail(_F,ERROR_ACCESS_DENIED,    _T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(pnFindId==NULL)           return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Parameter <SpanFindId> is NULL (DataTable <%s>)"),m_Path.c_str()));
	if(!GetFreeSpanFindId(pnFindId)) return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("No free <SpanFindId> (DataTable <%s>)"),m_Path.c_str()));

	return(SpanFindInit(nKeyIndex,pLowerKeyValue,pUpperKeyValue,pSetOfRecordsIndexes,*pnFindId,pSetToIntersectWith));

}


HRESULT DTablePhysC::SpanFindCancel(WORD nFindId)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())        return(LogFail(_F,ERROR_ACCESS_DENIED,    _T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(nFindId>=SPAN_FIND_LIM)      return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("SpanFind ID <%d> is out of range. DataTable <%s>"),nFindId,m_Path.c_str()));

	m_SpanFind_RecordFieldIdx[nFindId]   = 0;
	m_SpanFind_LowerKeyValuePtr[nFindId] = NULL;
	m_SpanFind_UpperKeyValuePtr[nFindId] = NULL;
	m_SpanFind_UserSetPointers[nFindId]  = NULL;

	return(S_OK);
}


HRESULT DTablePhysC::SpanFind(WORD nFindId)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())                        return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(nFindId>=FIND_LIM)                           return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("SpanFind ID <%d> is out of range. DataTable <%s>"),nFindId,m_Path.c_str()));
	if(m_SpanFind_UserSetPointers[nFindId]==NULL)   return(LogFail(_F,ERROR_NOT_READY,_T("Method <SpanFindInit()> was not called for DataTable <%s>"),m_Path.c_str()));

	m_SpanFind_UserSetPointers[nFindId]->clear();

	m_SpanFind_FindId               = nFindId;
	m_SpanFind_FieldInfoPtr         = &m_FieldsInfoArr[m_SpanFind_RecordFieldIdx[nFindId]];
	m_SpanFind_CurrBTreePtr         = &m_SpanFind_FieldInfoPtr->oIndexTree;
//	m_SpanFind_CurrBTreeArr         = m_SpanFind_CurrBTreePtr->pTree;
	m_SpanFind_CurrLowerKeyValuePtr = m_SpanFind_LowerKeyValuePtr[m_SpanFind_FindId];
	m_SpanFind_CurrUpperKeyValuePtr = m_SpanFind_UpperKeyValuePtr[m_SpanFind_FindId];
	m_SpanFind_CurrIntersectPointer = m_SpanFind_IntersectPointers[m_SpanFind_FindId];
/*
	printf("\nHead: %ld, TreeHeght: %ld, Nodes: %ld\n\n",m_SpanFind_CurrBTreePtr->m_iTreeRootIdx,m_SpanFind_CurrBTreePtr->m_iTreeHeight,m_SpanFind_CurrBTreePtr->uiRealNumOfNodes);
	for(UINT i=0; i<m_SpanFind_CurrBTreePtr->uiRealNumOfNodes; i++)   {
		printf("~%-2ld   ",i);
		for(int j=0; j<m_SpanFind_CurrBTreeArr[i].uiEntriesNum; j++)   {
//			printf("%-2ld:~%2ld ", m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].uiLinkToTableRecord, m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].iLinkToBTreeNode);	
			printf("%8.8lX:~%2ld ", m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].iLinkToHashRecord , m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].iLinkToBTreeNode);	
		}
		printf("\n");
	}
	printf("\n");
*/

	//for(int i=0;i<pFieldInfo->oIndexTree.uiRealNumOfNodes;i++)   {
	//	printf("\n%ld:  Rec:%ld (%ld:%ld) %s",
	//		i,
	//		m_SpanFind_TreeData[i].uiLinkToTableRecord,
	//		m_SpanFind_TreeData[i].uiLinkToLeft,
	//		m_SpanFind_TreeData[i].uiLinkToRight,(pFieldInfo->oIndexTree.m_iTreeRootIdx==i)?"*":"");
	//}

	m_SpanFind_CurrBTreePtr->SetNewOper();

	NodeSearchR(m_SpanFind_CurrBTreePtr->m_iTreeRootIdx,m_SpanFind_CurrBTreePtr->m_iTreeHeight);

	return(m_SpanFind_UserSetPointers[nFindId]->size()?S_OK:S_FALSE);
}

VOID DTablePhysC::NodeSearchR(UINT32 iNodeIdx, UINT32 m_iTreeHeight, BOOL bNeedCheck /*= TRUE*/)   {
	UINT j;

	SBTreeNodeMem * pNode = m_SpanFind_CurrBTreePtr->GetNode(iNodeIdx);

	if(m_iTreeHeight == 0)	  {
		BOOL bFound = FALSE;
		BOOL bResult;

		for(j=0; j<pNode->uiEntriesNum; j++)   {
			bResult = NodeEstimateAndSaveInfo(pNode->aoEntriesArr[j].iLinkToHashRecord, bNeedCheck);
			if(bFound)   {
				if(!bResult)   break;
			} else   {
				if(bResult)   bFound = TRUE;
			}
		}
	} else   {

		if(bNeedCheck)   {

			UINT32 n,iCmpRes, aiCmpResArr[BTREE_NODE_ENTRIES_LIM];

			for(j=0; j<pNode->uiEntriesNum; j++)   {
				 
				iCmpRes = aiCmpResArr[j] = NodeCheckInterval(pNode->aoEntriesArr[j].iLinkToHashRecord);

				if(!(iCmpRes & DEF_UpInEq))   {
					if(j==0)   j=1;
					break;
				}
			}

			n=j;

			for(j=0; j<n; j++)   {
				iCmpRes = aiCmpResArr[j];
				if(iCmpRes & DEF_LoInEq)  {
					if((iCmpRes & DEF_LoIn) && j && aiCmpResArr[j-1] != 0xFF00)   {
						NodeSearchR(pNode->aoEntriesArr[j-1].iLinkToBTreeNode, m_iTreeHeight-1, TRUE);
						aiCmpResArr[j-1] = 0xFF00;
					}
					NodeSearchR(pNode->aoEntriesArr[j].iLinkToBTreeNode, m_iTreeHeight-1, j+1==n);
					aiCmpResArr[j] = 0xFF00;
				}
			}
			if(j && aiCmpResArr[j-1] != 0xFF00)   {
				NodeSearchR(pNode->aoEntriesArr[j-1].iLinkToBTreeNode, m_iTreeHeight-1, TRUE);
				aiCmpResArr[j-1] = 0xFF00;
			}




/*		
			UINT32 iCmpRes, iStep=0, aiCmpResArr[BTREE_NODE_ENTRIES_LIM];
			for(j=0; j<pNode->uiEntriesNum; j++)   {
				 
				iCmpRes = aiCmpResArr[j] = NodeCheckInterval(pNode->aoEntriesArr[j].iLinkToHashRecord);

				if(iStep==0)   {
					if(iCmpRes & DEF_LoInEq)  {
						if((iCmpRes & DEF_LoIn) && j)   {
							NodeSearchR(pNode->aoEntriesArr[j-1].iLinkToBTreeNode, m_iTreeHeight-1, TRUE);
							aiCmpResArr[j-1] = 0xFF00;
						}
						NodeSearchR(pNode->aoEntriesArr[j].iLinkToBTreeNode, m_iTreeHeight-1);
						aiCmpResArr[j] = 0xFF00;
						iStep = 1;
					}
				} else if(iStep==1)   {
					if(!(iCmpRes & DEF_UpInEq))   {
						break;
					}
					NodeSearchR(pNode->aoEntriesArr[j].iLinkToBTreeNode, m_iTreeHeight-1);
					aiCmpResArr[j] = 0xFF00;
				}
			}

			if(j && aiCmpResArr[j-1]!=0xFF00)   {
				NodeSearchR(pNode->aoEntriesArr[j-1].iLinkToBTreeNode, m_iTreeHeight-1);
				aiCmpResArr[j-1] = 0xFF00;
			}
*/

		} else   {

			for(j=0; j<pNode->uiEntriesNum; j++)   {
				NodeSearchR(pNode->aoEntriesArr[j].iLinkToBTreeNode, m_iTreeHeight-1, FALSE);
			}

		}

			//if((iCmpRes & DEF_UpInEq))   {
			//	if(iCmpRes & DEF_LoInEq)  {
			//		if((iCmpRes & DEF_LoIn) && j && aiCmpResArr[j-1]!=0xFF00)   {
			//			NodeSearchR(pNode->aoEntriesArr[j-1].iLinkToBTreeNode, m_iTreeHeight-1);
			//			aiCmpResArr[j-1] = 0xFF00;
			//		}

			//		NodeSearchR(pNode->aoEntriesArr[j].iLinkToBTreeNode, m_iTreeHeight-1);
			//		aiCmpResArr[j] = 0xFF00;

			//		//if(j && aiCmpResArr[j-1]!=0xFF00)   {
			//		//	NodeSearchR(pNode->aoEntriesArr[j-1].iLinkToBTreeNode, m_iTreeHeight-1);
			//		//	aiCmpResArr[j-1] = 0xFF00;
			//		//}
			//	}
			//} else   {
			//	if(iCmpRes & DEF_LoInEq)  {
			//		if((iCmpRes & DEF_LoIn) && j && aiCmpResArr[j-1]!=0xFF00)   {
			//			NodeSearchR(pNode->aoEntriesArr[j-1].iLinkToBTreeNode, m_iTreeHeight-1);
			//			aiCmpResArr[j-1] = 0xFF00;
			//		}

			//		NodeSearchR(pNode->aoEntriesArr[j].iLinkToBTreeNode, m_iTreeHeight-1);
			//		aiCmpResArr[j] = 0xFF00;

			//		//if(j && aiCmpResArr[j-1]!=0xFF00)   {
			//		//	NodeSearchR(pNode->aoEntriesArr[j-1].iLinkToBTreeNode, m_iTreeHeight-1);
			//		//	aiCmpResArr[j-1] = 0xFF00;
			//		//}
			//	}

				//if(j && (aiCmpResArr[j-1] & DEF_UpInEq))   {
				//	NodeSearchR(pNode->aoEntriesArr[j-1].iLinkToBTreeNode, m_iTreeHeight-1);
				//	aiCmpResArr[j-1] = 0xFF00;
				//}
				//NodeSearchR(pNode->aoEntriesArr[j].iLinkToBTreeNode, m_iTreeHeight-1);
				//aiCmpResArr[j] = 0xFF00;
				//break;
//			}
		//if(j && (aiCmpResArr[j-1]!=0xFF00))   {
		//	NodeSearchR(pNode->aoEntriesArr[j-1].iLinkToBTreeNode, m_iTreeHeight-1);
		//	aiCmpResArr[j-1] = 0xFF00;
		//}
	}
}

INT32 DTablePhysC::NodeSplit(UINT32 iNodeIdx)   {

	SBTreeNodeMem * pNode    = m_SpanFind_CurrBTreePtr->GetNode(iNodeIdx);
	SBTreeNodeMem * pNewNode = m_SpanFind_CurrBTreePtr->AddNewNode();

//	printf("(Split: %ld->%ld) ",iNodeIdx,iIdx);

	memcpy(&pNewNode->aoEntriesArr[0],&pNode->aoEntriesArr[BTREE_NODE_ENTRIES_LIM/2],(BTREE_NODE_ENTRIES_LIM/2)*sizeof(SBTreeEntry));
//	for(int j=0; j<BTREE_NODE_ENTRIES_LIM/2; j++)   {
//		pNewNode->aoEntriesArr[j].iLinkToHashRecord    = pNode->aoEntriesArr[BTREE_NODE_ENTRIES_LIM/2+j].iLinkToHashRecord;
////		pNewNode->aoEntriesArr[j].uiLinkToTableRecord   = pNode->aoEntriesArr[BTREE_NODE_ENTRIES_LIM/2+j].uiLinkToTableRecord;
//		pNewNode->aoEntriesArr[j].iLinkToBTreeNode     = pNode->aoEntriesArr[BTREE_NODE_ENTRIES_LIM/2+j].iLinkToBTreeNode;
//	}

	pNewNode->uiEntriesNum = pNode->uiEntriesNum = BTREE_NODE_ENTRIES_LIM/2;
	pNode->SetChangeFlag();

	return(pNewNode->iNodeIdx);
}

INT32 DTablePhysC::NodeInsertR(UINT32 iNodeIdx, UINT32 iHashIdx, UINT32 m_iTreeHeight)   {
	UINT i, j;
//	Key v = value;

	SBTreeEntry t;

	SBTreeNodeMem * pNode = m_SpanFind_CurrBTreePtr->GetNode(iNodeIdx);

	t.iLinkToHashRecord    = iHashIdx;
//	t.uiLinkToTableRecord   = m_iCurrNodeTableIdx;
	t.iLinkToBTreeNode = -1; // !!!
	// printf("(%lX) ",t.iLinkToHashRecord);

	if(m_iTreeHeight == 0)   {
		for(j=0; j<pNode->uiEntriesNum; j++)   {
			NodeGetValueViaHash(pNode->aoEntriesArr[j].iLinkToHashRecord);
			if(NodeValueL())   {
//printf(" *%ld:0* ",iNodeIdx);
				break;
			}
		}
	} else   {
		for(j=0; j<pNode->uiEntriesNum; j++)   {
			BOOL bDoFlag = FALSE;
			if(j+1 == pNode->uiEntriesNum)   {
				bDoFlag = TRUE;
//				printf(" *%ld:1* ",iNodeIdx);
			} else   {
				NodeGetValueViaHash(pNode->aoEntriesArr[j+1].iLinkToHashRecord);
				if(NodeValueL())   {
					bDoFlag = TRUE;
//					printf(" *%ld:2* ",iNodeIdx);
				}
			}

			if(bDoFlag)   {
				INT32 iRetIdx = NodeInsertR(pNode->aoEntriesArr[j++].iLinkToBTreeNode,iHashIdx,m_iTreeHeight-1);

				if(iRetIdx==-1)   return(-1);

				SBTreeNodeMem * pRetNode    = m_SpanFind_CurrBTreePtr->GetNode(iRetIdx);
				t.iLinkToHashRecord    = pRetNode->aoEntriesArr[0].iLinkToHashRecord;
//				t.uiLinkToTableRecord   = pRetNode->aoEntriesArr[0].uiLinkToTableRecord;
				t.iLinkToBTreeNode = iRetIdx;
//				printf(" *1* ");
				break;
			}
		}
	}

	for(i=pNode->uiEntriesNum; i>j; i--)   {
//		printf(" *+* ");
		pNode->aoEntriesArr[i].iLinkToHashRecord    = pNode->aoEntriesArr[i-1].iLinkToHashRecord;
//		pNode->aoEntriesArr[i].uiLinkToTableRecord   = pNode->aoEntriesArr[i-1].uiLinkToTableRecord;
		pNode->aoEntriesArr[i].iLinkToBTreeNode = pNode->aoEntriesArr[i-1].iLinkToBTreeNode;
	}
//	printf(" *([%ld]<-%ld)* ",j,t.uiLinkToTableRecord);
	pNode->aoEntriesArr[j].iLinkToHashRecord    = t.iLinkToHashRecord;
//	pNode->aoEntriesArr[j].uiLinkToTableRecord   = t.uiLinkToTableRecord;
	pNode->aoEntriesArr[j].iLinkToBTreeNode = t.iLinkToBTreeNode;

	if(++pNode->uiEntriesNum < BTREE_NODE_ENTRIES_LIM)   {
		return(-1);
	} else   {
		return(NodeSplit(iNodeIdx));
	}
}


BOOL DTablePhysC::NodeAdd(UINT32 iHashIdx)   {
	VISIT_CRIT_SECT(m_oCriticalSection);

	m_SpanFind_CurrBTreePtr = &(m_SpanFind_FieldInfoPtr->oIndexTree);
//	m_SpanFind_CurrBTreeArr = m_SpanFind_CurrBTreePtr->pTree;

	m_SpanFind_CurrBTreePtr->SetNewOper();

	INT32 iRetIdx = NodeInsertR(m_SpanFind_CurrBTreePtr->m_iTreeRootIdx, iHashIdx, m_SpanFind_CurrBTreePtr->m_iTreeHeight);
/*
	if(*((INT *)pValuePtr)==20)   {
		printf("\nHead: %ld, TreeHeght: %ld, Nodes: %ld\n\n",m_SpanFind_CurrBTreePtr->m_iTreeRootIdx,m_SpanFind_CurrBTreePtr->m_iTreeHeight,m_SpanFind_CurrBTreePtr->uiRealNumOfNodes);
		for(UINT i=0; i<m_SpanFind_CurrBTreePtr->uiRealNumOfNodes; i++)   {
			printf("~%-2ld   ",i);
			for(int j=0; j<m_SpanFind_CurrBTreeArr[i].uiEntriesNum; j++)   {
				printf("%-2ld:~%2ld ", m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].uiLinkToTableRecord, m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].iLinkToBTreeNode);	
			}
			printf("\n");
		}
	}
*/
/*
	if(m_SpanFind_FieldInfoPtr->eType == TYPE_INT32 && iRetIdx == -1)   {
		printf("\nHead: %ld, TreeHeght: %ld, Nodes: %ld\n\n",m_SpanFind_CurrBTreePtr->m_iTreeRootIdx,m_SpanFind_CurrBTreePtr->m_iTreeHeight,m_SpanFind_CurrBTreePtr->uiRealNumOfNodes);
		for(UINT i=0; i<m_SpanFind_CurrBTreePtr->uiRealNumOfNodes; i++)   {
			printf("~%-2ld   ",i);
			INT iLast=-10000;
			for(int j=0; j<m_SpanFind_CurrBTreeArr[i].uiEntriesNum; j++)   {
				printf("%-2ld:~%2ld ", m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].uiLinkToTableRecord, m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].iLinkToBTreeNode);	
	//			printf("%8.8lX:~%2ld ", m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].iLinkToHashRecord , m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].iLinkToBTreeNode);	
				if(j && m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j-1].uiLinkToTableRecord > m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].uiLinkToTableRecord)   printf(" ### ");
			}
			printf("\n");
		}
		printf("\n");
	}
*/
	if(iRetIdx == -1)   return(TRUE);

	SBTreeNodeMem * pNewNode = m_SpanFind_CurrBTreePtr->AddNewNode();
	SBTreeNodeMem * pRetNode = m_SpanFind_CurrBTreePtr->GetNode(iRetIdx);
	SBTreeNodeMem * pTopNode = m_SpanFind_CurrBTreePtr->GetNode(m_SpanFind_CurrBTreePtr->m_iTreeRootIdx);

	pNewNode->uiEntriesNum = 2;

	pNewNode->aoEntriesArr[0].iLinkToHashRecord    = pTopNode->aoEntriesArr[0].iLinkToHashRecord;
//	pNewNode->aoEntriesArr[0].uiLinkToTableRecord   = pTopNode->aoEntriesArr[0].uiLinkToTableRecord;
	pNewNode->aoEntriesArr[1].iLinkToHashRecord    = pRetNode->aoEntriesArr[0].iLinkToHashRecord;
//	pNewNode->aoEntriesArr[1].uiLinkToTableRecord   = pRetNode->aoEntriesArr[0].uiLinkToTableRecord;
	pNewNode->aoEntriesArr[0].iLinkToBTreeNode     = m_SpanFind_CurrBTreePtr->m_iTreeRootIdx;
	pNewNode->aoEntriesArr[1].iLinkToBTreeNode     = iRetIdx;

	m_SpanFind_CurrBTreePtr->m_iTreeRootIdx = pNewNode->iNodeIdx;
	m_SpanFind_CurrBTreePtr->m_iTreeHeight++;

//printf("<2: %ld,%ld> ",m_SpanFind_CurrBTreePtr->uiRealNumOfNodes,m_SpanFind_CurrBTreePtr->m_iTreeHeight);
/*
	if(m_SpanFind_FieldInfoPtr->eType == TYPE_INT32)   {
		printf("\n!!! Head: %ld, TreeHeght: %ld, Nodes: %ld\n\n",m_SpanFind_CurrBTreePtr->m_iTreeRootIdx,m_SpanFind_CurrBTreePtr->m_iTreeHeight,m_SpanFind_CurrBTreePtr->uiRealNumOfNodes);
		for(UINT i=0; i<m_SpanFind_CurrBTreePtr->uiRealNumOfNodes; i++)   {
			printf("~%-2ld   ",i);
			INT iLast=-10000;
			for(int j=0; j<m_SpanFind_CurrBTreeArr[i].uiEntriesNum; j++)   {
				printf("%-2ld:~%2ld ", m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].uiLinkToTableRecord, m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].iLinkToBTreeNode);	
	//			printf("%8.8lX:~%2ld ", m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].iLinkToHashRecord , m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].iLinkToBTreeNode);	
				if(j && m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j-1].uiLinkToTableRecord > m_SpanFind_CurrBTreeArr[i].aoEntriesArr[j].uiLinkToTableRecord)   printf(" ### ");
			}
			printf("\n");
		}
		printf("\n");
	}
*/
	return(TRUE);
}

// !!! 