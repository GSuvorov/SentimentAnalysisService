#pragma once

namespace SS{ 
namespace Dictionary{
namespace Edit
{

/*
   Позволяет модифицировать таблицу ProperSourceForm (NominalSourceForm)
   В методах в поле IDSource Имеется ввиду TDictionaryIndex::GetId()
*/
class CSource
{
private:
   typedef ClassLevelLockable<CSource> TMutex; //Блокирование таблицы при добавлении

private:
   DBMS::IDataBase *m_pDataBase;
   wchar_t m_wzSourceFormTableName[50];
   wchar_t m_wzSourceToMorphoTableName[50];
   wchar_t m_wzMorphoTypeToMorphoInfoTable[50];
private:
	mutable CTablePtr m_pSourceForm;
	mutable CTablePtr m_pSourceToMorpho;
	mutable CTablePtr m_pMorphoTypeToMorphoInfo;
	TDictNumber m_CurInitDictNumber; //Номер словаря для которого инициализированы таблицы
	TMutex m_Mutex;                  //Мютекс уровня класса. Необходим для корректного добавления, при условии что несколько объектов используют одну таблицу

public:
   typedef unsigned int TIDSource;
public:
	CSource(): m_CurInitDictNumber(DATA_TYPE::NAMES::ednUnknown), m_pDataBase(NULL) 
	{
		memset(m_wzSourceFormTableName, 0, sizeof(wchar_t) * 50);
		memset(m_wzSourceToMorphoTableName, 0, sizeof(wchar_t) * 50);
		memset(m_wzMorphoTypeToMorphoInfoTable, 0, sizeof(wchar_t) * 50);
	}
	
	//Инициализируем таблицы, вызывать обязательно перед остальными методами
   /*
      CurInitDictNumber - Номер словаря (значение будет учитываться в выходных данных)
      SourceFormTableName, SourceToMorphoTableName - Имена соотв таблиц
   */
	void Init( 
      DBMS::IDataBase *pDataBase, 
      TDictNumber CurInitDictNumber, 
      const wchar_t *SourceFormTableName, 
      const wchar_t *SourceToMorphoTableName 
   );

	/// возвращает всю доступную информацию об основе
	/**
		Алгоритм:
			По IDSource находим в таблицах ProperSourceForm (NominalSourceForm), 
			ProperSourceToMorpho (NominalSourceToMorphoTable), MorphoTypeToMorphoInfo данные о основе.
	*/
	bool Get( TIDSource IDSource, SSource &Source ) const;

	/// добавляет всю доступную информацию для основы
	/**
		Алгоритм:
			Находим в таблице ProperSourceForm (NominalSourceForm) количество строк и 
			записываем в IDSource. Добавляем в эту таблицу запись - поле PartOfWord и только что
			созданный индекс(IDSource) и инкрементируем количество строк в таблице.  Затем добавляем в таблицу 
         ProperSourceToMorpho (NominalSourceToMorpho) новый индекс IDSource и pSource->m_MorphoTypeInfo.IDType
	*/
	void Add( const SSource &Source, TIDSource &IDSource );

	/// удаляет всю информацию об основе
	/**
		Алгоритм:
			Удаляем из таблицы ProperSourceForm (NominalSourceForm), записи с IDSource . Затем из таблицы 
			ProperSourceToMorpho (NominalSourceToMorpho) все записи с IDSource 
	*/
	void Delete( TIDSource IDSource );

private:
	void Open() const;
	void Close() const;

};


///////////////////////////////////////////////////////////////////////////////
// Адаптирует CSource под интерфейс ISource
// Является компановщиком нескольких объектов CSource
///////////////////////////////////////////////////////////////////////////////
class CSourceAdapter: public ISource
{
public:
   //Структура хранит инициализационные данные для инстанцирования отдельных объектов CSource
   struct SInitStruct
   {
      //Данные для вызова CSource::Init
      DBMS::IDataBase *pDataBase; 
      TDictNumber CurInitDictNumber; 
      std::wstring SourceFormTableName; 
      std::wstring SourceToMorphoTableName; 
   };

private:
   typedef CObjectsSelector<TDictNumber, CSource> TSources;

private:
   TSources m_Sources;
   CSource *m_pCurSource;
   CSource::TIDSource m_CurIDSource;

public:
   CSourceAdapter(): m_pCurSource(0), m_CurIDSource(0) {}
   
   //Создаёт и инициализивует внутренние объекты CSource при помощи итераторов [First, Last) ссылающихся на структуры SInitStruct
   //Предыдущие объекты удаляются
   template<class InputIterator>
   void Init( InputIterator First, InputIterator Last )
   {
      TSources::TAutoPtr AutoPtr;

      m_Sources.Clear();
      m_pCurSource = 0;

      for( ; First != Last; ++First )
      {
         AutoPtr.reset( new CSource() );

         AutoPtr->Init(
            First->pDataBase,
            First->CurInitDictNumber, 
            First->SourceFormTableName.c_str(), 
            First->SourceToMorphoTableName.c_str() 
         );

         if( !m_Sources.Add(First->CurInitDictNumber, AutoPtr) )
         {
            m_Sources.Clear();
            SS_THROW( L"Ошибка CSourceAdapter::Init" );
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
   
   /// удаляет всю информацию об основе
   void Delete();
};

}
} //namespace Dictionary
} //namespace SS
