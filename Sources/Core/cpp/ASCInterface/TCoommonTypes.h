#pragma once
#include <list>
#include <string>

namespace SS
{
	namespace Core
	{
		namespace Types
		{
			namespace IndexationSearch
			{

#pragma pack(push,1)  // Fix packing size 
				/// структура, описывающая индекс для отображения
				struct TViewIndex
				{
					int				m_DictionaryIndex;
					unsigned char	m_IDForm;
					TViewIndex() : m_IDForm(0), m_DictionaryIndex(0)
					{}
				};
#pragma pack(pop)

				///тип тект ката
				enum ETextCutType
				{
					/// текст
					tctText,
					/// заголовок
                    tctHeader,
					/// таблица
                    tctTable,
					/// url-адрес
					tctHttp
				};

				///тип индекса (совокупности индексов)
				enum EIndexesType
				{
					///обычный
					itNormal,
					///корректирующий
                    itAdjustment
				};

				typedef std::list<std::wstring> TAbsentWords;

				/// тип подсветки индекса
				enum EHighlightType{
					/// обычный индекс
					hltNone,
					/// слово из запроса
					hltQueryWord,
					/// слово ответа
					hltAnswerWord
				};

				///тип индексного процессора
				enum EIndexProcessor
				{
					///локальный
					ipLocal,
					///глобальный
					ipGlobal
				};

				/// текст (используется для подачи текста запроса, также служит базовым контейнером для текста индексации)
				class TNativeText
				{
				public:
					TNativeText() : m_wcText(0)	{};
					/// вернет текст
					const wchar_t* GetText()
					{
						return m_wcText;
					}
					/// примет текст
					void SetText(const wchar_t* wc)
					{
						m_wcText = wc;
					}
				protected:
					const wchar_t* m_wcText;
				};

				/// специальный контейнер текста для индексации
				class TNativeTextForIndexation : public TNativeText
				{
				public:
					TNativeTextForIndexation() : m_wcStructure(0), m_wcMeta(0){}

					/// вернет информацию о структуре текста
					const wchar_t* GetTextStructure()
					{
						return m_wcStructure;
					}
					/// примет информацию о структуре текста
					void SetTextStructure(const wchar_t* wc)
					{
						m_wcStructure = wc;
					}

					/// вернет мета-информацию файла-документа
					const wchar_t* GetTextInfo()
					{
						return m_wcMeta;
					}
					/// примет мета-информацию файла-документа
					void SetTextInfo(const wchar_t* wc)
					{
						m_wcMeta = wc;
					}
				protected:
					const wchar_t* m_wcStructure;
					const wchar_t* m_wcMeta;
				};

				/// параметры глубины анализа
				class AnalyseDepthParams
				{
				protected:
					bool m_bIsMorpho;
					bool m_bIsSemantic;
					bool m_bIsSyntax;
					bool m_bIsSynonims;
				public:
					AnalyseDepthParams() : m_bIsMorpho(false), m_bIsSemantic(false), m_bIsSyntax(false), m_bIsSynonims(false) {}
				public:
					/// вернет наличие/отсутствие морфологического анализа
					bool IsMorpho()	{return m_bIsMorpho;}
					/// вернет наличие/отсутствие семантического анализа
					bool IsSemantic() {return m_bIsSemantic;}
					/// вернет наличие/отсутствие синтаксического анализа
					bool IsSyntax()	{return m_bIsSyntax;}
					/// вернет наличие/отсутствие анализа на синонимы
					bool IsSynonims() {return m_bIsSynonims;}

					/// установит наличие/отсутствие морфологического анализа
					void SetIsMorpho(bool bIsMorpho){m_bIsMorpho = bIsMorpho;}
					/// установит наличие/отсутствие семантического анализа
					void SetIsSemantic(bool IsSemantic) {m_bIsSemantic = IsSemantic;}
					/// установит наличие/отсутствие синтаксического анализа
					void SetIsSyntax(bool bIsSyntax) {m_bIsSyntax = bIsSyntax;}
					/// установит наличие/отсутствие анализа на синонимы
					void SetIsSynonims(bool IsSynonims) {m_bIsSynonims = IsSynonims;}
				};


				/// параметры, передаваемые лингвистическому процессору при индексации
				class TIndexationParams
				{
				};

				
				///	Настройки для модуля параметризатора запроса (NdxQuery.dll)
				struct TQueryParametrizerSettings
				{
				public:
					///	ctr
					TQueryParametrizerSettings():
						ndxq_EnableObligatoryFlag(1)	//	по умолчанию работает
						,ndxq_EnableUnionsWords(1)		//	по умолчанию работает
					{}

					unsigned int
						///	Работает флаг обязательности или нет
						///	(устанавливать этот флаг в словах блоков и последовательностей)
						ndxq_EnableObligatoryFlag	: 1,
						///	Учитывать union-слова (добавлять информацию о них в последовательности)
						///	или нет
						ndxq_EnableUnionsWords		: 1;
				};

				/// параметры поиска, передаваемые в процессоры поиска
				class TSearchParams
				{
				public:
					/// тип области поиска
					enum ESearchArea
					{
						/// обычный режим
						saNormal,
						/// поиск в энциклопедии
						saEncyclopedia
					};

					/// тип области поиска
					ESearchArea m_SearchArea;

					/// тип языка запроса (используется для Glo)
					enum ESearchLanguage
					{
						/// русский
						slRus,
						/// английский
						slEng,
						slChinese,
						slSpanish,
						slPushtu,
						slEsperanto
					};

					/// язык запроса
					ESearchLanguage m_SearchLanguage;

					///режимы поиска в найденном
					enum ESearchInObtainedMode
					{
						esiomNone,
						///по в среди найденных текстов
						esiomText
					};
					///тип поиска в найденном
					ESearchInObtainedMode m_SearchInObtainedMode;

					TSearchParams(int uiMaxSearchResultsCount=0)
						: m_uiMaxSearchResultsCount(uiMaxSearchResultsCount)
						, m_SearchArea(saNormal)
						, m_bForceOrSearch(false)
						, m_SearchInObtainedMode(esiomNone)
						, m_SearchLanguage(slRus)
					{};

					///ограничение на количество найденных предложений
					unsigned int m_uiMaxSearchResultsCount;

					bool m_bForceOrSearch;

					///	Настройки для модуля NdxQuery (параметризатор запросов)
					TQueryParametrizerSettings	m_QPSettings;
				};

				/// Способ расширения/сужения области поиска
				enum ESearchDepth
				{
					/// стандартный режим - без расширения
					sdNormal,
					/// поиск строго по написанию (если поддерживается)
					sdStrict,
					/// поиск с учетом синонимов слов (если поддерживается)
					sdSynonyms,
					/// поиск с отбрасыванием малозначащих слов
					sdRejectUnimportant
				};

				/// режим ответа
				enum EAnswerMode
				{
					/// автоматический - определяется лингвистическим процессором по виду запроса
					amAuto,
					/// принудительный поиск по ключевым словам
					amOnlyKeywords
				};

				/// режим генерации запроса
				enum EGenerateQueryMode
				{
					/// генерируются числовые индексы
					gqmIndex,
					/// генерируются строки
                    gqmString					
				};

				/// параметры анализа запроса пользователя
				class TQueryParams
				{
				public:
					TQueryParams()
						: m_SearchDepth(sdNormal)
						, m_AnswerMode(amAuto)
						, m_bIgnoreAbsentWords(false)
						, m_wcQuery(L"")
						, m_GenerateQueryMode(gqmIndex)
					{
					}

					/// если в тру, то наличие отсутствующих слов игнорируется
					bool m_bIgnoreAbsentWords;

					/// Способ расширения/сужения области поиска
					ESearchDepth m_SearchDepth;
					/// режим ответа
					EAnswerMode m_AnswerMode;
					/// режим генерации запроса
					EGenerateQueryMode m_GenerateQueryMode;

					const wchar_t* GetQuery()
					{
						return m_wcQuery;
					}
					/// примет текст
					void SetQuery(const wchar_t* wc)
					{
						m_wcQuery = wc;
					}
				protected:
					const wchar_t* m_wcQuery;
				};

				/// параметры, передаваемые лингвистическому процессору при подготовке результатов поиска к отображению
				struct TPrepareForViewParams
				{
					/// номер первого ITextBlock, подлежащего обработке
					unsigned int m_uiFirstProcessedTextBlockNumber;
					/// количество ITextBlock, подлежащих обработке
					unsigned int m_uiProcessedTextBlockCount;
				};

				/// параметры, передаваемые лингвистическому процессору при подсчете лингвистической релевантности
				class TUpdateRelevanceParams
				{
				};

				/// режим работы лингвистического процессора
				class TLinguisticProcessorMode
				{
					AnalyseDepthParams m_AnalyseDepthParams;
				public:
					TLinguisticProcessorMode() : m_bIsLiteMode(false)
					{
					}

					bool m_bIsLiteMode;
					/// параметры глубины анализа
					AnalyseDepthParams* GetAnalyseDepthParams()
					{
						return &m_AnalyseDepthParams;
					}

					void Init()
					{
						m_bIsLiteMode = false;
					}
				};
			}
		}
	}
}