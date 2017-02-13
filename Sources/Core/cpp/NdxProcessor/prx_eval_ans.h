#pragma once

#include ".\r_eval.h"

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

///вычислитель интервалов с учетом слов ответа
class CProximityEvaluator_Answer : public CRelevanceEvaluator
{
public:
	CProximityEvaluator_Answer(unsigned int uiEvaluatorID, Containers::CContainersFactory* pContainersFactory);
	virtual ~CProximityEvaluator_Answer(void);

	///добавляет и  возвращает элемент дочерней коллекции по идентификатору
	Containers::CWordInSequence* AddPosition(unsigned char ucPositionID, unsigned char ucSequenceID, unsigned char ucPosInSequence, SS::Interface::Core::NdxSE::IWord* pWord);
	///добавляет позицию к эталонным предложениям
	void AddArgumentPosition(CComplexArgument* pComplexArgument, Containers::CCoordinateNode_4s* pCoordinateNode);

	///вычисляет вес по близости и порядку расположения индексов в предложении
	void Evaluate(unsigned int uiEvalID);

protected:
	typedef vector<Containers::CWordWithStatistic*> TWordsCollection;
	typedef map<unsigned int, Containers::CWordWithStatistic*> TSortedWordsCollection;

	///сбрасывает веса слов
	void ResetWordsWeights(void);
	///вычисляет интервалы с учетом слов ответа
	bool EvaluateWithAnswers(unsigned int uiEvalID);
	///готовит слова ответов к вычислениям
	bool PrepareAnswersWords(unsigned int uiEvalID);
	///готовит слова к вычислениям
	bool PrepareCommonWords(unsigned int uiEvalID);
	///вычисляет позиции слов ответа, пока возможно
	bool EvaluateNextPositions(unsigned int uiEvalID, unsigned int uiLimitBound);
	///возвращает центральную позицию результата
	unsigned int GetResultCenterPosition(void);
	///возвращает количество частых слов текущего запроса
	unsigned int GetRarityWordsCount(void);

	///создает интервал результата
	Containers::CInterval* CreateInterval(unsigned int uiHeadPos, unsigned int uiTailPos);
	///возвращает вес интервала
	unsigned int GetIntervalWeight(Containers::CInterval* pInterval);

	///позиции слов ответов
	TWordsCollection m_AnswerWords;
	///коллекция слов текущего результата
	TSortedWordsCollection m_CurrentResultWords;
	///коллекция слов по рангу
	TSortedWordsCollection m_WordsByRangeID;
	
	///количество частых слов текущего запроса
	unsigned int m_uiRarityWordsCount;
	///количество частых слов в текущем документе
	unsigned int m_uiCurrDocumentRarityWordsCount;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}
