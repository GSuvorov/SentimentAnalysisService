#pragma once

#include "..\ASSInterface\ICoSeDi.h"
#include  ".\Connect.h"

namespace SS   {
	namespace CoSeDi   {

		// --------------------------------------------------
		/// Интерфейс работы связывающей слова с деревом семантических классов

		class CSemanticClassesWords : 
			public SS::Core::CommonServices::CBaseCoreClass,
			public SS::Interface::Core::CoSeDi::ISemanticClassesWords   {

			public:

				CSemanticClassesWords();
				~CSemanticClassesWords();

				VOID SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager * pLoadManager);

			public:

				/// Устанавливает номер семантического класса для работы
				/** 
					\param iCurrentClass - номер семантического
				*/
				VOID SetClass(IN TYPE_DictIndex iCurrentClass)   {
					m_iCurSemClass = iCurrentClass;
				}

				/// Возвращает номер установленного семантического класса
				/** 
					\param VOID
				*/
				TYPE_DictIndex GetClass(VOID)   {
					return(m_iCurSemClass);
				}

				/// Возвращает список индексов семантических классов по индексу слова
				/** 
					\param iIDSource - индекс слова
					\param pClassesList - указатель на список индексов семантических классов
				*/
				BOOL GetClasses(IN TYPE_DictIndex iIDSource, OUT std::list<TYPE_DictIndex> * pClassesList);

				/// Возвращает список слов, которые относятся к текущему семантическому классу
				/** 
					\param pIDSourcesList -список индексов слов, относящиеся к установленному семантическому классу
					TRUE - операция выполнена
				*/
				BOOL GetWords(OUT std::list<TYPE_DictIndex> * pIDSourcesList);

				/// Добавляет слово к текущему семантическому классу
				/** 
					\param iIDSource - индекс слова
					\return TRUE - операция выполнена
				*/
				BOOL AddWord(IN TYPE_DictIndex iIDSource);

				/// Удаляет слово, относящееся к текущему семантическому классу
				/** 
					\param iIDSource - индекс слова
					\return TRUE - операция выполнена
				*/
				BOOL DeleteWord (IN TYPE_DictIndex iIDSource);

			public:
		
				BOOL   zm_fDbmsConnect(VOID);
				BOOL   zm_fDbmsDisConnect(VOID);
				INT    zm_iConnectionCnt;
				HANDLE zm_hMutex;

			private:

				TYPE_DictIndex m_iCurSemClass;

				SS::Interface::Core::ResourceManagers::ILingvoBaseManager * m_pLingvoBaseManager;
				SS::Interface::Core::DBMS::IDataBase                      * m_pDtaBaseR;
				SS::Interface::Core::DBMS::IDataBase                      * m_pDtaBaseE;
				SS::Interface::Core::DBMS::IDataTable                     * m_pTblWordR;
				SS::Interface::Core::DBMS::IDataTable                     * m_pTblWordE;

				SS::Interface::Core::DBMS::IDataTable                     * m_pCurrTbl;
				BOOL SetCurrTbl(UINT iDictNum);

				INT32        m_iWordId;
				INT32        m_iClassId;

				VOID       * m_aWordAddArr[2];
				VOID       * m_aWordFindArr[2];

				enum   {
					fenFindInTreeByIdWord  = 0,
					fenFindInTreeByIdClass = 1,
				};
		};

		class SCW_AutoConnect   {
			public:
				SCW_AutoConnect(CSemanticClassesWords * pSemanticClassesWords)   {
					m_pSemanticClassesWords = pSemanticClassesWords;
					m_pSemanticClassesWords->zm_fDbmsConnect();
				}
				~SCW_AutoConnect()   {
					m_pSemanticClassesWords->zm_fDbmsDisConnect();
				}
			private:
				CSemanticClassesWords * m_pSemanticClassesWords;
		};

		#define DBMS_CONNECT_SCW SCW_AutoConnect oAutoConnect(this)

	};
};

#define _F __FUNCTION__

///	Запись сообщения об ошибке в журнал системы
/**
	\param pFuncName  - имя функции в которой произошла ошибка
	\param nFailCode  - код ошибки
	\param pFormatStr - строка форматирования
	\param ...        - переменный список параметров
	\return HRESULT   - код ошибки nFailCode или код от GetLastError()

	Запись сообщения об ошибке в журнал системы
	*/
HRESULT LogFail(CHAR * pFuncName, HRESULT nFailCode, _TCHAR * pFormatStr, ...);

///	Запись простого сообщения в журнал системы
/**
	\param pFormatStr - строка форматирования
	\param ... - переменный список параметров
	\return VOID

	Запись простого сообщения в журнал системы
	*/
VOID LogMess(_TCHAR * pFormatStr, ...);
