#pragma once

#include ".\r_eval.h"
#include ".\tbl_containers.h"

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
class CTableTournament;

///контайнер слов строки
class CRowWords
{
public:
	///	ctr
	CRowWords(unsigned int uiMaxWordsCount);
	///	dtr
	~CRowWords(void);
	///выставляет флаг присутствия слова
	void SetWordID(unsigned int uiWordID, unsigned char ucCellID);
	///проверяет все ли слова выставлены
	bool IsFull(void){ return (all_words.size() == m_uiMaxWordsCount) ? (true) : (false); };
	///сбрасывает содержимое 
	void Reset(void){ all_words.clear(); words_by_cell.clear(); };
	///возвращает максимальное количество слов
	unsigned int	GetMaxWordsNumber(void){return m_uiMaxWordsCount;}
	///возвращает текущее количество выставленных слов
	unsigned int	GetCurrentWordsCount(void){return static_cast<unsigned int>(all_words.size());}
	///возвращает текущее количество ячеек
	unsigned int	GetCellsCount(void){return (unsigned int)words_by_cell.size();}
	///	возвращает количество слов в заданной ячейке
	unsigned int	GetCellWordCount(unsigned int const _id_cell);
	///возвращает количество слов в обоих строках, повторяющиееся слова не учитываются
	static unsigned int GetUniqWordsCount(CRowWords* pRowWords1, CRowWords* pRowWords2);

	///	Определяет, имеется ли кросс-релевантность для заданной строки (параметр _row)
	/**
	"Кросс-релевантность" ("кросс" в смысле "крест") наблюдается тогда когда
	объединение слов строки (параметр _Row) 
	и слов шапки (параметр _CapRow) некоторого столбца,
	при отсутствии слов на пересечении строки и столбца, даёт полное множество слов.
	Если в качестве шапки и строки подать одну и ту же строку, кросс-релевантности не будет (вернётся false).
	\param[in]	_CapRow			Строка-шапка
	\param[in]	_Row			Рассматриваемая строка
	\return	true - релевантность у строки есть
	*/
	static bool	IsCrossRelevance(
		CRowWords*			_pCapRow, 
		CRowWords*			_pRow);

	
protected:
	///общее количество слова в массиве
	const unsigned int m_uiMaxWordsCount;
	///	хранилище уникальных значений unsigned int
	typedef std::set<unsigned int>						word_collection;
	///	карта "номер ячейки - коллекция слов в ячейке"
	typedef std::map<unsigned int, word_collection >	cell_collection;
	///	хранилище слов в соответствии с ячейками; pair<номер ячейки, коллекция слов в ячейке>
	cell_collection										words_by_cell;
	///	хранилище всех уникальных слов строки
	word_collection										all_words;
};

//--------------------------------------------------------------------//

///табличный турнир, при обработке результатов одной таблицы
///в него добавляются слова с идентификаторами строк и ячеек
///турнир проходит в несколько шагов, в следующий шаг переходят
///только те строки которые удовлетворяют условия турнира
class CTableTournament
{
public:
	typedef std::map<unsigned int, CRowWords*> TRowWordsCollection;

	CTableTournament(void);
	~CTableTournament(void);
	
	///запускает турнир
	void StartTournament(unsigned int uiTableIndex, unsigned int uiRaceWordsCount, unsigned int uiWordID);
	///добавляет новое слово для участия турнира, если вернет false турнир закончился неудачно
	bool NextRace(unsigned int uiWordID);
	///прерывает турнир
	void Reset(void);
	///успешный туринир
	bool IsSuccess(void){return (!m_RowWordsCollection.empty() && m_uiAddedRowsPerRace!=0)?true:false;};

	///добавляет идентификатором строки
	void AddTrcID(Containers::CTrcID TrcID);
	///возвращает коллекция строк со словами
	TRowWordsCollection* GetRowWordsCollection(void){return &m_RowWordsCollection;};
	///возвращает идентификатор таблицы
	unsigned int GetTableIndex(void){return m_uiTableIndex;};
	///возвращает строку шапки таблицы
	CRowWords* GetCapRow(void);

protected:
	
	///идентификатор таблицы
	unsigned int m_uiTableIndex;
	///коллекция строк со словами
	TRowWordsCollection m_RowWordsCollection;
	///строка шапки таблицы
	CRowWords* m_pCapRow;
	///идентификатор текущего слова
	unsigned int m_uiCurrentWordID;
	///количество слов участвующих в турнире
	unsigned int m_uiRaceWordsCount;
	///номер текущей итерации
	unsigned int m_uiCurrentRace;
	///номер текущей итерации
	unsigned int m_uiAddedRowsPerRace;
};

//--------------------------------------------------------------------//

///вычислитель близости расположения слов в таблицах документа
class CProximityEvaluator_Table : public CRelevanceEvaluator
{
public:
	CProximityEvaluator_Table(unsigned int uiEvalID, bool bCheckSequence, Containers::CContainersFactory* pContainersFactory);
	virtual ~CProximityEvaluator_Table(void);

	///добавляет и  возвращает элемент дочерней коллекции по идентификатору
	Containers::CWordInSequence* AddPosition(unsigned char ucPositionID, unsigned char ucSequenceID, unsigned char ucPosInSequence, SS::Interface::Core::NdxSE::IWord* pWord);
	///добавляет позицию к эталонным предложениям
	void AddArgumentPosition(CComplexArgument* pComplexArgument, Containers::CCoordinateNode_4s* pCoordinateNode);
	///вычисляет вес по близости и порядку расположения индексов в предложении
	void Evaluate(unsigned int uiEvalID);

protected:

	///создает результат
	Containers::CInterval* CreateInterval(unsigned int uiTableIndex, unsigned int uiRowIndex);
	///возвращает вес интервала
	unsigned int GetIntervalWeight(Containers::CInterval* pInterval);

	///готовит вычисления 
	bool PrepareEvaluation(unsigned int uiEvalID);
	///добавляет результаты
	void AddResults(void);

	///коллекция текущих выделенных индексов ячеек
	Containers::CWord::TCommonPositions m_CurrentCellIDs;
	///табличный турнир
	CTableTournament m_TableTournament;
	///текущая строка-результат
	CTableTournament::TRowWordsCollection::iterator m_itCurrentRowWords;
	///коллекция для инициализации итераторов
	CTableTournament::TRowWordsCollection m_NullCollection;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}
