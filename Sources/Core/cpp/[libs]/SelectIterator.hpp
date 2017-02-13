#ifndef SelectIterator_HPP
#define SelectIterator_HPP

#include "StlAuxiliary.h"

namespace SS { namespace Lib {

///////////////////////////////////////////////////////////////////////////////
// Позволяет адаптировать итератор некоторого контейнера (или обычный указатель) 
// таким образом чтобы он возвращал объекты другого типа (при этом новый 
// итератор остаётся полнофункциональным итератором)
// Переопределяет у итератора  методы operator* и operator->
//
// IteratorT - адаптируемый тип итератора (необходимо отдельно определять для iterator
//    и для const_iterator
// SelectorT - Стратегия которая опредиляет три типа value_type, reference и pointer 
//    (для удобства можно использовать SelectorHelper). А также функцию, принимающую тип 
//       reference operator ()( typename std::iterator_traits<IteratorT>::reference ) const 
//    которая проделывает необходимое преобразование в тип объекта который должен быть 
//    получен.
//    При присваивании (копирующем конструировании) объектов CSelectIterator с разными 
//    типами стратегий SelectorT стратегия копируется только в том случае если определено (явно или нет) 
//    преобразование между этими типами. Если преобразование не определено, то стратегия конструируется из 
//    конструктора по-умолчанию.
//
// Например
/*
      typedef std::map<int, double> TStorage;

      template<class Iterator, class Value>
      struct Selector: SelectorHelper<Iterator, Value> 
      {
         reference operator ()( origin_iterator_reference V ) const { return V.second; }
      };

      typedef CSelectIterator<typename TStorage::iterator, Selector<typename TStorage::iterator, double> > TResultIterator;
      typedef CSelectIterator<typename TStorage::const_iterator, Selector<typename TStorage::const_iterator, const double> > TConstResultIterator;
*/
///////////////////////////////////////////////////////////////////////////////
// Можно воспользоваться такой реализацией, но она не способна работать с итераторами представленными указателями,
// а в STLport это стандартный тип итератора для vector.
//   template< class Iterator, class Selector>
//   class CSelectIterator: public Iterator
//   {
//   public:
//      typedef typename Selector::reference reference;
//      typedef typename Selector::pointer pointer;
//      typedef typename Selector::value_type value_type;
//
//   private:
//      Selector m_Selector;
//
//   public:
//      CSelectIterator() {}
//      CSelectIterator( const Selector &S ): m_Selector(S) {}
//
//      template< class T >
//         CSelectIterator( const T &obj ): Iterator(obj) {}
//
//         reference operator*() const
//         {	
//            return m_Selector(*static_cast<const Iterator &>(*this));
//         }
//
//         pointer operator->() const
//         {	
//            return &m_Selector(*static_cast<const Iterator &>(*this));
//         }
//   };
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Определяет несколько typedef упрощающих написание стратегии для класса
// CSelectIterator
///////////////////////////////////////////////////////////////////////////////
template< class IteratorT, class ValueT, class ReferenceT = ValueT &, class PointerT = ValueT * >
struct SelectorHelper
{
   typedef typename UnConst<ValueT>::Result value_type;
   typedef ReferenceT reference;
   typedef PointerT pointer;

protected:
   //Объявим так же для удобства
   typedef typename std::iterator_traits<IteratorT>::reference origin_iterator_reference; 

   //Данный тип уже не используется, оставлен для совместимости с уже написанным кодом
   typedef typename std::iterator_traits<IteratorT>::value_type origin_value_type; 
};

//Реализация основана на reverse_iterator из STLport, хотя с большими изменениями
template< class IteratorT, class SelectorT>
class CSelectIterator: 
   private SelectorT //для проведения оптимизации "пустого базового класса"
{
public:
  typedef typename SelectorT::value_type value_type;
  typedef typename SelectorT::pointer pointer;
  typedef typename SelectorT::reference reference;
  typedef typename std::iterator_traits<IteratorT>::difference_type difference_type;
  typedef typename std::iterator_traits<IteratorT>::iterator_category  iterator_category;
  
  typedef IteratorT iterator_type;

private:
  IteratorT Iterator_;

private:
  typedef CSelectIterator< IteratorT, SelectorT > SelfT;

  template< class OtherIterator, class OtherSelector >
  friend class CSelectIterator;

private:  
   struct OperatorHelper{ int i; }; 
   typedef int OperatorHelper::*TUndefinedBoolType;

public:
   CSelectIterator() {}
   CSelectIterator( TUndefinedBoolType pT ): Iterator_(0) {} //Позволяет конструировать только из 0
   CSelectIterator( const CSelectIterator &Other ): Iterator_(Other.Iterator_) {}

   template< class OtherIterator >
   explicit CSelectIterator( const OtherIterator &Iterator, const SelectorT &Selector = SelectorT() ): Iterator_(Iterator), SelectorT(Selector)  {}

   template< class OtherIterator, class OtherSelector >
   CSelectIterator( const OtherIterator &Iterator, const OtherSelector &Selector ): Iterator_(Iterator), SelectorT(Selector)  {}

   template< class OtherIterator, class OtherSelector >
   CSelectIterator( const CSelectIterator<OtherIterator, OtherSelector> &Other ) : Iterator_(Other.Iterator_), SelectorT(TryConvertOrCreate<OtherSelector, SelectorT>::Get(static_cast<const OtherSelector &>(Other)))  {}

   template< class OtherIterator, class OtherSelector >
   SelfT &operator=( const CSelectIterator<OtherIterator, OtherSelector> &Other )
   {
      Iterator_ = Other.Iterator_;     
      static_cast<SelectorT &>(*this) = TryConvertOrCreate<OtherSelector, SelectorT>::Get(static_cast<const OtherSelector &>(Other));
      
      return *this;
   }

   reference operator*() const 
   {
      return static_cast<const SelectorT &>(*this)(*Iterator_);
   }

   pointer operator->() const
   {	
      return &**this;
   }

   SelfT& operator++() 
   {
      ++Iterator_;
      return *this;
   }

   SelfT operator++(int) 
   {
      SelfT Tmp( *this );
      ++Iterator_;
      return Tmp;
   }

   SelfT& operator--() 
   {
      --Iterator_;
      return *this;
   }

   SelfT operator--(int) 
   {
      SelfT Tmp( *this );
      --Iterator_;
      return Tmp;
   }

   const SelfT operator+(difference_type n) const 
   {
      return SelfT(Iterator_ + n);
   }

   SelfT& operator+=(difference_type n) 
   {
      Iterator_ += n;
      return *this;
   }

   const SelfT operator-(difference_type n) const 
   {
      return SelfT(Iterator_ - n);
   }

   SelfT& operator-=(difference_type n) 
   {
      Iterator_ -= n;
      return *this;
   }

   reference operator[](difference_type n) const { return *(*this + n); } 

   IteratorT &GetOriginIterator() { return Iterator_; }
   const IteratorT &GetOriginIterator() const { return Iterator_; }

public:
   //Позволет проверять указатель на равенство/неравенство нулю и приводить к типу bool
   operator TUndefinedBoolType() const{ return Iterator_ != 0? &OperatorHelper::i : 0; }
   
   //Если не определять то будет использоваться автоматическое преобразование к TUndefinedBoolType
   //и встроенные операторы
   friend bool operator==( const CSelectIterator &P1, const CSelectIterator &P2 ){ return P1.Iterator_ == P2.Iterator_; }
   friend bool operator!=( const CSelectIterator &P1, const CSelectIterator &P2 ){ return P1.Iterator_ != P2.Iterator_; }

   //Если не опредилять по при проверке P1 == 0, 0 == P1, P1 != 0, 0 != P1 возникнет 
   //неоднозначность если конструктор будет не explicit
   friend bool operator==( const CSelectIterator &P1, TUndefinedBoolType P2 ){ return P1.Iterator_ == 0; }
   friend bool operator==( TUndefinedBoolType P2, const CSelectIterator &P1 ){ return P1.Iterator_ == 0; }
   friend bool operator!=( const CSelectIterator &P1, TUndefinedBoolType P2 ){ return P1.Iterator_ != 0; }
   friend bool operator!=( TUndefinedBoolType P2, const CSelectIterator &P1 ){ return P1.Iterator_ != 0; }
}; 
 
template<class IteratorT1, class SelectorT1, class IteratorT2, class SelectorT2>
inline bool operator==(const CSelectIterator<IteratorT1, SelectorT1>& X, 
                       const CSelectIterator<IteratorT2, SelectorT2>& Y) 
{
  return X.GetOriginIterator() == Y.GetOriginIterator();
}

template<class IteratorT1, class SelectorT1, class IteratorT2, class SelectorT2>
inline bool operator<(const CSelectIterator<IteratorT1, SelectorT1>& X, 
                      const CSelectIterator<IteratorT2, SelectorT2>& Y) 
{
   return X.GetOriginIterator() < Y.GetOriginIterator();
}

template<class IteratorT1, class SelectorT1, class IteratorT2, class SelectorT2>
inline bool operator!=(const CSelectIterator<IteratorT1, SelectorT1>& X, 
                       const CSelectIterator<IteratorT2, SelectorT2>& Y) 
{
   return !(X == Y);
}

template<class IteratorT1, class SelectorT1, class IteratorT2, class SelectorT2>
inline bool operator>(const CSelectIterator<IteratorT1, SelectorT1>& X, 
                       const CSelectIterator<IteratorT2, SelectorT2>& Y) 
{
   return Y < X;
}

template<class IteratorT1, class SelectorT1, class IteratorT2, class SelectorT2>
inline bool operator<=(const CSelectIterator<IteratorT1, SelectorT1>& X, 
                      const CSelectIterator<IteratorT2, SelectorT2>& Y) 
{
   return !(Y < X);
}

template<class IteratorT1, class SelectorT1, class IteratorT2, class SelectorT2>
inline bool operator>=(const CSelectIterator<IteratorT1, SelectorT1>& X, 
                       const CSelectIterator<IteratorT2, SelectorT2>& Y) 
{
   return !(X < Y);
}

template<class IteratorT1, class SelectorT1, class IteratorT2, class SelectorT2>
inline typename CSelectIterator<IteratorT1, SelectorT1>::difference_type
operator-(const CSelectIterator<IteratorT1, SelectorT1>& X, 
          const CSelectIterator<IteratorT2, SelectorT2>& Y) 
{
   return X.GetOriginIterator() - Y.GetOriginIterator();
}

template<class IteratorT, class SelectorT>
inline const CSelectIterator<IteratorT, SelectorT>
operator+(typename CSelectIterator<IteratorT, SelectorT>::difference_type n, 
          const CSelectIterator<IteratorT, SelectorT>& X) 
{
   return X.operator+(n);
}

}} //namespace SS::Lib 

#endif