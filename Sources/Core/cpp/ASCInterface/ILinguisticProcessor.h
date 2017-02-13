#pragma once
#include "ICommonContainers.h"
#include "TCoommonTypes.h"

#include <string>
#include <vector>

// {B3D22326-7558-4144-8C39-B9952C02403F}
static const GUID CLSID_LinguisticProcessor = 
{ 0xb3d22326, 0x7558, 0x4144, { 0x8c, 0x39, 0xb9, 0x95, 0x2c, 0x2, 0x40, 0x3f } };

// {41C0E09E-9E89-4286-BD80-54A4174E2FB4}
static const GUID IID_LinguisticProcessor = 
{ 0x41c0e09e, 0x9e89, 0x4286, { 0xbd, 0x80, 0x54, 0xa4, 0x17, 0x4e, 0x2f, 0xb4 } };


namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			///хранилище лексем текста
			class ILexemeBoard
			{
			public:
				ILexemeBoard() {}
				virtual ~ILexemeBoard() {}
			public:
				/// вернет емкость хранилища (измеряется в максимальном количестве единовременно загружаемых лексем текста)
				virtual unsigned int GetCapacity() = 0;
				/// вернет количество загруженных лексем текста
				virtual unsigned int GetCount() = 0;
				/// переход на первую лексему, вернет 0, если лексем нет
				virtual unsigned int MoveFirst() = 0;
				/// переход на следующую лексему, вернет 0, если находится на последней лексеме
				virtual unsigned int MoveNext() = 0;
				///	получение лексемы
				virtual const wchar_t* GetLexeme() = 0;
				virtual const char* GetLexemeA() = 0;
				/// получение абсолютного номера текущей лексемы в тексте (нумерация начинается с 0)
				virtual unsigned int GetAbsoluteIndex() = 0;
				/// получение абсолютного номера первого символа текущей лексемы
				virtual unsigned int GetAbsoluteSymbolIndex() = 0;
			};

			///интерфейс управления системной частью лексического анализатора
			class ILexicaPrimary : virtual public SS::Interface::IBase
			{
			public:
				ILexicaPrimary() {}
				virtual ~ILexicaPrimary() {}
			public:
				///подключение анализатора к тексту
				virtual void SetText(SS::Core::Types::IndexationSearch::TNativeTextForIndexation* pNativeText, unsigned int uiStartPosition = 0) = 0;
				///загрузка очередной (в т.ч. и первой) части текста в анализатор, вернет 0, если ничего не загружено (или если все части уже были до этого загружены)
				virtual unsigned int LoadNext(
					/// если ставитcя в true, то загрузка текста происходит без учета деления на параграфы
					bool bIgnoreParagraphs = false
					) = 0;
				///вернет интерфейс управления хранилищем лексем текущей загруженной части текста
				/**
					примечание: возвращаемая ссылка на интерфейс ILexemeBoard корректна
					на всем этапе жизни объекта интерфейса ILexicaPrimary
				*/
				virtual ILexemeBoard* GetLexemeBoard() = 0;
			};

			/// отражает набор функций, требуемых от лингвистического процессора при сценарии индексации
			class ILinguisticIndexation
			{
			public:
				ILinguisticIndexation(){}
				virtual ~ILinguisticIndexation(){}
			public:

				///запуск пакетной индексации документа
				virtual HRESULT StartDocumentIndexation() = 0;

				/// инициирует процесс индексации очередного участка текста
				virtual void Init(
					/// контейнер с индексируемым текстом
					SS::Core::Types::IndexationSearch::TNativeTextForIndexation* pNativeText
					/// параметры индексации текста
					, SS::Core::Types::IndexationSearch::TIndexationParams* pIndexationParams
					) = 0;

				/// инициирует процесс индексации очередной таблицы
				virtual void Init(
					/// контейнер с индексируемым текстом
					std::list< std::list<SS::Core::Types::IndexationSearch::TNativeTextForIndexation*> > * p_l_l_NativeText
					/// параметры индексации текста
					, SS::Core::Types::IndexationSearch::TIndexationParams* pIndexationParams
					) = 0;

				/// генерирует очередную порцию индекса
				/**
				вернет:
					0 - если есть еще части для анализа
					1 - если больше нет
					2 - если была ошибка
				*/
				virtual unsigned int GenerateNext(
					/// указатель на контейнер, который необходимо заполнить
					SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult
					) = 0;

				///останов пакетной индексации документа
				virtual HRESULT EndDocumentIndexation() = 0;
			};

			/// отражает набор функций, требуемых от лингвистического процессора при сценарии анализа запроса
			class ILinguisticQuery
			{
			public:
				ILinguisticQuery(){}
				virtual ~ILinguisticQuery(){}
			public:
				/// данная функция используется при поиске для получения запроса к индексному процессору по разобранному запросу 
				/**
					используется для подачи запроса к индексным процессорам, принимающим на вход стандартный контейнер запроса SS
				*/
				virtual void GenerateQuery(
					/// параметры поиска
					SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
					/// параметры анализа запроса
					, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
					/// контейнер, который необходимо заполнить
					, SS::Interface::Core::CommonContainers::IQueryResult* pQureResult
					/// критерий сохранения результата разбора текущего запроса
					, bool bIsRememberQuery
					) = 0;

				/// получение списка индексов основ слов запроса
				virtual void GetQuerySourceIndices(std::vector<unsigned int>* pvIndices) = 0;

				virtual void GetQueryKeywords(LPCWSTR szQuery, std::vector<std::wstring>* pKeywords, 
						std::vector<std::vector<std::wstring> >* pMorphoForms) = 0;
			};

			/// отражает набор функций, требуемых от лингвистического процессора при сценарии поиска и оценке релевантности
			class ILinguisticSearch
			{
			public:
				ILinguisticSearch(){}
				virtual ~ILinguisticSearch(){}
			public:
				/// подает результат анализа запроса, который 
//				virtual void SetQueryForFurtherUpdate(SS::Interface::Core::CommonContainers::IQueryResult* pQureResult);
				/// подсчитывает лингвистическую релевантность результатов поиска
				virtual void UpdateRelevance(
					/// контейнер с результатами поиска
					SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
					/// контейнер с запросом пользователя (с учетом изменений, сформированных при вызове ILinguisticQuery)
					,SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
					/// параметры анализа запроса (с учетом изменений, сформированных при вызове ILinguisticQuery)
					, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
					/// параметры подсчета релевантности
					, SS::Core::Types::IndexationSearch::TUpdateRelevanceParams* pUpdateRelevanceParams
					) = 0;
				/// устанавливает тест блок, содержащий заголовки
				virtual void SetTitles(SS::Interface::Core::CommonContainers::ITextBlock * pTextBlock) = 0;
			};

			/// отражает набор функций, требуемых от лингвистического процессора при сценарии вывода оригинала
			class ILinguisticShowResults
			{
			public:
				ILinguisticShowResults(){}
				virtual ~ILinguisticShowResults(){}
			public:
				virtual ILexicaPrimary* GetLexicaPrimary() = 0;

				/// проверяет наличие слова в словаре
				/** 
					\param psWord - указатель на слово
					\return unsigned int вернет 0, если слова нет в базе и 1 если есть
				*/				
				virtual unsigned int IsExistWord(
					const wchar_t* psWord
					) = 0;

				/// заполнение контейнера IIndexAndPosition индексами основ
//				virtual void FillSourceIndices(SS::Interface::Core::CommonContainers::IIndexAndPosition* pIAP);

				/// данная функция используется, если результат поиска содержит индексы слов для наполнения строковым описанием слов
				virtual void PrepareForView(
					/// контейнер с результатами поиска
					SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult
					/// параметры подготовки к выводу (кол-во подготавливаемых результатов и т.п.)
					, SS::Core::Types::IndexationSearch::TPrepareForViewParams* pPrepareForViewParams
					) = 0;

				/// получение списка индексов основ для слова
				virtual void GetSourceIndices(const wchar_t* wszWord, std::vector<unsigned int>* pvIndices) = 0;
			};

			/// главный интерфейс лингвистического процессора
			/**
				доступен для создания по CLSID = CLSID_LinguisticProcessor
				для интерфейса IID = IID_LinguisticProcessor 
			*/
			class ILinguisticProcessor : public SS::Interface::IBase
			{
			public:
				ILinguisticProcessor(){}
				virtual ~ILinguisticProcessor(){}
			public:
				/// возвращает интерфейс для выполнения лингвистической индексации
				virtual ILinguisticIndexation* GetLinguisticIndexation() = 0;
				/// возвращает интерфейс для выполнения лингвистического анализа запроса пользователя
				virtual ILinguisticQuery* GetLinguisticQuery() = 0;
				/// возвращает интерфейс для выполнения лингвистического поиска
				virtual ILinguisticSearch* GetLinguisticSearch() = 0;
				/// возвращает интерфейс для отображения результатов поиска
				virtual ILinguisticShowResults* GetLinguisticShowResults() = 0;
				/// служит для выставления режима работы лингвистического процессора
				virtual void SetMode(SS::Core::Types::IndexationSearch::TLinguisticProcessorMode* pMode) = 0;
			};
		}
	}
}