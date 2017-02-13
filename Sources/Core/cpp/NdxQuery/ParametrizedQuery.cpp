#include "stdafx.h"
#include ".\ObjectInserter.h"
#include ".\ParametrizedQuery.h"
#include ".\Block.h"
#include ".\Sequence.h"
#include ".\Word.h"
#include ".\Variant.h"
#include <iterator>

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxQuery
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CParametrizedQuery::CParametrizedQuery(void)
:
m_pSettingsServer(NULL),
m_HasAnswers(FALSE),
m_HasUB(FALSE),
m_HasNonObligatoryWords(FALSE),
m_pSuperTemplate(new CSuperTemplate())
{}

CParametrizedQuery::~CParametrizedQuery(void)
{
	Clear();
}

void CParametrizedQuery::Init(
	SS::Interface::Core::CommonContainers::IQueryResult*	_pQuery,
	SS::Core::Types::IndexationSearch::TSearchParams*	_pSearchParams)
{
	
	if (_pQuery == NULL)
	{
		ToConsole(L"\n.\nERROR!!! pQuery is NULL\n.\n.");
		return;
	}

	if (_pSearchParams == NULL)
	{
		ToConsole(L"\n.\nERROR!!! pSearchParams is NULL\n.\n.");
		return;
	}

	//	указатель на сервер настроек
	if	(m_pSettingsServer == NULL)
	{
		m_pSettingsServer = (SS::Interface::Core::CommonServices::ISettingsServer*)GetLoadManager()->
			GetInterface(L"./core/GSServer.dll", CLSID_SettingsServer , IID_SettingsServer);
	}

	ToConsole(L"\n.\nPARAMETRIZED QUERY, INIT\n.\n.");
	////////////////////////////////////////////////////////////////////////////
	//Существует 3 режима разбора запроса
	//1. При настройке NdxProcessor::ForcedSearchMode = 7 или при 
	//_pSearchParams->m_bForceOrSearch = true: "всё по или", он же AllOr
	//2. При наличии хоть в одном индексе флага ответа - "Answer режим"
	//3. Стандартный режим
	////////////////////////////////////////////////////////////////////////////


	//
	//	Определяем, надо ли работать в ALL OR mode
	//

	//	настройка NdxProcessor::ForcedSearchMode
	bool	IsForcedSearchMode7 = (m_pSettingsServer->GetUInt(L"NdxProcessor::ForcedSearchMode") == 7);

	//	Когда бывает OR-режим:
	//	Если pSearchParams.m_bForceOrSearch = true
	//	или если настройка "NdxProcessor::ForcedSearchMode" = 7 
	bool	bIsSetAllOrMode = _pSearchParams->m_bForceOrSearch || IsForcedSearchMode7;

	//
	//	Определяем есть ли в запросе индексы с Answer флагами
	//

	bool	bPresentAnswerFlag = false;
	for(IQI*	i = _pQuery->GetFirstQueryIndex(); i != NULL; i = _pQuery->GetNextQueryIndex())
	{
		if(i->GetAttributes().m_bAnswerIndex){ bPresentAnswerFlag = true; break; }
	}

	//	отладочный вывод настроек параметризатора запроса
	{
		const unsigned int buffer_size = 512;
		wchar_t	buf[buffer_size];
		unsigned int counter = 0;
		counter += _snwprintf(
			buf+counter,
			buffer_size-counter,
			L"\n.\n.\nНастройки параметризатора запроса:\n");
		counter += _snwprintf(
			buf+counter,
			buffer_size-counter,
			L"\n(NdxProcessor::ForcedSearchMode == 7)     = %u", (unsigned int)IsForcedSearchMode7);
		counter += _snwprintf(
			buf+counter,
			buffer_size-counter,
			L"\nbPresentAnswerFlag (есть ли флаги ответа) = %u", (unsigned int)bPresentAnswerFlag);
		counter += _snwprintf(
			buf+counter,
			buffer_size-counter,
			L"\n_pSearchParams->m_bForceOrSearch          = %u", (unsigned int)_pSearchParams->m_bForceOrSearch);
		counter += _snwprintf(
			buf+counter,
			buffer_size-counter,
			L"\nm_QPSettings.ndxq_EnableObligatoryFlag    = %u", (unsigned int)_pSearchParams->m_QPSettings.ndxq_EnableObligatoryFlag);
		counter += _snwprintf(
			buf+counter,
			buffer_size-counter,
			L"\nm_QPSettings.ndxq_EnableUnionsWords       = %u", (unsigned int)_pSearchParams->m_QPSettings.ndxq_EnableUnionsWords);
		counter += _snwprintf(buf+counter,buffer_size-counter,L"\n.\n.");
		ToConsole(buf);
	}

	//	Вызываем нужный режим обработки
	if (bPresentAnswerFlag)
		Init_AnswerMode(_pQuery, _pSearchParams->m_QPSettings);
	else if (bIsSetAllOrMode)
		Init_AllOrMode(_pQuery, _pSearchParams->m_QPSettings);
	else
		Init_Standard(_pQuery, _pSearchParams->m_QPSettings);

	//	вывод неоптимизированного запроса
	DebugViewRawQuery(_pQuery);
	//	вывод оптимизированного запроса
	DebugView();
}

SS::Interface::Core::NdxSE::ISuperTemplate* CParametrizedQuery::GetSuperTemplate()
{
	return m_pSuperTemplate.get();
}

unsigned int CParametrizedQuery::GetVariantCount()
{
	return static_cast<unsigned int>(m_Variants.size());
}

BOOL CParametrizedQuery::GetHasAnswers()
{
	return m_HasAnswers;
}

BOOL	CParametrizedQuery::GetHasUnionBlocks()
{
	return m_HasUB;
}

BOOL	CParametrizedQuery::GetHasNonObligatoryWords()
{
	return m_HasNonObligatoryWords;
}

SS::Interface::Core::NdxSE::IVariant* CParametrizedQuery::GetVariant(unsigned int uiIndex)
{
	//	Если задан неправильный индекс возвращаем NULL
	if (uiIndex >= m_Variants.size())
	{
		ToConsole(L"CParametrizedQuery::GetVariant, неправильный индекс");
		return NULL;
	}
	//	Правильный индекс, возвращаем uiIndex'ный элемент
	return m_Variants[uiIndex];
}

void	CParametrizedQuery::Clear()
{
	m_HasAnswers			= FALSE;
	m_HasUB					= FALSE;
	m_HasNonObligatoryWords	= FALSE;
	m_TransPos_Word.clear();
	m_TransSeqPos_pWord.clear();
	m_AllUnique_Words.clear();
	m_UniqueTrans_Words.clear();
	m_UnionBlocksInfo.clear();
	m_AllUnionWords.clear();
	m_pSuperTemplate->Clear();
	for_each(m_Variants.begin(), m_Variants.end(), CVariant::Del); m_Variants.clear();
}

HRESULT CParametrizedQuery::QueryInterface(REFIID pIID, void** ppBase)
{
	*ppBase = NULL;

	if(pIID == IID_ParametrizedQuery)
		*ppBase = (SS::Interface::Core::NdxSE::IParametrizedQuery*) this;
	else if (pIID == IID_Base)
		*ppBase = (SS::Interface::IBase*) this;

	return (*ppBase == NULL) ? S_FALSE : S_OK;
}

ULONG CParametrizedQuery::Release()
{
	delete this;
	return 0;
}

void CParametrizedQuery::ApplyWordAttributes(s_flagged_word &collection, flagged_word& currentWord)
{
	flagged_word word = currentWord;
	s_flagged_word::iterator current = collection.find(currentWord);
	if(current != collection.end())
	//	такое слово есть - удалим его
	{
		word = *current;
		collection.erase(currentWord);
		word.ApplyWordAttributes(currentWord);
	}
	collection.insert(word);
}

void CParametrizedQuery::Init_AnswerMode(
	SS::Interface::Core::CommonContainers::IQueryResult*				_pQuery,
	SS::Core::Types::IndexationSearch::TQueryParametrizerSettings const&	_Settings)
{
	ToConsole(L"\n.\nPARAMETRIZED QUERY, INIT ANSWER MODE\n.\n.");
	//	Сброс
	Clear();
	//	распределить индексы по контейнерам
	MakeWords(_pQuery);

	m_HasAnswers = TRUE;

	//
	//	Вычисляем AND-блок и OR-блок
	//

	//	уникальные слова для AND-блока
	s_flagged_word	AndWords;
	//	единственное слово для OR-блока
	flagged_word	OrWord;

	//	проходим по трансформациям
	for(mm_flagged_word::iterator
		itb_trans = m_TransPos_Word.begin(),
		ite_trans = m_TransPos_Word.end();
		itb_trans != ite_trans;
		++itb_trans)
	{
		//	проходим по словам в трансформации
		for(m_flagged_word::iterator
			itb_word = itb_trans->second.begin(),
			ite_word = itb_trans->second.end();
			itb_word != ite_word;
			++itb_word)
		{
			//	текущее слово
			flagged_word& CurrentWord = itb_word->second;
			//	в зависимости от наличия у текущего слова флага ответа раскладываем слова по контейнерам
			if (CurrentWord.get_AnswerFlag())
				ApplyWordAttributes(AndWords, CurrentWord);
			else
			{
				//	сливаем индексы в одно слово, сливаем флаги
				OrWord.ApplyWordAttributes(CurrentWord);
				OrWord.MergeWordIndexes(CurrentWord);
			}
		}
	}

	//
	//	собираем AND-блок
	//

	//	составитель объектов
	CObjectInserter		INS(m_pSuperTemplate.get(), &m_Variants);

	//	перекладываем уникальные слова с флагом в блок
	for(s_flagged_word::iterator
		itb_word = AndWords.begin(),
		ite_word = AndWords.end();
		itb_word != ite_word;
		++itb_word)
	{
		if(itb_word->m_Indexes.Empty()) continue;
		//
		if (_Settings.ndxq_EnableObligatoryFlag) INS.current_word()->SetObligatoryFlag(itb_word->get_ObligatoryFlag());
		INS.current_word()->SetAnswerFlag(itb_word->get_AnswerFlag());//	true
		INS.current_word()->SetAsString(itb_word->get_AsString().c_str());
		INS.current_word()->SetLinguisticWeight(itb_word->get_LingWeight());
		INS.current_word()->Fill(itb_word->m_Indexes);
		//
		INS.Word2AndBlock();
	}
	INS.AndBlock2ST();

	//
	//	собираем OR-Блок, состоящий из одного слова
	//

	if (!OrWord.m_Indexes.Empty())
	{
		if (_Settings.ndxq_EnableObligatoryFlag) INS.current_word()->SetObligatoryFlag(OrWord.get_ObligatoryFlag());
		INS.current_word()->SetAnswerFlag(OrWord.get_AnswerFlag());//	false
		INS.current_word()->SetAsString(OrWord.get_AsString().c_str());
		INS.current_word()->SetLinguisticWeight(OrWord.get_LingWeight());
		INS.current_word()->Fill(OrWord.m_Indexes);
		//
		INS.Word2OrBlock();
		INS.OrBlock2ST();
	}

	//
	//	собираем 1 вариант и 1 последовательность
	//

	if (!m_AllUnique_Words.empty())
	{
		//	складываем все уникальные слова из всех трансформаций, проставляем им флаги
		for(s_flagged_word::iterator
			itb_word = m_AllUnique_Words.begin(),
			ite_word = m_AllUnique_Words.end();
			itb_word != ite_word;
			++itb_word)
		{
			if (itb_word->m_Indexes.Empty()) continue;
			//
			if (_Settings.ndxq_EnableObligatoryFlag) INS.current_word()->SetObligatoryFlag(itb_word->get_ObligatoryFlag());
			INS.current_word()->SetAnswerFlag(itb_word->get_AnswerFlag());
			INS.current_word()->SetAsString(itb_word->get_AsString().c_str());
			INS.current_word()->SetLinguisticWeight(itb_word->get_LingWeight());
			INS.current_word()->Fill(itb_word->m_Indexes);
			//
			INS.Word2Sequence();
		}
		INS.Sequence2Variant();
		INS.Variant2ST();
	}
}

void CParametrizedQuery::Init_Standard(
	SS::Interface::Core::CommonContainers::IQueryResult*				_pQuery,
	SS::Core::Types::IndexationSearch::TQueryParametrizerSettings const&	_Settings)
{
	//
	//	Индексы всех слов юнион-блоков всех трансформаций складываются в одно
	//	слово которое составит один дополнительный OR-блок.
	//	При вычислении AND-блока в него не войдут слова union-блоков
	//
	ToConsole(L"\n.\nPARAMETRIZED QUERY, INIT STANDARD MODE\n.\n.");
	//	Сброс
	Clear();
	//	распределить индексы по контейнерам
	MakeWords(_pQuery);


	//
	//	Создание вариантов
	//

	//	составитель объектов
	CObjectInserter		INS(m_pSuperTemplate.get(), &m_Variants);

	//	трансформации
	for(mmm_pflagged_word::iterator
		itb_trans = m_TransSeqPos_pWord.begin(),
		ite_trans = m_TransSeqPos_pWord.end();
		itb_trans != ite_trans;
		++itb_trans)
	{
		//	по последовательностям
		for(mm_pflagged_word::iterator	
			itb_seq = itb_trans->second.begin(), 
			ite_seq = itb_trans->second.end();
			itb_seq != ite_seq;
			++itb_seq)
		{
			//	по словам в последовательности
			for(m_pflagged_word::iterator	
				itb_word = itb_seq->second.begin(), 
				ite_word = itb_seq->second.end();
				itb_word != ite_word;
				++itb_word)
			{
				//	не создаём пустое слово
				if (itb_word->second.get()->m_Indexes.Empty()) continue;
				//
				if (_Settings.ndxq_EnableObligatoryFlag) INS.current_word()->SetObligatoryFlag(itb_word->second.get()->get_ObligatoryFlag());
				INS.current_word()->SetAnswerFlag(itb_word->second.get()->get_AnswerFlag());
				INS.current_word()->SetAsString(itb_word->second.get()->get_AsString().c_str());
				INS.current_word()->SetLinguisticWeight(itb_word->second.get()->get_LingWeight());
				INS.current_word()->Fill(itb_word->second.get()->m_Indexes);
				//
				INS.Word2Sequence();
			}
			//	добавление информации о union-блоках
			if (_Settings.ndxq_EnableUnionsWords)
			{
				if (INS.current_sequence() != NULL) 
					INS.current_sequence()->AssignUnionWordsBlocks(m_UnionBlocksInfo[itb_trans->first][itb_seq->first]);
			}
			//	добавление последовательности в вариант
			INS.Sequence2Variant();
		}
		INS.Variant2ST();
	}

	//	
	//	Вычисление дополнительного OR-блока, состоящего из одного слова,
	//	содержащего индексы всех слов union-блоков
	//	(если позволено настройкой)
	//

	if (_Settings.ndxq_EnableUnionsWords)
	{
		//	слово содержащее все индексы union-слов
		flagged_word	AllUbIndexesWord;
		for(s_flagged_word::iterator
			itb_word = m_AllUnionWords.begin(),
			ite_word = m_AllUnionWords.end();
			itb_word != ite_word;
			++itb_word)
		{
			//	суммируем текстовые значения слов
			AllUbIndexesWord.ApplyWordAttributes(*itb_word);
			//	складываем в него индексы
			AllUbIndexesWord.MergeWordIndexes(*itb_word);
		}

		//	
		//	Заполнение дополнительного OR-блока словами union-блоков
		//

		if (!AllUbIndexesWord.m_Indexes.Empty())
		{
			//	флаги устанавливаем
			if (_Settings.ndxq_EnableObligatoryFlag) INS.current_word()->SetObligatoryFlag(AllUbIndexesWord.get_ObligatoryFlag());
			INS.current_word()->SetAnswerFlag(AllUbIndexesWord.get_AnswerFlag());
			INS.current_word()->SetAsString(AllUbIndexesWord.get_AsString().c_str());
			INS.current_word()->SetLinguisticWeight(AllUbIndexesWord.get_LingWeight());
			INS.current_word()->Fill(AllUbIndexesWord.m_Indexes);
			//
			INS.Word2OrBlock();
			INS.OrBlock2ST();
		}
	}

	//
	//	Вычисление AND-блока
	//

	//	в IntersectionWords останутся слова которые имеются во всех трансформациях
	s_flagged_word	IntersectionWords = m_AllUnique_Words;

	//	если включена обработка union-блоков, выкидываем из множества всех слов
	//	слова входящие в union-блоки.
	if (_Settings.ndxq_EnableUnionsWords)
	{
		s_flagged_word	temp;
		std::set_difference(
			IntersectionWords.begin(),
			IntersectionWords.end(),
			m_AllUnionWords.begin(),
			m_AllUnionWords.end(),
			std::inserter(temp, temp.begin()));
		IntersectionWords.swap(temp);
	}

	//	наборы уникальных слов в трансформациях
	ms_flagged_word	TransUniqueWords = m_UniqueTrans_Words;

	//	если включена обработка юнион-блоков, выкидываем из уникальных наборов слов трансформаций
	//	слова принадлежащие юнион-блокам
	if (_Settings.ndxq_EnableUnionsWords)
	{
		for(ms_flagged_word::iterator
			itb_trans = TransUniqueWords.begin(),
			ite_trans = TransUniqueWords.end();
			itb_trans != ite_trans;
			++itb_trans)
		{
			s_flagged_word	temp;
			std::set_difference(
				itb_trans->second.begin(),
				itb_trans->second.end(),
				m_AllUnionWords.begin(),
				m_AllUnionWords.end(),
				std::inserter(temp, temp.begin()));
			itb_trans->second.swap(temp);
		}
	}

	//	проходим по всем трансформациям, ищем слова которые имеются во ВСЕХ
	//	трансформациях
	for(ms_flagged_word::iterator
		itb_trans = TransUniqueWords.begin(),
		ite_trans = TransUniqueWords.end();
		itb_trans != ite_trans;
		++itb_trans)
	{
		//	временное множество, куда выводим слова которые есть во всех трансформациях
		s_flagged_word		temp;
		//	см. реализацию set_intersection:
		//	в результат флаги попадут из множества IntersectionWords!!!
		std::set_intersection(
			IntersectionWords.begin(),
			IntersectionWords.end(),
			itb_trans->second.begin(),
			itb_trans->second.end(),
			std::inserter(temp, temp.begin()));
		IntersectionWords.swap(temp);
	}
	//	в IntersectionWords остались слова которые есть во всех трансформациях

	//
	//	заполняем AND-блок
	//

	for(s_flagged_word::iterator
		itb_word = IntersectionWords.begin(),
		ite_word = IntersectionWords.end();
		itb_word != ite_word;
		++itb_word)
	{
		//	не создаём пустых слов
		if (itb_word->m_Indexes.Empty()) continue;
		//
		if (_Settings.ndxq_EnableObligatoryFlag) INS.current_word()->SetObligatoryFlag(itb_word->get_ObligatoryFlag());
		INS.current_word()->SetAnswerFlag(itb_word->get_AnswerFlag());
		INS.current_word()->SetAsString(itb_word->get_AsString().c_str());
		INS.current_word()->SetLinguisticWeight(itb_word->get_LingWeight());
		INS.current_word()->Fill(itb_word->m_Indexes);
		//
		INS.Word2AndBlock();
	}
	INS.AndBlock2ST();

	//
	//	Вычисление OR-блоков
	//

	//	проходим по словам трансформаций, выкидываем в них те слова которые 
	//	вошли в AND-блок
	for(ms_flagged_word::iterator
		itb_trans = TransUniqueWords.begin(),
		ite_trans = TransUniqueWords.end();
		itb_trans != ite_trans;
		++itb_trans)
	{
		//	временный набор слов
		s_flagged_word	temp;
		//	складываем разницу во временный контейнер
		std::set_difference(
			itb_trans->second.begin(),
			itb_trans->second.end(),
			IntersectionWords.begin(),
			IntersectionWords.end(),
			std::inserter(temp, temp.begin()));
		//	меняем временный контейнер с контейнером трансформации
		itb_trans->second.swap(temp);
	}

	//
	//	создание OR-блоков
	//

	//	проход по трансформациям
	for(ms_flagged_word::iterator
		itb_trans = TransUniqueWords.begin(),
		ite_trans = TransUniqueWords.end();
		itb_trans != ite_trans;
		++itb_trans)
	{
		//	проход по словам
		for(s_flagged_word::iterator
			itb_word = itb_trans->second.begin(),
			ite_word = itb_trans->second.end();
			itb_word != ite_word;
			++itb_word)
		{
			//	не создаём пустых слов
			if (itb_word->m_Indexes.Empty()) continue;
			//
			if (_Settings.ndxq_EnableObligatoryFlag) INS.current_word()->SetObligatoryFlag(itb_word->get_ObligatoryFlag());
			INS.current_word()->SetAnswerFlag(itb_word->get_AnswerFlag());
			INS.current_word()->SetAsString(itb_word->get_AsString().c_str());
			INS.current_word()->SetLinguisticWeight(itb_word->get_LingWeight());
			INS.current_word()->Fill(itb_word->m_Indexes);
			//
			INS.Word2OrBlock();
		}
		INS.OrBlock2ST();
	}
}

void CParametrizedQuery::Init_AllOrMode(
	SS::Interface::Core::CommonContainers::IQueryResult*				_pQuery,
	SS::Core::Types::IndexationSearch::TQueryParametrizerSettings const&	_Settings)
{
	ToConsole(L"\n\nPARAMETRIZED QUERY, INIT ALL OR MODE\n\n");
	//	Сброс
	Clear();
	//	распределить индексы по контейнерам
	MakeWords(_pQuery);

	//
	//	вычисление единственного слова включающего уникально все индексы запроса
	//	слово включает также все строковые представления всех слов запроса
	//

	//	составитель объектов
	CObjectInserter		INS(m_pSuperTemplate.get(), &m_Variants);

	flagged_word	SingleWord;
	for(s_flagged_word::iterator
		itb_word = m_AllUnique_Words.begin(),
		ite_word = m_AllUnique_Words.end();
		itb_word != ite_word;
		++itb_word)
	{
		//	применяем атрибуты к слову
		SingleWord.ApplyWordAttributes(*itb_word);
		//	добавляем в слово индексы
		SingleWord.MergeWordIndexes(*itb_word);
	}

	//
	//	Создание единственного слова единственного OR-блока, флаги не учитываются
	//

	if (!SingleWord.m_Indexes.Empty())
	{
		if (_Settings.ndxq_EnableObligatoryFlag) INS.current_word()->SetObligatoryFlag(SingleWord.get_ObligatoryFlag());
		INS.current_word()->SetAnswerFlag(SingleWord.get_AnswerFlag());
		INS.current_word()->SetAsString(SingleWord.get_AsString().c_str());
		INS.current_word()->SetLinguisticWeight(SingleWord.get_LingWeight());
		INS.current_word()->Fill(SingleWord.m_Indexes);
		//
		INS.Word2OrBlock();
		INS.OrBlock2ST();
	}

	//
	//	Создание единственной последовательности в единственном варианте
	//

	for(s_flagged_word::iterator
		itb_word = m_AllUnique_Words.begin(),
		ite_word = m_AllUnique_Words.end();
		itb_word != ite_word;
		++itb_word)
	{
		//	не создаем пустых слов
		if (itb_word->m_Indexes.Empty()) continue;
		//
		if (_Settings.ndxq_EnableObligatoryFlag) INS.current_word()->SetObligatoryFlag(itb_word->get_ObligatoryFlag());
		INS.current_word()->SetAnswerFlag(itb_word->get_AnswerFlag());
		INS.current_word()->SetAsString(itb_word->get_AsString().c_str());
		INS.current_word()->SetLinguisticWeight(itb_word->get_LingWeight());
		INS.current_word()->Fill(itb_word->m_Indexes);
		//
		INS.Word2Sequence();
	}

	INS.Sequence2Variant();
	INS.Variant2ST();
}

void	CParametrizedQuery::MakeWords(SS::Interface::Core::CommonContainers::IQueryResult* pQuery)
{
	//
	//	Распределяем индексы по вспомогательным контейнерам в соответствии с их (индексов) атрибутами
	//

	//	атрибуты
	//	отлавливалка минимальных позиций последовательностей
	mm_mcth_uc			SequencesBeginsCatcher;
	//	множества позиций union-блоков, [ID транс][ID послед][union-block ID](позиции)
	mmms_uchar			UnionPositionsInSeqences;
	//	коллекции позиций в последовательностях
	mms_uchar			PositionsInSequences;

	for(IQI*	i = pQuery->GetFirstQueryIndex();
		i != NULL; 
		i = pQuery->GetNextQueryIndex())
	{
		const SS::Interface::Core::CommonContainers::SQueryIndexAttributes Attribs = i->GetAttributes();
		unsigned int indexPosition = Attribs.m_IndexPosition;

		//	отлавливаем минимальный номер позиции для каждого union-блока
		SequencesBeginsCatcher[Attribs.m_TransformationID][Attribs.m_SequenceID] = Attribs.m_IndexPosition;

		//{
		//	const unsigned int buffer_size = 256;
		//	wchar_t	buf[buffer_size];
		//	unsigned int counter = 0;
		//	counter += _snwprintf(
		//		buf+counter,
		//		buffer_size-counter,
		//		L"\nBeginUBCatcher[%u][%u] = %u\n",
		//		Attribs.m_TransformationID,
		//		Attribs.m_SequenceID,
		//		Attribs.m_IndexPosition);
		//	ToConsole(buf);
		//}

		//	[трансформация][позиция](слово)
		m_TransPos_Word[Attribs.m_TransformationID][Attribs.m_IndexPosition].AddApplyQueryIndex(i);
		//	информация о словах в последовательностях
		m_TransSeqPos_pWord[Attribs.m_TransformationID][Attribs.m_SequenceID][Attribs.m_IndexPosition] = 
			&m_TransPos_Word[Attribs.m_TransformationID][Attribs.m_IndexPosition];
		//	коллекции позиций в последовательностях
		PositionsInSequences[Attribs.m_TransformationID][Attribs.m_SequenceID].insert(indexPosition);
		//	если этот индекс union-блока запомним позицию

		if (Attribs.m_WordsBlockID != 0)
		{
			//	запомним что юнион-блоки были
			m_HasUB = TRUE;
			//	запоминаем позицию в запросе индекса слова union-блока
			UnionPositionsInSeqences[Attribs.m_TransformationID][Attribs.m_SequenceID][Attribs.m_WordsBlockID].insert(indexPosition);
		}
	}

	//
	//	Переводим информацию о юнион-блоках в вид, пригодный для дальнейшего использования
	//	(в методах разбора запроса в разных режимах)
	//
	//	Собираем в множество все слова входящие в union-блоки
	//

	//	проход по трансформациям
	for(mmms_uchar::iterator
		itb_trans = UnionPositionsInSeqences.begin(),
		ite_trans = UnionPositionsInSeqences.end();
		itb_trans != ite_trans;
		++itb_trans)
	{
		//	проход по последовательностям в трансформации
		for(mms_uchar::iterator
			itb_useq = itb_trans->second.begin(),
			ite_useq = itb_trans->second.end();
			itb_useq != ite_useq;
			++itb_useq)
		{
			//	union-блоки в последовательности трансформации
			for(ms_uchar::iterator
				itb_ublock = itb_useq->second.begin(),
				ite_ublock = itb_useq->second.end();
				itb_ublock != ite_ublock;
				++itb_ublock)
			{
				//
				//	Вычисляем позиции начала и конца юнион-блока в последовательности
				//

				//	начальная позиция union-блока в последовательности
				unsigned int	StartUBPositionInSequence = 0;
				//	идём по последовательности, считаем количество существующих элементов последовательности
				//	между минимальной позицией последовательности и минимальной позицией слова union-блока
				//	это борьба против разрывов в последовательностях
				//
				//	ЭТОТ АЛГОРИТМ НЕ ЗАЩИЩЁН ОТ РАЗРЫВОВ В UNION-БЛОКАХ !!!
				//	если присутствуют разрывы в union-блоках, никакие индексы не выйдут за пределы,
				//	но будут указывать на неправильные слова
				for(unsigned int i = SequencesBeginsCatcher[itb_trans->first][itb_useq->first].get(),//минимальная позиция в последовательности
					i_size = *(itb_ublock->second.begin());//минимальная позиция слова union-блока
					i < i_size;
					++i)
				{
					if (PositionsInSequences[itb_trans->first][itb_useq->first].find(i) != 
						PositionsInSequences[itb_trans->first][itb_useq->first].end()) 
							++StartUBPositionInSequence;
				}
				//	начальная позиция union-блока в последовательности 
				//	(0-первый элемент в ПОСЛЕДОВАТЕЛЬНОСТИ а не в запросе!)
				unsigned int	pos_begin = StartUBPositionInSequence;

				//{
				//	const unsigned int buffer_size = 256;
				//	wchar_t	buf[buffer_size];
				//	unsigned int counter = 0;
				//	counter += _snwprintf(
				//		buf+counter,
				//		buffer_size-counter,
				//		L"\n*(itb_ublock->second.begin()) = %u\nBeginUBCatcher[itb_trans->first = %u][itb_useq->first = %u].get() = %u",
				//		(unsigned int)(*(itb_ublock->second.begin())),
				//		(unsigned int)itb_trans->first,
				//		(unsigned int)itb_useq->first,
				//		(unsigned int)SequencesBeginsCatcher[itb_trans->first][itb_useq->first].get());
				//	ToConsole(buf);
				//}

				//	конечная позиция слова union-блока в последовательности (включительно!)
				unsigned int	pos_end = pos_begin + (unsigned int)itb_ublock->second.size() - 1;
				//	сохраняем в контейнер информацию о union-блоке
				m_UnionBlocksInfo[itb_trans->first][itb_useq->first].push_back(UWB(pos_begin, pos_end));

				//{
				//	const unsigned int buffer_size = 256;
				//	wchar_t	buf[buffer_size];
				//	unsigned int counter = 0;
				//	counter += _snwprintf(
				//		buf+counter,
				//		buffer_size-counter,
				//		L"\nUWB(pos_begin = %u, pos_end = %u), ",
				//		pos_begin,
				//		pos_end);
				//	ToConsole(buf);
				//}

				//
				//	Собираем уникально все слова, входящие в юнион-блок
				//

				//	проходим по всем словам union-блока
				for(s_uchar::iterator
					itb_pos = itb_ublock->second.begin(),
					ite_pos = itb_ublock->second.end();
					itb_pos != ite_pos;
					++itb_pos)
				{
					ApplyWordAttributes(m_AllUnionWords, m_TransPos_Word[itb_trans->first][*itb_pos]);
				}
			}
		}
	}

	//	проходим по всем словам, собираем уникально все слова
	for(mm_flagged_word::iterator
		itb_trans = m_TransPos_Word.begin(),
		ite_trans = m_TransPos_Word.end();
		itb_trans != ite_trans;
		++itb_trans)
	{
		for(m_flagged_word::iterator
			itb_pos = itb_trans->second.begin(),
			ite_pos = itb_trans->second.end();
			itb_pos != ite_pos;
			++itb_pos)
		{
			//
			//	Собираем уникальные слова во всём запросе
			//
			ApplyWordAttributes(m_AllUnique_Words, itb_pos->second);

			//
			//	Собираем уникальные слова в каждой трансформации
			//
			ApplyWordAttributes(m_UniqueTrans_Words[itb_trans->first], itb_pos->second);

			//
			//	Запоминаем наличие необязательных слов в запросе
			//
			if (!itb_pos->second.get_ObligatoryFlag()) m_HasNonObligatoryWords = TRUE;
		}
	}
}

void	CParametrizedQuery::DebugView()
{
	ToConsole(ToString());
}

const wchar_t*	CParametrizedQuery::ToString()
{
	m_AsString.assign(L"\nCParametrizedQuery is empty\n");

	const unsigned int buffer_size = 20*1024;
	wchar_t	buf[buffer_size];
	unsigned int counter = 0;
	SS::Interface::Core::CommonContainers::SQueryIndexAttributes	Attribs;

	counter += _snwprintf(
		buf+counter,
		buffer_size-counter, L"\
\n.\n.\n-------------- OPTIMIZED QUERY (NdxQuery.dll) --------\n\
\nHas answers:              %u\
\nHas union-blocks:         %u\
\nHas non-obligatory words: %u\n.",
		(unsigned int)m_HasAnswers,
		(unsigned int)m_HasUB,
		(unsigned int)m_HasNonObligatoryWords);

	if (m_pSuperTemplate.get() != NULL)
	{
		counter += _snwprintf(
			buf+counter,
			buffer_size-counter, L"%s",
			m_pSuperTemplate->ToString());
	}

	if (!m_Variants.empty()) counter += _snwprintf(buf+counter, buffer_size-counter, L"\n.\n[[ VARIANTS ]]\n");

	for(v_Variant::iterator
		itb_var = m_Variants.begin(),
		ite_var = m_Variants.end();
		itb_var != ite_var;
		++itb_var)
	{
		if (*itb_var == NULL)
		{
			counter += _snwprintf(
				buf+counter,
				buffer_size-counter, L"warning! m_Variants contains NULL");
			continue;
		}

		counter += _snwprintf(
			buf+counter,
			buffer_size-counter,
			L"%s",
			(*itb_var)->ToString());
	}

	counter += _snwprintf(
		buf+counter,
		buffer_size-counter,
		L"\n.\n--------- END OF OPTIMIZED QUERY -----------\n.");

	m_AsString.assign(buf);

	return m_AsString.c_str();
}

const wchar_t*	CParametrizedQuery::ToStringRawQuery(SS::Interface::Core::CommonContainers::IQueryResult* pQuery)
{
	m_AsStringRawQuery.clear();

	//	имена типов индексов
	wchar_t*	inMorpho	= L"morpho  ";
	wchar_t*	inSyntax	= L"syntax  ";
	wchar_t*	inSemantic	= L"semantic";

	const unsigned int buffer_size = 20*1024;
	wchar_t	buf[buffer_size];
	unsigned int counter = 0;
	SS::Interface::Core::CommonContainers::SQueryIndexAttributes	Attribs;
	counter += _snwprintf(
		buf+counter,
		buffer_size-counter, L"\
\n                       RAW QUERY\n.\
\n-------------------------------------------------------------------\
\n    index      .Index Type.LingWeight.trID.seID.wbID.ipos|fOBL.fANS|\
\n-------------------------------------------------------------------");
	for(IQI*	i = pQuery->GetFirstQueryIndex();
		i != NULL; 
		i = pQuery->GetNextQueryIndex())
	{
		wchar_t*	wszIndexTypeName = NULL;
		switch(LingDecoder::get_index_type(i->GetQueryIndex()))
		{
		case LingDecoder::eitMorphoIndex:	wszIndexTypeName = inMorpho; break;
		case LingDecoder::eitSyntaxIndex:	wszIndexTypeName = inSyntax; break;
		case LingDecoder::eitSemanticIndex:	wszIndexTypeName = inSemantic; break;
		default: wszIndexTypeName = inMorpho;
		}
		std::wstring	word( (i->GetQueryWord() == NULL) ? (L"<NULL>") : (i->GetQueryWord()) );
		Attribs = i->GetAttributes();
		counter += _snwprintf(
			buf+counter,
			buffer_size-counter,
			L"\n  %10u   |%10s|%#10.2f|%4u|%4u|%4u|%4u|%4u|%4u| %s",
			i->GetQueryIndex(),
			wszIndexTypeName,
			(double)(i->GetIndexWeight()),
			(unsigned int)Attribs.m_TransformationID,
			(unsigned int)Attribs.m_SequenceID,
			(unsigned int)Attribs.m_WordsBlockID,
			(unsigned int)Attribs.m_IndexPosition,
			(unsigned int)Attribs.m_Obligatory,
			(unsigned int)Attribs.m_bAnswerIndex,
			word.c_str());
	}

	counter += _snwprintf(buf+counter, buffer_size-counter, L"\n");
	m_AsStringRawQuery.assign(buf);

	return m_AsStringRawQuery.c_str();
}

void	CParametrizedQuery::DebugViewRawQuery(SS::Interface::Core::CommonContainers::IQueryResult* pQuery)
{
	ToConsole(ToStringRawQuery(pQuery));
}


}
}
}
}

