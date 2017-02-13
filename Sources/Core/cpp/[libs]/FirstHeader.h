/*===============================================================*\
   Определение стандартных макросов и управление компиляцией.
   Должен включаться в начало каждого создаваемого H - файла

 + Определяет макрос APL_ASSERT( expr ), который по-умолчанию опредилён
   как assert, но можно переопредилить

 + Определяет макрос APL_ASSERT_PTR( ptr ), который проверяет указатель на то что из объекта
   по этому указателю можно читать или писать данные. (Как минимум указатель не равен 0).
   Для smart указателей используется проверка на неравенство 0
   
 + Определяет макрос APL_ASSERT_PTR_TYPE( ptr, type ), Подобен APL_ASSERT_PTR, но позволяет
   указать тип на который ссылается ptr отличный от *ptr. Например для указателей void или указа-
   телей на базовый класс

 + Определяет сакрос APL_CHECK( expr ), который в отладочном режиме
   розварачивается в APL_ASSERT( expr ), а в обычном режиме остаётся только
   ( expr ). Т.е. expr в обычном режиме всё равно выполнится, но его значение
   не будет проверяно.

 + Определяет APL_CHECK_SUCCEEDED( arg ), который работает подобно APL_CHECK, но сравнивает 
   значение при помощи макроса SUCCEEDED
   
 + Определяет APL_CHECK_S_OK( arg ), который работает подобно APL_CHECK, но сравнивает 
   значение на равенство S_OK

 + DEBUG_MSG( arg ) - Выводить отладочное сообщение (по-умолчанию с помощью
   OutputDebugString), использует потоки, так что можно писать
   <аргумент_1> << <аргумент_2> << ... << <аргумент_n>,
   при условии что для каждого аргумента определён оператор
   istream &operator<<( istream &, const T & ), не выводит ничего и не
   тратит никакого процессорного времени, если определён NOT_SHOW_DEBUG_MSG или 
   SHOW_DEBUG_MSG_LOG_LEVEL определён в 0

 + DEBUG_MSG_IF( cond, arg ) - Делает тоже что и DEBUG_MSG, но только в 
   том случае если cond == true

 + DEBUG_MSG_LOG<N>( arg ) и DEBUG_MSG_IF_LOG<N>( cond, arg )
   Делает тоже что и DEBUG_MSG и DEBUG_MSG_IF, но при условии что определён
   макрос SHOW_DEBUG_MSG_LOG_LEVEL со значением большим или равным N.
   Так же можно запретить выводить сообщения уровеня лога N определив макрос
   DONT_SHOW_DEBUG_MSG_LOG_LEVEL<N>

 + APP_ID - Идентификатор приложения, используется в DEBUG_MSG,
   как префикс каждой формируемой строки

 + Определяет макрос APL_DYNAMIC_CAST который позволяет проверять приведение
   типов вверх по иерархии наследования. Если NDEBUG опредилён то при привидении
   используется static_cast, а если не определён то dynamic_cast и проверка
   возвращаемого значения на == NULL. Если это так то выводится сообщение с
   помощью _assert

 + Определяет макрос APL_ARRSIZE( arg ) который вычесляет размер массива с
   помощью sizeof(arg) / sizeof( *arg )
   
 + APL_THROW бросает текущий отределённый в системе тип исключения. Синтаксис
   схож с DEBUG_MSG.
 
 + APL_LOG записывает в текущий отределённый в системе файл протокола строку. 
   Синтаксис схож с DEBUG_MSG.

 + APL_TYPEID_NAME( arg ) возвращает typeid(arg).name() если доступна информация
   о типах в режиме выполнения (Enable Run-Time Type Information) и строку заглушку
   если такая информация недоступна
\*===============================================================*/
#ifndef FirstHeaderH
#define FirstHeaderH

//Билдер и VC подключает при компиляции в режиме отладки
#ifdef _DEBUG
   //Билдер иногда подвисает при включении отладки STL
#  if !defined( __BORLANDC__)
//#     define _STLP_DEBUG   //для отладочного режима stlport
#  endif
#else
#  ifndef NDEBUG
#		define NDEBUG           //для assert
#  endif
#endif

#define  _WIN32_WINNT 0x0403 

#ifndef NDEBUG
   #include <windows.h>
#endif

#include <assert.h>
#include <crtdbg.h>

#define APL_BREAKPOINT() DebugBreak()

#define APL_ASSERT( arg ) _ASSERTE( arg )

//Мы дожны предотвратить ошибку для объектов которые находятся в памяти только для чтения (например строки)
//Для этого специализируем функцию проверки для указателя на константный объект
#ifndef NDEBUG
template< class T >
bool TestPtr__( T *pT )
{
   return pT != NULL && !IsBadReadPtr(pT, sizeof(*pT)) && !IsBadWritePtr((LPVOID)pT, sizeof(*pT));
}

template< class T >
bool TestPtr__( const T *pT )
{
   return pT != NULL && !IsBadReadPtr(pT, sizeof(*pT));
}

template< class T >
bool TestPtr__( const T &pSmartPointer )
{
   return pSmartPointer != 0;
}

#endif

#define APL_ASSERT_PTR( ptr ) \
APL_ASSERT( TestPtr__(ptr) )

#define APL_ASSERT_PTR_TYPE( ptr, type ) \
APL_ASSERT( ptr != NULL && !IsBadReadPtr(ptr, sizeof(type)) && !IsBadWritePtr((LPVOID)ptr, sizeof(type)))

//APL_CHECK
#ifndef NDEBUG
#  define APL_CHECK( arg ) APL_ASSERT( arg )
#else
#  define APL_CHECK( arg ) ( arg )
#endif

//APL_CHECK_HR
#ifndef NDEBUG
#  define APL_CHECK_SUCCEEDED( arg ) APL_ASSERT( SUCCEEDED(arg) )
#else
#  define APL_CHECK_SUCCEEDED( arg ) ( arg )
#endif

#ifndef NDEBUG
#  define APL_CHECK_S_OK( arg ) APL_ASSERT( (arg) == S_OK )
#else
#  define APL_CHECK_S_OK( arg ) ( arg )
#endif

//APL_ARRSIZE
#define APL_ARRSIZE( arg ) ( sizeof(arg) / sizeof(*(arg)) )

//APL_DYNAMIC_CAST
#ifndef NDEBUG
template <class T, class TArg>
inline T AplTestCast__( TArg V, char *File, unsigned int Line )
{
   T tmp = dynamic_cast<T>(V);
   if( tmp == NULL )
   {
      assert( "APL_DYNAMIC_CAST - See 'FirstHeader.h'", File, Line );
   }
   return tmp;
}

#  define APL_DYNAMIC_CAST( T, arg )  AplTestCast__<T>( arg, __FILE__, __LINE__ )
#else
#  define APL_DYNAMIC_CAST( T, arg )  static_cast<T>( arg )
#endif


//#define NOT_SHOW_DEBUG_MSG     //Вывод тестовых сообщений c помощью DEBUG_MSG

//Идентификатор приложения выводится перед всеми сообщениями DEBUG_MSG
#ifndef APP_ID
#define APP_ID "<APP_ID>"
#endif

//Если не определён текущий уровен лога, то он максимально возможный т.е. показываются все сообщения кроме последнего уровня, если определён
//NOT_SHOW_DEBUG_MSG то не показываются сообщения вообще
#ifndef SHOW_DEBUG_MSG_LOG_LEVEL 
   #ifdef NOT_SHOW_DEBUG_MSG
      #define SHOW_DEBUG_MSG_LOG_LEVEL 0
   #else
      #define SHOW_DEBUG_MSG_LOG_LEVEL 9
   #endif
#endif

#if SHOW_DEBUG_MSG_LOG_LEVEL > 0
#  include <sstream>
#  include <tchar.h>
#  include <windows.h>
#endif

#if SHOW_DEBUG_MSG_LOG_LEVEL > 0
#  define DEBUG_MSG( arg ) { std::basic_stringstream</*TCHAR*/CHAR> Stream; Stream << APP_ID << '\t' << arg << std::endl; OutputDebugStringA(Stream.str().c_str()); }
#  define DEBUG_MSG_IF( cond, arg ){ if( cond ) DEBUG_MSG( arg ) }
//Варианты ниже не подходт в силу того что Времменно создаваемый объект — rvalue, т.е. получить на него константную ссылку нельзя, а значит и вызывать operator<< тоже.
//#  define DEBUG_MSG( arg ) OutputDebugStringA( static_cast<std::basic_stringstream</*TCHAR*/CHAR> &>(std::basic_stringstream</*TCHAR*/CHAR>() << APP_ID << '\t' << arg << std::endl).str().c_str() )
//#  define DEBUG_MSG_IF( cond, arg ) if( !cond ); else DEBUG_MSG( arg ) //if-else для обхода проблемы использования в безблочном if-else
#else
#  define DEBUG_MSG( arg )
#  define DEBUG_MSG_IF( cond, arg )
#endif

#if SHOW_DEBUG_MSG_LOG_LEVEL >= 1 && !defined(DONT_SHOW_DEBUG_MSG_LOG_LEVEL1)
#  define DEBUG_MSG_LOG1( arg ) DEBUG_MSG( arg )
#  define DEBUG_MSG_IF_LOG1( cond, arg ) DEBUG_MSG_IF( cond, arg )
#else
#  define DEBUG_MSG_LOG1( arg )
#  define DEBUG_MSG_IF_LOG1( cond, arg )
#endif

#if SHOW_DEBUG_MSG_LOG_LEVEL >= 2 && !defined(DONT_SHOW_DEBUG_MSG_LOG_LEVEL2)
#  define DEBUG_MSG_LOG2( arg ) DEBUG_MSG( arg )
#  define DEBUG_MSG_IF_LOG2( cond, arg ) DEBUG_MSG_IF( cond, arg )
#else
#  define DEBUG_MSG_LOG2( arg )
#  define DEBUG_MSG_IF_LOG2( cond, arg )
#endif

#if SHOW_DEBUG_MSG_LOG_LEVEL >= 3 && !defined(DONT_SHOW_DEBUG_MSG_LOG_LEVEL3)
#  define DEBUG_MSG_LOG3( arg ) DEBUG_MSG( arg )
#  define DEBUG_MSG_IF_LOG3( cond, arg ) DEBUG_MSG_IF( cond, arg )
#else
#  define DEBUG_MSG_LOG3( arg )
#  define DEBUG_MSG_IF_LOG3( cond, arg )
#endif

#if SHOW_DEBUG_MSG_LOG_LEVEL >= 4 && !defined(DONT_SHOW_DEBUG_MSG_LOG_LEVEL4)
#  define DEBUG_MSG_LOG4( arg ) DEBUG_MSG( arg )
#  define DEBUG_MSG_IF_LOG4( cond, arg ) DEBUG_MSG_IF( cond, arg )
#else
#  define DEBUG_MSG_LOG4( arg )
#  define DEBUG_MSG_IF_LOG4( cond, arg )
#endif

#if SHOW_DEBUG_MSG_LOG_LEVEL >= 5 && !defined(DONT_SHOW_DEBUG_MSG_LOG_LEVEL5)
#  define DEBUG_MSG_LOG5( arg ) DEBUG_MSG( arg )
#  define DEBUG_MSG_IF_LOG5( cond, arg ) DEBUG_MSG_IF( cond, arg )
#else
#  define DEBUG_MSG_LOG5( arg )
#  define DEBUG_MSG_IF_LOG5( cond, arg )
#endif

#if SHOW_DEBUG_MSG_LOG_LEVEL >= 6 && !defined(DONT_SHOW_DEBUG_MSG_LOG_LEVEL6)
#  define DEBUG_MSG_LOG6( arg ) DEBUG_MSG( arg )
#  define DEBUG_MSG_IF_LOG6( cond, arg ) DEBUG_MSG_IF( cond, arg )
#else
#  define DEBUG_MSG_LOG6( arg )
#  define DEBUG_MSG_IF_LOG6( cond, arg )
#endif

#if SHOW_DEBUG_MSG_LOG_LEVEL >= 7 && !defined(DONT_SHOW_DEBUG_MSG_LOG_LEVEL7)
#  define DEBUG_MSG_LOG7( arg ) DEBUG_MSG( arg )
#  define DEBUG_MSG_IF_LOG7( cond, arg ) DEBUG_MSG_IF( cond, arg )
#else
#  define DEBUG_MSG_LOG7( arg )
#  define DEBUG_MSG_IF_LOG7( cond, arg )
#endif

#if SHOW_DEBUG_MSG_LOG_LEVEL >= 8 && !defined(DONT_SHOW_DEBUG_MSG_LOG_LEVEL8)
#  define DEBUG_MSG_LOG8( arg ) DEBUG_MSG( arg )
#  define DEBUG_MSG_IF_LOG8( cond, arg ) DEBUG_MSG_IF( cond, arg )
#else
#  define DEBUG_MSG_LOG8( arg )
#  define DEBUG_MSG_IF_LOG8( cond, arg )
#endif

#if SHOW_DEBUG_MSG_LOG_LEVEL >= 9 && !defined(DONT_SHOW_DEBUG_MSG_LOG_LEVEL9)
#  define DEBUG_MSG_LOG9( arg ) DEBUG_MSG( arg )
#  define DEBUG_MSG_IF_LOG9( cond, arg ) DEBUG_MSG_IF( cond, arg )
#else
#  define DEBUG_MSG_LOG9( arg )
#  define DEBUG_MSG_IF_LOG9( cond, arg )
#endif

#if SHOW_DEBUG_MSG_LOG_LEVEL >= 10 && !defined(DONT_SHOW_DEBUG_MSG_LOG_LEVEL10)
#  define DEBUG_MSG_LOG10( arg ) DEBUG_MSG( arg )
#  define DEBUG_MSG_IF_LOG10( cond, arg ) DEBUG_MSG_IF( cond, arg )
#else
#  define DEBUG_MSG_LOG10( arg )
#  define DEBUG_MSG_IF_LOG10( cond, arg )
#endif

#ifdef _CPPRTTI 
#  define APL_TYPEID_NAME( arg ) typeid(arg).name()
#else
#  define APL_TYPEID_NAME( arg ) "<Run-Time Type Information is not avalible>"
#endif

//VS опредиляляет эти макросы, а потом они мешают
#undef min
#undef max

#ifdef NWLIB_SS_FRAMEWORK
#include "SSFramework/FirstHeaderImpl.h"
#else
#include "StandartFramework/FirstHeaderImpl.h"
#endif

#endif
