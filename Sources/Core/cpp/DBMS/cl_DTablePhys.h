#pragma once

#include "..\ASCInterface\IDBMS.h"
#include "fn_Generic.h"
#include "cl_Topo_DBMS.h"
#include "cl_DiskFile.h"
#include "cl_SyncroObjects.h"
#include "cl_DTablePhysTree.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_DTablePhys.h"

using namespace SS::Interface::Core::DBMS;

///	Заглушка указателя на данные пользователя
#define DUMMY_USER_ARRAY_OF_POINTERS_TO_DATA   (VOID **)(NULL+1)

///	Версия DBMS
#define DBMS_VERSION   350

///	Имя файла хэш-таблицы
#define F_NAME_HASH     _T("DT_HashArr.dta")

///	Имя файла записей
#define F_NAME_RECORDS  _T("DT_RecordsArr.dta")

///	Имя файла строковых и векторных величин
#define F_NAME_VALUES   _T("DT_ValuesArr.dta")

///	Имя файла статистики
#define F_NAME_STAT     _T("DT_Statistics.txt")

///	Поцент удаленных записей при котором вызывается процедура Shrink()
#define SHRINK_PERCENT                         25

///	Размер считываемых данных при первой попытке считать строку или вектор
#define STR_BUFF_INI_SIZE                      32

///	Максимальный размер строки или вектора
#define STR_BUFF_MAX_SIZE                     (64*1024) - 1

///	Размер внутренней информации о таблице
#define SIZE_OF_TABLE_INFO                     512   // Size of RECORD_PTR record

///	Размер указателя на запись
#define SIZE_OF_RECORD_OFFSET                  4   // Size of RECORD_PTR record

///	Размер данных или указателя на данные
#define SIZE_OF_VALUE_OFFSET                   4   // Size of NAME_PTR field of RECORD_UNIT

///	Размер указателя на следующий элемент
#define SIZE_OF_NEXT_OFFSET                    4   // Size of NEXT_PTR field of RECORD_UNIT

///	Размер стандартной ячейки таблицы
#define SIZE_OF_RECORD_UNIT                    (SIZE_OF_VALUE_OFFSET + SIZE_OF_NEXT_OFFSET) // Size of full RECORD_UNIT

///	Маска для старших битов зависи в хэш-таблице (старшие биты хэш-ключа)
#define HASH_IDX_HIGH_BITS_MASK                0xFF000000

///	Размеры хеш блоков начнут увеличиваться с указанного номера
#define HASH_BLOCKS_GROW_UP_START              2

///	Пустая запись в хэш-таблице
#define ZERO_HASH_RECORD                       0xFFFFFFFF

///	Пустое смещение следующей (NEXT) записи в таблице
#define ZERO_NEXT_RECORD_IDX                   0xFFFFFFFF

///	Макс. число идентификаторов поиска
#define FIND_LIM                               64

///	Макс. число идентификаторов просмотрв
#define SCAN_LIM                               64

///	Макс. число идентификаторов поиска в интервале
#define SPAN_FIND_LIM                          16

///	Макс. число сохраненных пользовательских значений
#define USER_VALUES_LIM                        16

/////	Заглушка <FinfId> для функции Scan()
//#define GET_RECORD_DATA_FIND_ID_FOR_SCAN       FIND_LIM


/*! \struct SHashBlockInfo
	*  \brief Внутрняя структура класса DTablePhysC
*/
struct SHashBlockInfo   {
	/// Число реальных элементов в блоке
	UINT32 uiHashBlockSize;

	/// Число позиций для элементов в блоке
	UINT32 uiHashBlockVolume;

	/// Размер блока в байтах
	UINT32 uiHashBlockVolumeInBytes;

	/// Число реальных элементов могущих поместиться предыдущих и текущем блоках
	UINT32 uiHashBlockGlobalLimit;

	/// Смещение начала блока
	UINT32 uiOffsetOfStartOfHashBlock;

	/// Смещение конца блока
	UINT32 uiOffsetOfEndOfHashBlock;
};

/*! \struct SRecordField
	*  \brief Информация в ячейке таблицы (Внутрняя структура класса DTablePhysC)
*/
struct SRecordField   {
	/// Смещение в файле строк или числовое значение
	UINT32 StrValueOffsetOrNumValue;

	/// Смещение следующей (NEXT) записи в таблице
	UINT32 NextRecordOffset;
};

/*! \struct SFieldInfo
	*  \brief Информация о поле таблицы (Внутрняя структура класса DTablePhysC)
*/
struct SFieldInfo   {

	/// Тип данных
	EFieldDataTypes  eType;

	/// Индекс поля
	UINT32          iIndex;

	/// Размер поля
	UINT32          iSize;

	/// Ориентировочная длина считываемых данных (std:string & std:vector)
	UINT16          wValueMiddleSize;

	/// Поле имеет ссылку на следующий элемент
	BOOL            bHasNextFlag;

	/// По полю нет поиска
	BOOL            bNoFindFlag;

	/// Поле - уникальное
	BOOL            bIsUnicFlag;

	/// Поле - данные
	BOOL            bIsDataFlag;

	/// Поле - проиндексировано
	BOOL            bIndexedFlag;

	/// Смещение данных поля в записи
	UINT32          iRecordFieldBegOffset;

	/// Указатель на данные в записи
	SRecordField  * pRecordFieldPtr;

	/// Индексный файл
	DiskFileC       oIndexFile;

	/// Индексное дерево в памяти
	CIndexTree      oIndexTree;

	/// Указатель на имя поля
	CONST _TCHAR  * pszFieldName;

	/// Указатель на имя типа поля
	CONST _TCHAR  * pszTypeName;


};

#pragma pack(push,8) // ???

/*! \struct SDTableInfo
	*  \brief Внутрняя информация о таблице данных
*/
struct SDTableInfo   {
	/// Версия DBMS
	UINT32 m_DbmsVersion;

	/// Контрольная сумма таблицы
	UINT32 m_TableCheckSum;

	/// Общая длина данных
	UINT32 m_DataGlobalLen;

	/// Число Hash-блоков
	UINT32 m_HashBlocksNum;

	/// Число "хешированных" элементов
	UINT32 m_HashElemsNum;

	/// Число записей
	UINT32 m_RecordsNum;

	/// Число записей помеченных на удаление
	UINT32 m_DeletedRecordsNum;

	/// Статистика: число поисков в считанном хэш-блоке
	UINT32 m_ReTryNum;

	/// Статистика: число повторных считываний хэш-блока
	UINT32 m_ReReadNum;

	/// Статистика: число повторных считываний длинных данных при коллизиях
	UINT32 m_ReValueNum;

	/// Сохраненные данные пользователя
	UINT32 m_UserValues[USER_VALUES_LIM];

};

#pragma pack(pop)


/*! \class DTablePhysC
	*  \brief Физический образ DataTable
*/
class DTablePhysC   : public SDTableInfo  {

	/// Дружественный класс
	friend class DTableTopoC;

	/// Дружественный класс
	friend class DFieldTopoC;

	public: //

		/// Конструктор
		DTablePhysC();

		/// Деструктор
		~DTablePhysC();

	public: //

		/// Начальная инициализация
		HRESULT SetDTFilesPathsAndNames(VOID);

		/// Открыть или создать DataTable. Return: HRESULT
		HRESULT Open(VOID);

		/// Закрыть DataTable. Return: HRESULT
		HRESULT Close(BOOL bClearAfterClose = FALSE);

		/// Сохранить BACKUP-копию таблицы
		HRESULT SaveBackup(BOOL bSaveAnyTable = FALSE);

		/// Очистить таблицу
		HRESULT Clear(BOOL bClearUserInitializations = TRUE);

		/// Очищает таблицу при условии, что счетчик числа открытий равен нулю\n
		HRESULT TryClear(VOID);

		/// Инициализация процедуры добавления
		HRESULT AddInit(VOID * ppArrayOfPointersToData[]);

		/// Выделение памяти под переменные строки таблицы и массив указателей на эти переменные
		VOID **  AllocRecordDataStorage(VOID);

		/// Освобождение памяти переменных строки таблицы и массиа указателей на эти переменные
		HRESULT  FreeRecordDataStorage(VOID ** ppvppArrayOfPointersToData);

		/// Инициализация изменения
		HRESULT ChangeInit(VOID * ppArrayOfPointersToData[]);

		/// Добавление записи в таблицу
		HRESULT Add(VOID);

		/// Инициализация процедуры поиска
		HRESULT FindInit(_TCHAR * pKeyName,  VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD nFindId = 0);

		/// Инициализация процедуры поиска
		HRESULT FindInit(UINT32   nKeyIndex, VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD nFindId = 0);

		/// Инициализация процедуры поиска с возвратом идентификатора поиска
		HRESULT FindInitEx(_TCHAR * pKeyName,  VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD * pnFindId);

		/// Инициализация процедуры поиска с возвратом идентификатора поиска
		HRESULT FindInitEx(UINT32   nKeyIndex, VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD * pnFindId);

		/// Освобождение идентификатора поиска
		HRESULT FindCancel(WORD nFindId = 0);

		/// Поиск записи в таблице
		HRESULT Find(WORD nFindId = 0);

		/// Поиск и замена данных
		HRESULT FindAndChange(WORD nFindId = 0);

		/// Поиск будет начат заново
		HRESULT FindStart(WORD nFindId = 0);

		/// Останов текущего поиска
		HRESULT FindStop(WORD nFindId = 0);

		/// Поиск одной записи
		HRESULT FindOne(WORD nFindId = 0);

		/// Поиск одной записи и изменение ея
		HRESULT FindOneAndChange(WORD nFindId = 0);

//		HRESULT FindOneAndInc(WORD nFindId = 0);

		/// Инициализация процедуры сканирования
		HRESULT ScanInit(UINT32 nStartRecordIdx, VOID * ppArrayOfPointersToData[], WORD nScanId = 0);

		/// Инициализация процедуры сканирования
		HRESULT ScanInitEx(UINT32 nStartRecordIdx, VOID * ppArrayOfPointersToData[], WORD * pnScanId);

		/// Отказ от просмотра по заданному nScanId
		HRESULT ScanCancel(WORD nScanId = 0);

		/// Установка начальной позиции для сканирования
		HRESULT ScanStart(UINT32 nStartRecordIdx, WORD nScanId = 0);

		/// Последовательное сканирование записей
		HRESULT Scan(WORD nScanId = 0);

		/// Последовательное сканирование записей и изменение их
		HRESULT ScanAndChange(WORD nScanId = 0);

		/// Сканирование одной записи
		HRESULT ScanOne(UINT32 nRecordIdx, WORD nScanId = 0);

		/// Сканирование одной записи и изменение ея
		HRESULT ScanOneAndChange(UINT32 nRecordIdx, WORD nScanId = 0);

		/// Получение индекса ранее найденой записи таблицы
		UINT32 GetFoundIdx(WORD nFindId = 0);

		/// Получение индекса ранее сосканированной записи таблицы
		UINT32 GetScanedIdx(WORD nScanId = 0);

		/// Удаление записи
		HRESULT DeleteRecord(UINT32 nRecordIdx);

		/// Получение числа записей в таблице
		UINT32 GetRecordsNumber(VOID);

		/// Получение числа удаленных (помеченых на удаление) записей в таблице\n
		UINT32 GetDeletedRecordsNumber(VOID);

		/// Сжатие данных таблицы. Исключение записей помеченых на удаление
		HRESULT Shrink(VOID);

		/// Сохранить в данных таблицы пользовательское значение (одно из 16-и возможных). <iValueIdx> - индекс значения [0..15]
		HRESULT SetUserValue(UINT iValueIdx, UINT32 uiValue);

		/// Получить из данных таблицы пользовательское значение (одно из 16-и возможных). <iValueIdx> - индекс значения [0..15]
		UINT32  GetUserValue(UINT iValueIdx);

		/// Инициализация процедуры поиска в заданном интервале (включая границы)
		HRESULT SpanFindInit(
			_TCHAR             * pKeyName,
			VOID               * pLowerKeyValue,
			VOID               * pUpperKeyValue,
			std::set< UINT32 > * pSetOfRecordsIndexes,
			WORD                 nFindId = 0,
			std::set< UINT32 > * pSetToIntersectWith = NULL
			);

		/// Те же действия, что и в SpanFindInit(), но задается не имя ключевого поля, а индекс 
		HRESULT SpanFindInit(
			UINT32               nKeyIndex,
			VOID               * pLowerKeyValue,
			VOID               * pUpperKeyValue,
			std::set< UINT32 > * pSetOfRecordsIndexes,
			WORD                 nFindId = 0,
			std::set< UINT32 > * pSetToIntersectWith = NULL
			);

		/// Те же действия, что и в SpanFindInit(), за исключением того, что функции сами определяют значение nFindId
		HRESULT SpanFindInitEx(
			_TCHAR             * pKeyName,
			VOID               * pLowerKeyValue,
			VOID               * pUpperKeyValue,
			std::set< UINT32 > * pSetOfRecordsIndexes,
			WORD               * pnFindId,
			std::set< UINT32 > * pSetToIntersectWith = NULL
			);

		/// Те же действия, что и в SpanFindInit(), за исключением того, что функции сами определяют значение nFindId
		HRESULT SpanFindInitEx(
			UINT32               nKeyIndex,
			VOID               * pLowerKeyValue,
			VOID               * pUpperKeyValue,
			std::set< UINT32 > * pSetOfRecordsIndexes,
			WORD               * pnFindId,
			std::set< UINT32 > * pSetToIntersectWith = NULL
			);


		///
		HRESULT SpanFindCancel(WORD nFindId);

		/// Получение индексов записей таблицы (по значениям ключа, указанного в SpanFindInit())
		HRESULT SpanFind(WORD nFindId = 0);


	public:

		/// Переключение на другую базу данных
		HRESULT Switch(VOID);

		/// Установить начальный размер таблицы
		VOID    SetStartSize(UINT32 iStartSize);

		/// Установить величину по достижении которой создается BACKUP-копия таблицы
		VOID    SetSaveAfterValue(UINT32 iSaveAfterValue)   {	m_SaveAfterValue=iSaveAfterValue; }

		/// Установить режим роста
		VOID    SetGrowModeS(_TCHAR * pModeStr);

		/// Установить режим роста
		VOID    SetGrowModeI(UINT32 iModeVal)   { m_HashGrowMode = iModeVal; }

		/// Пересчитать размер хаш-таблиц
		VOID    ReCountHashInfoArray(VOID);

		/// Установить предопределенный путь
		HRESULT SetDynamicInfo(CONST _TCHAR * pszDynamicPath);

		/// Создать директорию
		static  HRESULT MkDir(CONST _TCHAR * pDirName);

		/// Проверить существование директории
		static  BOOL ChkDirExistence(CONST _TCHAR * pDirName);

		std::string  m_sTableIniStructure;

	private:

		/// Файл хеш-таблицы
		DiskFileC fH;

		/// Файл записей
		DiskFileC fR;

		/// Файл строковых и векторных данных
		DiskFileC fV;

		/// Файл статистики
		DiskFileC fS;

	private: //

		/// Путь к файлам таблицы
		STLSTR             m_Path;

		/// Путь к динамической таблице
		STLSTR             m_DynamicPath;

		// Имя динамической таблицы
		// STLSTR          m_DynamicName;

		/// Иерархические данные таблицы
		DTableTopoC      * m_TopoPtr;

		/// Информация о хеш-блоках
		SHashBlockInfo   * m_HashInfoArr;

		/// Информация о полях
		SFieldInfo       * m_FieldsInfoArr;

		/// Число полей (общее)
		UINT32             m_FieldsNum;

		/// Число ключевых полей (общее)
		UINT32             m_KeyFieldsNum;

		/// Запись таблицы в байтовом формате
		SRecordField     * m_RecordArr;

		/// Размер записи таблицы
		UINT32             m_RecordSize;

		/// Указатель на новую запись
		UINT32             m_RecordNewPtr;

		/// Индекс нового значения в файле строковых и векторных данных
		UINT32             m_ValueNewPtr;

		/// Данные нового значения
		BYTE             * m_ValueBuff;

		/// Длина буфера данных значения
		UINT32             m_ValueBuffLen;

		/// Флаг очистки для мерода Close()
		BOOL               m_ClearFlag;

		/// Флаг удаления установок/инициализаций пользователя
		BOOL               m_bClearUserInitializations;

		/// Таблица  - динамическая
		BOOL               m_bTableIsDynamic;

		/// Таблица  - времнная
		BOOL               m_bTableIsTemporary;

		/// Таблица имеет уникальные поля
		BOOL               m_bTableHasUnicFields;

		//BOOL             m_bIncFlag;

		/// Текущий идентификатор поиска
		WORD               m_Find_CurrId;

		/// Индексы ключевых полей (для разных FindId)
		UINT32             m_Find_RecordFieldIdx[FIND_LIM+1];

		/// Cvtotybz найденых записей (для разных FindId)
		UINT32             m_Find_RecordNextOffs[FIND_LIM+1];

		/// Индексы  найденых записей (для разных FindId)
		UINT32             m_Find_RecordFoundIdx[FIND_LIM+1];

		/// Режимы ответа (Find/FindOne/FindStart) (для разных FindId)
		BOOL               m_Find_AnswerNoMore[FIND_LIM+1];

		/// Указатели на ключи поиска переданные в FindInit() (для разных FindId)
		CONST VOID       * m_Find_KeyValuePtr[FIND_LIM+1];

		// ------------------------------------------------------------

		/// Указатели на массивы переменных переданные в FindInit() (для разных FindId)
		VOID            ** m_Find_ppArrayOfPointersToData[FIND_LIM];

		/// Указатель на массив переменных переданный в AddInit()
		VOID            ** m_Add_UserDataPointers;

		/// Указатель на массив переменных переданный в ChangeInit()
		VOID            ** m_Change_UserDataPointers;

		/// Указатели на массивы переменных переданные в ScanInit()
		VOID            ** m_Scan_ppArrayOfPointersToData[SCAN_LIM];

		/// Индекс текущей сканируемой записи
		UINT32             m_Scan_RecordIdx[SCAN_LIM];

		/// Режим роста хеш-таблицы
		UINT32             m_HashGrowMode;

		/// Стартовый размер хеш-таблицы заданный пользователем
		UINT32             m_HashBlockStartUserSize;

		/// Стартовый размер хеш-таблицы (реальный)
		UINT32             m_HashStartVolume;

		/// Сохранять BACKUP-копию таблицы после каждых m_SaveAfterValue записей
		UINT32             m_SaveAfterValue;

		/// Счетчик записей до BACKUP-процедуры
		UINT32             m_SaveAfterCnt;


		/// Информация о таблице
		SDTableInfo      * m_InfoPtr;

	private: //

		/// Установить новую величину для указанного поля
		HRESULT SaveNewValue(SFieldInfo * pFieldInfo, BYTE * pValuePtr, WORD nValueLen);

		/// Поиск по ключу и возможное добавление
		HRESULT FindKeyAndAdd(SFieldInfo * pFieldInfo, BYTE * pValue, WORD nValueLen, BOOL bAddFlag = FALSE);

		/// Получить данные записи таблицы
		HRESULT GetRecordData(VOID * pDataPtrArr[], WORD nId, BOOL bFindMode);

		/// Установить данные записи таблицы
		HRESULT SetRecordData(VOID * pDataPtrArr[], WORD nId, BOOL bFindMode);

		/// Очистить ползовательскую информацию
		VOID    ClearUserSetings(VOID);

		/// Найти свободный идентификатор поиска
		BOOL    GetFreeFindId(WORD * pnFindId);

		/// Найти свободный идентификатор просмотра
		BOOL    GetFreeScanId(WORD * pnScanId);

		/// Найти свободный идентификатор Span-поиска
		BOOL    GetFreeSpanFindId(WORD * pnFindId);

		/// Подсчитать контрольную сумму, размер записи и т.п.
		VOID    SetInternalInfo(VOID);

		/// Критическая секция
		CritSectC          m_oCriticalSection;

		// -----------------------------------------------------

		/// Вспомогательное/временное хранилище значения
		UINT32             m_oNodeValue_UINT32;

		/// Вспомогательное/временное хранилище значения
		BYTE               m_oNodeValue_BYTE_ARR[10];

		/// Вспомогательное/временное хранилище значения
		std::string        m_oNodeValue_STRING;

		/// Вспомогательное/временное хранилище значения
		std::wstring       m_oNodeValue_WSTRING;

		/// Вспомогательное/временное хранилище значения
		std::vector<BYTE>  m_oNodeValue_VECTOR;

		/// Указатель на вспомогательное/временное хранилище значения
		BYTE             * m_pNodeValue_PTR;

		BYTE       * m_pCurrNodeValuePtr;
//		WORD         m_iCurrNodeValueLen;
		INT32        m_iCurrNodeHashIdx;
		INT32        m_iCurrNodeTableIdx;
		INT32        m_iCurrFoundRecordIdx;
		INT32        m_iCurrFoundRecordNextOffs;

		// -------------------------------------------------

		/// Индексы ключевых полей (для разных SpanFindId)
		UINT32             m_SpanFind_RecordFieldIdx[  SPAN_FIND_LIM];

		/// Указатели на нижние границы поиска переданные в SpanFindInit() (для разных SpanFindId)
		CONST VOID       * m_SpanFind_LowerKeyValuePtr[SPAN_FIND_LIM];

		/// Указатель тек. нижнюю границу поиска
		CONST VOID       * m_SpanFind_CurrLowerKeyValuePtr;

		/// Указатели на верхние границы поиска переданные в SpanFindInit() (для разных SpanFindId)
		CONST VOID       * m_SpanFind_UpperKeyValuePtr[SPAN_FIND_LIM];

		/// Указатель тек. верхнюю границу поиска
		CONST VOID       * m_SpanFind_CurrUpperKeyValuePtr;

		/// Указатель на STL::SET переданный в SpanFindInit()
		std::set<UINT32> * m_SpanFind_UserSetPointers[SPAN_FIND_LIM];

		/// Указатель на STL::SET переданный для операции INTERSECT
		std::set<UINT32> * m_SpanFind_IntersectPointers[SPAN_FIND_LIM];

		std::set<UINT32> * m_SpanFind_CurrIntersectPointer;

		/// Указатель на текущую информацию о поле данных 
		SFieldInfo       * m_SpanFind_FieldInfoPtr;
		
		/// Указатель на текущее дерево
		CIndexTree       * m_SpanFind_CurrBTreePtr;

		/// Указатель на массив текущего дерева
		// SBTreeNodeMem   * m_SpanFind_CurrBTreeArr; !!!

		/// Текущий FindId
		UINT32             m_SpanFind_FindId;

		// -------------------------------------------------------------

		/// Добавление новой записи к индексному файлу поля данных
		BOOL    NodeAdd(UINT32 iHashIdx);

		/// Внутреняя вставка
		INT32   NodeInsertR(UINT32 iNodeIdx, UINT32 iHashIdx, UINT32 m_iTreeHeight);

		/// "Расколоть" узел
		INT32   NodeSplit(UINT32 iNodeIdx);

		/// Поиск по заданному интервалу в индексном дереве (внутренний поиск)
		VOID    NodeSearchR(UINT32 iNodeIdx, UINT32 m_iTreeHeight, BOOL bNeedCheck = TRUE);
		
		/// Получить значение узла дерева
		HRESULT NodeGetValue(UINT32 iRecordNum);

		/// Получить значение узла дерева по HASH-индексу
		HRESULT NodeGetValueViaHash(UINT32 iHashIdx);

		// Сравнение "LowerOrEqual"
		// BOOL    NodeValueLE(SFieldInfo * pFieldInfo, BYTE * pValuePtr, WORD iValueLen);

		/// Сравнение "Lower"
		BOOL    NodeValueL(VOID);

		/// Оценка узла и сохранение номера записи таблицы
		BOOL    NodeEstimateAndSaveInfo(UINT32 iLinkToHashRecord, BOOL bNeedCheck = TRUE);

		/// Оценка узла на попадание в интервал
		UINT32  NodeCheckInterval(UINT32 iLinkToHashRecord);



};
