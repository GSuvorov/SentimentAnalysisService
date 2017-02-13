#include "stdafx.h"

namespace SS{ 
namespace Dictionary{

CMorphoTypesSearchCreateManager::TMutex CMorphoTypesSearchCreateManager::m_Mutex;
CPartOfWordsSearchCreateManager::TMutex CPartOfWordsSearchCreateManager::m_Mutex;
CSourceToTypeCreateManager::TMutex CSourceToTypeCreateManager::m_Mutex;
CSymbolCharacteristicCreateManager::TMutex CSymbolCharacteristicCreateManager::m_Mutex;
CAbbreviationsDictionaryBaseMemoryManager::TMutex CAbbreviationsDictionaryBaseMemoryManager::m_Mutex;
CConstDataCreateManager::TMutex CConstDataCreateManager::m_Mutex;

CMorphoTypesSearchCreateManager::ThisType *CMorphoTypesSearchCreateManager::pInstance_ = 0;
CPartOfWordsSearchCreateManager::ThisType *CPartOfWordsSearchCreateManager::pInstance_ = 0;
CSourceToTypeCreateManager::ThisType *CSourceToTypeCreateManager::pInstance_ = 0;
CSymbolCharacteristicCreateManager::TConstPointer CSymbolCharacteristicCreateManager::pInstance_ = 0;
CAbbreviationsDictionaryBaseMemoryManager::ThisType *CAbbreviationsDictionaryBaseMemoryManager::pInstance_ = 0;
CConstDataCreateManager::TConstPointer CConstDataCreateManager::pInstance_ = 0;

///////////////////////////////////////////////////////////////////////////////

CMorphoTypesSearchCreateManager::TPointer CMorphoTypesSearchCreateManager::Construct( DBMS::IDataBase *pIDataBase , const TCHAR * szPathToDataFiles)
{
   APL_ASSERT_PTR(pIDataBase);

   TMutex::Lock Guard(m_Mutex);

   TMapKey Key(pIDataBase->GetName());   
   TObjectsMap::iterator I = m_ObjectsMap.find(Key);

   if( I == m_ObjectsMap.end() )
   {
      DEBUG_MSG_LOG2( "Создаём CMorphoTypesSearch процесс: "  << GetCurrentProcessId() << 
         " поток: " << GetCurrentThreadId() << 
         " DB: " << Convert( pIDataBase->GetName(), std::string() ) 
         );

      auto_ptr_ex<Core::CMorphoTypesSearch> pNewObj( new Core::CMorphoTypesSearch );

      if( !pNewObj->Init(szPathToDataFiles, pIDataBase) ) //Первый параметр игрнорируется
         SS_THROW( L"Ошибка при инициализации CMorphoTypesSearch" );

      if( !pNewObj->Load() )
         SS_THROW( L"Ошибка при загрузке CMorphoTypesSearch" );

      m_ObjectsMap.insert( TObjectsMap::value_type(Key, pNewObj.get()) );

      return TPointer( pNewObj.release() );
   }

   return TPointer( I->second );
}
///////////////////////////////////////////////////////////////////////////////

CMorphoTypesSearchCreateManager::~CMorphoTypesSearchCreateManager()
{
   for( TObjectsMap::iterator I = m_ObjectsMap.begin(); I != m_ObjectsMap.end(); ++I )
   {
      DEBUG_MSG_LOG2( "Удаляем CMorphoTypesSearch процесс: "  << GetCurrentProcessId() << 
         " поток: " << GetCurrentThreadId() << 
         " DB: " << Convert( I->first, std::string() ) 
         );

      delete I->second;
   }
}
///////////////////////////////////////////////////////////////////////////////

CPartOfWordsSearchCreateManager::TPointer CPartOfWordsSearchCreateManager::Construct( const TCHAR * szPathToDataFiles, DBMS::IDataTable *pIDataTable )
{
   APL_ASSERT_PTR(szPathToDataFiles);
   APL_ASSERT_PTR(pIDataTable);

   TMutex::Lock Guard(m_Mutex);

   TMapKey Key(szPathToDataFiles, pIDataTable->GetName());   
   TObjectsMap::iterator I = m_ObjectsMap.find(Key);

   if( I == m_ObjectsMap.end() )
   {
      DEBUG_MSG_LOG2( "Создаём CPartOfWordsSearch процесс: "  << GetCurrentProcessId() << 
         " поток: " << GetCurrentThreadId() << 
         " путь: " << Convert( szPathToDataFiles, std::string() ) << 
         " DB: " << Convert( pIDataTable->GetName(), std::string() ) 
         );

      auto_ptr_ex<Core::CPartOfWordsSearch> pNewObj( new Core::CPartOfWordsSearch );

      if( !pNewObj->Init(szPathToDataFiles, pIDataTable) )
         SS_THROW( L"Ошибка при инициализации CPartOfWordsSearch" );

      if( !pNewObj->LoadIfFilesExists() )
         SS_THROW( L"Ошибка при загрузке CPartOfWordsSearch" );

      m_ObjectsMap.insert( TObjectsMap::value_type(Key, pNewObj.get()) );

      return TPointer( pNewObj.release() );
   }

   return TPointer( I->second );
}
///////////////////////////////////////////////////////////////////////////////

CPartOfWordsSearchCreateManager::~CPartOfWordsSearchCreateManager()
{
   for( TObjectsMap::iterator I = m_ObjectsMap.begin(); I != m_ObjectsMap.end(); ++I )
   {
      DEBUG_MSG_LOG2( "Удаляем CPartOfWordsSearch процесс: "  << GetCurrentProcessId() << 
         " поток: " << GetCurrentThreadId() << 
         " путь: " << Convert( I->first.first, std::string() ) << 
         " DB: " << Convert( I->first.second, std::string() )  
         );

      delete I->second;
   }
}
///////////////////////////////////////////////////////////////////////////////

CSourceToTypeCreateManager::TPointer CSourceToTypeCreateManager::Construct( DBMS::IDataBase *pDataBase, std::wstring TableName )
{
   APL_ASSERT_PTR(pDataBase);
   
   TMutex::Lock Guard(m_Mutex);

   TMapKey Key(pDataBase->GetName(), TableName);   
   TObjectsMap::iterator I = m_ObjectsMap.find(Key);

   if( I == m_ObjectsMap.end() )
   {
      DEBUG_MSG_LOG2( "Создаём CSourceToType процесс: "  << GetCurrentProcessId() << 
         " поток: " << GetCurrentThreadId() << 
         " таблица: " << Convert( TableName, std::string() ) << 
         " DB: " << Convert( pDataBase->GetName(), std::string() ) 
         );

      auto_ptr_ex<Detail::CSourceToType> pNewObj( new Detail::CSourceToType );

      pNewObj->Init(pDataBase, TableName.c_str());

      if( !pNewObj->Load() )  
         SS_THROW( L"Ошибка при загрузке CSourceToType" );

      m_ObjectsMap.insert( TObjectsMap::value_type(Key, pNewObj.get()) );

      return TPointer( pNewObj.release() );
   }

   return TPointer( I->second );
}
///////////////////////////////////////////////////////////////////////////////

CSourceToTypeCreateManager::~CSourceToTypeCreateManager()
{
   for( TObjectsMap::iterator I = m_ObjectsMap.begin(); I != m_ObjectsMap.end(); ++I )
   {
      DEBUG_MSG_LOG2( "Удаляем CSourceToType процесс: "  << GetCurrentProcessId() << 
         " поток: " << GetCurrentThreadId() << 
         " таблица: " << Convert( I->first.second, std::string() ) << 
         " DB: " << Convert( I->first.first, std::string() ) 
         );

      delete I->second;
   }
}
///////////////////////////////////////////////////////////////////////////////

CAbbreviationsDictionaryBaseMemoryManager::TPointer CAbbreviationsDictionaryBaseMemoryManager::Construct( 
   DBMS::IDataBase *pMorphoDataBase, DBMS::IDataBase *pAbbreviationsDataBase, const TCHAR *TableName, 
   TDictNumber DictionaryIndex 
)
{
   APL_ASSERT_PTR(pMorphoDataBase);
   APL_ASSERT_PTR(pAbbreviationsDataBase);
   APL_ASSERT_PTR(TableName);

   TMutex::Lock Guard(m_Mutex);

   TMapKey Key(pAbbreviationsDataBase->GetName(), TableName);   
   TObjectsMap::iterator I = m_ObjectsMap.find(Key);

   if( I == m_ObjectsMap.end() )
   {
      DEBUG_MSG_LOG2( "Создаём CAbbreviationsDictionaryBaseMemory процесс: "  << GetCurrentProcessId() << 
         " поток: " << GetCurrentThreadId() << 
         " MorphoDB: " << Convert( pMorphoDataBase->GetName(), std::string() ) << 
         " AbbreviationsDB: " << Convert( pAbbreviationsDataBase->GetName(), std::string() ) << 
         " таблица: " << Convert( TableName, std::string() ) <<
         " DictNumber: " << DictNumberDescription(DictionaryIndex)
      );

      auto_ptr_ex<CAbbreviationsDictionaryBaseMemory> pNewObj( new CAbbreviationsDictionaryBaseMemory );

      //Иногда появляются пустые таблицы
      //if( !pNewObj->Init(pMorphoDataBase, pAbbreviationsDataBase, TableName, DictionaryIndex) )
      //   SS_THROW( L"Ошибка при инициализации CAbbreviationsDictionaryBaseMemory" );
      pNewObj->Init(pMorphoDataBase, pAbbreviationsDataBase, TableName, DictionaryIndex);

      m_ObjectsMap.insert( TObjectsMap::value_type(Key, pNewObj.get()) );

      return TPointer( pNewObj.release() );
   }

   return TPointer( I->second );
}
///////////////////////////////////////////////////////////////////////////////

CAbbreviationsDictionaryBaseMemoryManager::~CAbbreviationsDictionaryBaseMemoryManager()
{
   for( TObjectsMap::iterator I = m_ObjectsMap.begin(); I != m_ObjectsMap.end(); ++I )
   {
      DEBUG_MSG_LOG2( "Удаляем CAbbreviationsDictionaryBaseMemory процесс: "  << GetCurrentProcessId() << 
         " поток: " << GetCurrentThreadId() << 
         " MorphoDB: " << Convert( I->first.first, std::string() ) << 
         " таблица: " << Convert( I->first.second, std::string() )  
         );

      delete I->second;
   }
}


//void test()
//{
//   CMorphoTypesSearchCreateManager::Create()->Construct( L"", 0 )->Clear();
//   CPartOfWordsSearchCreateManager::Create()->Construct( L"", 0 )->Clear();
//   CSourceToTypeCreateManager::Create()->Construct( 0, L"" )->Clear();
//
//
//   typedef LockedPtr<Core::CMorphoTypesSearch> TPointer;
//   typedef LockedPtr<const Core::CMorphoTypesSearch> TConstPointer;
//   
//   TPointer Pointer1, Pointer2;
//   TConstPointer ConstPointer1, ConstPointer2;
//
//   Pointer1 = Pointer2;
//   ConstPointer1 = ConstPointer2;
//   ConstPointer1 = Pointer1;
//   //Pointer1 = ConstPointer1; //так нельзя
//
//   TConstPointer ConstPointer3(Pointer1); 
//   //TPointer Pointer3(ConstPointer1); //так нельзя
//}

} //namespace Dictionary
} //namespace SS

///////////////////////////////////////////////////////////////////////////////