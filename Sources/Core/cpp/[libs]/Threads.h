#ifndef Threads_H
#define Threads_H

#include "StlAuxiliary.h"
#include <set>
#include <process.h>

/**
   Модуль предоставляет ряд классов для обеспечения синхронизации в многопоточных приложениях
*/ 

//Набор макросов настройки поведения для отладки
//#define DEFAULT_THREAD_DEBUG_INFO LockDebugInfo
#define DEFAULT_THREAD_DEBUG_INFO NoDebugInfo

#ifndef NDEBUG
   #define CHECK_DEADLOCKS_FOR_POINTERS true
#else
   #define CHECK_DEADLOCKS_FOR_POINTERS false
#endif


// Привет от товарища Рихтера.
// This macro function calls the C runtime's _beginthreadex function.
// The C runtime library doesn't want to have any reliance on Windows' data
// types such as HANDLE. This means that a Windows programmer needs to cast
// values when using _beginthreadex. Since this is terribly inconvenient,
// I created this macro to perform the casting.
typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr,     \
   pvParam, fdwCreate, pdwThreadId)                     \
   ((HANDLE)_beginthreadex(                             \
   (void *)         (psa),                              \
   (unsigned)       (cbStack),                          \
   (PTHREAD_START) (pfnStartAddr),                      \
   (void *)         (pvParam),                          \
   (unsigned)       (fdwCreate),                        \
   (unsigned *)     (pdwThreadId)))


namespace SS { namespace Lib {

namespace Detail
{
/**
   Синхронизация в сценарии "один писатель/группа читателей" от Джефри Рихтера.

   - Когда один поток что-то пишет в область общих данных, другие этого делать не могут. 
   - Когда один поток что-то пишет в область общих данных, другие не могут ничего считывать оттуда. 
   - Когда один поток считывает что-то из области общих данных, другие не могут туда ничего записывать. 
   - Когда один поток считывает что-то из области общих данных, другие тоже могут это делать. 
*/

class CSWMRG {
public:
   CSWMRG();                 // Constructor
   ~CSWMRG();                // Destructor

   VOID WaitToRead();        // Call this to gain shared read access
   VOID WaitToWrite();       // Call this to gain exclusive write access
   VOID Done();              // Call this when done accessing the resource

private:
   CRITICAL_SECTION m_cs;    // Permits exclusive access to other members
   HANDLE m_hsemReaders;     // Readers wait on this if a writer has access
   HANDLE m_hsemWriters;     // Writers wait on this if a reader has access
   int    m_nWaitingReaders; // Number of readers waiting for access
   int    m_nWaitingWriters; // Number of writers waiting for access
   int    m_nActive;         // Number of threads currently with access
   //   (0=no threads, >0=# of readers, -1=1 writer)
};
} // namespace Detail

//Стратегии используемые для реализации блокировки
//Стратегия критической секции
struct SCriticalSectionImplStategy
{
   typedef CRITICAL_SECTION TLockType;

   static void Create( TLockType &LT ){ ::InitializeCriticalSectionAndSpinCount(&LT, 4000); }
   static void Destroy( TLockType &LT ){ ::DeleteCriticalSection(&LT); }
   static void Lock( TLockType &LT ){ ::EnterCriticalSection(&LT); }
   static void UnLock( TLockType &LT ){ ::LeaveCriticalSection(&LT); }
};

//Стратегия для использования сценария "один писатель/группа читателей", на основе класса CSWMRG
struct SSWMRImplStategy
{
   typedef Detail::CSWMRG TLockType;

   static void Create( TLockType &LT ) {}
   static void Destroy( TLockType &LT ) {}
   static void LockRead( TLockType &LT ) { LT.WaitToRead(); }
   static void UnLockRead( TLockType &LT ) { LT.Done(); }
   static void LockWrite( TLockType &LT ) { LT.WaitToWrite(); }
   static void UnLockWrite( TLockType &LT ) { LT.Done(); }
};

/**
   Несколько взяимозаменяемых классов для обеспечения различных видов блокировки.
   Блокировка осуществляется созданием класса Loсk и снимается при его удалении.

   Таже существувует возможность блокировки с использованием Концепции 
   "один писатель/несколько читателей" Для этого создаются объекты:
      LockRead - для блокировки чтения
      LockWrite - для блокировки записи 
*/

//Вывод отладочной информации о запираниях объектов
template<class Host>
class LockDebugInfo
{
   static size_t Count_;
   size_t CurNum_;

   void ShowMessage( const char *Str ) const 
   {  DEBUG_MSG( APL_TYPEID_NAME(Host) << " " << (unsigned int)CurNum_ << " " << Str << " " << GetCurrentThreadId() ); }
public:
   LockDebugInfo(): CurNum_( Count_++ ) {}

   void TryLock() const { ShowMessage( "TryLock" ); } 
   void LockOk() const { ShowMessage( "LockOk" ); }
   void Unlock() const { ShowMessage( "Unlock" ); }
   void TryLockRead() const { ShowMessage( "TryLockRead" ); } 
   void LockOkRead() const { ShowMessage( "LockOkRead" ); }
   void UnlockRead() const { ShowMessage( "UnlockRead" ); }
   void TryLockWrite() const { ShowMessage( "TryLockWrite" ); } 
   void LockOkWrite() const { ShowMessage( "LockOkWrite" ); }
   void UnlockWrite() const { ShowMessage( "UnlockWrite" ); }
};

template<class Host>
size_t LockDebugInfo<Host>::Count_ = 0;  

template<class Host>
class NoDebugInfo
{
public:
   void TryLock() const {}
   void LockOk() const {}
   void Unlock() const {}
   void TryLockRead() const {} 
   void LockOkRead() const {}
   void UnlockRead() const {}
   void TryLockWrite() const {} 
   void LockOkWrite() const {}
   void UnlockWrite() const {}
};

///Отсутсвие всякой блокировки (заглушка)
template <class Host, class ImplStategy = SCriticalSectionImplStategy, template<class> class DebugInfo = DEFAULT_THREAD_DEBUG_INFO>
class NoLockable: public NonCopyable
{
public:
   struct Lock: public NonCopyable
   {
      Lock() {}
      explicit Lock(const NoLockable&) {}
   };
   
   struct LockWrite: public NonCopyable
   {
      LockWrite() {}
      explicit LockWrite(const NoLockable&) {}
   };

   struct LockRead: public NonCopyable
   {
      LockRead() {}
      explicit LockRead(const NoLockable&) {}
   };

   enum { NoLockableClass = true }; //Является ли этот класс NoLockable
};


//Блокировка на уровне объекта
template <class Host, class ImplStategy = SCriticalSectionImplStategy, template<class> class DebugInfo = DEFAULT_THREAD_DEBUG_INFO>
class ObjectLevelLockable: public NonCopyable, private DebugInfo<Host>
{
   mutable typename ImplStategy::TLockType mtx_;

public:
   explicit ObjectLevelLockable()
   {
      ImplStategy::Create(mtx_);
   }

   ~ObjectLevelLockable()
   {
      ImplStategy::Destroy(mtx_);
   }

   class Lock;
   class LockRead;
   class LockWrite;

   friend class Lock;
   friend class LockRead;
   friend class LockWrite;

   class Lock: public NonCopyable
   {
      const ObjectLevelLockable& host_;
      
      Lock(); //Нельзя вызывать
   public:

      explicit Lock(const ObjectLevelLockable& host) : host_(host)
      {
         host_.TryLock();
         ImplStategy::Lock(host_.mtx_);
         host_.LockOk();
      }

      ~Lock()
      {
         ImplStategy::UnLock(host_.mtx_);
         host_.Unlock();
      }
   };

   class LockRead: public NonCopyable
   {
      const ObjectLevelLockable& host_;

      LockRead(); //Нельзя вызывать
   public:

      explicit LockRead(const ObjectLevelLockable& host) : host_(host)
      {
         host_.TryLockRead();
         ImplStategy::LockRead(host_.mtx_);
         host_.LockOkRead();
      }

      ~LockRead()
      {
         ImplStategy::UnLockRead(host_.mtx_);
         host_.UnlockRead();
      }
   };

   class LockWrite: public NonCopyable
   {
      const ObjectLevelLockable& host_;

      LockWrite(); //Нельзя вызывать
   public:

      explicit LockWrite(const ObjectLevelLockable& host) : host_(host)
      {
         host_.TryLockWrite();
         ImplStategy::LockWrite(host_.mtx_);
         host_.LockOkWrite();
      }

      ~LockWrite()
      {
         ImplStategy::UnLockWrite(host_.mtx_);
         host_.UnlockWrite();
      }
   };

   enum { NoLockableClass = false }; //Является ли этот класс NoLockable
};

//Блокировка на уровне класса
template <class Host, class ImplStategy = SCriticalSectionImplStategy, template<class> class DebugInfo = DEFAULT_THREAD_DEBUG_INFO>
class ClassLevelLockable: public NonCopyable
{
   struct Initializer: public DebugInfo<Host> 
   {   
      typename ImplStategy::TLockType mtx_;

      Initializer()
      {
         ImplStategy::Create(mtx_);
      }
      ~Initializer()
      {
         ImplStategy::Destroy(mtx_);
      }
   };

   static Initializer initializer_;

public:
   class Lock;
   class LockRead;
   class LockWrite;

   friend class Lock;
   friend class LockRead;
   friend class LockWrite;

   class Lock: public NonCopyable
   {
    public:
      Lock()
      {
         initializer_.TryLock();
         ImplStategy::Lock(initializer_.mtx_);
         initializer_.LockOk();
      }
      explicit Lock(const ClassLevelLockable&)
      {
         initializer_.TryLock();
         ImplStategy::Lock(initializer_.mtx_);
         initializer_.LockOk();
      }
      ~Lock()
      {
         ImplStategy::UnLock(initializer_.mtx_);
         initializer_.Unlock();
      }
   };

   class LockRead: public NonCopyable
   {
   public:
      LockRead()
      {
         initializer_.TryLockRead();
         ImplStategy::LockRead(initializer_.mtx_);
         initializer_.LockOkRead();
      }
      explicit LockRead(const ClassLevelLockable&)
      {
         initializer_.TryLockRead();
         ImplStategy::LockRead(initializer_.mtx_);
         initializer_.LockOkRead();
      }
      ~LockRead()
      {
         ImplStategy::UnLockRead(initializer_.mtx_);
         initializer_.UnlockRead();
      }
   };

   class LockWrite: public NonCopyable
   {
   public:
      LockWrite()
      {
         initializer_.TryLockWrite();
         ImplStategy::LockWrite(initializer_.mtx_);
         initializer_.LockOkWrite();
      }
      explicit LockWrite(const ClassLevelLockable&)
      {
         initializer_.TryLockWrite();
         ImplStategy::LockWrite(initializer_.mtx_);
         initializer_.LockOkWrite();
      }
      ~LockWrite()
      {
         ImplStategy::UnLockWrite(initializer_.mtx_);
         initializer_.UnlockWrite();
      }
   };

   enum { NoLockableClass = false }; //Является ли этот класс NoLockable
};

template <class Host, class ImplStategy, template<class> class DebugInfo>
typename ClassLevelLockable<Host, ImplStategy, DebugInfo>::Initializer ClassLevelLockable<Host, ImplStategy, DebugInfo>::initializer_;

/**
   LockedPtr
   Класс позволяет эмулировать поведение указателя, но при этом автоматически блокирует 
   объект при вызове оператора ->. Т.е. создаётся временный объект в конструкторе он бло-
   кирует объект, вызывается необходимый метод и затем в деструкторе снимается блокировка
   Объекты класса можно свободно присваивать друг другу без каких либо побочных эфектов
   Необходимо чтобы поддерживающий блокировку класс наследовался от одного из объектов
   Lockable.
   
   + Указателям на константные объекты можно присваивать указатели на неконстантные (но не ноборот)
   + Указатели можно привести к типу bool и проверить на равенство (неравенство 0)

   ВНИМАНИЕ:
      Есть очень важное отличие указателей с автоматической блокировкой от обычных указателей это, то
      что блокировка всех объектов в выражение остаётся на всё время выполнения выражение. Т.е. одновременно 
      могут быть запертыми сразу несколько объектов, например есть два указателя с автоматической блокировкой
      указывающее на разные объекты pA и pB, тогда если написать выражение:
         pA->AnyFunc1() && pB->AnyFunc2()
      
      Объекты на которые указывает pA и pB остануться запертыми на всё время выполнения AnyFunc1 и AnyFunc2. Если 
      другой поток вызовет pB->AnyFunc2() && pA->AnyFunc1() то возможен DeadLock, т.е. взаимная блокировка двук 
      потокв.

      В связи с этим необходимо не допустить создания двух объектов LockedPtrBase<T>::Proxy в одном потоку.
      Для этого будем использовать стратегию которая следит за этим.
*/

namespace Detail
{
template <class T>
class LockedPtrBase
{
private:
   T *pObj_;

private:
   //Занимается блокировкой объекта
   template<class LockObj>
   class Proxy;

protected:  
   LockedPtrBase( T *pObj = 0 ): pObj_(pObj) {}
   //LockedPtrBase( TUndefinedBoolType pT = 0 ): pObj_(0) {} //Позволяет конструировать только из 0


   //Такой подход позволяет компилятору (хотя бы VC7) полносью сократить накладные расходы на блокировку
   //объекта унаследованного от NoLockable (т.е. такого который в блокировки не нуждается)
   //В принципе можно было всегда возвращать Proxy, но компилятор не достаточно оптимизирует такой подход
   //(У VC7 остаются два вызова функции)
   typedef typename Select< T::NoLockableClass, T *, Proxy<typename T::Lock> >::Result TReturnType;
public:
   void Set( T *pObj ){ pObj_ = pObj; }
   T *Get() const { return pObj_; }
   TReturnType operator->() const { APL_ASSERT(pObj_ != 0); return TReturnType(pObj_); }

   //Позволим проверять указатель на равенство нулю
private:  
   struct OperatorHelper{ int i; }; //см. Александровску(loki)
   typedef int OperatorHelper::*TUndefinedBoolType;
public:
   //Позволет проверять указатель на равенство/неравенство нулю и приводить к типу bool
   operator TUndefinedBoolType() const { return pObj_ != 0? &OperatorHelper::i : 0; }

   //Если не определять то будет использоваться автоматическое преобразование к TUndefinedBoolType
   //и встроенные операторы
   friend bool operator==( const LockedPtrBase &P1, const LockedPtrBase &P2 ){ return P1.pObj_ == P2.pObj_; }
   friend bool operator!=( const LockedPtrBase &P1, const LockedPtrBase &P2 ){ return P1.pObj_ != P2.pObj_; }

   //Если не опредилять по при проверке P1 == 0, 0 == P1, P1 != 0, 0 != P1 возникнет 
   //неоднозначность т.к. конструтор не explicit
   friend bool operator==( const LockedPtrBase &P1, TUndefinedBoolType P2 ){ return P1.pObj_ == 0; }
   friend bool operator==( TUndefinedBoolType P2, const LockedPtrBase &P1 ){ return P1.pObj_ == 0; }
   friend bool operator!=( const LockedPtrBase &P1, TUndefinedBoolType P2 ){ return P1.pObj_ != 0; }
   friend bool operator!=( TUndefinedBoolType P2, const LockedPtrBase &P1 ){ return P1.pObj_ != 0; }

};

//Слежение за тем чтобы создавалось не более одного объекта Proxy в одном потоке
template<bool Check, int Dummy>
class CDeadLockCheck
{};

//Переменная Dummy нужна для того чтобы не создавать cpp-шный файл с определением статических переменных
template<int Dummy>
class CDeadLockCheck<true, Dummy>
{
   //Будем в set хранить идентификаторы потоков в которых уже создан объект Proxy
   typedef std::set<DWORD> TThreadsID;
   typedef ObjectLevelLockable< CDeadLockCheck<true, Dummy> > TMutex;
   static TThreadsID m_ThreadsID;
   static TMutex m_Mutex;

public:
   CDeadLockCheck()
   {
      TMutex::Lock Guard( m_Mutex );

      //Срабатывает если создано несколько объектов Proxy в одном потоке. См. выше.
      APL_ASSERT( m_ThreadsID.insert(::GetCurrentThreadId()).second == true );
   }

   ~CDeadLockCheck()
   {
      TMutex::Lock Guard( m_Mutex );

      APL_ASSERT( m_ThreadsID.erase(::GetCurrentThreadId()) == 1 );
   }
};

template<int Dummy> typename CDeadLockCheck<true, Dummy>::TThreadsID CDeadLockCheck<true, Dummy>::m_ThreadsID;
template<int Dummy> typename CDeadLockCheck<true, Dummy>::TMutex CDeadLockCheck<true, Dummy>::m_Mutex;

//Времнный объект который запирает и отпирает объект
template< class T >
template<class LockObj>
class LockedPtrBase<T>::Proxy: private LockObj, private CDeadLockCheck<CHECK_DEADLOCKS_FOR_POINTERS, 0>
{
   T *pObj_;

   Proxy& operator=( const Proxy& ); 
public:
   //Компилятор должен провести RVO и функция не должна вызваться иначе ошибка при линковки.
   //Стандарт к этому не обязывает и поэтому не гарантируется что будет работать на всех компиляторах
   Proxy( const Proxy & ); 

   explicit Proxy( T *pObj ): pObj_(pObj), LockObj(*pObj) {}
   T *operator->() const { return pObj_; }
};

}  //namespace Detail

template <class T>
class LockedPtr: public Detail::LockedPtrBase<T>
{
public:
   /*explicit*/ LockedPtr( T *pObj = 0): Detail::LockedPtrBase<T>( pObj ) {}
};

//Специализируем для константного объекта
template <class T>
class LockedPtr<const T>: public Detail::LockedPtrBase<const T>
{
public:
   /*explicit*/ LockedPtr( T *pObj = 0): Detail::LockedPtrBase<const T>( pObj ) {}

   //Мы можем присвоить константному указателю неконстантный, но не наоборот
   LockedPtr( const LockedPtr<T> & Other ): Detail::LockedPtrBase<const T>( Other.Get() ) {}

   LockedPtr<const T> &operator=( const LockedPtr<T> & Other ) 
   {
      Set( Other.Get() );
      return *this;
   }
};

}} //namespace SS::Lib 

#endif