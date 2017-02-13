// ICommonContainers.h

#pragma once

#include "ITextFeature.h"
#include "tcoommontypes.h"
//ссылки на не общие аш-файлы не допускаются!


// {DC26B1F5-F463-4419-BC5D-0F3E5E704057}
static const GUID CLSID_IndexationResult = 
{ 0xdc26b1f5, 0xf463, 0x4419, { 0xbc, 0x5d, 0xf, 0x3e, 0x5e, 0x70, 0x40, 0x57 } };
// {DE2B93EE-1329-4020-9BE1-DC141EC2DCFA}
static const GUID IID_IndexationResult = 
{ 0xde2b93ee, 0x1329, 0x4020, { 0x9b, 0xe1, 0xdc, 0x14, 0x1e, 0xc2, 0xdc, 0xfa } };
// {1D50D5DD-DBC3-4793-95F2-C162F4F1CB3B}
static const GUID CLSID_QueryResult  = 
{ 0x1d50d5dd, 0xdbc3, 0x4793, { 0x95, 0xf2, 0xc1, 0x62, 0xf4, 0xf1, 0xcb, 0x3b } };
// {3BAB5827-C836-4284-9300-D46F666DE78F}
static const GUID IID_QueryResult  = 
{ 0x3bab5827, 0xc836, 0x4284, { 0x93, 0x0, 0xd4, 0x6f, 0x66, 0x6d, 0xe7, 0x8f } };
// {D9B2AED8-ACC6-45c0-A220-7268E2F2CCEB}
static const GUID CLSID_SearchResult  = 
{ 0xd9b2aed8, 0xacc6, 0x45c0, { 0xa2, 0x20, 0x72, 0x68, 0xe2, 0xf2, 0xcc, 0xeb } };
// {0B4797BB-95F1-47d9-A8A0-DEF9B6803F4A}
static const GUID IID_SearchResult  = 
{ 0xb4797bb, 0x95f1, 0x47d9, { 0xa8, 0xa0, 0xde, 0xf9, 0xb6, 0x80, 0x3f, 0x4a } };


namespace SS
{
  namespace Interface
  {
	namespace Core
	{
		namespace CommonContainers
		{
			class ITextBlock;
			class ITextBlockCut;
			class ITextFeature;
			class IIndexAndPosition;
			class IIndexationIndex;
			class IServiceIndex;

			/// атрибуты индекса запроса
			struct SQueryIndexAttributes
			{
				unsigned int
				///идентификатор трансформации индекса
				m_TransformationID : 8,
				///идентификатор последовательности индекса
				m_SequenceID : 8,
				///позиция индекса
				m_IndexPosition : 8,
				///идентификатор блока слов, 
				//если слова объеденены в блок, в результате достаточно присутствуие хотя бы N слов из блока
				m_WordsBlockID : 6,
				//в данный момент флаг обязательности
				m_Obligatory : 1,
				//флаг индексов ответа				
				m_bAnswerIndex : 1;
				
				SQueryIndexAttributes(void):
					m_TransformationID(0xff),
					m_SequenceID(0xff),
					m_WordsBlockID(0),
					m_IndexPosition(0xff),
					m_Obligatory(0),
					m_bAnswerIndex(0){};
			};

			/// "индекс запроса" инкапсулирует индекс запроса к индексному процессору
			class IQueryIndex // при проходе дерева будем получать *IItem
			{																						// необходимо привести к *IQueryIndex
			public:
				virtual ~IQueryIndex(){};
				/// получение слова запроса (если это запрос к глобальному движку)
				virtual const wchar_t* GetQueryWord() = 0;
				/// получение слова запроса (если это запрос к глобальному движку)
				virtual void SetQueryWord(const wchar_t* wszWord) = 0;

				/// получение индекса
				virtual unsigned int GetQueryIndex() = 0;
				/// установление индекса
				virtual void SetQueryIndex(unsigned int uiIndex) = 0;

				///возвращает вес индекса
				virtual float GetIndexWeight(void) = 0;
				///устанавливает вес индекса
				virtual void SetIndexWeight(float fWeight) = 0;
				///возвращает аттрибуты
				virtual SQueryIndexAttributes GetAttributes(void) = 0;
				///устанавливает аттрибуты
				virtual void SetAttributes(SQueryIndexAttributes oAttributes) = 0;
				///возвращает оператор слова
				virtual const char GetOperator() = 0;
				///устанавливает оператор слова
				virtual void SetOperator(char chOperator) = 0;
				/// получение кол-ва скобок слева
				virtual const unsigned int GetLeftBrackets() = 0;
				///	получение кол-ва скобок справа
				virtual const unsigned int GetRightBrackets() = 0;
				/// установление кол-ва скобок слева
				virtual void SetLeftBrackets(unsigned int uiBracketCount) = 0;
				/// установление кол-ва скобок справа
				virtual void SetRightBrackets(unsigned int uiBracketCount) = 0;
			};

			/// "индексный запрос" интерфейс предназначен для обмена результатами анализа и формирования индексов для поиска
			class IQueryResult : public SS::Interface::IBase
			{	
			public:
				virtual ~IQueryResult(){};
				/// очистка хранилища (удаляет все индексы)
				virtual void Clear() = 0;
				/// добавление индекса в хранилище (добавит себе индекс и вернет указатель на него)
				virtual IQueryIndex* AddQueryIndex() = 0;

				/// получение первого индекса коллекции (0 - если нет)
				virtual IQueryIndex* GetFirstQueryIndex() = 0;
				/// получение очередного индекса коллекции (не забывать вызвать получение первого индекса) (0 - если закончились)
				virtual IQueryIndex* GetNextQueryIndex() = 0;
				/// вернет список отсутствующих слов
				virtual SS::Core::Types::IndexationSearch::TAbsentWords* GetAbsentWords() = 0;
			};
			
			/// "результат поиска" интерфейс предназначен для обмена результатами поиска
			class ISearchResult : public SS::Interface::IBase
			{
			public:
				virtual ~ISearchResult(){};

				/// вернет количество результатов поиска
				virtual unsigned int Count() = 0;

				/// установит режим обхода результатов 
				/**
				если ставится в true, то результаты возвращаются отсортированными по релевантности по убывающей
				в противном случае, порядок значения не имеет
				*/
				virtual void SetSortedMode(bool bOn) = 0;

				/// очистка хранилища (удаляет все текстовые блоки)
				virtual void Clear() = 0;
				/// добавление текстового блока в хранилище (добавит себе блок и вернет указатель на него)
				virtual ITextBlock* AddTextBlock() = 0;

				/// получение первого текстового блока коллекции (0 - если нет)
				virtual ITextBlock* GetFirstTextBlock() = 0;
				/// получение очередного текстового блока коллекции (не забывать вызвать получение первого текстового блока) (0 - если закончились)
				virtual ITextBlock* GetNextTextBlock() = 0;
				/// вернет результат поиска под определенным номером
				/**
					если номер некорректный, вернет НУЛЛ
					также сдвинет внутренний итератор на соответствующую позицию - так, что функцией GetNextTextBlock() можно получить очередной результат
					учитывает режим сортировки 
					nPosition начинается с 0
				*/
				virtual ITextBlock* GetTextBlock(unsigned int nPosition) = 0;

				virtual bool GetXML(std::wstring* pXML) = 0;
				virtual bool GetXML(std::wstring* pXML, int iFirst, int iCount) = 0;
				virtual void SetRequestID(unsigned int id) = 0;
				virtual unsigned int GetRequestID() = 0;
				//Установить результат из xml (каждый вызов создает новый TextBlock)
				virtual void SetGsXML(const wchar_t* wzXml) = 0;
			};

			// "индексный контейнер" интерфейс предназначен для обмена результатами анализа и формирования индексов при индексации
			/**
				содержит блоки индексов текста, которые надо проиндексировать
				каждый блок соответствует одному текста 
			*/
			class IIndexationResult : public SS::Interface::IBase
			{
			public:
				virtual ~IIndexationResult(){};
				// очистка хранилища (удаляет все текстовые блоки)
				virtual void Clear() = 0;
				// добавление текстового блока в хранилище (добавит себе блок и вернет указатель на него)
				virtual ITextBlock* AddTextBlock() = 0;

				// получение первого текстового блока коллекции (0 - если нет)
				virtual ITextBlock* GetFirstTextBlock() = 0;
				// получение очередного текстового блока коллекции (не забывать вызвать получение первого текстового блока) (0 - если закончились)
				virtual ITextBlock* GetNextTextBlock() = 0;
			};

			/// "Текстовый блок". Используется как при индексации, так и при поиске
			/**
				содержит отдельные куски текста и информацию по индексируемому тексту в развернутом виде
				особенности следующие:
				= При индексации
					при индексации каждый текстовый блок содержит по одной вырезке, в которой идут подряд все индексы
				= При поиске
					после поиска содержит один или более вырезок, содержащих фрагмент (или фрагменты) найденного текста
					эти вырезки используются для отображения, а также для пост-анализа релевантности
			*/
			class ITextBlock
			{
			public:
				virtual ~ITextBlock(){};
				// характеристики текста, которому принадлежит сей текстовый блок
				/**
					при первом вызове создаст пустой экземпляр характеристик
					при индексации данный экземпляр предполагается заполнять по мере поступления очередных текстов
				*/
				virtual ITextFeature* GetTextFeature() = 0;

				/// очистка блока (удалит все вырезки из текста)
				virtual void Clear() = 0;
				/// добавит вырезку в хранилище (добавит себе и вернет ссылку)
				virtual ITextBlockCut* AddBlockCut() = 0;

				/// получение первой вырезки коллекции (0 - если нет)
				virtual ITextBlockCut* GetFirstBlockCut() = 0;
				/// получение очередной вырезки коллекции (0 - если закончились)
				virtual ITextBlockCut* GetNextBlockCut() = 0;

				/// получение релевантности данного текстового блока
				virtual unsigned int GetRelevance() = 0;
				/// установление релевантности данного текстового блока
				virtual void SetRelevance(unsigned int uiRelevance) = 0;
				/// получение XML-строки содержимого контейнера
				virtual std::wstring* GetXML() = 0;

				SS::Core::Types::IndexationSearch::EIndexProcessor m_IndexProcessorMark;
			};

			/// "Вырезка" из тескстового блока, получаемая при доставании результатов поиска. Для одного блока может быть несколько вырезок
			/**
				содержит индексы на позиции, идущие подряд в тексте
			*/
			class ITextBlockCut
			{
			public:
				virtual ~ITextBlockCut(){};
				/// очистка вырезки (удалит все индексы)
				virtual void Clear() = 0;
				/// добавление индекса в коллекцию (добавит в себя и вернет указатель)
				virtual IIndexAndPosition* AddIndexAndPosition() = 0;

				/// получение первого индекса коллекции (0 - если нет)
				virtual IIndexAndPosition* GetFirstIndexAndPosition() = 0;
				/// получение очередного индекса коллекции (не забывать вызвать получение первого индекса) (0 - если закончились)
				virtual IIndexAndPosition* GetNextIndexAndPosition() = 0;

				/// получение релевантности данной текстовой части
				virtual unsigned int GetRelevance() = 0;
				/// установление релевантности данной текстовой части
				virtual void SetRelevance(unsigned int uiRelevance) = 0;

				/// присвоение текстового куска в виде строки (используется плагинами GlobalSearch)
				virtual void SetTextCut(const wchar_t* wszText) = 0;
				/// получение текстового куска в виде строки
				virtual const wchar_t* GetTextCut() = 0;

				///присвоение "имени" типа индекса данной текстовой части
				virtual void SetIndexTypeName(const wchar_t* wszName) = 0;
				///получение "имени" типа индекса данной текстовой части
				virtual const wchar_t* GetIndexTypeName() = 0;
				
				/// установление "имени" плагина
				virtual void SetPluginName(const wchar_t* wsName) = 0;
				/// получение "имени" плагина
				virtual const wchar_t* GetPluginName() const = 0;
				
				/// возвращает тип текст блок 
				virtual SS::Core::Types::IndexationSearch::ETextCutType GetType() = 0;
				/// устанавливает тип текст блока
				virtual void SetType(SS::Core::Types::IndexationSearch::ETextCutType eTextCutType) = 0;
				/// возвращает уровень заголовка
				virtual unsigned int GetLevel() = 0;
				/// устанавливает уровень заголовка
				virtual void SetLevel(unsigned int uiLevel) = 0;
				/// возвращает номер группы текст блок катов
				virtual unsigned int GetNumber() = 0;
				/// устанавливает номер группы текст блок катов
				virtual void SetNumber(unsigned int uiNumber) = 0;

				/// вес содержащегося индекса в процентах от нормы (норма равна 100 процентов)
				unsigned int m_IndexesPercentage;
				/// тип содержащегося индекса
				SS::Core::Types::IndexationSearch::EIndexesType m_IndexesType;
			};

			/// "индекс на позиции" индекс на одной позиции в тексте
			/**
				включает в себя индексируемый и сохраняемый индексы, располагающиеся на одной позиции текстового блока.
				используется при индексации и при поиске
			*/
			class IIndexAndPosition
			{
			public:

				virtual ~IIndexAndPosition(){};
				/// получение позиции индекса в текстовом блоке
				virtual unsigned int GetPosition() = 0;
				/// установление позиции индекса в текстовом блоке
				virtual void SetPosition(unsigned int uiPosition) = 0;

				/// получение порядкового номера в тексте первого символа слова
				virtual unsigned int GetFirstCharPosition() = 0;
				/// установление порядкового номера в тексте первого символа слова
				virtual void SetFirstCharPosition(unsigned int uiPosition) = 0;

				/// вернет сохраняемый индекс
				virtual IServiceIndex* GetServiceIndex() = 0;
				/// вернет индексируемый индекс
				virtual IIndexationIndex* GetIndexationIndex() = 0;

				/// установление слова
				virtual void SetWord(const wchar_t* szWord) = 0;
				///вернет само слово (для вывода пользователю)
				virtual const wchar_t* GetWord() = 0;

				/// вернет тип подсветки индекса
				virtual SS::Core::Types::IndexationSearch::EHighlightType GetHighlightType() = 0;
				/// установит тип подсветки индекса
				virtual void SetHighlightType(SS::Core::Types::IndexationSearch::EHighlightType hlt) = 0;

				/// вернет тип подсветки индекса
				virtual unsigned int GetIsInAnswerSentence() = 0;
				/// установит тип подсветки индекса
				virtual void SetIsInAnswerSentence(unsigned int uiIn) = 0;
			};

			/// "сохраняемый индекс" инкапсулирует индекс, сохраняемый/доставаемый в сервисной базе
			/**
				всегда входит в состав IIndexAndPosition (т.е. на одной позиции-один такой индекс)
				обязательно включает в себя индекс для отображения, а также может включать любую дополнительною сохраняемую информацию
			*/
			class IServiceIndex
			{
			public:
				virtual ~IServiceIndex(){};
				/// вернет индекс для отображения
				virtual SS::Core::Types::IndexationSearch::TViewIndex* GetViewIndex() = 0;
				/// вернет буфер дополнительной информации 
				virtual void GetInfoBuff(char** ppBuff, unsigned int* pSize) = 0;
				/// запишет себе буфер дополнительной информации 
				virtual void SetInfoBuff(char* pBuff, unsigned int Size) = 0;
			};

			/// "индексируемый индекс" инкапсулирует индекс, индексируемый индексным процессором
			/**
				всегда входит в состав IIndexAndPosition (т.е. на одной позиции-один такой индекс)
				может включать в себя несколько числовых индексов - каждый из них проиндексируется на одной и той же позиции
			*/
			class IIndexationIndex
			{
			public:
				virtual ~IIndexationIndex(){};
				/// получение первого индекса коллекции (0 - если нет)
				virtual unsigned int GetFirstIndex() = 0;
				/// получение очередного индекса коллекции (0 - если закончились)
				virtual unsigned int GetNextIndex() = 0;

				/// очистка коллекции (удалит все индексы)
				virtual void Clear() = 0;
				/// добавление индекса в коллекцию
				virtual void AddIndex(unsigned int uiIndex) = 0;
			};

	  }
	}
  }
}