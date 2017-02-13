#ifndef ReferenceCounters_HPP
#define ReferenceCounters_HPP

///////////////////////////////////////////////////////////////////////////////
// –азличные реализации счЄтчиков ссылок
///////////////////////////////////////////////////////////////////////////////

namespace SS { namespace Lib {

///////////////////////////////////////////////////////////////////////////////
//  ласс позвол€ет добавл€ть счЄтчик ссылок любому классу от которого можно 
// наследоватьс€ и который имеет конструктор по-умолчанию.
// ѕри помощи статического метода Create можно получить интел-ый указатель на 
// на тип переданный как параметр шаблона в CAutoRefCount, которые можно 
// копировать друг в друга. хранить в STL контейнерах и т.п. ѕосле того как 
// последний такой указатель будет уничтожен автоматически вызоветс€ метод 
// delete
///////////////////////////////////////////////////////////////////////////////
template< class ObjectT >
class CAutoRefCount
{
   //¬нутренний класс который предоставл€ет методы по подсчЄту ссылок
   class CImpl: public ObjectT
   {
      size_t m_RefCount;

      CImpl(): m_RefCount(1) {}
   public:
      static CImpl *Create() { return new CImpl; }

      void AddRef(){ ++m_RefCount; }
      void Release(){ APL_ASSERT( m_RefCount > 0 ); if( --m_RefCount == 0 ) delete this; }
   };

   //–еализаци€ интелликтуального указател€
   template<class ResultT>
   class CPtrBase
   {
      CImpl *m_pT;

   public:                                           
      //template< class > template< class T2 > friend class CAutoRefCount<T1>::CPtrBase;
      template< class T2 > friend class CPtrBase;

      CPtrBase( CImpl *pT = 0 ): m_pT(pT) {}
      //CPtrBase( TUndefinedBoolType pT = 0 ): m_pT(0) {} //ѕозвол€ет конструировать только из 0


      CPtrBase( const CPtrBase &Other )
      { 
         m_pT = Other.m_pT;

         if( m_pT != 0 )
            m_pT->AddRef();
      }

      ~CPtrBase()
      {
         if( m_pT != 0 )
            m_pT->Release();
      }

      void Swap( CPtrBase &Other )
      {
         std::swap(m_pT, Other.m_pT);
      }

      void Release()
      {
         if( m_pT != 0 )
         {
            m_pT->Release();
            m_pT = 0;
         }
      }

      CPtrBase &operator=( const CPtrBase &Other )
      {
         CPtrBase(Other).Swap(*this);
         return *this;
      }

      //ѕозволим провер€ть указатель на равенство/неравенство нулю и приводить к типу bool
   private:  
      struct OperatorHelper{ int i; }; //см. јлександровску(loki)
      typedef int OperatorHelper::*TUndefinedBoolType;
   public:
      //ѕозволет провер€ть указатель на равенство/неравенство нулю и приводить к типу bool
      operator TUndefinedBoolType() const { return m_pT != 0? &OperatorHelper::i : 0; }

      ResultT *Get() const { return m_pT; }
      ResultT &operator*() const { APL_ASSERT( Get() != 0 ); return *Get(); }
      ResultT *operator->() const { return &**this; }
  
      //≈сли не определ€ть то будет использоватьс€ автоматическое преобразование к TUndefinedBoolType
      //и встроенные операторы
      friend bool operator==( const CPtrBase &P1, const CPtrBase &P2 ){ return P1.m_pT == P2.m_pT; }
      friend bool operator!=( const CPtrBase &P1, const CPtrBase &P2 ){ return P1.m_pT != P2.m_pT; }

      //≈сли не опредил€ть по при проверке P1 == 0, 0 == P1, P1 != 0, 0 != P1 возникнет 
      //неоднозначность т.к. конструтор не explicit
      friend bool operator==( const CPtrBase &P1, TUndefinedBoolType P2 ){ return P1.m_pT == 0; }
      friend bool operator==( TUndefinedBoolType P2, const CPtrBase &P1 ){ return P1.m_pT == 0; }
      friend bool operator!=( const CPtrBase &P1, TUndefinedBoolType P2 ){ return P1.m_pT != 0; }
      friend bool operator!=( TUndefinedBoolType P2, const CPtrBase &P1 ){ return P1.m_pT != 0; }


   protected:
      template< class T >
         CPtrBase( const CPtrBase<T> &Other ) //Ўаблоный конструктор не замен€ет конструктор по умолчанию
      { 
         m_pT = Other.m_pT;

         if( m_pT != 0 )
            m_pT->AddRef();
      }

      template< class T >
         CPtrBase &operator=( const CPtrBase<T> &Other ) //Ўаблоный оператор присваивани€ не замен€ет оператор присваивани€ по умолчанию
      {
         CPtrBase(Other).Swap(*this);
         return *this;
      }
   };
public:

   //»нетлликтуальный указатель возращаемый функцией Create
   class SmartPtr: public CPtrBase<ObjectT>
   {
   public:
      SmartPtr( CImpl * pImpl = 0 ): CPtrBase<ObjectT>(pImpl){}
   };

   // онстантный ител-ый указатель (позвол€ет получить доступ только к const ObjectT)
   //ћожет быть инициилизован из SmartPtr, но не наоборот
   class ConstSmartPtr: public CPtrBase<const ObjectT>
   {
   public:
      ConstSmartPtr( CImpl * pImpl = 0 ): CPtrBase<const ObjectT>(pImpl){}
      ConstSmartPtr( const SmartPtr &SM ): CPtrBase<const ObjectT>(SM) {}
      ConstSmartPtr &operator=( const SmartPtr &SM ){ CPtrBase<const ObjectT>::operator=(SM); return *this; }
   };

   //—оздаЄт новый объект типа ObjectT
   static SmartPtr Create(){ return SmartPtr( CImpl::Create() ); }
};

}} //namespace SS::Lib 

#endif
