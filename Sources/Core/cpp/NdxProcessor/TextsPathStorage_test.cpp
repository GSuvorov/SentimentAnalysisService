#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\textspathstorage_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\texts_path_storage.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxProcessor::Storages::CTextsPathsTree_test);

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

using namespace SS::Core::NdxSE::NdxProcessor::Storages;
using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Interface::Core::NdxSE::NdxDataStorage;
using namespace SS::Interface::Core::NdxSE;

//--------------------------------------------------------------------//

void CTextsPathsTree_test::TestAll(void)
{
	UT_STOP_LOGING

	//получаем менеджер индексной базы
	INdxBaseManager* pNdxBaseManager=(INdxBaseManager*)GetLoadManager()->
	GetInterface(L"./core/NdxBaseManager.dll", CLSID_NdxBaseManager, IID_NdxBaseManager);
	
	pNdxBaseManager->ResetNdxBaseStructure();
	//добавляем базе индекс с нужной структурой
	pNdxBaseManager->AddNdxBaseUnit(ObligatoryStorageNames::TextsPathTreeStorage, escAbsSentence);	

	//устанавливаем путь к базе
	pNdxBaseManager->SetBasePath(GetUTWorkingPath());
	//открываем базу
	pNdxBaseManager->OpenBase(L"test_tpath_base");
	//очищаем базу для индексации
	pNdxBaseManager->ClearBase();

	//создаем менеджер хранилищ
	CStorageManager* pStorageManager=new CStorageManager();
	pStorageManager->Init(pNdxBaseManager);
	pStorageManager->SetIndexingMode();

	CTextsPathsTreeStorage* pTextsPathsTreeStorage=pStorageManager->GetTextsPathsTreeStorage();
	
	//проверка заполнения путями
	CPPUNIT_ASSERT(TestFilling(pTextsPathsTreeStorage));

	//проверка извлечения путей 
	CPPUNIT_ASSERT(TestExtracting(pTextsPathsTreeStorage));

	//удаляем менеджер хранилищ
	if(pStorageManager) delete pStorageManager;

	//закрываем базу
	pNdxBaseManager->CloseBase();

	UT_START_LOGING
}

bool CTextsPathsTree_test::TestFilling(CTextsPathsTreeStorage* pTextsPathsTreeStorage)
{
	if(ISNULL(pTextsPathsTreeStorage)) return false;

	bool bTextPathExist=pTextsPathsTreeStorage->IsTextPathExist(L"C:\\1.txt");

	//формируем тестовые пути
	vector<std::wstring> Paths;
	MakePaths(GetUTWorkingPath(), &Paths);
	
	//добавляем пути к дереву
	for(unsigned int i=0; i<Paths.size(); i++){
		pTextsPathsTreeStorage->AddTextPath(i, (wchar_t*)Paths[i].c_str());
	}
	
	//сбрасываем дерево на диск
	pTextsPathsTreeStorage->MoveDown();
	return true;
}

bool CTextsPathsTree_test::TestExtracting(CTextsPathsTreeStorage* pTextsPathsTreeStorage)
{
	if(ISNULL(pTextsPathsTreeStorage)) return false;

	vector<unsigned int> buff(1, 0);
	do{
		STextPathChunk TextPathChunk(NULL, 0);
		TextPathChunk.m_uiMyOffset=buff[0];
		pTextsPathsTreeStorage->FillTextPathChunk(&TextPathChunk);

		buff.clear();
		pTextsPathsTreeStorage->GetChildsOffsets(&TextPathChunk, &buff);
	
	}while(!buff.empty());
	
	bool bTextPathExist=pTextsPathsTreeStorage->IsTextPathExist(L"D:\\SS\\UnitTestsBin\\Core\\ndxse\\ut\\test_se_base\\tpinf.srv");
	CPPUNIT_ASSERT(bTextPathExist);

	return true;
}

void CTextsPathsTree_test::MakePaths(const wchar_t* szPathFrom, vector<std::wstring>* pPaths)
{
	WIN32_FIND_DATA fd; 
	wchar_t szPath[MAX_PATH]; 
	wchar_t szFilename[MAX_PATH]; 
	try
	{
		if(szPathFrom[wcslen(szPathFrom)-1]!='\\'){
			wcscat((wchar_t*)szPathFrom, L"\\");
		}
		wsprintf(szPath, L"%s*.*", szPathFrom); 

		HANDLE hFile = FindFirstFile(szPath, &fd); 
		if (hFile != INVALID_HANDLE_VALUE) 
		{ 
		     
			wsprintf(szFilename, L"%s%s", szPathFrom, fd.cFileName); 
			if (IsDirectory(&fd)) 
			{ 
				MakePaths(szFilename, pPaths); 
			}else{ 
				pPaths->push_back(szFilename); 
			}
		 
			while (FindNextFile(hFile, &fd)!=0) 
			{ 
				wsprintf(szFilename, L"%s%s", szPathFrom, fd.cFileName); 
				if(IsDirectory(&fd)) 
				{ 
					MakePaths(szFilename, pPaths); 
				} 
				else{ 
					pPaths->push_back(szFilename); 
				}
			} 
			FindClose(hFile); 
		} 
	}
	catch(...)
	{
		ERR_TO_CONSOLE(erTryCatch, L"eh-h...");
	};
	
}

bool CTextsPathsTree_test::IsDirectory(WIN32_FIND_DATA* pFD) 
{
  if (wcscmp(pFD->cFileName, L".")==0) return false;
  if (wcscmp(pFD->cFileName, L"..")==0) return false;
  return ( pFD->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )!=0?true:false;
}

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS