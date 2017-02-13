#ifndef LikePointer_HPP
#define LikePointer_HPP

#include "SelectIterator.hpp"
#include "ReferenceCounters.hpp"

#include "AssocVector.h"
#include "StlAuxiliary.h"

namespace SS { namespace Lib {

/*
   Модуль содержит ряд классов поведение которых напоминает поведение указателей
   (перегружены опрераторы * и ->), в том числе и итераторы.
   А так же различные классы реализующие парадигму владения объектом. 
*/

///////////////////////////////////////////////////////////////////////////////
// Расширенная реализация auto_ptr, тем что позволяет вместо оператора delete 
// использовать произвольную функцию, определяемую стратегией 
// реализация основана на:
// The C++ Standard Library - A Tutorial and Reference
// by Nicolai M. Josuttis, Addison-Wesley, 1999
// Наследуем стратегию для того чтобы позволить провести компилятору "оптимизацию
// пустого базового класса"
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Стратегии удаления. Требования:
//    Стратегии должны корректно обрабатывать попытку удаления нулевого указателя
//    Стратегии должны конструироваться и присваиваться стратегиям с другим шаблонным типом
///////////////////////////////////////////////////////////////////////////////

//Вспомогательный класс, базовый для всех стратегий
template<class ValueT, class PointerT = ValueT *, class ReferenceT = ValueT &>
struct auto_ptr_ex_strategy_base
{
   typedef ValueT value_type;
   typedef PointerT pointer;
   typedef ReferenceT reference;
};

//Стратегия удаления при помощи оператора delete
template< class T >
struct auto_ptr_ex_delete_strategy: public auto_ptr_ex_strategy_base<T> 
{
   auto_ptr_ex_delete_strategy() {}
   
   template< class Y >
   auto_ptr_ex_delete_strategy( const auto_ptr_ex_delete_strategy<Y> & ) {}
   
   template< class Y >
   auto_ptr_ex_delete_strategy &operator=( const auto_ptr_ex_delete_strategy<Y> & ) { return *this; }

   void operator()( pointer pT ) const { delete pT; }
};
///////////////////////////////////////////////////////////////////////////////

//Стратегия удаления при помощи метода Release
template< class T >
struct auto_ptr_ex_release_strategy: public auto_ptr_ex_strategy_base<T>
{
   auto_ptr_ex_release_strategy() {}
   
   template< class Y >
   auto_ptr_ex_release_strategy( const auto_ptr_ex_release_strategy<Y> & ) {}

   template< class Y >
   auto_ptr_ex_release_strategy &operator=( const auto_ptr_ex_release_strategy<Y> & ) { return *this; }

   void operator()( pointer pT ) const { if(pT != 0) pT->Release(); }
};
///////////////////////////////////////////////////////////////////////////////

//Пустая стратегия, удаления не происходит
template< class T >
struct auto_ptr_ex_empty_strategy: public auto_ptr_ex_strategy_base<T> 
{
   auto_ptr_ex_empty_strategy() {}
   
   template< class Y >
   auto_ptr_ex_empty_strategy( const auto_ptr_ex_empty_strategy<Y> & ) {}

   template< class Y >
   auto_ptr_ex_empty_strategy &operator=( const auto_ptr_ex_empty_strategy<Y> & ) { return *this; }

   void operator()( pointer pT ) const { /*ничего не делаем*/}
};


// auxiliary type to enable copies and assignments (now global)
template<class Y, template <class> class DeleteStrategy>
struct auto_ptr_ref_ex: private DeleteStrategy<Y>
{
   typedef DeleteStrategy<Y> TDeleteStrategy;
   typedef typename DeleteStrategy<Y>::pointer pointer;

   template<class OtherT, template <class> class OtherDeleteStrategy > friend class auto_ptr_ex;

   pointer yp;

   auto_ptr_ref_ex (pointer rhs, const TDeleteStrategy &DS)
   : yp(rhs), TDeleteStrategy(DS) {
   }
};

template<class T, template <class> class DeleteStrategy = auto_ptr_ex_delete_strategy >
class auto_ptr_ex: private DeleteStrategy<T> {
public:
   typedef DeleteStrategy<T> TDeleteStrategy;
   typedef typename DeleteStrategy<T>::pointer pointer;
   typedef typename DeleteStrategy<T>::reference reference;
   typedef typename DeleteStrategy<T>::value_type value_type;
   typedef value_type element_type; //Синоним value_type, для совместимости
  
private:
   pointer ap;    // refers to the actual owned object (if any)

   template<class OtherT, template <class> class OtherDeleteStrategy > friend class auto_ptr_ex;

private:  
   struct OperatorHelper{ int i; }; 
   typedef int OperatorHelper::*TUndefinedBoolType;

public:

   // constructor
   
   //Нельзя писать конструктор который принимает TDeleteStrategy, т.к. это позволит 
   //конструировать auto_ptr_ex из константного auto_ptr_ex минуя auto_ptr_ref_ex
   //Если наследовать DeleteStrategy закрыто то ситуация не изменится
   //explicit auto_ptr_ex( const TDeleteStrategy &DS ) throw()
   //   : ap(0), TDeleteStrategy(DS) {}

   explicit auto_ptr_ex (pointer ptr = 0, const TDeleteStrategy &DS = TDeleteStrategy()) throw()
      : ap(ptr), TDeleteStrategy(DS) {}

   // copy constructors (with implicit conversion)
   // - note: nonconstant parameter
   auto_ptr_ex (auto_ptr_ex& rhs) throw()
   : ap(rhs.release()), TDeleteStrategy(rhs) {
   }
   template<class Y>
   auto_ptr_ex (auto_ptr_ex<Y, DeleteStrategy>& rhs) throw()
   : ap(rhs.release()), TDeleteStrategy(rhs) {
   }
   
   //Конструируем из std::auto_ptr
   template<class Y>
   auto_ptr_ex (std::auto_ptr<Y>& rhs) throw()
   : ap(rhs.release()), TDeleteStrategy(auto_ptr_ex_delete_strategy<T>()) {
   }

   // assignments (with implicit conversion)
   // - note: nonconstant parameter
   auto_ptr_ex& operator= (auto_ptr_ex& rhs) throw() {
      reset(rhs.release());
      static_cast<TDeleteStrategy&>(*this) = rhs;
      return *this;
   }
   
   template<class Y>
   auto_ptr_ex& operator= (auto_ptr_ex<Y, DeleteStrategy>& rhs) throw() {
      reset(rhs.release());
      static_cast<TDeleteStrategy&>(*this) = rhs;
      return *this;
   }

   //Присваиваем std::auto_ptr
   template<class Y>
   auto_ptr_ex& operator= (std::auto_ptr<Y>& rhs) throw() {
      reset(rhs.release());
      static_cast<TDeleteStrategy&>(*this) = auto_ptr_ex_delete_strategy<T>();
      return *this;
   }
   
   // destructor
   ~auto_ptr_ex() throw() {
      static_cast<TDeleteStrategy&>(*this)( ap );
   }

   // value access
   pointer get() const throw() {
      return ap;
   }
   reference operator*() const throw() {
      APL_ASSERT_PTR(ap);
      return *ap;
   }
   pointer operator->() const throw() {
      APL_ASSERT_PTR(ap);
      return ap;
   }

   // release ownership
   pointer release() throw() {
      pointer tmp(ap);
      ap = 0;
      return tmp;
   }

   // reset value
   void reset ( pointer ptr = 0 ) throw() {
      if (ap != ptr) {
         static_cast<TDeleteStrategy&>(*this)( ap );
         ap = ptr;
      }
   }

   /* special conversions with auxiliary type to enable copies and assignments
   */
   auto_ptr_ex(auto_ptr_ref_ex<T, DeleteStrategy> rhs) throw()
   : ap(rhs.yp), TDeleteStrategy(rhs) {
   }
   auto_ptr_ex& operator= (auto_ptr_ref_ex<T, DeleteStrategy> rhs) throw() {  // new
         reset(rhs.yp);
         static_cast<TDeleteStrategy&>(*this) = rhs;
         return *this;
   }

   template<class Y>
   operator auto_ptr_ref_ex<Y, DeleteStrategy>() throw() {
      return auto_ptr_ref_ex<Y, DeleteStrategy>(release(), static_cast<TDeleteStrategy&>(*this));
   }
   template<class Y>
   operator auto_ptr_ex<Y, DeleteStrategy>() throw() {
      return auto_ptr_ex<Y, DeleteStrategy>(release(), static_cast<TDeleteStrategy&>(*this));
   }

   void SetDeleteStrategy(const TDeleteStrategy &DS) {
      static_cast<TDeleteStrategy&>(*this) = DS;
   }

   const TDeleteStrategy &GetDeleteStrategy() const {
      return static_cast<const TDeleteStrategy&>(*this);
   }

public:
   //Позволет проверять указатель на равенство/неравенство нулю и приводить к типу bool
   operator TUndefinedBoolType() const{ return ap != 0? &OperatorHelper::i : 0; }
   
   //Нельзя определить auto_ptr_ex( TUndefinedBoolType ), т.к. возникает неоднозначность 
   //с auto_ptr_ex(auto_ptr_ref_ex<T, DeleteStrategy> rhs)

   //Если не определять то будет использоваться автоматическое преобразование к TUndefinedBoolType
   //и встроенные операторы
   friend bool operator==( const auto_ptr_ex &P1, const auto_ptr_ex &P2 ){ return P1.ap == P2.ap; }
   friend bool operator!=( const auto_ptr_ex &P1, const auto_ptr_ex &P2 ){ return P1.ap != P2.ap; }

   //Если не опредилять по при проверке P1 == 0, 0 == P1, P1 != 0, 0 != P1 возникнет 
   //неоднозначность если конструктор будет не explicit
   friend bool operator==( const auto_ptr_ex &P1, TUndefinedBoolType P2 ){ return P1.ap == 0; }
   friend bool operator==( TUndefinedBoolType P2, const auto_ptr_ex &P1 ){ return P1.ap == 0; }
   friend bool operator!=( const auto_ptr_ex &P1, TUndefinedBoolType P2 ){ return P1.ap != 0; }
   friend bool operator!=( TUndefinedBoolType P2, const auto_ptr_ex &P1 ){ return P1.ap != 0; }
};

///////////////////////////////////////////////////////////////////////////////
// Интеллектуальный указатель обладающей семантикой клонирования. Т.е. при 
// присвоении и конструировании создаётся дубликат объетка и указатель на него
// переходит во владение класса
// Благодаря этому такие указатели можно хранить в контейнерах STL
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Стратегии клонирования. Требования:
//    Стратегии должны корректно обрабатывать попытку клонирования нулевого указателя (возвращать 0)
//    Стратегии должны конструироваться и присваиваться стратегиям с другим шаблонным типом
///////////////////////////////////////////////////////////////////////////////

//Стратегия конирования при помощи метода Clone, который возвращает укаатель на клонируемый объект
template< class T >
struct clone_ptr_ex_strategy_clone_member: public auto_ptr_ex_strategy_base<T>
{
   clone_ptr_ex_strategy_clone_member() {}

   template< class Y >
      clone_ptr_ex_strategy_clone_member( const clone_ptr_ex_strategy_clone_member<Y> & ) {}

   template< class Y >
      clone_ptr_ex_strategy_clone_member &operator=( const clone_ptr_ex_strategy_clone_member<Y> & ) { return *this; }

   pointer operator()( pointer pT ) const { return pT ? pT->Clone() : static_cast<pointer>(0); }
};
///////////////////////////////////////////////////////////////////////////////

//Стратегия конирования при помощи метода Clone, который возвращает auto_ptr
template< class T >
struct clone_ptr_ex_strategy_clone_member_auto_ptr: public auto_ptr_ex_strategy_base<T> 
{
   clone_ptr_ex_strategy_clone_member_auto_ptr() {}

   template< class Y >
      clone_ptr_ex_strategy_clone_member_auto_ptr( const clone_ptr_ex_strategy_clone_member<Y> & ) {}

   template< class Y >
      clone_ptr_ex_strategy_clone_member_auto_ptr &operator=( const clone_ptr_ex_strategy_clone_member<Y> & ) { return *this; }

   pointer operator()( pointer pT ) const { return pT ? pT->Clone().release() : static_cast<pointer>(0); }
};
///////////////////////////////////////////////////////////////////////////////


template<
   class T, 
   template <class> class CloneStrategy = clone_ptr_ex_strategy_clone_member, 
   template <class> class DeleteStrategy = auto_ptr_ex_delete_strategy
>
class clone_ptr_ex: private CloneStrategy<T>, private DeleteStrategy<T> {
public:
   typedef DeleteStrategy<T> TDeleteStrategy;
   typedef CloneStrategy<T> TCloneStrategy;
   typedef typename DeleteStrategy<T>::pointer pointer;
   typedef typename DeleteStrategy<T>::reference reference;
   typedef typename DeleteStrategy<T>::value_type value_type;
   typedef value_type element_type; //Синоним value_type, для совместимости

private:
   pointer ap;    // refers to the actual owned object (if any)

   template<class OtherT, template <class> class OtherCloneStrategy, template <class> class OtherDeleteStrategy > friend class clone_ptr_ex;

private:  
   struct OperatorHelper{ int i; }; 
   typedef int OperatorHelper::*TUndefinedBoolType;

public:
   // constructor
   explicit clone_ptr_ex ( pointer ptr = 0, const TCloneStrategy &CS = TCloneStrategy(), const TDeleteStrategy &DS = TDeleteStrategy() ) throw()
      : ap(ptr), TCloneStrategy(CS), TDeleteStrategy(DS) {}

   // copy constructors (with implicit conversion)
   clone_ptr_ex (const clone_ptr_ex& rhs)
   : ap(TCloneStrategy::operator()(rhs.ap)), TCloneStrategy(rhs), TDeleteStrategy(rhs) {
   }
   template<class Y>
   clone_ptr_ex (const clone_ptr_ex<Y, CloneStrategy, DeleteStrategy>& rhs)
   : ap(TCloneStrategy::operator()(rhs.ap)), TCloneStrategy(rhs), TDeleteStrategy(rhs) {
   }
   
   //Конструируем из std::auto_ptr
   template<class Y>
   clone_ptr_ex (const std::auto_ptr<Y>& rhs) throw()
   : ap(TCloneStrategy::operator()(rhs.get())), TDeleteStrategy(auto_ptr_ex_delete_strategy<T>()) {
   }

   //Конструируем из std::auto_ptr_ex
   template<class Y>
   clone_ptr_ex (const auto_ptr_ex<Y, DeleteStrategy>& rhs) throw()
   : ap(TCloneStrategy::operator()(rhs.get())), TDeleteStrategy(rhs.GetDeleteStrategy()) {
   }

   // assignments (with implicit conversion)
   clone_ptr_ex& operator=(const clone_ptr_ex& rhs) 
   {   
      reset(static_cast<TCloneStrategy&>(*this)(rhs.ap));
      static_cast<TCloneStrategy&>(*this) = rhs;
      static_cast<TDeleteStrategy&>(*this) = rhs;
      return *this;
   }
   
   template<class Y>
   clone_ptr_ex& operator=( const clone_ptr_ex<Y, CloneStrategy, DeleteStrategy>& rhs)
   {
      reset(static_cast<TCloneStrategy&>(*this)(rhs.ap));
      static_cast<TCloneStrategy&>(*this) = rhs;
      static_cast<TDeleteStrategy&>(*this) = rhs;
      return *this;
   }

   //Присваиваем std::auto_ptr
   template<class Y>
   clone_ptr_ex& operator= (const std::auto_ptr<Y>& rhs) throw() {
      reset(static_cast<TCloneStrategy&>(*this)(rhs.get()));
      static_cast<TDeleteStrategy&>(*this) = auto_ptr_ex_delete_strategy<T>();
      return *this;
   }
   
   //Присваиваем std::auto_ptr_ex
   template<class Y>
   clone_ptr_ex& operator= (const auto_ptr_ex<Y, DeleteStrategy>& rhs) throw() {
      reset(static_cast<TCloneStrategy&>(*this)(rhs.get()));
      static_cast<TDeleteStrategy&>(*this) = rhs.GetDeleteStrategy();
      return *this;
   }

  
   // destructor
   ~clone_ptr_ex() throw() {
      static_cast<TDeleteStrategy&>(*this)( ap );
   }

   // value access
   pointer get() const throw() {
      return ap;
   }
   reference operator*() const throw() {
      APL_ASSERT_PTR(ap);
      return *ap;
   }
   pointer operator->() const throw() {
      APL_ASSERT_PTR(ap);
      return ap;
   }

   // release ownership
   pointer release() throw() {
      pointer tmp(ap);
      ap = 0;
      return tmp;
   }

   // reset value
   void reset (pointer ptr=0) throw() {
      if (ap != ptr) 
      {
         static_cast<TDeleteStrategy&>(*this)( ap );
         ap = ptr;
      }
   }

   void SetDeleteStrategy(const TDeleteStrategy &DS) {
      static_cast<TDeleteStrategy&>(*this) = DS;
   }

   void SetCloneStrategy(const TCloneStrategy &DS) {
      static_cast<TCloneStrategy&>(*this) = DS;
   }

   const TDeleteStrategy &GetDeleteStrategy() const {
      return static_cast<const TDeleteStrategy&>(*this);
   }
   
   const TCloneStrategy &GetCloneStrategy() const {
      return static_cast<const TCloneStrategy&>(*this);
   }

public:
   //Позволет проверять указатель на равенство/неравенство нулю и приводить к типу bool
   operator TUndefinedBoolType() const{ return ap != 0? &OperatorHelper::i : 0; }

   //Если не определять то будет использоваться автоматическое преобразование к TUndefinedBoolType
   //и встроенные операторы
   friend bool operator==( const clone_ptr_ex &P1, const clone_ptr_ex &P2 ){ return P1.ap == P2.ap; }
   friend bool operator!=( const clone_ptr_ex &P1, const clone_ptr_ex &P2 ){ return P1.ap != P2.ap; }

   //Если не опредилять по при проверке P1 == 0, 0 == P1, P1 != 0, 0 != P1 возникнет 
   //неоднозначность если конструктор будет не explicit
   friend bool operator==( const clone_ptr_ex &P1, TUndefinedBoolType P2 ){ return P1.ap == 0; }
   friend bool operator==( TUndefinedBoolType P2, const clone_ptr_ex &P1 ){ return P1.ap == 0; }
   friend bool operator!=( const clone_ptr_ex &P1, TUndefinedBoolType P2 ){ return P1.ap != 0; }
   friend bool operator!=( TUndefinedBoolType P2, const clone_ptr_ex &P1 ){ return P1.ap != 0; }
};

///////////////////////////////////////////////////////////////////////////////
// Класс связывает указатели на объёкты с некоторым количеством ключей.
// При этом реализуется стратегия владения, т.е. объекты по сохранённым 
// адресам уничтожаются в деструкторе при помощи DeleteStrategy.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Сводное описание:
//    CObjectsSelector -             С одним ключом связан один объект (один к одному)
//    CObjectsSelectorMultiKey  -    С несколькими ключами связан один объект, по одному ключу можно получить только один объект (много к одному)
//    CObjectsSelectorMultiObject -  С несколькими ключами связано несколько объектов, по одному ключу можно получить сразу несколько объектов (много ко многим)
//    CObjectsSelectorIndex       -  Связь объектов с последовательными индаксами в интервале [0; <Кол-во объектов>)
///////////////////////////////////////////////////////////////////////////////
template < class Key, class Object, template <class> class DeleteStrategy = auto_ptr_ex_delete_strategy >
class CObjectsSelector: public DeleteStrategy<Object>, public NonCopyable
{
public:  
   typedef DeleteStrategy<Object> TDeleteStrategy;
   typedef auto_ptr_ex<Object, DeleteStrategy> TAutoPtr;
   typedef auto_ptr_ex<Object, DeleteStrategy> &TAutoPtrRef; 
   typedef Key KeyType;
   
   typedef auto_ptr_ex<const Object, DeleteStrategy> TConstAutoPtr;
   typedef typename TAutoPtr::value_type ObjectType;
   typedef typename TAutoPtr::pointer ObjectPtrType;
   typedef typename TConstAutoPtr::pointer ObjectConstPtrType;
   typedef typename TAutoPtr::reference ObjectRefType;
   typedef typename TConstAutoPtr::reference ObjectConstRefType;

private:
   typedef AssocVector<Key, ObjectPtrType> TStorage;

   template< class Iterator, class Value >
   struct Selector: public SelectorHelper<Iterator, Value>
   {
      reference operator ()( origin_iterator_reference V ) const { return *V.second; }
   };

private:
  TStorage m_Storage;

public:  
   typedef CSelectIterator<typename TStorage::iterator, Selector<typename TStorage::iterator, Object> > TIterator;
   typedef CSelectIterator<typename TStorage::const_iterator, Selector<typename TStorage::const_iterator, const Object> > TConstIterator;

public:  
   CObjectsSelector() {}
   CObjectsSelector(const TDeleteStrategy &DS): TDeleteStrategy(DS) {}

   TIterator Begin() { return TIterator(m_Storage.begin()); }
   TConstIterator Begin() const { return TConstIterator(m_Storage.begin()); }
   TIterator End() { return TIterator(m_Storage.end()); }
   TConstIterator End() const { return TConstIterator(m_Storage.end()); }

   //Удалить все объекты которыми владеет класс
   void Clear()
   {
      for(TStorage::iterator I = m_Storage.begin(); I != m_Storage.end(); ++I )
         static_cast<TDeleteStrategy&>(*this)( I->second );
      
      m_Storage.clear();
   }
   
   bool Empty() const 
   {
      return m_Storage.empty();
   }

   ~CObjectsSelector()
   { 
      Clear();
   }

   //Вернуть указатель на объект по ключу. Если такого объекта нет то возвращается 0
   ObjectConstPtrType Get( const KeyType& K ) const
   {
      TStorage::const_iterator I = m_Storage.find( K );

      if( I != m_Storage.end() )
         return I->second;

      return 0;
   }

   ObjectPtrType Get( const KeyType& K )
   {
      TStorage::iterator I = m_Storage.find( K );

      if( I != m_Storage.end() )
         return I->second;

      return 0;
   }

   //Возвращает ссылку на хранящийся объект по индексу. Если объект по индексу не 
   //зарегистнрирован происходит разыменование нулевого указателя
   ObjectRefType operator[]( const KeyType& K ){ APL_ASSERT_PTR( Get(K) ); return *Get(K); }
   ObjectConstRefType operator[]( const KeyType& K ) const { APL_ASSERT_PTR( Get(K) ); return *Get(K); }

   //Добавить объект, при этом при этом происходит смена владельца. Если по ключу объект уже есть то возвращается
   //false и владение не захватывается иначе происходит смена владельца и возвращается true
   bool Add( const KeyType& K, TAutoPtrRef AutoPtrRef )
   {
      if( m_Storage.find(K) != m_Storage.end() )
         return false;

      m_Storage.insert( TStorage::value_type( K, AutoPtrRef.release() ) );

      return true;
   }
   
   void SetDeleteStrategy(const TDeleteStrategy &DS) {
      static_cast<TDeleteStrategy&>(*this) = DS;
   }

   const TDeleteStrategy &GetDeleteStrategy() const {
      return static_cast<const TDeleteStrategy&>(*this);
   }

};

///////////////////////////////////////////////////////////////////////////////
// Тоже что и CObjectsSelector, но позволяет связать несколько ключей с одним 
// указателем на объект
///////////////////////////////////////////////////////////////////////////////
template < class Key, class Object, template <class> class DeleteStrategy = auto_ptr_ex_delete_strategy >
class CObjectsSelectorMultiKey: public DeleteStrategy<Object>, public NonCopyable
{
public:  
   typedef DeleteStrategy<Object> TDeleteStrategy;

   typedef auto_ptr_ex<Object, DeleteStrategy> TAutoPtr;
   typedef auto_ptr_ex<Object, DeleteStrategy> &TAutoPtrRef;
   typedef Key KeyType;
   
   typedef auto_ptr_ex<const Object, DeleteStrategy> TConstAutoPtr;
   typedef typename TAutoPtr::value_type ObjectType;
   typedef typename TAutoPtr::pointer ObjectPtrType;
   typedef typename TConstAutoPtr::pointer ObjectConstPtrType;
   typedef typename TAutoPtr::reference ObjectRefType;
   typedef typename TConstAutoPtr::reference ObjectConstRefType;

private:
   typedef AssocVector<Key, ObjectPtrType> TStorage;
   typedef std::vector<ObjectPtrType> TStorageForDelete;
 
public:
   typedef typename TStorageForDelete::iterator TIterator;
   typedef typename TStorageForDelete::const_iterator TConstIterator;

private:
   TStorage m_Storage;
   TStorageForDelete m_StorageForDelete; //Объекты для удаления

public:  
   CObjectsSelectorMultiKey() {}
   CObjectsSelectorMultiKey(const TDeleteStrategy &DS): TDeleteStrategy(DS) {}
   
   //Итераторы возвращаются только для зарегистрированных объектов
   TIterator Begin() { return m_StorageForDelete.begin(); }
   TConstIterator Begin() const { return m_StorageForDelete.begin(); }
   TIterator End() { return m_StorageForDelete.end(); }
   TConstIterator End() const { return m_StorageForDelete.end(); }

   //Удалить все объекты которыми владеет класс
   void Clear()
   {
      for(TStorageForDelete::iterator I = m_StorageForDelete.begin(); I != m_StorageForDelete.end(); ++I )
         static_cast<TDeleteStrategy&>(*this)( *I );

      m_StorageForDelete.clear();
      m_Storage.clear();
   }

   bool Empty() const 
   {
      return m_Storage.empty();
   }

   ~CObjectsSelectorMultiKey()
   { 
      Clear();
   }

   //Вернуть указатель на объект по ключу. Если такого объекта нет то возвращается 0
   ObjectConstPtrType Get( const KeyType& K ) const
   {
      TStorage::const_iterator I = m_Storage.find( K );

      if( I != m_Storage.end() )
         return I->second;

      return 0;
   }

   ObjectPtrType Get( const KeyType& K )
   {
      TStorage::iterator I = m_Storage.find( K );

      if( I != m_Storage.end() )
         return I->second;

      return 0;
   }
   
   //Возвращает ссылку на хранящийся объект по индексу. Если объект по индексу не 
   //зарегистнрирован происходит разыменование нулевого указателя
   ObjectRefType operator[]( const KeyType& K ){ APL_ASSERT_PTR( Get(K) ); return *Get(K); }
   ObjectConstRefType operator[]( const KeyType& K ) const { APL_ASSERT_PTR( Get(K) ); return *Get(K); }
 
   //Добавить объект и ассоциировать его с групой ключей при этом при этом происходит 
   //смена владельца этого объекта. Если хотя бы по одному ключу объект уже есть возвращается
   //false и владение не захватывается иначе происходит смена владельца и возвращается true
   template< class InputIterator >
   bool Add( InputIterator FirstKey, InputIterator LastKey, TAutoPtrRef AutoPtrRef )
   {
      std::pair<TStorage::iterator, bool> ResPair;

      APL_ASSERT( std::find(m_StorageForDelete.begin(),  m_StorageForDelete.end(), AutoPtrRef.get()) == m_StorageForDelete.end() );
      
      for( InputIterator CurKey = FirstKey; CurKey != LastKey; ++CurKey )
      {
         ResPair = m_Storage.insert( TStorage::value_type( *CurKey, AutoPtrRef.get() ) );

         if( ResPair.second == false )
         {
            //Необходимо удалить уже добавленные объекты
            for( ; FirstKey != CurKey; ++FirstKey )
               m_Storage.erase( *FirstKey );

            return false;
         }
      }
      
      //Меняем владельца
      m_StorageForDelete.push_back(0);
      m_StorageForDelete.back() = AutoPtrRef.release();

      return true;
   }

   //Эквиваленто вызову Add( FirstKey, FirstKey + 1, AutoPtrRef )
   bool Add( const KeyType& K, TAutoPtrRef AutoPtrRef )
   {
      return Add( &K, &K + 1, AutoPtrRef );
   }

   void SetDeleteStrategy(const TDeleteStrategy &DS) {
      static_cast<TDeleteStrategy&>(*this) = DS;
   }

   const TDeleteStrategy &GetDeleteStrategy() const {
      return static_cast<const TDeleteStrategy&>(*this);
   }
};

///////////////////////////////////////////////////////////////////////////////
// Тоже что и CObjectsSelector, но позволяет связать несколько ключей с одним 
// указателем на объект и допускает что по одному ключу может быть несколько 
// объектов. Но всё вавно в CObjectsSelectorMultiObject должны хранится только 
// уникальные объекты
///////////////////////////////////////////////////////////////////////////////
template < class Key, class Object, template <class> class DeleteStrategy = auto_ptr_ex_delete_strategy >
class CObjectsSelectorMultiObject: public DeleteStrategy<Object>, public NonCopyable
{
public:  
   typedef DeleteStrategy<Object> TDeleteStrategy;

   typedef auto_ptr_ex<Object, DeleteStrategy> TAutoPtr;
   typedef auto_ptr_ex<Object, DeleteStrategy> &TAutoPtrRef;
   typedef Key KeyType;
   
   typedef auto_ptr_ex<const Object, DeleteStrategy> TConstAutoPtr;
   typedef typename TAutoPtr::value_type ObjectType;
   typedef typename TAutoPtr::pointer ObjectPtrType;
   typedef typename TConstAutoPtr::pointer ObjectConstPtrType;
   typedef typename TAutoPtr::reference ObjectRefType;
   typedef typename TConstAutoPtr::reference ObjectConstRefType;

private:
   typedef MultiAssocVector<Key, ObjectPtrType> TStorage;
   typedef std::vector<ObjectPtrType> TStorageForDelete;

private:
   template<class Iterator, class Value>
   struct Selector: SelectorHelper<Iterator, Value> 
   {
      reference operator ()( origin_iterator_reference V ) const { return *V.second; }
   };

public:
   typedef typename TStorageForDelete::iterator TIterator;
   typedef typename TStorageForDelete::const_iterator TConstIterator;

   typedef CSelectIterator<typename TStorage::iterator, Selector<typename TStorage::iterator, Object> > TResultIterator;
   typedef CSelectIterator<typename TStorage::const_iterator, Selector<typename TStorage::const_iterator, const Object> > TConstResultIterator;

   typedef std::pair< TResultIterator, TResultIterator > TResultPair;
   typedef std::pair< TConstResultIterator, TConstResultIterator > TConstResultPair;

private:
   TStorage m_Storage;
   TStorageForDelete m_StorageForDelete; //Объекты для удаления

public:  
   CObjectsSelectorMultiObject() {}
   CObjectsSelectorMultiObject(const TDeleteStrategy &DS): TDeleteStrategy(DS) {}
   
   //Итераторы возвращаются только для зарегистрированных объектов
   TIterator Begin() { return m_StorageForDelete.begin(); }
   TConstIterator Begin() const { return m_StorageForDelete.begin(); }
   TIterator End() { return m_StorageForDelete.end(); }
   TConstIterator End() const { return m_StorageForDelete.end(); }

   //Удалить все объекты которыми владеет класс
   void Clear()
   {
      for(TStorageForDelete::iterator I = m_StorageForDelete.begin(); I != m_StorageForDelete.end(); ++I )
         static_cast<TDeleteStrategy&>(*this)( *I );

      m_StorageForDelete.clear();
      m_Storage.clear();
   }

   bool Empty() const 
   {
      return m_Storage.empty();
   }

   ~CObjectsSelectorMultiObject()
   { 
      Clear();
   }

   //Вернуть итераторы объектов по ключу K. Если таких объектов нет возвращается пустой интервал
   TResultPair Get( const KeyType& K )
   {
	  //std::pair<TConstResultIterator, TConstResultIterator> p = m_Storage.equal_range(K);
      //return (p);

	  auto& temp = m_Storage.equal_range(K);
	  return make_pair((TResultIterator)temp.first, (TResultIterator)temp.second);
   }

   TConstResultPair Get( const KeyType& K ) const
   {
      return (m_Storage.equal_range(K));
   }
   
   //То же что и Get
   TResultPair operator[]( const KeyType& K ){ return Get(K); }
   TConstResultPair operator[]( const KeyType& K ) const { return Get(K); }

   //Добавить объект и ассоциировать его с групой ключей при этом при этом происходит 
   //смена владельца этого объекта. 
   template< class InputIterator >
   void Add( InputIterator FirstKey, InputIterator LastKey, TAutoPtrRef AutoPtrRef )
   {
      APL_ASSERT( std::find(m_StorageForDelete.begin(),  m_StorageForDelete.end(), AutoPtrRef.get()) == m_StorageForDelete.end() );

      for( InputIterator CurKey = FirstKey; CurKey != LastKey; ++CurKey )
         m_Storage.insert( TStorage::value_type( *CurKey, AutoPtrRef.get() ) );

      //Меняем владельца
      m_StorageForDelete.push_back(0);
      m_StorageForDelete.back() = AutoPtrRef.release();
   }

   //Эквиваленто вызову Add( FirstKey, FirstKey + 1, AutoPtrRef )
   void Add( const KeyType& K, TAutoPtrRef AutoPtrRef )
   {
      Add( &K, &K + 1, AutoPtrRef );
   }

   void SetDeleteStrategy(const TDeleteStrategy &DS) {
      static_cast<TDeleteStrategy&>(*this) = DS;
   }

   const TDeleteStrategy &GetDeleteStrategy() const {
      return static_cast<const TDeleteStrategy&>(*this);
   }
};

///////////////////////////////////////////////////////////////////////////////
// Класс связывает указатели на объёкты с последовательными индексами, 
// начинающимися с нуля (как в обычных массивах)
// При этом реализуется стратегия владения, т.е. объекты по сохранённым 
// адресам уничтожаются в деструкторе при помощи DeleteStrategy.
// В качестве хранилища объектов используется std::vector, поэтому итераторы 
// произвольного доступа
///////////////////////////////////////////////////////////////////////////////
template < class Object, template <class> class DeleteStrategy = auto_ptr_ex_delete_strategy >
class CObjectsSelectorIndex: public DeleteStrategy<Object>, public NonCopyable
{
public:  
   typedef DeleteStrategy<Object> TDeleteStrategy;
   typedef auto_ptr_ex<Object, DeleteStrategy> TAutoPtr;
   typedef auto_ptr_ex<Object, DeleteStrategy> &TAutoPtrRef; 
   
   typedef auto_ptr_ex<const Object, DeleteStrategy> TConstAutoPtr;
   typedef typename TAutoPtr::value_type ObjectType;
   typedef typename TAutoPtr::pointer ObjectPtrType;
   typedef typename TConstAutoPtr::pointer ObjectConstPtrType;
   typedef typename TAutoPtr::reference ObjectRefType;
   typedef typename TConstAutoPtr::reference ObjectConstRefType;

private:
   typedef std::vector<ObjectPtrType> TStorage;

   template< class Iterator, class Value >
   struct Selector: public SelectorHelper<Iterator, Value>
   {
      reference operator ()( origin_iterator_reference V ) const { return *V; }
   };

public:
   typedef typename TStorage::size_type KeyType;

private:
   TStorage m_Storage;

public:  
   typedef CSelectIterator<typename TStorage::iterator, Selector<typename TStorage::iterator, Object> > TIterator;
   typedef CSelectIterator<typename TStorage::const_iterator, Selector<typename TStorage::const_iterator, const Object> > TConstIterator;

public:  
   CObjectsSelectorIndex() {}
   CObjectsSelectorIndex(const TDeleteStrategy &DS): TDeleteStrategy(DS) {}

   TIterator Begin() { return TIterator(m_Storage.begin()); }
   TConstIterator Begin() const { return TConstIterator(m_Storage.begin()); }
   TIterator End() { return TIterator(m_Storage.end()); }
   TConstIterator End() const { return TConstIterator(m_Storage.end()); }

   //Удалить все объекты которыми владеет класс
   void Clear()
   {
      for(TStorage::iterator I = m_Storage.begin(); I != m_Storage.end(); ++I )
         static_cast<TDeleteStrategy&>(*this)( *I );

      m_Storage.clear();
   }

   bool Empty() const { return m_Storage.empty(); }

   ~CObjectsSelectorIndex() { Clear(); }

   //Выделить память под ожидаемое количество объктов 
   //Только, выделяет памыть под объекты, количество объектов остаётся неизменным
   void Reserve( KeyType N ){ m_Storage.reserve( N ); }

   //Ссылка на первый/последний элемент
   ObjectRefType Front(){ APL_ASSERT( !Empty() ); return *m_Storage.front(); }
   ObjectConstRefType Front() const { APL_ASSERT( !Empty() ); return *m_Storage.front(); }
   ObjectRefType Back(){ APL_ASSERT( !Empty() ); return *m_Storage.back(); }
   ObjectConstRefType Back() const { APL_ASSERT( !Empty() ); return *m_Storage.back(); }

   //Количество хранящихся объектов
   KeyType Size() const { return m_Storage.size(); }

   //Удаление объекта по ключу
   void Erase( KeyType K )
   {
      APL_ASSERT( K >= 0 && K < m_Storage.size() );

      static_cast<TDeleteStrategy&>(*this)( m_Storage[K] );
      m_Storage.erase(m_Storage.begin() + K);
   }

   //Вернуть указатель на объект по индексу.
   ObjectConstPtrType Get( KeyType K ) const
   {
      APL_ASSERT( K >= 0 && K < m_Storage.size() );
      
      return m_Storage[K];
   }

   ObjectPtrType Get( KeyType K )
   {
      APL_ASSERT( K >= 0 && K < m_Storage.size() );

      return m_Storage[K];
   }

   //Возвращает ссылку на хранящийся объект по индексу. Если объект по индексу не 
   //зарегистнрирован происходит разыменование нулевого указателя
   ObjectRefType operator[]( KeyType K ){ APL_ASSERT_PTR( Get(K) ); return *Get(K); }
   ObjectConstRefType operator[]( KeyType K ) const { APL_ASSERT_PTR( Get(K) ); return *Get(K); }

   //Добавить объект, при этом при этом происходит смена владельца. Индекс добавляемого объекта
   //будет равен количеству объектов до добавления. (Происходит добавление в конец)
   //Возвращает индекс добавленного элемента
   KeyType Append( TAutoPtrRef AutoPtrRef )
   {
      m_Storage.push_back( AutoPtrRef.get() );
      AutoPtrRef.release();

      return m_Storage.size() - 1;
   }

   void SetDeleteStrategy(const TDeleteStrategy &DS) {
      static_cast<TDeleteStrategy&>(*this) = DS;
   }

   const TDeleteStrategy &GetDeleteStrategy() const {
      return static_cast<const TDeleteStrategy&>(*this);
   }
};

}} //namespace SS::Lib 

#endif