#pragma once

#include "IInterface.h"
#include <set>
#include <list>
#include <algorithm>
#include <functional>

// #include <tchar.h>
#undef      _TCHAR
#define     _TCHAR wchar_t

// --------------------------------------------------------------------------------
// GUIDs & IIDs :

/// Идентификатор класса DBMS
static const GUID CLSID_DBMS = { 0xac10aa0c, 0xa04a, 0x49de, { 0xb3, 0x21, 0xb8, 0x46, 0x7c, 0xc2, 0xb9, 0x2f } };

/// Идентификатор интерфейса DBMS (он-же - идентификатор интерфейса DBManager)
#define      IID_DBMS IID_DBManager // For compatibility 

/// Идентификатор интерфейса DBManager
static const GUID IID_DBManager = { 0xa97222fc, 0xd98d, 0x49f5, { 0xb9, 0x78, 0xc5, 0x97, 0xa9, 0x76, 0x7b, 0xd4 } };

/// Идентификатор интерфейса DBMS DataBase
static const GUID IID_DataBase = { 0xa66bf09b, 0x5a3c, 0x4b60, { 0x91, 0xe9, 0x42, 0xbb, 0xf1, 0xd5, 0xa1, 0xbd } };

// --------------------------------------------------------------------------------

namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace DBMS
			{

				// --------------------------------------------------------------------------------
				// Краткая декларация интерфейсных классов

//				class IBase;      // Базовый интерфейсный класс из "IBase.h"
				class IDBManager; // Интерфейс IDBManager (upper level)
				class IDataBase;  // Интерфейс IDataBase
				class IDataTable; // Интерфейс IDataTable (lower level)

				// --------------------------------------------------------------------------------
				// Полная декларация интерфейсных классов

				// --------------------------------------------------------------------------------
				// NOTE: 
				//
				//    Все методы интерфейсов возвращающие HRESULT, 
				//    возвращают только константы: S_OK, S_FALSE и E_FAIL.
				//    S_OK означает OK, S_FALSE означает отрицательный результат (при поиске),
				//    E_FAIL ознаяает серьезную ошибку, сопровождающуюся записью в журнале событий.

				// --------------------------------------------------------------------------------
				// 
				// --------------------------------------------------------------------------------

				/*! \interface IDBManager
				*   \brief     Интерфейс менеджера баз данных (DB Manager interface)
				*/
				class IDBManager : public virtual SS::Interface::IBase   {

				public:

					/// Возвращает флаг "задействованности"
					virtual BOOL IsOpen(VOID) = 0;

					/// Возвращает имя объекта
					virtual CONST _TCHAR * GetName(VOID) = 0;

					/// Читает конфигурационный файл и создает внутренние и дисковые структуры, располагающиеся по указанному pFullPathToDbmsBases пути
					/** 
						\param pFullPathToIniFile
						\param pFullPathToDbmsBases
						\return virtual HRESULT
					 
					Читает конфигурационный файл и создает внутренние и дисковые структуры, располагающиеся по указанному pFullPathToDbmsBases пути
					*/
					virtual HRESULT Init(CONST _TCHAR * pFullPathToIniFile, CONST _TCHAR * pFullPathToDbmsBases) = 0;

					/// Создает внутреннюю (НЕ дисковую) структуру с единственной базой данных "DynamicDataBase" (индекс 0) -  для работы только с динамическими таблицами  
					/** 
						\param pIDataBase - Если не NULL, то база "DynamicDataBase" будет открыта и указатель будет передан в вызывающую процедуру
						\return virtual HRESULT
					 
					Создает внутреннюю (НЕ дисковую) структуру с единственной базой данных "DynamicDataBase" (индекс 0) -  для работы только с динамическими таблицами  

					*/
					virtual HRESULT InitDynamic(OUT IDataBase ** ppIDataBase = NULL) = 0;

					/// Переключает текущий контекст DBMS на аналогичный по структуре. Вызывает метод Init(), в случае, когда он еще не вызывался.
					/** 
						\param pFullPathToIniFile
						\param pFullPathToDbmsBases
						\return virtual HRESULT
					 
					Переключает текущий контекст DBMS на аналогичный по структуре. Вызывает метод Init(), в случае, когда он еще не вызывался.
					*/
					virtual HRESULT Switch(CONST _TCHAR * pFullPathToIniFile, CONST _TCHAR * pFullPathToDbmsBases) = 0;

					/// Перемещает текущий_указатель на первую базу данных
					virtual HRESULT MoveFirstDataBase(VOID) = 0;

					/// Перемещает текущий_указатель на следующую базу данных
					virtual HRESULT MoveNextDataBase(VOID) = 0;

					/// Перемещает текущий_указатель на предыдущую базу данных
					virtual HRESULT MovePrevDataBase(VOID) = 0;

					/// Перемещает текущий_указатель на указанную (индексом) базу данных
					virtual HRESULT MoveToDataBase(UINT32  nDBaseIndex) = 0;

					/// Перемещает текущий_указатель на указанную (именем) базу данных
					virtual HRESULT MoveToDataBase(CONST _TCHAR * pDBaseName) = 0;

					/// Получает указатель на интерфейс базы данных в позиции текущего_указателя
					virtual IDataBase * OpenDataBase(VOID) = 0;

				};

				// --------------------------------------------------------------------------------
				// 
				// --------------------------------------------------------------------------------

				/// Перечисление типов данных полей таблиц (для IDataBase::GreateDataTable())
				typedef enum EFieldDataTypes   {

					TYPE_INT32   = 0, // 32-х_битовое_целое 
					TYPE_BYTE_10 = 1, // 10-и_байтовая_структура
					TYPE_STRING  = 2, // Строка std::string
					TYPE_VECTOR  = 3, // Вектор std::vector<BYTE>
					TYPE_WSTRING = 4, // Строка std::wstring
					TYPE_UINT32  = 5, // 32-х_битовое_беззнаковое_целое 

					TYPE_FINAL = 100  // Завершающий псевдо-тип

				};

				/// Перечисление типов модификаторов полей таблиц (для IDataBase::GreateDataTable())
				typedef enum EFieldModificatorTypes   {
					MOD_COMMON  = 0x0000, // Обычные значения. (произвольные значения c возможностью поиска).
					MOD_IS_UNIC = 0x0001, // Значения в указанном поле будут уникальными. Методы поиска будут возвращать не более одной записи, а метод добавления вернет S_FALSE в случае, когда заданное значение поля в таблице уже есть. Экономия памяти за счет отсутствия ссылок "NEXT". (Пример использования: индексы и порядковые номера)    
					MOD_NO_FIND = 0x0002, // Нет поиска по данному полю. Экономия памяти за счет отсутствия ссылок "NEXT". (Пример использования: словарные статьи и предложения, связанные со многими ключами)   
					MOD_IS_DATA = 0x0004, // Нет поиска по данному полю. Возможно изменение полей с типами данных INT32 и BYTE_10. Значения поступают в таблицу минуя HASH-функцию (повторяющиеся значения будут записаны многократно !!!). Экономия памяти за счет отсутствия ссылок "NEXT". (Пример использования: не повторяющиеся, не ключевые данные)   
					MOD_INDEXED = 0x0008, // Возможен поиск по заданному интервалу (SpanFind()). Используется как дополнение к другим модификаторам ( Пример: MOD_COMMON | MOD_INDEXED)
					MOD_FINAL   = 0x8000  // Завершающий псевдо-модификатор

				};

				/// Перечисление видов роста таблиц (для IDataBase::GreateDataTable())
				typedef enum ETableGrowTypes   {

					GROW_EVEN = 0, // Увеличивать на такой же размер
					GROW_DOWN = 1, // Уменьшать в 2 раза
					GROW_UP   = 2  // Увеличивать в 2 раза

				};

				/// Описание поля таблицы (для IDataBase::GreateDataTable())
				struct TDataTableFieldDescription   {
					_TCHAR *               szFieldName;
					UINT32                 etFieldType;        // See enum <EFieldDataTypes> above
					UINT32                 etFieldModificator; // See enum <EFieldModificatorTypes> above
				};

				// --------------------------------------------------------------------------------
				// 
				// --------------------------------------------------------------------------------

				/*! \interface IDataBase
				*   \brief     Интерфейс базы данных (DataBase interface)
				*/
				class IDataBase   { // : public virtual SS::Interface::IBase   {

				public:

					/// Возвращает флаг "задействованности"
					virtual BOOL IsOpen(VOID) = 0;

					/// Возвращает имя объекта
					virtual CONST _TCHAR * GetName(VOID) = 0;

					/// Очищает/реинициализирует базу данных
					/** 
						\param bClearUserInitializations
						\return virtual HRESULT
					 
					Очищает/реинициализирует базу данных\n
					Полученный ранее указатель на интерфейс базы данных остается действительным \n
					Полученные ранее указателели на интерфейсы таблиц данных остаются действительными \n
					Открытые таблицы остаются открытыми.\n
					Если bClearUserInitializations == TRUE, то выполненые пользователем инициализации (AddInit(), FindInit(), и пр.) теряются.
					*/
					virtual HRESULT Clear(BOOL bClearUserInitializations = TRUE) = 0;

					/// Очищает базу при условии, что нет открытых таблиц\n
					/** 
						\return virtual HRESULT : S_OK - база очищена, S_FALSE - база НЕ очищена, т.к. ее таблицы еще используют
					 
					Очищает базу при условии, что нет открытых таблиц\n
					*/
					virtual HRESULT TryClear(VOID) = 0;

					/// Завершает работу с полученым ранее указателем на интерфейс базы данных
					/** 
						\return virtual HRESULT
					 
					Завершает работу с полученым ранее указателем на интерфейс базы данных\n
					Полученный ранее указатель на интерфейс базы данных становится недействительным \n
					Полученные ранее указателели на интерфейсы таблиц данных становятся недействительными \n
					*/
					virtual HRESULT Close(VOID) = 0;

					/// Перемещает текущий_указатель на первую таблицу в базе
					virtual HRESULT MoveFirstDataTable(VOID) = 0;

					/// Перемещает текущий_указатель на следующую таблицу в базе
					virtual HRESULT MoveNextDataTable(VOID) = 0;

					/// Перемещает текущий_указатель на предыдущую таблицу в базе
					virtual HRESULT MovePrevDataTable(VOID) = 0;

					/// Перемещает текущий_указатель на указанную (индексом) таблицу в базе
					virtual HRESULT MoveToDataTable(UINT32  nDTableIndex) = 0;

					/// Перемещает текущий_указатель на указанную (именем) таблицу в базе
					virtual HRESULT MoveToDataTable(CONST _TCHAR * pDTableName)  = 0;

					/// Получает указатель на интерфейс таблицы в позиции текущего_указателя
					virtual IDataTable * OpenDataTable(VOID) = 0;

					// --------------------------------------------------------
					// Методы работы с динамически созданными таблицами данных
					// 

					/// Динамически создать таблицу данных по заданному пути и c заданными параменрами (и возможно открыть)
					virtual IDataTable * DynamicDataTableCreate(CONST _TCHAR * pPathToDTable, CONST _TCHAR * pDTableName, INT iInitialSize, ETableGrowTypes eTableGrowType, CONST TDataTableFieldDescription aoFieldDescription[], BOOL bOpenAfterCreation = FALSE) = 0;

					/// Открыть динамически созданную таблицу данных
					virtual HRESULT DynamicDataTableOpen(IDataTable * pIDataTable) = 0;

					/// Отказаться от дальнейщей работы с динамически созданной таблицей данных
					virtual HRESULT DynamicDataTableRefuse(IDataTable * pDTable) = 0;

					/// Получить список имен таблиц из указанной дирректории
					virtual HRESULT GetDataTablesNames(IN CONST _TCHAR * pDirectoryToExplore, OUT std::list< std::wstring > * plstDataTablesNamesList) = 0;

				};

				// --------------------------------------------------------------------------------
				// 
				// --------------------------------------------------------------------------------

				/*! \interface IDataTable
				*   \brief     Интерфейс таблицы данных (DataTable interface)
				*/
				class IDataTable   { // : public virtual SS::Interface::IBase   {

				public:

					/// Возвращает флаг "задействованности"
					virtual BOOL IsOpen(VOID) = 0;

					/// Возвращает имя объекта
					virtual CONST _TCHAR * GetName(VOID) = 0;

					/// Сохраняет промежуточную копию таблицы (в случае сбоя, в последствии, будет загружена копия)
					/** 
						\return virtual HRESULT
					 
					Сохраняет промежуточную копию таблицы (в случае сбоя, в последствии, будет загружена копия)\n
					*/
					virtual HRESULT SaveBackup(VOID) = 0;

					/// Сохраняет текущие данные таблицы (Если добавлений больше не будет, то в случае сбоя, данные останутся корректными)
					/** 
						\return virtual HRESULT
					 
					Сохраняет текущие данные таблицы (Если добавлений больше не будет, то в случае сбоя, данные останутся корректными)\n
					*/
					virtual HRESULT Save(VOID) = 0;

					/// Очищает/реинициализирует таблицу\n
					/** 
						\param bClearUserInitializations
						\return virtual HRESULT
					 
					Очищает/реинициализирует таблицу\n
					Полученный ранее указатель на интерфейс таблицы остается действительным.\n
					Открытые таблицы остаются открытыми.\n
					Если bClearUserInitializations == TRUE, то выполненые пользователем инициализации (AddInit(), FindInit(), и пр.) теряются.
					*/
					virtual HRESULT Clear(BOOL bClearUserInitializations = TRUE) = 0;

					/// Очищает таблицу при условии, что счетчик числа открытий равен нулю\n
					/** 
						\return virtual HRESULT : S_OK - тблица очищена, S_FALSE - тблица НЕ очищена, т.к. ее еще используют
					 
					Очищает таблицу при условии, что счетчик числа открытий таблицы равен нулю\n
					*/
					virtual HRESULT TryClear(VOID) = 0;

					/// Завершает работу с полученым ранее указателем на интерфейс таблицы.\n
					/** 
						\param bClearAfterClose
						\return virtual HRESULT
					 
					Завершает работу с полученым ранее указателем на интерфейс таблицы.\n
					Таблица может быть очищена.\n
					Полученный ранее указатель на интерфейс таблицы становится недействительным.
					*/
					virtual HRESULT Close(BOOL bClearAfterClose = FALSE) = 0;

					// -------------------------------------------------------
					
					/// Инициализация процедуры добавления записей в таблицу
					/** 
						\param ppArrayOfPointersToData[]
						\return virtual HRESULT
					 
					Инициализация процедуры добавления записей в таблицу\n
					Параметр есть массив "VOID"-указателей на переменные,	которые будут содержать данные полей таблицы\n
					Подготовка новой записи сводится к изменению значений в уже указанных переменных.
					*/
					virtual HRESULT AddInit(VOID * ppArrayOfPointersToData[]) = 0;

					/// Добавление записи в таблицу
					/** 
						\return virtual HRESULT
					 
					Добавление записи в таблицу\n
					В таблицу записываются данные находящиеся по указателям, переданным в AddInit()
					*/
					virtual HRESULT Add(VOID) = 0;

					/// Получение числа записей в таблице
					/** 
						\return virtual UINT32
					 
					Получение числа записей в таблице\n
					*/
					virtual UINT32 GetRecordsNumber(VOID) = 0;

					/// Удаление записи
					/** 
						\param nRecordIdx
						\return virtual HRESULT Возвращает S_OK или S_FALSE в случае когда указанной записи нет.

					Удаление записи\n
					Запись помечается на удаление. В последствии она не возвращается метдами поиска и сканирования\n
					Реальное удаление из файлов данных происходит автоматически при числе удаленных записей > 25% или\n
					при явном вызове метода Shrink()\n
					*/
					virtual HRESULT DeleteRecord(UINT32 nRecordIdx) = 0;

					/// Получение числа удаленных (помеченых на удаление) записей в таблице
					/** 
						\return virtual UINT32
					 
					Получение числа удаленных (помеченых на удаление) записей в таблице\n
					*/
					virtual UINT32 GetDeletedRecordsNumber(VOID) = 0;

					/// Сжатие данных таблицы. Исключение записей помеченых на удаление
					/** 
						\return virtual HRESULT Возвращает S_OK, S_FAIL или S_FALSE в случае, когда сжатие не нужно.

					Сжатие данных таблицы. Исключение записей помеченых на удаление\n
					Перед вызовом метода таблица должна быть закрыта\n
					*/
					virtual HRESULT Shrink(VOID) = 0;

					// -------------------------------------------------------
					
					/// Инициализация процедуры поиска
					/** 
						\param pKeyName задает имя ключевого поля.\n
						\param pKeyValue задает указатель на переменную в которой будут находиться значения ключа поиска.\n
						\param ppArrayOfPointersToData[] задает массив "VOID"-указателей на переменные в которые будут записаны данные полей таблицы.\n
						\param nFindId - идентификатор подпрограммы поиска. Одновременно возможно до 128 поисков. По умолчанию все методы работают с нулевой подпрограммой поиска.\n
						\return virtual HRESULT
					 
					Инициализация процедуры поиска.\n
					Если Вам не нужны определенные поля, то их указатели в ppArrayOfPointersToData[] могут быть установлены в NULL.\n
					Выполнение нового поиска сводится к изменению значения в уже указанной переменной и вызову в цикле метода Find().\n
					*/
					virtual HRESULT FindInit(_TCHAR * pKeyName, VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD nFindId = 0) = 0;

					/// Те же действия, что и в FindInit(), но задается не имя ключевого поля, а индекс 
					virtual HRESULT FindInit(UINT32 nKeyIndex, VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD nFindId = 0) = 0;

					/// Те же действия, что и в FindInit(), за исключением того, что функция сама определяет значение nFindId
					virtual HRESULT FindInitEx(_TCHAR * pKeyName, VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD * pnFindId) = 0;

					/// Те же действия, что и в FindInit(), за исключением того, что функция сама определяет значение nFindId
					virtual HRESULT FindInitEx(UINT32   nKeyIndex, VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD * pnFindId) = 0;

					/// Отказ от поиска по заданному nFindId
					/** 
						\param nFindId идентификатор подпрограммы поиска.
						\return virtual HRESULT
					*/
					virtual HRESULT FindCancel(WORD nFindId = 0) = 0;

					/// Поиск и получение найденых записей таблицы (по значению ключа, указанного в FindInit())
					/** 
						\param nFindId - идентификатор подпрограммы поиска. Одновременно возможно до 128 поисков. По умолчанию все методы работают с нулевой подпрограммой поиска.
						\return virtual HRESULT
					 
					Поиск и получение найденых записей таблицы (по значению ключа, указанного в FindInit())\n
					Данные записываются по указателям заданным в FindInit(). \n
					Возвращает S_OK в случае удачного поиска и получения или S_FALSE в случае когда записей нет или больше нет.\n
					После возвращения S_FALSE подразумевается, что следующий вызов Find() будет работать с новым значением ключа, указанного в FindInit()
					*/
					virtual HRESULT Find(WORD nFindId = 0) = 0;
				
					/// Поиск и получение единственной записи таблицы (по значению ключа, указанного в FindInit())
					/** 
						\param nFindId - идентификатор подпрограммы поиска. Одновременно возможно до 128 поисков. По умолчанию все методы работают с нулевой подпрограммой поиска.
						\return virtual HRESULT
					 
					Поиск и получение единственной записи таблицы (по значению ключа, указанного в FindInit())\n
					Данные записываются по указателям заданным в FindInit(). \n
					Возвращает S_OK в случае удачного поиска и получения или	S_FALSE в случае когда такой записи нет.\n
					Следующий вызовы Find???() будут работать с новым значением ключа.
					*/
					virtual HRESULT FindOne(WORD nFindId = 0) = 0;

					/// Получение индекса ранее найденой записи таблицы
					/** 
						\param nFindId - идентификатор подпрограммы поиска.
						\return virtual UINT32
					 
					Получение индекса ранее найденой записи таблицы.\n
					Возвращает индекс или 0xFFFFFFFF в случае ошибки.\n
					*/
					virtual UINT32 GetFoundIdx(WORD nFindId = 0) = 0;

					/// Дополнительная функция.
					/** 
						\param nFindId - идентификатор подпрограммы поиска. Одновременно возможно до 128 поисков. По умолчанию все методы работают с нулевой подпрограммой поиска.\n
						\return virtual HRESULT Возвращает S_OK в случае если заданное значение ключа в таблице есть или S_FALSE - в случае неудачного поиска (последующий вызов Find() так же вернет S_FALSE).\n
					 
					Гарантирует, что последующий вызов Find() будет работать с новым
					значением ключа (указанного в FindInit()), независимо от того,
					завершился предыдущий поиск (Find()==S_FALSE) или нет (Find()==S_OK).\n
					!!! Перед вызовом значение нового ключа должно уже быть определено.\n
					Данные записи таблицы никуда не записываются
					*/

					virtual HRESULT FindStart(WORD nFindId = 0) = 0;
					
					/// Дополнительная функция.
					/** 
						\param nFindId - идентификатор подпрограммы поиска. Одновременно возможно до 128 поисков. По умолчанию все методы работают с нулевой подпрограммой поиска.\n
						\return virtual HRESULT Возвращает S_OK или E_FAIL в случае неверного nFindId.\n
					 
					Гарантирует, что последующий вызов Find() будет работать с новым значением ключа (указанного в FindInit()),
					независимо от того,	завершился предыдущий поиск (Find()==S_FALSE) или нет (Find()==S_OK).\n
					!!! Определение перед вызовом нового значения ключа  - не требуется.\n
					Данные записи таблицы никуда не записываются\n
					*/
					virtual HRESULT FindStop(WORD nFindId = 0) = 0;

					// -------------------------------------------------------

					/// Инициализация процедуры поиска в заданном интервале (включая границы)
					/** 
						\param nKeyIndex            - задает имя ключевого поля.\n
						\param pLowerKeyValue       - задает указатель на переменную в которой будут находиться значения нижней границы интервала.\n
						\param pUpperKeyValue       - задает указатель на переменную в которой будут находиться значения верхней границы интервала.\n
						\param pSetOfRecordsIndexes - задает STL::SET в который будут записаны индексы строк удовлетворяющих условиям поиска.\n
						\param nSpanFindId          - идентификатор подпрограммы поиска. Одновременно возможно до 16 поисков. По умолчанию все методы работают с нулевой подпрограммой поиска.\n
						\param pSetToIntersectWith  - задает STL::SET пересечение с которым - есть необходимое условие для занесения в pSetOfRecordsIndexes.\n
						\return virtual HRESULT.\n
					 
					Инициализация процедуры поиска в заданном интервале (включая границы)\n
					Выполнение нового поиска сводится к изменению значений в уже указанных переменных и единственному вызову метода FindSpan().\n
					*/
					virtual HRESULT SpanFindInit(
						_TCHAR             * pKeyName,
						VOID               * pLowerKeyValue,
						VOID               * pUpperKeyValue,
						std::set< UINT32 > * pSetOfRecordsIndexes,
						WORD                 nSpanFindId = 0,
						std::set< UINT32 > * pSetToIntersectWith = NULL
						) = 0;

					/// Те же действия, что и в SpanFindInit(), но задается не имя ключевого поля, а индекс 
					virtual HRESULT SpanFindInit(
						UINT32               nKeyIndex,
						VOID               * pLowerKeyValue,
						VOID               * pUpperKeyValue,
						std::set< UINT32 > * pSetOfRecordsIndexes,
						WORD                 nSpanFindId = 0,
						std::set< UINT32 > * pSetToIntersectWith = NULL
						) = 0;

					/// Те же действия, что и в SpanFindInit(), за исключением того, что функция сама определяет значение nSpanFindId
					virtual HRESULT SpanFindInitEx(
						_TCHAR             * pKeyName,
						VOID               * pLowerKeyValue,
						VOID               * pUpperKeyValue,
						std::set< UINT32 > * pSetOfRecordsIndexes,
						WORD               * pnSpanFindId,
						std::set< UINT32 > * pSetToIntersectWith = NULL
						) = 0;

					/// Те же действия, что и в SpanFindInit(), за исключением того, что функция сама определяет значение nSpanFindId
					virtual HRESULT SpanFindInitEx(
						UINT32               nKeyIndex,
						VOID               * pLowerKeyValue,
						VOID               * pUpperKeyValue,
						std::set< UINT32 > * pSetOfRecordsIndexes,
						WORD               * pnSpanFindId,
						std::set< UINT32 > * pSetToIntersectWith = NULL
						) = 0;


					/// Получение индексов записей таблицы (по значениям ключа, указанного в SpanFindInit())
					/** 
						\param nSpanFindId - идентификатор подпрограммы поиска. Одновременно возможно до 16 поисков. По умолчанию все методы работают с нулевой подпрограммой поиска.
						\return virtual HRESULT
					 
					Получение индексов записей таблицы (по значениям ключа, указанного в SpanFindInit())\n
					Данные записываются в STL::SET заданный в SpanFindInit(). \n
					Возвращает S_OK в случае удачного поиска и получения или S_FALSE в случае когда записей нет.\n
					*/
					virtual HRESULT SpanFind(WORD nSpanFindId = 0) = 0;

					// -------------------------------------------------------

					/// Инициализация процедуры просмотра
					/** 
						\param nStartRecordIdx - индекс звписи таблицы с которой начнется сканирование.\n
						\param ppArrayOfPointersToData[] - задает массив "VOID"-указателей на переменные в которые будут записаны данные полей таблицы.\n
						\param nScanId - идентификатор подпрограммы просмотра.
						\return virtual HRESULT Возвращает S_OK или S_FALSE в случае указанной записи нет.\n
					 
					Инициализация процедуры просмотра\n
					Первый параметр nStartRecordIdx задает индекс записи с которой будет начат просмотр\n
					Второй параметр ppArrayOfPointersToData задает массив "VOID"-указателей на переменные в
					которые будут записаны данные полей таблицы.\n
					Если Вам не нужны определенные поля, то их указатели могут быть установлены в NULL\n
					Выполнение просмотра сводится к вызову в цикле метода Scan().\n
					*/
					virtual HRESULT ScanInit(UINT32 nStartRecordIdx, VOID * ppArrayOfPointersToData[], WORD nScanId = 0) = 0;

					/// Те же действия, что и в ScanInit(), за исключением того, что функция сама определяет значение nScanId
					virtual HRESULT ScanInitEx(UINT32 nStartRecordIdx, VOID * ppArrayOfPointersToData[], WORD * pnScanId) = 0;

					/// Отказ от просмотра по заданному nScanId
					/** 
						\param nScanId идентификатор подпрограммы просмотра.
						\return virtual HRESULT
					*/
					virtual HRESULT ScanCancel(WORD nScanId = 0) = 0;

					/// Инициализация стартового индекса процедуры просмотра
					/** 
						\param nStartRecordIdx - индекс звписи таблицы с которой начнется сканирование.\n
						\param nScanId - идентификатор подпрограммы просмотра.
						\return virtual HRESULT Возвращает S_OK или S_FALSE в случае указанной записи нет.\n
					*/
					virtual HRESULT ScanStart(UINT32 nStartRecordIdx, WORD nScanId = 0) = 0;

					/// Просмотр и получение записей таблицы (со значения индекса, заданного в ScanInit())
					/** 
						\param nScanId - идентификатор подпрограммы просмотра.
						\return virtual HRESULT Возвращает S_OK или S_FALSE в случае когда записей больше нет.
					 
					Просмотр и получение записей таблицы (со значения индекса, заданного в ScanInit())\n
					Данные записываются по указателям заданным в ScanInit(). 
					*/
					virtual HRESULT Scan(WORD nScanId = 0) = 0;
					
					/// Получение единственной записеи таблицы
					/** 
						\param nRecordIdx - номер сканируемой записи
						\param nScanId    - идентификатор подпрограммы просмотра.
						\return virtual HRESULT Возвращает S_OK или S_FALSE в случае когда заданной записи нет.
					 
					Получение единственной записеи таблицы\n
					Данные записываются по указателям заданным в ScanInit(). \n
					*/
					virtual HRESULT ScanOne(UINT32 nRecordIdx, WORD nScanId = 0) = 0;

					/// Получение индекса ранее сосканированной записи таблицы
					/** 
						\param nScanId - идентификатор подпрограммы просмотра.
						\return virtual UINT32
					 
					Получение индекса ранее сосканированной записи таблицы.\n
					Возвращает индекс или 0xFFFFFFFF в случае ошибки.\n
					*/
					virtual UINT32 GetScanedIdx(WORD nScanId = 0) = 0;

					// -------------------------------------------------------

					/// Инициализация процедуры изменения
					/** 
						\param ppArrayOfPointersToData[]
						\return virtual HRESULT
					 
					Инициализация процедуры изменения\n
					Параметр есть массив "VOID"-указателей на переменные,	которые будут содержать новые данные полей таблицы
					Если Вам не нужно изменять определенные поля, то их указатели могут быть установлены в NULL. \n
					Выполнение изменения записи таблицы сводится к изменению
					значения в уже указанной переменной и вызову методов FindOneAndChange() или ScanOneAndChange().\n
					Изменяются только те поля таблицы, которые имеют модификатор IS_DATA.
					*/
					virtual HRESULT ChangeInit(VOID * ppArrayOfPointersToData[]) = 0;

					/// Поиск (по значению ключа, указанного в FindInit()) и изменение единственной записи таблицы
					/** 
						\return virtual HRESULT Возвращает S_OK в случае удачного поиска и изменения или S_FALSE в случае когда такой записи нет или E_FAIL при неверных параметрах.
					 
					Поиск (по значению ключа, указанного в FindInit()) и изменение единственной записи таблицы\n
					Данные для изменения берутся по указателям заданным в ChangeInit(). \n
					Изменяются только те поля таблицы, которые имеют модификатор IS_DATA.\n
					Следующий вызовы Find???() будут работать с новым значением ключа.
					*/
					virtual HRESULT FindOneAndChange(WORD nFindId = 0) = 0;

					/// Поиск (по значению ключа, указанного в FindInit()) и инкримент счетчика
					/** 
						\return virtual HRESULT Возвращает S_OK в случае удачного поиска и изменения или S_FALSE в случае когда такой записи нет или E_FAIL при неверных параметрах.
					 
					Поиск (по значению ключа, указанного в FindInit()) и инкримент счетчика\n
					Изменяются счетчик миеющий модификатор IS_DATA и идущий за ключем поиска.\n
					Следующий вызовы Find???() будут работать с новым значением ключа.
					*/
					// virtual HRESULT FindOneAndInc(WORD nFindId = 0) = 0;

					/// Изменение единственной записи таблицы
					/** 
						\param nRecordIdx
						\return virtual HRESULT Возвращает S_OK в случае удачного поиска и изменения или	S_FALSE в случае когда такой записи нет или E_FAIL при неверных параметрах.
					 
					Изменение единственной записи таблицы\n
					Данные для изменения берутся по указателям заданным в ChangeInit(). \n
					Изменяются только те поля таблицы, которые имеют модификатор IS_DATA.\n
					*/
					virtual HRESULT ScanOneAndChange(UINT32 nRecordIdx, WORD nScanId = 0) = 0;

					/// Сохранить в данных таблицы пользовательское значение (одно из 16-и возможных)
					/** 
						\param iValueIdx - индекс значения [0..15]
						\param uiValue   - значение
						\return virtual HRESULT
					*/
					virtual HRESULT SetUserValue(UINT iValueIdx, UINT32 uiValue) = 0;

					/// Получить из данных таблицы пользовательское значение (одно из 16-и возможных)
					/** 
						\param iValueIdx - индекс значения [0..15]
						\return virtual UINT32 
					*/
					virtual UINT32  GetUserValue(UINT iValueIdx ) = 0;
				};
			}
		}
	}
}
