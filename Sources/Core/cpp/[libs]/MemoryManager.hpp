///////////////////////////////////////////////////////////////////////////////
// Модуль содержит функционал отвечающий за физическую организацию и работу с 
// памятью, кешированием, пулом и т.п.
///////////////////////////////////////////////////////////////////////////////
#ifndef MEMORYMANAGER_HPP
#define MEMORYMANAGER_HPP

#include "LikePointer.hpp"

namespace SS { namespace Lib {

///////////////////////////////////////////////////////////////////////////////
// Пул объектов
// Позволяет создавать и удалять объекты, причём при удалении объекты не 
// уничтожаются физически и сохраняются до тех пор пока в них снова не возникнет 
// необходимость. Это делается для предотвращения расходов на повторное 
// создание/удаление объектов.
// В деструкторе все объекты находящияся в пуле уничтожаются.
//
// Данным класом следует пользоваться в том случае когда объекты часто создаются и 
// уничтожаются и затраты на их конструирование и/или деструкцию значительные.
//
// Параметры шаблона
// ObjectT - Тип объектов которые предполагается создавать
// InitStrategyT - Стратегия инициализации и удаления объектов ObjectT
///////////////////////////////////////////////////////////////////////////////
 
///////////////////////////////////////////////////////////////////////////////
// Базовое поведение для стратегии инициализации и удаления создаваемых объектов
// все опредиляемые клиентом стратегии должны наследоваться от даннонной стратегии
// замещая по необходимости нужные методы. Стратегия передаётся в TObjectsPool
// в качестве наблонного параметра InitStrategyT
///////////////////////////////////////////////////////////////////////////////
template<class ObjectT>
struct TObjectsPoolBaseInitStrategy
{
   typedef ObjectT TObject;
   
   //Создать объект в уже выделенной области памяти размером sizeof(ObjectT)
   //В TObjectsPoolBaseInitStrategy используется placement new и конструктор по-умолчанию
   void Construct( void *pBuf ) const { new(pBuf) ObjectT(); }
   
   //Уничтожить объект pT, не освобождая при этом паимять по адресу pT, т.е.
   //вызвать деструктор объекта pT (или иной способ уничтожения), при этом
   //не вызывая оператор delete для pT
   //В TObjectsPoolBaseInitStrategy используется вызов деструктора
   void Destroy( TObject &Obj ) const { Obj.~ObjectT(); }
   
   //Подготовить объект ObjectT к повторному использованию.
   //вернуть в начальное состояние объект перед тем как он, будучи однажды 
   //использованным клиентом, вместо уничтожения, был помещён в пул, снова 
   //будет возвращён клиенту из пула.
   //В TObjectsPoolBaseInitStrategy функция ничего не делает
   void ReInit( TObject &Obj ) const { /*По-умолчанию ничего не делаем*/ }
};

template<class ObjectT, class InitStrategyT = TObjectsPoolBaseInitStrategy<ObjectT> >
class TObjectsPool: private InitStrategyT, public NonCopyable
{ 
   // Удалённые объекты выстраиваются в связанный односторонний список - пул и ждут
   // запроса создание объекта. При создании объекта если пул не пуст объект извлекается 
   // из него, а если пуст то создаётся новый объект.

private:   
   //Элемент пула, содержит сам объект и укзатель на следующий такой же элемент 
   //для организации списка. Объект защищён от вызова деструктора и конструктора и 
   //создаётся явно.
   struct TStoreData: NonCreateable
   {
      ObjectT Object; //Сохранённый объект 
   
   private:    
      friend class TObjectsPool;
      TStoreData *pNext;  //Следующий элемент списка

   private:
      ~TStoreData(); //Убираем предупреждение C4624
   };

   template<class Iterator, class Value>
   struct TObjectSelector: SelectorHelper<Iterator, Value> 
   {
      reference operator ()( origin_iterator_reference V ) const { return V.Object; }
   };

public:
   //Указатели на создаваемый объект
   typedef CSelectIterator< TStoreData *, TObjectSelector< TStoreData *, ObjectT> > TPointer;
   typedef CSelectIterator< const TStoreData *, TObjectSelector< const TStoreData *, const ObjectT> > TConstPointer;

private: 
   //Базовый класс стратегии удаления со специализацией для констрантных указателей
   template< class T >  
   struct TObjectsPoolDeleteStrategyBase: public auto_ptr_ex_strategy_base<T, TPointer> {};
   
   template< class T >
   struct TObjectsPoolDeleteStrategyBase<const T>: public auto_ptr_ex_strategy_base<T, TConstPointer> {};

public:
   //Стратегия удаления для классов auto_ptr_ex и подобных 
   template< class T >
   class TObjectsPoolDeleteStrategy: public TObjectsPoolDeleteStrategyBase<T> 
   {
      TObjectsPool *m_pObjectsPool;  
      
   public: 
      //не explicit
      TObjectsPoolDeleteStrategy( TObjectsPool *pObjectsPool ): m_pObjectsPool(pObjectsPool) {}

      template< class Y >
         TObjectsPoolDeleteStrategy( const TObjectsPoolDeleteStrategy<Y> &Other ): m_pObjectsPool(Other.m_pObjectsPool) {}

      template< class Y >
         TObjectsPoolDeleteStrategy &operator=( const TObjectsPoolDeleteStrategy<Y> &Other ) { m_pObjectsPool = Other.m_pObjectsPool; return *this; }

      void operator()( pointer pT ) const { if(pT != 0){ APL_ASSERT_PTR(m_pObjectsPool); m_pObjectsPool->Dealloc(pT); } }

      TObjectsPool &GetObjectsPool() const { APL_ASSERT_PTR(m_pObjectsPool); return *m_pObjectsPool; }
   };

public:
   //Типы smart-указателей
   typedef auto_ptr_ex<ObjectT, TObjectsPoolDeleteStrategy> TAutoPtr;
   typedef auto_ptr_ex<const ObjectT, TObjectsPoolDeleteStrategy> TConstAutoPtr;

private:
   TStoreData *m_pFirst; //Начало пула, связанного списка
   
public:
   explicit TObjectsPool(const InitStrategyT &IS): InitStrategyT(IS), m_pFirst(0) {}
   TObjectsPool(): m_pFirst(0) {}
   ~TObjectsPool() { Clear(); } 

   //Возможные манипуляции со стратегией
   const InitStrategyT &GetInitStrategy() const { return *this; }
   void SetInitStrategy( const InitStrategyT &IS ) { static_cast<InitStrategyT &>(*this) = IS; }

   //Получение указателя на готовый к использованию объект ObjectT. После использования объекта 
   //его необходимо освободить вызовом Dealloc
   TPointer AlloсPtr()
   {
      if( m_pFirst == 0 )
      {
         TPointer RetVal( static_cast<TStoreData *>(operator new(sizeof(TStoreData))) );
         InitStrategyT::Construct(&*RetVal);
         
         return RetVal;   
      }
      else
      {
         TStoreData *pCur = m_pFirst;
         m_pFirst = pCur->pNext;

         InitStrategyT::ReInit(pCur->Object);
         return TPointer(pCur);
      }
   }
   
   //Получение smart указателя на готовый к использованию объект ObjectT. После использования
   //smart указатель автоматически освободить объект (вызывать Dealloc не надо)
   TAutoPtr Alloc()
   {
      return TAutoPtr(AlloсPtr(), this);
   }

   //Освобождение объекта полученного при помощи метода AlloсPtr, после использования
   void Dealloc( TPointer pObject )
   {
      pObject.GetOriginIterator()->pNext = m_pFirst;
      m_pFirst = pObject.GetOriginIterator();
   }

   //Удаление всех объектов хранящихся в пуле
   void Clear()
   {
      TStoreData *pCur;

      while( m_pFirst != 0 )
      {
         pCur = m_pFirst;
         m_pFirst = m_pFirst->pNext;
        
         InitStrategyT::Destroy(pCur->Object);
         operator delete( pCur );
      }
   }
};

}} //namespace SS::Lib 
#endif // MEMORYMANAGER_HPP

