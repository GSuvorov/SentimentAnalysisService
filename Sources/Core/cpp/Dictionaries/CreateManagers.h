#pragma once

namespace SS{ 
namespace Dictionary{


/**
   Набор менеджеров для создания объектов в зависимости от 
   параметров переданных в функцию. Причём если ранее для этой комбинации параметров уже был 
   создан такой объект, то новый объект не создаётся, а возвращается указатель на созданый 
   ранее объект. Менеджеры автоматически удаляеют все объекты которые были ими созданны.
   Все менеджеры реализованы как singleton'ы. для того чтобы получить указатель на менеджер
   необходимо вызвать его статический метод Create()

   Для создания зависимого от параметров объекта используется функция TPointer Construct(...)
   Которая возвращает объект который можно присваивать такому же объекту и у которого 
   определён опрератор -> (т.е. это могут быть как обычные указатели на создаваемый объект так 
   и smart указатели или чтото ещё)

   Кроме этого менеджеры опредиляют TConstPointer - константный указатель на объект
*/
                           
//Менеджер CMorphoTypesSearch
class CMorphoTypesSearchCreateManager: public NonCopyable
{
private:
   CMorphoTypesSearchCreateManager() {} //Запрещаем инстанцирование из вне

private:
   typedef std::wstring TMapKey;

   typedef std::map<TMapKey, Core::CMorphoTypesSearch *> TObjectsMap;
   typedef ClassLevelLockable<CMorphoTypesSearchCreateManager, SCriticalSectionImplStategy, CURRENT_THREAD_DEBUG_INFO> TMutex;

public:
   typedef LockedPtr<Core::CMorphoTypesSearch> TPointer;
   typedef LockedPtr<const Core::CMorphoTypesSearch> TConstPointer;
   typedef CMorphoTypesSearchCreateManager ThisType;

private:
   TObjectsMap m_ObjectsMap;
   static TMutex m_Mutex;
   static ThisType *pInstance_;

   static ThisType *CreateImpl()
   {
      static ThisType Obj;
      return &Obj;
   }


public:
   static ThisType *Create()
   {
      //Мы реализуем блокировку с двумя проверками
      if( pInstance_ == 0 )
      {
         TMutex::Lock Guard(m_Mutex);

         if( pInstance_ == 0 )
            pInstance_ = CreateImpl();
      }
      
      return pInstance_;
   }

   TPointer Construct( DBMS::IDataBase *pIDataBase, const TCHAR * szPathToDataFiles);
   ~CMorphoTypesSearchCreateManager();
};


//Менеджер CPartOfWordsSearch
class CPartOfWordsSearchCreateManager: public NonCopyable
{
private:
   CPartOfWordsSearchCreateManager() {} //Запрещаем инстанцирование из вне

private:
   typedef std::pair<
      std::wstring,
      std::wstring
   > TMapKey;

   typedef std::map<TMapKey, Core::CPartOfWordsSearch *> TObjectsMap;
   typedef ClassLevelLockable<CPartOfWordsSearchCreateManager, SCriticalSectionImplStategy, CURRENT_THREAD_DEBUG_INFO> TMutex;

public:
   typedef LockedPtr<Core::CPartOfWordsSearch> TPointer;
   typedef LockedPtr<const Core::CPartOfWordsSearch> TConstPointer;
   typedef CPartOfWordsSearchCreateManager ThisType;

private:
   TObjectsMap m_ObjectsMap;
   static TMutex m_Mutex;
   static ThisType *pInstance_;
   
   static ThisType *CreateImpl()
   {
      static ThisType Obj;
      return &Obj;
   }

public:
   static ThisType *Create()
   {
      //Мы реализуем блокировку с двумя проверками
      if( pInstance_ == 0 )
      {
         TMutex::Lock Guard(m_Mutex);

         if( pInstance_ == 0 )
            pInstance_ = CreateImpl();
      }

      return pInstance_;
   }

   TPointer Construct( const TCHAR * szPathToDataFiles, DBMS::IDataTable *pIDataTable );
   ~CPartOfWordsSearchCreateManager();
};

//Менеджер CSourceToType
//Возвращает константный указатель без синхронизации
class CSourceToTypeCreateManager: public NonCopyable
{
private:
   CSourceToTypeCreateManager() {} //Запрещаем инстанцирование из вне

private:
   typedef std::pair<
      std::wstring,
      std::wstring
   > TMapKey;

   typedef std::map<TMapKey, Detail::CSourceToType *> TObjectsMap;
   typedef ClassLevelLockable<CSourceToTypeCreateManager, SCriticalSectionImplStategy, CURRENT_THREAD_DEBUG_INFO> TMutex;

public:
   typedef LockedPtr<Detail::CSourceToType> TPointer;
   typedef LockedPtr<const Detail::CSourceToType> TConstPointer;
   //typedef Detail::CSourceToType* TPointer;
   //typedef const Detail::CSourceToType* TConstPointer;
   typedef CSourceToTypeCreateManager ThisType;

private:
   TObjectsMap m_ObjectsMap;
   static TMutex m_Mutex;
   static ThisType *pInstance_;

   static ThisType *CreateImpl()
   {
      static ThisType Obj;
      return &Obj;
   }

public:
   static ThisType *Create()
   {
      //Мы реализуем блокировку с двумя проверками
      if( pInstance_ == 0 )
      {
         TMutex::Lock Guard(m_Mutex);

         if( pInstance_ == 0 )
            pInstance_ = CreateImpl();
      }

      return pInstance_;
   }

   TPointer Construct( DBMS::IDataBase *pDataBase, std::wstring TableName );
   
   ~CSourceToTypeCreateManager();
};

//Менеджер создания CSymbolCharacteristic
//Создаётся один объект в памяти и возвращается константный указатель на него
class CSymbolCharacteristicCreateManager: public NonCopyable
{   
public:
   typedef Detail::CSymbolCharacteristic<wchar_t, InitData::ObjectID::FULL_CHARACTERISTIC_COUNT/*, InitData::ObjectID::DICTS_COUNT*/> TCreateType;
   typedef TCreateType* TPointer;
   typedef const TCreateType* TConstPointer;

private:
   CSymbolCharacteristicCreateManager() {} //Запрещаем инстанцирование из вне

private:
   typedef ClassLevelLockable<CSymbolCharacteristicCreateManager, SCriticalSectionImplStategy, CURRENT_THREAD_DEBUG_INFO> TMutex;

   struct SCreateStruct
   {
      TCreateType CreateType;

      SCreateStruct()
      {
         DEBUG_MSG_LOG2( "Создаём CSymbolCharacteristic процесс: "  << GetCurrentProcessId() << 
            " поток: " << GetCurrentThreadId() << " размер класса: " << (unsigned int)CreateType.MemSize()
            );
      }
      
      ~SCreateStruct()
      {
         DEBUG_MSG_LOG2( "Удаляем CSymbolCharacteristic процесс: "  << GetCurrentProcessId() << 
            " поток: " << GetCurrentThreadId()
            );
      }
   };

private:
   static TMutex m_Mutex;
   static TConstPointer pInstance_;

private:
   static TConstPointer CreateImpl()
   {
      static SCreateStruct Obj;
      static const wchar_t rgHypenChars[] = L"-";
      static const wchar_t rgQuotationSChars[] = L"'sS";
      static const wchar_t rgQuotationNTChars[] = L"'ntNT";

      for( size_t i = 0; i < InitData::ObjectID::DICTS_COUNT; ++i )
         Obj.CreateType.SetFlags(InitData::g_hModuleCurrentDLL, InitData::DictionaryResClass, InitData::DictionaryToResId[i], i, false );

      //Символы в словаре символов могут быть любыми
      Obj.CreateType.SetFlags( InitData::ObjectID::Symbols );
      Obj.CreateType.SetFlagsInvert( rgHypenChars, rgHypenChars + APL_ARRSIZE(rgHypenChars) - 1, InitData::ObjectID::ContainHypen );
      Obj.CreateType.SetFlagsInvert( rgQuotationSChars, rgQuotationSChars + APL_ARRSIZE(rgQuotationSChars) - 1, InitData::ObjectID::ContainQuotationS );
      Obj.CreateType.SetFlagsInvert( rgQuotationNTChars, rgQuotationNTChars + APL_ARRSIZE(rgQuotationNTChars) - 1, InitData::ObjectID::ContainQuotationNT );

      return &Obj.CreateType;
   }

public:
   static TConstPointer Create()
   {
      //Мы реализуем блокировку с двумя проверками
      if( pInstance_ == 0 )
      {
         TMutex::Lock Guard(m_Mutex);

         if( pInstance_ == 0 )
            pInstance_ = CreateImpl();
      }

      return pInstance_;
   }
};

class CAbbreviationsDictionaryBaseMemory;

//Менеджер CAbbreviationsDictionaryBaseMemory
class CAbbreviationsDictionaryBaseMemoryManager: public NonCopyable
{
private:
   CAbbreviationsDictionaryBaseMemoryManager() {} //Запрещаем инстанцирование из вне

private:
   typedef std::pair<
      std::wstring,
      std::wstring
   > TMapKey;

   typedef std::map<TMapKey, CAbbreviationsDictionaryBaseMemory *> TObjectsMap;
   typedef ClassLevelLockable<CAbbreviationsDictionaryBaseMemoryManager, SCriticalSectionImplStategy, CURRENT_THREAD_DEBUG_INFO> TMutex;

public:
   typedef CAbbreviationsDictionaryBaseMemory *TPointer;
   typedef const CAbbreviationsDictionaryBaseMemory *TConstPointer;
   typedef CAbbreviationsDictionaryBaseMemoryManager ThisType;

private:
   TObjectsMap m_ObjectsMap;
   static TMutex m_Mutex;
   static ThisType *pInstance_;

   static ThisType *CreateImpl()
   {
      static ThisType Obj;
      return &Obj;
   }

public:
   static ThisType *Create()
   {
      //Мы реализуем блокировку с двумя проверками
      if( pInstance_ == 0 )
      {
         TMutex::Lock Guard(m_Mutex);

         if( pInstance_ == 0 )
            pInstance_ = CreateImpl();
      }

      return pInstance_;
   }

   //Возвращается TPointer потмау как все функкции, кроме перезагрузки из базы являются 
   //потокобезовапсными, а загрузка из базы синхронизируется более высоким уровнем.
   TPointer Construct( 
      DBMS::IDataBase *pMorphoDataBase, DBMS::IDataBase *pAbbreviationsDataBase, const TCHAR *TableName, 
      TDictNumber DictionaryIndex 
   );

   ~CAbbreviationsDictionaryBaseMemoryManager();
};

//Менеджер создания TConstData
//Создаётся один объект в памяти и возвращается константный указатель на него
class CConstDataCreateManager: public NonCopyable
{   
public:
   typedef Detail::TConstData TCreateType;
   typedef TCreateType* TPointer;
   typedef const TCreateType* TConstPointer;

private:
   CConstDataCreateManager() {} //Запрещаем инстанцирование из вне

private:
   typedef ClassLevelLockable<CConstDataCreateManager, SCriticalSectionImplStategy, CURRENT_THREAD_DEBUG_INFO> TMutex;

   struct SCreateStruct
   {
      TCreateType CreateType;

      SCreateStruct()
      {
         DEBUG_MSG_LOG2( "Создаём CConstDataCreateManager процесс: "  << GetCurrentProcessId() << 
            " поток: " << GetCurrentThreadId()
            );
      }
      
      ~SCreateStruct()
      {
         DEBUG_MSG_LOG2( "Удаляем CConstDataCreateManager процесс: "  << GetCurrentProcessId() << 
            " поток: " << GetCurrentThreadId()
            );
      }
   };

private:
   static TMutex m_Mutex;
   static TConstPointer pInstance_;

private:
   static TConstPointer CreateImpl()
   {
      static SCreateStruct Obj;
  
      Obj.CreateType.LoadFromResource( InitData::g_hModuleCurrentDLL, InitData::ConstDataResClass, IDR_ENG_NEGATION_REDUCTION, Obj.CreateType.GetEngNegationReduction() );
      //Obj.CreateType.LoadFromResource( InitData::g_hModuleCurrentDLL, InitData::ConstDataResClass, IDR_PRE_SEARCH_TRANSFORM, Obj.CreateType.GetPreSearchTransform() );

      return &Obj.CreateType;
   }

public:
   static TConstPointer Create()
   {
      //Мы реализуем блокировку с двумя проверками
      if( pInstance_ == 0 )
      {
         TMutex::Lock Guard(m_Mutex);

         if( pInstance_ == 0 )
            pInstance_ = CreateImpl();
      }

      return pInstance_;
   }
};

} //namespace Dictionary
} //namespace SS


