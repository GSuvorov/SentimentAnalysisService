#include "stdafx.h"
#include "../ASCInterface/defines.h"
#include "../ASCInterface/ICommonContainers.h"
#include "./Block.h"
#include "./Variant.h"
#include "./Sequence.h"
#include "./Word.h"
#include "./SuperTemplate.h"

#ifdef _SS_UNITTESTS
#include "./tst_ParametrizedQuery.h"
#include "./testsettings.h"


CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxQuery::CParametrizedQuery_test);

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxQuery
{
using SS::Core::NdxSE::NdxQuery::ToConsole;
void SS::UnitTests::NdxSE::NdxQuery::CParametrizedQuery_test::MainTest()
{
	ToConsole(L"\n.\n--- nCParametrizedQuery_test::MainTest() ---\n.\n");
	//	считывание рабочей папки
	tsett::CSettings	settings;
	std::wstring		sWorkingDir;
	if (settings.SettingRead(sWorkingDir, tsett::wcszKey_PathWork))
	{
		ToConsole(L"Working Path = ");
		ToConsole(sWorkingDir.c_str());
	}
	else
	{
		ToConsole(L"reading working path failed!");
		return;
	}

	//	путь к LoadManager
	std::wstring	sLoadManagerPath = 
		sWorkingDir+std::wstring(L"\\Core\\LoadManager.dll");

	//	считывание пути к БД
	std::wstring		sDatabasePath;
	if (settings.SettingRead(sDatabasePath, tsett::wcszKey_PathDatabase))
	{
		sDatabasePath += std::wstring(tsett::wszSlash);
		ToConsole(L"\ndatabase path = ");
		ToConsole(sDatabasePath.c_str());
	}
	else
	{
		ToConsole(L"reading database path failed!");
		return;
	}
	
	//	считывание названия папки в БД
	std::wstring		sDatabaseName;
	if (settings.SettingRead(sDatabaseName, tsett::wcszKey_NameInd))
	{
		ToConsole(L"Ind folder name = ");
		ToConsole(sDatabaseName.c_str());
	}
	else
	{
		ToConsole(L"reading Ind folder name failed!");
		return;
	}
	
	//	считываем имя файла запросов
	std::wstring	QueriesFileName;
	if (settings.SettingRead(QueriesFileName, tsett::wcszKey_TxtQueries))
	{
		QueriesFileName = settings.GetExeDir() + std::wstring(tsett::wszSlash) + QueriesFileName;
		ToConsole(L"\nqueries full file name = \n");
		ToConsole(QueriesFileName.c_str());
	}
	else
	{
		ToConsole(L"reading queries file name failed!");
		return;
	}

	//	чтение файла с запросами
	VectorQueries	Queries;
	GetQueries(QueriesFileName, Queries);

	////	загружаем менеджер
	HMODULE	hLoadManager = NULL;
	hLoadManager = ::LoadLibraryW(sLoadManagerPath.c_str());
	if (hLoadManager == NULL) return;
	typedef HRESULT (*CREATE_INSTANCE)(const GUID* pGuid, void** ppBase);
	CREATE_INSTANCE pCreateInstance=(CREATE_INSTANCE)GetProcAddress(hLoadManager, "CreateInstance");
	if(pCreateInstance == NULL) return;
	const GUID Guid = CLSID_LoadManager;
	SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager = NULL;
	(*pCreateInstance)(&Guid, (void**)&pLoadManager);
	if (pLoadManager == NULL) return;
	pLoadManager->SetWorkPath(sWorkingDir.c_str());

	//	создание лингвопроцессора
	m_pLinguisticProcessor = (SS::Interface::Core::ILinguisticProcessor*)(pLoadManager->GetInterface(
		L"./core/linguistics/MainAnalyse.dll",
		CLSID_LinguisticProcessor,
		IID_LinguisticProcessor)); 
	if (m_pLinguisticProcessor == NULL)
	{
		ToConsole(L"ERROR:, pLinguisticProcessor is NULL");
		return;
	}

	//Создание экземпляра поискового запроса
	m_pQuery = (SS::Interface::Core::CommonContainers::IQueryResult*)(pLoadManager->GetInterface(
		L"./core/linguistics/CommonContainers.dll", 
		CLSID_QueryResult, 
		IID_QueryResult));
	if (m_pQuery == NULL)
	{
		ToConsole(L"ERROR:, pQuery is NULL");
		return;
	}

	//	Создание планировщика запросов
	m_pPrmQuery = (SS::Interface::Core::NdxSE::IParametrizedQuery*)(pLoadManager->GetInterface(
		L"./core/NdxQuery.dll",
		CLSID_ParametrizedQuery,
		IID_ParametrizedQuery)); 
	if (m_pPrmQuery == NULL)
	{
		ToConsole(L"ERROR: m_pPrmQuery is NULL");
		return;
	}

	//	Открываем базу новых слов
	m_pBaseManager = (SS::Interface::Core::ResourceManagers::IBaseManager*)(pLoadManager->GetInterface(
		L"./core/linguistics/LingvoBaseManager.dll",
		CLSID_LinguoBaseManager,
		IID_BaseManager));
	if (m_pPrmQuery == NULL)
	{
		ToConsole(L"ERROR: m_pBaseManager is NULL");
		return;
	}

	//	открытие базы данных
	m_pBaseManager->SetBasePath(sDatabasePath.c_str());
	m_pBaseManager->OpenBase(sDatabaseName.c_str());

	//	установка параметров БД
	SS::Core::Types::IndexationSearch::TQueryParams		oQueryParams;
	oQueryParams.m_GenerateQueryMode = SS::Core::Types::IndexationSearch::gqmIndex;
	//
	SS::Core::Types::IndexationSearch::TSearchParams		oSearchParams(1000);
	//	тест поиска по OR
	//oSearchParams.m_bForceOrSearch = true;
	//
	SS::Core::Types::IndexationSearch::TLinguisticProcessorMode	oMode;
	oMode.GetAnalyseDepthParams()->SetIsMorpho(true);
	//	новые
	oMode.GetAnalyseDepthParams()->SetIsSemantic(true);
	oMode.GetAnalyseDepthParams()->SetIsSyntax(true);
	oMode.GetAnalyseDepthParams()->SetIsSynonims(false);
	//
	m_pLinguisticProcessor->SetMode(&oMode);
	//получение логического запросов из человеческих
	for(unsigned int i = 0; i< Queries.size(); i++)
	{
		{
			const unsigned int buffer_size = 256;
			wchar_t	buf[buffer_size];
			unsigned int counter = 0;

			counter += _snwprintf(
				buf+counter,
				buffer_size-counter,
				L"\n.\n.\n---  ЗАПРОС ---\n%s\n.\n.\n.",
				Queries[i].c_str());

			ToConsole(buf);
		}
		oQueryParams.SetQuery(Queries[i].c_str());
		m_pLinguisticProcessor->GetLinguisticQuery()->GenerateQuery(&oSearchParams, &oQueryParams, m_pQuery, true);
		m_pPrmQuery->Init(m_pQuery, &oSearchParams);
	}

	m_pBaseManager->CloseBase();
}

void CParametrizedQuery_test::GetQueries(wstring& _wsFilePath, VectorQueries& _Queries)
{
	FILE* pFileStream=_wfopen(_wsFilePath.c_str(), L"r");
	if(pFileStream == NULL)
	{
		ToConsole(L"файл запросов не открылся, его имя:");
		ToConsole(_wsFilePath.c_str());
		return;
	}

	char szLine[10000];
	wchar_t wszLine[10000];
	while(!feof(pFileStream)){
		szLine[0]=0;
		wszLine[0]=0;

		if(!fgets(szLine, 10000, pFileStream))
			break;

		szLine[strlen(szLine)-1]=0;
		MultiByteToWideChar(CP_ACP, 0, szLine, -1, wszLine, 10000); 
		wszLine[strlen(szLine)]=0;
		std::wstring	temp(wszLine);
		if (!temp.empty()) _Queries.push_back(temp);
	}

	fclose(pFileStream);
}

}
}
}
}

#endif	//	_SS_UNITTESTS