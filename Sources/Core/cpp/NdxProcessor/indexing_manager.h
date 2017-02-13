#pragma once

#include ".\storage_manager.h"
#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASDInterface\INdxSearchEngine.h"

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

//--------------------------------------------------------------------//

///управление процессом индексации
class CIndexingManager : public SS::Interface::Core::NdxSE::INdxSearchEngineIndexation
{
public:
	CIndexingManager(void);
	virtual ~CIndexingManager(void);

	///инициализирует менеджер процесса индексации
	void Init(SS::Core::NdxSE::NdxProcessor::Storages::CStorageManager* pStorageManager);

	///запуск сессии индексации коллекции документов
	HRESULT StartIndexationSession(void);
	///останов сессии индексации коллекции документов
	HRESULT EndIndexationSession(void);
	///запуск пакетной индексации документа
	HRESULT StartDocumentIndexation(
		SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult);
	///индексаци€ очередного куска документа
	HRESULT IndexateDocumentChunk(
		SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult);

	///индексаци€ лингвистической текстовой информаци€ !!! вызываетс€ один раз дл€ документа
	HRESULT IndexateTextFeature(
		SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult);

	///останов пакетной индексации документа
	HRESULT EndDocumentIndexation(
		SS::Interface::Core::NdxSE::TNdxMetaFieldCollection* pNdxMetaFieldCollection,
		SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult);

protected:
	///менеджер логических хранилищ индексной базы
	SS::Core::NdxSE::NdxProcessor::Storages::CStorageManager* m_pStorageManager;
	///пул индексных хранилищ на сессию индексации
	map<wstring, Storages::CIndexStorage*> m_IndexStorageCollection;

	///флаг занесени€ информации по тексту, текстфичи
	bool m_bTextFeatureAdded;
};

//--------------------------------------------------------------------//

}
}
}
}
}