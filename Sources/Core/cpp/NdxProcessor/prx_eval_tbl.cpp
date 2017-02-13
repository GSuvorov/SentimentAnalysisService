#include "StdAfx.h"
#include ".\prx_eval_tbl.h"
#include <iterator>

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

using namespace SS::Interface::Core::CommonContainers;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;
using namespace SS::Core::NdxSE::NdxProcessor::Extracting;

//--------------------------------------------------------------------//

CRowWords::CRowWords(unsigned int uiMaxWordsCount)
:m_uiMaxWordsCount(uiMaxWordsCount)
{
	Reset();
}

CRowWords::~CRowWords(void)
{
}

void CRowWords::SetWordID(unsigned int uiWordID, unsigned char ucCellID)
{
	//	проверка правильности значения
	if (uiWordID >= m_uiMaxWordsCount) return;
	//	вставляем слово и ячейку в нужные контейнеры
	words_by_cell[ucCellID].insert(uiWordID);
	all_words.insert(uiWordID);
}

unsigned int	CRowWords::GetCellWordCount(unsigned int const _id_cell)
{
	//ToConsole(L"PRX_TBL:: GetCellWordCount");
	//	есть ли заданная ячейка?
	cell_collection::iterator		itf = words_by_cell.find(_id_cell);
	//	если такая ячейка не найдена то возвращаем 0
	if (itf == words_by_cell.end()) return 0;
	//	ячейка найдена, возвращаем количество слов в ней
	return static_cast<unsigned int>(itf->second.size());
}

unsigned int CRowWords::GetUniqWordsCount(CRowWords* pRowWords1, CRowWords* pRowWords2)
{
	if(!pRowWords1 && !pRowWords2) return 0;
	if(!pRowWords1) return pRowWords2->GetCurrentWordsCount();
	if(!pRowWords2) return pRowWords1->GetCurrentWordsCount();
	
	if(pRowWords1->GetMaxWordsNumber()!=pRowWords2->GetMaxWordsNumber()){
		TO_CONSOLE(L"MaxWordsNumber not equal!!!");
		return 0;
	}

	//	уже имеем столько слов сколько в 1-й строке
	unsigned int uiInc = static_cast<unsigned int>(pRowWords1->all_words.size());
	//	проходим по 2-й строке, ищем её элементы в 1-й. Если не нашли то увеличиваем счётчик.
	for(word_collection::iterator	b = pRowWords2->all_words.begin(), e = pRowWords2->all_words.end(); b != e; ++b)
		//	если элемент правого массива не нашёлся в левом, значит он уникальный, увеличиваем счётчик
		if (pRowWords1->all_words.find(*b) == pRowWords1->all_words.end()) ++uiInc;

	return uiInc;
}


bool	CRowWords::IsCrossRelevance(
	CRowWords*			_pCapRow, 
	CRowWords*			_pRow)
{
	//ToConsole(L"PRX_TBL: CRowWords::IsCrossRelevance\n");
	//	если нет шапки или строки то релевантности не будет
	if ((_pCapRow == NULL) || (_pRow == NULL)) return false;
	//	максимальные количества слов в шапке и в строке должны быть одинаковыми
	if (_pCapRow->GetMaxWordsNumber() != _pRow->GetMaxWordsNumber()) return false;
	//	максимальные количества слов обеих строк не должны быть нулём
	if (_pCapRow->GetMaxWordsNumber() == 0) return false;
	//	если строка и шапка совпадают то релевантности не будет
	if (_pCapRow == _pRow) return false;

	/*	отладочный вывод слов в ячейках шапки и строки
	ToConsole(L"\nPRX_TBL: CRowWords::IsCrossRelevance: выводим шапку\n");
	for (cell_collection::iterator	b = _pCapRow->words_by_cell.begin(), e = _pCapRow->words_by_cell.end();
		b != e;
		++b)
	{
		ToConsole(L"\nячейка: ", static_cast<unsigned int>(b->first));
		for (word_collection::iterator	b2 = b->second.begin(), e2 = b->second.end();
			b2 != e2;
			++b2)
		{
			ToConsole(L"\nслово в ячейке: ", static_cast<unsigned int>(*b2));
		}

	}

	ToConsole(L"\nPRX_TBL: CRowWords::IsCrossRelevance: выводим строку");
	for (cell_collection::iterator	b = _pRow->words_by_cell.begin(), e = _pRow->words_by_cell.end();
		b != e;
		++b)
	{
		ToConsole(L"\nячейка: ", static_cast<unsigned int>(b->first));
		for (word_collection::iterator	b2 = b->second.begin(), e2 = b->second.end();
			b2 != e2;
			++b2)
		{
			ToConsole(L"\nслово в ячейке: ", static_cast<unsigned int>(*b2));
		}
	}
	*/

	//	проходим по ячейкам шапки
	for (cell_collection::iterator it_cap_b = _pCapRow->words_by_cell.begin(), it_cap_e = _pCapRow->words_by_cell.end();
		it_cap_b != it_cap_e;
		++it_cap_b)
	{
		//	проверка на всякий случай
		if (it_cap_b->second.empty()) continue;

		//	если номер ячейки шапки есть в ячейках строки, переходим к следующей ячейке шапки
		if (_pRow->words_by_cell.find(it_cap_b->first) != _pRow->words_by_cell.end()) continue;

		//	Проверка на полноту объединения множества слов столбца в шапке и множества слов строки.
		//	Проверяем сначала сумму числа слов в шапке и числа слов в строке, это проще. 
		//	Если сумма уникальных слов в строке и шапке столбца меньше нужного количества слов, 
		//	условие кросс-релевантности не выполнилось, переходим к следующей ячейке шапки.
		if (it_cap_b->second.size()+			//	число слов в шапке столбца
			_pRow->GetCurrentWordsCount()		//	число слов в строке
			< _pCapRow->GetMaxWordsNumber())	//	размер полного множества
				continue;	//	переходим к следующей ячейке шапки

		//	проверяем полноту суммарного набора слов строки и шапки столбца
		word_collection	words_union;	//	объединённое множество слов из шапки столбца и строки

		//	объединяем слова строки и шапки в одно множество
		std::set_union(
			it_cap_b->second.begin(),	//	слова шапки столбца, начало
			it_cap_b->second.end(),		//	слова шапки столбца, конец
			_pRow->all_words.begin(),	//	слова строки, начало
			_pRow->all_words.end(),		//	слова строки, конец
			std::inserter(words_union, words_union.begin()));

		//	если получилось полное множество слов то релевантность есть
		if (words_union.size() == _pCapRow->GetMaxWordsNumber())
			return true;
	}

	return false;
}

//--------------------------------------------------------------------//

CTableTournament::CTableTournament(void) :
	m_uiTableIndex(EMPTY_VALUE), m_uiCurrentWordID(EMPTY_VALUE), 
	m_pCapRow(NULL), m_uiRaceWordsCount(EMPTY_VALUE), 
	m_uiCurrentRace(EMPTY_VALUE), m_uiAddedRowsPerRace(0)
{
}

CTableTournament::~CTableTournament(void)
{
	Reset();
}

void CTableTournament::Reset(void)
{
	//ToConsole(L"PRX_TBL: CTableTournament::Reset");
	m_uiTableIndex=EMPTY_VALUE;
	m_uiRaceWordsCount=EMPTY_VALUE;
	m_uiCurrentWordID=EMPTY_VALUE;
	m_uiCurrentRace=EMPTY_VALUE;
	m_uiAddedRowsPerRace=EMPTY_VALUE;
	m_pCapRow=NULL;

	TRowWordsCollection::iterator itRowWords=m_RowWordsCollection.begin();
	while(itRowWords!=m_RowWordsCollection.end()){
		delete itRowWords->second;
		++itRowWords;
	}
	m_RowWordsCollection.clear();
}

CRowWords* CTableTournament::GetCapRow(void)
{
	if(!m_pCapRow && !m_RowWordsCollection.empty()){
		if(m_RowWordsCollection.begin()->first==0)
			m_pCapRow=m_RowWordsCollection.begin()->second;
	}

	return m_pCapRow;
}

void CTableTournament::StartTournament(unsigned int uiTableIndex, unsigned int uiRaceWordsCount, unsigned int uiWordID)
{
	//ToConsole(L"PRX_TBL: CTableTournament::StartTournament");
	//ToConsole(L"CTableTournament::StartTournament", uiTableIndex);
	//ToConsole(L"CTableTournament::StartRace", uiWordID);

	Reset();
	//обновляем счетчики
	m_uiTableIndex=uiTableIndex;
	m_uiRaceWordsCount=uiRaceWordsCount;
	m_uiCurrentWordID=uiWordID;
	m_uiCurrentRace=0;
	m_uiAddedRowsPerRace=0;
}

bool CTableTournament::NextRace(unsigned int uiWordID)
{
	//ToConsole(L"CTableTournament::NextRace", uiWordID);
	//ToConsole(L"PRX_TBL: CTableTournament::NextRace");

	if(!m_uiAddedRowsPerRace){
		//если в предыдущем этапе не было добавленно ни одной строки
		//турнир прерывается
		Reset();
		return false;
	}else{
		//обновляем счетчики
		m_uiCurrentWordID=uiWordID;
		++m_uiCurrentRace;
		m_uiAddedRowsPerRace=0;
		return true;
	}
}

void CTableTournament::AddTrcID(Containers::CTrcID TrcID)
{
	//ToConsole(L"PRX_TBL: CTableTournament::AddTrcID");
	//!!!шапка таблицы, это строка с индексом 0
	//ToConsole(L"CTableTournament::AddTrcID");
	//TrcID.View();
	//ToConsole(L"RowID", TrcID.GetIDByType(CTrcID::etrcRow));

	bool stringExist = true;
	//получаем строку в находиться текущее слово
	TRowWordsCollection::iterator itRowWords=m_RowWordsCollection.find(TrcID.GetIDByType(CTrcID::etrcRow));
	if(itRowWords==m_RowWordsCollection.end()){
		if(TrcID.GetIDByType(CTrcID::etrcRow)==0 || !m_uiCurrentRace){
			//если добавляется шапка или имеем первый старт добавляем без проверки
			itRowWords=m_RowWordsCollection.insert(TRowWordsCollection::value_type(
				TrcID.GetIDByType(CTrcID::etrcRow), new CRowWords(m_uiRaceWordsCount))).first;
		}else{
			//для обычной строки выполняем проверки

			//если такой строки нет проверяем, чтобы оставшиейся слова были в шапке таблицы
			//шапка таблицы, это строка с индексом 0
			CRowWords* pCapRowWords=GetCapRow();
			if(pCapRowWords && pCapRowWords->GetCurrentWordsCount()==m_uiCurrentRace){
				//если шапка есть, все предыдущие играющие слова, должны быть в шапке
				itRowWords=m_RowWordsCollection.insert(TRowWordsCollection::value_type(
					TrcID.GetIDByType(CTrcID::etrcRow), new CRowWords(m_uiRaceWordsCount))).first;
			}else stringExist = false;
		}
	}

	//если такая строка есть добавляем туда идентификатор слова и ячейки
	if(stringExist){
		//проверяем все предыдущие слова должны быть или в данной строке или в шапке
		if(CRowWords::GetUniqWordsCount(GetCapRow(), itRowWords->second)==m_uiCurrentRace){
			itRowWords->second->SetWordID(m_uiCurrentRace, TrcID.GetIDByType(CTrcID::etrcCell));
			//строка добавлена, увеличиваем счетчик
			++m_uiAddedRowsPerRace;
		}
	}
}

//--------------------------------------------------------------------//

CProximityEvaluator_Table::CProximityEvaluator_Table(unsigned int uiEvalID, bool bCheckSequence, CContainersFactory* pContainersFactory)
:CRelevanceEvaluator(uiEvalID, bCheckSequence, pContainersFactory)
{
	m_itCurrentRowWords = m_NullCollection.end();
}

CProximityEvaluator_Table::~CProximityEvaluator_Table(void)
{
}

Containers::CInterval* CProximityEvaluator_Table::CreateInterval(unsigned int uiTableIndex, unsigned int uiRowIndex)
{
	//в данном вычислителе в результатом является
	//координата строки в таблице, поэтому в интервал заноситься
	//индекс таблицы и инлдекс строки в этой таблице
	return new CInterval(uiTableIndex, uiRowIndex);
}

unsigned int CProximityEvaluator_Table::GetIntervalWeight(Containers::CInterval* pInterval)
{
	if(ISNULL(pInterval)) return 0;
	unsigned int uiIntervalLength=pInterval->GetLength();

	//вес результата тем больше чем больше слов запроса находятся в одной ячейке строки таблицы, 
	//то есть определяется количестовм слов запроса, участвующих в расчете и
	//количеством ячееек строки, в которых они присутствуют
	
	if(m_itCurrentRowWords==m_NullCollection.end()) return 0;

	unsigned int uiEvaluatedWordsCount=GetEvaluatedChildsNumber();
	CRowWords* pCapRow=m_TableTournament.GetCapRow();

	if(m_itCurrentRowWords->first==0){
		//для шапки результат заносим только если в ней находятся все слова
		if(m_itCurrentRowWords->second->GetCurrentWordsCount()!=GetEvaluatedChildsNumber())
			return 0;
	}else{
		//если строка не шапка, проверяем что все слова запроса находятся
		//либо в строке либо в шапке
		if(CRowWords::GetUniqWordsCount(pCapRow, m_itCurrentRowWords->second)!=uiEvaluatedWordsCount)
			return 0;
		
		//если строка не шапка, а часть слов в шапке, 
		//определяем что они пересекаются в ячейке где нет слов запрос
		//если так добавляем релевантность

		//	Если есть кросс-релевантность, выдаём самую высокую релевантность
		if (CRowWords::IsCrossRelevance(pCapRow, m_itCurrentRowWords->second))
		{
			//ToConsole(L"PRX_TBL: CrossRelevance FOUND!!!!!!!!!");
			return 250;
		}
	}

	unsigned int uiCellsCount=(unsigned int)m_itCurrentRowWords->second->GetCellsCount();
	
	if(uiCellsCount==0) return 0;

	float fFillKoef=(float)uiCellsCount/(float)uiEvaluatedWordsCount;

	if(fFillKoef>=0.9)
		return 200;
	else
	if(fFillKoef>=0.7)
		return 180;
	else
	if(fFillKoef>=0.5)
		return 160;
	else
	if(fFillKoef>=0.3)
		return 140;
	else
	if(fFillKoef>0.0)
		return 120;
	else
		return 0;
}

bool CProximityEvaluator_Table::PrepareEvaluation(unsigned int uiEvalID)
{
	m_SortedChildCollection.clear();
	m_SequenceCollection.Init();

	if(m_ChildCollection.empty()){
		TO_CONSOLE(L"Common positions not exist");
		return false;
	}

	//ToConsole(L"uiEvalID", uiEvalID);

	TChildCollection::iterator itSP=m_ChildCollection.begin();
	unsigned int uiRangeID;
	do{
		if((*itSP) && (*itSP)->IsEvaluated()){
			//готовим позиции только для слов коорые участвуют в вычислениях
			(*itSP)->ClearPositions();

			uiRangeID=(*itSP)->PrepareMakeCommonPositions(uiEvalID);
			if(uiRangeID==0) return false;
				
			//чтобы не затереть уже добавленную с таким же рангом позицию, слегка меняем ранг
			while(m_SortedChildCollection.find(uiRangeID)!=m_SortedChildCollection.end()){
				++uiRangeID;
			}
			//добавляем в коллекцию
			m_SortedChildCollection.insert(TSortedChildCollection::value_type(uiRangeID, (*itSP)));
		}
	}while(++itSP!=m_ChildCollection.end());

	return !m_SortedChildCollection.empty();
}

void CProximityEvaluator_Table::Evaluate(unsigned int uiEvalID)
{
	unsigned char ucStep=0;
	try{
		bool bResult=false;
		ucStep=0;

		//ToConsole(L"//---------------------");
		//ToConsole(L"DocumentIndex", uiEvalID+m_uiDocumentBasisIndex);

		m_TableTournament.Reset();

		ucStep=1;
		//определяем лучшее слово и по нему создаем коллекцию масок
		START_TICK(L"CProximityEvaluator_Table::PrepareEvaluation");		
		bResult=PrepareEvaluation(uiEvalID);
		STOP_TICK(L"CProximityEvaluator_Table::PrepareEvaluation");		
		if(!bResult) return;
		
		//по первому наилучшему слову формрмируем коллекцию масок TRC-идентификаторов
		TSortedChildCollection::iterator itSSP=m_SortedChildCollection.begin();
		if(!itSSP->second) return;	
		START_TICK(L"MakeCommonPositions");
		itSSP->second->MakeCommonPositions(uiEvalID, EMPTY_VALUE, EMPTY_VALUE);
		STOP_TICK(L"MakeCommonPositions");
		//itSSP->second->View();
		
		ucStep=2;
		//последовательно, сравниваем табличные координаты слов с выделеными масками
		//слова должны находиться в одной таблице, в одной строке, 
		//и чем больше слов в одной ячейке тем лучше.
		CWord::TCommonPositions* pCommonPositions=itSSP->second->GetCurrentPositions();
		CWord::TCommonPositions::iterator itTRC=pCommonPositions->begin();
		while(itTRC!=pCommonPositions->end()){
			itSSP=m_SortedChildCollection.begin();

			//определяем граничные маски индексом таблицы
			CTrcID HeadTrcIDMask(*itTRC);
			HeadTrcIDMask.SetIDByType(CTrcID::etrcCell, 0);
			HeadTrcIDMask.SetIDByType(CTrcID::etrcRow, 0);
			//HeadTrcIDMask.View();

			CTrcID TailTrcIDMask(*itTRC);
			TailTrcIDMask.SetIDByType(CTrcID::etrcCell, 0xff);
			TailTrcIDMask.SetIDByType(CTrcID::etrcRow, 0xffff);
			//TailTrcIDMask.View();
		
			ucStep=3;
			
			//начинаем турнир
			m_TableTournament.StartTournament(HeadTrcIDMask.GetIDByType(CTrcID::etrcTable), GetEvaluatedChildsNumber(), itSSP->second->GetID());
			
			while(itTRC!=pCommonPositions->end()){
				CTrcID TempTrcID(*itTRC);
				if(HeadTrcIDMask.GetIDByType(CTrcID::etrcTable)!=TempTrcID.GetIDByType(CTrcID::etrcTable)) break;
				//добавляем все строки данной таблицы
				//TempTrcID.View();
				m_TableTournament.AddTrcID(TempTrcID);
				++itTRC;
			}
			ucStep=4;
			
			if(++itSSP!=m_SortedChildCollection.end()){
				//проходим по остальным словам
				do{
					if(itSSP->second){
						if(!m_TableTournament.NextRace(itSSP->second->GetID())){
							//если турнир продолжать нельзя прерываем вычисления
							break;
						}

						//заполянем TRC-идентификаторы с учетом граничных масок
						START_TICK(L"MakeCommonPositions");
						itSSP->second->MakeCommonPositions(uiEvalID, HeadTrcIDMask.GetTrcID(), TailTrcIDMask.GetTrcID());
						STOP_TICK(L"MakeCommonPositions");
						//itSSP->second->View();
						
						if(itSSP->second->GetCurrentPositions()->empty()){ 
							//если хотя бы у одного слова нет, нужных TRC-идентификаторов, прерываем вычисления
							m_TableTournament.Reset();
							break;
						}
						
						//заносим индексы ячеек в общую коллекцию
						CWord::TCommonPositions::iterator itTempTRC=itSSP->second->GetCurrentPositions()->begin();
						while(itTempTRC!=itSSP->second->GetCurrentPositions()->end()){
							m_TableTournament.AddTrcID(CTrcID(*itTempTRC));
							++itTempTRC;
						}
					}
				}while(++itSSP!=m_SortedChildCollection.end());
			}

			ucStep=5;

			//добавляем результаты
			AddResults();
		}
	
		ucStep=6;

		////отображаем результат
		//ViewIntervalCollection();
		
	}catch(...){
		wchar_t buf[256]=L"";
		wsprintf(buf, L"At step: %u", (unsigned int)ucStep);
		ERR_TO_CONSOLE(erTryCatch, buf);
		throw;
	}

}

void CProximityEvaluator_Table::AddResults(void)
{
	//если все прошло хорошо, добавляем результат
	if(m_TableTournament.IsSuccess()){
		m_itCurrentRowWords=m_TableTournament.GetRowWordsCollection()->begin();
		while(m_itCurrentRowWords!=m_TableTournament.GetRowWordsCollection()->end()){
			AddInterval(m_TableTournament.GetTableIndex(), m_itCurrentRowWords->first);
			++m_itCurrentRowWords;
		}
		m_itCurrentRowWords=m_NullCollection.end();
	}
}

Containers::CWordInSequence* CProximityEvaluator_Table::AddPosition(unsigned char ucPositionID, unsigned char ucSequenceID, unsigned char ucPosInSequence, SS::Interface::Core::NdxSE::IWord* pWord)
{
	if(!pWord) return NULL;

	if(pWord->GetObligatoryFlag()){
		//добавляем только обязательные аргументы
		return CRelevanceEvaluator::AddPosition(ucPositionID, ucSequenceID, ucPosInSequence, pWord);
	}
	return NULL;
}

void CProximityEvaluator_Table::AddArgumentPosition(CComplexArgument* pComplexArgument, CCoordinateNode_4s* pCoordinateNode)
{
	if(ISNULL(pComplexArgument)) return;
	if(pComplexArgument->GetAttributes().m_Obligatory != 0){
		//добавляем только обязательные аргументы
		CRelevanceEvaluator::AddArgumentPosition(pComplexArgument, pCoordinateNode);
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}
}
