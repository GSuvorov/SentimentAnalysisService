#ifndef _SYNC_OBJ_H
#define _SYNC_OBJ_H

//#include "__Def.h"
//#include "OtxType.h"

#define EXEPTION_NO_SYNC_OBJECT                 "Synchronization::NoSyncObject"
#define EXEPTION_NULL_OBJECT_OPERATION          "Synchronization::NotNullObjectExpected"
#define EXEPTION_NOT_NULL_OBJECT_CREATION       "Synchronization::TheObjectIsBusy"

#pragma pack(push,8)  // Fix packing

#ifndef DRIVER


	/*! \class CritSectC
	*  \brief Класс Поддержки критической секции CRITICAL_SECTION
	*/
	class CritSectC   {

		/// Дружественый класс "посещение критической секции"
		friend class VisitCritSectC;

		private: // Data

			/// Счетчик вхождений
			LONG                EnterCnt;

			/// Флаг "удалить объект"
			BOOL                DeleteFl;

			/// Структура критической секции
			CRITICAL_SECTION    CritSect;

		public: // Constructor/Destructor

			/// Конструктор
			CritSectC(void)   {   DeleteFl=FALSE; EnterCnt=0; memset(&CritSect,0,sizeof(CRITICAL_SECTION)); InitializeCriticalSection(&CritSect);   }

			/// Деструктор
			~CritSectC(void)   {  DeleteCriticalSection(&CritSect); }

		public: // Functions

			/// Войти в КС
			void Enter(void)   { EnterCriticalSection(&CritSect);    InterlockedIncrement(&EnterCnt); }

			/// Покинуть КС
			void Leave(void)   { LeaveCriticalSection(&CritSect); if(InterlockedDecrement(&EnterCnt)==0 && DeleteFl) delete(this); }

			/// Удалить КС
			void Destroy(void) { DeleteFl=TRUE; }

	};

	/*! \class VisitCritSectC
	*  \brief Класс Посещения критической секции CritSectC
	*/
	class VisitCritSectC   {


		/// Дружественый класс "критическая секция"
		friend class CritSectC;

		private: // Data

			/// Указатель на КС
			CritSectC * CritSectPtr;

		public: // Constructor/Destructor

			/// Конструктор - Войти в КС
			VisitCritSectC(CritSectC & rCritSect)   { CritSectPtr=&rCritSect; if(CritSectPtr)   CritSectPtr->Enter(); }

			/// Конструктор - Войти в КС
			VisitCritSectC(CritSectC * pCritSect)   { CritSectPtr= pCritSect; if(CritSectPtr)   CritSectPtr->Enter(); }

			/// Деструктор  - Покинуть КС
			~VisitCritSectC(void)                    { if(CritSectPtr)   CritSectPtr->Leave(); }

			/// Прервать вхождение в КС
			void AbortVisitCritSect(void)           { if(CritSectPtr)   CritSectPtr->Leave();  CritSectPtr=NULL; }

	};

	/// Макро - создать критическую секцию
	#define NEW_CRIT_SECT(pCritSect)          if(pCritSect==NULL) { pCritSect = new CritSectC; }

	/// Макро - удалить критическую секцию
	#define DEL_CRIT_SECT(pCritSect)          if(pCritSect!=NULL) { pCritSect->Destroy(); pCritSect=NULL; }

	/// Макро - посетить критическую секцию
	#define VISIT_CRIT_SECT(CRIT_SECT)        VisitCritSectC Visit##CRIT_SECT(CRIT_SECT)

	/// Макро - прервать посещение критической секции
	#define ABORT_VISIT_CRIT_SECT(CRIT_SECT)  Visit##CRIT_SECT.AbortVisitCritSect()


	// // // // // // // // // // // // // // // // // // // // // // // //


	/*! \class SyncObjC
	*  \brief Базовый класс для объектов синхронизации
	*/
	class SyncObjC   {

		public:

			HANDLE Hnd;

		public:

			SyncObjC(void);
			~SyncObjC(void);

	};


	/*! \class AMEventC
	*  \brief Базовый класс для объектов Event
	*/
	class AMEventC : public SyncObjC  {

		public:

			AMEventC(bool bManual);
			~AMEventC(void);

			void Set(void);
			void Reset(void);
			void Pulse(void);

	};


	/*! \class ManualEventC
	*  \brief Класс для объектов NonAutoEvent
	*/
	class ManualEventC : public AMEventC   {

		public:

			ManualEventC(void) : AMEventC(TRUE) {};
			~ManualEventC(void)                  {};

	};


	/*! \class AutoEventC
	*  \brief Класс для объектов AutoEvent
	*/
	class AutoEventC : public AMEventC   {

		public:

			AutoEventC(void) : AMEventC(FALSE) {};
			~AutoEventC(void)                   {};

	};


	/*! \class SemaphoreC
	*  \brief Класс для объектов Semaphore
	*/
	class SemaphoreC : public SyncObjC  {

		private: // Data

			long MaxCount;

		public:

			SemaphoreC(void);
			~SemaphoreC(void);

			void Make(long nMaxCount, long nInitCount = 0L);
			long Release(long lReleaseCount, long * lpPreviousCount = NULL);
			long GetCount(void);
			void Restore(void);
	};

#else // DRIVER

	#define NEW_CRIT_SECT(pCritSect)
	#define DEL_CRIT_SECT(pCritSect)
	#define VISIT_CRIT_SECT(CRIT_SECT)
	#define ABORT_VISIT_CRIT_SECT(CRIT_SECT)

#endif

#pragma pack(pop)

#endif // _SYNC_OBJ_H


