#include "StdAfx.h"

#include ".\indexing_manager.h"
#include ".\console.h"
#include ".\const.h"
#include ".\global_pool.h"
#include "..\ASCInterface\defines.h"
#include ".\sr_filler.h"

USES_LOAD_MANAGER;

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Indexing
{

using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Interface::Core::CommonContainers;
using namespace SS::Core::NdxSE::NdxProcessor::MemoryManagement;
using namespace SS::Interface::Core::NdxSE;

//--------------------------------------------------------------------//

CIndexingManager::CIndexingManager(void) : m_bTextFeatureAdded(false), m_pStorageManager(NULL)
{	
}

CIndexingManager::~CIndexingManager(void)
{
}

void CIndexingManager::Init(SS::Core::NdxSE::NdxProcessor::Storages::CStorageManager* pStorageManager)
{
	if(ISNULL(pStorageManager)) SS_THROW(L"pStorageManager is NULL");

	m_pStorageManager=pStorageManager;

}

HRESULT CIndexingManager::StartIndexationSession(void)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");

	m_IndexStorageCollection.clear();
	
	m_pStorageManager->Update();
	//ToConsole(L"StartIndexationSession");

	SS_TRY

	//устанавливаем режим индексации
	m_pStorageManager->SetIndexingMode();

	//готовим хранилища к индексации
	m_pStorageManager->PrepareIndexing();

	return S_OK;

	SS_CATCH(L"");
	return E_FAIL;
}

HRESULT CIndexingManager::EndIndexationSession(void)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	if(ISNULL(m_pStorageManager->GetTextsInfoStorage())) SS_THROW(L"GetTextsInfoStorage is NULL");
	if(ISNULL(m_pStorageManager->GetTextsPathsTreeStorage())) SS_THROW(L"GetTextsPathsTreeStorage is NULL");
	
	m_pStorageManager->Update();

	//ToConsole(L"EndIndexationSession");

	SS_TRY

	if(!m_pStorageManager->IsIndexingMode()){
		///если режим индексации не выставлен ничего не получится
		TO_CONSOLE(L"IndexingMode not set!!!");
		SS_THROW(L"IndexingMode not set!!!");
	}

	//в завершении индексации строим дерево путей текстов
	m_pStorageManager->GetTextsPathsTreeStorage()->BuildTree(m_pStorageManager->GetTextsFieldsStorage());

	//сигнализируем хранилищам о завершении индексации
	m_pStorageManager->CompleteIndexing();

	//сбрасыаем пулы памяти
	g_PoolsManager.ResetPools();

	//индексация закончена устанавливаем поиска
	m_pStorageManager->SetExtractingMode();
	
	m_IndexStorageCollection.clear();

	return S_OK;

	SS_CATCH(L"");
	return E_FAIL;
}

HRESULT CIndexingManager::StartDocumentIndexation(IIndexationResult* pIndexationResult)
{
	if(ISNULL(pIndexationResult)) SS_THROW(L"pIndexationResult is NULL");
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");

	SS_TRY

	m_pStorageManager->Update();

	if(!m_pStorageManager->IsIndexingMode()){
		///если режим индексации не выставлен ничего не получится
		TO_CONSOLE(L"IndexingMode not set!!!");
		SS_THROW(L"IndexingMode not set!!!");
	}
	
	//взводим флаг
	m_bTextFeatureAdded=false;

	return S_OK;

	SS_CATCH(L"");
	return E_FAIL;
}

HRESULT CIndexingManager::IndexateDocumentChunk(IIndexationResult* pIndexationResult)
{
	if(ISNULL(pIndexationResult)) SS_THROW(L"pIndexationResult is NULL");
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");

	SS_TRY

	m_pStorageManager->Update();

	if(!m_pStorageManager->IsIndexingMode()){
		///если режим индексации не выставлен ничего не получится
		TO_CONSOLE(L"IndexingMode not set!!!");
		SS_THROW(L"IndexingMode not set!!!");
	}
	
	ITextBlock* pTextBlock=NULL;
	ITextBlockCut* pTextBlockCut=NULL;
	IIndexAndPosition* pIndexAndPosition=NULL;

	START_TICK(L"Add_Indexes");

	bool bTableData=false;
	map<wstring, Storages::CIndexStorage*> IndexStorageCollection;
	wstring wsIndexedFieldName;
	Storages::CIndexStorage* pIndexStorage=NULL;
	map<wstring, Storages::CIndexStorage*>::iterator itIndexStorage;
	pTextBlock=pIndexationResult->GetFirstTextBlock();
	while(pTextBlock){
		pTextBlockCut=pTextBlock->GetFirstBlockCut();
		while(pTextBlockCut){
			
			//пока для заголовков, принудительно выставляем имя хранилища
			if(pTextBlockCut->GetType()==SS::Core::Types::IndexationSearch::tctHeader){
				pTextBlockCut->SetIndexTypeName(L"HEADERS");
			}
			
			//получаем индексное хранилище, куда будем заносить индексы куска текста
			wsIndexedFieldName.assign(pTextBlockCut->GetIndexTypeName());
			pIndexStorage=NULL;
			itIndexStorage=IndexStorageCollection.find(wsIndexedFieldName);
			if(itIndexStorage==IndexStorageCollection.end())
			{
				pIndexStorage=m_pStorageManager->GetIndexStorage(wsIndexedFieldName.c_str());
				if(ISNULL(pIndexStorage)){
					SS_THROW(L"IndexStorage by name " AND wsIndexedFieldName.c_str() AND L" is NULL");
				}
				IndexStorageCollection.insert(
					map<wstring, Storages::CIndexStorage*>::value_type(wsIndexedFieldName, pIndexStorage));
			}else{
				pIndexStorage=itIndexStorage->second;
			}
			
			if(wsIndexedFieldName==L"TABLES"){
				//табличные данные заносим по особому и выходим
				((Storages::CIndexStorageTables*)pIndexStorage)->AddTableIndexes(pIndexationResult);
				bTableData=true;
				break;
			}else{
				//отображаем блоккат
				//if(pTextBlockCut->GetType()==SS::Core::Types::IndexationSearch::tctHeader)
				//Extracting::CSearchResultFiller::ViewTextBlockCut(pTextBlockCut, true, true);
				//заносим индексы в хранилище
				pIndexStorage->AddIndexes(pTextBlockCut);
			}

			//переходим к следующему куску
			if(bTableData) break;
			pTextBlockCut=pTextBlock->GetNextBlockCut();
		}

		//переходим к следующему блоку
		if(bTableData) break;
		pTextBlock=pIndexationResult->GetNextTextBlock();
	}

	STOP_TICK(L"Add_Indexes");
	//VIEW_TICK(L"Add_Indexes");
	
	START_TICK(L"UpdateIndexedData");
	itIndexStorage=IndexStorageCollection.begin();
	while(itIndexStorage!=IndexStorageCollection.end())
	{
		//обновляем проиндексированные данные в индекса
		itIndexStorage->second->UpdateIndexedData();
		itIndexStorage++;
	}
	STOP_TICK(L"UpdateIndexedData");
	//VIEW_TICK(L"UpdateIndexedData");
	
	//g_PoolsManager.View();
	//ViewMemoryStatus();

	return S_OK;

	SS_CATCH(L"");
	return E_FAIL;
}

HRESULT CIndexingManager::IndexateTextFeature(
	SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult)
{
	if(ISNULL(pIndexationResult)) SS_THROW(L"pIndexationResult is NULL");
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");

	SS_TRY

	m_pStorageManager->Update();

	if(!m_pStorageManager->IsIndexingMode()){
		///если режим индексации не выставлен ничего не получится
		TO_CONSOLE(L"IndexingMode not set!!!");
		SS_THROW(L"IndexingMode not set!!!");
	}

	ITextBlock* pTextBlock=pIndexationResult->GetFirstTextBlock();
	if(pTextBlock && pTextBlock->GetTextFeature() && !m_bTextFeatureAdded){
		//кладем информацию по тексту 
		m_pStorageManager->GetTextsInfoStorage()->AddTextInfo(pTextBlock->GetTextFeature());
		//устанавливаем флаг 
		m_bTextFeatureAdded=true;
	}else{
		return E_FAIL;
	}

	return S_OK;

	SS_CATCH(L"");
	return E_FAIL;
}

HRESULT CIndexingManager::EndDocumentIndexation(TNdxMetaFieldCollection* pNdxMetaFieldCollection,
												IIndexationResult* pIndexationResult)
{
	//if(ISNULL(pIndexationResult)) SS_THROW(L"pIndexationResult is NULL");
	//if(ISNULL(pNdxMetaFieldCollection)) SS_THROW(L"pNdxMetaFieldCollection is NULL");
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");

	SS_TRY

	m_pStorageManager->Update();

	if(!m_pStorageManager->IsIndexingMode()){
		///если режим индексации не выставлен ничего не получится
		TO_CONSOLE(L"IndexingMode not set!!!");
		SS_THROW(L"IndexingMode not set!!!");
	}

	////добавляем поля документов
	m_pStorageManager->GetTextsFieldsStorage()->AddTextInfo(pNdxMetaFieldCollection);

	if(!m_bTextFeatureAdded){
		//если фича не была добавлена кладем пустую
		m_pStorageManager->GetTextsInfoStorage()->AddTextInfo(NULL);
		m_bTextFeatureAdded=true;
	}

	//сигнализируеум об окончании индексации документа
	m_pStorageManager->CompleteDocumentIndexing();

	return S_OK;

	SS_CATCH(L"");
	return E_FAIL;
}

//--------------------------------------------------------------------//

}
}
}
}
}