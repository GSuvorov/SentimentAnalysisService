#pragma once

#include ".\r_eval.h"
#include ".\prx_union.h"

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

///вычислитель близости расположения слов в тексте обладает 
///коллекцией слов с позициями
class CProximityEvaluator_Text : public CRelevanceEvaluator
{
public:
	CProximityEvaluator_Text(unsigned int uiEvalID, bool bCheckSequence, Containers::CContainersFactory* pContainersFactory);
	virtual ~CProximityEvaluator_Text(void);

	///добавляет и  возвращает элемент дочерней коллекции по идентификатору
	Containers::CWordInSequence* AddPosition(unsigned char ucPositionID, unsigned char ucSequenceID, unsigned char ucPosInSequence, SS::Interface::Core::NdxSE::IWord* pWord);

	///вычисляет вес по близости и порядку расположения индексов в предложении
	void Evaluate(unsigned int uiEvalID);

	///отображает позиции текущего интервала
	void ViewIntervalPositions(void);

protected:
	typedef vector<pair<unsigned int, unsigned int> > TEvaledIntervalCollection;

	///возвращает вес интервала
	unsigned int GetIntervalWeight(Containers::CInterval* pInterval);
	///создает интервал результата
	Containers::CInterval* CreateInterval(unsigned int uiHeadPos, unsigned int uiTailPos);

	//добавляет позицию в интервал
	bool ReplaceIntervalPosition(Containers::CWordInSequence* pCommonPosition,
		Containers::TIntervalPositions::iterator* pOldPosition, unsigned int uiPopedPosition);
	///обновляет вспомогательный интервал
	//void UpdateTempInterval(unsigned int uiHeadPos, unsigned int uiTailPos);
	///готовит вычисления 
	bool PrepareEvaluation(unsigned int uiEvalID);
	bool PreparePositions(unsigned int uiEvalID, unsigned int uiIntervalIndex);
	///вычисляет интервалы
	void EvaluateIntervals(void);
	///вычисляет интервалы дял случая с одни словом
	bool SingleChildEvaluation(void);

	///колекция позиций интервала
	SS::Core::NdxSE::NdxProcessor::Containers::TIntervalPositions m_IntervalPositions;
	///коллекция где временно хранятся интервалы, в гранницах которых будет вычислятся близость
	TEvaledIntervalCollection m_EvaledIntervalCollection;
	///коллекция объединений слов
	vector<Containers::CWordUnion*> m_WordUnionCollection;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}
