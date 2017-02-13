#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\textsbinarystorage_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\int_containers.h"
#include ".\search_engine.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxProcessor::Storages::CTextsBinaryStorage_test);

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Storages
{

using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Core::NdxSE::NdxProcessor::Storages;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;
using namespace SS::Interface::Core::NdxSE;
using namespace SS::Interface::Core::CommonContainers;
using namespace SS::Interface::Core::DBMS;

//--------------------------------------------------------------------//

void CTextsBinaryStorage_test::TestExtracting(void)
{
	wstring wsBasePath;
	wstring wsBaseName;
	wstring wsIndexName;

	//wsBasePath=L"d:\\IndexBasesBackup\\Demotool\\";
	wsBasePath=L"d:\\SS\\UnitTestsBin\\Core\\NdxSE\\Ut\\";
	//wsBasePath=L"d:\\SSPE\\Local Search Personal\\Database\\";
	//wsBaseName=L"IndNSNZZWNMXOAMNO0X";
	//wsBaseName=L"IndNL0O70MV07AWNO0X";
	wsBaseName=L"test_se_base";

	wsIndexName=L"TEXT";

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

	wstring wsValue;
	pNdxSearchEngine->GetTablesStructure(0, &wsValue);

	CInterval Interval;
	unsigned int uiTextNumber=34508;
	Interval.m_uiHeadPos=0;
	Interval.m_uiTailPos=0;

	SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock = GetSearchResult()->AddTextBlock();

	pNdxSearchEngine->GetTextInfo(uiTextNumber, pTextBlock->GetTextFeature());
	pTextBlock->GetTextFeature()->SetTextNumber(uiTextNumber);
	SS::Interface::Core::CommonContainers::ITitle* pTitle=pTextBlock->GetTextFeature()->GetFirstTitle();
	Interval.m_uiHeadPos=pTitle->GetFirstWord();
	Interval.m_uiTailPos=pTitle->GetLastWord();
	//pTitle=pTitle->GetNextTitle();
	//Interval.m_uiHeadPos=pTitle->GetFirstWord();
	//Interval.m_uiTailPos=pTitle->GetLastWord();

	pNdxSearchEngine->FillTextContent(&wsIndexName, Interval.m_uiHeadPos, Interval.m_uiTailPos, pTextBlock);
	SS::Core::NdxSE::NdxProcessor::Extracting::CSearchResultFiller::ViewSearchResult(GetSearchResult());
	
	wchar_t buf1[500];
	unsigned int uiSize=0;
	TNdxMetaFieldCollection NdxMetaFieldCollection;
	NdxMetaFieldCollection.push_back(CNdxMetaField(L"FileSize", TYPE_UINT32, MOD_COMMON));
	NdxMetaFieldCollection.push_back(CNdxMetaField(L"FileName", TYPE_WSTRING, MOD_COMMON));
	wstring* pwsPath;
	pNdxSearchEngine->INdxSearchEngineTextsInfo()->GetTextInfo(uiTextNumber, &NdxMetaFieldCollection);
	memcpy((void*)&uiSize, NdxMetaFieldCollection[0].GetValue(), sizeof(unsigned int));
	pwsPath=(wstring*)NdxMetaFieldCollection[1].GetValue();
	wsprintf(buf1, L"FileID: %u FileSize: %u FilePath: %s", uiTextNumber, uiSize, pwsPath->c_str());
	ToConsole(buf1);

	delete pNdxSearchEngine;

	//закрываем базу
	pNdxBaseManager->CloseBase();

}

void CTextsBinaryStorage_test::TestAll(void)
{
	//UT_STOP_LOGING
	
	//получаем менеджер индексной базы
	INdxBaseManager* pNdxBaseManager=(INdxBaseManager*)GetLoadManager()->
	GetInterface(L"./core/NdxBaseManager.dll", CLSID_NdxBaseManager, IID_NdxBaseManager);
	
	pNdxBaseManager->ResetNdxBaseStructure();
	//добавляем базе индекс с нужной структурой
	pNdxBaseManager->AddNdxBaseUnit(L"TEXT", escAbsSentence);	

	//устанавливаем путь к базе
	pNdxBaseManager->SetBasePath(GetUTWorkingPath());
	//открываем базу
	pNdxBaseManager->OpenBase(L"test_tbin_base");
	//очищаем базу для индексации
	pNdxBaseManager->ClearBase();

	//создаем менеджер хранилищ
	CStorageManager* pStorageManager=new CStorageManager();
	pStorageManager->Init(pNdxBaseManager);
	pStorageManager->SetIndexingMode();

	CTextsBinaryStorage* pTextsBinaryStorage=pStorageManager->GetIndexStorage(L"TEXT")->GetTextsBinaryStorage();

	CInterval Interval;
	unsigned int uiTextNumber=0;
	vector<unsigned int> CheckVector;

	Interval.m_uiHeadPos=990;
	Interval.m_uiTailPos=1100;
	uiTextNumber=12;

	//наполняем хранилище данными
	FillTextsBinaryStorage(pTextsBinaryStorage, &Interval, uiTextNumber, &CheckVector);
	
	//тестируем
	TestTextsBinaryStorage(pTextsBinaryStorage, &Interval, uiTextNumber, &CheckVector);

	//удаляем менеджер хранилищ
	if(pStorageManager) delete pStorageManager;

	//закрываем базу
	pNdxBaseManager->CloseBase();

	//UT_START_LOGING

}

void CTextsBinaryStorage_test::FillTextsBinaryStorage(CTextsBinaryStorage* pTextsBinaryStorage,
													  CInterval* pInterval, 
													  unsigned int uiTextNumber, vector<unsigned int>* pCheckVector)
{
	if(ISNULL(pTextsBinaryStorage)) return;
	if(ISNULL(GetIndexationResult())) return;
	if(ISNULL(pInterval)) return;
	if(ISNULL(pCheckVector)) return;


	ITextBlock* pTextBlock=NULL;
	ITextBlockCut* pTextBlockCut=NULL;
	IIndexAndPosition* pIndexAndPosition=NULL;

	StartFillIndexationResult();

	for(unsigned int i=0; i<10000; i++){
		START_TICK(L"Full");
		
		ToConsole(L"TextID", i);

		//заполняем контейнер индексов
		START_TICK(L"FillIndexationResult");
		FillIndexationResult();
		STOP_TICK(L"FillIndexationResult");

		pTextBlock=GetIndexationResult()->GetFirstTextBlock();
		while(pTextBlock){
			pTextBlockCut=pTextBlock->GetFirstBlockCut();
			while(pTextBlockCut){
				pIndexAndPosition=pTextBlockCut->GetFirstIndexAndPosition();
				while(pIndexAndPosition){
					START_TICK(L"AddIndex");
					pTextsBinaryStorage->AddIndex(pIndexAndPosition->GetServiceIndex(), pIndexAndPosition->GetFirstCharPosition());
					STOP_TICK(L"AddIndex");
					if(uiTextNumber==i){
						unsigned int uiPos=pIndexAndPosition->GetPosition();
						unsigned int uiFirstCharPosition=pIndexAndPosition->GetFirstCharPosition();
						if(uiTextNumber==i && pInterval->m_uiHeadPos<=uiPos && 
							pInterval->m_uiTailPos>=uiPos){
								pCheckVector->push_back(uiFirstCharPosition);
						}
					}

					pIndexAndPosition=pTextBlockCut->GetNextIndexAndPosition();
				}
				pTextBlockCut=pTextBlock->GetNextBlockCut();
			}
			
			pTextsBinaryStorage->SetTextBlockComplete();
			pTextBlock=GetIndexationResult()->GetNextTextBlock();
		}

		START_TICK(L"SetTextComplete");
		unsigned int uiText=pTextsBinaryStorage->SetTextComplete();
		STOP_TICK(L"SetTextComplete");

		STOP_TICK(L"Full");
		NextText();
	}

	VIEW_TICKS();
}

void CTextsBinaryStorage_test::TestTextsBinaryStorage(CTextsBinaryStorage* pTextsBinaryStorage, 
		CInterval* pInterval, unsigned int uiTextNumber, vector<unsigned int>* pCheckVector)
{
	if(ISNULL(pTextsBinaryStorage)) return;
	if(ISNULL(GetSearchResult())) return;
	if(ISNULL(pInterval)) return;

	if(pInterval->GetLength()!=pCheckVector->size()){
		TO_CONSOLE(L"CheckVector or Interval not correct");
		return;
	}

	ITextBlock* pTextBlock=GetSearchResult()->AddTextBlock();
	pTextBlock->GetTextFeature()->SetTextNumber(uiTextNumber);

	//заполянем TextBlock по интервалу
	wstring wsIndexStorageName=L"TEXT";
	pTextsBinaryStorage->FillTextBlock(pTextBlock, pInterval, &wsIndexStorageName);

	ITextBlockCut* pTextBlockCut=NULL;
	IIndexAndPosition* pIndexAndPosition=NULL;
	unsigned int uiIndex=0;
	unsigned int uiCharPosition=0;
	pTextBlockCut=pTextBlock->GetFirstBlockCut();
	while(pTextBlockCut){
		pIndexAndPosition=pTextBlockCut->GetFirstIndexAndPosition();
		unsigned int iuInc=0;
		while(pIndexAndPosition){

			uiIndex=pIndexAndPosition->GetServiceIndex()->GetViewIndex()->m_DictionaryIndex;
			uiCharPosition=pIndexAndPosition->GetFirstCharPosition();	

			CPPUNIT_ASSERT_EQUAL(uiCharPosition, pCheckVector->at(iuInc));

			iuInc++;
			pIndexAndPosition=pTextBlockCut->GetNextIndexAndPosition();
		}
		pTextBlockCut=pTextBlock->GetNextBlockCut();
	}

	GetSearchResult()->Clear();
}

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS