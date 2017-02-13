#pragma once

namespace SS{ 
namespace Dictionary{
namespace Edit{


struct IOneRootBasic
{
   typedef std::list<TDictionaryIndex> TDIList;
   typedef unsigned int TIDSource;

   virtual ~IOneRootBasic() {}

   /// возвращает список однокоренных слов для текущего индекса основы
   virtual void Get( TIDSource IDSource, TDIList &SourceList ) const = 0;

   /// добавляет однокоренные слова к текущему
   virtual void Add( TIDSource IDSource, const TDIList &SourceList )
   {  APL_THROW( _T("Для данного типа словаря не поддерживается добавление однокоренных слов") ); }

   /// удаляет однокоренные слова у текущего слова
   virtual void Delete( TIDSource IDSource, const TDIList &SourceList )
   {  APL_THROW( _T("Для данного типа словаря не поддерживается удаление однокоренных слов") ); } 
};


/*
   Простое конвертирование DictionariesNumber
*/
class COneRootSimpleConvert: public IOneRootBasic
{
   DATA_TYPE::NAMES::DictionariesNumber m_DictNumber;

public:
   COneRootSimpleConvert( DATA_TYPE::NAMES::DictionariesNumber DictNumber ): m_DictNumber(DictNumber) {} 
   
   void Get( TIDSource IDSource, TDIList &SourceList ) const;
};

/*
   Конвертирование для римских чисел
*/
class CArabic2RomanConvert: public IOneRootBasic
{

public:
   void Get( TIDSource IDSource, TDIList &SourceList ) const;
};

/*
   Конвертирование символов, например такие символы однокоренные " “ ”« »
*/
class CSymbolsOneRoot: public IOneRootBasic
{
public:
   typedef std::wstring TSymbols;

private:
   typedef AssocVector<TSymbols::value_type, TSymbols> TStorage;

private:
   TStorage m_Storage;

private:
   void AddData( const TSymbols &Symbols );

public:
   CSymbolsOneRoot();
   
   void Get( TIDSource IDSource, TDIList &SourceList ) const;
};


/*
   Работа с однокоренными словами
*/
class COneRoot: public IOneRootBasic
{
	wchar_t m_wzOneRootTableName[50];
	DBMS::IDataBase *m_pDataBase;
	mutable CTablePtr m_pTable;
	TDictNumber m_CurInitDictNumber; //Номер словаря для которого инициализированы таблицы

private:
	void Open() const;
	void Close() const;
	//Открывает необходдимую таблицу при условии что изменился текущий DictionaryIndex
	void InitTable();

public:
	COneRoot() : m_CurInitDictNumber(TDictNumber::ednUnknown), m_pDataBase(NULL) 
	{
		memset(m_wzOneRootTableName, 0, sizeof(wchar_t) * 50);
	}

	/// Инициализируем таблицу
   /**
      CurInitDictNumber - Номер словаря (значение будет учитываться в выходных данных)
      OneRootTableName - Имя таблицы в БД
   */
   void Init( DBMS::IDataBase *pDataBase, TDictNumber CurInitDictNumber, const wchar_t *OneRootTableName );
	
	/// возвращает список однокоренных слов для текущего индекса основы
	/**
		Алгоритм:
			Находит в таблице NominalOneRoot все записи с IDSource
	*/
	void Get( TIDSource IDSource, TDIList &SourceList ) const;

	/// добавляет однокоренные слова к текущему
	/**
		Алгоритм:
			Добавляет в таблицу NominalOneRoot все записи с IDSource  и IDOneRoot == значению в списке
	*/
	void Add( TIDSource IDSource, const TDIList &SourceList );
	
	/// удаляет однокоренные слова у текущего слова
	/**
		Алгоритм:
			Удаляет из таблицы NominalOneRoot все записи с IDSource  и IDOneRoot == значению в списке
	*/
	void Delete( TIDSource IDSource, const TDIList &SourceList );
};

///////////////////////////////////////////////////////////////////////////////
// Адаптирует COneRoot под интерфейс IOneRoot
// Является компановщиком нескольких объектов COneRoot
///////////////////////////////////////////////////////////////////////////////
class COneRootAdapter: public IOneRoot
{
public:
   //Структура хранит инициализационные данные для инстанцирования отдельных объектов COneRoot
   struct SInitStruct
   {
      //Данные для вызова COneRoot::Init
      DBMS::IDataBase *pDataBase;
      TDictNumber CurInitDictNumber; 
      std::wstring OneRootTableName;   
   };

private:
   typedef CObjectsSelector<TDictNumber, IOneRootBasic> TOneRoots;

private:
   TOneRoots m_OneRoots;
   IOneRootBasic *m_pCurOneRoot;
   IOneRootBasic::TIDSource m_CurIDSource;

public:
   COneRootAdapter(): m_pCurOneRoot(0), m_CurIDSource(0) {}

   //Создаёт и инициализивует внутренние объекты COneRoot при помощи итераторов [First, Last) ссылающихся на структуры SInitStruct
   //Предыдущие объекты удаляются
   template<class InputIterator>
   void Init( InputIterator First, InputIterator Last )
   {
      TOneRoots::TAutoPtr AutoPtr;
      CObjectsSelector<TDictNumber, COneRoot>::TAutoPtr AutoPtrTmp;

      m_OneRoots.Clear();
      m_pCurOneRoot = 0;

      for( ; First != Last; ++First )
      {
         AutoPtrTmp.reset( new COneRoot() );

         AutoPtrTmp->Init(
            First->pDataBase,
            First->CurInitDictNumber, 
            First->OneRootTableName.c_str() 
         );

         AutoPtr = AutoPtrTmp;

         if( !m_OneRoots.Add(First->CurInitDictNumber, AutoPtr) )
         {
            m_OneRoots.Clear();
            SS_THROW( L"Ошибка COneRootAdapter::Init" );
         }
      }

      AutoPtr.reset( new COneRootSimpleConvert(DATA_TYPE::NAMES::ednNumeric) );

      if( !m_OneRoots.Add(DATA_TYPE::NAMES::ednRomanNumeric, AutoPtr) )
      {
         m_OneRoots.Clear();
         SS_THROW( L"Ошибка COneRootAdapter::Init" );
      }

      AutoPtr.reset( new CArabic2RomanConvert );

      if( !m_OneRoots.Add(DATA_TYPE::NAMES::ednNumeric, AutoPtr) )
      {
         m_OneRoots.Clear();
         SS_THROW( L"Ошибка COneRootAdapter::Init" );
      }

      AutoPtr.reset( new CSymbolsOneRoot );  
      if( !m_OneRoots.Add(DATA_TYPE::NAMES::ednSymbols, AutoPtr) )
      {
         m_OneRoots.Clear();
         SS_THROW( L"Ошибка COneRootAdapter::Init" );
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   // IOneRoot
   
   /// устанавливает индекс слова
   unsigned int SetDictionaryIndex(SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex);
  
   /// возвращает список однокоренных слов для текущего индекса основы
   void Get(std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_IDSource);
   
   /// добавляет однокоренные слова к текущему
   void Add(std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_IDSource);
   
   /// удаляет однокоренные слова у текущего слова
   void Delete(std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_IDSource);
};

} //namespace Edit
} //namespace Dictionary
} //namespace SS
