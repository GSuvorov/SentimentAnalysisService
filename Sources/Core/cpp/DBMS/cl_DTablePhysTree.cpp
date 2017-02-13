#include "StdAfx.h"
#include "cl_Topo_DBMS.h"
#include "cl_DTablePhys.h"
#include "fn_HashFunc.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_DTablePhysTree.cpp"

/// Ctor
CIndexTree::CIndexTree() : m_pCashArr(NULL), m_pFile(NULL)   {
	Init();
}

/// Dtor
CIndexTree::~CIndexTree()   {
	if(m_pCashArr)   {
		free(m_pCashArr);
		m_pCashArr = NULL;
	}
	m_oCashMap.Clear();
}

/// Init
HRESULT CIndexTree::Init(VOID)   {
	m_iNumOfNodes  = 0;
	m_iTreeHeight  = 0;
	m_iTreeRootIdx = 0;
	m_iCurrOper    = 0;
	m_iCashNum     = 0;

	m_oCashMap.Clear();

	return(S_OK);
}

/// Добавить новый узел дерева в кэш
SBTreeNodeMem * CIndexTree::AddNewNode(VOID)   {

	SBTreeNodeMem * pNode = FindFreeCashCell();

	pNode->bChangeFlag  = TRUE;
	pNode->iHitCnt      = 1;
	pNode->iNodeIdx     = m_iNumOfNodes++;
	pNode->iLastOper    = m_iCurrOper;
	pNode->uiEntriesNum = 0;

	m_oCashMap.Add(pNode->iNodeIdx,pNode);

	return(pNode);
}

/// Получить узел дерева из файла или кэша
SBTreeNodeMem * CIndexTree::GetNode(UINT32 iNodeIdx)   {
	SBTreeNodeMem * pNode;
	if(m_oCashMap.Find(iNodeIdx))   {
		pNode = static_cast<SBTreeNodeMem *>(m_oCashMap.Second());
		pNode->iHitCnt++;
	} else   {
		pNode = FindFreeCashCell();
		LoadNode(iNodeIdx, pNode);
	}

	pNode->iLastOper = m_iCurrOper;
	return(pNode);
}

/// Начать работу с индексным файлом
HRESULT CIndexTree::FileOpenAndLoad(DiskFileC * pFile)   {
	HRESULT HR;

	m_pFile = pFile;

	if(m_pCashArr==NULL)   {
		if((m_pCashArr = (SBTreeNodeMem *)malloc(BTREE_CASH_SIZE * sizeof(SBTreeNodeMem)))==NULL)   {
			return(LogFail(_F,0,_T("No memory for IndexCashArr")));
		}
	}

	if(m_pFile->Open()!=S_OK)   return(LogFail(_F,0,_T("Index file <%s> open error"),m_pFile->m_FFullPath.c_str()));
	if((HR=Init())!=S_OK)   return(HR);
	if((HR=LoadHeader())!=S_OK)   return(HR);

	return(S_OK);
}

/// Закончить работу с индексным файлом
HRESULT CIndexTree::FileSaveAndClose(VOID)   {
	HRESULT HR;

	SBTreeNodeMem * pCash = m_pCashArr;

	for(UINT i=0; i<m_iCashNum; i++, pCash++)   {
		if(pCash->bChangeFlag)	  {
			if((HR=SaveNode(pCash))!=S_OK)   return(HR);
		}
	}

	if((HR=SaveHeader())!=S_OK)   return(HR);
	if(m_pFile->Close()!=S_OK)   	return(LogFail(_F,0,_T("Index file <%s> close error"),m_pFile->m_FFullPath.c_str()));

	return(S_OK);
}

/// Загрузить заголовок
HRESULT CIndexTree::LoadHeader(VOID)   {
	UINT iFSize = m_pFile->GetSize();
	if(iFSize<sizeof(SBTreeDataDisk) || m_pFile->ReadAt(0, static_cast<SBTreeDataDisk *>(this),sizeof(SBTreeDataDisk))!=S_OK)   {
		Init();
		AddNewNode();
		m_iTreeRootIdx = 0;
	} else   {
		UINT iNumOfNodesPlusHeader = (iFSize - 1) / BTREE_DISK_READ_BLOCK;
		if(iNumOfNodesPlusHeader != m_iNumOfNodes)   {
			return(LogFail(_F,0,_T("Read BTree: Bad idx file (%ld!=%ld)"),iNumOfNodesPlusHeader,m_iNumOfNodes));
		}
	}
	return(S_OK);
}

/// Загрузить узел в кэш
HRESULT CIndexTree::LoadNode(UINT32 iNodeIdx, SBTreeNodeMem * pNode)   {
	if(m_pFile->ReadAt(BTREE_DISK_READ_BLOCK * (iNodeIdx+1), static_cast<SBTreeNodeDisk *>(pNode),sizeof(SBTreeNodeDisk))!=S_OK)   {
		return(LogFail(_F,0,_T("Read BTree node error (Node: %ld)"),pNode->iNodeIdx));
	} else   {
		pNode->iNodeIdx    = iNodeIdx;
		pNode->bChangeFlag = FALSE;
		pNode->iLastOper   = m_iCurrOper;
		pNode->iHitCnt     = 1;
		m_oCashMap.Add(pNode->iNodeIdx,pNode);
	}
	return(S_OK);
}

/// Сохранить заголовок
HRESULT CIndexTree::SaveHeader(VOID)   {
	if(m_pFile->WriteAt(0, static_cast<SBTreeDataDisk *>(this),sizeof(SBTreeDataDisk))!=S_OK)   {
		return(LogFail(_F,0,_T("Save BTree header error")));
	}
	return(S_OK);
}

/// Сохранить узел
HRESULT CIndexTree::SaveNode(SBTreeNodeMem * pNode)   {
	if(pNode->bChangeFlag)   {
		if(m_pFile->WriteAt(BTREE_DISK_READ_BLOCK * (pNode->iNodeIdx+1), static_cast<SBTreeNodeDisk *>(pNode),sizeof(SBTreeNodeDisk))!=S_OK)   {
			return(LogFail(_F,0,_T("Save BTree node error (Node: %ld)"),pNode->iNodeIdx));
		} else   {
			pNode->bChangeFlag = FALSE;
		}
	}
	return(S_OK);
}

/// Найти свободную ячейку кэша
SBTreeNodeMem * CIndexTree::FindFreeCashCell(VOID)   {

	if(m_iCashNum<BTREE_CASH_SIZE)   {

		return(&m_pCashArr[m_iCashNum++]);

	} else   {

		UINT32 iMinHit    = 0xFFFFFFFF;
		UINT32 iMinHitIdx = 0xFFFFFFFF;

		SBTreeNodeMem * pCash = m_pCashArr;

		for(int i=0; i<BTREE_CASH_SIZE; i++, pCash++)   {
			if(pCash->iLastOper!=m_iCurrOper && iMinHit>pCash->iHitCnt)	  {
				iMinHit    = pCash->iHitCnt;
				iMinHitIdx = i;
				if(iMinHit==1)   {
					break;
				}
			}
		}

		if(iMinHit == 0xFFFFFFFF)   {
			LogFail(_F,0,_T("Cash BTree: No free node "));
			return(NULL);
		} else   {
			pCash = &m_pCashArr[iMinHitIdx]; 
			SaveNode(pCash);
			if(m_oCashMap.Find(pCash->iNodeIdx))   {
				m_oCashMap.DeleteFound();
			} else   {
				LogFail(_F,0,_T("CashMap BTree: Find in map error (Node: %ld)"),pCash->iNodeIdx);
			}
			return(pCash);
		}
	}
}

