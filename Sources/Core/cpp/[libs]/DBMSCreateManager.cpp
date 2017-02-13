#include "stdafx.h"
#include "DBMSCreateManager.h"
#include "Compose.hpp"

namespace SS { namespace Lib {

CDBMSCreateManager::TMutex CDBMSCreateManager::m_Mutex;
CDBMSCreateManager::ThisType *CDBMSCreateManager::pInstance_ = 0;

///////////////////////////////////////////////////////////////////////////////

CDBMSCreateManager::TPointer CDBMSCreateManager::OpenDataBase( 
   ILoadManager *pLoadManager,
   const std::wstring &FullPathToIniFile, 
   const std::wstring &FullPathToDbmsBases, 
   const std::wstring &DBName 
)
{
   APL_ASSERT_PTR( pLoadManager );
   TMutex::Lock Guard(m_Mutex);

   DEBUG_MSG_LOG4( 
      "Открываем DB LoadManager: " << pLoadManager << 
      " FullPathToIniFile: " << Convert( FullPathToIniFile, std::string() ) << 
      " FullPathToDbmsBases: " << Convert( FullPathToDbmsBases, std::string() ) <<
      " DBName: " << Convert( DBName, std::string() ) 
   ); 

   TMapKey Key(FullPathToDbmsBases);   
   TObjectsMap::iterator I = m_ObjectsMap.find(Key);

   //Получаем если необходимо IDBManager
   if( I == m_ObjectsMap.end() )
   {
      DEBUG_MSG_LOG2( "Создаём DBMS::IDBManager процесс: "  << GetCurrentProcessId() << 
         " поток: " << GetCurrentThreadId() << 
         " FullPathToIniFile: " << Convert( FullPathToIniFile, std::string() ) <<
         " FullPathToDbmsBases: " << Convert( FullPathToDbmsBases, std::string() ) 
      );

      IDBManager *pDBManager = 0;
      pLoadManager->Create(L"./core/linguistics/DBMS.dll", CLSID_DBMS, IID_DBManager, reinterpret_cast<void**>(&pDBManager));

      auto_ptr_ex<IDBManager, auto_ptr_ex_release_strategy> pNewObj( pDBManager );

      if( pNewObj == 0 )
         SS_THROW( L"Ошибка при получении IID_DBManager" );

      
      if( pNewObj->Init(FullPathToIniFile.c_str(), FullPathToDbmsBases.c_str()) != S_OK )
         SS_THROW( L"Ошибка при инициализации IDBManager" );

      std::pair<TObjectsMap::iterator, bool> RezPair = m_ObjectsMap.insert( TObjectsMap::value_type(Key, pNewObj.get()) );
      pNewObj.release();

      APL_ASSERT( RezPair.second );
      I = RezPair.first;
   }

   //Получаем IDataBase
   if( I->second.GetDBManager()->MoveToDataBase(DBName.c_str()) != S_OK )
      SS_THROW( L"Ошибка при попытке перейти к БД" );

   IDataBase *pDataBase = I->second.GetDBManager()->OpenDataBase();
   APL_ASSERT_PTR( pDataBase );
   I->second.IncRef(); //У IDBManager стало на одну ссылку больше

   m_IDataBase2ObjectsMapIter.insert(TIDataBase2ObjectsMapIter::value_type(pDataBase, I)).first->second.IncRef(); //Полученных БД, так же стало на одну больше (или появилась новая)

   return pDataBase;
}

///////////////////////////////////////////////////////////////////////////////

void CDBMSCreateManager::CloseDataBase(TPointer pDB)
{  
   APL_ASSERT_PTR(pDB);
   TMutex::Lock Guard(m_Mutex);

   TIDataBase2ObjectsMapIter::iterator IAux = m_IDataBase2ObjectsMapIter.find(pDB);

   if( IAux == m_IDataBase2ObjectsMapIter.end() )
      SS_THROW( L"Указатель на DB не содержит корректных данных" );
   
   TObjectsMap::iterator IMain = IAux->second.GetIter();
   
   DEBUG_MSG_LOG4( 
      "Закрываем DB Name: " << Convert( pDB->GetName(), std::string() )  << 
      " FullPathToDbmsBases: " << Convert( IMain->first, std::string() ) 
   ); 

   pDB->Close();

   //Удаляем если необходимо элемент в списке m_IDataBase2ObjectsMapIter
   IAux->second.DecRef();

   if( IAux->second.RefCount() == 0 )
      m_IDataBase2ObjectsMapIter.erase(IAux);

   //Удаляем если необходимо элемент в списке m_ObjectsMap
   IMain->second.DecRef();

   if( IMain->second.RefCount() == 0 )
   {
      //Не должно быть ссылкок на данный IDBManager
      APL_ASSERT( 
         std::find_if( m_IDataBase2ObjectsMapIter.begin(), m_IDataBase2ObjectsMapIter.end(),
            bind2nd(
               binary_compose_1arg( 
                  std::equal_to<CDataBaseData>(), 
                  select_second_t<TIDataBase2ObjectsMapIter::value_type>(),
                  select_dummy_t<CDataBaseData>()
               ),
               IMain
            )
         ) == m_IDataBase2ObjectsMapIter.end()  
      );

      DEBUG_MSG_LOG2( "Удаляем DBMS::IDBManager процесс: "  << GetCurrentProcessId() << 
         " поток: " << GetCurrentThreadId() << 
         " FullPathToDbmsBases: " << Convert( IMain->first, std::string() )
      );

      IMain->second.GetDBManager()->Release();
      m_ObjectsMap.erase(IMain);
   }
}
///////////////////////////////////////////////////////////////////////////////

CDBMSCreateManager::~CDBMSCreateManager()
{
   TMutex::Lock Guard(m_Mutex); //В принципе не нужен

   //Сообщаем о том что есть открытые базы, хотя их быть не должно быть
   for( TObjectsMap::iterator I = m_ObjectsMap.begin(); I != m_ObjectsMap.end(); ++I )
   {
      std::wstringstream OutStr;
      std::wstring Str;

      OutStr << L"Не все базы данных были закрыты, по пути: " << I->first << L". Пытаемся закрыть автоматически...";
      Str = OutStr.str();
      DEBUG_MSG_LOG2( Convert( Str, std::string() ) );
      SAVE_LOG( (wchar_t *)Str.c_str() );
   }

   //Попытаемся закрыть всё что было открыто
   TIDataBase2ObjectsMapIter::iterator IAux = m_IDataBase2ObjectsMapIter.begin();
   TIDataBase2ObjectsMapIter::iterator ICur;
   size_t CurRefCount;
   
   while( IAux != m_IDataBase2ObjectsMapIter.end() )
   {
      ICur = IAux++;
      
      CurRefCount = ICur->second.RefCount();
      
      DEBUG_MSG_LOG2( "Закрываем принудительно БД: " << Convert( ICur->first->GetName(), std::string() ) << ", " << (unsigned int)CurRefCount << " раз" );

      while( CurRefCount-- )
         CloseDataBase( ICur->first );
   }

   APL_ASSERT( m_ObjectsMap.empty() );
   APL_ASSERT( m_IDataBase2ObjectsMapIter.empty() );
}

}} //namespace SS::Lib 

