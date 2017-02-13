#include "stdafx.h"

namespace SS{ 
namespace Dictionary{

///////////////////////////////////////////////////////////////////////////////
// class CMALanguagesManager
///////////////////////////////////////////////////////////////////////////////
CMALanguagesManager::CMALanguagesManager(): 
   m_pSymbolCharacteristic(0), m_pOneRoot(0), 
   m_UseNewWordsTables(false), m_pConstData(NULL), 
   m_pBlackBoxTextStorage(NULL), m_pAMConverter(NULL)
{	
   memset(&m_ObjectIDToMorphoAnal, 0, sizeof(Detail::IBasicMorphologyAnalyser*) * InitData::ObjectID::DICTS_COUNT);
   memset(&m_LangToLangBasedDicts, 0, sizeof(SLangBasedDicts) * InitData::Languages::COUNT);
   DEBUG_MSG_LOG2("CMALanguagesManager()");
} 

CMALanguagesManager::~CMALanguagesManager()
{
   DEBUG_MSG_LOG2("~CMALanguagesManager()");
}

template< class InputIterator >
void CMALanguagesManager::Init( InputIterator First, InputIterator Last, IAMConverterMorpho *pAMConverter, IOneRoot *pOneRoot, IBlackBoxTextStorage *pBlackBoxTextStorage )
{
   DEBUG_MSG_LOG5("CMALanguagesManager::Init()");

   APL_ASSERT_PTR( pAMConverter );
   APL_ASSERT_PTR( pOneRoot );
   APL_ASSERT_PTR( pBlackBoxTextStorage );
   APL_ASSERT( std::distance(First, Last) == InitData::Languages::COUNT );

   //Нет необходимости
   //Detail::TSharedObjectMutex::LockWrite Guard( m_SharedMutex );

   m_pOneRoot = pOneRoot;
   m_pBlackBoxTextStorage = pBlackBoxTextStorage;
   m_pAMConverter = pAMConverter;
   m_UseNewWordsTables = false;

   typedef CSelectIterator<
      const CAbbreviationsDictionary::SInitStruct *, 
      CAbbreviationsDictionary::SInitStruct::SSelectDictNumber<const CAbbreviationsDictionary::SInitStruct *> > 
   TSelectDictNumber;

   //При создании необходимо учитывать последовательность определённую в InitData::ObjectID::eObjectID
   m_DictNumberToMorphoAnal.Clear();
   m_DictNumberToSmartDictionary.clear();

   m_pSymbolCharacteristic = CSymbolCharacteristicCreateManager::Create();
   m_pConstData = CConstDataCreateManager::Create();
   m_CurWordInfo.m_pPreSearchTransform = &m_pConstData->GetPreSearchTransform();

   TDictNumberToMorphoAnal::TAutoPtr MAAutoPtr;

   //Создаём зависмые от языка словари
   size_t CurDict = 0;
   size_t CurLang = 0;
   for( ; First != Last; ++First, ++CurLang )
   {
      MAAutoPtr.reset( m_LangToLangBasedDicts[ CurLang ].pSmartAdpter = new CSmartDictionaryAdapter() );

      m_LangToLangBasedDicts[ CurLang ].pSmartAdpter->Init( 
         First->pMorphoDataBase, pAMConverter, First->SmartDictionaryInitData.DBPath.c_str(),
         First->SmartDictionaryInitData.DictionaryIndexes[SLanguage::SSmartDictionaryInitData::EDNNominal],
         First->SmartDictionaryInitData.DictionaryIndexes[SLanguage::SSmartDictionaryInitData::EDNProper],
         First->SmartDictionaryInitData.DictionaryIndexes[SLanguage::SSmartDictionaryInitData::EDNNewWords],
         First->SmartDictionaryInitData.MinRecognizableWordLen,
         First->SmartDictionaryInitData.DictionaryTypes
      );
         
      m_LangToLangBasedDicts[ CurLang ].pSmart = &m_LangToLangBasedDicts[CurLang].pSmartAdpter->GetOrigin();

      m_ObjectIDToMorphoAnal[ CurDict++ ] = m_LangToLangBasedDicts[CurLang].pSmartAdpter;

      m_DictNumberToMorphoAnal.Add( 
         First->SmartDictionaryInitData.DictionaryIndexes, 
         First->SmartDictionaryInitData.DictionaryIndexes + SLanguage::SSmartDictionaryInitData::EDNCOUNT,
         MAAutoPtr 
      );

      MAAutoPtr.reset( m_LangToLangBasedDicts[CurLang].pAbbreviations = new CAbbreviationsDictionary() );

      m_LangToLangBasedDicts[CurLang].pAbbreviations->Init( 
         First->pMorphoDataBase, 
         First->AbbreviationsInitData.pAbbreviationsDataBase,
         First->AbbreviationsInitData.InitStruct,
         First->AbbreviationsInitData.InitStruct+ SLanguage::CAbbreviationsInitData::EDNCOUNT
      );                                            

      m_ObjectIDToMorphoAnal[CurDict++] = m_LangToLangBasedDicts[CurLang].pAbbreviations;
      
      std::vector<TDictNumber> DictNumbers;
      
      m_DictNumberToMorphoAnal.Add( 
         TSelectDictNumber( First->AbbreviationsInitData.InitStruct ), 
         TSelectDictNumber( First->AbbreviationsInitData.InitStruct ) + SLanguage::CAbbreviationsInitData::EDNCOUNT, 
         MAAutoPtr 
      );     

      for( const TDictNumber *Cur = First->MorphoTypesSearcInitData.First; Cur != First->MorphoTypesSearcInitData.Last; ++Cur )
      {   
         APL_CHECK( m_DictNumberToSmartDictionary.insert( 
            TDictNumberToSmartDictionary::value_type(*Cur, m_LangToLangBasedDicts[CurLang].pSmart)
            ).second );
      } 

      m_IntdexTableNames[CurLang] = First->NdxInitData.TableName;
   }        

   APL_ASSERT( CurLang == InitData::Languages::COUNT );


   const DATA_TYPE::NAMES::DictionariesNumber rgNumberDictionaryKeys[] = { DATA_TYPE::NAMES::ednNumeric, DATA_TYPE::NAMES::ednFloatPointNumeric };

   //Создаём независмые от языка словари
	MAAutoPtr.reset( new COrderingRussian(pAMConverter) );
	m_ObjectIDToMorphoAnal[CurDict++] = MAAutoPtr.get();
	m_DictNumberToMorphoAnal.Add(DATA_TYPE::NAMES::ednOrderingRussian, MAAutoPtr);

   MAAutoPtr.reset( new CNumberDictionary(pAMConverter) );
   m_ObjectIDToMorphoAnal[CurDict++] = MAAutoPtr.get();
   m_DictNumberToMorphoAnal.Add( rgNumberDictionaryKeys, rgNumberDictionaryKeys + APL_ARRSIZE(rgNumberDictionaryKeys), MAAutoPtr );
   
   MAAutoPtr.reset( new CRomanNumberDictionary(pAMConverter) );
   m_ObjectIDToMorphoAnal[CurDict++] = MAAutoPtr.get();
   m_DictNumberToMorphoAnal.Add( DATA_TYPE::NAMES::ednRomanNumeric, MAAutoPtr);
   
   MAAutoPtr.reset( new CNumericOrderingDictionary(pAMConverter) );
   m_ObjectIDToMorphoAnal[CurDict++] = MAAutoPtr.get();
   m_DictNumberToMorphoAnal.Add( DATA_TYPE::NAMES::ednOrderingEnglish, MAAutoPtr);

   MAAutoPtr.reset( new CSymbolsDictionary(pAMConverter) );
   m_ObjectIDToMorphoAnal[CurDict++] = MAAutoPtr.get();
   m_DictNumberToMorphoAnal.Add( DATA_TYPE::NAMES::ednSymbols, MAAutoPtr );

   APL_ASSERT( CurDict == InitData::ObjectID::DICTS_COUNT );

   SetAnalyseParamsNoLock(0);
}

template void CMALanguagesManager::Init( const CMALanguagesManager::SLanguage *, const CMALanguagesManager::SLanguage *, IAMConverterMorpho *pAMConverter, IOneRoot *pOneRoot, IBlackBoxTextStorage *pBlackBoxTextStorage );
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::GetWordInfo/*ByWordIndex*/(
	/*in*/SS::Dictionary::Types::SWordIndex* pWordIndex, 
	/*out*/SS::Dictionary::Types::SWordInfo* pWordInfo
)
{
   DEBUG_MSG_LOG5("CMALanguagesManager::GetWordInfoByWordIndex() " << DictionaryIndexDescription(pWordIndex->m_DictIndex) << "  " << pWordIndex->m_DictIndex.GetId() );

   APL_ASSERT_PTR( pWordIndex );
   APL_ASSERT_PTR( pWordInfo );
   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);
   
   Detail::TSharedObjectMutex::LockRead Guard( m_SharedMutex );

   SS_TRY
   {
      CheckNewWordsTables();

      TDictNumberToMorphoAnal::TResultPair ResultPair( GetMorphologyAnalyserFormDictNumber( pWordIndex->m_DictIndex.GetDictionaryNumber() ));
         
      for( ; ResultPair.first != ResultPair.second; ++ ResultPair.first )
         if( ResultPair.first->GetWordInfo( pWordIndex, pWordInfo ) )
            return true;
   }
   SS_CATCH( L"" );

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::GetWord(
   /*in*/SS::Dictionary::Types::SWordIndex* pWordIndex,
   /*out*/wchar_t* wcWord
)
{
   DEBUG_MSG_LOG5("CMALanguagesManager::GetWord() " << DictionaryIndexDescription(pWordIndex->m_DictIndex) << "  " << pWordIndex->m_DictIndex.GetId() );

   APL_ASSERT_PTR( pWordIndex );
   APL_ASSERT_PTR( wcWord );
   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);

   Detail::TSharedObjectMutex::LockRead Guard( m_SharedMutex );

   SS_TRY
   {
      CheckNewWordsTables();
      
      *wcWord = L'\0';
      
      TDictNumberToMorphoAnal::TResultPair ResultPair( GetMorphologyAnalyserFormDictNumber( pWordIndex->m_DictIndex.GetDictionaryNumber() ));

      for( ; ResultPair.first != ResultPair.second; ++ ResultPair.first )
         if( ResultPair.first->GetWord( pWordIndex, wcWord ) )
         {
            DEBUG_MSG_LOG5("          " << Convert(wcWord, std::string()) );
            return true;     
         }
   }
   SS_CATCH( L"" );

   DEBUG_MSG_LOG5("          НЕ НАЙДЕНО!"  );
   return false;
}
///////////////////////////////////////////////////////////////////////////////

/// поиск основы слова слова (части слова без оканчания) по его индексу 
bool CMALanguagesManager::GetWordSource(
   /*in*/const SS::Dictionary::Types::TDictionaryIndex &DictIndex,
   /*out*/wchar_t* wcWordSource
)
{
   DEBUG_MSG_LOG5("CMALanguagesManager::GetWordSource() " << DictionaryIndexDescription(DictIndex) << "  " << DictIndex.GetId() );

   APL_ASSERT_PTR( wcWordSource );
   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);

   Detail::TSharedObjectMutex::LockRead Guard( m_SharedMutex );

   SS_TRY
   {
      CheckNewWordsTables();

      *wcWordSource = L'\0';

      TDictNumberToMorphoAnal::TResultPair ResultPair( GetMorphologyAnalyserFormDictNumber( DictIndex.GetDictionaryNumber() ));

      for( ; ResultPair.first != ResultPair.second; ++ ResultPair.first )
         if( ResultPair.first->GetWordSource( DictIndex, wcWordSource ) )
         {
            DEBUG_MSG_LOG5("          " << Convert(wcWordSource, std::string()) );
            return true;     
         }
   }
   SS_CATCH( L"" );

   DEBUG_MSG_LOG5("          НЕ НАЙДЕНО!"  );
   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::GetWordFormsByIDSource(
   /*in*/SS::Dictionary::Types::TDictionaryIndex* pDictionaryIndex,
   /*out*/TWIList* p_l_WordInfo
)
{
   DEBUG_MSG_LOG5("CMALanguagesManager::GetWordFormsByIDSource() " << DictionaryIndexDescription(*pDictionaryIndex) << "  " << pDictionaryIndex->GetId() );

   APL_ASSERT_PTR( pDictionaryIndex );
   APL_ASSERT_PTR( p_l_WordInfo );
   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);

   TWIList WIListBuf;

   Detail::TSharedObjectMutex::LockRead Guard( m_SharedMutex );
   p_l_WordInfo->clear();

   SS_TRY
   {
      CheckNewWordsTables();

      TDictNumberToMorphoAnal::TResultPair ResultPair( GetMorphologyAnalyserFormDictNumber( pDictionaryIndex->GetDictionaryNumber() ));

      for( ; ResultPair.first != ResultPair.second; ++ ResultPair.first )
         if( ResultPair.first->GetWordFormsByIDSource( pDictionaryIndex, &WIListBuf ) )
            p_l_WordInfo->splice( p_l_WordInfo->end(), WIListBuf );
   }
   SS_CATCH( L"" );

   //Уже отсортировано
   //p_l_WordInfo->sort(TIDFormCmp());

   return !p_l_WordInfo->empty();
}
///////////////////////////////////////////////////////////////////////////////

void CMALanguagesManager::GetMorphoInfoByIDType(
   /*in*/SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber eDictionariesNumber,
   /*in*/SS::Dictionary::Types::TIDType oIDType, 
   /*in*/SS::Dictionary::Types::TIDForm oIDForm, 
   /*out*/SS::Dictionary::Types::TMorphoInfo* pMorphoInfo, 
   /*out*/SS::Dictionary::Types::TSpecialMorpho* pSpecialMorpho
)
{
   APL_ASSERT_PTR( pMorphoInfo );
   APL_ASSERT_PTR( pSpecialMorpho );
   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);

   Detail::TSharedObjectMutex::LockRead Guard( m_SharedMutex );

   SS_TRY
   {
      CheckNewWordsTables();

      TDictNumberToSmartDictionary::const_iterator I = m_DictNumberToSmartDictionary.find( eDictionariesNumber );

      if( I == m_DictNumberToSmartDictionary.end() )
      {
         *pMorphoInfo = 0;
         *pSpecialMorpho = 0;

         return;
      }

      if( I->second->GetMorphoInfoByIDType( oIDType, oIDForm, pMorphoInfo, pSpecialMorpho ) == FALSE )
         if( oIDForm == 0 || I->second->GetMorphoInfoByIDType( oIDType, 0, pMorphoInfo, pSpecialMorpho ) == FALSE )
         {
            *pMorphoInfo = 0;
            *pSpecialMorpho = 0;
         }

   }
   SS_CATCH( L"" );
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::IsExistWord(/*in*/const wchar_t* wcWord)
{
   DEBUG_MSG_LOG5("CMALanguagesManager::IsExistWord() " << Convert(wcWord, std::string()) );
   
   APL_ASSERT_PTR( wcWord );
   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);
   
   Detail::TSharedObjectMutex::LockRead Guard( m_SharedMutex );

   if( *wcWord == L'\0' )
      return false;

   SS_TRY
   {
      CheckNewWordsTables();

      m_CurWordInfo.Fill( wcWord );

      //Опредиляем область поиска 
      TDictsCharacteristic DictsCharacteristic( m_pSymbolCharacteristic->GetFlags(wcWord) );
      size_t i;
      
      //проверим сначала есть ли такое слово в словарях зависимых от языка
      for( i = 0; i < InitData::Languages::COUNT; ++i )
      {
         //Может ли каждый символ слова быть в основном словаре 
         if( DictsCharacteristic.test( InitData::Convert(static_cast<InitData::Languages::eLanguages>(i), InitData::LangDepDict::Smart)) )
         {
            //Если есть слово в словаре
            if( m_LangToLangBasedDicts[i].pSmart->IsExistWord(m_CurWordInfo.LowerBuf) ) 
               return true;
         }

         //Может ли каждый символ слова быть в словаре аббревиатур
         if( DictsCharacteristic.test( InitData::Convert(static_cast<InitData::Languages::eLanguages>(i), InitData::LangDepDict::Abbreviations)) )
         {
            //Если есть слово в словаре
            if( m_LangToLangBasedDicts[i].pAbbreviations->IsExistWord(m_CurWordInfo.LowerBuf) )
               return true;
         }
      }

      //Теперь проверим словари не зависящие от языка
      for( i = InitData::FirstUndepDict(); i < InitData::ObjectID::DICTS_COUNT; ++i )
      {
         //Может ли каждый символ слова быть в данном словаре 
         if( DictsCharacteristic.test(i) )
         {
            //Если есть слово в словаре
            if( m_ObjectIDToMorphoAnal[i]->IsExistWord(m_CurWordInfo.LowerBuf) ) 
               return true;
         }
      }
   }
   SS_CATCH( L"" );

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::GetWordInfo_NewWord(
   const wchar_t* wcWord,
   const wchar_t* wcWordLower,
   bool IsFirstLetterUpperCase,
   TWIList* p_l_WordInfo,
   const TDictsCharacteristic &DictsCharacteristic 
)
{
   DEBUG_MSG_LOG5("CMALanguagesManager::GetWordInfo_NewWord() " << Convert(wcWord, std::string()) );

   size_t i;
   bool ExistAnyCharacteristic = false; //Могут ли хотябы в одном основном словаре присутствовать все символы слова  

   //проверим сначала есть ли такое слово в словарях зависимых от языка
   for( i = 0; i < InitData::Languages::COUNT; ++i )
   {
      //Может ли каждый символ слова быть в данном словаре 
      if( DictsCharacteristic.test( InitData::Convert(static_cast<InitData::Languages::eLanguages>(i), InitData::LangDepDict::Smart)) )
      {
         ExistAnyCharacteristic = true;

         //Если есть слово в словаре
         if( m_LangToLangBasedDicts[i].pSmart->GetWordInfo(wcWordLower, wcWord, IsFirstLetterUpperCase, p_l_WordInfo, false, true) )
            return true;
      }
   }

   //Если не нашлось ни одного основоного словаря в котором могут присутствовать все символы слова, то мы могли
   //добавить такое слово в словарь по умолчанию (из первого языка)
   if( !ExistAnyCharacteristic )
   {
      if( m_LangToLangBasedDicts[0].pSmart->GetWordInfo(wcWordLower, wcWord, IsFirstLetterUpperCase, p_l_WordInfo, false, true) )
         return true;
   }

   return false;
}
///////////////////////////////////////////////////////////////////////////////

inline bool CMALanguagesManager::FillVirtualPartInSearch(    
   const wchar_t* wcWord, 
   const wchar_t* wcWordLower,
   bool IsFirstLetterUpperCase,
   TWIList* p_l_WordInfo,
   const TDictsCharacteristic &DictsCharacteristic
)
{
   TWIList WIListBuf;
   size_t i;

   //Со словарём символов отдельная история
   //Если слово найдено в одном из реальных словарей, то в виртуальном словаре символов не ищется
   //Словарь символов должен быть последним словарём
   APL_ASSERT( InitData::ObjectID::Symbols + 1 == InitData::ObjectID::DICTS_COUNT );

   if( p_l_WordInfo->empty() )
   {
      //Может ли каждый символ слова быть в словаре символов 
      if( DictsCharacteristic.test(InitData::ObjectID::Symbols) )
      {
         //Если есть слово в словаре
         if( m_ObjectIDToMorphoAnal[InitData::ObjectID::Symbols]->GetWordInfo(wcWordLower, wcWord, IsFirstLetterUpperCase, &WIListBuf) )
            p_l_WordInfo->splice( p_l_WordInfo->end(), WIListBuf );
      }
   }

   //Теперь проверим словари не зависящие от языка
   for( i = InitData::FirstUndepDict(); i < InitData::ObjectID::Symbols; ++i )
   {
      //Может ли каждый символ слова быть в данном словаре 
      if( DictsCharacteristic.test(i) )
      {
         //Если есть слово в словаре
         if( m_ObjectIDToMorphoAnal[i]->GetWordInfo(wcWordLower, wcWord, IsFirstLetterUpperCase, &WIListBuf) )
            p_l_WordInfo->splice( p_l_WordInfo->end(), WIListBuf );
      }
   }

   return !p_l_WordInfo->empty();
}
///////////////////////////////////////////////////////////////////////////////

template<bool UseSmartDictionary, bool UseAbbreviationsDictionary>
inline void CMALanguagesManager::FillBaseDictionaryPart(
   const wchar_t* wcWord, 
   const wchar_t* wcWordLower,
   bool IsFirstLetterUpperCase,
   TWIList* p_l_WordInfo,
   const TDictsCharacteristic &DictsCharacteristic,
   bool UseMainDictionary, bool UseNewWordsDictionary,
   InitData::Languages::eLanguages &FirstExistLang
)
{
   FirstExistLang = InitData::Languages::Undefined;

   size_t i;
   TWIList WIListBuf;

   //проверим сначала есть ли такое слово в словарях зависимых от языка
   for( i = 0; i < InitData::Languages::COUNT && p_l_WordInfo->empty(); ++i )
   {    
      if( UseSmartDictionary ) //Значение известно на этапе компиляции, компилятор должен убрать ветвление
      {
         //Может ли каждый символ слова быть в основном словаре 
         if( DictsCharacteristic.test( InitData::Convert(static_cast<InitData::Languages::eLanguages>(i), InitData::LangDepDict::Smart)) )
         {
            if( FirstExistLang == InitData::Languages::Undefined ) FirstExistLang = static_cast<InitData::Languages::eLanguages>(i);

            //Если есть слово в словаре
            if( m_LangToLangBasedDicts[i].pSmart->GetWordInfo(wcWordLower, wcWord, IsFirstLetterUpperCase, &WIListBuf, UseMainDictionary, UseNewWordsDictionary) )
            {
               APL_ASSERT( !WIListBuf.empty() );
               p_l_WordInfo->splice( p_l_WordInfo->end(), WIListBuf );
            }
         }
      }

      if( UseAbbreviationsDictionary ) //Значение известно на этапе компиляции, компилятор должен убрать ветвление
      {
         //Может ли каждый символ слова быть в словаре абревиатур
         if( DictsCharacteristic.test( InitData::Convert(static_cast<InitData::Languages::eLanguages>(i), InitData::LangDepDict::Abbreviations)) )
         {
            //Если есть слово в словаре
            if( m_LangToLangBasedDicts[i].pAbbreviations->GetWordInfo(wcWordLower, wcWord, IsFirstLetterUpperCase, &WIListBuf) )
            {
               APL_ASSERT( !WIListBuf.empty() );
               p_l_WordInfo->splice( p_l_WordInfo->end(), WIListBuf );
            }
         }
      }
   }

   //if( !p_l_WordInfo->empty() && m_UseAuxSearchInfo ) 
   //   AppendOneRoot(*p_l_WordInfo);
}
///////////////////////////////////////////////////////////////////////////////

inline bool CMALanguagesManager::FillDeterminatonWordsPart(
	const wchar_t* wcWord, 
	const wchar_t* wcWordLower,
	bool IsFirstLetterUpperCase,
	TWIList* p_l_WordInfo,
	const CSmartDictionary::IDeterminationWordCheck &DWC,
	InitData::Languages::eLanguages FirstExistLang
	)
{
	//Попробуем определить автоматически морфоинформацию о слове
	APL_ASSERT( FirstExistLang != InitData::Languages::Undefined );

	CSmartDictionary::SHypothesisInfo HypothesisInfo;

	if( m_LangToLangBasedDicts[FirstExistLang].pSmart->DeterminationWordInfo(wcWord, wcWordLower, IsFirstLetterUpperCase, HypothesisInfo, DWC) )
	{
		//Мы успешно определили морфоинформацию
		//Теперь нам необходимо добавить уникальные IDType:EndingLen  в словарь новых слов, и уникальные IDType:IDForm в
		//p_l_WordInfo
		//Благодаря тому что мы не можем заранее узнать какой TDictionaryIndex будет у вновь добавленного слова, 
		//нам придётся делать дополнительный вызов GetWordInfo после добавления новых слов. 
		//Вообще можно вести два списка гипотез и добавлять в оба синхронизируя TDictionaryIndex, но не факт что это
		//будет быстрее
		CSmartDictionary::SHypothesisInfo::TFormsInfoList::iterator I;
		TDictionaryIndex DI;

		//std::cout << "Origin: " << std::endl;
		//UnitTests::Dictionary::Print( std::cout, HypothesisInfo, pSDExistAnyCharacteristic ); 

		APL_ASSERT( !HypothesisInfo.Empty() );
		HypothesisInfo.UniqueIDTypeEndingLen();
		APL_ASSERT( !HypothesisInfo.Empty() );

		//std::cout << std::endl << "UniqueIDTypeEndingLen: " << std::endl;
		//UnitTests::Dictionary::Print( std::cout, HypothesisInfo, pSDExistAnyCharacteristic ); 
		//std::cout << std::endl;

		for(I = HypothesisInfo.m_FormsInfoList.begin(); I != HypothesisInfo.m_FormsInfoList.end(); ++I)
		{
			m_LangToLangBasedDicts[FirstExistLang].pSmart->AddNewWord( wcWord, wcWordLower, I->m_MorphoType.m_MorphoTypeInfo.m_IDType, I->m_EndingLen, DI );
		}
		APL_CHECK( m_LangToLangBasedDicts[FirstExistLang].pSmart->GetWordInfo(wcWordLower, wcWord, IsFirstLetterUpperCase, p_l_WordInfo, false, true) );
		DEBUG_MSG_LOG5( "       " << "Определили новое слово и добавили словарь новых слов" );
		return true;

		/*
		отключение пополнения морфословаря

		return m_LangToLangBasedDicts[FirstExistLang].pSmart->GetWordInfoFromHypothesis(
			wcWord, 
			wcWordLower,
			IsFirstLetterUpperCase,
			HypothesisInfo,
			p_l_WordInfo);
		*/
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::GetWordInfo_BaseDictionary(
   const wchar_t* wcWord, 
   const wchar_t* wcWordLower,
   bool IsFirstLetterUpperCase,
   TWIList* p_l_WordInfo,
   const TDictsCharacteristic &DictsCharacteristic
)
{
   DEBUG_MSG_LOG5("CMALanguagesManager::GetWordInfo_BaseDictionary() " << Convert(wcWord, std::string()) );
   InitData::Languages::eLanguages FirstExistLang;

   FillBaseDictionaryPart<true, false>(wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic, true, false, FirstExistLang);
   
   return FillVirtualPartInSearch( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic );
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::GetWordInfo_Abbreviation(
   const wchar_t* wcWord,
   const wchar_t* wcWordLower,
   bool IsFirstLetterUpperCase,
   TWIList* p_l_WordInfo,
   const TDictsCharacteristic &DictsCharacteristic
)
{
   DEBUG_MSG_LOG5("CMALanguagesManager::GetWordInfo_Abbreviation() " << Convert(wcWord, std::string()) );
   InitData::Languages::eLanguages FirstExistLang;

   FillBaseDictionaryPart<false, true>(wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic, false, false, FirstExistLang);
   
   return !p_l_WordInfo->empty();
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::GetWordInfo_BaseAndAbbreviation(
   const wchar_t* wcWord,
   const wchar_t* wcWordLower,
   bool IsFirstLetterUpperCase,
   TWIList* p_l_WordInfo,
   const TDictsCharacteristic &DictsCharacteristic
)
{
   DEBUG_MSG_LOG5("CMALanguagesManager::GetWordInfo_BaseAndAbbreviation() " << Convert(wcWord, std::string()) );
   InitData::Languages::eLanguages FirstExistLang;

   FillBaseDictionaryPart<true, true>( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic, true, false, FirstExistLang );

   return FillVirtualPartInSearch( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic );
}
///////////////////////////////////////////////////////////////////////////////

template<bool UseCompoundSearch>
bool CMALanguagesManager::GetWordInfo_OnlySearch(
   const wchar_t* wcWord, 
   const wchar_t* wcWordLower,
   bool IsFirstLetterUpperCase,
   TWIList* p_l_WordInfo,
   const TDictsCharacteristic &DictsCharacteristic
)
{
   DEBUG_MSG_LOG5("CMALanguagesManager::GetWordInfo_OnlySearch() " << Convert(wcWord, std::string()) );

   InitData::Languages::eLanguages FirstExistLang;

   FillBaseDictionaryPart<true, true>( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic, true, false, FirstExistLang );

   if( FillVirtualPartInSearch(wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic) )
   {
      DEBUG_MSG_LOG5( "       " << "Нашли уже существующее слово" );
      return true;
   }

   if( UseCompoundSearch )
   {
      if( m_LangToLangBasedDicts[FirstExistLang].pSmart->GetCompoundWordInfo(wcWordLower, wcWord, IsFirstLetterUpperCase, *p_l_WordInfo) )
      {
         DEBUG_MSG_LOG5( "       " << "Нашли в составное слово" );
         return true;
      }
   }

   //Если не нашлось ни одного основоного словаря в котором могут присутствовать все символы слова, то мы могли
   //добавить такое слово в словарь по умолчанию (из первого языка)
   if( FirstExistLang == InitData::Languages::Undefined )
   {
      if( m_LangToLangBasedDicts[0].pSmart->GetWordInfo(wcWordLower, wcWord, IsFirstLetterUpperCase, p_l_WordInfo, false, true) )
      {
         DEBUG_MSG_LOG5( "       " << "Нашли в словаре по умолчанию" );
         return true;
      }
   }

   DEBUG_MSG_LOG5( "       " << "Не нашли!!!" );
   return false;
}
///////////////////////////////////////////////////////////////////////////////

template<bool UseCompoundSearch>
bool CMALanguagesManager::GetWordInfo_FullAddNewWords(
   const wchar_t* wcWord, 
   const wchar_t* wcWordLower,
   bool IsFirstLetterUpperCase,
   TWIList* p_l_WordInfo,
   const CSmartDictionary::IDeterminationWordCheck &DWC,
   bool IsAddEmptyData,
   const TDictsCharacteristic &DictsCharacteristic
)
{
   DEBUG_MSG_LOG5("CMALanguagesManager::GetWordInfo_FullAddNewWords() " << Convert(wcWord, std::string()) );

   InitData::Languages::eLanguages FirstExistLang;

   FillBaseDictionaryPart<true, true>( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic, true, true, FirstExistLang );

   if( FillVirtualPartInSearch(wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic) )
   {
      DEBUG_MSG_LOG5( "       " << "Нашли уже существующее слово" );
      return true;
   }

   //Если не нашлось ни одного основоного словаря в котором могут присутствовать все символы слова, то мы могли
   //добавить такое слово в словарь по умолчанию (из первого языка)
   if( FirstExistLang == InitData::Languages::Undefined )
   {
      if( m_LangToLangBasedDicts[0].pSmart->GetWordInfo(wcWordLower, wcWord, IsFirstLetterUpperCase, p_l_WordInfo, false, true) )
      {
        DEBUG_MSG_LOG5( "       " << "Нашли в словаре по умолчанию" );
        return true;
      }

      if( IsAddEmptyData )
      {
         //Слово нигде не встречается и к тому же опредилить его параметры автоматически мы не можем
         //Добавляем пустую информацию по нему в словарь
         AddEmptyData( m_LangToLangBasedDicts[0].pSmart, wcWord, wcWordLower, IsFirstLetterUpperCase, *p_l_WordInfo );

         DEBUG_MSG_LOG5( "       " << "Добавили в словарь по умолчанию (не нашлось ни одного основоного словаря в котором могут присутствовать все символы слова)" );
         return true;
      }
      else
      {
         DEBUG_MSG_LOG5( "       " << "Запрещено добавлять в словарь по умолчанию (не нашлось ни одного основоного словаря в котором могут присутствовать все символы слова)" );
         return false;
      }
   }
   APL_ASSERT( FirstExistLang != InitData::Languages::Undefined );

   if( UseCompoundSearch )
   {
      if( m_LangToLangBasedDicts[FirstExistLang].pSmart->GetCompoundWordInfo(wcWordLower, wcWord, IsFirstLetterUpperCase, *p_l_WordInfo) )
      {
         DEBUG_MSG_LOG5( "       " << "Нашли в составное слово" );
         return true;
      }
   }

   if( FillDeterminatonWordsPart(wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DWC, FirstExistLang) )
      return true;

   //Слово нигде не встречается и к тому же опредилить его параметры автоматически мы не можем
   //Добавляем пустую информацию по нему в словарь
   if( IsAddEmptyData )
   {
      AddEmptyData( m_LangToLangBasedDicts[FirstExistLang].pSmart, wcWord, wcWordLower, IsFirstLetterUpperCase, *p_l_WordInfo );
      DEBUG_MSG_LOG5( "       " << "Определить новое слово невозможно добавили в словарь по умолчанию" );

      return true;
   }
   else
   {
      DEBUG_MSG_LOG5( "       " << "Определить новое слово невозможно запрещено добавлять в словарь по умолчанию" );

      return false;
   }
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::GetWordInfo_FullAddEmptyWords(
   const wchar_t* wcWord, const wchar_t* wcWordLower,
   bool IsFirstLetterUpperCase,
   TWIList* p_l_WordInfo,
   const TDictsCharacteristic &DictsCharacteristic 
)
{
   DEBUG_MSG_LOG5("CMALanguagesManager::GetWordInfo_FullAddEmptyWords() " << Convert(wcWord, std::string()) );

   InitData::Languages::eLanguages FirstExistLang;

   FillBaseDictionaryPart<true, true>( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic, true, true, FirstExistLang );

   if( FillVirtualPartInSearch(wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic) )
      return true;

   //Если не нашлось ни одного основоного словаря в котором могут присутствовать все символы слова, то мы могли
   //добавить такое слово в словарь по умолчанию (из первого языка)
   if( FirstExistLang == InitData::Languages::Undefined )
   {
      if( m_LangToLangBasedDicts[0].pSmart->GetWordInfo(wcWordLower, wcWord, IsFirstLetterUpperCase, p_l_WordInfo, false, true) )
         return true;

      //Слово нигде не встречается и к тому же опредилить его параметры автоматически мы не можем
      //Добавляем пустую информацию по нему в словарь
      AddEmptyData( m_LangToLangBasedDicts[0].pSmart, wcWord, wcWordLower, IsFirstLetterUpperCase, *p_l_WordInfo );

      return true;
   }

   //Слово нигде не встречается и к тому же опредилить его параметры автоматически мы не можем
   //Добавляем пустую информацию по нему в словарь
   AddEmptyData( m_LangToLangBasedDicts[FirstExistLang].pSmart, wcWord, wcWordLower, IsFirstLetterUpperCase, *p_l_WordInfo );

   return true;
}
///////////////////////////////////////////////////////////////////////////////

void CMALanguagesManager::AppendOneRoot( TWIList &WIList )
{
   typedef std::set<TDictionaryIndex> TDISet;
   typedef std::list<TDictionaryIndex> TDIList;

   TDISet BaseDI;

   //Заполняем список уникальными IDSource, для исключения лишних вызовов OneRoot
   TWIList::iterator I;
   TWordInfo::iterator J;
 
   for( I = WIList.begin(); I != WIList.end(); ++I )
      for( J = I->begin(); J != I->end(); ++J )
         BaseDI.insert( J->m_WordIndex.m_DictIndex );

   //Получаем список однокоренных IDSource
   TDISet OneRootDI;
   TDIList DIList;
   TDISet::iterator IDISet;
   TDIList::iterator IDIList;

   for( IDISet = BaseDI.begin(); IDISet != BaseDI.end(); ++IDISet )
   {
     if( m_pOneRoot->SetDictionaryIndex(*IDISet) )
     {
         m_pOneRoot->Get( &DIList );

         for( IDIList = DIList.begin(); IDIList != DIList.end(); ++IDIList )
            if( BaseDI.find( *IDIList ) == BaseDI.end() ) //Так же необходимо проверить отсутсвие в основном списке
               OneRootDI.insert(*IDIList);
     }
   }

   //Опредиляем морфоинформацию и добавляем в WIList
   SWordInfo WordInfo;
   SWordIndex WordIndex;

   TDictNumberToSmartDictionary::const_iterator IDITOSD;

   for( IDISet = OneRootDI.begin(); IDISet != OneRootDI.end(); ++IDISet )
   {
      IDITOSD = m_DictNumberToSmartDictionary.find( IDISet->GetDictionaryNumber() );

      if( IDITOSD != m_DictNumberToSmartDictionary.end() )
      {
         WordIndex.m_DictIndex = *IDISet;
         WordIndex.m_IDForm = 0;

         if( IDITOSD->second->GetWordInfo( &WordIndex, &WordInfo) == TRUE )
         {
            WIList.push_back( TWIList::value_type() );
            WIList.back().push_back( WordInfo );
         }
      }
   }       
}
///////////////////////////////////////////////////////////////////////////////

void CMALanguagesManager::AppendUndepDict( TWIList &WIList )
{
   //Заполняем список уникальными IDSource, для исключения лишних вызовов OneRoot
   TWIList::iterator I;
   TWordInfo::iterator J;
   TWIList WIListTmp;

   for( I = WIList.begin(); I != WIList.end(); ++I )
      for( J = I->begin(); J != I->end(); ++J )
         if( J->m_WordIndex.m_DictIndex.GetDictionaryNumber() == DATA_TYPE::NAMES::ednRomanNumeric )
         {
            //Если мы имеем дело со словарём римских чисел, то автоматически добавляем арабскую интерпретацию числа
            SWordIndex CurWI;
            CurWI.m_DictIndex.SetDictionaryNumber( DATA_TYPE::NAMES::ednNumeric );
            CurWI.m_DictIndex.SetId( J->m_WordIndex.m_DictIndex.GetId() );

            WIListTmp.push_back( std::list<TWordInfo>::value_type() );
            WIListTmp.back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );

            if( !m_ObjectIDToMorphoAnal[InitData::ObjectID::ArabicNumbers]->GetWordInfo(&CurWI, &WIListTmp.back().back()) )
               WIListTmp.pop_back();
         }
         else if( J->m_WordIndex.m_DictIndex.GetDictionaryNumber() == DATA_TYPE::NAMES::ednNumeric )
         {
            //Если мы имеем дело со словарём арабских чисел, то автоматически добавляем римскую интерпретацию числа
            SWordIndex CurWI;
            CurWI.m_DictIndex.SetDictionaryNumber( DATA_TYPE::NAMES::ednRomanNumeric );
            CurWI.m_DictIndex.SetId( J->m_WordIndex.m_DictIndex.GetId() );

            WIListTmp.push_back( std::list<TWordInfo>::value_type() );
            WIListTmp.back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );

            if( !m_ObjectIDToMorphoAnal[InitData::ObjectID::RomanNumbers]->GetWordInfo(&CurWI, &WIListTmp.back().back()) )
               WIListTmp.pop_back();
         }
   
   WIList.splice( WIList.end(), WIListTmp );
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::GetWordInfoImpl(
   /*in*/const wchar_t* wcWord, 
   const wchar_t* wcWordLower,
   bool IsFirstLetterUpperCase,
   /*out*/TWIList* p_l_WordInfo,
   /*in*/EFindZone eFindZone,
   const CSmartDictionary::IDeterminationWordCheck &DWC,
   bool IsAddEmptyData,
   const TDictsCharacteristic &DictsCharacteristic
)
{
   APL_ASSERT_PTR( wcWord );
   APL_ASSERT_PTR( wcWordLower );
   APL_ASSERT_PTR( p_l_WordInfo );
   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);
   
   bool Result = false;

   p_l_WordInfo->clear();

   switch( eFindZone )
   {
   case efzFullAddNewWords:
      if( GetAnalyseParams() != 0 && GetAnalyseParams()->GetAnalyseMod()== eamQuery && GetAnalyseParams()->GetCurrentSystemMode() == ecsmLocal) 
         Result = GetWordInfo_OnlySearch<false>( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic );
      else
         Result = GetWordInfo_FullAddNewWords<false>( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DWC, IsAddEmptyData, DictsCharacteristic );
      break;
      
   case efzOnlySearch:	
      if( GetAnalyseParams() != 0 && GetAnalyseParams()->GetAnalyseMod()== eamQuery && GetAnalyseParams()->GetCurrentSystemMode() == ecsmGlobal) 
         Result = GetWordInfo_FullAddNewWords<false>( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DWC, IsAddEmptyData, DictsCharacteristic );         
      else
         Result = GetWordInfo_OnlySearch<false>( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic );
      break;

   case efzAbbreviation:
      Result = GetWordInfo_Abbreviation( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic );
      break;

   case efzBaseDictionary:
      Result = GetWordInfo_BaseDictionary( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic );
      break;

   case efzNewWord:
      Result = GetWordInfo_NewWord( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic );
      break;
  
   case efzBaseAndAbbreviation:
      Result = GetWordInfo_BaseAndAbbreviation( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DictsCharacteristic );
      break;

   case efzFullAddNewWordsCompound:
      Result = GetWordInfo_FullAddNewWords<true>( wcWord, wcWordLower, IsFirstLetterUpperCase, p_l_WordInfo, DWC, IsAddEmptyData, DictsCharacteristic );
      break;

   default:
      SS_THROW( L"Была указана неизвестная область поиска EFindZone" );
   };
   
#ifndef DISABLE_MORPHO_SORTING
   //Сортируем морфотипы - долгая операция
   p_l_WordInfo->sort( TDictionaryIndexCmp() ); 
#endif


   return Result;
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::GetWordInfo/*ByWord*/(
   /*in*/const wchar_t* wcWord, 
   /*out*/TWIList* p_l_WordInfo,
   /*in*/EFindZone eFindZone /*= efzFullAddNewWords*/
   )
{
   APL_ASSERT_PTR( wcWord );
   APL_ASSERT_PTR( p_l_WordInfo );
   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);

   Detail::TSharedObjectMutex::LockRead Guard( m_SharedMutex );

   SS_TRY
   {
      CheckNewWordsTables();
      m_CurWordInfo.Fill( wcWord );
      TDictsCharacteristic DictsCharacteristic( m_pSymbolCharacteristic->GetFlags(wcWord) );

      return GetWordInfoImpl(wcWord, m_CurWordInfo.LowerBuf, m_CurWordInfo.IsFirstLetterUpperCase, p_l_WordInfo, eFindZone, m_StandartDeterminationWordCheck, true, DictsCharacteristic);
   }
   SS_CATCH( const_cast<wchar_t*>(wcWord) );

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::GenetiveCompoundWord(
   const wchar_t* wcWord,
   const wchar_t* wcWordLower,
   bool IsFirstLetterUpperCase,
   size_t WordLen,
   IUnit * pUnit,
   EFindZone eFindZone,
   const TDictsCharacteristic &DictsCharacteristic
)
{
   static const wchar_t GenetivePostfix[] = L"'s";

   //Если слово оканчивается на 's то разбиваем обработку на две части: основа слова и 's
   if( 
      WordLen <= APL_ARRSIZE(GenetivePostfix) - 1 ||  
      memcmp( wcWordLower + WordLen - (APL_ARRSIZE(GenetivePostfix) - 1), GenetivePostfix, (APL_ARRSIZE(GenetivePostfix) - 1) * sizeof(*GenetivePostfix)) != 0
   )
      return false;

   TWIList WIList;
   TWIList::iterator I;
   TWordInfo::iterator J;
   std::wstring Base(wcWord, wcWord + WordLen - (APL_ARRSIZE(GenetivePostfix) - 1) );
   std::wstring BaseLower(wcWordLower, wcWordLower + WordLen - (APL_ARRSIZE(GenetivePostfix) - 1) );

   //Если есть основа в словаре или можно найти новое слово существительное
   if( !GetWordInfoImpl(Base.c_str(), BaseLower.c_str(), IsFirstLetterUpperCase, &WIList, eFindZone, m_OnlyNounDeterminationWordCheck, false, m_pSymbolCharacteristic->GetFlags(Base)) )
      return false;

   //pUnit получает характеристику основы но с CaseType = Genetive, и к нему 
   //добавляются потомки: основа и "'s" с оригинальными характеристиками
   CMorphoFeature MorphoFeature;
   IUnit *pFirstUnit = m_pBlackBoxTextStorage->CreateUnit();
   int TmpMorphoInfo;
   int TmpSpecialMorpho;

   pFirstUnit->SetWord( Base.c_str() );
   Detail::FillUnit( WIList, pFirstUnit, m_pBlackBoxTextStorage );

   pUnit->AddUnitVertical( pFirstUnit );

   //Добавляем характеристику CaseType=Genetive 
   for( I = WIList.begin(); I != WIList.end(); ++I )
      for( J = I->begin(); J != I->end(); ++J )
      {
         m_pAMConverter->EnCode( 
            J->m_WordIndex.m_DictIndex.GetDictionaryNumber(), 
            J->m_MorphoTypeInfo.m_MorphoInfo, 
            J->m_SpecialMorpho,
            &MorphoFeature
         );

         MorphoFeature.m_CaseType.Assign( SS::Core::Features::Values::CaseType.ctGenitive );

         m_pAMConverter->Code( 
            J->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
            &MorphoFeature,
            &TmpMorphoInfo, 
            &TmpSpecialMorpho
         );

         J->m_MorphoTypeInfo.m_MorphoInfo = static_cast<TMorphoInfo>(TmpMorphoInfo);
         J->m_SpecialMorpho = static_cast<TSpecialMorpho>(TmpSpecialMorpho);
      }
      
   Detail::FillUnit( WIList, pUnit, m_pBlackBoxTextStorage );

   wchar_t TmpStr[2] = {};
   wchar_t TmpStrLower[2] = {};
   IUnit *pCurUnit;

   for( size_t i = 0; i < APL_ARRSIZE(GenetivePostfix) - 1; ++i )
   {
      TmpStr[0] = wcWord[WordLen - (APL_ARRSIZE(GenetivePostfix) - 1) + i];
      TmpStrLower[0] = wcWordLower[WordLen - (APL_ARRSIZE(GenetivePostfix) - 1) + i];

      APL_CHECK( m_ObjectIDToMorphoAnal[InitData::ObjectID::Symbols]->GetWordInfo(TmpStrLower, TmpStr, IsFirstLetterUpperCase, &WIList) ); 

      pCurUnit = m_pBlackBoxTextStorage->CreateUnit();
      pCurUnit->SetWord( TmpStr );
      Detail::FillUnit( WIList, pCurUnit, m_pBlackBoxTextStorage );

      pFirstUnit->AddUnitHorizontal(pCurUnit);
      pFirstUnit = pCurUnit;
   }
                             
   return true;
}
///////////////////////////////////////////////////////////////////////////////   

bool CMALanguagesManager::NegateCompoundWord(
   const wchar_t* wcWord,
   const wchar_t* wcWordLower,
   bool IsFirstLetterUpperCase,
   size_t WordLen,
   IUnit * pUnit
)
{
   static const wchar_t NegatePostfix[] = L"n't";

   //Если слово оканчивается на n't то обработываем только основу слова
   if( 
      WordLen <= APL_ARRSIZE(NegatePostfix) - 1 ||  
      memcmp( wcWordLower + WordLen - (APL_ARRSIZE(NegatePostfix) - 1), NegatePostfix, (APL_ARRSIZE(NegatePostfix) - 1) * sizeof(*NegatePostfix)) != 0
   )
      return false;
   
   std::wstring BaseLower(wcWordLower, wcWordLower + WordLen - (APL_ARRSIZE(NegatePostfix) - 1) );
   Detail::TConstData::TString2String::const_iterator IFindWord = m_pConstData->GetEngNegationReduction().find(BaseLower);

   if( IFindWord == m_pConstData->GetEngNegationReduction().end() )
      return false;

   TWIList WIList;
   std::wstring Base( IFindWord->second );

   //Слово в верхнем регистре
   if( IsFirstLetterUpperCase ) CharUpperBuffW( &Base[0], 1 );

   //Слово должно обязательно присутствовать в словаре
   if( !m_LangToLangBasedDicts[InitData::Languages::English].pSmart->GetWordInfo(IFindWord->second.c_str(), Base.c_str(), IsFirstLetterUpperCase, &WIList, true, false) )
      APL_THROW( _T("Слово ") << IFindWord->second << _T(" не найдено в основном словаре") );
   
   //pUnit получает характеристику основы с ntNegation
   CMorphoFeature MorphoFeature;
   int TmpMorphoInfo;
   int TmpSpecialMorpho;
   TWIList::iterator I;
   TWordInfo::iterator J;

   pUnit->SetWord( Base.c_str() );

   //Добавляем характеристику ntNegation 
   for( I = WIList.begin(); I != WIList.end(); ++I )
      for( J = I->begin(); J != I->end(); ++J )
      {
         m_pAMConverter->EnCode( 
            J->m_WordIndex.m_DictIndex.GetDictionaryNumber(), 
            J->m_MorphoTypeInfo.m_MorphoInfo, 
            J->m_SpecialMorpho,
            &MorphoFeature
         );

         MorphoFeature.m_NegativeType.Assign( SS::Core::Features::Values::NegativeType.ntNegation );

         m_pAMConverter->Code( 
            J->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
            &MorphoFeature,
            &TmpMorphoInfo, 
            &TmpSpecialMorpho
         );

         J->m_MorphoTypeInfo.m_MorphoInfo = static_cast<TMorphoInfo>(TmpMorphoInfo);
         J->m_SpecialMorpho = static_cast<TSpecialMorpho>(TmpSpecialMorpho);
      }
      
   Detail::FillUnit( WIList, pUnit, m_pBlackBoxTextStorage );
                            
   return true;
}
///////////////////////////////////////////////////////////////////////////////   

bool CMALanguagesManager::HyphenCompoundWord(
   const wchar_t* wcWord,
   const wchar_t* wcWordLower,
   bool IsFirstLetterUpperCase,
   size_t WordLen,
   IUnit * pUnit,
   EFindZone eFindZone,
   const TDictsCharacteristic &DictsCharacteristic
)
{
   typedef TCompoundWordAnalysis::TWordPartInfoList TWordPartInfoList;
   typedef TCompoundWordAnalysis::TFeatureList TFeatureList;

   const TCHAR *pEnd = wcWordLower + WordLen;
   const TCHAR *pBegWord = wcWordLower;
   const TCHAR *pCur = wcWordLower;
   TWordPartInfoList WordPartInfoList;
   TWordPartInfoList::iterator I;

   WordPartInfoList.reserve(4);

   //Находим все дефисы в слове
   for( ; pCur != pEnd; ++pCur )
      if( *pCur == _T('-') )
      {   
         if( pBegWord == pCur ) //Если одна из частей пустая
            return false;
         
         WordPartInfoList.push_back(TWordPartInfoList::value_type());   
         WordPartInfoList.back().WordLower.assign( pBegWord, pCur );
         WordPartInfoList.back().WordOrigin.assign( wcWord + (pBegWord - wcWordLower), wcWord + (pCur - wcWordLower) );
         pBegWord = pCur + 1;
      }
  
   if( WordPartInfoList.empty() )
      return false;

   //Мы нашли хотябы один дефис и добавили левую часть, значит осталась и правая часть
   if( pBegWord == pEnd ) //Дефис последний символ в слове
         return false;

   WordPartInfoList.push_back(TWordPartInfoList::value_type());   
   WordPartInfoList.back().WordLower.assign( pBegWord, pEnd );
   WordPartInfoList.back().WordOrigin.assign( wcWord + (pBegWord - wcWordLower), wcWord + WordLen );

   //Зона поиска должна быть скорректирована
   EFindZone eCurFindZone = CorrectFindZoneForPartSearch(eFindZone);

   //Находим морфоинформацию частей слова
   for( I = WordPartInfoList.begin(); I != WordPartInfoList.end(); ++I )
   {
      if( !GetWordInfoImpl(I->WordOrigin.c_str(), I->WordLower.c_str(), IsFirstLetterUpperCase, &I->MorphoInfo, eCurFindZone, m_StandartDeterminationWordCheck, false, m_pSymbolCharacteristic->GetFlags(I->WordOrigin) ) )
         return false;

      APL_ASSERT( !I->MorphoInfo.empty() && !I->MorphoInfo.front().empty() );
   }

   //Узнаём язык
   I = WordPartInfoList.begin();
   EAnalyseLanguage Lang = I->MorphoInfo.front().front().m_WordIndex.m_DictIndex.GetLanguage();
   
   //Проверяем чтобы у всех частей был один и тот же язык
   for( ++I; I != WordPartInfoList.end(); ++I )
      if( Lang != I->MorphoInfo.front().front().m_WordIndex.m_DictIndex.GetLanguage() )
         return false;

   TWIList HypenMorpho;
   TFeatureList FeatureList;
   TDictionaryIndex NewDI;

   APL_CHECK( m_ObjectIDToMorphoAnal[InitData::ObjectID::Symbols]->GetWordInfo(_T("-"), _T("-"), false, &HypenMorpho) );    

   if( Lang == ealRussian )
   {
      if( WordPartInfoList.size() != 2 )
         return false;

      FeatureList.push_back(TFeatureList::value_type());
      if(!m_CompoundWordAnalysis.HyphenAnalisisRussian(WordPartInfoList[0].WordLower, WordPartInfoList[1].WordLower, WordPartInfoList[0].MorphoInfo, WordPartInfoList[1].MorphoInfo, m_pAMConverter, FeatureList.back()) )
         return false;

      NewDI.SetDictionaryNumber(DATA_TYPE::NAMES::ednRCombinatory);
   }
   else if( Lang == ealEnglish ) 
   {
      if( WordPartInfoList.size() > 4 )
         return false;
      
      if(!m_CompoundWordAnalysis.HyphenAnalisisEnglish(wcWord, WordPartInfoList, m_pAMConverter, FeatureList) )
         return false;

      NewDI.SetDictionaryNumber(DATA_TYPE::NAMES::ednRCombinatory);

   }
   else   
      return false;

#ifdef CREATE_CHILDREN_UNITS_IN_HYPEN_DETECTION //Создаём дочерние юниты  
   IUnit *pFirstUnit = m_pBlackBoxTextStorage->CreateUnit();
   IUnit *pCurUnit;
   
   I = WordPartInfoList.begin();

   pFirstUnit->SetWord( I->WordOrigin.c_str() );  
   Detail::FillUnit( I->MorphoInfo, pFirstUnit, m_pBlackBoxTextStorage );
   pUnit->AddUnitVertical( pFirstUnit );
   
   for( ++I; I != WordPartInfoList.end(); ++I )
   {
      pCurUnit = m_pBlackBoxTextStorage->CreateUnit();
      pCurUnit->SetWord( _T("-") );  
      Detail::FillUnit( HypenMorpho, pCurUnit, m_pBlackBoxTextStorage );
      pFirstUnit->AddUnitHorizontal( pCurUnit );
      pFirstUnit = pCurUnit;

      pCurUnit = m_pBlackBoxTextStorage->CreateUnit();
      pCurUnit->SetWord( I->WordOrigin.c_str() );  
      Detail::FillUnit( I->MorphoInfo, pCurUnit, m_pBlackBoxTextStorage );
      pFirstUnit->AddUnitHorizontal( pCurUnit );
      pFirstUnit = pCurUnit;
   }
#endif

   Detail::FillUnit( FeatureList, 0, NewDI, pUnit, m_pBlackBoxTextStorage, m_pAMConverter );

   return true;
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::GetWordInfo/*ByWord*/(
   /*in*/const wchar_t* wcWord, 
   /*in out*/SS::Interface::Core::BlackBox::IUnit * pUnit,
   /*in*/EFindZone eFindZone/* = efzFullAddNewWords*/
)
{
   APL_ASSERT_PTR( pUnit );
   APL_ASSERT_PTR( wcWord );
   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);

   Detail::TSharedObjectMutex::LockRead Guard( m_SharedMutex );
   
   SS_TRY
   {
      CheckNewWordsTables();

      m_CurWordInfo.Fill( wcWord );
      TDictsCharacteristic DictsCharacteristic( m_pSymbolCharacteristic->GetFlags(wcWord) );

      if( !DictsCharacteristic.test(InitData::ObjectID::ContainHypen) && HyphenCompoundWord(wcWord, m_CurWordInfo.LowerBuf, m_CurWordInfo.IsFirstLetterUpperCase, m_CurWordInfo.Len, pUnit, eFindZone, DictsCharacteristic) )
      {
         pUnit->SetWord( wcWord );
         return true;
      }

      if( !DictsCharacteristic.test(InitData::ObjectID::ContainQuotationS) && GenetiveCompoundWord(wcWord, m_CurWordInfo.LowerBuf, m_CurWordInfo.IsFirstLetterUpperCase, m_CurWordInfo.Len, pUnit, eFindZone, DictsCharacteristic) )
      {
         //Делать pUnit->SetWord( wcWord ); надо т.к. GenetiveCompoundWord уже изменяет слово
         return true;
      }
      
      if( !DictsCharacteristic.test(InitData::ObjectID::ContainQuotationNT) && NegateCompoundWord(wcWord, m_CurWordInfo.LowerBuf, m_CurWordInfo.IsFirstLetterUpperCase, m_CurWordInfo.Len, pUnit) )
      {
         //Делать pUnit->SetWord( wcWord ); надо т.к. NegateCompoundWord уже изменяет слово
         return true;
      }

      TWIList WIList;

      //Добавляем полное слово
      GetWordInfoImpl( wcWord, m_CurWordInfo.LowerBuf, m_CurWordInfo.IsFirstLetterUpperCase, &WIList, eFindZone, m_StandartDeterminationWordCheck, true, DictsCharacteristic );
      
      APL_ASSERT( WIList.empty() || !WIList.back().empty() );

      //Обработаем отдельно составные слова (если длина слова в морфонформации меньше искомого слова то мы нашли составное слово)
      if( eFindZone == efzFullAddNewWordsCompound && !WIList.empty() &&  WIList.back().back().m_wWord.length() < m_CurWordInfo.Len )
      {
         Detail::FillSubUnits( WIList, pUnit, m_pBlackBoxTextStorage );
         Detail::FillUnit( WIList.back(), pUnit, m_pBlackBoxTextStorage );
      }
      else
         Detail::FillUnit( WIList, pUnit, m_pBlackBoxTextStorage );
      
      pUnit->SetWord( wcWord );
      return !WIList.empty();
   }
   SS_CATCH( const_cast<wchar_t*>(wcWord) );

   return false;
}
///////////////////////////////////////////////////////////////////////////////

void CMALanguagesManager::NextText()
{
   DEBUG_MSG_LOG5("CMALanguagesManager::NextText() " );

   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);

   Detail::TSharedObjectMutex::LockWrite Guard( m_SharedMutex );

   SS_TRY
   {    
      for( size_t i = 0; i < InitData::Languages::COUNT; ++i )
         m_LangToLangBasedDicts[i].pSmart->ClearNewWordsCache();

   }
   SS_CATCH( L"" );
}
///////////////////////////////////////////////////////////////////////////////

void CMALanguagesManager::SetAnalyseParamsNoLock(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams)
{
   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);

   SS_TRY
   {   
      IMorphologyAnalyser::SetAnalyseParams( pAnalyseParams );

	   m_UseNewWordsTables = pAnalyseParams == 0 || pAnalyseParams->GetCurrentSystemMode() == ecsmLocal;
		
	   if( m_UseNewWordsTables )
      {
         for( size_t i = 0; i < InitData::Languages::COUNT; ++i )
            m_LangToLangBasedDicts[i].pSmart->SetNewWordsTable( m_IndexTables[i].Get() );
      }
      else
      {
         for( size_t i = 0; i < InitData::Languages::COUNT; ++i )
            m_LangToLangBasedDicts[i].pSmart->SetNewWordsTable( 0 );
      }
   }
   SS_CATCH( L"" );

   DEBUG_MSG_LOG5(
      "CMALanguagesManager::SetAnalyseParamsNoLock() " << 
      (pAnalyseParams != 0? AnalyseModsDescription(pAnalyseParams->GetAnalyseMod()) + ":" + CurrentSystemModeDescription(pAnalyseParams->GetCurrentSystemMode()): "pAnalyseParams == 0") << 
      " Использовать таблицы новых слов = " << m_UseNewWordsTables
   );
}
///////////////////////////////////////////////////////////////////////////////

void CMALanguagesManager::SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams)
{
   Detail::TSharedObjectMutex::LockWrite Guard( m_SharedMutex );
   SetAnalyseParamsNoLock( pAnalyseParams );
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::ToBinary()
{
   DEBUG_MSG_LOG5("CMALanguagesManager::ToBinary()");

   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);

   Detail::TSharedObjectMutex::LockWrite Guard( m_SharedMutex );

   bool RetVal = true;

   SS_TRY
   {    
      for( size_t i = 0; i < InitData::Languages::COUNT; ++i )
      {
         if( !m_LangToLangBasedDicts[i].pSmart->ToBinary() )
            RetVal = false;

         if( !m_LangToLangBasedDicts[i].pAbbreviations->Reload() )
            RetVal = false;
      }
   }
   SS_CATCH( L"" );

   return RetVal;
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::ReLoad()
{
   DEBUG_MSG_LOG5("CMALanguagesManager::ReLoad()");

   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);

   Detail::TSharedObjectMutex::LockWrite Guard( m_SharedMutex );

   bool RetVal = true;

   SS_TRY
   {    
      for( size_t i = 0; i < InitData::Languages::COUNT; ++i )
      {
         if( !m_LangToLangBasedDicts[i].pSmart->ReLoad() )
            RetVal = false;

         if( !m_LangToLangBasedDicts[i].pAbbreviations->Reload() )
            RetVal = false;
      }
   }
   SS_CATCH( L"" );

   return RetVal;
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::Load()
{
   DEBUG_MSG_LOG5("CMALanguagesManager::Load()");

   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);

   Detail::TSharedObjectMutex::LockWrite Guard( m_SharedMutex );

   bool RetVal = true;

   SS_TRY
   {    
      for( size_t i = 0; i < InitData::Languages::COUNT; ++i )
      {
         if( !m_LangToLangBasedDicts[i].pSmart->Load() )
            RetVal = false;

         //Абревиатуры уже точно загружены
      }
   }
   SS_CATCH( L"" );

   return RetVal;
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::Clear()
{
   DEBUG_MSG_LOG5("CMALanguagesManager::Clear()");

   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0);

   Detail::TSharedObjectMutex::LockWrite Guard( m_SharedMutex );

   SS_TRY
   {    
      for( size_t i = 0; i < InitData::Languages::COUNT; ++i )
         m_LangToLangBasedDicts[i].pSmart->Clear();
   }
   SS_CATCH( L"" );

   return true;
}                                
///////////////////////////////////////////////////////////////////////////////

void CMALanguagesManager::OpenNewWordsTables( DBMS::IDataBase *pDB )
{
   DEBUG_MSG_LOG5("MALanguagesManager::OpenNewWordsTables()");

   APL_ASSERT_PTR( pDB );

   Detail::TSharedObjectMutex::LockWrite Guard( m_SharedMutex );

   SS_TRY
   {    
      for( size_t CurLang = 0; CurLang < InitData::Languages::COUNT; ++CurLang )
         if ( m_IndexTables[CurLang].Open(pDB, m_IntdexTableNames[CurLang].c_str()) != S_OK )
            SS_THROW( L"Ошибка при получениии указателя на индексную базу новых слов" );
      
     SetAnalyseParamsNoLock(GetAnalyseParams());
  }
   SS_CATCH( L"" );
}
///////////////////////////////////////////////////////////////////////////////

void CMALanguagesManager::CloseNewWordsTables()
{
   DEBUG_MSG_LOG5("MALanguagesManager::CloseNewWordsTables()");

   Detail::TSharedObjectMutex::LockWrite Guard( m_SharedMutex );

   SS_TRY
   {    
      for( size_t CurLang = 0; CurLang < InitData::Languages::COUNT; ++CurLang )
         m_IndexTables[CurLang].Close();

      SetAnalyseParamsNoLock(GetAnalyseParams());
   }
   SS_CATCH( L"" );
}
///////////////////////////////////////////////////////////////////////////////

bool CMALanguagesManager::GetDeterminationWordInfo(
   const wchar_t* wcWord, 
   CSmartDictionary::SHypothesisInfo &HypothesisInfo
)
{
   DEBUG_MSG_LOG5("CMALanguagesManager::GetDeterminationWordInfo() " << Convert(wcWord, std::string()) );

   APL_ASSERT_PTR( wcWord );
   APL_ASSERT( m_pSymbolCharacteristic != 0 && !m_DictNumberToMorphoAnal.Empty() && m_pOneRoot != 0 );

   TStandartDeterminationWordCheck SDWC;

   Detail::TSharedObjectMutex::LockRead Guard( m_SharedMutex );

   HypothesisInfo.Clear();

   SS_TRY
   {
      CheckNewWordsTables();

      m_CurWordInfo.Fill( wcWord );

      //Опредиляем область поиска 
      TDictsCharacteristic DictsCharacteristic( m_pSymbolCharacteristic->GetFlags(wcWord) );
      size_t i;

      //проверим сначала есть ли такое слово в словарях зависимых от языка
      for( i = 0; i < InitData::Languages::COUNT; ++i )
         if( DictsCharacteristic.test( InitData::Convert(static_cast<InitData::Languages::eLanguages>(i), InitData::LangDepDict::Smart)) )
            return m_LangToLangBasedDicts[i].pSmart->DeterminationWordInfo( wcWord, m_CurWordInfo.LowerBuf, m_CurWordInfo.IsFirstLetterUpperCase, HypothesisInfo, SDWC ); 
   }
   SS_CATCH( L"" );

   return false;
}

} //namespace Dictionary
} //namespace SS
