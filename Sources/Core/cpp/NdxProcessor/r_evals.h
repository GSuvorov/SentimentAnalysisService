#pragma once

#include ".\r_eval.h"
#include ".\res_containers.h"
#include ".\index_storage_hdr.h"
#include ".\index_storage_tbl.h"

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

///коллекция вычислителей близости расположения
class CRelevanceEvaluators : 
	public SS::Core::NdxSE::NdxProcessor::Containers::CMasterCollection<CRelevanceEvaluator>,
	public CFactoryClient<SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory>
{
public:
	CRelevanceEvaluators(Containers::CContainersFactory* pContainersFactory);
	CRelevanceEvaluators(Containers::CContainersFactory* pContainersFactory, 
		unsigned int uiIntervalLimitWeight, unsigned int uiIntervalLimitNumber);
	CRelevanceEvaluators(Containers::CContainersFactory* pContainersFactory, 
		bool bCheckSequence, unsigned int uiIntervalLimitWeight, unsigned int uiIntervalLimitNumber);
	CRelevanceEvaluators(Containers::CContainersFactory* pContainersFactory, 
		bool bCheckSequence, unsigned int uiIntervalLimitWeight, unsigned int uiIntervalLimitNumber,
		Containers::CFindResultCollection* pFindResultCollection, Storages::CIndexStorageHeaders* pIndexStorageHeaders);
	CRelevanceEvaluators(Containers::CContainersFactory* pContainersFactory, 
		bool bCheckSequence, unsigned int uiIntervalLimitWeight, unsigned int uiIntervalLimitNumber,
		Containers::CFindResultCollection* pFindResultCollection, 
		Storages::CIndexStorageHeaders* pIndexStorageHeaders, Storages::CIndexStorageTables* pIndexStorageTables);

	virtual ~CRelevanceEvaluators(void);
	
	///добавляет вычилситель в коллекцию
	virtual CRelevanceEvaluator* AddEvaluator(unsigned int uiEvaluatorID);
	///добавляет позиции аргумента в предложении
	void AddArgumentPosition(CComplexArgument* pComplexArgument, SS::Core::NdxSE::NdxProcessor::Containers::CCoordinateNode_4s* pCoordinateNode);
	///вычисляет вес по близости и порядку расположения индексов в предложении
	float Evaluate(unsigned int uiEvalID);
	///возвращает коллекцию вычисленных интервалов
	SS::Core::NdxSE::NdxProcessor::Containers::TIntervalCollection* GetBestIntervals(void);
	///заполняет коллекцию вычисленных интервалов, нулевые значения порогов означают их отсутствие
	void FillIntervals(CResultArgumentWithIntervals* pResultArgumentWithIntervals);
	///устанавлваиет базовую часть идентификатора документа
	void SetDocumentBasisIndex(unsigned int uiValue);

	///очищает коллекцию интервалов
	void ClearRangedIntervals(void);
	///добавляет интервал в общую коллекцию
	bool AddInterval(Containers::CInterval* pInterval);
	///возвращает текущее количество добавленных интервалов
	unsigned int GetAddedIntervalsCount(void){return (unsigned int)m_IntervalsByHeadTail.size();};

protected:
	typedef map<unsigned int, Containers::TIntervalCollection*> TRangedIntervalCollection;
	class CCompareInterval
	{
	public:
		unsigned int m_uiHeadPos;
		unsigned int m_uiTailPos;
		bool operator<(const CCompareInterval& Value) const
		{
			if(m_uiHeadPos<Value.m_uiHeadPos)
				return true;
			else if(m_uiHeadPos==Value.m_uiHeadPos){
				if(m_uiTailPos<Value.m_uiTailPos)
					return true;
				else if(m_uiTailPos==Value.m_uiTailPos)
					return false;
			}
			return false;
		}
	
	};
	typedef map<CCompareInterval, Containers::CInterval*> TIntervalsByHeadTail;

	///режимы создания вычислителей
	enum EEvalProductionMode{
		///вычислители близости в тексте
		epmTxtProximity,
		///вычислители близости в таблицах
		epmTblProximity,
		///вычислители близости в таблицах с учетом заголовков
		epmHdrAndTblProximity,
		///вычислители близости в тексте с учетом заголовков
		epmHdrAndTxtProximity,
		///вычислители близости в тексте с учетом статистики слов и слов ответов
		epmTxtStatProximity,
	};

	///добавляет коллекцию интервалов в ранжированую коллекцию
	void AddToRangedIntervals();

	///режим создания вычислителей
	const EEvalProductionMode m_EvalProductionMode;
	///коллекция вычисленных интервалов отсортированные по весам
	TRangedIntervalCollection m_RangedIntervals;
	///коллекция вычислямых интервалов по значения головы и хвоста, для определения похожих интервалов
	TIntervalsByHeadTail m_IntervalsByHeadTail;
	///флаг учета последовательностей
	const bool m_bCheckSequence;
	///пороговый вес интервалов
	unsigned int m_uiIntervalLimitWeight;
	///предельное количество интервалов, которое может извлекается для одного текста
	unsigned int m_uiIntervalLimitNumber;
	///коллекция результатов поиска
	Containers::CFindResultCollection* m_pFindResultCollection;
	///индексное хранилище заголовков
	Storages::CIndexStorageHeaders* m_pIndexStorageHeaders;
	///индексное хранилище таблиц
	Storages::CIndexStorageTables* m_pIndexStorageTables;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}
