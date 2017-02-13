#pragma once

namespace SS{ 
namespace Dictionary{


class CAbbreviationsBasic
{
protected:
   CAbbreviationsBasic():  m_DictIndex( DATA_TYPE::NAMES::ednUnknown ) {}

   TDictNumber m_DictIndex;

   void FillSWordInfo( 
      const std::wstring &wcWord,
      unsigned int IDSource,
      const TIDForm &IDForm,
      const TIDType &IDType,
      const TSpecialMorpho &SpecialMorpho,
      const TMorphoInfo &MorphoInfo,
      SWordInfo &WordInfo,
      bool FirstLetterUpperCase
   ) const
   {
      WordInfo.m_wWord = wcWord;
      WordInfo.m_SpecialMorpho = SpecialMorpho;
      WordInfo.m_WordIndex.m_DictIndex.SetId(IDSource) ; 
      WordInfo.m_WordIndex.m_DictIndex.SetDictionaryNumber(m_DictIndex); 
      WordInfo.m_WordIndex.m_DictIndex.SetFirstLetterUpperCase(FirstLetterUpperCase); 
      WordInfo.m_WordIndex.m_IDForm = IDForm; 
      WordInfo.m_MorphoTypeInfo.m_IDType = IDType;
      WordInfo.m_MorphoTypeInfo.m_MorphoInfo = MorphoInfo;
   }
};
///////////////////////////////////////////////////////////////////////////////   
   
/** 
   Работает с таблицей абревиатур со след. структурой:
      0  DictionaryIndex     : INT32   - COMMON KEY-FIELD
      1  FormLower           : WSTRING - COMMON KEY-FIELD
      2  Form                : WSTRING - SEARCH IS NOT AVAILABLE
      3  IDType              : INT32   - DATA (search is not available)
      4  IDForm              : INT32   - DATA (search is not available)

   Поиск осуществляется по полю FormLower, но возвращаются значения из Form
   При этом необходимо чтобы все входные строки для поиска были приведены к нижнему регистру.

   IDForm в таблице абревиатур не есть IDForm из MorphoTypes и при поиске в MorphoTypes IDForm всегда равен 0 
*/

class CAbbreviationsDictionaryBase: public CAbbreviationsBasic
{
   typedef std::list<TWordInfo> TWIList;
   typedef CMorphoTypesSearchCreateManager::TPointer TMorphoTypesSearchPointer;

private:
    CTablePtr m_pTable;
    TMorphoTypesSearchPointer m_pMorphoTypesSearch;

public:
   CAbbreviationsDictionaryBase(): m_pMorphoTypesSearch(0) {}
     
   //Инициализиреут пути и настраивает объекты
   /// pMorphoDataBase, pAbbreviationsDataBase - Используемые БД
	/// TableName  -   Имя таблицы   
	/// DictionaryIndex - Индекc словаря для представления в TDictionaryIndex
	void Init(
		DBMS::IDataBase *pMorphoDataBase, DBMS::IDataBase *pAbbreviationsDataBase, const TCHAR *TableName, 
		TDictNumber DictionaryIndex 
	);
	
   /// Перезагружает данные необходимо чтобы Init был вызван
   bool Reload() { /*Ничего делать не надо*/ return true; }

	///////////////////////////////////////////////////////////////////////////////
	//IMorphologyAnalyser

	/// проверка наличия слова в словаре
	bool IsExistWord(/*in*/const wchar_t* wcWord);
	
	/// поиск полной морфологической информации по слову 
	bool GetWordInfo/*ByWord*/(
		/*in*/const wchar_t* wcWord,
      /*in*/const wchar_t* wcWordOriginalCase,
      /*in*/bool IsFirstLetterUpperCase,
		/*out*/std::list<TWordInfo>* p_l_WordInfo
	);

	/// поиск полной морфологической информации по индексу слова
	bool GetWordInfo/*ByWordIndex*/(
		/*in*/SWordIndex* pWordIndex, 
		/*out*/SWordInfo* pWordInfo
	);


	/// поиск слова по его индексу 
	/// wcWord - массив с количеством элементов MAX_CHARS_IN_WORD
    bool GetWord(
		/*in*/SWordIndex* pWordIndex,
		/*out*/wchar_t* wcWord
	);

	/// поиск словоформ слова по индексу основы
    bool GetWordFormsByIDSource(
		/*in*/TDictionaryIndex* pDictionaryIndex,
		/*out*/std::list<TWordInfo>* p_l_WordInfo
	);
};

/**
  Работает так же как и CAbbreviationsDictionaryBase, но при инициализации загружает 
  все данные из таблицы в память.

  Вызов константных методов безопасен в многопоточной среде
*/
class CAbbreviationsDictionaryBaseMemory: public CAbbreviationsBasic
{
   typedef std::list<TWordInfo> TWIList;
   typedef CMorphoTypesSearchCreateManager::TPointer TMorphoTypesSearchPointer;

   //Строка в таблице
   struct SRecord
   {
      INT32 IDSource;
      std::wstring FormLower;
      std::wstring Form;           
      INT32  IDType;             
      INT32  IDForm; 
   };

   //Представление в памяти таблицы для поиска по FormLower и DictionaryIndex
   typedef stdext::hash_multimap< std::wstring, SRecord > TFormLowerToRecord;
   typedef stdext::hash_multimap< INT32, const SRecord * > TDictionaryIndexToRecordPtr;
   //typedef std::multimap< std::wstring, SRecord > TFormLowerToRecord;
   //typedef std::multimap< INT32, const SRecord * > TDictionaryIndexToRecordPtr;

   typedef std::pair<TFormLowerToRecord::const_iterator, TFormLowerToRecord::const_iterator> TFormToRecordIterPair;
   typedef std::pair<TDictionaryIndexToRecordPtr::const_iterator, TDictionaryIndexToRecordPtr::const_iterator> TDIToRecordPtrIterPair;

private:
   TMorphoTypesSearchPointer m_pMorphoTypesSearch;
   TFormLowerToRecord m_FormLowerToRecord;
   TDictionaryIndexToRecordPtr m_DictionaryIndexToRecordPtr;

   DBMS::IDataBase *m_pMorphoDataBase;
   DBMS::IDataBase *m_pAbbreviationsDataBase;
   std::basic_string<TCHAR> m_TableName;

public:
   CAbbreviationsDictionaryBaseMemory(): m_pMorphoTypesSearch(0), m_pMorphoDataBase(0), m_pAbbreviationsDataBase(0) {}

   /// Инициализиреут пути и настраивает объекты и загружает данные
   /// pMorphoDataBase, pAbbreviationsDataBase - Используемые БД
   /// TableName  -   Имя таблицы   
   /// DictionaryIndex - Индекc словаря для представления в TDictionaryIndex
   bool Init(
      DBMS::IDataBase *pMorphoDataBase, DBMS::IDataBase *pAbbreviationsDataBase, const TCHAR *TableName, 
      TDictNumber DictionaryIndex 
      );

   /// Перезагружает данные необходимо чтобы Init был вызван
   bool Reload();

   ///////////////////////////////////////////////////////////////////////////////
   //IMorphologyAnalyser

   /// проверка наличия слова в словаре
   bool IsExistWord(/*in*/const wchar_t* wcWord) const;

   /// поиск полной морфологической информации по слову 
   bool GetWordInfo/*ByWord*/(
      /*in*/const wchar_t* wcWord, 
      /*in*/const wchar_t* wcWordOriginalCase,
      /*in*/bool IsFirstLetterUpperCase,
      /*out*/TWIList* p_l_WordInfo
   ) const;

   /// поиск полной морфологической информации по индексу слова
   bool GetWordInfo/*ByWordIndex*/(
      /*in*/SWordIndex* pWordIndex, 
      /*out*/SWordInfo* pWordInfo
   ) const;


   /// поиск слова по его индексу 
   /// wcWord - массив с количеством элементов MAX_CHARS_IN_WORD
   bool GetWord(
      /*in*/SWordIndex* pWordIndex,
      /*out*/wchar_t* wcWord
   ) const;

   /// поиск словоформ слова по индексу основы
   bool GetWordFormsByIDSource(
      /*in*/TDictionaryIndex* pDictionaryIndex,
      /*out*/TWIList* p_l_WordInfo
   ) const;
};


///////////////////////////////////////////////////////////////////////////////
// Адаптирует интерфейс IBasicMorphologyAnalyser для поиска в двух таблицах
// CAbbreviationsDictionaryBase
///////////////////////////////////////////////////////////////////////////////
class CAbbreviationsDictionary: public Detail::IBasicMorphologyAnalyser
{
   //typedef CObjectsSelector<TDictNumber, CAbbreviationsDictionaryBase> TDictNumberToAbbreviation;
   typedef AssocVector<TDictNumber, CAbbreviationsDictionaryBaseMemoryManager::TPointer> TDictNumberToAbbreviation;

private:
   TDictNumberToAbbreviation m_DictNumberToAbbreviation;

private:
   CAbbreviationsDictionaryBaseMemoryManager::TConstPointer Get( TDictNumber DictNumber ) const 
   {
      //CAbbreviationsDictionaryBase *pADB = m_DictNumberToAbbreviation.Get( DictNumber );

      //if( pADB == 0 )
      //   SS_THROW( L"Ошибка при поиске зарегистрированного DictionaryNumber" );

      //return pADB;
      TDictNumberToAbbreviation::const_iterator I = m_DictNumberToAbbreviation.find(DictNumber);

      if( I == m_DictNumberToAbbreviation.end() )
         SS_THROW( L"Ошибка при поиске зарегистрированного DictionaryNumber" );

      return  I->second;
   }

public:
   struct SInitStruct
   {
      template<class Iterator>
      struct SSelectDictNumber: public SelectorHelper<Iterator, const TDictNumber>
      {
         reference operator ()( origin_iterator_reference V ) const { return V.DictNumber; }
      };

      TDictNumber DictNumber;      //Номер словаря
      std::wstring TableName;      //Соответствующий номер таблицы
   };

public:
   //Создаёт и инициализивует внутренние объекты CAbbreviationsDictionary
   //Инициализиреут пути и настраивает объекты
   /// pMorphoDataBase, pAbbreviationsDataBase - Используемые БД
   /// [First, Last) - Интервал итераторов для структуры SInitStruct
   template<class InputIterator>
   void Init(
      DBMS::IDataBase *pMorphoDataBase, DBMS::IDataBase *pAbbreviationsDataBase, 
      InputIterator First, InputIterator Last
   )
   {
      //TDictNumberToAbbreviation::TAutoPtr AutoPtr;

      m_DictNumberToAbbreviation.clear();

      for( ; First != Last; ++First )
      {
         //AutoPtr.reset( new CAbbreviationsDictionaryBase() );

         //AutoPtr->Init(
         //   pMorphoDataBase,
         //   pAbbreviationsDataBase, 
         //   First->TableName.c_str(), 
         //   First->DictNumber 
         //   );

         //if( !m_DictNumberToAbbreviation.Add(First->DictNumber, AutoPtr) )
         //{
         //   m_DictNumberToAbbreviation.Clear();
         //   SS_THROW( L"Ошибка CAbbreviationsDictionary::Init" );
         //}

         m_DictNumberToAbbreviation.insert( TDictNumberToAbbreviation::value_type
            ( 
               First->DictNumber,
               CAbbreviationsDictionaryBaseMemoryManager::Create()->Construct(
                  pMorphoDataBase,
                  pAbbreviationsDataBase, 
                  First->TableName.c_str(), 
                  First->DictNumber 
               )
            )
         );
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   //IMorphologyAnalyser

   /// Перезагружает данные из БД
   bool Reload();


   /// проверка наличия слова в словаре
   bool IsExistWord(/*in*/const wchar_t* wcWord);

   /// поиск полной морфологической информации по слову 
   bool GetWordInfo/*ByWord*/(
      /*in*/const wchar_t* wcWord,
      /*in*/const wchar_t* wcWordOriginalCase,
      /*in*/bool IsFirstLetterUpperCase,
      /*out*/std::list<TWordInfo>* p_l_WordInfo
      );

   /// поиск полной морфологической информации по индексу слова
   bool GetWordInfo/*ByWordIndex*/(
      /*in*/SWordIndex* pWordIndex, 
      /*out*/SWordInfo* pWordInfo
      );


   /// поиск слова по его индексу 
   /// wcWord - массив с количеством элементов MAX_CHARS_IN_WORD
   bool GetWord(
      /*in*/SWordIndex* pWordIndex,
      /*out*/wchar_t* wcWord
      );

   /// поиск основы слова слова (части слова без оканчания) по его индексу 
   /// wcWord - массив с количеством элементов MAX_CHARS_IN_WORD
   bool GetWordSource(
      /*in*/const TDictionaryIndex &DictIndex,
      /*out*/wchar_t* wcWordSource
   );									   

   /// поиск словоформ слова по индексу основы
   bool GetWordFormsByIDSource(
      /*in*/TDictionaryIndex* pDictionaryIndex,
      /*out*/std::list<TWordInfo>* p_l_WordInfo
   );
};

} //namespace Dictionary
} //namespace SS
