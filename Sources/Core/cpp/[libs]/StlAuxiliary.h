#ifndef StlAuxiliary_H
#define StlAuxiliary_H

/*
   Модуль предоставляет несколько классов и функций которые могут служить расширением stl
*/

namespace SS { namespace Lib {

///////////////////////////////////////////////////////////////////////////////
// Пустой тип
///////////////////////////////////////////////////////////////////////////////
class NullType {};

///////////////////////////////////////////////////////////////////////////////
// Запрещает копирование объекта
///////////////////////////////////////////////////////////////////////////////
class NonCopyable
{
protected:
   NonCopyable() {}
   ~NonCopyable() {}
private:  
   NonCopyable( const NonCopyable& );
   NonCopyable& operator=( const NonCopyable& ); 
};

///////////////////////////////////////////////////////////////////////////////
// Запрещает создавать класс любым методом который вызывает конструктор и 
// удалять любым методом который вызывает деструктор.
// Например это можно использовать при запрещении явного создания объектов.
// Для предотвращения предупрежедения (C4624) о том что деструктор не может быть 
// сгенерирован надо явно его объявить (не опредилить) в произодном классе
///////////////////////////////////////////////////////////////////////////////
struct NonCreateable 
{
private:
   NonCreateable();    
   ~NonCreateable();  
};

////////////////////////////////////////////////////////////////////////////////
// class template Int2Type
// Converts each integral constant into a unique type
// Invocation: Int2Type<v> where v is a compile-time constant integral
// Defines 'value', an enum that evaluates to v
// Реализация из loki
////////////////////////////////////////////////////////////////////////////////
template <int v>
struct Int2Type
{
   enum { value = v };
};

////////////////////////////////////////////////////////////////////////////////
// class template Type2Type
// Converts each type into a unique, insipid type
// Invocation Type2Type<T> where T is a type
// Defines the type OriginalType which maps back to T
// Реализация из loki
////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct Type2Type
{   
   typedef T OriginalType;
   Type2Type(){} // VC7
};

////////////////////////////////////////////////////////////////////////////////
// class template Select
// Selects one of two types based upon a boolean constant
// Invocation: Select<flag, T, U>::Result
// where:
// flag is a compile-time boolean constant
// T and U are types
// Result evaluates to T if flag is true, and to U otherwise.
// Реализация из loki
////////////////////////////////////////////////////////////////////////////////
template <bool flag, typename T, typename U>
struct Select
{
private:
   template<bool>
   struct In 
   { typedef T Result; };

   template<>
   struct In<false>
   { typedef U Result; };

public:
   typedef typename In<flag>::Result Result;
};


////////////////////////////////////////////////////////////////////////////////
// class template IsSameType
// Return true if two given types are the same
// Invocation: IsSameType<T, U>::value
// where:
// T and U are types
// Result evaluates to true if U == T (types equal)
// Реализация из loki
////////////////////////////////////////////////////////////////////////////////
template <typename T, typename U>
struct IsSameType
{
private:
   template<typename>
   struct In 
   { enum { value = false }; };

   template<>
   struct In<T>
   { enum { value = true };  };

public:
   enum { value = In<U>::value };
};

///////////////////////////////////////////////////////////////////////////////
// Убирает квалификатор const, если таковой имеется у типа.
// Реализация из loki
///////////////////////////////////////////////////////////////////////////////
template <class U> struct UnConst
{
   typedef U Result;
   enum { isConst = 0 };
};

template <class U> struct UnConst<const U>
{
   typedef U Result;
   enum { isConst = 1 };
};

////////////////////////////////////////////////////////////////////////////////
// class template Conversion
// Figures out the conversion relationships between two types
// Invocations (T and U are types):
// a) Conversion<T, U>::exists
// returns (at compile time) true if there is an implicit conversion from T
// to U (example: Derived to Base)
// b) Conversion<T, U>::exists2Way
// returns (at compile time) true if there are both conversions from T
// to U and from U to T (example: int to char and back)
// c) Conversion<T, U>::sameType
// returns (at compile time) true if T and U represent the same type
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
// Реализация из loki
////////////////////////////////////////////////////////////////////////////////

template <class T, class U>
struct Conversion
{
   template <class T2, class U2>
   struct ConversionHelper
   {
      typedef char Small;
      struct Big { char dummy[2]; };
      static Big   Test(...);
      static Small Test(U2);
      static T2 MakeT();
   };
  
   typedef ConversionHelper<T, U> H;
#ifndef __MWERKS__
   enum { exists = sizeof(typename H::Small) == sizeof((H::Test(H::MakeT()))) };
#else
   enum { exists = false };
#endif
   enum { exists2Way = exists && Conversion<U, T>::exists };
   enum { sameType = false };
};

template <class T>
struct Conversion<T, T>    
{
   enum { exists = 1, exists2Way = 1, sameType = 1 };
};

template <class T>
struct Conversion<void, T>    
{
   enum { exists = 0, exists2Way = 0, sameType = 0 };
};

template <class T>
struct Conversion<T, void>    
{
   enum { exists = 0, exists2Way = 0, sameType = 0 };
};

template <>
struct Conversion<void, void>    
{
public:
   enum { exists = 1, exists2Way = 1, sameType = 1 };
};

///////////////////////////////////////////////////////////////////////////////
// Создание из одного типа второго, только в том случае если это возможно. 
// Если создание невозможно то первый тип создаётся из конструктора по умолчанию
// Синтаксис:
//    T1 t1;
//    T2 t2(TryConvertOrCreate<T1, T2>::Get(t1));
// Синтаксис 2:
//    T1 t1;
//    T2 t2;
//    t2 = TryConvertOrCreate<T1, T2>::Get(t1);
// Принцип работы:
//    Если возможно преобразование из T1 в T2 то функция Get возвратит ссылку 
//    на t1 и t2 корректно создаться из t1, а если преобразование не определено
//    то Get возвратит T2 созданный при помощи конструктора по умолчанию и t2
//    создаться из конструктора копирования.
///////////////////////////////////////////////////////////////////////////////
namespace Private
{
template<class FromT, class ToT, bool Flag>
struct TryConvertOrCreateHelper
{
   static const FromT &Get( const FromT &From ) { return From; }
};

template<class FromT, class ToT>
struct TryConvertOrCreateHelper<FromT, ToT, false>
{
   static ToT Get( const FromT &From ) { return ToT(); }
};
} //namespace Private

template<class FromT, class ToT>
struct TryConvertOrCreate: Private::TryConvertOrCreateHelper<FromT, ToT, Conversion<FromT, ToT>::exists>{};

///////////////////////////////////////////////////////////////////////////////
// Позволяет считать количество инстанцированных в данный момент объектов
///////////////////////////////////////////////////////////////////////////////
template< class T >
class CObjectCount
{
   static size_t m_N;

private:
   static void Increment(){ APL_ASSERT(m_N < std::numeric_limits<size_t>::max() ); ++m_N; }
   static void Decrement(){ APL_ASSERT( m_N > 0 ); --m_N; }

public:
   CObjectCount() { Increment(); }
   CObjectCount( const CObjectCount& ) { Increment(); }
   ~CObjectCount() { Decrement(); }

   static size_t Count(){ return m_N; }
};

template< class T >
size_t CObjectCount<T>::m_N = 0;

///////////////////////////////////////////////////////////////////////////////
// Преобразовыввает строку в формат числа типа int
// Строка символов должна иметь следующий формат:
//    [ws][sn][ddd]
// Где:
//    [ws]  = необязательные пробелы или табуляции;
//    [sn]  = необязательный знак (+ или -);
//    [ddd] = обязательные цифры;
//
// Beg  = Итератор начиная с которого начнётся распознавание;
// End  = Итератор конца строки;
//
// При обнаружении первого нерспознаваемого символа или при достижении End
// функция прекращает преобразование, возвращает итератор текущего символа
// и записывает получившеяся число в Val (если не встретилось ни одного числа
// то 0). Если не встретилась ни одна цифра то возвращает Beg
// Функция не проверяет число на переполнение
///////////////////////////////////////////////////////////////////////////////
template<class InputIterator, class T>
InputIterator ConvertStringToInteger( InputIterator Beg, InputIterator End, T &Val )
{
//В связи с тем что все символы для проверки которые используются в этой функции в UNICODE представлении
//имеют тот же код что и в ANSI (только 16 битный) можно использовать одну функцию как для UNICODE так и для 
//ANSI строк
#ifdef WIDE_PREF
   #error "Уже определён WIDE_PREF"
#endif

#define WIDE_PREF( arg ) /*L##arg*/ arg

   bool IsNegate = false;

   InputIterator BegBack(Beg); //Сохраняем позицию начального итератора

   Val = 0;

   //Пропускаем пробелы
   while( Beg != End && (*Beg == WIDE_PREF('\t') || *Beg == WIDE_PREF(' ') ) ) ++Beg;

   if( Beg == End ) return BegBack;

   //Разбираемся со знаком
   if( *Beg == WIDE_PREF('-') )
   {
      IsNegate = true;
      ++Beg;
   }
   else if( *Beg == WIDE_PREF('+') )
   {
      ++Beg;
   }

   InputIterator FirstDigit(Beg); //Сохраняем позицию предполагаемой первой цифры

   //Начинаем обрабатывать цифры
   while( Beg != End && (*Beg >= WIDE_PREF('0') && *Beg <= WIDE_PREF('9')) )
   {
      Val = 10 * Val + *Beg - WIDE_PREF('0');
      ++Beg;
   }

   if( Beg == FirstDigit ) //Мы не преобразовали не одну цифру
      return BegBack;

   #pragma warning( push )
   #pragma warning( disable: 4146 )
   if( IsNegate ) Val = -Val;
   #pragma warning( pop )

   return  Beg;

#undef WIDE_PREF
}

///////////////////////////////////////////////////////////////////////////////
// Преобразовать число типа int в строку, если число отрицательное так же выводится знак '-'
// Val - преобразуемое число
// [Beg, End) - Строка в которую будет записано литеральное представление переданного числа
// Возвр: Итератор стоящий за последним записанным символом. Если интервала [Beg, End) для
//        записи всех символов числа не хватает возвращается Beg
//
// ФУНКЦИЯ НЕ ЗАПИСЫВАЕТ В КОНЦЕ '\0' и поэтому при записи в массив смволов
// имеет смысел передавать End = Beg + RG_SIZE - 1, где RG_SIZE - размер массива 
// для того чтобы оставить одну позицию для '\0'
///////////////////////////////////////////////////////////////////////////////

template<class InputIterator, class T>
InputIterator ConvertIntegerToString( T Val, InputIterator Beg, InputIterator End )
{
   InputIterator Cur(Beg);        //Текущий символ
   InputIterator FirstDigit(Beg); //Первая записанная цифра
   T Radix(10);                   //База системы исчисления
   
   if( Val < 0 ) //Если число отрицательное выводим знак
   {
      if( Cur == End ) return Beg;

      #pragma warning( push )
      #pragma warning( disable: 4146 )
      Val = -Val;
      #pragma warning( pop )

      *Cur = '-';
      ++Cur;
      ++FirstDigit;
   }

   do
   {
      if( Cur == End ) return Beg;

      *Cur = (Val % Radix) + '0';   
      ++Cur;

      Val /= Radix;
   }
   while( Val > 0 );

   InputIterator RetVal(Cur); //Итератор стоящий за последним записанным символом

   //Мы получили перевёрнутое число. Востанавливаем нормальный порядок
   --Cur;

   while( FirstDigit < Cur ) 
   {
      //используем Radix, как временную переменную
      Radix = *Cur; *Cur = *FirstDigit; *FirstDigit = Radix;
      ++FirstDigit;
      --Cur;
   } 

   return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
//Выполняет тоже что и stl::mismatch, но отличается тем что не требует чтобы
//второй диапозон был не меньше чем первый. Т.е. функция останавливается как
//при first1 != last1 так и при first2 != last2
///////////////////////////////////////////////////////////////////////////////
template <class InputIterator1, class InputIterator2>
std::pair<InputIterator1, InputIterator2>
Mismatch(InputIterator1 first1, InputIterator1 last1,
         InputIterator2 first2, InputIterator2 last2 )
{
   while( first1 != last1 && first2 != last2 && *first1 == *first2 )
   {
      ++first1;
      ++first2;
   }

   return std::pair< InputIterator1, InputIterator2 >( first1, first2 );
}

template <class InputIterator1, class InputIterator2, class BinaryPredicate>
std::pair<InputIterator1, InputIterator2>
Mismatch(InputIterator1 first1, InputIterator1 last1,
         InputIterator2 first2, InputIterator2 last2, BinaryPredicate binary_pred )
{
   while( first1 != last1 && first2 != last2 && binary_pred(*first1, *first2) )
   {
      ++first1;
      ++first2;
   }

   return std::pair< InputIterator1, InputIterator2 >( first1, first2 );
}

}} //namespace SS::Lib 

#endif