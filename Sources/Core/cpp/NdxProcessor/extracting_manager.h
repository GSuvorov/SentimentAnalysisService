#pragma once

#include "..\ASDInterface\INdxSearchEngine.h"
#include "..\ASCInterface\ICommonServices.h"
#include ".\eval.h"
#include ".\search_factory.h"
#include ".\sr_filler.h"
#include ".\storage_manager.h"
#include ".\res_containers.h"
#include ".\text_info_manager.h"


namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Extracting
{

//--------------------------------------------------------------------//

///управляет извлечением данных из индексной базы
class CExtractingManager :  public SS::Interface::Core::NdxSE::INdxSearchEngineFind
{
	
	Extracting::CTextInfoManager* m_pTextInfoManager;
public:
	CExtractingManager(void);
	virtual ~CExtractingManager(void);

	///инициализирует менеджер процесса индексации
	void Init(SS::Core::NdxSE::NdxProcessor::Storages::CStorageManager* pStorageManager,
		SS::Interface::Core::CommonServices::ISettingsServer* pSettingsServer, 
		Extracting::CTextInfoManager* pTextInfoManager);

	///запуск сессии поиска, получает контайнер результатов поиска 
	//где по окончании поисковой сессии будут лежать результаты
	HRESULT StartFindSession(
		///контейнер результатов поиска 
		SS::Interface::Core::IISContainerUM* pISContainer);

	///останов сессии поиска, записывает в контайнер поданный в StartFindSession результаты поиска 
	HRESULT EndFindSession(void);
	///останов сессии поиска, записывает в контайнер поданный в StartFindSession результаты поиска 
	///для формирования результатов поиска ТОЛЬКО по полям точного значения
	HRESULT EndFindByFieldSession(void);

	///продолжает поиск в текущей индексной базе, с места на котором он был прерван 
	bool FindNext(SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult);

	///поиск в индексном хранилище по запросу
	HRESULT Find(
		///имя индексного хранилища, где выполнять поиск
		const wchar_t* wszIndexStorageName,
		///контайнер поискового запроса
		SS::Interface::Core::NdxSE::IParametrizedQuery* pPrmQueryResult,
		///общие настройки поиска
		SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams, 
		///настройки поиска поискового движка
		SS::Interface::Core::NdxSE::CNdxSearchSettings* pSearchSettings);

	///поиск в таблице мета полей по фильтру
	HRESULT Find(
		///коллекция мета-полей документа (фильтр)
		SS::Interface::Core::NdxSE::TNdxMetaFieldCollection* pNdxMetaFieldCollection,
		///общие настройки поиска
		SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams, 
		///настройки поиска поискового движка
		SS::Interface::Core::NdxSE::CNdxSearchSettings* pSearchSettings);
protected:
	/// Возвращает текстовый блок по имени
	SS::Interface::Core::CommonContainers::ITextBlock* GetTextBlock(wstring& wsIndexStorageName,
		SS::Core::NdxSE::NdxProcessor::Containers::CFindResult* pFindResult);

	///Очищает коллекцию текстовых блоков (по числу найденных хранилищ)
	void ClearTextBlocksStorageCollection();

	///текщие результаты поиска
	SS::Interface::Core::IISContainerUM* m_pISContainer;

	///Хранилище вновь созданных текстовых блоков
	typedef map<wstring, SS::Interface::Core::CommonContainers::ITextBlock*> TTextBlocksStorageCollection;

	///Для каждого хранилища создается свой текстовый блок.
	TTextBlocksStorageCollection m_TextBlocksStorageCollection;

	///поиск в индексном хранилище по запросу
	HRESULT ComplexFind(
		const wchar_t* wszIndexStorageName,
		SS::Interface::Core::NdxSE::IParametrizedQuery*	pPrmQueryResult,
		SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams, 
		SS::Interface::Core::NdxSE::CNdxSearchSettings* pSearchSettings,
		Containers::CFindResultCollection::EAddingMode ResultsAddingMode);
	///поиск в индексном хранилище по запросу
	bool FindByQuery(
		const wchar_t* wszIndexStorageName,
		SS::Interface::Core::NdxSE::IParametrizedQuery*	pPrmQueryResult,
		SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams, 
		SS::Interface::Core::NdxSE::CNdxSearchSettings* pSearchSettings,
		Containers::CFindResultCollection::EAddingMode ResultsAddingMode);

	///готовит менеджера к поиску
	/**
	\param[in]	wszIndexStorageName		Название хранилища
	\param[in]	pCommonSearchParameter	Параметры поиска
	\param[in]	pSearchSettings			Настройки поиска
	\return	Наверное успешность
	*/
	bool PrepareToFind(
		const wchar_t* wszIndexStorageName,
		SS::Core::Types::IndexationSearch::TSearchParams* pCommonSearchParameter,
		SS::Interface::Core::NdxSE::CNdxSearchSettings* pSearchSettings);

	///сбрасывает менеджер поиска
	void Reset(void);

	//заполняет результат поиска в зависимости от режимов поиска
	void FillSearchResult(SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock, 
			Containers::CInterval* pInterval, wstring* pwsIndexStorageName, unsigned int uiResultGroupID);
private:
	///текущий запрос
	SS::Interface::Core::NdxSE::IParametrizedQuery* m_pCurrentPrmQueryResult;

	///вычислитель - управляет процессом наполнения контейнера результатов
	CEvaluator* m_pEvaluator;

	///поисковая фабрика
	CSearchFactory* m_pSearchFactory;

	///интерфейс для работы с настройками
	SS::Interface::Core::CommonServices::ISettingsServer* m_pSettingsServer;

	///менеджер логических хранилищ индексной базы
	SS::Core::NdxSE::NdxProcessor::Storages::CStorageManager* m_pStorageManager;

	///коллекция номеров текстов результатов 
	///(формируется по мере конвертации результатов за всю сессию поиска включая поиск далее)
	///добавление номеров происходит поступательно в порядке возрастания
	CSearchResultFiller::TResultsTextsNumbers m_ResultsTextsNumbers;

	///текщие результаты поиска
	Containers::CFindResultCollection m_SessionFindResults;
};

//--------------------------------------------------------------------//

}
}
}
}
}