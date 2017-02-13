#include "StdAfx.h"

#include ".\SemanticClassesTree.h"
#include "..\ASSInterface\TBaseNames.h"

#include <set>

using namespace SS::CoSeDi;
using namespace SS::Interface::Core::DBMS;
using namespace SS::Dictionary::DATA_TYPE::NAMES;
using namespace SS::MANAGER::Constants;

CSemanticClassesTree::CSemanticClassesTree() : zm_iConnectionCnt(0), zm_hMutex(NULL), m_pLingvoBaseManager(NULL), m_pCurrTbl(NULL) {
	m_pDtaBaseR = 
	m_pDtaBaseE = NULL;
	m_pTblTreeR =
	m_pTblTreeE = NULL;

	m_aTreeAddArr[0] = &m_iClassId;
	m_aTreeAddArr[1] = &m_iParentId;
	m_aTreeAddArr[2] = &m_sClassName;
	m_aTreeAddArr[3] = &m_sClassShortName;
	m_aTreeAddArr[4] = &m_iClassColor;

	m_aTreeFindArr[0] = 
	m_aTreeFindArr[1] = 
	m_aTreeFindArr[2] = 
	m_aTreeFindArr[3] = 
	m_aTreeFindArr[4] = NULL;
}

CSemanticClassesTree::~CSemanticClassesTree()   {
	//if(m_pTblTreeR)   m_pTblTreeR->Close();
	//if(m_pTblTreeE)   m_pTblTreeE->Close();

	//m_pLingvoBaseManager->UnRegisterDeleteAndClose( c_szCoSeDiRus, &m_pDtaBaseR);
	//m_pLingvoBaseManager->UnRegisterDeleteAndClose( c_szCoSeDiEng, &m_pDtaBaseE);

	zm_iConnectionCnt=1;
	zm_fDbmsDisConnect();

	if(zm_hMutex)     { ReleaseMutex(zm_hMutex); CloseHandle(zm_hMutex); zm_hMutex=NULL; };
}

void CSemanticClassesTree::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)   {
	SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);

	m_pLingvoBaseManager = 
		(SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)
		GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);

};

BOOL CSemanticClassesTree::SetCurrTbl(UINT iDictNum)   {
	if(zm_iConnectionCnt)   {
		if(iDictNum==ednRSemanticClass)   {
			m_pCurrTbl = m_pTblTreeR;
			return(TRUE);
		} else if(iDictNum==ednESemanticClass)   {
			m_pCurrTbl = m_pTblTreeE;
			return(TRUE);
		} else   {
			m_pCurrTbl = NULL;
			LogMess(L"Неверный номер словаря : %ld",iDictNum);	
			return(FALSE); 
		}
	} else   {
		LogMess(L"Таблицы DBMS не подключены.");	
		return(FALSE); 
	}
}

BOOL CSemanticClassesTree::zm_fDbmsConnect(VOID)   {

	if(zm_iConnectionCnt++==0)   {

		if(zm_hMutex==NULL)   zm_hMutex = CreateMutex(NULL, FALSE, L"CSemanticClassesTree_MUTEX");
//printf(" PTR:%p ",zm_hMutex);
		if(zm_hMutex)   WaitForSingleObject(zm_hMutex,1000); // 1 sec
//printf(" 1 ");
		if(m_pLingvoBaseManager == NULL)   {
			m_pLingvoBaseManager = 
						(SS::Interface::Core::ResourceManagers::ILingvoBaseManager *)
						GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);

			if(m_pLingvoBaseManager == NULL)   {
				LogMess(L"Интерфейс модуля <LingvoBaseManager> не получен.");
				return(FALSE);
			}
		}

		m_pLingvoBaseManager->CreateOpenAndRegister( c_szCoSeDiRus, &m_pDtaBaseR);
		m_pLingvoBaseManager->CreateOpenAndRegister( c_szCoSeDiEng, &m_pDtaBaseE);

		if(m_pDtaBaseR == NULL || m_pDtaBaseE == NULL)   {
			BOOL bFlagR = FALSE, bFlagE = FALSE;

			if(m_pDtaBaseR != NULL)    { 
				bFlagR = TRUE; 
				m_pLingvoBaseManager->UnRegisterDeleteAndClose( c_szCoSeDiRus, &m_pDtaBaseR);
			}
			if(m_pDtaBaseE != NULL)    { 
				bFlagE = TRUE; 
				m_pLingvoBaseManager->UnRegisterDeleteAndClose( c_szCoSeDiEng, &m_pDtaBaseE); 
			}

			if(!bFlagR)    { 
				LogMess(L"Интерфейс базы <CoSeDiRussian> не получен.");	
				return(FALSE); 
			}
			if(!bFlagE)    { 
				LogMess(L"Интерфейс базы <CoSeDiEnglish> не получен.");	
				return(FALSE); 
			}
		}

		// ---

		if(m_pDtaBaseR->MoveToDataTable(L"SemanticClassesTree")!=S_OK)   {
			LogMess(L"Интерфейс таблицы <CoSeDiRussian::SemanticClassesTree> не получен.");	
			return(FALSE); 
		}

		if(m_pDtaBaseE->MoveToDataTable(L"SemanticClassesTree")!=S_OK)   {
			LogMess(L"Интерфейс таблицы <CoSeDiEnglish::SemanticClassesTree> не получен.");	
			return(FALSE); 
		}

		// ---

		if(m_pDtaBaseR->MoveToDataTable(L"SemanticClassesTree")!=S_OK)   {
			LogMess(L"Таблица <CoSeDiRussian::SemanticClassesTree> не найдена.");	
			return(FALSE); 
		}
		if((m_pTblTreeR = m_pDtaBaseR->OpenDataTable())==NULL)   {
			LogMess(L"Интерфейс таблицы <CoSeDiRussian::SemanticClassesTree> не получен.");	
			return(FALSE); 
		}
		if(m_pTblTreeR->AddInit(m_aTreeAddArr)!=S_OK)   {
			LogMess(L"Инициализация добавления для <CoSeDiRussian::SemanticClassesTree> не прошла.");	
			return(FALSE); 
		}
		if(m_pTblTreeR->FindInit(L"IDParent",&m_iParentId,m_aTreeFindArr,fenFindInTreeByIdParent)!=S_OK)   {
			LogMess(L"Инициализация поиска для <CoSeDiRussian::SemanticClassesTree> по <IDParent> не прошла.");	
			return(FALSE); 
		}
		if(m_pTblTreeR->FindInit(L"IDClass",&m_iClassId,m_aTreeFindArr,fenFindInTreeByIdClass)!=S_OK)   {
			LogMess(L"Инициализация поиска для <CoSeDiRussian::SemanticClassesTree> по <IDClass> не прошла.");	
			return(FALSE); 
		}
		if(m_pTblTreeR->FindInit(L"Name",&m_sClassName,m_aTreeFindArr,fenFindInTreeByName)!=S_OK)   {
			LogMess(L"Инициализация поиска для <CoSeDiRussian::SemanticClassesTree> по <Name> не прошла.");	
			return(FALSE); 
		}
		if(m_pTblTreeR->FindInit(L"ShortName",&m_sClassShortName,m_aTreeFindArr,fenFindInTreeByShortName)!=S_OK)   {
			LogMess(L"Инициализация поиска для <CoSeDiRussian::SemanticClassesTree> по <ShortName> не прошла.");	
			return(FALSE); 
		}

		// ---

		if(m_pDtaBaseE->MoveToDataTable(L"SemanticClassesTree")!=S_OK)   {
			LogMess(L"Таблица <CoSeDiEnglish::SemanticClassesTree> не найдена.");	
			return(FALSE); 
		}
		if((m_pTblTreeE = m_pDtaBaseE->OpenDataTable())==NULL)   {
			LogMess(L"Интерфейс таблицы <CoSeDiEnglish::SemanticClassesTree> не получен.");	
			return(FALSE); 
		}
		if(m_pTblTreeE->AddInit(m_aTreeAddArr)!=S_OK)   {
			LogMess(L"Инициализация добавления для <CoSeDiEnglish::SemanticClassesTree> не прошла.");	
			return(FALSE); 
		}
		if(m_pTblTreeE->FindInit(L"IDParent",&m_iParentId,m_aTreeFindArr,fenFindInTreeByIdParent)!=S_OK)   {
			LogMess(L"Инициализация поиска для <CoSeDiEnglish::SemanticClassesTree> по <IDParent> не прошла.");	
			return(FALSE); 
		}
		if(m_pTblTreeE->FindInit(L"IDClass",&m_iClassId,m_aTreeFindArr,fenFindInTreeByIdClass)!=S_OK)   {
			LogMess(L"Инициализация поиска для <CoSeDiEnglish::SemanticClassesTree> по <IDClass> не прошла.");	
			return(FALSE); 
		}
		if(m_pTblTreeE->FindInit(L"Name",&m_sClassName,m_aTreeFindArr,fenFindInTreeByName)!=S_OK)   {
			LogMess(L"Инициализация поиска для <CoSeDiEnglish::SemanticClassesTree> по <Name> не прошла.");	
			return(FALSE); 
		}
		if(m_pTblTreeE->FindInit(L"ShortName",&m_sClassShortName,m_aTreeFindArr,fenFindInTreeByShortName)!=S_OK)   {
			LogMess(L"Инициализация поиска для <CoSeDiEnglish::SemanticClassesTree> по <ShortName> не прошла.");	
			return(FALSE); 
		}
	}
	return(TRUE);
}

BOOL CSemanticClassesTree::zm_fDbmsDisConnect(VOID)   {

	if(zm_iConnectionCnt==1)   {
		zm_iConnectionCnt =0;

		if(m_pTblTreeR)   { m_pTblTreeR->Close(); m_pTblTreeR=NULL; }
		if(m_pTblTreeE)   { m_pTblTreeE->Close(); m_pTblTreeE=NULL; }

		if(m_pDtaBaseR)   { m_pLingvoBaseManager->UnRegisterDeleteAndClose( c_szCoSeDiRus, &m_pDtaBaseR); m_pDtaBaseR=NULL; }
		if(m_pDtaBaseE)   { m_pLingvoBaseManager->UnRegisterDeleteAndClose( c_szCoSeDiEng, &m_pDtaBaseE); m_pDtaBaseE=NULL; }

		if(zm_hMutex)     { ReleaseMutex(zm_hMutex); };

	} else   {
		zm_iConnectionCnt--;
	}
	return(TRUE);
}


class CSet_Str   {

	public:

		BOOL Add(CONST _TCHAR * pszStr)   {
			_oRet=_oSet.insert(pszStr);
			_oItr=_oRet.first;	
			return(_oRet.second==TRUE);
		}

		BOOL Find(CONST _TCHAR * pszStr)  {
			_oItr=_oSet.find(pszStr);
			return(_oItr!=_oSet.end());
		}

		UINT           Size(VOID)    { return((UINT)_oSet.size()); }
		VOID           Clear(VOID)   { _oSet.clear();	}
		VOID           Start(VOID)   { _oItr = _oSet.begin(); _iIdx=-1; }
		BOOL           Get(VOID)     { if(++_iIdx) _oItr++; return(_oItr != _oSet.end()); }
		const std::wstring * First(VOID)   { return(&((*_oItr))); }

	private:

		typedef std::set< std::wstring >   TSet;
		typedef TSet::iterator             TItr;
		typedef std::pair<TItr, BOOL>      TRet;

		TSet  _oSet;
		TItr  _oItr;
		TRet  _oRet; 
		INT   _iIdx;

};



TYPE_DictIndex CSemanticClassesTree::GetClassParentId(IN TYPE_DictIndex iClassId)   {
	DBMS_CONNECT_SCT;
	if(!SetCurrTbl(iClassId.GetDictionaryNumber()))   return(FALSE);

	TYPE_DictIndex iParentId;

	m_aTreeFindArr[1] = &m_iParentId;

	m_iClassId = iClassId.GetDictionaryIndexWithoutFlags();
	if(m_pCurrTbl->FindOne(fenFindInTreeByIdClass)!=S_OK)   {
		iParentId = -1;
	} else   {
		iParentId = m_iParentId;
	}
	
	m_aTreeFindArr[1] = NULL;

	return(iParentId);
}

BOOL CSemanticClassesTree::ZMethod(VOID * pDataPtr1 /*= NULL*/, VOID * pDataPtr2 /*= NULL*/)   {

	TYPE_DictIndex iClassId = *((TYPE_DictIndex *)pDataPtr1);

	DBMS_CONNECT_SCT;
	if(!SetCurrTbl(iClassId.GetDictionaryNumber()))   return(FALSE);

	SetShortName(iClassId);

	return(TRUE);
}

BOOL CSemanticClassesTree::SetShortName(IN TYPE_DictIndex iClassId)   {
	BOOL           bRet;
	std::wstring   sName;
	std::wstring   sShortName;
	UINT32         iColor;
	TYPE_DictIndex iParent;

	GetClassNameById(iClassId, &sName, &iColor, NULL);
	iParent = GetClassParentId(iClassId);
	if(iParent.GetDictionaryIndex() != -1)   {
		GetShortName(iParent, &sShortName);
		bRet = RenameClass(iClassId, &sName, iColor, &sShortName);
	} else   {
		bRet = FALSE;
	}

	return( bRet );
}

BOOL CSemanticClassesTree::GetShortName(IN TYPE_DictIndex iParentClassId, OUT std::wstring * psClassShortName)   {
	//m_aTreeFindArr[0] = &m_iClassId;
	//m_aTreeFindArr[1] = &m_iParentId;
	//m_aTreeFindArr[2] = &m_sClassName;
	//m_aTreeFindArr[3] = &m_sClassShortName;
	//m_aTreeFindArr[4] = &m_iClassColor;

	std::wstring sShortName;
	WCHAR        szShortName[128], * pCh;
	CSet_Str     oNamesSet;

	std::list<TYPE_DictIndex>           oChildrenList;
	std::list<TYPE_DictIndex>::iterator iListItr;

	GetChildren(iParentClassId, &oChildrenList);
	iListItr = oChildrenList.begin();
	while(iListItr != oChildrenList.end())   {
		GetClassNameById(*iListItr, NULL, NULL, &sShortName);
		oNamesSet.Add(sShortName.c_str());
		iListItr++;
	}

	BOOL bBreak = FALSE;

	if(iParentClassId.GetId()==0)   {
		szShortName[0] = L'\0';
	} else   {
		TYPE_DictIndex iGrandParentClassId = GetClassParentId(iParentClassId);
		if(iGrandParentClassId.GetId()==0)   {
			for(UINT i=0; i<500 && !bBreak; i++)   {
				for(UINT j=L'A'; j<L'Z' && !bBreak; j++)   {
					if(i==0)   {
						wsprintf(szShortName, L"%c", j);
					} else   {
						wsprintf(szShortName, L"%c%ld", j ,i);
					}
					if(!oNamesSet.Find(szShortName))   {
						bBreak = TRUE;
					}
				}
			}
		} else   {
			GetClassNameById(iParentClassId, NULL, NULL, &sShortName);
			wcscpy(szShortName,sShortName.c_str());
			pCh = szShortName + wcslen(szShortName);
			for(UINT i=1; i<1000; i++)   {
				wsprintf(pCh, L".%2ld", i);
				if(!oNamesSet.Find(szShortName))   {
					break;
				}
			}
		}
	}

	*psClassShortName = szShortName;

	//m_aTreeFindArr[0] = 
	//m_aTreeFindArr[1] = 
	//m_aTreeFindArr[2] = 
	//m_aTreeFindArr[3] = 
	//m_aTreeFindArr[4] = NULL;

	return(TRUE);
}


TYPE_DictIndex CSemanticClassesTree::CreateClass(IN TYPE_DictIndex iParentClassId, IN std::wstring * psClassName, IN UINT32 iColor /*= DEFAULT_CLASS_COLOR*/, IN std::wstring * psShortName /*= NULL*/)   {
	UINT32 iDictNum           = iParentClassId.GetDictionaryNumber();
	UINT32 iPureParentClassId = iParentClassId.GetId();

	DBMS_CONNECT_SCT;
	if(!SetCurrTbl(iDictNum))   return(0);

	UINT32 iNewPureClassId = m_pCurrTbl->GetUserValue(0);
	if(iNewPureClassId==0)   iNewPureClassId = 1;
	m_pCurrTbl->SetUserValue(0,iNewPureClassId+1);

	if(iPureParentClassId!=0)   {
		m_iParentId = iParentClassId.GetDictionaryIndex();
		if(m_pCurrTbl->FindStart(fenFindInTreeByIdClass)!=S_OK)   {
			LogMess(L"Неверный номер класса родителя : %ld (Словарь: %ld).",iPureParentClassId,iDictNum);	
			return(0); 
		}
		m_pCurrTbl->FindStop(fenFindInTreeByIdParent);
	}

	//TYPE_DictIndex iTstId2 = GetClassIdByName(psClassName, (TYPE_DictNumber)iDictNum);
	//if(iTstId2.GetDictionaryIndex()!=0)   {
	//	LogMess(L"Имя создаваемого класса уже существует.");
	//	return(FALSE); 
	//}

	if(psShortName)   {
		m_sClassShortName = psShortName?*psShortName:L"";
	} else   {
		GetShortName(iParentClassId, &m_sClassShortName);
	}
	m_iClassId    =  iNewPureClassId | (iDictNum << 24);
	m_iParentId   =  iParentClassId.GetDictionaryIndex();
	m_sClassName  = *psClassName;
	m_iClassColor =  iColor; 

	if(m_pCurrTbl->Add()!=S_OK)   {
		LogMess(L"Добавление нового класса <%ld:%ld> не прошло.",iDictNum,iNewPureClassId);	
		return(0); 
	}

	return(m_iClassId);
}

BOOL CSemanticClassesTree::DeleteClass(IN TYPE_DictIndex iClassId)   {

	DBMS_CONNECT_SCT;
	if(!SetCurrTbl(iClassId.GetDictionaryNumber()))   return(FALSE);

	UINT iRecordIdx;
	std::set<UINT>           oTableRecordsToDeleteSet;
	std::set<UINT>::iterator iSetItr;
	std::list<INT>           oIDClassesToCheckList;
	std::list<INT>::iterator iListItr;

	m_iClassId = iClassId.GetDictionaryIndex();
	if(m_pCurrTbl->FindOne(fenFindInTreeByIdClass)!=S_OK)   {
		return(FALSE);
	} else   {
		iRecordIdx = m_pCurrTbl->GetFoundIdx(fenFindInTreeByIdClass);
		oTableRecordsToDeleteSet.insert(iRecordIdx);
	}

	m_aTreeFindArr[0] = &m_iClassId;
	oIDClassesToCheckList.push_back(iClassId.GetDictionaryIndex());
	iListItr = oIDClassesToCheckList.begin();
	while(iListItr != oIDClassesToCheckList.end())   {
		
		m_iParentId  = *iListItr;
		while(m_pCurrTbl->Find(fenFindInTreeByIdParent)==S_OK)   {
			oIDClassesToCheckList.push_back(m_iClassId);
			iRecordIdx = m_pCurrTbl->GetFoundIdx(fenFindInTreeByIdParent);
			oTableRecordsToDeleteSet.insert(iRecordIdx);
		}
		iListItr++;
	}
	m_aTreeFindArr[0] = NULL;

	iSetItr = oTableRecordsToDeleteSet.begin();
	while(iSetItr != oTableRecordsToDeleteSet.end())   {
		m_pCurrTbl->DeleteRecord(*iSetItr);
		iSetItr++;
	}

	return(TRUE);
}

BOOL CSemanticClassesTree::RenameClass(IN TYPE_DictIndex iClassId, IN std::wstring * psNewClassName, IN UINT32 iNewColor /*= DEFAULT_CLASS_COLOR*/, IN std::wstring * psNewShortName /*= NULL*/)   {

	DBMS_CONNECT_SCT;
	if(!SetCurrTbl(iClassId.GetDictionaryNumber()))   return(FALSE);

	UINT iRecordIdx;
	std::set<UINT>           oTableRecordsToDeleteSet;
	std::set<UINT>::iterator iSetItr;
	std::list<INT>           oIDClassesToCheckList;
	std::list<INT>::iterator iListItr;
	UINT32                   iDictNum     = iClassId.GetDictionaryNumber();
	UINT32                   iPureClassId = iClassId.GetId();

	//if(psNewClassName)   {
	//	TYPE_DictIndex iTstId2 = GetClassIdByName(psNewClassName, (TYPE_DictNumber)iDictNum);
	//	if(iTstId2.GetDictionaryIndex()!=0)   {
	//		if(iTstId2!=iClassId)   {
	//			LogMess(L"Имя создаваемого класса уже существует.");
	//			return(FALSE); 
	//		}
	//	}
	//}

	m_iClassId        = iClassId.GetDictionaryIndexWithoutFlags();
	m_aTreeFindArr[1] = &m_iParentId;
	m_aTreeFindArr[2] = &m_sClassName;
	m_aTreeFindArr[3] = &m_sClassShortName;
	m_aTreeFindArr[4] = &m_iClassColor;

	if(m_pCurrTbl->FindOne(fenFindInTreeByIdClass)!=S_OK)   {
		iRecordIdx = -1;
	} else   {
		iRecordIdx = m_pCurrTbl->GetFoundIdx(fenFindInTreeByIdClass);
		m_pCurrTbl->DeleteRecord(iRecordIdx);
	}
	m_aTreeFindArr[1] =
	m_aTreeFindArr[2] =
	m_aTreeFindArr[3] =
	m_aTreeFindArr[4] = NULL;

	if(iRecordIdx == -1)   {
		return(FALSE);
	}


//	m_iClassId   = // same;
//	m_iParentId  = // same;
	if(psNewClassName)   m_sClassName      = *psNewClassName;
	if(psNewShortName)   m_sClassShortName = *psNewShortName;
	                     m_iClassColor     =  iNewColor;

	if(m_pCurrTbl->Add()!=S_OK)   {
		LogMess(L"Изменение имени класса <%ld:%ld> не прошло.",iDictNum,iPureClassId);	
		return(FALSE); 
	}

	return(TRUE);
}

//TYPE_DictIndex CSemanticClassesTree::GetClassIdByName(IN std::wstring * psClassName, IN TYPE_DictNumber eDictNumber)   {
//	DBMS_CONNECT_SCT;
//	if(!SetCurrTbl(eDictNumber))   return(FALSE);
//
//	BOOL bRet;
//	m_aTreeFindArr[0] = &m_iClassId;
//	m_sClassName = *psClassName;
//	if(m_pCurrTbl->FindOne(fenFindInTreeByName)!=S_OK)   {
//		bRet=0;
//	} else   {
//		bRet=m_iClassId;
//	}
//	m_aTreeFindArr[0] = NULL;
//
//	return(bRet);
//}

TYPE_DictIndex CSemanticClassesTree::GetClassIdByShortName(IN std::wstring * psClassShortName, IN TYPE_DictNumber eDictNumber)   {
	DBMS_CONNECT_SCT;
	if(!SetCurrTbl(eDictNumber))   return(FALSE);

	BOOL bRet;
	m_aTreeFindArr[0] = &m_iClassId;
	m_sClassShortName = *psClassShortName;
	if(m_pCurrTbl->FindOne(fenFindInTreeByShortName)!=S_OK)   {
		bRet=0;
	} else   {
		bRet=m_iClassId;
	}
	m_aTreeFindArr[0] = NULL;

	return(bRet);
}

BOOL CSemanticClassesTree::GetClassNameById(IN TYPE_DictIndex iClassId, OUT std::wstring * psClassName, OUT UINT32 * piColor /*= NULL*/, OUT std::wstring * psShortName /*= NULL*/)   {
	DBMS_CONNECT_SCT;
	if(!SetCurrTbl(iClassId.GetDictionaryNumber()))   return(FALSE);

	BOOL bRet;
	m_aTreeFindArr[2] = &m_sClassName;
	m_aTreeFindArr[3] = &m_sClassShortName;
	m_aTreeFindArr[4] = &m_iClassColor;
	m_iClassId        = iClassId.GetDictionaryIndex();

	if(m_pCurrTbl->FindOne(fenFindInTreeByIdClass)!=S_OK)   {
		bRet=FALSE;
	} else   {
		if(psClassName)   *psClassName = m_sClassName;
		if(psShortName)   *psShortName = m_sClassShortName;
		if(piColor)       *piColor     = m_iClassColor;
		bRet=TRUE;
	}
	
	m_aTreeFindArr[2] = NULL;
	m_aTreeFindArr[3] = NULL;
	m_aTreeFindArr[4] = NULL;

	return(bRet);
}

/*
UINT CSemanticClassesTree::GetClassParent(IN TYPE_DictIndex iClassId)   {
	m_aTreeFindArr[1] = &m_iParentId;
	m_iClassId        = iClassId.GetDictionaryIndex();
	if(m_pCurrTbl->FindOne(fenFindInTreeByIdClass)!=S_OK)   {
		m_iParentId = 0;
	}	
	m_aTreeFindArr[1] = NULL;

	return(m_iParentId);
}
*/

BOOL CSemanticClassesTree::GetChildren(IN TYPE_DictIndex iParentClassId, OUT std::list<TYPE_DictIndex> * pChildrenList)   {
	DBMS_CONNECT_SCT;
	if(!SetCurrTbl(iParentClassId.GetDictionaryNumber()))   return(FALSE);

	std::list<INT>           oIDClassesToCheckList;
	std::list<INT>::iterator iListItr;

	pChildrenList->clear();

	UINT iLevelIdx = 0;
	m_aTreeFindArr[0] = &m_iClassId;
	oIDClassesToCheckList.push_back(iParentClassId.GetDictionaryIndex());
	iListItr = oIDClassesToCheckList.begin();
	while(iListItr != oIDClassesToCheckList.end())   {
		
		m_iParentId  = *iListItr;
		while(m_pCurrTbl->Find(fenFindInTreeByIdParent)==S_OK)   {
			oIDClassesToCheckList.push_back(m_iClassId);
			pChildrenList->push_back(m_iClassId);
		}
		iListItr++;

		if(iLevelIdx++==0)   {
			break;
		}

	}
	m_aTreeFindArr[0] = NULL;

	return(pChildrenList->size()!=0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////


////	Запись сообщения об ошибке в журнал системы
//HRESULT LogMess(CHAR * pFuncName, HRESULT nFailCode, _TCHAR * pFormatStr, ...)   {
//
//	if(nFailCode==0)   nFailCode=GetLastError();
//
//	SetLastError(nFailCode);
//
//	if(nFailCode)   {
//
//		_TCHAR  ErrStr[1024];
//		va_list ArgList;
//		int     Len=0;
//
//		va_start(ArgList,pFormatStr);
//			Len =_stprintf( ErrStr,_T("   SemClassWork: "));
//			Len+=_vstprintf(ErrStr+_tcslen(ErrStr),pFormatStr,ArgList);
////			_TCHAR tcsStr[1024];
////			Len+=_stprintf( ErrStr+_tcslen(ErrStr),_T(". Method: %s(), FailCode: 0x%lX."),CHAR2TCHAR(pFuncName,tcsStr),nFailCode);
//			Len+=_stprintf( ErrStr+_tcslen(ErrStr),_T(". FailCode: 0x%lX."),nFailCode);
//			#ifdef _DEBUG
//				_tprintf(_T("%s\n"),ErrStr);
//				Beep(300,100);
//			#endif
////			WCHAR wcsStr[1024];
////			SS_THROW(TCHAR2WCHAR(ErrStr,wcsStr));
//			SS_THROW(ErrStr);
//		va_end(ArgList);
//
//		return(E_FAIL);
//	} else   {
//		return(S_OK);
//	}
//}

//	Запись простого сообщения в журнал системы
VOID LogMess(_TCHAR * pFormatStr, ...)   {
	_TCHAR  MStr[1024];
	va_list ArgList;
	int     Len=0;

	va_start(ArgList,pFormatStr);
		Len =_stprintf( MStr,_T("   SemClassWork: "));
		Len+=_vstprintf(MStr+_tcslen(MStr),pFormatStr,ArgList);
		#ifdef _DEBUG
			_tprintf(_T("%s\n"),MStr);
			Beep(300,100);
		#endif
//		WCHAR wcsStr[1024];
//		SAVE_LOG(SS_MESSAGE AND TCHAR2WCHAR(MStr,wcsStr));
		SAVE_LOG(SS_MESSAGE AND MStr);
	va_end(ArgList);
}
