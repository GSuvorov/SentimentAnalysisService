#include "StdAfx.h"

#include ".\SemanticClassesWords.h"
#include "..\ASSInterface\TBaseNames.h"

// #include <set>

using namespace SS::CoSeDi;
using namespace SS::Interface::Core::DBMS;
using namespace SS::Dictionary::DATA_TYPE::NAMES;
using namespace SS::MANAGER::Constants;

CSemanticClassesWords::CSemanticClassesWords() : zm_iConnectionCnt(0), zm_hMutex(NULL), m_pLingvoBaseManager(NULL), m_pCurrTbl(NULL) {
	m_pDtaBaseR = 
	m_pDtaBaseE = NULL;
	m_pTblWordR =
	m_pTblWordE = NULL;

	m_aWordAddArr[0] = &m_iWordId;
	m_aWordAddArr[1] = &m_iClassId;

	m_aWordFindArr[0] = 
	m_aWordFindArr[1] = NULL;
}

CSemanticClassesWords::~CSemanticClassesWords()   {
	//if(m_pTblWordR)   m_pTblWordR->Close();
	//if(m_pTblWordE)   m_pTblWordE->Close();

	//m_pLingvoBaseManager->UnRegisterDeleteAndClose( c_szCoSeDiRus, &m_pDtaBaseR);
	//m_pLingvoBaseManager->UnRegisterDeleteAndClose( c_szCoSeDiEng, &m_pDtaBaseE);

	zm_iConnectionCnt=1;
	zm_fDbmsDisConnect();

	if(zm_hMutex)     { ReleaseMutex(zm_hMutex); CloseHandle(zm_hMutex); zm_hMutex=NULL; };
}

void CSemanticClassesWords::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)   {
	SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);

	m_pLingvoBaseManager = 
		(SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)
		GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);

};

BOOL CSemanticClassesWords::SetCurrTbl(UINT iDictNum)   {

	if(zm_iConnectionCnt)   {

		switch(iDictNum)   {
			case ednRNominalName:
			case ednRProperName:
			case ednAbbreviationsRus:
			case ednRCombinatory:
			case ednRSemanticClass:
			case ednSynonimsRus:
			case ednUnknownRus:
				m_pCurrTbl = m_pTblWordR;
				return(TRUE);
	//		break;

			case ednENominalName:
			case ednEProperName:
			case ednAbbreviationsEng:
			case ednECombinatory:
			case ednESemanticClass:
			case ednSynonimsEng:
			case ednUnknownEng:
				m_pCurrTbl = m_pTblWordE;
				return(TRUE);
	//		break;
		}

		m_pCurrTbl = NULL;
		LogMess(L"Неверный номер словаря : %ld",iDictNum);	
		return(FALSE); 

	} else   {
		LogMess(L"Таблицы DBMS не подключены.");	
		return(FALSE); 
	}
}

BOOL CSemanticClassesWords::zm_fDbmsConnect(VOID)   {

	if(zm_iConnectionCnt++==0)   {

		if(zm_hMutex==NULL)   zm_hMutex = CreateMutex(NULL, FALSE, L"CSemanticClassesWords_MUTEX");
		if(zm_hMutex)   WaitForSingleObject(zm_hMutex,1000); // 1 sec

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

		if(m_pDtaBaseR->MoveToDataTable(L"SemanticClassesWords")!=S_OK)   {
			LogMess(L"Интерфейс таблицы <CoSeDiRussian::SemanticClassesWords> не получен.");	
			return(FALSE); 
		}

		if(m_pDtaBaseE->MoveToDataTable(L"SemanticClassesWords")!=S_OK)   {
			LogMess(L"Интерфейс таблицы <CoSeDiEnglish::SemanticClassesWords> не получен.");	
			return(FALSE); 
		}

		// ---

		if(m_pDtaBaseR->MoveToDataTable(L"SemanticClassesWords")!=S_OK)   {
			LogMess(L"Таблица <CoSeDiRussian::SemanticClassesWords> не найдена.");	
			return(FALSE); 
		}
		if((m_pTblWordR = m_pDtaBaseR->OpenDataTable())==NULL)   {
			LogMess(L"Интерфейс таблицы <CoSeDiRussian::SemanticClassesWords> не получен.");	
			return(FALSE); 
		}
		if(m_pTblWordR->AddInit(m_aWordAddArr)!=S_OK)   {
			LogMess(L"Инициализация добавления для <CoSeDiRussian::SemanticClassesWords> не прошла.");	
			return(FALSE); 
		}
		if(m_pTblWordR->FindInit(L"IDSource",&m_iWordId,m_aWordFindArr,fenFindInTreeByIdWord)!=S_OK)   {
			LogMess(L"Инициализация поиска для <CoSeDiRussian::SemanticClassesWords> по <IDParent> не прошла.");	
			return(FALSE); 
		}
		if(m_pTblWordR->FindInit(L"IDClass",&m_iClassId,m_aWordFindArr,fenFindInTreeByIdClass)!=S_OK)   {
			LogMess(L"Инициализация поиска для <CoSeDiRussian::SemanticClassesWords> по <IDClass> не прошла.");	
			return(FALSE); 
		}

		// ---

		if(m_pDtaBaseE->MoveToDataTable(L"SemanticClassesWords")!=S_OK)   {
			LogMess(L"Таблица <CoSeDiEnglish::SemanticClassesWords> не найдена.");	
			return(FALSE); 
		}
		if((m_pTblWordE = m_pDtaBaseE->OpenDataTable())==NULL)   {
			LogMess(L"Интерфейс таблицы <CoSeDiEnglish::SemanticClassesWords> не получен.");	
			return(FALSE); 
		}
		if(m_pTblWordE->AddInit(m_aWordAddArr)!=S_OK)   {
			LogMess(L"Инициализация добавления для <CoSeDiEnglish::SemanticClassesWords> не прошла.");	
			return(FALSE); 
		}
		if(m_pTblWordE->FindInit(L"IDSource",&m_iWordId,m_aWordFindArr,fenFindInTreeByIdWord)!=S_OK)   {
			LogMess(L"Инициализация поиска для <CoSeDiEnglish::SemanticClassesWords> по <IDParent> не прошла.");	
			return(FALSE); 
		}
		if(m_pTblWordE->FindInit(L"IDClass",&m_iClassId,m_aWordFindArr,fenFindInTreeByIdClass)!=S_OK)   {
			LogMess(L"Инициализация поиска для <CoSeDiEnglish::SemanticClassesWords> по <IDClass> не прошла.");	
			return(FALSE); 
		}
	}
	return(TRUE);
}

BOOL CSemanticClassesWords::zm_fDbmsDisConnect(VOID)   {

	if(zm_iConnectionCnt==1)   {
		zm_iConnectionCnt =0;

		if(m_pTblWordR)   { m_pTblWordR->Close(); m_pTblWordR=NULL; }
		if(m_pTblWordE)   { m_pTblWordE->Close(); m_pTblWordE=NULL; }

		if(m_pDtaBaseR)   { m_pLingvoBaseManager->UnRegisterDeleteAndClose(c_szCoSeDiRus, &m_pDtaBaseR); m_pDtaBaseR=NULL; }
		if(m_pDtaBaseE)   { m_pLingvoBaseManager->UnRegisterDeleteAndClose(c_szCoSeDiEng, &m_pDtaBaseE); m_pDtaBaseE=NULL; }

		if(zm_hMutex)     { ReleaseMutex(zm_hMutex); };

	} else   {
		zm_iConnectionCnt--;
	}
	return(TRUE);
}

BOOL CSemanticClassesWords::GetClasses(IN TYPE_DictIndex iIDSource, OUT std::list<TYPE_DictIndex> * pClassesList)   {
	DBMS_CONNECT_SCW;
	if(!SetCurrTbl(iIDSource.GetDictionaryNumber()))   return(FALSE);

	pClassesList->clear();

	m_aWordFindArr[1] = &m_iClassId;
	m_iWordId = iIDSource.GetDictionaryIndex();
	while(m_pCurrTbl->Find(fenFindInTreeByIdWord)==S_OK)   {
		pClassesList->push_back(m_iClassId);
	}
	m_aWordFindArr[1] = NULL;

	return(pClassesList->size()!=0);
}

BOOL CSemanticClassesWords::GetWords(OUT std::list<TYPE_DictIndex> * pIDSourcesList)   {
	DBMS_CONNECT_SCW;
	if(!SetCurrTbl(m_iCurSemClass.GetDictionaryNumber()))   return(FALSE);

	pIDSourcesList->clear();
	m_aWordFindArr[0] = &m_iWordId;
	m_iClassId = m_iCurSemClass.GetDictionaryIndexWithoutFlags();
	while(m_pCurrTbl->Find(fenFindInTreeByIdClass)==S_OK)   {
		pIDSourcesList->push_back(m_iWordId);
	}
	m_aWordFindArr[0] = NULL;

	return(pIDSourcesList->size()!=0);
}

BOOL CSemanticClassesWords::AddWord(IN TYPE_DictIndex iIDSource)   {
	DBMS_CONNECT_SCW;
	if(!SetCurrTbl(iIDSource.GetDictionaryNumber()))   return(FALSE);

	BOOL bFound = FALSE;
	m_aWordFindArr[1] = &m_iClassId;
	m_iWordId = iIDSource.GetDictionaryIndexWithoutFlags();
	while(m_pCurrTbl->Find(fenFindInTreeByIdWord)==S_OK)   {
		if(m_iClassId==m_iCurSemClass.GetDictionaryIndex())   {
			bFound = TRUE;
			m_pCurrTbl->FindStop(fenFindInTreeByIdWord);
			break;
		}
	}
	m_aWordFindArr[1] = NULL;

	if(!bFound)   {
		m_iWordId  = iIDSource.GetDictionaryIndexWithoutFlags();
		m_iClassId = m_iCurSemClass.GetDictionaryIndex();
		return(m_pCurrTbl->Add()==S_OK);
	} else   {
		return(FALSE);
	}
}

BOOL CSemanticClassesWords::DeleteWord(IN TYPE_DictIndex iIDSource)   {
	DBMS_CONNECT_SCW;
	if(!SetCurrTbl(iIDSource.GetDictionaryNumber()))   return(FALSE);

	BOOL bRet = FALSE;
	m_aWordFindArr[1] = &m_iClassId;
	m_iWordId = iIDSource.GetDictionaryIndex();
	while(m_pCurrTbl->Find(fenFindInTreeByIdWord)==S_OK)   {
		if(m_iClassId==m_iCurSemClass.GetDictionaryIndex())   {
			m_pCurrTbl->DeleteRecord(m_pCurrTbl->GetFoundIdx());
			bRet = TRUE;
		}
	}
	m_aWordFindArr[1] = NULL;

	return(bRet);
}


