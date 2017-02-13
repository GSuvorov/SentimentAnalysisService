#pragma once

namespace SS{ 
namespace Dictionary{

///////////////////////////////////////////////////////////////////////////////
// Класс позволяет просматривать таблицу новых слов
///////////////////////////////////////////////////////////////////////////////
class CNewWordsListBasic
{
   typedef CMorphoTypesSearchCreateManager::TPointer TMorphoTypesSearchPtr;

   CTablePtr m_Table;
   TDictNumber m_DictNumber; 
   TMorphoTypesSearchPtr m_pMorphoTypesSearch;
   INT32 m_dbIDSource;
   std::wstring m_dbSource;
   std::wstring m_dbSourceLower;
   INT32 m_dbIDType;
   void *m_RecMap[4];

public:
   CNewWordsListBasic(): m_DictNumber(DATA_TYPE::NAMES::ednUnknown), m_dbIDSource(0), m_dbIDType(0) 
   {
      m_RecMap[0] = &m_dbIDSource;
      m_RecMap[1] = &m_dbSource;
      m_RecMap[2] = &m_dbSourceLower;
      m_RecMap[3] = &m_dbIDType;
   
   }
   void Init( DBMS::IDataBase *pNewWordsDataBase, DBMS::IDataBase *pMorphoDataBase, TDictNumber DictNumber, const TCHAR *TableName );
   
   //Закрыть открытые таблицы ранее деструктора
   void CloseTables() { m_Table.Close(); }

   /// вернет колличество слов данного типа
   unsigned int GetCount();
   
   /// перейдет к первому слову в списке (в порядке занесения).
   bool MoveFirst();
   
   /// перейдет к следующему слову в списке (в порядке занесения).
   /// если результат функции true следовательно слово есть, если false то слов больше нет
   bool MoveNext(TWordInfo &WordInfo);
   
   /// вернет текущее слово
   void GetWord(TWordInfo &WordInfo);
};

///////////////////////////////////////////////////////////////////////////////
// Реализация интерфейса IWordsList
// Чтение таблицы новых слов
///////////////////////////////////////////////////////////////////////////////
class CNewWordsList: public IWordsList
{
public:
   //Структура хранит инициализационные данные для инстанцирования отдельных объектов CNewWordsListBasic
   struct SInitStruct
   {
      //Данные для вызова CNewWordsList::Init
      DBMS::IDataBase *pNewWordsDataBase; 
      DBMS::IDataBase *pMorphoDataBase;
      TDictNumber CurInitDictNumber; 
      std::wstring TableName;   
   };

private:
   typedef CObjectsSelector<TDictNumber, CNewWordsListBasic> TNewWordsLists;

private:
   TNewWordsLists m_NewWordsLists;
   CNewWordsListBasic *m_pCurNewWordsList;

public:
   CNewWordsList(): m_pCurNewWordsList(0) {}

   //Создаёт и инициализивует внутренние объекты CNewWordsListBasic при помощи итераторов [First, Last) ссылающихся на структуры SInitStruct
   //Предыдущие объекты удаляются
   template<class InputIterator>
   void Init( InputIterator First, InputIterator Last )
   {
      TNewWordsLists::TAutoPtr AutoPtr;

      m_NewWordsLists.Clear();
      m_pCurNewWordsList = 0;

      for( ; First != Last; ++First )
      {
         AutoPtr.reset( new CNewWordsListBasic() );

         AutoPtr->Init(
            First->pNewWordsDataBase,
            First->pMorphoDataBase,
            First->CurInitDictNumber, 
            First->TableName.c_str() 
         );

         if( !m_NewWordsLists.Add(First->CurInitDictNumber, AutoPtr) )
         {
            m_NewWordsLists.Clear();
            SS_THROW( L"Ошибка CNewWordsList::Init" );
         }
      }
   }

   //Закрыть открытые таблицы ранее деструктора
   void CloseTables() 
   { 
      for( TNewWordsLists::TIterator I = m_NewWordsLists.Begin(); I != m_NewWordsLists.End(); ++I )
         I->CloseTables(); 
   }

   ///////////////////////////////////////////////////////////////////////////////
   // IWordsList

   /// выставляет номер словаря, из которого необходимо получать список слов
   void SetDictionaryNumber( TDictNumber eDictionariesNumber );
   
   /// вернет колличество слов данного типа
   unsigned int GetCount();
   
   /// перейдет к первому слову в списке (в порядке занесения).
   unsigned int MoveFirst();
   
   /// перейде к следующему слову в списке (в порядке занесения).
   /// если результат функции true следовательно слово есть, если false то слов больше нет
   unsigned int MoveNext( TWordInfo * pWordInfo );
   
   /// вернет текущее слово
   void GetWord( TWordInfo * pWordInfo );
};


} //namespace Dictionary
} //namespace SS
