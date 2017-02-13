#pragma once
#include ".\prx_pos_containers.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

//--------------------------------------------------------------------//

///объединение слов. содержит несколько слов 
///не в все слова должны присутствоватьв результате
///присутствующие слова должны находится максимально близко друг к другу
class CWordUnion : public CWordInSequence
{
public:
	CWordUnion(unsigned char ucPositionID, CContainersFactory* pContainersFactory, unsigned char ucSequenceID, unsigned char ucPosInSequence);
	virtual ~CWordUnion(void);

	///добавляет слово в объединение
	void AddWord(CWordInSequence* pWord);
	///добавляет позицию к эталонным предложениям
	void AddArgumentPosition(SS::Core::NdxSE::NdxProcessor::Extracting::CComplexArgument* pComplexArgument, 
		CCoordinateNode_4s* pCoordinateNode);
	//создает общую коллекцию позиций для коллекции аргументов в заданных границах
	void MakeCommonPositions(unsigned int uiEvalSentenceID, 
		unsigned int uiLeftBound = EMPTY_VALUE, unsigned int uiRightBound = EMPTY_VALUE);
	///сбрасывает позиции и контайнеры индексов
	void Clear(void);
	///очищает собранные позиции
	void ClearPositions(void);
	//возвращает число по которому будет сортироваться данная CWord
	unsigned int PrepareMakeCommonPositions(unsigned int uiEvalSentenceID);

	///возвращает вес объединения
	float GetUnionWeight(unsigned int* puiHeadPos, unsigned int* puiTailPos);

	///возвращает вес объединения
	unsigned int GetWordsCount(void){return (unsigned int)(m_GrandWordCollection.size()+m_SimpleWordCollection.size());};

protected:
	typedef std::list<CWordInSequence*> TWordCollection;

	///добавляет индекс в эталонную позицию
	void AddIndex(unsigned int uiIndex){};

	///коллекция слов объединения, которые обязательно должны присутствовать в результатах
	TWordCollection m_GrandWordCollection;
	///коллекция слов объединения, которые могут не присутствовать в результатах
	TWordCollection m_SimpleWordCollection;

	///предельная число "лишних" слов в объединении
	const unsigned int m_uiDirtyWordsCount;
	///необходимый процентый порог от общего веса слов объединения
	const unsigned int m_uiEvalLimitWeightPart;
	///коэффициент веса по близости
	const unsigned int m_uiDistanceWeightKoef;
	///коэффициент общего веса слов объединения 
	const unsigned int m_uiWordsWeightKoef;
	///коэффициент общего веса объедиения
	const unsigned int m_uiUnionWeightKoef;
	///коэффициент общего веса объедиения
	const unsigned int m_uiGrandWordWeightLimit;
};

//--------------------------------------------------------------------//

}
}
}
}
}


