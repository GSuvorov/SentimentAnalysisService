#pragma once


#include "../ASCInterface/ILinguisticProcessor.h"

#include "./LinguisticWorkingInterfaces.h"

#include "..\ASSInterface\IBlackBoxQuery.h"
using namespace SS::Interface::Core::BlackBox::Query;



//#include "../ASSInterface/ILexicaLib.h"
//#include "../ASSInterface/ISyntaxAnalyzer.h"
//#include "../ASSInterface/ISemanticAnalyze.h"
//#include "../ASSInterface/IRev.h"

//#include "../ASSInterface/IDictionaryManager.h"

namespace SS
{
namespace LinguisticProcessor
{

	class CLinguisticQuery 
        : public SS::Interface::Core::ILinguisticQuery
		, public SS::LinguisticProcessor::CLinguisticWorkingInterfaces
	{
	private:// параметры лингвистического процессора
		/// маркер запоминания запроса
		bool m_bRemember;		
	protected:// анализаторы
		/// лексический анализатор
	public:
		CLinguisticQuery(void);
		~CLinguisticQuery(void);
	public:
		/// данная функция используется при поиске для получения запроса к индексному процессору по разобранному запросу 
		/**
			используется для подачи запроса к индексным процессорам, принимающим на вход стандартный контейнер запроса SS
		*/
		void GenerateQuery(
			/// параметры поиска
			SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
			/// параметры анализа запроса
			, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
			/// контейнер, который необходимо заполнить
			, SS::Interface::Core::CommonContainers::IQueryResult* pQueryResult
			/// критерий сохранения результата разбора текущего запроса
			, bool bIsRememberQuery					
			);

		/// данная функция используется при поиске для получения запроса к индексному процессору по разобранному запросу 
		/**
			используется для подачи запроса к индексным процессорам, принимающим на вход стандартный контейнер запроса SS
		*/
		void GenerateQuery(
			/// параметры поиска
			SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
			/// параметры анализа запроса
			, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
			/// контейнер, который необходимо заполнить
			, SS::Core::Types::IndexationSearch::TAbsentWords* pAbsentWords
			);

		virtual void GetQuerySourceIndices(std::vector<unsigned int>* pvIndices);

		virtual void GetQueryKeywords(LPCWSTR szQuery, std::vector<std::wstring>* pKeywords, 
				std::vector<std::vector<std::wstring> >* pMorphoForms);

	public:
		/// возвращает последний результат анализа
		virtual SS::Interface::Core::BlackBox::Query::IQuery* GetLastQuery()
		{
			return m_pQuery;
		}
	protected:
		void AddCollectionIndexes(std::set<unsigned int>* pSet, IQueryUnitIndexCollection* pColl);
		/// добавляет морфологические индексы синонимов в список для подсветки
		void GetSynonims( SS::Dictionary::Types::TDictionaryIndex& oDictionaryIndex, std::vector<unsigned int>* pvIndices);
		/// добавляет однокоренные слова для каждого слова запроса
		void AddOneRootWords();
		/// применяет параметры системы к Лингвистическому процессору
		void ApplyQueryParams(
			/// параметры поиска
			SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
			/// параметры анализа запроса
			, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
			);
		/// заполнение отсутствующих слов
		void FillAbsentWords(SS::Core::Types::IndexationSearch::TAbsentWords* pAbsentWords);
		/// проверка текста на наличие в нем какой-либо информации
		bool IsITextEmpty();
	};
}
}