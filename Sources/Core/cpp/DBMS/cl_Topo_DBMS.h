#pragma once

#include "..\ASCInterface\IDBMS.h"
#include "cl_DTablePhys.h"

using namespace SS::Interface::Core::DBMS;

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_Topo_DBMS.h"


///	Класс Менеджер DBMS (Верхний в иерархии)
class DBManagerTopoC;

///	Класс База данных
class DBaseTopoC;

///	Класс Таблица данных
class DTableTopoC;

///	Класс Поле данных (Нижний в иерархии)
class DFieldTopoC;

/*! \class DObjC
	*  \brief Базовый класс для классов топологии: DBManagerTopoC, DBaseTopoC, DTableTopoC, DFieldTopoC
*/
class DObjC   {

	public :

		/// Constructor
		DObjC() : m_HierLevel(0), m_ParentPtr(NULL), m_LockedFlag(FALSE), m_IndexInParentMap(0)   { 
			// OutputDebugString(_T("DObjC +++\n"));
		}
//		~DObjC()   { 
//		  // OutputDebugString(_T("DObjC ---\n"));
//		}

	public :

		/// Уровень в иерархии
		WORD         m_HierLevel;

		/// Имя бъекта
		STLSTR       m_Name;

		/// Указатель на родителя
		DObjC      * m_ParentPtr;

		/// Индекс объекта (в карте детей)
		WORD         m_IndexInParentMap;

		/// Флаг "ОБЪЕКТ ЗАЛОЧЕН"
		BYTE         m_LockedFlag;

		/// Критическая секция
		CritSectC    m_oTopoCriticalSection;
};

/*! \class DObjParentTC
	*  \brief Базовый класс для классов топологии имеющих детей: DBManagerTopoC, DBaseTopoC, DTableTopoC
*/
template <typename ChildTypeT> class DObjParentTC : public DObjC   {

	public:

		/// Карта ссылкок на детей по имени
		typedef std::map < STLSTR, ChildTypeT * >  ChildNameMapT;
		/// Карта ссылкок на детей по имени
		typedef std::pair< STLSTR, ChildTypeT * >  ChildNamePairT;
		/// Карта ссылкок на детей по индексу
		typedef std::map < WORD,        ChildTypeT * >  ChildIndexMapT;
		/// Карта ссылкок на детей по индексу
		typedef std::pair< WORD,        ChildTypeT * >  ChildIndexPairT;

		// Итератор для карты детей
		#define ChildIteratorT ChildIndexMapT::iterator

		/// Consrtructor
		DObjParentTC(CONST _TCHAR * sName, WORD nIndex):
			m_ReferenceCounter(0),
			m_ChildrenNum(0),
			m_ChildCurrent(0),
			m_ChildMoveResult(S_FALSE)   {

			m_ParentPtr        = NULL;
			m_Name             = sName;
			m_IndexInParentMap = nIndex;

			// OutputDebugString(_T("DObjParentTC +++\n"));
		}

		/// Destructor
		~DObjParentTC()   {

			// OutputDebugString(_T("DObjParentTC ---\n"));

			if(m_ReferenceCounter)   {
				if(m_HierLevel!=1 || m_IndexInParentMap!=0)   { // DynamicDataBase
					LogMess(_T("Object <%s> was not closed before destructor call (Reference_Counter != 0)"),m_Name.c_str());
					m_ReferenceCounter = 0;
				}
			}

			for(ChildIndexMapT::const_iterator  ci =m_ChildIndexMap.begin();
															ci!=m_ChildIndexMap.end();
															ci++)   if(ci->second)   delete (ChildTypeT *)ci->second;

			m_ChildIndexMap.clear();
			m_ChildNameMap.clear();
		}

	public:

		/// Создать и добавить дочерний объект
		ChildTypeT * AddChild(CONST _TCHAR * sChildName)   {
			ChildTypeT * pC;
			std::pair<ChildNameMapT::iterator,  bool> retN;
			std::pair<ChildIndexMapT::iterator, bool> retI;

			#pragma warning( disable : 4291)
			if((pC = new ChildTypeT(sChildName,m_ChildrenNum))==NULL)   {
				LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("New_Child_Class_Instance was not build(%s::%s)"),m_Name.c_str(),sChildName);
				return(NULL);
			}
			#pragma warning( default : 4291)

			pC->m_ParentPtr=static_cast<DObjC *>(this);
			pC->m_HierLevel=m_HierLevel+1;

			retN = m_ChildNameMap.insert(ChildNamePairT(sChildName,pC));
			retI = m_ChildIndexMap.insert(ChildIndexPairT(m_ChildrenNum,pC));

			if(retN.second && retI.second)   {
				m_ChildrenNum++;
			} else   {
				LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("New_Child_Class_Instance was not inserted in STD::MAP (%s\\%s)Error"),m_Name.c_str(),sChildName);
				delete pC;
				pC=NULL;
			}

			return(pC);
		}

		/// Удалить дочерний объект
		HRESULT DeleteChild(ChildTypeT * pChild)   {
			size_t iCnt=0;

			iCnt+=m_ChildNameMap.erase(pChild->m_Name);
			iCnt+=m_ChildIndexMap.erase(pChild->m_IndexInParentMap);

			delete pChild;

			if(iCnt==2)   {
				m_ChildrenNum--;
			} else   {
				LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("Child_Class_Instance was not deleted in STD::MAP (%s\\%s)Error"),m_Name.c_str(),pChild->m_Name.c_str());
			}

			return(S_OK);
		}

		/// Найти ребенка по имени
		ChildTypeT * FindChild(CONST _TCHAR * sChildName)   {
			ChildNameMapT::const_iterator ci =  m_ChildNameMap.find(sChildName);
			return((ci!=m_ChildNameMap.end())?(ci->second):(NULL));
		}

		/// Найти ребенка по индексу
		ChildTypeT * FindChild(WORD nChildIndex)   {
			ChildIndexMapT::const_iterator ci =  m_ChildIndexMap.find(nChildIndex);
			return((ci!=m_ChildIndexMap.end())?(ci->second):(NULL));
		}

		/// Передвинуть внутренний указатель на первого ребенка
		HRESULT MoveFirstChild(VOID)   {
			m_ChildCurrent=0;
			return(m_ChildMoveResult=S_OK);
		}

		/// Передвинуть внутренний указатель на следующего ребенка
		HRESULT MoveNextChild(VOID)   {
			if(++m_ChildCurrent==m_ChildrenNum)   {
				m_ChildCurrent--;
				return(m_ChildMoveResult=S_FALSE);
			} else   {
				return(m_ChildMoveResult=S_OK);
			}
		}

		/// Передвинуть внутренний указатель на предыдущего ребенка
		HRESULT MovePrevChild(VOID)   {
			if(m_ChildCurrent==0)   {
				return(m_ChildMoveResult=S_FALSE);
			} else   {
				m_ChildCurrent--;
				return(m_ChildMoveResult=S_OK);
			}
		}

		/// Передвинуть внутренний указатель на указанного индексом ребенка
		HRESULT MoveToChild(UINT32  nChildIndex)   {
			if(FindChild(nChildIndex))   {
				m_ChildCurrent=nChildIndex;
				return(m_ChildMoveResult=S_OK);
			} else   {
				return(m_ChildMoveResult=S_FALSE);
			}
		}

		/// Передвинуть внутренний указатель на указанного именем ребенка
		HRESULT MoveToChild(CONST _TCHAR * pChildName)   {

			ChildTypeT * pChild=FindChild(pChildName);

			if(pChild)   {
				m_ChildCurrent=pChild->m_IndexInParentMap;
				return(m_ChildMoveResult=S_OK);
			} else   {
				return(m_ChildMoveResult=S_FALSE);
			}
		}

		/// Получить указатель (pointer) на ребенка в позиции текущего индекса
		HRESULT GetChild(ChildTypeT ** ppChild)   {
			ChildTypeT * pChild=FindChild(m_ChildCurrent);

			if(pChild)   {
				if(m_ChildMoveResult==S_OK)   {
					*ppChild=pChild;
					return(S_OK);
				} else   {
					*ppChild=NULL;
					return(LogFail(_F,ERROR_INVALID_ACCESS,_T("Previous MOVE_TO_CHILD command does not succeeded")));
				}
			} else   {
				*ppChild=NULL;
				return(LogFail(_F,ERROR_INVALID_ACCESS,_T("Expected CHILD_OBJECT was not accessed")));
			}
		}

		/// Освободить ранее полученный указатель
		HRESULT UnGetChild(ChildTypeT * pChild)   {
			return(S_OK);
		}


	public:

		/// Число детей
		WORD            m_ChildrenNum;

		/// Индекс текущего ребенка
		WORD            m_ChildCurrent;

		/// Карта имен детей
		ChildNameMapT   m_ChildNameMap;

		/// Карта индексов детей
		ChildIndexMapT  m_ChildIndexMap;

		/// Результат операций передвижения по детям
		HRESULT         m_ChildMoveResult;

		/// Счетчик ссылок (Счетчик числа открытий)
		UINT32          m_ReferenceCounter;
};



/*! \class DBManagerTopoC
	*  \brief Класс топологии: Менеджер баз данных
*/
class DBManagerTopoC : public SS::Interface::Core::DBMS::IDBManager, public DObjParentTC<DBaseTopoC>   {

	public: //
/*
		DBManagerTopoC(const STLSTR * sName, WORD nIndex) :
			DObjParentTC<DBaseTopoC>(sName,nIndex) { g_pDBManager=this; };
*/
		/// Конструктор
		DBManagerTopoC() :
			DObjParentTC<DBaseTopoC>(_T(""),0) { m_Initialized=FALSE; };

	public: // Interface

		/// Освободить менеджер
		virtual ULONG   Release();

		/// Получить интерфейс
		virtual HRESULT QueryInterface(REFIID pIID, VOID ** pBase);


	public: // Interface

		/// Флаг "ОБЪЕКТ ОТКРЫТ"
		BOOL    IsOpen(VOID) { return(m_ReferenceCounter); }

		/// Возвращает имя объекта
		CONST _TCHAR * GetName(VOID) { return(m_Name.c_str()); }

		/// Читает указанный INI-файл и по нему создает образ DBMS на диске
		HRESULT Init(CONST _TCHAR * pFullPathToIniFile, CONST _TCHAR * pFullPathToDbmsBases);

		/// Создает внутреннюю (НЕ дисковую) структуру для работы только с динамическими таблицами с единственной базой данных "DynamicDataBase" (индекс - 0)
		HRESULT InitDynamic(OUT IDataBase ** ppIDataBase = NULL);

		/// Читает указанный INI-файл и по нему создает образ DBMS на диске
		HRESULT Switch(CONST _TCHAR * pFullPathToIniFile, CONST _TCHAR * pFullPathToDbmsBases);

		/// Вспомогательный метод. Читает указанный INI-файл и по нему создает образ DBMS в памяти
		HRESULT InitReadDbmsName(CONST _TCHAR * pFullPathToIniFile, CONST _TCHAR * pFullPathToDbmsBases);

		/// Передвинуть внутренний указатель на первую базу данных - <DynamicDataBase> для динамических таблиц
		HRESULT MoveFirstDataBase(VOID) { return(MoveFirstChild()); }

		/// Передвинуть внутренний указатель на следующую базу данных
		HRESULT MoveNextDataBase(VOID) { return(MoveNextChild()); }

		/// Передвинуть внутренний указатель на предыдущую базу данных
		HRESULT MovePrevDataBase(VOID) { return(MovePrevChild()); }

		/// Передвинуть внутренний указатель на указанную индексом базу данных (Idx: 0 - база <DynamicDataBase> для динамических таблиц)
		HRESULT MoveToDataBase(UINT32  nDBaseIndex) { return(MoveToChild(nDBaseIndex)); }

		/// Передвинуть внутренний указатель на указанную именем базу данных
		HRESULT MoveToDataBase(CONST _TCHAR * pDBaseName) { return(MoveToChild(pDBaseName)); }

		/// Получить указатель (pointer) на базу данных в позиции текущего индекса
		IDataBase * OpenDataBase(VOID);

	private:

		/// Флаг: DBMS была проинициализированна
		BOOL    m_Initialized; //

		/// Вспомогательный метод. Читает указанный INI-файл и по нему создает образ DBMS в памяти
		HRESULT InitReadTopoFile(CONST _TCHAR * pFullPathToIniFile, CONST _TCHAR * pFullPathToDbmsBases);

		/// Создание образа DBMS на диске
		HRESULT InitBuildPhysicalImage(VOID);
};


/*! \class DBaseTopoC
	*  \brief Класс топологии: База данных
*/
class DBaseTopoC : public SS::Interface::Core::DBMS::IDataBase, public DObjParentTC<DTableTopoC>  {

	public:

		/// Конструктор
		DBaseTopoC(CONST _TCHAR * sName, WORD nIndex) :
			DObjParentTC<DTableTopoC>(sName,nIndex) {};

	//public:

	//	// Освободить базу данных
	//	virtual ULONG   Release();

	//	// Получить интерфейс
	//	virtual HRESULT QueryInterface(REFIID pIID, VOID ** pBase);

	public:

		/// Флаг "ОБЪЕКТ ОТКРЫТ"
		BOOL    IsOpen(VOID) { return(m_ReferenceCounter); }

		/// Возвращает имя объекта
		CONST _TCHAR * GetName(VOID) { return(m_Name.c_str()); }

		/// Очистить базу данных
		HRESULT Clear(BOOL bClearUserInitializations = TRUE);

		/// Очищает базу при условии, что нет открытых таблиц\n
		HRESULT TryClear(VOID);

		/// Закрыть базу данных
		HRESULT Close(VOID);

		/// Передвинуть внутренний указатель на первую таблицу данных
		HRESULT MoveFirstDataTable(VOID) { return(MoveFirstChild()); }

		/// Передвинуть внутренний указатель на следующую таблицу данных
		HRESULT MoveNextDataTable(VOID) { return(MoveNextChild()); }

		/// Передвинуть внутренний указатель на предыдущую таблицу данных
		HRESULT MovePrevDataTable(VOID) { return(MovePrevChild()); }

		/// Передвинуть внутренний указатель на указанную индексом таблицу данных
		HRESULT MoveToDataTable(UINT32  nDTableIndex) { return(MoveToChild(nDTableIndex)); }

		/// Передвинуть внутренний указатель на указанную именем таблицу данных
		HRESULT MoveToDataTable(CONST _TCHAR * pDTableName) { return(MoveToChild(pDTableName)); }

		/// Получить указатель (pointer) на таблицу данных в позиции текущего индекса
		IDataTable * OpenDataTable(VOID);

		/// Динамически создать таблицу данных по заданному пути и c заданными параменрами (и возможно открыть)
		IDataTable * DynamicDataTableCreate(CONST _TCHAR * pPathToDTable, CONST _TCHAR * pDTableName, INT iInitialSize, ETableGrowTypes eTableGrowType, CONST TDataTableFieldDescription aoFieldDescription[], BOOL bOpenAfterCreation = FALSE);

		/// Открыть динамически созданную таблицу данных
		HRESULT DynamicDataTableOpen(IDataTable * pIDataTable);

		/// Отказаться от дальнейщей работы с динамически созданной таблицей данных
		HRESULT DynamicDataTableRefuse(IDataTable * pDTable);

		/// Получить список имен таблиц из указанной дирректории
		HRESULT GetDataTablesNames(IN CONST _TCHAR * pDirectoryToExplore, OUT std::list< std::wstring > * plstDataTablesNamesList);
		
};

/*! \class DTableTopoC
	*  \brief Класс топологии: Таблица данных
*/
class DTableTopoC : public SS::Interface::Core::DBMS::IDataTable, public DObjParentTC<DFieldTopoC>, public DTablePhysC   {

	public: //

		DTableTopoC(CONST _TCHAR * sName, WORD nIndex) : DObjParentTC<DFieldTopoC>(sName,nIndex)  { DTablePhysC::m_TopoPtr = this; }

	public: // Interface

		/// Состояние - "ОТКРЫТО"
		BOOL    IsOpen(VOID) { return(m_ReferenceCounter); }

		/// Возвращает имя объекта
		CONST _TCHAR * GetName(VOID) { return(m_Name.c_str()); }

		/// Очистить таблицу данных
		HRESULT Clear(BOOL bClearUserInitializations = TRUE)  { return(DTablePhysC::Clear(bClearUserInitializations)); }

		/// Очищает таблицу при условии, что счетчик числа открытий равен нулю\n
		HRESULT TryClear(VOID) { return(DTablePhysC::TryClear()); }

		/// Сохраняет промежуточную копию таблицы (в случае сбоя, в последствии, будет загружена копия)
		HRESULT SaveBackup(VOID) { return(DTablePhysC::SaveBackup(TRUE)); }

		/// Сохраняет текущие данные таблицы (Если добавлений больше не будет, то в случае сбоя, данные останутся корректными)
		HRESULT Save(VOID) { return(DTablePhysC::SaveBackup()); }

		/// Закрыть таблицу данных
		HRESULT Close(BOOL bClearAfterClose = FALSE)  { return(DTablePhysC::Close(bClearAfterClose)); }

		/// Инициализация добавления данных в таблицу
		HRESULT AddInit(VOID * ppArrayOfPointersToData[]) { return(DTablePhysC::AddInit(ppArrayOfPointersToData)); }

		/// Инициализация добавления данных в таблицу
		HRESULT ChangeInit(VOID * ppArrayOfPointersToData[]) { return(DTablePhysC::ChangeInit(ppArrayOfPointersToData)); }

		/// Добавление данных в таблицу
		HRESULT Add(VOID) { return(DTablePhysC::Add()); }

		/// Получение числа записей в таблице
		UINT32 GetRecordsNumber(VOID) { return(DTablePhysC::GetRecordsNumber()); }

		/// Удаление записи
		HRESULT DeleteRecord(UINT32 nRecordIdx) { return(DTablePhysC::DeleteRecord(nRecordIdx)); }

		/// Получение числа удаленных (помеченых на удаление) записей в таблице\n
		UINT32 GetDeletedRecordsNumber(VOID) { return(DTablePhysC::GetDeletedRecordsNumber()); }

		/// Сжатие данных таблицы. Исключение записей помеченых на удаление
		HRESULT Shrink(VOID) { return(DTablePhysC::Shrink()); }

		/// Инициализация процедуры поиска
		HRESULT FindInit(_TCHAR * pKeyName,  VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD nFindId = 0) { return(DTablePhysC::FindInit(pKeyName, pKeyValue,ppArrayOfPointersToData,nFindId)); }

		/// Инициализация процедуры поиска
		HRESULT FindInit(UINT32 nKeyIndex, VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD nFindId = 0) { return(DTablePhysC::FindInit(nKeyIndex,pKeyValue,ppArrayOfPointersToData,nFindId)); }

		/// Инициализация процедуры поиска
		HRESULT FindInitEx(_TCHAR * pKeyName,  VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD * pnFindId) { return(DTablePhysC::FindInitEx(pKeyName, pKeyValue,ppArrayOfPointersToData,pnFindId)); }

		/// Инициализация процедуры поиска
		HRESULT FindInitEx(UINT32   nKeyIndex, VOID * pKeyValue, VOID * ppArrayOfPointersToData[], WORD * pnFindId) { return(DTablePhysC::FindInitEx(nKeyIndex,pKeyValue,ppArrayOfPointersToData,pnFindId)); }

		/// Прерывание поиска
		HRESULT FindCancel(WORD nFindId = 0) { return(DTablePhysC::FindCancel(nFindId)); }

		/// Поиск в таблице
		HRESULT Find(WORD nFindId = 0) { return(DTablePhysC::Find(nFindId)); }

		/// Поиск единственной записи
		HRESULT FindOne(WORD nFindId = 0) { return(DTablePhysC::FindOne(nFindId)); }

		/// Получение индекса ранее найденой записи таблицы
		UINT32 GetFoundIdx(WORD nFindId = 0) { return(DTablePhysC::GetFoundIdx(nFindId)); };

		/// Запуск поиска с начала
		HRESULT FindStart(WORD nFindId = 0) { return(DTablePhysC::FindStart(nFindId)); }

		/// Остановка поиска
		HRESULT FindStop(WORD nFindId = 0) { return(DTablePhysC::FindStop(nFindId)); }

		/// Поиск единственной записи и изменение ее
		HRESULT FindOneAndChange(WORD nFindId = 0) { return(DTablePhysC::FindOneAndChange(nFindId)); }

//		HRESULT FindOneAndInc(WORD nFindId = 0)  { return(DTablePhysC::FindOneAndInc(nFindId)); }

		/// Инициализация процедуры сканирования
		HRESULT ScanInit(UINT32 nStartRecordIdx, VOID * ppArrayOfPointersToData[], WORD nScanId = 0) { return(DTablePhysC::ScanInit(nStartRecordIdx,ppArrayOfPointersToData,nScanId)); }

		/// Инициализация процедуры сканирования
		HRESULT ScanInitEx(UINT32 nStartRecordIdx, VOID * ppArrayOfPointersToData[], WORD * pnScanId) { return(DTablePhysC::ScanInitEx(nStartRecordIdx,ppArrayOfPointersToData,pnScanId)); }

		/// Прерывание процедуры сканирования
		HRESULT ScanCancel(WORD nScanId = 0) { return(DTablePhysC::ScanCancel(nScanId)); }

		/// Установка стартового номера строки для сканирования
		HRESULT ScanStart(UINT32 nStartRecordIdx, WORD nScanId = 0) { return(DTablePhysC::ScanStart(nStartRecordIdx,nScanId)); }

		/// Последовательное сканирование данных
		HRESULT Scan(WORD nScanId = 0) { return(DTablePhysC::Scan(nScanId)); }

		/// Считать одно заданное значение
		HRESULT ScanOne(UINT32 nRecordIdx, WORD nScanId = 0) { return(DTablePhysC::ScanOne(nRecordIdx,nScanId)); }

		/// Получение индекса ранее сосканированной записи таблицы
		UINT32 GetScanedIdx(WORD nScanId = 0) { return(DTablePhysC::GetScanedIdx()); };

		/// Считать одно заданное значение и изменить его
		HRESULT ScanOneAndChange(UINT32 nRecordIdx, WORD nScanId = 0) { return(DTablePhysC::ScanOneAndChange(nRecordIdx,nScanId)); }

		/// Записать пользовательское значение
		HRESULT SetUserValue(UINT iValueIdx, UINT32 uiValue) { return(DTablePhysC::SetUserValue(iValueIdx,uiValue)); }

		/// Получить пользовательское значение
		UINT32  GetUserValue(UINT iValueIdx ) { return(DTablePhysC::GetUserValue(iValueIdx)); }

		/// Инициализация процедуры поиска в заданном интервале (включая границы)
		HRESULT SpanFindInit(
			_TCHAR             * pKeyName,
			VOID               * pLowerKeyValue,
			VOID               * pUpperKeyValue,
			std::set< UINT32 > * pSetOfRecordsIndexes,
			WORD                 nFindId = 0,
			std::set< UINT32 > * pSetToIntersectWith = NULL
			) { return(DTablePhysC::SpanFindInit(pKeyName,pLowerKeyValue,pUpperKeyValue,pSetOfRecordsIndexes,nFindId,pSetToIntersectWith)); }

		/// Те же действия, что и в SpanFindInit(), но задается не имя ключевого поля, а индекс 
		HRESULT SpanFindInit(
			UINT32               nKeyIndex,
			VOID               * pLowerKeyValue,
			VOID               * pUpperKeyValue,
			std::set< UINT32 > * pSetOfRecordsIndexes,
			WORD                 nFindId = 0,
			std::set< UINT32 > * pSetToIntersectWith = NULL
			) { return(DTablePhysC::SpanFindInit(nKeyIndex,pLowerKeyValue,pUpperKeyValue,pSetOfRecordsIndexes,nFindId,pSetToIntersectWith)); }

		/// Те же действия, что и в SpanFindInit(), за исключением того, что функции сами определяют значение nFindId
		HRESULT SpanFindInitEx(
			_TCHAR             * pKeyName,
			VOID               * pLowerKeyValue,
			VOID               * pUpperKeyValue,
			std::set< UINT32 > * pSetOfRecordsIndexes,
			WORD               * pnFindId,
			std::set< UINT32 > * pSetToIntersectWith = NULL
			) { return(DTablePhysC::SpanFindInitEx(pKeyName,pLowerKeyValue,pUpperKeyValue,pSetOfRecordsIndexes,pnFindId,pSetToIntersectWith)); }

		/// Те же действия, что и в SpanFindInit(), за исключением того, что функции сами определяют значение nFindId
		HRESULT SpanFindInitEx(
			UINT32               nKeyIndex,
			VOID               * pLowerKeyValue,
			VOID               * pUpperKeyValue,
			std::set< UINT32 > * pSetOfRecordsIndexes,
			WORD               * pnFindId,
			std::set< UINT32 > * pSetToIntersectWith = NULL
			) { return(DTablePhysC::SpanFindInitEx(nKeyIndex,pLowerKeyValue,pUpperKeyValue,pSetOfRecordsIndexes,pnFindId,pSetToIntersectWith)); }


		/// Получение индексов записей таблицы (по значениям ключа, указанного в SpanFindInit())
		HRESULT SpanFind(WORD nFindId = 0) { return(DTablePhysC::SpanFind(nFindId)); }

};


/*! \class DFieldTopoC
	*  \brief Класс топологии: Поле данных
*/
class DFieldTopoC : public DObjC  {

	public:

		/// Конструктор
		DFieldTopoC(CONST _TCHAR * sName, WORD nIndex) :
			m_NoFindFlag(FALSE),
			m_IsUnicFlag(FALSE),
			m_IsDataFlag(FALSE),
			m_IndexedFlag(FALSE),
			m_FType(EFieldDataTypes::TYPE_INT32),
			m_FSize(0)
		{
			m_Name  = sName;
			m_IndexInParentMap = nIndex;
		}

	public: //

		/// Устанавливает переменные m_FType и m_FSize согласно имени типа
		BOOL         SetTypeInfo(_TCHAR * pTypeName);

		/// Устанавливает переменные m_FType и m_FSize согласно имени типа
		BOOL         SetTypeInfo(UINT32 iTValue);

		/// Получить имя типа
		CONST _TCHAR * GetTypeName(VOID);

		/// Тип поля
		EFieldDataTypes   m_FType;

		/// Размер поля в байтах
		INT16            m_FSize;

		/// Флаг запрета поиска
		BOOL             m_NoFindFlag;

		/// Флаг уникальности (конкретное значения поля может втречаться в таблице только один раз)
		BOOL             m_IsUnicFlag;

		/// Флаг данных (нет поиска и хеширования, и есть возможность изменения)
		BOOL             m_IsDataFlag;

		/// Флаг индекса (возможность поиска в заданном интервале)
		BOOL             m_IndexedFlag;

};





