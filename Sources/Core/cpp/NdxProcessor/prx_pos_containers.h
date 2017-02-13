#pragma once
#include ".\cnt_factory.h"
#include ".\index_container_nodes_s.h"
#include ".\arg_complex.h"
#include ".\logged_index_container.h"

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

///позиция с колекциями индексов
class CWord : public CFactoryClient<CContainersFactory>
{
public:
	typedef set<unsigned int> TCommonPositions;
	typedef map<unsigned int, CCoordinateNode_4s*> TIndexPositionsCollection;
	typedef list<CPosCnt*> TIndexPositionsList;

	CWord(unsigned char ucPositionID, CContainersFactory* pContainersFactory, const wchar_t* wszWord, float fWordWeight);
	virtual ~CWord(void);

	///возвращает идентификатор позиции
	unsigned char GetID(void){return m_ucPositionID;};
	///строка слова
	const wchar_t*  GetString(void){return m_wsWord.c_str();};
	///возвращает вес слова
	virtual float GetWeight(void){return m_fWordWeight;};

	///добавляет индекс в эталонную позицию
	virtual void AddIndex(unsigned int uiIndex);
	///добавляет позицию к эталонным предложениям
	virtual void AddArgumentPosition(SS::Core::NdxSE::NdxProcessor::Extracting::CComplexArgument* pComplexArgument, 
		CCoordinateNode_4s* pCoordinateNode);
	//создает общую коллекцию позиций для коллекции аргументов в заданных границах
	virtual void MakeCommonPositions(unsigned int uiEvalSentenceID, 
		unsigned int uiLeftBound = EMPTY_VALUE, unsigned int uiRightBound = EMPTY_VALUE);
	//создает общую коллекцию позиций для коллекции аргументов в заданных границах
	void MakePositions(unsigned int uiEvalSentenceID, TCommonPositions* pResultPositions,
		unsigned int uiLeftBound = EMPTY_VALUE, unsigned int uiRightBound = EMPTY_VALUE);
	///сбрасывает позиции и контайнеры индексов
	virtual void Clear(void);
	///очищает собранные позиции
	virtual void ClearPositions(void);
	///возвращает коллекцию текущий позиций эталона
	TCommonPositions* GetCurrentPositions(void){return &m_CurrentPositions;};
	///выбрасывает из первый элемент из коллекции позиций
	virtual unsigned int CurrentPositionsPopFront(void);
	///выбрасывает из первый элемент из коллекции позиций
	unsigned int GetNearPosition(unsigned int uiCenterPosition);

	///подготавливает создания коллеции общих позиций
	//возвращает число по которому будет сортироваться данная CWord
	virtual unsigned int PrepareMakeCommonPositions(unsigned int uiEvalSentenceID);
	//возвращает коллекцию индексов слова
	TIndexPositionsCollection* GetIndexesCollection(void){return &m_IndexCollection;};
	///устанавливает позицию находящаяся в текущей обработке
	void SetCurrentEvalPosition(unsigned int uiValue){m_uiCurrentEvalPosition=uiValue;};

	///возвращает коллекцию текущий позиций эталона
	void View(void);

	///контайнер выборки залогированных индексов с координатами
	///используется при проведении верификационного тестирования индексов
	static CLoggedIndexContainer* m_pLoggedIndexContainer;

protected:
	///убирает одинаковые контайнеры из m_IndexPositionsCollection
	void RemoveEqualPosConataners(void);
	///наполняет m_IndexPositionsCollection
	bool MakeIndexPositionsCollection(unsigned int uiEvalID);
	///вычисляет размерный коэффициент
	unsigned int EvaluateRangeId(void);

	///идентификатор позиции
	const unsigned char m_ucPositionID;
	///строка слова
	const wstring m_wsWord;
	///вес слова
	float m_fWordWeight;

	///коллекция индексов с позициями
	TIndexPositionsCollection m_IndexCollection;
	///коллекция текущий позиций эталона
	TCommonPositions m_CurrentPositions;
	///коллекция контайнерров индексов из которых будет формироваться общие позиции
	TIndexPositionsList m_IndexPositionsCollection;
	///позиция находящаяся в текущей обработке
	unsigned int m_uiCurrentEvalPosition;
};

//--------------------------------------------------------------------//

///позиция в последовательности
class CWordInSequence : public CWord
{
public:
	CWordInSequence(unsigned char ucPositionID, CContainersFactory* pContainersFactory, const wchar_t* wszWord, float fWordWeight, unsigned char ucSequenceID, unsigned char ucPosInSequence);
	virtual ~CWordInSequence(void);

	///возвращает идентификатор последовательности в которую входит позиция
	unsigned char GetSequenceID(void){return m_ucSequenceID;};
	///возвращает идентификатор позиции в последовательности, в которую входит позиция
	unsigned char GetPosInSequence(void){return m_ucPosInSequence;};
	///вовзвращает флаг участия позиции в вычисления
	bool IsEvaluated(void){return m_bEvaluated;};
	///устанавливает флаг участия позиции в вычисления
	void SetEvaluated(bool bValue){m_bEvaluated=bValue;};
	///	Устанавливает флаг поисковой необязательности
	void SetObligatory(bool const bOblitatory){m_Obligatory = bOblitatory;};
	///	Возвращает флаг поисковой необязательности
	bool GetObligatory(void){return m_Obligatory;};

	///отображает информацию об объекте
	void View(void);

protected:
	///идентификатор последовательности, в которую входит позиция
	const unsigned char m_ucSequenceID;
	///идентификатор позиции в последовательности, в которую входит позиция
	const unsigned char m_ucPosInSequence;
	///флаг участия позиции в вычисления
	bool m_bEvaluated;
	///флаг поисковой необязательности
	bool m_Obligatory;
};


//--------------------------------------------------------------------//

///эталонное слово с возможностью получения статистических характеристик
class CWordWithStatistic : public CWordInSequence
{
public:
	CWordWithStatistic(unsigned char ucPositionID, CContainersFactory* pContainersFactory, const wchar_t* wszWord, float fWordWeight, bool bAnswerWord);

	///добавляет индекс
	void AddIndex(unsigned int uiIndex);

	///возвращает частотный вес слова в коллекции докуметов
	float GetAbsFrequencyWeight(void){return m_fAbsFrequencyWeight;};
	///возвращает вес слова
	float GetWeight(void);
	///сбрасывает вес слова
	void ResetWeight(void){m_fTotalWordWeight=EmptyWeight;};
	///вычисляет вес позиции в данном документе
	void EvaluateWeight(unsigned int uiEvalID);
	///возвращает флаг слова ответа
	bool IsAnswerWord(void){return m_bAnswerWord;};

	///нормирующий коэффициент текущего документа
	static unsigned int m_uiCurrentDocumentNormKoef;
	///количество документов в базе
	static unsigned int m_uiTotalDocumentsNumber;
	///текущий вычислительный идентификатор
	static unsigned int m_uiCurrentEvalID;

	///пустой вес слова
	const static int EmptyWeight=-1;

protected:
	///вычисляет IDF индекса
	float CalculateIndexIDF(unsigned int uiIndex, unsigned int uiTotalDocumentsNumber);

	///общий вес слова
	float m_fTotalWordWeight;
	///частотный вес слова в коллекции докуметов
	float m_fAbsFrequencyWeight;
	///флаг слова ответа
	bool m_bAnswerWord;

	///коэффициент важности лингвитсического веса слов
	const float m_fWordsLingvoWeightKoef;
};

//--------------------------------------------------------------------//

}
}
}
}
}
