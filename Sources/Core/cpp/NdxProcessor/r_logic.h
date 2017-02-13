#pragma once

#include ".\logic.h"
#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASDInterface\INdxQuery.h"
#include ".\cnt_factory.h"
#include ".\r_evals.h"
#include ".\stat_eval.h"

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
namespace Logics
{

//--------------------------------------------------------------------//

///логика подсчета релевантности результатов
class CRelevanceLogic : public CLogic 
{
public:
	CRelevanceLogic(void);
	virtual ~CRelevanceLogic(void);
	
	///устанавливает использование расчетов по статистике
	void UseStatistic(SS::Core::NdxSE::NdxProcessor::Storages::CIndexStatisticStorage* pIndexStorage, unsigned int uiTotalTextsNumber);
	///устанавливает использование расчетов по близости расположения с учетом статистики слов
	void UseStatisticProximity(SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory* pContainersFactory);
	///устанавливает использование расчетов по близости расположения
	void UseTextProximity(SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory* pContainersFactory, 
		bool bCheckSequence, unsigned int uiIntervalLimitWeight, unsigned int uiIntervalLimitNumber);
	///устанавливает использование расчетов по близости расположения
	void UseTableProximity(SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory* pContainersFactory, 
		unsigned int uiIntervalLimitWeight, unsigned int uiIntervalLimitNumber);
	///устанавливает использование расчетов по близости расположения
	void UseTextAndHeadersProximity(SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory* pContainersFactory, 
		Containers::CFindResultCollection* pFindResultCollection, Storages::CIndexStorageHeaders* pIndexStorageHeaders, 
		bool bCheckSequence, unsigned int uiIntervalLimitWeight, unsigned int uiIntervalLimitNumber);
	///устанавливает использование расчетов по близости расположения
	void UseTableAndHeadersProximity(SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory* pContainersFactory, 
		Containers::CFindResultCollection* pFindResultCollection, Storages::CIndexStorageHeaders* pIndexStorageHeaders, Storages::CIndexStorageTables* pIndexStorageTables, 
		bool bCheckSequence, unsigned int uiIntervalLimitWeight, unsigned int uiIntervalLimitNumber);

	///устанавливает запрос для инициализации вычислителей
	virtual void SetQuery(SS::Interface::Core::NdxSE::IParametrizedQuery* pPrmQueryResult);

	///инициализация логики перед вычислениями
	void Init(SS::Core::NdxSE::NdxProcessor::Extracting::CResultArgument* pResultArgument);
	///образует значение логики по значению аргумента
	SS::Core::NdxSE::NdxProcessor::Containers::CValue* CreateLogicValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue, 
		SS::Core::NdxSE::NdxProcessor::Extracting::CComplexArgument* pComplexArgument);
	///образует значение аргумента по значению логики
	void ToResultValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue, 
		SS::Core::NdxSE::NdxProcessor::Extracting::CResultArgument* pResultArgument);

protected:
	///выполняет операцию AND
	void MakeAnd(SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppChangedValue, 
		SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppValue){};
	///выполняет операцию OR
	void MakeOr(SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppChangedValue, 
		SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppValue){};

	///коллекция вычислителей релевантности близости расположения индексов
	CRelevanceEvaluators* m_pRelevanceEvaluators;
	///вычислитель статистической релевантности
	CStatisticEvaluator* m_pStatisticEvaluator;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}
