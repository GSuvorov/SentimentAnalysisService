// ControlDynamicLibrary.cpp: implementation of the CControlDynamicLibrary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ControlDynamicLibrary.h"
#include "tchar.h"
#include "WorkRegisterSingleton.h"
#include "CLoadManager.h"
//#include "exception.h"
#include "..\ASCInterface\defines.h"
#include "..\[libs]\_Paths.h"
#include "atlconv.h"

USES_LOAD_MANAGER;
using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef HRESULT (*CREATE_INSTANCE)(const GUID* pGuid, void** ppBase);
typedef void (*SET_LOAD_MANAGER)(SS::Interface::Core::ResourceManagers::ILoadManager*);
typedef bool (*IS_LOAD_MANAGER_EXIST)(void);


CControlDynamicLibrary::CControlDynamicLibrary()
{
}

CControlDynamicLibrary::~CControlDynamicLibrary()
{
	MNameLibrary_HModule::iterator itCurr=m_mNameLibrary_HModule.begin();
    while(itCurr!=m_mNameLibrary_HModule.end())
	{
		::FreeLibrary(itCurr->second);
		itCurr++;
	}
}

void CControlDynamicLibrary::Init()
{
	if ( !m_oPathMap.IsEmpty() )
	{
		return;
	}

	m_oPathMap.AddPathByID( DICTIONARIES_DLL_PATH, dllDictionary );
	m_oPathMap.AddPathByID(L"./core/linguistics/DBMS.dll", dllDBMS);
	m_oPathMap.AddPathByID(L"./core/linguistics/CoSeDi.dll", dllCoSeDi);
	m_oPathMap.AddPathByID(L"./core/linguistics/SyntaxConstructions.dll", dllSyntaxConstructions);
	m_oPathMap.AddPathByID(L"./core/linguistics/LingvoBaseManager.dll", dllLingvoBaseManager);

	m_oPathMap.AddPathByID(L"./core/linguistics/DataBaseTransformer.dll", dllDataBaseTransformer);
	m_oPathMap.AddPathByID(L"./core/linguistics/CommonServices.dll", dllCommonService);
	//m_oPathMap.AddPathByID(L"./core/ndx.dll", dllNdx);
	//m_oPathMap.AddPathByID(L"./core/linguistics/rev.dll", dllRev);
	//m_oPathMap.AddPathByID(L"./core/AISBroker.dll", dllAISBroker);

	//m_oPathMap.AddPathByID(L"./core/LoadManager_test_dll.dll", dllTest1);
	//m_oPathMap.AddPathByID(L"./core/LoadManager_test_dll2.dll", dllTest2);
	//m_oPathMap.AddPathByID(L"./core/LoadManager.dll", dllLoadManager);
	//m_oPathMap.AddPathByID(L"./core/HtmlParser.dll", dllHtmlParser);
	//m_oPathMap.AddPathByID(L"./CommonServices/QPCounter.dll", dllQPCounter);
	//m_oPathMap.AddPathByID(L"./GSPlugins/GSPluginManager.dll", dllGSPluginManager);

	//m_oPathMap.AddPathByID(L"./core/Statistics/StatisticsCollector.dll", dllStatisticsCollector);
	//m_oPathMap.AddPathByID(L"./core/Statistics/TextStatisticsProcessor.dll", dllTextStatisticsProcessor);

	//m_oPathMap.AddPathByID(L"./core/linguistics/synonims.dll", dllSynonims);
	//m_oPathMap.AddPathByID(L"./core/linguistics/LexicaLib.dll", dllLexicaLib);
	//m_oPathMap.AddPathByID(L"./core/linguistics/AMConverter.dll", dllAMConverter);
	//m_oPathMap.AddPathByID(L"./core/linguistics/SyntaxAnalyzer.dll", dllSyntaxAnalyzer);
	//m_oPathMap.AddPathByID(L"./core/linguistics/BlackBox.dll", dllBlackBox);
	//m_oPathMap.AddPathByID(L"./core/linguistics/MainAnalyse.dll", dllMainAnalyse);
	//m_oPathMap.AddPathByID(L"./core/linguistics/FeatureFactory.dll", dllFeatureFactory);
	//m_oPathMap.AddPathByID(L"./core/linguistics/TransformGenerator.dll", dllTransformGenerator);
	//m_oPathMap.AddPathByID(L"./core/linguistics/LexicalAnalyzer.dll", dllLexicalAnalyzer);
	//m_oPathMap.AddPathByID(L"./core/linguistics/SemanticAnalyzer.dll", dllSemanticAnalyzer);
	//m_oPathMap.AddPathByID(L"./core/linguistics/LinguisticRev.dll", dllLinguisticRev);
	//m_oPathMap.AddPathByID(L"./core/linguistics/CommonContainers.dll", dllCommonContainers);
	//m_oPathMap.AddPathByID(L"./core/ISContainer.dll", dllISContainer);
	//m_oPathMap.AddPathByID(L"./core/NdxQuery.dll", dllNdxQuery);

	m_oPathMap.AddPathByID(L"./core/BasesManager.dll", dllBasesManager);
	m_oPathMap.AddPathByID(L"./core/NdxDataStorage.dll", dllNdxDataStorage);
	m_oPathMap.AddPathByID(L"./core/NdxBaseManager.dll", dllNdxBaseManager);
	m_oPathMap.AddPathByID(L"./core/NdxProcessor.dll", dllNdxProcessor);

	//m_oPathMap.AddPathByID(L"./CommonServices/CommonServices.dll", dllCommonServices);
	//m_oPathMap.AddPathByID(L"./CommonServices/UMCommonServices.dll", dllUMCommonServices);
	//m_oPathMap.AddPathByID(L"./core/GSServer.dll", dllGSServer);
	//m_oPathMap.AddPathByID(L"./CommonServices/UMServerLog.dll", dllServerLog);
}

BOOL CControlDynamicLibrary::FileExists(const wchar_t* wszLibraryPath)
{
  DWORD dwAttrib = GetFileAttributesW(wszLibraryPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
HMODULE CControlDynamicLibrary::GetLibrary(wstring sNameLibrary, const wchar_t* wszPath)
{
	//удаление . и /
	if ( 2 <= sNameLibrary.size() && sNameLibrary[ 0 ] == L'.' )
	{
		sNameLibrary.erase(sNameLibrary.begin());
		sNameLibrary.erase(sNameLibrary.begin());
	};

	if ( m_mNameLibrary_HModule.count(sNameLibrary) )
	{
		return m_mNameLibrary_HModule[sNameLibrary];
	}

	const wchar_t* wszLibraryPath;
	if ( FileExists(sNameLibrary.c_str()) )
	{
		wszLibraryPath = sNameLibrary.c_str();
	}
	else
	{
		SS::Core::CommonServices::CWorkRegister oWorkRegister;
		std::wstring wsTemp = oWorkRegister.GetParameter(L"WorkPath", wszPath);

		wsTemp += sNameLibrary;

		wszLibraryPath = wsTemp.c_str();
	}

	HMODULE hModule = LoadLibrary(wszLibraryPath);

	if( hModule == NULL )
	{
		SAVE_LOG(L"Модуль " AND wszLibraryPath AND L" не загружается, возможно его просто нет");
		//SS_THROW("Модуль " AND wszLibraryPath AND " не загружается, возможно его просто нет");
	};

	m_mNameLibrary_HModule.insert(MNameLibrary_HModule::value_type(wszLibraryPath, hModule));
	
	return hModule;
}

BOOL CControlDynamicLibrary::FreeLibrary(wstring sNameLibrary)
{
	//удаление . и /
	if ( 2 <= sNameLibrary.size() && sNameLibrary[ 0 ] == L'.' )
	{
		sNameLibrary.erase(sNameLibrary.begin());
		sNameLibrary.erase(sNameLibrary.begin());
	}

	MNameLibrary_HModule::iterator itCurr = m_mNameLibrary_HModule.find(sNameLibrary);
	if ( itCurr != m_mNameLibrary_HModule.end() ) {
		::FreeLibrary(itCurr->second);
		m_mNameLibrary_HModule.erase(sNameLibrary);
		return true;
	}
	return false;
}

void CControlDynamicLibrary::SetLog(HMODULE hModule, const wchar_t* wszPath)
{
	/// делаем попытку передать указатель на ILog
	IS_LOAD_MANAGER_EXIST pIsLoadManagerExist = (IS_LOAD_MANAGER_EXIST)GetProcAddress(hModule, "IsLoadManagerExist");

	if(pIsLoadManagerExist)
	{
		if( !(*pIsLoadManagerExist)() )
		{
			SET_LOAD_MANAGER pSetLoadManager = (SET_LOAD_MANAGER)GetProcAddress(hModule, "SetLoadManager");
			if(pSetLoadManager)
			{
				SS::Core::ResourceManagers::CLoadManager* pLoadManager = new SS::Core::ResourceManagers::CLoadManager();
				pLoadManager->SetWorkPath(wszPath);
				(*pSetLoadManager)(static_cast<SS::Interface::Core::ResourceManagers::ILoadManager*>(pLoadManager));
			};
		};
	};

};

///Создает экземпляр требуемого класса и возвращает указатель на его базовый интерфейс
void CControlDynamicLibrary::CreateInstance(const GUID* pGuid, void** ppBase, TCoreLibrary CoreLibrary, const wchar_t* wszPath)
{
	*ppBase = NULL;
	wstring sNameLibrary = m_oPathMap.GetPath(CoreLibrary);
	HMODULE hModule = GetLibrary(sNameLibrary, wszPath);

	CREATE_INSTANCE pCreateInstance = (CREATE_INSTANCE) GetProcAddress( hModule, "CreateInstance" );

	if ( pCreateInstance == NULL ) {
		SAVE_LOG(L"не найдена точка входа в модуль " AND sNameLibrary.c_str() AND L" в функцию CreateInstance");
		//SS_THROW("не найдена точка входа в модуль " AND sNameLibrary.c_str() AND " в функцию CreateInstance");
	}

	(*pCreateInstance)(pGuid, ppBase);
	SetLog(hModule, wszPath);
};

///Создает экземпляр требуемого класса(по имени dll) и возвращает указатель на его базовый интерфейс 
void CControlDynamicLibrary::CreateInstance(const GUID* pGuid, void** ppBase, const wchar_t* wszDllName, const wchar_t* wszPath)
{
	*ppBase = NULL;
	wstring sNameLibrary = wszDllName;
	HMODULE hModule = GetLibrary(sNameLibrary, wszPath);

	CREATE_INSTANCE pCreateInstance = (CREATE_INSTANCE) GetProcAddress( hModule, "CreateInstance" );

	if ( pCreateInstance == NULL ) {		
		SAVE_LOG(L"не найдена точка входа в модуль " AND sNameLibrary.c_str() AND L" в функцию CreateInstance");
		//SS_THROW("не найдена точка входа в модуль " AND sNameLibrary.c_str() AND " в функцию CreateInstance");
	}

	(*pCreateInstance)(pGuid, ppBase);
	SetLog(hModule, wszPath);
};

TCoreLibrary CControlDynamicLibrary::AddLibraryByPath(const wchar_t* wszPath)
{
	Init();
	return (TCoreLibrary) m_oPathMap.AddPath(wszPath);
}
