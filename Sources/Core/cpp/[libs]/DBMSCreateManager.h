#ifndef DBMSCreateManager_H
#define DBMSCreateManager_H

#include <map>
#include "StlAuxiliary.h"
#include "Threads.h"
#include "..\ASCInterface\IDBMS.h"

namespace SS { namespace Lib {

/**
   Управляет созданием и удалением объектов, реализуещих интерфейс IDBManager. 
   Singleton
   Позволяет получать объекты IDataBase
   Гарантирует что всегда для одной копии инициализационных параметров (путь к БД) будет 
   не более одного объекта реализуещего интерфейс IDBManager
   Не передаёт во вне IDBManager, только объекты IDataBase

   Ведёт счётчик ссылок объектов IDataBase прозрачно от пользователя и самостоятельно удаляет все объекты 
   реализуещие интерфейс IDBManager, как только будут освобождены все объекты типа IDataBase.

   Для того чтобы получить указатель на IDataBase необходимо вызвать 
      CDBMSCreateManager::Create()->OpenDataBase(...)
   
   А для освобождения базы (автоматически вызывается Close) 
      CDBMSCreateManager::Create()->CloseDataBase(...)
*/

class CDBMSCreateManager: public NonCopyable
{
private:
   CDBMSCreateManager() {} //Запрещаем инстанцирование из вне

private:              
   typedef std::wstring TMapKey;                          //Путь к базе
   typedef Interface::Core::DBMS::IDBManager IDBManager;  
   typedef Interface::Core::DBMS::IDataBase IDataBase;  
   typedef Interface::Core::ResourceManagers::ILoadManager ILoadManager;  

   class CRefCounter
   {
      size_t m_RefCount; //Количество ссылок на объёкт

   public:
      CRefCounter(): m_RefCount(0) {}
      size_t RefCount() const { return m_RefCount; }
      size_t IncRef(){ return ++m_RefCount; }
      size_t DecRef(){ APL_ASSERT(m_RefCount > 0); return --m_RefCount; }
   };

   class CManagerData: public CRefCounter
   {
      IDBManager *m_pDBManager;    //Сам сохранённый менеджер

   public:
      CManagerData( IDBManager *pDBManager = 0 ): m_pDBManager(pDBManager) {}
      IDBManager *GetDBManager() const { return m_pDBManager; }
   };

   typedef std::map<TMapKey, CManagerData> TObjectsMap;

   class CDataBaseData: public CRefCounter
   {
      typedef TObjectsMap::iterator TIter;
      TIter m_Iter;   //Сохранённый итератор 

   public:
      CDataBaseData( TIter Iter = TIter() ): m_Iter(Iter) {}
      TIter GetIter() const { return m_Iter; }

      friend inline bool operator==( const CDataBaseData &ob1, const CDataBaseData &ob2 )
      {
         return ob1.GetIter() == ob2.GetIter();
      }
   };

   typedef std::map<IDataBase*, CDataBaseData> TIDataBase2ObjectsMapIter;  //Открытые базы у менеджеров

   typedef ClassLevelLockable<CDBMSCreateManager> TMutex;

public:
   typedef IDataBase *TPointer;
   typedef const IDataBase *TConstPointer;
   typedef CDBMSCreateManager ThisType;

private:
   //Будем вести два списка у каждого элемента из них будет свой счётчик ссылкок.
   TObjectsMap m_ObjectsMap;
   TIDataBase2ObjectsMapIter m_IDataBase2ObjectsMapIter;
   static TMutex m_Mutex;
   static ThisType *pInstance_;

   static ThisType *CreateImpl()
   {
      static ThisType Obj;
      return &Obj;
   }


public:
   //Получить указатель на CDBMSCreateManager
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

   // Открыть БД IDataBase, при этом если ещё не создан, создаётся IDBManager
   // pLoadManager - Используется для создания IDBManager (Методом Create)
   // FullPathToIniFile - Путь к ini-файлу БД
   // FullPathToDbmsBases - Путь к базам данных
   // DBName - Имя  БД
   TPointer OpenDataBase( 
      ILoadManager *pLoadManager,
      const std::wstring &FullPathToIniFile, 
      const std::wstring &FullPathToDbmsBases, 
      const std::wstring &DBName 
   );

   //Закрыть БД (автоматически выполняется Close), если нет больше открытых ссылок на IDBManager
   //то он удаляется
   void CloseDataBase(TPointer pDB);

   ~CDBMSCreateManager();
};


}} //namespace SS::Lib 

#endif //DBMSCreateManager_H
