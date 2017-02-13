#pragma once

#include ".\ev_factory.h"
#include ".\sr_filler.h"
#include ".\es_filler_query.h"
#include ".\res.h"
#include ".\lgc_usings.h"
#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASCInterface\TCoommonTypes.h"
#include "..\ASDInterface\TNdxSearchEngineTypes.h"
#include "..\ASDInterface\INDXQuery.h"
#include ".\storage_manager.h"

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

///поисковые настройки
struct SSearchSettings
{
	///общие настройки поиска
	SS::Core::Types::IndexationSearch::TSearchParams m_CommonSettings;
	///настройки поиска поисквого движка
	SS::Interface::Core::NdxSE::CNdxSearchSettings m_EngineSettings;
};

//--------------------------------------------------------------------//

///фабрика для создания объектов для поиска
class CSearchFactory : public CEvalFactory
{
public:
	CSearchFactory(
		const wchar_t* wszIndexStorageName,
		Storages::CStorageManager* pStorageManager, 
		Containers::CFindResultCollection* pSearchResult,
		CSearchResultFiller::TResultsTextsNumbers* pResultsTextsNumbers,
		SS::Interface::Core::NdxSE::IParametrizedQuery* pPrmQueryResult,
		SS::Core::Types::IndexationSearch::TSearchParams* pSearchParameter, 
		SS::Interface::Core::NdxSE::CNdxSearchSettings* pSearchSettings);
	virtual ~CSearchFactory(void);

	///создает логику вычислений формулы
	Logics::CCompositeLogic* CreateEquationLogic(void);
	///создает контейнер результатов
	CResultsContainer* CreateResultsContainer(void);
	///создает проверщик результатов
	CResultsChecker* CreateResultsChecker(unsigned int uiValidResultsDepth);
	///создает аргумент формулы результатов
	CResultArgument* CreateResultArgument(unsigned int uiArgumentID, float fArgumentWeight, unsigned int uiArgumentDepth);
	///создает вычислитель
	CEvaluator* CreateEvaluator(void);
	///создает вычислительную формулу
	CBlockEquation* CreateCalcEquation(void);

	///возвращает конвертор результатов в линейные
	CSearchResultFiller* GetConvertorToLinearResults(void){return m_pConvertorToLinearResults;};
	///возвращает наполнитель формул (он же конвертор запроса)
	CESFiller* GetESFiller(void){return m_pESFillerFromQuery;};

	///возвращает текущие поисковые параметры
	SSearchSettings* GetSearchSettings(void){return &m_SearchSettings;};

protected:
	///текущие поисковые параметры
	SSearchSettings m_SearchSettings;
	
	///конвертор результатов в линейные
	CSearchResultFiller* m_pConvertorToLinearResults;
	///наполнитель формул (он же конвертор запроса)
	CESFillerFromQuery* m_pESFillerFromQuery;
	///менеджер логических хранилищ индексной базы
	Storages::CStorageManager* m_pStorageManager;
};

//--------------------------------------------------------------------//

}
}
}
}
}