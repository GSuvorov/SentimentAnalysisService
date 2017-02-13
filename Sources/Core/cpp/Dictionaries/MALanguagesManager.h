#pragma once

namespace SS{ 
namespace Dictionary{


///////////////////////////////////////////////////////////////////////////////
// Класс агрегирует все словари в себе (вирутуальные и обычные) и предоставляет 
// при помощи интерфейса IMorphologyAnalyser доступ к ним как к одному целому.
// Поддерживается рейтинг популярности языков (сколько слов было найденно в словарях
// определённого языка), благодоря которому словари с большим рейтингом имеют 
// больший приоритет при поиске (поиск начинается именно с них)
///////////////////////////////////////////////////////////////////////////////
class CMALanguagesManager: public IMorphologyAnalyser, public ILoad
{
public: 
   typedef unsigned int TIDSource;
   typedef std::list<SS::Dictionary::Types::TWordInfo> TWIList;

   //Инициализация для языков
   struct SLanguage 
   {      
      //Иницализационные данные для CSmartDictionary
      struct SSmartDictionaryInitData
      {
         std::wstring DBPath; 
         
         enum eDictNumbers
         {
            EDNNominal,
            EDNProper,
            EDNNewWords,

            EDNCOUNT
         };

         TDictNumber DictionaryIndexes[EDNCOUNT];
         CSmartDictionaryAdapter::TDictionaryTypes DictionaryTypes; 
         int MinRecognizableWordLen;
      };

      //Иницализационные данные для CAbbreviationsDictionary
      struct CAbbreviationsInitData
      {
         DBMS::IDataBase *pAbbreviationsDataBase; 

         enum eDictNumbers
         {
            SingleNominal,
            SingleProper,
            Multiple,
            CombinatorySingle,

            EDNCOUNT
         };

         CAbbreviationsDictionary::SInitStruct InitStruct[EDNCOUNT]; 
      };

      //Набор словарей сопоставленных с CMorphoTypesSearch, сам CMorphoTypesSearch получается при помощи CSmartDictionary
      struct CMorphoTypesSearcInitData
      {
         const TDictNumber *First;
         const TDictNumber *Last;
      };

      //Имена таблиц и базы данных для инициализации индексных баз для новых слов
      struct CNdxInitData
      {
         std::wstring TableName;
      };

   public:
      //Общие инициализационные данные
      DBMS::IDataBase *pMorphoDataBase;

      //Данные для отдельных классов
      SSmartDictionaryInitData SmartDictionaryInitData;
      CAbbreviationsInitData AbbreviationsInitData;
      CMorphoTypesSearcInitData MorphoTypesSearcInitData;
      CNdxInitData NdxInitData;
   };

private:
   typedef CObjectsSelectorMultiObject<TDictNumber, Detail::IBasicMorphologyAnalyser> TDictNumberToMorphoAnal;
   typedef AssocVector<TDictNumber, CSmartDictionary *> TDictNumberToSmartDictionary;
   typedef CSymbolCharacteristicCreateManager::TCreateType::TItem TDictsCharacteristic;
   
   struct SLangBasedDicts
   {
      CSmartDictionaryAdapter *pSmartAdpter;
      CSmartDictionary *pSmart;
      CAbbreviationsDictionary *pAbbreviations;
   };

   //Информация по текущему слову
   struct SCurWordInfo
   {  
      //Таблица трансформации символов
      const Detail::TConstData::TChar2Char *m_pPreSearchTransform;
            
      //Буфер с текущей обрабатываемой строкой приведённой к нижнему регистру
      TCHAR LowerBuf[Constants::MAX_CHARS_IN_WORD];
      
      //Длина строки
      size_t Len;

      bool IsFirstLetterUpperCase; //Верхний регистр первой буквы

   public:
      SCurWordInfo(): m_pPreSearchTransform(0), IsFirstLetterUpperCase(false), Len(0) 
	  {
		  memset(LowerBuf, 0, sizeof(TCHAR) * Constants::MAX_CHARS_IN_WORD);
	  }

      //Заполнить данные 
      void Fill( const TCHAR *wcWord )
      {
         Detail::PrepareWordForSearch( wcWord, LowerBuf, Len, IsFirstLetterUpperCase );
      }

      void Fill_NEW( const TCHAR *wcWord )
      {
         // Такой метод Fill работает только на четверть быстрее чем обычный, так что 
         // использование его пока нецелесообразно
         APL_ASSERT_PTR( m_pPreSearchTransform );
         
         TCHAR *pBuf = LowerBuf;
         Detail::TConstData::TChar2Char::const_iterator I/* = m_pPreSearchTransform->end()*/;

         for( ; *wcWord; ++wcWord )
         {
            APL_ASSERT( pBuf - LowerBuf < Constants::MAX_CHARS_IN_WORD );
            
            I = m_pPreSearchTransform->find( *wcWord );

            *(pBuf++) = I == m_pPreSearchTransform->end() ? *wcWord : I->second;
         }
         
         *pBuf = _T('\0');
         Len = pBuf - LowerBuf;
      }

   };

   struct TDictionaryIndexCmp: public std::binary_function<TWordInfo, TWordInfo, bool>
   {
      bool operator()( const TWordInfo &WI1, const TWordInfo &WI2 ) const
      {
         APL_ASSERT( WI1.size() == 1 && WI2.size() );

         return 
            WI1.front().m_WordIndex.m_DictIndex.GetDictionaryIndexWithoutFlags() < 
            WI2.front().m_WordIndex.m_DictIndex.GetDictionaryIndexWithoutFlags();
      }
   };

   struct TIDFormCmp: public std::binary_function<TWordInfo, TWordInfo, bool>
   {
      bool operator()( const TWordInfo &WI1, const TWordInfo &WI2 ) const
      {
         APL_ASSERT( WI1.size() == 1 && WI2.size() );

         return 
            WI1.front().m_WordIndex.m_IDForm < 
            WI2.front().m_WordIndex.m_IDForm;
      }
   };

private:
   TDictNumberToMorphoAnal m_DictNumberToMorphoAnal;           //Объект сам следит за удалением словарей 
   TDictNumberToSmartDictionary m_DictNumberToSmartDictionary; //Сопоставим возможные номера словарей конкретному объекту SmartDictionary
   Detail::IBasicMorphologyAnalyser *m_ObjectIDToMorphoAnal[InitData::ObjectID::DICTS_COUNT];
   SLangBasedDicts m_LangToLangBasedDicts[InitData::Languages::COUNT];                  
   CSymbolCharacteristicCreateManager::TConstPointer m_pSymbolCharacteristic; 
   CConstDataCreateManager::TConstPointer m_pConstData; 
   CTablePtr m_IndexTables[InitData::Languages::COUNT];                                  //Индексные таблицы новых слов
   std::wstring m_IntdexTableNames[InitData::Languages::COUNT];                          //Имена индексных таблиц
   Detail::TSharedObjectMutex m_SharedMutex;
   
   mutable SCurWordInfo m_CurWordInfo;
   IOneRoot *m_pOneRoot;
   IBlackBoxTextStorage *m_pBlackBoxTextStorage;
   IAMConverterMorpho *m_pAMConverter;
   bool m_UseNewWordsTables;    //Использовать таблицы новых слов

   TStandartDeterminationWordCheck m_StandartDeterminationWordCheck;
   TOnlyNounDeterminationWordCheck m_OnlyNounDeterminationWordCheck;
   
   TCompoundWordAnalysis m_CompoundWordAnalysis;

private:
   //Поиск по TDictNumber зарегистрированного класса IMorphologyAnalyser.
   //Если такого класса нет кидается исключение.
   TDictNumberToMorphoAnal::TResultPair GetMorphologyAnalyserFormDictNumber( TDictNumber DictNumber )
   {
      TDictNumberToMorphoAnal::TResultPair ResultPair(m_DictNumberToMorphoAnal.Get(DictNumber) );

      return ResultPair;
   }
   
   //Заполнение p_l_WordInfo морфоинформацией полученной из виртуальных словарей.
   //Не очищает p_l_WordInfo и предполагает что если p_l_WordInfo->empty() то предыдущие 
   //поиски окончились неудачно, Имеется ввиду прежде всего поиск в основном словаре и словаре аббревиатур
   bool FillVirtualPartInSearch(    
      const wchar_t* wcWord, 
      const wchar_t* wcWordLower,
      bool IsFirstLetterUpperCase,
      TWIList* p_l_WordInfo,
      const TDictsCharacteristic &DictsCharacteristic
   );


   //Заполнение p_l_WordInfo морфоинформацией полученной из основного словаря и словаря аббревиатур.
   //Не очищает p_l_WordInfo.
   // UseSmartDictionary - Искать в основном словаре
   // UseAbbreviationsDictionary - Искать в словаре аббревиатур
   // UseMainDictionary - При поиске в основном словаре использовать для поиска подсловарь нарицательных и собственных
   // UseNewWordsDictionary - При поиске в основном словаре использовать для поиска подсловарь новых слов
   // FirstExistLang - Индекс языка в котором встречаются все буквы из wcWord или Undefined если язык неизвестен
   template<bool UseSmartDictionary, bool UseAbbreviationsDictionary>
   void FillBaseDictionaryPart(
      const wchar_t* wcWord, 
      const wchar_t* wcWordLower,
      bool IsFirstLetterUpperCase,
      TWIList* p_l_WordInfo,
      const TDictsCharacteristic &DictsCharacteristic,
      bool UseMainDictionary, bool UseNewWordsDictionary,
      InitData::Languages::eLanguages &FirstExistLang
   );


   //Определение новых слов основываясь на языке FirstExistLang
   //DWC - Интерфейс для проверки подходит ли найденая гипотеза или нет
   bool CMALanguagesManager::FillDeterminatonWordsPart(
      const wchar_t* wcWord, 
      const wchar_t* wcWordLower,
      bool IsFirstLetterUpperCase,
      TWIList* p_l_WordInfo,
      const CSmartDictionary::IDeterminationWordCheck &DWC,
      InitData::Languages::eLanguages FirstExistLang
   );

   //Реализация GetWordInfo для eFindZone = efzNewWord
   bool GetWordInfo_NewWord(
      const wchar_t* wcWord, 
      const wchar_t* wcWordLower,
      bool IsFirstLetterUpperCase,
      TWIList* p_l_WordInfo,
      const TDictsCharacteristic &DictsCharacteristic 
   );

   //Реализация GetWordInfo для eFindZone = efzBaseDictionary
   bool GetWordInfo_BaseDictionary(
      const wchar_t* wcWord, 
      const wchar_t* wcWordLower, 
      bool IsFirstLetterUpperCase,
      TWIList* p_l_WordInfo,
      const TDictsCharacteristic &DictsCharacteristic
   );

   //Реализация GetWordInfo для eFindZone = efzAbbreviation
   bool GetWordInfo_Abbreviation(
      const wchar_t* wcWord, 
      const wchar_t* wcWordLower, 
      bool IsFirstLetterUpperCase,
      TWIList* p_l_WordInfo,
      const TDictsCharacteristic &DictsCharacteristic
   );

   //Реализация GetWordInfo для eFindZone = efzOnlySearch
   //UseCompoundSearch - Использовать ли определение составных слов при поиске
   template<bool UseCompoundSearch>
   bool GetWordInfo_OnlySearch(
      const wchar_t* wcWord, 
      const wchar_t* wcWordLower, 
      bool IsFirstLetterUpperCase,
      TWIList* p_l_WordInfo,
      const TDictsCharacteristic &DictsCharacteristic
   );

   //Реализация GetWordInfo для eFindZone = efzFullAddNewWords
   //DWC - Интерфейс для проверки подходит ли найденая гипотеза или нет
   //AddEmptyData - Добавлять ли пустую информаю о слове, если AddEmptyData == true поиск всегда увенчается успехом
   //UseCompoundSearch - Использовать ли определение составных слов при поиске
   template<bool UseCompoundSearch>
   bool GetWordInfo_FullAddNewWords(
      const wchar_t* wcWord,
      const wchar_t* wcWordLower,
      bool IsFirstLetterUpperCase,
      TWIList* p_l_WordInfo,
      const CSmartDictionary::IDeterminationWordCheck &DWC,
      bool IsAddEmptyData,
      const TDictsCharacteristic &DictsCharacteristic
   );

   //Реализация GetWordInfo при котором новые слова не опредяляются и всегда 
   //добавляются пустые данные. Тестовый режим
   bool GetWordInfo_FullAddEmptyWords(
      const wchar_t* wcWord,
      const wchar_t* wcWordLower, 
      bool IsFirstLetterUpperCase,
      TWIList* p_l_WordInfo,
      const TDictsCharacteristic &DictsCharacteristic
      );

   //Реализация GetWordInfo для eFindZone = efzBaseAndbbreviation
   bool GetWordInfo_BaseAndAbbreviation(
      const wchar_t* wcWord,
      const wchar_t* wcWordLower,
      bool IsFirstLetterUpperCase,
      TWIList* p_l_WordInfo,
      const TDictsCharacteristic &DictsCharacteristic
   );

   //Добавить один элемент в WordInfoList, основываясь на wcWord, DI и WI
   static void AddToList( 
      const wchar_t* wcWord,
      const TDictionaryIndex &DI,
      const CSmartDictionary::SHypothesisInfo::WordInfo &WI,
      TWIList &WordInfoList 
   )
   {
      WordInfoList.push_back( CSmartDictionary::TWIList::value_type() );
      WordInfoList.back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );
      SWordInfo &WordInfo = WordInfoList.back().back();

      WordInfo.m_wWord = wcWord;
      WordInfo.m_SpecialMorpho = WI.m_MorphoType.m_SpecialMorpho;
      WordInfo.m_WordIndex.m_DictIndex = DI;
      WordInfo.m_WordIndex.m_IDForm = WI.m_MorphoType.m_IDForm;
      WordInfo.m_MorphoTypeInfo = WI.m_MorphoType.m_MorphoTypeInfo;
   }

   //Добавить в pSmartDictionary новое слово с пустой морфоинформацией. Затем добавить пустую 
   //морфоинформацию о слове в WordInfoList
   static void AddEmptyData(
      CSmartDictionary *pSmartDictionary,
      const wchar_t* wcWord,
      const wchar_t* wcWordLower,
      bool IsFirstLetterUpperCase,
      TWIList &WordInfoList
   )
   {
      APL_ASSERT_PTR( pSmartDictionary );

      TDictionaryIndex DI;
      CSmartDictionary::SHypothesisInfo::WordInfo WI;

      WI.m_EndingLen = 0;
      WI.m_MorphoType.m_IDEnding = 0;
      WI.m_MorphoType.m_IDForm = 0;
      WI.m_MorphoType.m_MorphoTypeInfo.m_IDType = 0;
      WI.m_MorphoType.m_MorphoTypeInfo.m_MorphoInfo = 0;
      WI.m_MorphoType.m_SpecialMorpho = 0;

      pSmartDictionary->AddNewWord( wcWord, wcWordLower, WI.m_MorphoType.m_MorphoTypeInfo.m_IDType, WI.m_EndingLen, DI );
      DI.SetFirstLetterUpperCase(IsFirstLetterUpperCase);
      AddToList( wcWord, DI, WI, WordInfoList );
   };

   //Подключает или наооборот отключает словари новых слов к таблицам, но не блокирует 
   void SetAnalyseParamsNoLock(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams);


   //Дополняет список WIList однкоренными словами
   //При это для всех IDSource из WIList производится поиск уникальных однокоренных слов, затем определяется
   //морфологическая информация по ним (для IDForm = 0) и добавляется в конец WIList
   void AppendOneRoot( TWIList &WIList );

   //Дополняет список WIList расширенной информацией
   //Для Римских чисел добавляем информацию об их арабском эквиваленте
   void AppendUndepDict( TWIList &WIList );

   //Проверка открытия таблиц новых слов
   void CheckNewWordsTables()
   {
      if( m_UseNewWordsTables )
         for( size_t i = 0; i < InitData::Languages::COUNT; ++i )
            if( !m_IndexTables[i].IsOpen() )
               SS_THROW(L"Не инициализированы таблицы новых слов, проверьте наличие вызыва метода IBaseManager::OpenBase");
   }

   //Делает основную работу для функции GetWordInfo, кроме того что не блокирует m_SharedMutex, 
   //не заполняет m_CurWordInfo и не вызывает CheckNewWordsTables();
   //DWC - Интерфейс для проверки подходит ли найденая гипотеза или нет
   //AddEmptyData - Добавлять ли пустую информаю о слове, если AddEmptyData == true поиск всегда увенчается успехом
   //DictsCharacteristic - Заранее заполненная, характеристика слова.
   bool GetWordInfoImpl(
      /*in*/const wchar_t* wcWord, 
      const wchar_t* wcWordLower, 
      bool IsFirstLetterUpperCase,
      /*out*/TWIList* p_l_WordInfo,
      /*in*/EFindZone eFindZone,
      const CSmartDictionary::IDeterminationWordCheck &DWC,
      bool IsAddEmptyData,
      const TDictsCharacteristic &DictsCharacteristic
      );

   //Обработка составного слова окончивающегося на 's
   //ВОЗВР: Удалось ли обработать слово
   bool GenetiveCompoundWord(
      const wchar_t* wcWord,
      const wchar_t* wcWordLower,
      bool IsFirstLetterUpperCase,
      size_t WordLen,
      IUnit * pUnit,
      EFindZone eFindZone,
      const TDictsCharacteristic &DictsCharacteristic
   );

   //Обработка составного слова окончивающегося на n't
   //ВОЗВР: Удалось ли обработать слово
   bool NegateCompoundWord(
      const wchar_t* wcWord,
      const wchar_t* wcWordLower,
      bool IsFirstLetterUpperCase,
      size_t WordLen,
      IUnit * pUnit
   );

   
   //Обработка составного слова написанного через дефис
   //ВОЗВР: Удалось ли обработать слово
   bool HyphenCompoundWord(
      const wchar_t* wcWord,
      const wchar_t* wcWordLower,
      bool IsFirstLetterUpperCase,
      size_t WordLen,
      IUnit * pUnit,
      EFindZone eFindZone,
      const TDictsCharacteristic &DictsCharacteristic
   );

   ///Корректирует eFindZone для поиска частей слова
   EFindZone CorrectFindZoneForPartSearch( EFindZone eFindZone ) const
   {
      switch(eFindZone)
      {
      case efzFullAddNewWordsCompound:
         return efzFullAddNewWords;

      default:
         return eFindZone;
      }
   }

public:
   CMALanguagesManager();
   
   //Проинициализировать словарь
   //[First, Last) - Итераторы на структуру SLanguage
   template< class InputIterator >
   void Init( InputIterator First, InputIterator Last, IAMConverterMorpho *pAMConverter, IOneRoot *pOneRoot, IBlackBoxTextStorage *pBlackBoxTextStorage );
   
   
   /// проверка наличия слова в словаре
   /*
      Проверка наличия слова во всех словарях, кроме новых слов и аббревиатур
   */
   bool IsExistWord(/*in*/const wchar_t* wcWord);
   
   /// поиск полной морфологической информации по слову 
   bool GetWordInfo/*ByWord*/(
      /*in*/const wchar_t* wcWord, 
      /*out*/TWIList* p_l_WordInfo,
      /*in*/EFindZone eFindZone = efzFullAddNewWords
   );

   bool GetWordInfo/*ByWord*/(	
      /*in*/const wchar_t* wcWord, 
      /*in out*/SS::Interface::Core::BlackBox::IUnit * pUnit,
      /*in*/EFindZone eFindZone = efzFullAddNewWords
   );

   /// поиск полной морфологической информации по индексу слова
   bool GetWordInfo/*ByWordIndex*/(
      /*in*/SS::Dictionary::Types::SWordIndex* pWordIndex, 
      /*out*/SS::Dictionary::Types::SWordInfo* pWordInfo);
   
   /// поиск слова по его индексу 
   bool GetWord(
      /*in*/SS::Dictionary::Types::SWordIndex* pWordIndex,
      /*out*/wchar_t* wcWord
   );

   /// поиск основы слова слова (части слова без оканчания) по его индексу 
   bool GetWordSource(
      /*in*/const SS::Dictionary::Types::TDictionaryIndex &DictIndex,
      /*out*/wchar_t* wcWordSource
   );

   /// поиск словоформ слова по индексу основы
   bool GetWordFormsByIDSource(
      /*in*/SS::Dictionary::Types::TDictionaryIndex* pDictionaryIndex,
      /*out*/TWIList* p_l_WordInfo);

   /// возвращает морфологическую информацию по типу морфообразования
   /*
         Если не удаётся найти информации по переданному oIDForm, то мы пытаемся её найти по IDForm == 0
   */
   void GetMorphoInfoByIDType(
         /*in*/SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber eDictionariesNumber,
         /*in*/SS::Dictionary::Types::TIDType oIDType, 
         /*in*/SS::Dictionary::Types::TIDForm oIDForm, 
         /*out*/SS::Dictionary::Types::TMorphoInfo* pMorphoInfo, 
         /*out*/SS::Dictionary::Types::TSpecialMorpho* pSpecialMorpho
         );
         
   /// сообщает морфологическому словарю о том, необходимо переходить к анализу очередного текста
   void NextText();

   //Подключает или наооборот отключает словари новых слов к таблицам
   void SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams);

   //Функции используются только для отладки
   CSmartDictionary *GetSmartDictionary( InitData::Languages::eLanguages Lang )
   { 
      APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() );
      return m_LangToLangBasedDicts[Lang].pSmart; 
   }

   IAMConverterMorpho *GetAMConverter() const 
   { 
      APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() );
      return m_LangToLangBasedDicts[InitData::Languages::English].pSmart->GetAMConverter(); 
   }
      
   SCurWordInfo &GetCurWordInfo()const
   {
      return m_CurWordInfo;
   }
   ///////////////////////////////////////////////////////////////////////////////
   // ILoad
   bool ToBinary(); 
   bool ReLoad();
   bool Load();
   bool Clear();


   ///////////////////////////////////////////////////////////////////////////////
    
   //Открыть таблицы новых слов и вызвать SetAnalyseParams(GetAnalyseParams())
   void OpenNewWordsTables( DBMS::IDataBase *pDB );

   //Закрыть таблицы новых слов и вызвать SetAnalyseParams(GetAnalyseParams())
   void CloseNewWordsTables();

   ///////////////////////////////////////////////////////////////////////////////
   //Тестовые функции

   //Только определение морфологии слов. как будто все слова новые
   bool GetDeterminationWordInfo(
      const wchar_t* wcWord, 
      CSmartDictionary::SHypothesisInfo &HypothesisInfo
   );

   ~CMALanguagesManager();
};

} //namespace Dictionary
} //namespace SS
