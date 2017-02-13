#pragma once

#pragma once

#include ".\collection_containers.h"
#include ".\int_containers.h"
#include ".\prx_pos_containers.h"
#include ".\arg_res_int.h"
#include "..\ASDInterface\INDXQuery.h"

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

///коллекция последовательностей, с возможностью их проверки
class CSequences
{
public:
	typedef vector<SS::Core::NdxSE::NdxProcessor::Containers::TIntervalPositions::iterator> TSequence; 
	typedef vector<TSequence*> TSequenceCollection; 

	CSequences(void);
	virtual ~CSequences(void);
	
	///добавляет позицию в последовательность !добавление последовательно!
	void AddPosition(unsigned char ucSequenceID, unsigned char ucPosInSequence);
	///проверяет коллекцию позиций интервала на удовлетворение последовательностям
	bool CheckOnSequences(Containers::TIntervalPositions::iterator* pitIntervalPos);
	///проверяет нарушает ли данная позиция последовательности в коллекции позиций интервала
	bool IsSequencesBreaked(Containers::SIntervalPos* pIntervalPos);
	///обновляет коллекцию последовательностей
	void UpdateSequences(Containers::SIntervalPos* pIntervalPos, Containers::TIntervalPositions::iterator itIntervalPos);
	///очищает последователности
	void Clear(void);
	///инит последователности
	void Init(void);

	///получение коллекции для инициализации итераторов
	SS::Core::NdxSE::NdxProcessor::Containers::TIntervalPositions& GetNullCollection()
	{
		return m_NullCollection;
	}

protected:
	///получает последовательность для данной позиции
	CSequences::TSequence* GetSequence(Containers::SIntervalPos* pIntervalPos);

	///коллекция требуемых в интервале последовательностей
	TSequenceCollection m_SequenceCollection;
	///коллекция для инициализации итераторов
	SS::Core::NdxSE::NdxProcessor::Containers::TIntervalPositions m_NullCollection;
};

//--------------------------------------------------------------------//

class CRelevanceEvaluators;

///вычислитель релевантности по расположению слов в документе
///содержит коллекцию слов, а также требования по последоватльности их расположения
class CRelevanceEvaluator : 
	public SS::Core::NdxSE::NdxProcessor::Containers::CMasterCollection<SS::Core::NdxSE::NdxProcessor::Containers::CWordInSequence>,
	public CFactoryClient<SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory>
{
public:
	CRelevanceEvaluator(unsigned int uiEvalID, bool bCheckSequence, Containers::CContainersFactory* pContainersFactory);
	virtual ~CRelevanceEvaluator(void);

	///добавляет и  возвращает элемент дочерней коллекции по идентификатору
	virtual Containers::CWordInSequence* AddPosition(unsigned char ucPositionID, unsigned char ucSequenceID, unsigned char ucPosInSequence, SS::Interface::Core::NdxSE::IWord* pWord);
	///добавляет позицию к эталонным предложениям
	void AddArgumentPosition(CComplexArgument* pComplexArgument, Containers::CCoordinateNode_4s* pCoordinateNode);
	///вычисляет вес по близости и порядку расположения индексов в предложении
	virtual void Evaluate(unsigned int uiEvalID) = 0;
	///устанавлваиет базовую часть идентификатора документа
	void SetDocumentBasisIndex(unsigned int uiValue){m_uiDocumentBasisIndex=uiValue;};
	///устанавлваиет владельца вычислетеля для добавления результатов
	void SetOwner(CRelevanceEvaluators* pRelevanceEvaluatorsOwner){m_pRelevanceEvaluatorsOwner=pRelevanceEvaluatorsOwner;};

	///пороговый вес "хороших" интервалов
	static const unsigned int m_uiGoodIntervalLimitWeight=80;
	///максимальное количество хороших интервалов
	static const unsigned int m_uiGoodIntervalsLimitNumber=20;
	///максимально валидная длина интервала
	static const unsigned int m_uiMaxIntervalLength=128;

protected:
	typedef map<unsigned int, SS::Core::NdxSE::NdxProcessor::Containers::CWordInSequence*> TSortedChildCollection;

	///возвращает вес интервала
	virtual unsigned int GetIntervalWeight(Containers::CInterval* pInterval){return 0;};
	///создает интервал результата
	virtual Containers::CInterval* CreateInterval(unsigned int uiHeadPos, unsigned int uiTailPos) = 0;
	///добавляет интервал в коллекцию
	void AddInterval(unsigned int uiHeadPos, unsigned int uiTailPos);

	///возвращает текущее количество слов участвующих в вычислении
	unsigned int GetEvaluatedChildsNumber(void);
	///сбрасывает текущее количество слов участвующих в вычислении
	void ResetEvaluatedChildsNumber(void){m_uiEvaluatedChildsNumber=EMPTY_VALUE;};

	///коллекция требуемых в интервале последовательностей
	CSequences m_SequenceCollection;
	///коллекция где временно хранятся отсортированные CWord
	TSortedChildCollection m_SortedChildCollection;
	///текущее количество слов участвующих в вычислении
	unsigned int m_uiEvaluatedChildsNumber;
	///базовая часть идентификатора документа
	unsigned int m_uiDocumentBasisIndex;
	///владелец вычислетелей
	CRelevanceEvaluators* m_pRelevanceEvaluatorsOwner;

	///флаг учета последовательностей
	const bool m_bCheckSequence;

	///текущее количество хороших интервалов
	unsigned int m_uiGoodIntervalsNumber;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}
