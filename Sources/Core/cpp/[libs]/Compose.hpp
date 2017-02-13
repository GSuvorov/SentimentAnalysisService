#ifndef Compose_HPP
#define Compose_HPP

#include <iterator>
#include <functional>

namespace SS { namespace Lib {

/*
   Адаптеры для объектов-функций - функторов
   Композиция функторов подробнее см. http://www.sgi.com/tech/stl/binary_compose.html
   Добавлены объекты и изменены имена:

   unary_compose_1arg( f1, f2 )             : f(x)    -> f1( f2(x) )
   unary_compose_2arg( f1, f2, f3 )         : f(x)    -> f1( f2(x), f3(x) )
   binary_compose_1arg( f1, f2, f3 )        : f(x, y) -> f1( f2(x), f3(y) )
   binary_compose_2arg( f1, f2, f3 )        : f(x, y) -> f1( f2(x, y), f3(x, y) )

   select_first_t<_Pair>                    : Выбор first из std::pair
   select_second_t<_Pair>                   : Выбор second из std::pair
   iterator_deref_t<Iterator>               : Разыменование указателя или итератора. Тип после разыменования опредиляется при помощи iterator_traits
   pointer_deref_t<Pointer, T>              : Разыменование указателя или итератора. Тип после разыменования T
   select_dummy_t<T>                        : Пустышка которая возвращает то что ей передают
*/

template <class _Operation1, class _Operation2>
class unary_compose_1arg_t: 
   public std::unary_function<typename _Operation2::argument_type,
   typename _Operation1::result_type > {
protected:
   _Operation1 _M_fn1;
   _Operation2 _M_fn2;
public:
   unary_compose_1arg_t(const _Operation1& __x = _Operation1(), const _Operation2& __y = _Operation2()) 
      : _M_fn1(__x), _M_fn2(__y) {}

      typename _Operation1::result_type
         operator()(const typename _Operation2::argument_type& __x) const {
            return _M_fn1(_M_fn2(__x));
         }

         typename _Operation1::result_type
            operator()(typename _Operation2::argument_type& __x) const {
               return _M_fn1(_M_fn2(__x));
            }
   };

template <class _Operation1, class _Operation2>
inline unary_compose_1arg_t<_Operation1,_Operation2> 
unary_compose_1arg(const _Operation1& __fn1, const _Operation2& __fn2)
{
   return unary_compose_1arg_t<_Operation1,_Operation2>(__fn1, __fn2);
}

template <class _Operation1, class _Operation2, class _Operation3>
class unary_compose_2arg_t : 
   public std::unary_function<typename _Operation2::argument_type,
   typename _Operation1::result_type> {
protected:
   _Operation1 _M_fn1;
   _Operation2 _M_fn2;
   _Operation3 _M_fn3;
public:
   unary_compose_2arg_t(const _Operation1& __x = _Operation1(), const _Operation2& __y = _Operation2(), 
      const _Operation3& __z = _Operation3() ) 
      : _M_fn1(__x), _M_fn2(__y), _M_fn3(__z) { }

      typename _Operation1::result_type
         operator()(const typename _Operation2::argument_type& __x) const {
            return _M_fn1(_M_fn2(__x), _M_fn3(__x));
         }

      typename _Operation1::result_type
         operator()(typename _Operation2::argument_type& __x) const {
            return _M_fn1(_M_fn2(__x), _M_fn3(__x));
         }

   };

template <class _Operation1, class _Operation2, class _Operation3>
inline unary_compose_2arg_t<_Operation1, _Operation2, _Operation3> 
unary_compose_2arg(const _Operation1& __fn1, const _Operation2& __fn2, 
         const _Operation3& __fn3)
{
   return unary_compose_2arg_t<_Operation1,_Operation2,_Operation3>
      (__fn1, __fn2, __fn3);
}

template <class _Operation1, class _Operation2, class _Operation3>
class binary_compose_1arg_t: 
   public std::binary_function<typename _Operation2::argument_type, typename _Operation3::argument_type,
   typename _Operation1::result_type> {
protected:
   _Operation1 _M_fn1;
   _Operation2 _M_fn2;
   _Operation3 _M_fn3;
public:
   binary_compose_1arg_t(const _Operation1& __x = _Operation1(), const _Operation2& __y = _Operation2(), 
      const _Operation3& __z = _Operation3()) 
      : _M_fn1(__x), _M_fn2(__y), _M_fn3(__z) { }


      typename _Operation1::result_type
         operator()(const typename _Operation2::argument_type& __x, const typename _Operation3::argument_type& __y) const {
            return _M_fn1(_M_fn2(__x), _M_fn3(__y));
         }

      typename _Operation1::result_type
         operator()(typename _Operation2::argument_type& __x, typename _Operation3::argument_type& __y) const {
            return _M_fn1(_M_fn2(__x), _M_fn3(__y));
         }

   };

template <class _Operation1, class _Operation2, class _Operation3>
inline binary_compose_1arg_t<_Operation1, _Operation2, _Operation3> 
binary_compose_1arg(const _Operation1& __fn1, const _Operation2& __fn2, 
         const _Operation3& __fn3)
{
   return binary_compose_1arg_t<_Operation1,_Operation2,_Operation3>
      (__fn1, __fn2, __fn3);
}

template <class _Operation1, class _Operation2, class _Operation3>
class binary_compose_2arg_t: 
   public std::binary_function<typename _Operation2::first_argument_type, typename _Operation2::second_argument_type,
   typename _Operation1::result_type> {
protected:
   _Operation1 _M_fn1;
   _Operation2 _M_fn2;
   _Operation3 _M_fn3;
public:
   binary_compose_2arg_t(const _Operation1& __x = _Operation1(), const _Operation2& __y = _Operation2(), 
      const _Operation3& __z = _Operation3()) 
      : _M_fn1(__x), _M_fn2(__y), _M_fn3(__z) { }


      typename _Operation1::result_type
         operator()(const typename _Operation2::first_argument_type& __x, const typename _Operation2::second_argument_type& __y) const {
            return _M_fn1(_M_fn2(__x, __y), _M_fn3(__x, __y));
         }

      typename _Operation1::result_type
         operator()( typename _Operation2::first_argument_type& __x, typename _Operation2::second_argument_type& __y) const {
            return _M_fn1(_M_fn2(__x, __y), _M_fn3(__x, __y));
         }

   };

template <class _Operation1, class _Operation2, class _Operation3>
inline binary_compose_2arg_t<_Operation1, _Operation2, _Operation3> 
binary_compose_2arg(const _Operation1& __fn1, const _Operation2& __fn2, 
                    const _Operation3& __fn3)
{
   return binary_compose_2arg_t<_Operation1,_Operation2,_Operation3>
      (__fn1, __fn2, __fn3);
}

///////////////////////////////////////////////////////////////////////////////
// Выбор элементов из пары std::pair из STLPORT, немного модернизированный
///////////////////////////////////////////////////////////////////////////////
template <class _Pair>
struct select_first_t : public std::unary_function<_Pair, typename _Pair::first_type> 
{
   const typename _Pair::first_type& operator()(const _Pair& __x) const {
      return __x.first;
   }

   typename _Pair::first_type& operator()( _Pair& __x) const {
      return __x.first;
   }
};

template <class _Pair>
struct select_second_t : public std::unary_function<_Pair, typename _Pair::second_type>
{
   const typename _Pair::second_type& operator()(const _Pair& __x) const {
      return __x.second;
   }

   typename _Pair::second_type& operator()( _Pair& __x) const {
      return __x.second;
   }
}; 

///////////////////////////////////////////////////////////////////////////////
// Разыменование итератора
///////////////////////////////////////////////////////////////////////////////
template< class Iterator >
struct iterator_deref_t: public std::unary_function< Iterator, typename std::iterator_traits<Iterator>::value_type >
{
   typename std::iterator_traits<Iterator>::reference operator()( const Iterator &arg ) const { return *arg; }  
};

///////////////////////////////////////////////////////////////////////////////
// Разыменование указателя или итератора. Тип после разыменования 
// опредиляется при помощи iterator_traits
///////////////////////////////////////////////////////////////////////////////

template< class Pointer, class T >
struct pointer_deref_t: public std::unary_function< Pointer, T >
{
   T &operator()( const Pointer &arg ) const { return *arg; }  
};

///////////////////////////////////////////////////////////////////////////////
// Разыменование указателя или итератора. Тип после разыменования T 
///////////////////////////////////////////////////////////////////////////////
template< class T >
struct select_dummy_t: public std::unary_function< T, T >
{
   const T &operator()( const T &arg ) const { return arg; }  
   T &operator()( T &arg ) const { return arg; }  
};

}} //namespace SS::Lib 

#endif

