#pragma once

#include "..\ASSInterface\ICoSeDi.h"
#include  ".\Connect.h"

namespace SS   {
	namespace CoSeDi   {

		// --------------------------------------------------
		///  ласс работы с деревом семантических классов

		class CSemanticClassesTree : 

			public SS::Core::CommonServices::CBaseCoreClass,
			public SS::Interface::Core::CoSeDi::ISemanticClassesTree   {

			public:

				CSemanticClassesTree();
				~CSemanticClassesTree();

				VOID SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager * pLoadManager);

			public:

				/// —оздать семантический класс
				/** 
					\param  IN iParentClassId - номер родительского семантического класса ( -1 == родител€ нет, класс - верхний в иерархии )
					\param  IN psClassName - им€ семантического класса
					\return TYPE_DictIndex - номер семантического класса ( 0 == oшибка)
				*/
				TYPE_DictIndex CreateClass(IN TYPE_DictIndex iParentClassId, IN std::wstring * psClassName, IN UINT32 iColor = DEFAULT_CLASS_COLOR, IN std::wstring * psShortName = NULL); 

				/// ”далить семантический класс
				/**
					\param  IN   iClassId    - номер семантического класса
					\return BOOL ( FALSE == oшибка )
				*/
				BOOL DeleteClass(IN TYPE_DictIndex iClassId);

				/// ѕереименовать семантический класс
				/**
					\param  IN   iClassId       - номер семантического класса
					\param  IN   psNewClassName - новое им€ семантического класса
					\return BOOL ( FALSE == oшибка )
				*/
				BOOL RenameClass(IN TYPE_DictIndex iClassId, IN std::wstring * psNewClassName, IN UINT32 iNewColor = DEFAULT_CLASS_COLOR, IN std::wstring * psNewShortName = NULL);


				///// ѕолучить номер семантического класса по его имени 
				///** 
				//	\param  IN psClassName - им€ семантического класса
				//	\return номер семантического класса  ( -1 == ошибка )
				//*/
				//TYPE_DictIndex GetClassIdByName(IN std::wstring * psClassName, IN TYPE_DictNumber eDictNumber);

				/// ѕолучить номер семантического класса по его короткому имени 
				/** 
					\param  IN psClassShortName - короткое им€ семантического класса
					\return номер семантического класса  ( -1 == ошибка )
				*/
				TYPE_DictIndex GetClassIdByShortName(IN std::wstring * psClassShortName, IN TYPE_DictNumber eDictNumber);

				/// ѕолучить им€ семантического класса по его номеру 
				/**
					\param  IN   iClassId    - номер семантического класса
					\param  OUT  psClassName - им€ семантического класса
					\return BOOL ( FALSE == oшибка )
				*/
				BOOL GetClassNameById(IN TYPE_DictIndex iClassId, OUT std::wstring * psClassName, OUT UINT32 * piColor = NULL, OUT std::wstring * psShortName = NULL);


				/// ѕолучить список ID дочерних семантических классов
				/** 
					\param  IN  iParentClassId - номер родительского семантического класса
					\param  OUT pChildrenList  - список номер дочерних семантических классов
					\return BOOL ( FALSE == oшибка )
				*/
				BOOL GetChildren(IN TYPE_DictIndex iParentClassId, OUT std::list<TYPE_DictIndex> * pChildrenList);

				/// ѕолучить номер семантического класса родител€
				/** 
					\param  IN iClassId    - номер семантического класса
					\return TYPE_DictIndex - номер семантического класса родител€ ( -1 == ошибка )
				*/
				TYPE_DictIndex GetClassParentId(IN TYPE_DictIndex iClassId);

				// --------------------------------

				/// 
				BOOL GetShortName(IN TYPE_DictIndex iParentClassId, OUT std::wstring * psClassShortName);

				///
				BOOL SetShortName(IN TYPE_DictIndex       iClassId);

				/// 
				BOOL ZMethod(VOID * pDataPtr1 = NULL, VOID * pDataPtr2 = NULL);


			public:

				BOOL   zm_fDbmsConnect(VOID);
				BOOL   zm_fDbmsDisConnect(VOID);
				INT    zm_iConnectionCnt;
				HANDLE zm_hMutex;

			private:

				SS::Interface::Core::ResourceManagers::ILingvoBaseManager * m_pLingvoBaseManager;
				SS::Interface::Core::DBMS::IDataBase                      * m_pDtaBaseR;
				SS::Interface::Core::DBMS::IDataBase                      * m_pDtaBaseE;
				SS::Interface::Core::DBMS::IDataTable                     * m_pTblTreeR;
				SS::Interface::Core::DBMS::IDataTable                     * m_pTblTreeE;

				SS::Interface::Core::DBMS::IDataTable                     * m_pCurrTbl;
				BOOL SetCurrTbl(UINT iDictNum);

				INT32        m_iClassId;
				INT32        m_iParentId;
				std::wstring m_sClassName;
				std::wstring m_sClassShortName;
				UINT32       m_iClassColor;

				VOID       * m_aTreeAddArr[5];
				VOID       * m_aTreeFindArr[5];

				enum   {
					fenFindInTreeByIdParent  = 0,
					fenFindInTreeByIdClass   = 1,
					fenFindInTreeByName      = 2,
					fenFindInTreeByShortName = 3
				};
		};

		class SCT_AutoConnect   {
			public:
				SCT_AutoConnect(CSemanticClassesTree * pSemanticClassesTree)   {
					m_pSemanticClassesTree = pSemanticClassesTree;
					m_pSemanticClassesTree->zm_fDbmsConnect();
				}
				~SCT_AutoConnect()   {
					m_pSemanticClassesTree->zm_fDbmsDisConnect();
				}
			private:
				CSemanticClassesTree * m_pSemanticClassesTree;
		};

		#define DBMS_CONNECT_SCT SCT_AutoConnect oAutoConnect(this)


	};
};

#define _F __FUNCTION__

///	«апись сообщени€ об ошибке в журнал системы
/**
	\param pFuncName  - им€ функции в которой произошла ошибка
	\param nFailCode  - код ошибки
	\param pFormatStr - строка форматировани€
	\param ...        - переменный список параметров
	\return HRESULT   - код ошибки nFailCode или код от GetLastError()

	«апись сообщени€ об ошибке в журнал системы
	*/
HRESULT LogFail(CHAR * pFuncName, HRESULT nFailCode, _TCHAR * pFormatStr, ...);

///	«апись простого сообщени€ в журнал системы
/**
	\param pFormatStr - строка форматировани€
	\param ... - переменный список параметров
	\return VOID

	«апись простого сообщени€ в журнал системы
	*/
VOID LogMess(_TCHAR * pFormatStr, ...);

