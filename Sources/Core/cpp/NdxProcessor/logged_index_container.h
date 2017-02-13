#pragma once

#include ".\console.h"
#include ".\const.h"

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

class CIndexInTextInfo
{
public:
    typedef set<unsigned int> TPositionsCollection;

	CIndexInTextInfo(void)
		:m_uiTextIndex(EMPTY_VALUE), m_pPositionsCollection(NULL){};
	~CIndexInTextInfo(void){if(m_pPositionsCollection){ delete m_pPositionsCollection; m_pPositionsCollection=NULL;}};
	
	///возвращает индекс текста
	unsigned int GetTextIndex(void){return m_uiTextIndex;};
	///устанавливает индекс текста
	void SetTextIndex(unsigned int uiTextIndex){m_uiTextIndex=uiTextIndex;};

	///добавляет позицию текста
	void AddPosition(unsigned int uiPosition)
	{
		if(!m_pPositionsCollection){
			m_pPositionsCollection=new vector<unsigned int>();
		}
		m_pPositionsCollection->push_back(uiPosition);
	};
	
	///ковнертирует позиции в другое хранилище
	void ConvertPosition(TPositionsCollection* pPositionsCollection)
	{
		if(!pPositionsCollection) return;

		for(unsigned int i=0; i<m_pPositionsCollection->size(); i++){
			pPositionsCollection->insert(m_pPositionsCollection->at(i));
		}
	}

protected:
	///индекс текста
	unsigned int m_uiTextIndex;
	///коллекция позиций в тексте
	vector<unsigned int>* m_pPositionsCollection;
};

//--------------------------------------------------------------------//

///контайнер залогированных индексов с позициями
class CLoggedIndexContainer
{
public:
    typedef vector<CIndexInTextInfo*> TIndexInTextInfoCollection;
    typedef map<unsigned int, TIndexInTextInfoCollection*> TLoggedIndexesCollection;
	typedef vector<CIndexInTextInfo::TPositionsCollection*> TTextCollection;

	CLoggedIndexContainer(void);
	~CLoggedIndexContainer(void);
	
	///инициализирует коллекцию по лог-файлу
	void Init(wstring* pwsLogFilePath);
	
	///переходит к первому индексу
	bool MoveFirstIndex(void);
	///переходит к очередному индексу
	bool MoveNextIndex(void);
	///возвращает текущий индекс 
	unsigned int GetCurrentIndex(void){return (m_itCurrentIndex==m_NullCollection.end())?SS::Core::NdxSE::NdxProcessor::EMPTY_VALUE:m_itCurrentIndex->first;};
	///проверяет координаты текущего индекса
	void CurrentIndexCheckCoordinates(unsigned int uiTextIndex, unsigned int uiPosition);

	///текущий базовый номер текстов
	void SetCurrBasisTextsNumber(unsigned int uiCurrBasisTextsNumber){m_uiCurrBasisTextsNumber=uiCurrBasisTextsNumber;};

protected:
	///добавляет инфу по индексу
	void AddIndexInfo(const wchar_t* pwszIndexInfo);
	///заполняет инфу по индекс контайнеры для проверки
	void FillIndexInfoForChecking(TLoggedIndexesCollection::iterator itCurrentIndex);
	///кладет в лог результаты проверки индекса
	void LogCheckResults(void);
	///очищает все коллекции
	void Clear(void);

	///коллекция индексов с координатами
	TLoggedIndexesCollection m_LoggedIndexesCollection;
	///текущий индекс
	TLoggedIndexesCollection::iterator m_itCurrentIndex;
	///коллекция для инициализации итераторов
	TLoggedIndexesCollection m_NullCollection;

	///коллекция текстов с позициями дл проверки
	TTextCollection m_CheckingTextCollection;
	///коллекция позиций которых нет в базе
	vector<unsigned int> m_LostPositions;
	///коллекция позиций которые есть в базе, но их не было при индексации
	vector<unsigned int> m_NotCorrectPositions;

	///лог-файл результатов проверки
	FILE* m_pCheckResultLog;

	///текущий базовый номер текстов
	unsigned int m_uiCurrBasisTextsNumber;

};

//--------------------------------------------------------------------//

}
}
}
}
}