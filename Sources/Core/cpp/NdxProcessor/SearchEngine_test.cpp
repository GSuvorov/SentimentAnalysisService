#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\searchengine_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include "../ASCInterface/TCoommonTypes.h"
#include "../ASCInterface/ILinguisticProcessor.h"
#include ".\logged_index_container.h"
#include ".\prx_pos_containers.h"
#include ".\testsettings.h"
#include <crtdbg.h>

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxProcessor::Indexing::CSearchEngine_test );

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Indexing
{

using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Interface::Core::NdxSE;
using namespace SS::Interface::Core::CommonContainers;
using namespace SS::Interface::Core::DBMS;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;

//--------------------------------------------------------------------//

void CSearchEngine_test::TestIndexesCoordinates(void)
{	
	TestAll();

	wstring wsLogFilePath;
	wstring wsBasePath;
	wstring wsBaseName;
	
	wsLogFilePath=L"./log_icoord_TEXT.txt";

	wsBasePath=L"d:\\SS\\UnitTestsBin\\Core\\NdxSE\\Ut\\";
	//wsBasePath=L"d:\\SSPE\\Local Search Personal\\Database\\";
	//wsBasePath=L"d:\\IndexBasesBackup\\XIE\\";
	
	//wsBaseName=L"IndNSNZZWNMXOAMNO0X";
	//wsBaseName=L"IndXONKAO7IOMAWNO0X";
	wsBaseName=L"test_se_base";

	CLoggedIndexContainer LoggedIndexContainer;
	
	ToConsole(L"//-------------LoggedIndexContainer.Init");
	LoggedIndexContainer.Init(&wsLogFilePath);
	ToConsole(L"//-------------Eof-LoggedIndexContainer.Init");
	
	//получаем менеджер индексной базы
	INdxBaseManager* pNdxBaseManager=(INdxBaseManager*)GetLoadManager()->
	GetInterface(L"./core/NdxBaseManager.dll", CLSID_NdxBaseManager, IID_NdxBaseManager);

	//устанавливаем путь к базе
	pNdxBaseManager->SetBasePath(wsBasePath.c_str());
	//открываем базу
	pNdxBaseManager->OpenBase(wsBaseName.c_str());

	//создаем процессор индексной базы
	CNdxSearchEngine* pNdxSearchEngine=new CNdxSearchEngine;
	pNdxSearchEngine->SetLoadManager(GetLoadManager());

	CWord::m_pLoggedIndexContainer=&LoggedIndexContainer;

	wchar_t buf[20];
	unsigned int uiCurrentIndex;
	if(LoggedIndexContainer.MoveFirstIndex()){
		do{
			//получаем индекс
			uiCurrentIndex=LoggedIndexContainer.GetCurrentIndex();
			//ToConsole(L"uiCurrentIndex", uiCurrentIndex);
			//uiCurrentIndex=17410;

			//формируем строку запроса
			wstring wszQuery(L"(((((");
			wszQuery.append(_itow(uiCurrentIndex, buf, 10));
			wszQuery.append(L" <-0-0-0> )))))");
			
			//выполняем поиск
			SetCurrentQuery(wszQuery.c_str());
			TestExtracting(pNdxSearchEngine);
			//return;
		}while(LoggedIndexContainer.MoveNextIndex());
	}

	
	delete pNdxSearchEngine;

	//закрываем базу
	pNdxBaseManager->CloseBase();
}

void CSearchEngine_test::TestAll(void)
{
	UT_STOP_LOGING
	
	//получаем менеджер индексной базы
	INdxBaseManager* pNdxBaseManager=(INdxBaseManager*)GetLoadManager()->
	GetInterface(L"./core/NdxBaseManager.dll", CLSID_NdxBaseManager, IID_NdxBaseManager);

	//устанавливаем путь к базе
	pNdxBaseManager->SetBasePath(GetUTWorkingPath());
	//открываем базу
	pNdxBaseManager->OpenBase(L"test_se_base");
	//очищаем базу для индексации
	pNdxBaseManager->ClearBase();

	//создаем процессор индексной базы
	CNdxSearchEngine* pNdxSearchEngine=new CNdxSearchEngine;
	pNdxSearchEngine->SetLoadManager(GetLoadManager());

	UT_START_LOGING

	//тестируем индексацию
	//for(int i=0; i<10; i++)
		TestIndexing(pNdxSearchEngine, 1);
	
	//тестируем извлечение
	//TestExtracting(pNdxSearchEngine);

	delete pNdxSearchEngine;

	//закрываем базу
	pNdxBaseManager->CloseBase();

	//Test();

	UT_START_LOGING
}

void CSearchEngine_test::Test(void)
{
	UT_START_LOGING
	ToConsole(L"//-----Bof SearchEngine extracting test");

	wstring			wsBasePath;
	wstring			wsBaseName;
	wstring			wsFilePath;
	vector<wstring>	Queries;

	//wsBasePath=L"d:\\SS\\UnitTestsBin\\Core\\NdxSE\\Ut\\";
	wsBasePath=L"d:\\IndexBasesBackup\\Trec\\";
	//wsBasePath=L"d:\\IndexBasesBackup\\Demotool_2.3.29\\";
	//wsBasePath=L"d:\\IndexBasesBackup\\err_1475\\";
	//wsBasePath=L"d:\\SS\\Database\\";
	
	//wsBaseName=L"Ind0VAIAZXIOWAWNO0X";
	//wsBaseName=L"Ind7EXEXSNYXVAVNO0X";
	wsBaseName=L"IndXS007K7XOEAVNO0X";
	//wsBaseName=L"test_se_base";

	Queries.push_back(
		L"(((((83905049 <-0-0-0-1> | 83905050 <-0-0-0-1> ))& ((83918330 <-1-0-0-1> | 83918331 <-1-0-0-1> | 83918332 <-1-0-0-1> | 100663397 <-1-0-0-1> | 100688623 <-1-0-0-1> )))))"
	);

	for(int i=0; i<1; i++){
		TestSearching(
			wsBasePath.c_str(), 		
			wsBaseName.c_str(), 
			Queries
			);

		ToConsole(L"SearchEngine extracting complete");
		ViewMemoryStatus();
	}

	ToConsole(L"//-----Eof SearchEngine extracting test");
	
	UT_STOP_LOGING
}

void CSearchEngine_test::TestNdxQuery(void)
{
	//	считываем из файла настроек теста пути и имена
	if (!ReadTestingPaths()) return;
	//	считываем запросы из файла запросов
	vector<wstring>		Queries;
	GetQueries(m_wsQueryFile, Queries);
	//	проверка на нулевое число запросов
	if (Queries.size() == 0)
	{
		ToConsole(L"\n.\n.\nERROR!!! No questions in file!!!\n.\n.\n.");
		return;
	}

	//	получаем лингвистический процессор
	SS::Interface::Core::ILinguisticProcessor* pLinguisticProcessor = NULL;
	pLinguisticProcessor = (SS::Interface::Core::ILinguisticProcessor*)
		GetLoadManager()->GetInterface(
			L"./core/linguistics/MainAnalyse.dll",
			CLSID_LinguisticProcessor,
			IID_LinguisticProcessor); 

	//	получаем менеджер БД
	SS::Interface::Core::ResourceManagers::IBaseManager* pBaseManager = NULL;
	pBaseManager = (SS::Interface::Core::ResourceManagers::IBaseManager*)
		GetLoadManager()->GetInterface(
			L"./core/linguistics/LingvoBaseManager.dll",
			CLSID_LinguoBaseManager,
			IID_BaseManager);

	//	устанавливаем путь базе и открываем её
	pBaseManager->SetBasePath(m_wsBasePath.c_str());
	pBaseManager->OpenBase(m_wsBaseName.c_str());

	//получаем менеджер индексной базы
	INdxBaseManager* pNdxBaseManager=(INdxBaseManager*)GetLoadManager()->
	GetInterface(L"./core/NdxBaseManager.dll", CLSID_NdxBaseManager, IID_NdxBaseManager);
	//устанавливаем путь к базе
	pNdxBaseManager->SetBasePath(m_wsBasePath.c_str());
	//открываем базу
	pNdxBaseManager->OpenBase(m_wsBaseName.c_str());

	//	создаем процессор индексной базы
	std::auto_ptr<CNdxSearchEngine> apNdxSearchEngine(new CNdxSearchEngine());
	apNdxSearchEngine->SetLoadManager(GetLoadManager());
	//	устанавливаем параметры поиска
	SS::Interface::Core::NdxSE::CNdxSearchSettings SearchSettings;
	//SearchSettings.m_SearchSettings.m_StorageUsingMode=CNdxSearchSettings::SSearchSettings::esumAll;
	//SearchSettings.m_SearchSettings.m_StorageUsingMode=CNdxSearchSettings::SSearchSettings::esumHeadersAndTablesStorage;
	//SearchSettings.m_SearchSettings.m_StorageUsingMode=CNdxSearchSettings::SSearchSettings::esumHeadersAndTextStorage;
	SearchSettings.m_SearchSettings.m_EvalRelevanceMode = 
		CNdxSearchSettings::SSearchSettings::erWordProximityWithStatistic;
	SearchSettings.m_SearchSettings.m_StorageUsingMode =
		CNdxSearchSettings::SSearchSettings::esumSingleStorage;

	//	устанавливаем параметры запроса
	SS::Core::Types::IndexationSearch::TQueryParams		oQueryParams;
	oQueryParams.m_GenerateQueryMode = SS::Core::Types::IndexationSearch::gqmIndex;

	//	устанавливаем параметры поиска
	SS::Core::Types::IndexationSearch::TSearchParams		oSearchParams(1000);

	//	устанавливаем режим лингвистического процессора
	SS::Core::Types::IndexationSearch::TLinguisticProcessorMode oMode;			
	oMode.GetAnalyseDepthParams()->SetIsMorpho(true);
	oMode.GetAnalyseDepthParams()->SetIsSemantic(false);
	oMode.GetAnalyseDepthParams()->SetIsSyntax(false);
	oMode.GetAnalyseDepthParams()->SetIsSynonims(false);
	pLinguisticProcessor->SetMode(&oMode);

	FILE* pFileStream=_wfopen(L"./ndx_time.txt", L"wt");
	if(ISNULL(pFileStream)) return;

	//	обработка запросов из файла
	for(unsigned int i = 0; i < Queries.size(); ++i)
	{
		{	const unsigned int buffer_size = 256;
			wchar_t	buf[buffer_size];
			unsigned int counter = 0;
			counter += _snwprintf(buf+counter, buffer_size-counter, L"\n.\n.\nQUERY FROM FILE: %s\n.\n.\n.", Queries[i].c_str());
			ToConsole(buf);		}

		//	устанавливаем запрос
		oQueryParams.SetQuery(Queries[i].c_str());
		//	получаем запрос в логическом виде
		pLinguisticProcessor->GetLinguisticQuery()->GenerateQuery(&oSearchParams, &oQueryParams, GetQueryResult(), true);

		//	делаем поиск
		START_TICK(L"TestExtracting");
		apNdxSearchEngine->StartFindSession(GetISContainer());
		GetPrmQueryResult()->Init(GetQueryResult(), &oSearchParams);	//	преобразуем (оптимизируем) запрос
		apNdxSearchEngine->Find(/*L"TABLES"*/L"TEXT", GetPrmQueryResult(), &oSearchParams, &SearchSettings);
		//pSearchEngine->Find(GetNdxMetaFieldCollection(), &SearchParams, &SearchSettings);
		apNdxSearchEngine->EndFindSession();
		STOP_TICK(L"TestExtracting");

		fwprintf(pFileStream, L"%u.\tFull: %f\tSearch: %f\tRead:%f\tQry: %s \n", 
			i, TickManager.GetTicks(L"TestExtracting"), TickManager.GetTicks(L"FullSearch"), 
			TickManager.GetTicks(L"ReadNode"), Queries[i].c_str());
		VIEW_TICKS();
		fflush(pFileStream);
	}
	fclose(pFileStream);

	apNdxSearchEngine.reset();
	pNdxBaseManager->CloseBase();

	pBaseManager->CloseBase();
}

void CSearchEngine_test::TestMultiQueries(void)
{
	ToConsole(L"//-----Bof SearchEngine multi extracting test");
////////////////////////////////////////////////////////////////////////////////
	ReadTestingPaths();

	vector<wstring>					Queries;
	vector<wstring>					LogicQueries;
	//получаем запросы из файла
	m_wsQueryFile=L"d:\\SS\\UnitTestsBin\\Core\\NdxSE\\lqry.txt";
	GetQueries(m_wsQueryFile, Queries);
	//приводим их в логический вид
	//QueryToLogicView(m_wsBasePath, m_wsBaseName, Queries, LogicQueries);
	//Queries.clear();

	//FILE* pFileStream=_wfopen(L"./lqry.txt", L"wt");
	//if(ISNULL(pFileStream)) return;
	//for(unsigned int i=0; i<LogicQueries.size(); i++){
	//	fwprintf(pFileStream, L"%s\n", LogicQueries[i].c_str());
	//}
	//fclose(pFileStream);
	//return;

	//ToConsole(L"//---finish generation start searching...");

	//выполняем поиск по всем запросам
	TestSearching(
		m_wsBasePath.c_str(), 		
		m_wsBaseName.c_str(), 
		Queries/*LogicQueries*/,
		/*&Queries*/NULL);

	ToConsole(L"//-----Eof SearchEngine multi extracting test");
}

void CSearchEngine_test::TestIndexes(void)
{
	UT_START_LOGING
	GetLoadManager();
	vector<unsigned int> IndexCollection;
	wstring wsResultWords;
	
	SetCurrentQuery(
		//L"(((((100678654 <-0-0-0> ))& ((100695849 <-1-0-0> | 100680353 <-1-0-0> | 100674894 <-1-0-0> | 100670777 <-1-0-0> | 83955485 <-1-0-0> | 83955484 <-1-0-0> | 83955483 <-1-0-0> )))& (((83893226 <-2-0-0> | 83893225 <-2-0-0> )))& (((83939805 <-3-1-0> )))& (((83892098 <-4-2-0> | 83892097 <-4-2-0> ))))| ((((83893226 <-0-0-1> | 83893225 <-0-0-1> )))& (((100678654 <-1-0-1> ))& ((100695849 <-2-0-1> | 100680353 <-2-0-1> | 100674894 <-2-0-1> | 100670777 <-2-0-1> | 83955485 <-2-0-1> | 83955484 <-2-0-1> | 83955483 <-2-0-1> )))& (((83939805 <-3-1-1> )))& (((83892098 <-4-2-1> | 83892097 <-4-2-1> )))))"
		//L"(((((100665946 <-0-0-0> | 83915808 <-0-0-0> | 83915807 <-0-0-0> | 83915806 <-0-0-0> )))& (((83932211 <-1-1-0> | 83932210 <-1-1-0> )))& (((83897397 <-2-2-0> | 83897396 <-2-2-0> )))))"
		//L"(((((83896626 <-0-0-0> | 83896618 <-0-0-0> | 83896617 <-0-0-0> )))& ((((100689101 <-1-0-0> | 83941298 <-1-0-0> | 83916652 <-1-0-0> | 83916651 <-1-0-0> | 67108913 <-1-0-0> ))& ((100693394 <-2-0-0> | 100676060 <-2-0-0> | 100673886 <-2-0-0> ))))))"
		L"(((((83893226 <-0-0-0> | 83893225 <-0-0-0> )))& (((83941299 <-1-1-0> | 83941298 <-1-1-0> | 67108913 <-1-1-0> ))))| (((((83893226 <-0-0-1> | 83893225 <-0-0-1> ))))& (((83941299 <-1-1-1> | 83941298 <-1-1-1> | 67108913 <-1-1-1> )))))"
		//L"(((((100679486 <-0-0-0> | 83929739 <-0-0-0> )))& (((100670021 <-1-1-0> | 83906922 <-1-1-0> | 83906921 <-1-1-0> | 83906920 <-1-1-0> | 83907792 <-1-1-0> )))& (((83945135 <-2-2-0> | 83945134 <-2-2-0> )))& (((83910750 <-3-3-0> )))& (((83932066 <-4-4-0> | 83932065 <-4-4-0> )))))"

		);

	FillQueryResult();
	IQueryIndex* pQueryIndex=GetQueryResult()->GetFirstQueryIndex();
	while(pQueryIndex){
		IndexCollection.push_back(pQueryIndex->GetQueryIndex());
		pQueryIndex=GetQueryResult()->GetNextQueryIndex();
	}

	GetWordFromDictionary(&IndexCollection, &wsResultWords);
	ToConsole(wsResultWords.c_str());

	UT_STOP_LOGING
}

void CSearchEngine_test::TestTextInfoExtracting(const wchar_t* wszIndexBasePath, 
									   const wchar_t* wszIndexBaseName, unsigned int uiTextIndex)
{
	//получаем менеджер индексной базы
	INdxBaseManager* pNdxBaseManager=(INdxBaseManager*)GetLoadManager()->
	GetInterface(L"./core/NdxBaseManager.dll", CLSID_NdxBaseManager, IID_NdxBaseManager);

	//устанавливаем путь к базе
	pNdxBaseManager->SetBasePath(wszIndexBasePath);
	//открываем базу
	pNdxBaseManager->OpenBase(wszIndexBaseName);

	//создаем процессор индексной базы
	CNdxSearchEngine* pNdxSearchEngine=new CNdxSearchEngine;
	pNdxSearchEngine->SetLoadManager(GetLoadManager());
	
	//SS::Interface::Core::CommonContainers::ITextFeature* pTextFeature=
	//(SS::Interface::Core::CommonContainers::ITextFeature*)
	//	GetLoadManager()->GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_TextFeature, IID_TextFeature);
	//
	//pNdxSearchEngine->GetTextInfo(uiTextIndex, pTextFeature);

	STextPathChunk TextPathChunk(NULL, 0);
	TextPathChunk.m_uiMyOffset=102;
	pNdxSearchEngine->DeleteTextPathChunk(&TextPathChunk);

	delete pNdxSearchEngine;

	//закрываем базу
	pNdxBaseManager->CloseBase();
}

void CSearchEngine_test::TestSearching(const wchar_t* wszIndexBasePath, 
									   const wchar_t* wszIndexBaseName, 
									   vector<wstring>& Queries, vector<wstring>* pStringQueries)
{
	//получаем менеджер индексной базы
	INdxBaseManager* pNdxBaseManager=(INdxBaseManager*)GetLoadManager()->
	GetInterface(L"./core/NdxBaseManager.dll", CLSID_NdxBaseManager, IID_NdxBaseManager);

	//устанавливаем путь к базе
	pNdxBaseManager->SetBasePath(wszIndexBasePath);
	//открываем базу
	pNdxBaseManager->OpenBase(wszIndexBaseName);

	//создаем процессор индексной базы
	CNdxSearchEngine* pNdxSearchEngine=new CNdxSearchEngine;
	pNdxSearchEngine->SetLoadManager(GetLoadManager());

	FILE* pFileStream=_wfopen(L"./ndx_time.txt", L"wt");
	if(ISNULL(pFileStream)) return;


	for(unsigned int i=0; i<Queries.size(); i++){
		SetCurrentQuery(Queries[i].c_str());

		//тестируем извлечение
		START_TICK(L"TestExtracting");
		TestExtracting(pNdxSearchEngine);
		STOP_TICK(L"TestExtracting");
		
		//_ASSERTE(_CrtCheckMemory());

		fwprintf(pFileStream, L"%u.\tFull: %f\tSearch: %f\tRead:%f\tQry: %s \n", 
			i, TickManager.GetTicks(L"TestExtracting"), TickManager.GetTicks(L"FullSearch"), 
			TickManager.GetTicks(L"ReadNode"), pStringQueries?pStringQueries->at(i).c_str():L"");
		VIEW_TICKS();
		fflush(pFileStream);
	}

	fclose(pFileStream);
	
	delete pNdxSearchEngine;

	//закрываем базу
	pNdxBaseManager->CloseBase();
}

void CSearchEngine_test::TestIndexing(CNdxSearchEngine* pSearchEngine, unsigned int uiTextsCount)
{
	if(ISNULL(pSearchEngine)) return;
	if(ISNULL(GetIndexationResult())) return;

	ToConsole(L"//------------------------------TestIndexing");

	//START_TICK(L"TestIndexing");
	//процесс индексации
	pSearchEngine->StartIndexationSession();

	StartFillIndexationResult();
	InitMetaFieldCollection();
	InitDocumentStructure();

	unsigned int uiTextsNumber=uiTextsCount;
	unsigned int uiTextsBlockNumber=6;

	
	
	for(unsigned int i=0; i<uiTextsNumber; i++){
		//ToConsole(L"//-------------DocumentIndexation");
		START_TICK(L"DocumentIndexation");
		ToConsole(L"DocumentNumber", i);
		pSearchEngine->StartDocumentIndexation(GetIndexationResult());
		for(unsigned int k=0; k<uiTextsBlockNumber; k++){
			//заполняем контейнер индексов
			//FillIndexationResult();
			FillIndexationResultByStructure(k);

			//ToConsole(L"//-------IndexateDocumentChunk");
			//FillIndexationResult(false);
			//pSearchEngine->IndexateDocumentChunk(GetIndexationResult());
			//FillIndexationResult(true);
			pSearchEngine->IndexateDocumentChunk(GetIndexationResult());
			//ToConsole(L"//-------Eof-IndexateDocumentChunk");
		}

		FillMetaFieldCollection();
		//ViewMetaFieldCollection();
		pSearchEngine->EndDocumentIndexation(GetNdxMetaFieldCollection(), GetIndexationResult());
		STOP_TICK(L"DocumentIndexation");
		//if(i%50==0)
			VIEW_TICKS();
		//ToConsole(L"//------------Eof-DocumentIndexation");
		NextText();
	}

	pSearchEngine->EndIndexationSession();

	//STOP_TICK(L"TestIndexing");
	//VIEW_TICK(L"TestIndexing");
	ToConsole(L"//--------------------------------Eof-TestIndexing");

}

void CSearchEngine_test::TestExtracting(CNdxSearchEngine* pSearchEngine)
{
	if(ISNULL(pSearchEngine)) return;
	if(ISNULL(GetSearchResult())) return;

	//заполняем контейнер запроса
	FillQueryResult();

	//unsigned int uiTextSize=pSearchEngine->GetTextSize(1);

	//pSearchEngine->GetTextInfo(1, GetNdxMetaFieldCollection());
	//ViewMetaFieldCollection();

	//wstring sFieldName;

	//for(unsigned int i=0; i<GetNdxMetaFieldCollection()->size(); i++){
	//	sFieldName.assign(GetNdxMetaFieldCollection()->at(i).GetFieldName());
	//	if(sFieldName==L"FILESIZE"){
	//		unsigned int uiSize=1000;
	//		GetNdxMetaFieldCollection()->at(i).SetValue((void*)&uiSize, TYPE_UINT32);
	//	}else{
	//		GetNdxMetaFieldCollection()->at(i).Disable();
	//	}

	//}

	SS::Core::Types::IndexationSearch::TSearchParams SearchParams(1000);
	//SearchParams.UseStatistic(true, true, false);
	SS::Interface::Core::NdxSE::CNdxSearchSettings SearchSettings;
	SearchSettings.m_SearchSettings.m_EvalRelevanceMode=CNdxSearchSettings::SSearchSettings::erWordProximityWithStatistic;
	//SearchSettings.m_SearchSettings.m_StorageUsingMode=CNdxSearchSettings::SSearchSettings::esumAll;
	//SearchSettings.m_SearchSettings.m_StorageUsingMode=CNdxSearchSettings::SSearchSettings::esumHeadersAndTablesStorage;
	//SearchSettings.m_SearchSettings.m_StorageUsingMode=CNdxSearchSettings::SSearchSettings::esumHeadersAndTextStorage;
	SearchSettings.m_SearchSettings.m_StorageUsingMode=CNdxSearchSettings::SSearchSettings::esumSingleStorage;
	pSearchEngine->StartFindSession(GetISContainer());
	GetPrmQueryResult()->Init(GetQueryResult(), &SearchParams);	//	преобразуем (оптимизируем) запрос
	pSearchEngine->Find(/*L"TABLES"*/L"TEXT", GetPrmQueryResult(), &SearchParams, &SearchSettings);
	//pSearchEngine->Find(GetNdxMetaFieldCollection(), &SearchParams, &SearchSettings);
	pSearchEngine->EndFindSession();
	

	set<unsigned int> QueryIndexCollection;

	IQueryIndex* pQueryIndex=GetQueryResult()->GetFirstQueryIndex();
	while(pQueryIndex){
		QueryIndexCollection.insert(pQueryIndex->GetQueryIndex()); 
		pQueryIndex=GetQueryResult()->GetNextQueryIndex();
	}

	//отображаем хранилище результатов поиска
	//SS::Core::NdxSE::NdxProcessor::Extracting::CSearchResultFiller::ViewSearchResult(GetISContainer()->GetSearchResult(L"TEXT"), &QueryIndexCollection);
	
	//wstring Words;
	//GetWordFromDictionary(&Words);
	//ToConsole(Words.c_str());
}

void CSearchEngine_test::GetWordFromDictionary(vector<unsigned int>* pIndexCollection, wstring* pwsWords)
{
	if(ISNULL(pIndexCollection)) return;
	if(ISNULL(pwsWords)) return;

	if(pIndexCollection->empty()) return;

	SS::Interface::Core::ILinguisticProcessor* pLinguisticProcessor = NULL;
    pLinguisticProcessor = (SS::Interface::Core::ILinguisticProcessor*)GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_LinguisticProcessor, IID_LinguisticProcessor ); 

	//SS::Interface::Core::ResourceManagers::IBaseManager* pBaseManager=NULL;
	//pBaseManager = (SS::Interface::Core::ResourceManagers::IBaseManager*)
	//			GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_BaseManager);
	//pBaseManager->SetBasePath(L"d:\\SS\\Database\\");
	//pBaseManager->OpenBase(L"Test");

	SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult=GetISContainer()->GetSearchResult(L"TEXT");
	pSearchResult->Clear();
	ITextBlockCut* pTextBlockCut=pSearchResult->AddTextBlock()->AddBlockCut();
	for(unsigned int i=0; i<(unsigned int)pIndexCollection->size(); i++){
		IIndexAndPosition* pIndexAndPosition=pTextBlockCut->AddIndexAndPosition();	
		SS::Core::Types::IndexationSearch::TViewIndex ViewIndex;
		ViewIndex.m_DictionaryIndex=pIndexCollection->at(i);
		pIndexAndPosition->GetServiceIndex()->SetInfoBuff(reinterpret_cast<char*>(&ViewIndex), sizeof(ViewIndex));

		//pTextBlockCut->AddIndexAndPosition()->
		//	GetServiceIndex()->GetViewIndex()->m_DictionaryIndex=pIndexCollection->at(i);	
	}

	SS::Core::Types::IndexationSearch::TPrepareForViewParams PrepareForViewParams;
	pLinguisticProcessor->GetLinguisticShowResults()->PrepareForView(pSearchResult, &PrepareForViewParams);
	//pBaseManager->CloseBase();

	pTextBlockCut=pSearchResult->GetFirstTextBlock()->GetFirstBlockCut();
	wchar_t buf[500];
	IIndexAndPosition* pIndexAndPosition=pTextBlockCut->GetFirstIndexAndPosition();
	while(pIndexAndPosition){
		wsprintf(buf, L"%u - %s \n", 
			pIndexAndPosition->GetServiceIndex()->GetViewIndex()->m_DictionaryIndex, pIndexAndPosition->GetWord());
		pwsWords->append(buf);
		pIndexAndPosition=pTextBlockCut->GetNextIndexAndPosition();
	}
}

void CSearchEngine_test::QueryToLogicView(wstring& wsBasePath, wstring& wsBaseName, vector<wstring>& Queries, vector<wstring>& LogicQueries)
{
	SS::Interface::Core::ILinguisticProcessor* pLinguisticProcessor = NULL;
    pLinguisticProcessor = (SS::Interface::Core::ILinguisticProcessor*)GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_LinguisticProcessor, IID_LinguisticProcessor ); 

	//открываем базу новых слов
	SS::Interface::Core::ResourceManagers::IBaseManager* pBaseManager=NULL;
	pBaseManager = (SS::Interface::Core::ResourceManagers::IBaseManager*)
				GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_BaseManager);
	pBaseManager->SetBasePath(wsBasePath.c_str());
	pBaseManager->OpenBase(wsBaseName.c_str());

	SS::Core::Types::IndexationSearch::TQueryParams oQueryParams;
	SS::Core::Types::IndexationSearch::TSearchParams oSearchParams(1000);

	oQueryParams.m_GenerateQueryMode = SS::Core::Types::IndexationSearch::gqmIndex;

	SS::Core::Types::IndexationSearch::TLinguisticProcessorMode oMode;			
	oMode.GetAnalyseDepthParams()->SetIsMorpho(true);
	oMode.GetAnalyseDepthParams()->SetIsSemantic(false);
	oMode.GetAnalyseDepthParams()->SetIsSyntax(false);
	oMode.GetAnalyseDepthParams()->SetIsSynonims(false);
	pLinguisticProcessor->SetMode(&oMode);

	//LogicQueries.clear();
	for(unsigned int i=0; i<Queries.size(); i++){
		//получаем запрос от лингвистического процессора
		ToConsole(Queries[i].c_str());
		oQueryParams.SetQuery(Queries[i].c_str());
		pLinguisticProcessor->GetLinguisticQuery()->GenerateQuery(&oSearchParams, &oQueryParams, GetQueryResult(), true);
		
		//переводит его в строку, кладем в коллекцию
		wstring wsLogicQuery;
		QueryResultToString(&wsLogicQuery);

		ToConsole(wsLogicQuery.c_str());
		LogicQueries.push_back(wsLogicQuery);
	}

	pBaseManager->CloseBase();
}

void CSearchEngine_test::GetWordFromDictionary(wstring* pwsWords)
{
	if(ISNULL(pwsWords)) return;

	SS::Interface::Core::ILinguisticProcessor* pLinguisticProcessor = NULL;
    pLinguisticProcessor = (SS::Interface::Core::ILinguisticProcessor*)GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_LinguisticProcessor, IID_LinguisticProcessor ); 

	//SS::Interface::Core::ResourceManagers::IBaseManager* pBaseManager=NULL;
	//pBaseManager = (SS::Interface::Core::ResourceManagers::IBaseManager*)
	//			GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_BaseManager);
	//pBaseManager->SetBasePath(L"d:\\SS\\Database\\");
	//pBaseManager->OpenBase(L"Test");

	SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult=GetISContainer()->GetSearchResult(L"TEXT");

	SS::Core::Types::IndexationSearch::TPrepareForViewParams PrepareForViewParams;
	pLinguisticProcessor->GetLinguisticShowResults()->PrepareForView(pSearchResult, &PrepareForViewParams);
	//pBaseManager->CloseBase();

	ITextBlock* pTextBlock=NULL;
	ITextBlockCut* pTextBlockCut=NULL;
	IIndexAndPosition* pIndexAndPosition=NULL;
	wstring szValue;
	wchar_t buf[50];

	ToConsole(L"//-----------------SearchResult");
	pTextBlock=pSearchResult->GetFirstTextBlock();
	while(pTextBlock){
		szValue.clear();
		szValue.append(L"//----------TextBlock\n");
		wsprintf(buf, L"Text Number:%u\n", pTextBlock->GetTextFeature()->GetTextNumber());
		szValue.append(buf);
		wsprintf(buf, L"Text Relevance:%u\n", pTextBlock->GetRelevance());
		szValue.append(buf);

		pTextBlockCut=pTextBlock->GetFirstBlockCut();
		while(pTextBlockCut){
			szValue.append(L"  //----------TextBlockCut\n");
			szValue.append(L"    ");
			pIndexAndPosition=pTextBlockCut->GetFirstIndexAndPosition();
			int i=0;
			while(pIndexAndPosition){
				
				//wsprintf(buf, L" %u ", pIndexAndPosition->GetServiceIndex()->GetViewIndex()->m_DictionaryIndex);
				wsprintf(buf, L"%s ", pIndexAndPosition->GetWord());
				szValue.append(buf);
				
				if(i==10){
					szValue.append(L"\n    ");
					i=0;
				}

				pIndexAndPosition=pTextBlockCut->GetNextIndexAndPosition();
				i++;
			}
			szValue.append(L"\n  //----------Eof-TextBlockCut\n");
			pTextBlockCut=pTextBlock->GetNextBlockCut();
		}
		szValue.append(L"//----------Eof-TextBlock\n");
		ToConsole(szValue.c_str());
		pTextBlock=pSearchResult->GetNextTextBlock();
	}
	ToConsole(L"//-----------------Eof-SearchResult");

}


bool	CSearchEngine_test::ReadTestingPaths()
{
	//	считывание настроек из ини-файла
	tsett::CSettings	settings;
	if (!settings.SettingRead(m_wsBasePath, tsett::wcszKey_PathDatabase)	||
		!settings.SettingRead(m_wsBaseName, tsett::wcszKey_NameInd)		||
		!settings.SettingRead(m_wsQueryFile, tsett::wcszKey_TxtQueries))
	{
		ToConsole(L"\n.\n.\n.\nERROR!!! Can't read ini-file!\n.\n.\n.\n.");
		return false;
	}

	//	исправляем путь к БД (добавление слеша)
	m_wsBasePath += std::wstring(tsett::wszSlash);
	//	исправляем путь к файлу запросов (делаем полный путь)
	m_wsQueryFile =
		settings.GetExeDir()			+
		std::wstring(tsett::wszSlash)	+
		m_wsQueryFile;

	return true;
}
//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS