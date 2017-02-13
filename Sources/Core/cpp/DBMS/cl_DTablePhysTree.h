#pragma once

#include "..\ASCInterface\IDBMS.h"
#include "fn_Generic.h"
#include "cl_Topo_DBMS.h"
#include "cl_DiskFile.h"
#include "cl_SyncroObjects.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_DTablePhysTree.h"

using namespace SS::Interface::Core::DBMS;

#pragma pack(push,1) // ???

#define DEF_LoIn     0x0001
#define DEF_LoEq     0x0002
#define DEF_LoInEq  (DEF_LoIn | DEF_LoEq)
#define DEF_UpIn     0x0004
#define DEF_UpEq     0x0008
#define DEF_UpInEq  (DEF_UpIn | DEF_UpEq)

///
#define BTREE_DISK_READ_BLOCK  (2*512)

///
#define BTREE_CASH_SIZE        32

///
#define BTREE_NODE_ENTRIES_LIM (  ( (BTREE_DISK_READ_BLOCK - sizeof(UINT32)) / ( sizeof(SBTreeEntry)) ) & 0xFFFE) //


/*! \struct SBTreeEntry
	*  \brief Структура - информационно-ссылочный элемент узла В-дерева (для индексированного поиска)
*/
struct SBTreeEntry   {

	/// Ссылка на запись в HASH-таблице
	UINT32 iLinkToHashRecord;

	/// Ссылка узел индексного дерева
	INT32  iLinkToBTreeNode;

	/// Отладочное значение
//	UINT32 uiLinkToTableRecord;

};


/*! \struct SBTreeNodeDisk
	*  \brief Структура узла В-дерева на диске (для индексированного поиска)
*/
struct SBTreeNodeDisk   {

	/// Число точек входа
	UINT32      uiEntriesNum;

	/// Массив точек входа
	SBTreeEntry aoEntriesArr[BTREE_NODE_ENTRIES_LIM];

};


/*! \struct SBTreeNodeMem
	*  \brief Структура узла В-дерева в памяти (для индексированного поиска)
*/
struct SBTreeNodeMem : public SBTreeNodeDisk   {

	/// Флаг изменений
	BYTE            bChangeFlag;

	/// Индекс узла на диске
	UINT32          iNodeIdx;

	/// Последняя операция
	UINT32          iLastOper;

	/// Число попаданий
	UINT32          iHitCnt;

	/// Увеличить переменную числа попаданий
	VOID IncHit(VOID)        { iHitCnt++; }

	/// Установить флаг изменений
	VOID SetChangeFlag(VOID) { bChangeFlag = TRUE; }

};

#pragma pack(pop)

/*! \struct SBTreeDataDisk
	*  \brief Заголовок В-дерева на диске (для индексированного поиска)
*/
struct SBTreeDataDisk   {

	/// Число узлов в дереве
	UINT32 m_iNumOfNodes;

	/// Высота дерева
	UINT32 m_iTreeHeight;

	/// Индекс корневого узла
	UINT32 m_iTreeRootIdx;

};

/*! \class CIndexTree
	*  \brief В-дерево (для индексированного поиска)
*/
class CIndexTree : public SBTreeDataDisk  {

	public:

		/// Массив для кэширования
		SBTreeNodeMem         * m_pCashArr;

		/// Число элементов в кэше
		UINT                    m_iCashNum;

		/// Номер текущей операции
		UINT32                  m_iCurrOper;

		/// Карта ( номер_узла, ссылка_на_ячейку_кэша )
		CContainer_HMap_Int_Int m_oCashMap;

		/// Файл индекса
		DiskFileC             * m_pFile;

		/// Ctor
		CIndexTree();

		/// Dtor
		~CIndexTree();

		/// Init
		HRESULT Init(VOID);

		/// Новый номер операции
		VOID SetNewOper(VOID)   { if(++m_iCurrOper>100000) m_iCurrOper=0; }

		/// Добавить новый узел дерева в кэш
		SBTreeNodeMem * AddNewNode(VOID);

		/// Получить узел дерева из файла или кэша
		SBTreeNodeMem * GetNode(UINT32 iNodeIdx);

		/// Начать работу с индексным файлом
		HRESULT FileOpenAndLoad(DiskFileC * pFile);

		/// Закончить работу с индексным файлом
		HRESULT FileSaveAndClose(VOID);

		/// Загрузить заголовок
		HRESULT LoadHeader(VOID);

		/// Загрузить узел в кэш
		HRESULT LoadNode(UINT32 iNodeIdx, SBTreeNodeMem * pNode);

		/// Сохранить заголовок
		HRESULT SaveHeader(VOID);

		/// Сохранить узел
		HRESULT SaveNode(SBTreeNodeMem * pNode);

		/// Найти свободную ячейку кэша
		SBTreeNodeMem * FindFreeCashCell(VOID);

};


