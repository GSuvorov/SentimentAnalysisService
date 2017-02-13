#pragma once

namespace SS{ 
namespace Dictionary{
namespace Edit{

/*
   Позволяет модифицировать таблицы Аббревиатур со структурой:
      #DATA_FIELD_INFO = "DictionaryIndex" : INT32              // Номер словаря и индекс основы/слова
      #DATA_FIELD_INFO = "FormLower"       : WSTRING            // Литеральная фрма в нижнем регистре
      #DATA_FIELD_INFO = "Form"            : WSTRING - NO_FIND  // Литеральная форма
      #DATA_FIELD_INFO = "IDType"          : INT32   - IS_DATA  // Ссылка на морфо-тип
      #DATA_FIELD_INFO = "IDForm"          : INT32   - IS_DATA  // Номер морфо-формы
*/
class CAbbreviationSource
{
private:
	typedef ClassLevelLockable<CAbbreviationSource> TMutex; //Блокирование таблицы при добавлении
private:
	DBMS::IDataBase *m_pDataBase;
	DBMS::IDataBase *m_pMorphoTypeToMorphoInfoDataBase;
	mutable CTablePtr m_pAbbreviation;
	mutable CTablePtr m_pMorphoTypeToMorphoInfo;
	wchar_t m_wzAbbreviationTableName[50];
	wchar_t m_szMorphoTypeToMorphoInfoTable[50];
	TDictNumber m_CurInitDictNumber; //Номер словаря для которого инициализированы таблицы
	TMutex m_Mutex;                  //Мютекс уровня класса. Необходим для корректного добавления, при условии что несколько объектов используют одну таблицу
public:
	typedef unsigned int TIDSource;
private:
	void Open() const;
	void Close() const;
public:
	CAbbreviationSource(); 
	//Инициализируем таблицы, вызывать обязательно перед остальными методами
	/*
		CurInitDictNumber - Номер словаря (значение будет учитываться в выходных данных)
		SourceFormTableName - Имена соотв таблиц
	*/
	void Init( 
		DBMS::IDataBase *pDataBase, 
		DBMS::IDataBase *pMorphoTypeToMorphoInfoDataBase, 
		TDictNumber CurInitDictNumber, 
		const wchar_t *AbbreviationTableName 
	);

	/// возвращает всю доступную информацию об основе
	/**
		Алгоритм:
			По IDSource находим в таблицах Abbreviation, MorphoTypeToMorphoInfo данные о основе.
	*/
	bool Get( TIDSource IDSource, SSource &Source ) const;

	/// добавляет всю доступную информацию для основы
	/**
		Алгоритм:
			Находим в таблице Abbreviation количество строк и 
			записываем в IDSource. Добавляем в эту таблицу запись - поля FormLower, Form, IDType и 
			созданный индекс(IDSource) и инкрементируем количество строк в таблице.
	*/
	void Add( const SSource &Source, TIDSource &IDSource );

	/// Обновляем информацию хранящиюся по индексу IDSource (или добавляем новую если такой информации нет)
	/**
		Алгоритм:
			Удаляем в таблице Abbreviation строки с IDSource и добавляем новую запись
			с IDSource и информацией из Source
	*/
	void Update( TIDSource IDSource, const SSource &Source );


	/// удаляет всю информацию об основе
	/**
		Алгоритм:
			Удаляем из таблицы Abbreviation, записи с IDSource .
	*/
	void Delete( TIDSource IDSource );
};


///////////////////////////////////////////////////////////////////////////////
// Адаптирует CSource под интерфейс ISource
// Является компановщиком нескольких объектов CSource
///////////////////////////////////////////////////////////////////////////////
class CAbbreviationSourceAdatper: public ISourceEx
{
public:
   //Структура хранит инициализационные данные для инстанцирования отдельных объектов CSource
   struct SInitStruct
   {
      //Данные для вызова CSource::Init
      DBMS::IDataBase *pDataBase; 
      DBMS::IDataBase *pMorphoTypeToMorphoInfoDataBase;
      TDictNumber CurInitDictNumber; 
      std::wstring AbbreviationTableName; 
   };

private:
   typedef CObjectsSelector<TDictNumber, CAbbreviationSource> TAbbreviationSources;

private:
   TAbbreviationSources m_Abbreviation;
   CAbbreviationSource *m_pCurAbbreviation;
   CAbbreviationSource::TIDSource m_CurIDSource;

public:
   CAbbreviationSourceAdatper(): m_pCurAbbreviation(0), m_CurIDSource(0) {}
   
   //Создаёт и инициализивует внутренние объекты CSource при помощи итераторов [First, Last) ссылающихся на структуры SInitStruct
   //Предыдущие объекты удаляются
   template<class InputIterator>
   void Init( InputIterator First, InputIterator Last )
   {
      TAbbreviationSources::TAutoPtr AutoPtr;

      m_Abbreviation.Clear();
      m_pCurAbbreviation = 0;

      for( ; First != Last; ++First )
      {
         AutoPtr.reset( new CAbbreviationSource() );

         AutoPtr->Init(
            First->pDataBase,
            First->pMorphoTypeToMorphoInfoDataBase,
            First->CurInitDictNumber, 
            First->AbbreviationTableName.c_str() 
         );

         if( !m_Abbreviation.Add(First->CurInitDictNumber, AutoPtr) )
         {
            m_Abbreviation.Clear();
            SS_THROW( L"Ошибка CAbbreviationSourceAdatper::Init" );
         }
      }
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   // ISource
   /// устанавливает индекс слова
   unsigned int SetDictionaryIndex(SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex);
   
   /// возвращает всю доступную информацию об основе
   unsigned int Get(SS::Dictionary::Types::SSource* pSource);
   
   /// Добавлят всю доступную информацию для основы
   void Add(SS::Dictionary::Types::SSource* pSource);
   
   /// Изменяет информацию пл теущему IDSource
   void Update(SS::Dictionary::Types::SSource* pSource);
   
   /// удаляет всю информацию об основе
   void Delete();
};

} //namespace Edit
} //namespace Dictionary
} //namespace SS
