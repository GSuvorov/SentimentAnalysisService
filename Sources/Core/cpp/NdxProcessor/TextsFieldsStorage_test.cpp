#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\textsfieldsstorage_test.h"
#include ".\test_const.h"
#include ".\console.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxProcessor::Storages::CTextsFieldsStorage_test);

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

void CTextsFieldsStorage_test::TestAll(void)
{
	//UT_STOP_LOGING
	
	//получаем менеджер индексной базы
	INdxBaseManager* pNdxBaseManager=(INdxBaseManager*)GetLoadManager()->
	GetInterface(L"./core/NdxBaseManager.dll", CLSID_NdxBaseManager, IID_NdxBaseManager);
	
	pNdxBaseManager->ResetNdxBaseStructure();
	//добавляем базе индекс с нужной структурой
	pNdxBaseManager->AddNdxBaseUnit(ObligatoryStorageNames::TextsFieldsStorage, escUndefined);	

	//устанавливаем путь к базе
	pNdxBaseManager->SetBasePath(GetUTWorkingPath());
	//открываем базу
	pNdxBaseManager->OpenBase(L"test_tfld_base");
	//очищаем базу для индексации
	pNdxBaseManager->ClearBase();

	//создаем менеджер хранилищ
	CStorageManager* pStorageManager=new CStorageManager();
	pStorageManager->Init(pNdxBaseManager);
	pStorageManager->SetIndexingMode();

	CTextsFieldsStorage* pTextsFieldsStorage=pStorageManager->GetTextsFieldsStorage();
	
	//наполняем хранилище
	FillStorage(pTextsFieldsStorage);
	
	//тестируем извлечение
	TestExtracting(pTextsFieldsStorage);
	TestFilterExtracting(pTextsFieldsStorage);

	//удаляем менеджер хранилищ
	if(pStorageManager) delete pStorageManager;

	//закрываем базу
	pNdxBaseManager->CloseBase();

	//UT_START_LOGING

}

void CTextsFieldsStorage_test::FillStorage(CTextsFieldsStorage* pTextsFieldsStorage)
{
	if(ISNULL(pTextsFieldsStorage)) return;

	InitMetaFieldCollection();

	ToConsole(L"Bof appending...");
	
	StartFillIndexationResult();

	pTextsFieldsStorage->PrepareIndexing();

	for(int i=0; i<10000; i++){
		
		FillMetaFieldCollection();
		pTextsFieldsStorage->AddTextInfo(GetNdxMetaFieldCollection());
		
		//if(i>=1600 && i<=1610){
		//	wchar_t buf[100];
		//	wsprintf(buf, L"FileID: %u FileSize: %u FileDate: %u", i, uiSize, (unsigned int)oFT.dwLowDateTime);
		//	ToConsole(buf);
		//}
		NextText();
	}
	ToConsole(L"Eof appending...");
	
	//pTextsFieldsStorage->Scan();
}

void CTextsFieldsStorage_test::TestExtracting(CTextsFieldsStorage* pTextsFieldsStorage)
{
	ToConsole(L"Bof extracting...");
	unsigned int uiSize=0;

	wchar_t buf1[100];
	CNdxMetaField NdxMetaField(L"FileSize", TYPE_UINT32, MOD_COMMON);
	for(int i=1600; i<=1610; i++){
		pTextsFieldsStorage->GetTextInfo(i, &NdxMetaField);
		memcpy((void*)&uiSize, NdxMetaField.GetValue(), sizeof(unsigned int));
		wsprintf(buf1, L"FileID: %u FileSize: %u", i, uiSize);

		ToConsole(buf1);
	}

	TNdxMetaFieldCollection NdxMetaFieldCollection;
	NdxMetaFieldCollection.push_back(CNdxMetaField(L"FileSize", TYPE_UINT32, MOD_COMMON));
	NdxMetaFieldCollection.push_back(CNdxMetaField(L"FileName", TYPE_WSTRING, MOD_COMMON));
	wstring* pwsPath;
	for(int i=1600; i<=1610; i++){
		pTextsFieldsStorage->GetTextInfo(i, &NdxMetaFieldCollection);
		memcpy((void*)&uiSize, NdxMetaFieldCollection[0].GetValue(), sizeof(unsigned int));
		pwsPath=(wstring*)NdxMetaFieldCollection[1].GetValue();
		wsprintf(buf1, L"FileID: %u FileSize: %u FilePath: %s", i, uiSize, pwsPath->c_str());

		ToConsole(buf1);
	}

	ToConsole(L"Eof extracting...");
}

void CTextsFieldsStorage_test::TestFilterExtracting(CTextsFieldsStorage* pTextsFieldsStorage)
{
	Containers::CFindResultCollection FindResults;
	GetNdxMetaFieldCollection()->at(2).Disable();

	ToConsole(L"Bof filter extracting...");
	char buf[10];
	memset(buf, 0, 10);
	FILETIME oFT;
	unsigned int uiSize=0;
	
	FindResults.ResetAddingSession();

	for(int i=0; i<1000; i++){

		FindResults.StartFindResultAdding(Containers::CFindResultCollection::eamAlways);
		
		uiSize=rand()&0x0000000f;
		GetNdxMetaFieldCollection()->at(0).SetValue((void*)&uiSize, TYPE_UINT32);

		oFT.dwLowDateTime = rand()&0x0000000f;
		oFT.dwHighDateTime=0;
		memcpy((void*)buf, (void*)&oFT, 8);
		GetNdxMetaFieldCollection()->at(1).SetValue((void*)&buf, TYPE_BYTE_10);
		GetNdxMetaFieldCollection()->at(3).Disable();

		pTextsFieldsStorage->GetTexts(GetNdxMetaFieldCollection(), &FindResults, NULL);
		
		FindResults.StopFindResultAdding();
		//FindResults.View();
	}

	FindResults.View();

	for(int i=0; i<1; i++){

		FindResults.StartFindResultAdding(Containers::CFindResultCollection::eamIfExist);
		
		uiSize=8;
		GetNdxMetaFieldCollection()->at(0).SetLowerValue((void*)&uiSize, TYPE_UINT32);
		uiSize=16;
		GetNdxMetaFieldCollection()->at(0).SetUpperValue((void*)&uiSize, TYPE_UINT32);
		GetNdxMetaFieldCollection()->at(1).Disable();
		GetNdxMetaFieldCollection()->at(3).Disable();

		pTextsFieldsStorage->GetTexts(GetNdxMetaFieldCollection(), &FindResults, NULL);
		
		FindResults.StopFindResultAdding();
		//FindResults.View();
	}

	FindResults.View();

	ToConsole(L"Eof filter extracting...");
}

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS
