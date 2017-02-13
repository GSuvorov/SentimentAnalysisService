#pragma once
#include "..\ASCInterface\iinterface.h"
//--------------------------------------------------------------------//
/// guid дл€ класса, поддерживающего IHtmlData
static const GUID CLSID_HtmlData =  { 0x28ad6bc1, 0x8431, 0x49e6, { 0x9e, 0xe4, 0x27, 0xfe, 0xba, 0x02, 0x1e, 0x12 } };
/// guid дл€ интерфейса IHtmlData
static const GUID IID_HtmlData =  { 0xf11090b5, 0x3307, 0x4f27, { 0x95, 0x5d, 0xdc, 0xbe, 0x8c, 0x18, 0xea, 0x43 } };
/// guid дл€ класса, поддерживающего IHtmlParser
static const GUID CLSID_HtmlParser =  { 0xf2371f7f, 0xee13, 0x42a0, { 0x96, 0x1d, 0xf3, 0xfa, 0xc6, 0xe2, 0x96, 0x72 } };
/// guid дл€ интерфейса IHtmlParser
static const GUID IID_HtmlParser =  { 0x6f7dde8b, 0x7638, 0x4af1, { 0x83, 0x60, 0xdd, 0x4b, 0x94, 0x25, 0xb5, 0x6b } };
//--------------------------------------------------------------------//
namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace  PluginsUtils
			{
				//--------------------------------------------------------------------//
				// интерфейс дл€ работы с парсером HTML документов
				class IHtmlData : public SS::Interface::IBase
				{
					public:
						virtual void Init() = 0;
						virtual void AddConstraints(int iKey, int iValue) = 0;
						virtual bool GetStringValue(int iKey, const wchar_t** pValue) = 0;
						virtual bool GetIntValue(int iKey, unsigned int* iValue) = 0;
						virtual bool SetCurrentTable(unsigned int iTableNum) = 0;
						virtual bool SetCurrentCell(unsigned int iCellNum) = 0;
				};
				//--------------------------------------------------------------------//
				///интерфейс парсинга HTML документов.
				class IHtmlParser : public SS::Interface::IBase
				{
					public:
						virtual bool ParseHtml(	const unsigned char* pHtmlContent,
												unsigned int iContentLen,
												SS::Interface::Core::PluginsUtils::IHtmlData* pData) = 0;
				};
				//--------------------------------------------------------------------//
			}
		}
	}
}

//--------------------------------------------------------------------//

// пользовательские типы маркировки
// нет маркировки
#define MarkNone		0
// заголовок 1-го уровн€
#define MarkH1			1
// заголовок 2-го уровн€
#define MarkH2			2
// заголовок 3-го уровн€
#define MarkH3			3
// заголовок 4-го уровн€
#define MarkH4			4
// заголовок 5-го уровн€
#define MarkH5			5
// заголовок 6-го уровн€
#define MarkH6			6
// €чейка таблицы
#define MarkCell		7
// определение
#define MarkDefinition	8
// определ€емый термин
#define MarkDefTerm		9

// идентификаторы строк
// текст из HTML
#define StrText			1
// заголовок
#define StrTitle		2
// описание
#define StrDescription	3
// ключевые слова
#define StrKeyword		4
// автор
#define StrAuthor		5
// кодировка
#define StrCharset		6
// название текущей таблицы
#define StrTableName	7
// заголовок текущей таблицы
#define StrTableHeader	8
// текст текущей €чейки
#define StrCellText		9


// идентификаторы чисел
/// число таблиц
#define IntNumOfTables		10
/// число €чеек в таблице
#define IntNumOfCells		11
/// начало текста таблицы
#define IntTableStart		12
/// конец текста таблицы
#define IntTableEnd			13
/// начало текста таблицы
#define IntTableMaxRow		14
/// конец текста таблицы
#define IntTableMaxCol		15
/// строка
#define IntRow				51
/// столбец
#define IntCol				52
/// раст€жение строки
#define IntRowSpan			53
/// раст€жение столбца
#define IntColSpan			54
/// начало текста €чейки
#define IntCellStart		55
/// длина текста €чейки
#define IntCellLength		56
/// €чейка - заголовок 
#define IntCellHeader		57
/// €чейка с пустым текстом
#define IntCellEmptyText	58





// ограничени€
/// макс количество строк
#define  CtrMaxRows				1
/// макс количество столбцов
#define  CtrMaxCols				2
/// минимальное количество строк
#define  CtrMinRows				3
/// минимальное количество столбцов
#define  CtrMinCols				4
/// линки в таблице 1 Ц запрещены
#define  CtrDisableLinks		5
/// скрипты в таблице 1 Ц запрещены
#define  CtrDisableScripts		6
/// изображени€ true Ц заперщены
#define  CtrDisableImg			7
/// допустимое число пустых строк (0-100 %)
#define  CtrEmptyRows			8
/// допустимое число пустых столбцов (0-100 %)
#define  CtrEmptyCols			9
/// допустимое число непустых строк (шт)
#define  CtrNonEmptyRows		10
/// допустимое число непустых столбцов (шт)
#define  CtrNonEmptyCols		11
/// перва€ строка об€зательно заполнена 1=да
#define  CtrFullFirstRow		12
/// максимальный размер текста в €чейке
#define  CtrMaxCellTextSize		13
/// 1=режим отладки - в датасет записываетс€ причина отбраковки таблицы
#define  CtrDebugMode			14

//--------------------------------------------------------------------//