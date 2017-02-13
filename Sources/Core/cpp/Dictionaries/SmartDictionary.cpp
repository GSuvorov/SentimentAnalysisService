#include "stdafx.h"

namespace SS{ 
namespace Dictionary{                                           

//CSmartDictionary
const wchar_t * const CSmartDictionary::szNominalTable = _T("NominalSourceForm");
const wchar_t * const CSmartDictionary::szProperTable = _T("ProperSourceForm");
const wchar_t * const CSmartDictionary::szEndingTable = _T("Ending");
const wchar_t * const CSmartDictionary::szNominalSourceToMorphoTable = _T("NominalSourceToMorpho");
const wchar_t * const CSmartDictionary::szProperSourceToMorphoTable = _T("ProperSourceToMorpho");
const wchar_t * const CSmartDictionary::szMorphoTypesTable = _T("MorphoTypes");

//CMinimalHypothesisTreeSmartDictionary
const wchar_t * const CMinimalHypothesisTreeSmartDictionary::szNewWordsDeductionTable = _T("NewWordsDeduction");



///////////////////////////////////////////////////////////////////////////////
// class CSmartDictionary
///////////////////////////////////////////////////////////////////////////////

//unsigned SSDStatistic::TotalFillWordInfo = 0;
//unsigned SSDStatistic::SuccessfulFillWordInfo = 0;
//unsigned SSDStatistic::TotalFillHypothesisInfo = 0;
//unsigned SSDStatistic::SuccessfulFillHypothesisInfo = 0;

namespace Detail
{
   struct TSelectIDType: public std::unary_function<CSmartDictionary::SHypothesisInfo::WordInfo, TIDType>
   {
      const TIDType &operator()( const CSmartDictionary::SHypothesisInfo::WordInfo &HWI ) const
      {
         return HWI.m_MorphoType.m_MorphoTypeInfo.m_IDType;
      }
   };


   struct TSelectIDForm: public std::unary_function<CSmartDictionary::SHypothesisInfo::WordInfo, TIDForm>
   {
      const TIDForm &operator()( const CSmartDictionary::SHypothesisInfo::WordInfo &HWI ) const
      {
         return HWI.m_MorphoType.m_IDForm;
      }
   };

   struct TSelectEndingLen: public std::unary_function<CSmartDictionary::SHypothesisInfo::WordInfo, int>
   {
      const int operator()( const CSmartDictionary::SHypothesisInfo::WordInfo &HWI ) const
      {
         return HWI.m_EndingLen;
      }
   };


   struct TIDTypeIDFormLess: public std::binary_function<CSmartDictionary::SHypothesisInfo::WordInfo, CSmartDictionary::SHypothesisInfo::WordInfo, bool>
   {
      bool operator()( const CSmartDictionary::SHypothesisInfo::WordInfo &HWI1, const CSmartDictionary::SHypothesisInfo::WordInfo &HWI2 ) const
      {
         TSelectIDType SelectIDType;
         TSelectIDForm SelectIDForm;
         
         return 
            (SelectIDType(HWI1) < SelectIDType(HWI2)) || 
            (  
               !(SelectIDType(HWI2) < SelectIDType(HWI1)) && 
               SelectIDForm(HWI1) < SelectIDForm(HWI2) 
            );
      }
   };

   struct TIDTypeEndingLenLess: public std::binary_function<CSmartDictionary::SHypothesisInfo::WordInfo, CSmartDictionary::SHypothesisInfo::WordInfo, bool>
   {
      bool operator()( const CSmartDictionary::SHypothesisInfo::WordInfo &HWI1, const CSmartDictionary::SHypothesisInfo::WordInfo &HWI2 ) const
      {
         TSelectIDType SelectIDType;
         TSelectEndingLen SelectEndingLen;

         return 
            (SelectIDType(HWI1) < SelectIDType(HWI2)) || 
            (  
            !(SelectIDType(HWI2) < SelectIDType(HWI1)) && 
            SelectEndingLen(HWI1) < SelectEndingLen(HWI2) 
            );
      }
   };
}
///////////////////////////////////////////////////////////////////////////////

void CSmartDictionary::SHypothesisInfo::UniqueIDTypeEndingLen()
{
   //Алгоритмическая сложность такого подхода N*log(N) + N. Может быть есть алгоритмы и порядка N*log(N),
   //но в данной ситуации и такая сложность удовлетворительна
   m_FormsInfoList.sort( Detail::TIDTypeEndingLenLess() );

   m_FormsInfoList.unique( 
      binary_compose_2arg(std::logical_and<bool>(),  
         binary_compose_1arg(std::equal_to<TIDType>(), Detail::TSelectIDType(), Detail::TSelectIDType()), 
         binary_compose_1arg(std::equal_to<int>(), Detail::TSelectEndingLen(), Detail::TSelectEndingLen())
      )
   );
}
///////////////////////////////////////////////////////////////////////////////

void CSmartDictionary::SHypothesisInfo::SortIDTypeIDForm()
{
   m_FormsInfoList.sort( Detail::TIDTypeIDFormLess() );
}
///////////////////////////////////////////////////////////////////////////////

void CSmartDictionary::SHypothesisInfo::UniqueIDTypeIDForm()
{
   SortIDTypeIDForm();

   m_FormsInfoList.unique(
      binary_compose_2arg(std::logical_and<bool>(),  
         binary_compose_1arg(std::equal_to<TIDType>(), Detail::TSelectIDType(), Detail::TSelectIDType()), 
         binary_compose_1arg(std::equal_to<TIDForm>(), Detail::TSelectIDForm(), Detail::TSelectIDForm())
      )
   );
}

///////////////////////////////////////////////////////////////////////////////
void CSmartDictionary::Init( 
      DBMS::IDataBase *pDataBase,
      IAMConverterMorpho *pAMConverter, const TCHAR *DBPath, 
      TDictNumber NominalDictionaryIndex,
      TDictNumber ProperDictionaryIndex, 
      TDictNumber NewWordsDictionaryIndex,
      int MinRecognizableWordLen
)
{
   APL_ASSERT_PTR( pDataBase );
   APL_ASSERT_PTR( pAMConverter );
   APL_ASSERT( DBPath != 0);

   m_pAMConverter = pAMConverter;
   m_NominalDictionaryIndex = NominalDictionaryIndex;
	m_ProperDictionaryIndex = ProperDictionaryIndex;  
   m_NewWordsDictionaryIndex = NewWordsDictionaryIndex; 
   m_MinRecognizableWordLen = MinRecognizableWordLen;

	SS_TRY
	{
		//Формируем пути к файлам с даными и инициализуем словари
      TString NominalInitPath(DBPath);
		TString ProperInitPath(DBPath);
		TString EndingInitPath(DBPath);
		TString MorphoTypesInitPath(DBPath);

		NominalInitPath += _T("\\");     NominalInitPath += szNominalTable;         NominalInitPath += _T("\\");
		ProperInitPath  += _T("\\");     ProperInitPath  += szProperTable;          ProperInitPath  += _T("\\");
		EndingInitPath  += _T("\\");     EndingInitPath  += szEndingTable;          EndingInitPath  += _T("\\");
		MorphoTypesInitPath += _T("\\"); MorphoTypesInitPath += szMorphoTypesTable; MorphoTypesInitPath += _T("\\");
		

		//Открыем таблицы для CPartOfWordsSearch
		if( m_pNominalDT.Open(pDataBase, szNominalTable) != S_OK )
			SS_THROW(L"Open NominalSourceForm");

		if( m_pProperDT.Open(pDataBase, szProperTable ) != S_OK )
			SS_THROW(L"Open ProperSourceForm");

		if( m_pEndingDT.Open(pDataBase, szEndingTable ) != S_OK )
			SS_THROW(L"Open EndingSourceForm");

      m_pNominalWS = CPartOfWordsSearchCreateManager::Create()->Construct(NominalInitPath.c_str(), m_pNominalDT.Get());
      m_pProperWS = CPartOfWordsSearchCreateManager::Create()->Construct(ProperInitPath.c_str(), m_pProperDT.Get());
      m_pEndingWS = CPartOfWordsSearchCreateManager::Create()->Construct(EndingInitPath.c_str(), m_pEndingDT.Get());
      m_pNominalSourceToMorpho = CSourceToTypeCreateManager::Create()->Construct(pDataBase, szNominalSourceToMorphoTable);
      m_pProperSourceToMorpho = CSourceToTypeCreateManager::Create()->Construct(pDataBase, szProperSourceToMorphoTable);
      m_pMorphoTypesSearch = CMorphoTypesSearchCreateManager::Create()->Construct(pDataBase, MorphoTypesInitPath.c_str());

      m_NewWords.SetTable( 0, m_NewWordsDictionaryIndex );
	}
	SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

bool CSmartDictionary::IsFindWord(
                const wchar_t* wcWord,
                const SPartOfWord  &IDEnding,
                TPartOfWordsSearchPtr pSourseWS, 
                TSourceToTypeConstPtr pSourceToType
) const
{
	TMTList	   MorphoTypeListBuf;
	TPOFVector::const_iterator ISource;
	
	//Для каждого окончания ищем возможный источник
	m_SourceVecBuf.clear();
	pSourseWS->GetByForm(wcWord, IDEnding.PositionInWord, &m_SourceVecBuf, TRUE);

	//Для каждого источника узнаём узнаём его IDType и затем проверяем есть ли такая комбинация IDEnding - IDTypes в
   //m_MorphoTypesSearch
	for( ISource = m_SourceVecBuf.begin(); ISource != m_SourceVecBuf.end(); ++ISource  )
	{
		Detail::CSourceToType::TTypeIndex TypeIndex = pSourceToType->Find(ISource->Id);
      
      APL_ASSERT( TypeIndex != Detail::CSourceToType::EmptyIndex() );

		if( m_pMorphoTypesSearch->GetByForm(TypeIndex, IDEnding.Id, &MorphoTypeListBuf) )
			return true;
	}

	return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CSmartDictionary::IsExistWord(/*in*/const wchar_t* wcWord)
{
	APL_ASSERT_PTR( wcWord );
   APL_ASSERT( *wcWord != L'\0' );
	APL_ASSERT( CheckLoad() );

	SS_TRY
	{
		TPOFVector::iterator IEnding;

		int SearchPos = static_cast<int>(std::wcslen( wcWord )) - 1;
		APL_ASSERT( SearchPos >= 0 );

 		//Ищем все возможные окончания
		m_EndingVecBuf.clear();
		m_pEndingWS->GetByForm(wcWord, SearchPos, &m_EndingVecBuf, FALSE);

		//Проходим по всем окончаниям и пытаемся найти соотв. основу
      for( IEnding = m_EndingVecBuf.begin(); IEnding != m_EndingVecBuf.end(); ++IEnding )
		{
			CorrectIDEnding(*IEnding, SearchPos );

			//Попробуем найти сначала имена нарицательные
			if( IsFindWord(wcWord, *IEnding, m_pNominalWS, m_pNominalSourceToMorpho) )
				return true;
			
			//Затем собственные
			if( IsFindWord(wcWord, *IEnding,  m_pProperWS, m_pProperSourceToMorpho) )
				return true;
		}
	}
	SS_CATCH(L"")

	return false;
}
///////////////////////////////////////////////////////////////////////////////

void CSmartDictionary::FillWordInfo(
      const wchar_t* wcWord,
      const wchar_t* wcWordOriginalCase,
      bool IsFirstLetterUpperCase,
      const SPartOfWord  &IDEnding,
      TPartOfWordsSearchPtr pSourseWS, 
      TSourceToTypeConstPtr pSourceToType,
      TDictNumber DictionaryIndex,
      TWIList &WIList
) const
{
	TMTList	 MorphoTypeListBuf;
	SWordInfo *pWordInfo;
	TPOFVector::const_iterator ISource;
	TMTList::iterator  IMTList;  

	WIList.clear();
	
   //Для каждого окончания ищем возможный источник
	m_SourceVecBuf.clear();
	pSourseWS->GetByForm(wcWord, IDEnding.PositionInWord, &m_SourceVecBuf, TRUE);

	//Для каждого источника узнаём узнаём его IDType и затем проверяем есть ли такая комбинация IDEnding - IDTypes в 
   //m_MorphoTypesSearch
	for( ISource = m_SourceVecBuf.begin(); ISource != m_SourceVecBuf.end(); ++ISource  )
	{
      Detail::CSourceToType::TTypeIndex TypeIndex = pSourceToType->Find(ISource->Id);

      APL_ASSERT( TypeIndex != Detail::CSourceToType::EmptyIndex() );

      //++SSDStatistic::TotalFillWordInfo;
		if( m_pMorphoTypesSearch->GetByForm(TypeIndex, IDEnding.Id, &MorphoTypeListBuf) )
		{
			//++SSDStatistic::SuccessfulFillWordInfo;
         //Мы нашли необходимую комбинацию теперь необходимо её добавить в WIList
			for( IMTList = MorphoTypeListBuf.begin(); IMTList != MorphoTypeListBuf.end(); ++IMTList )
			{
				APL_ASSERT( TypeIndex == IMTList->m_MorphoTypeInfo.m_IDType );
				
				//Добавляем новый элемент
				WIList.push_back( TWIList::value_type() );
				WIList.back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );
				pWordInfo = &WIList.back().back();

				//Заполняем его данными
            FillSWordInfo( *IMTList, wcWordOriginalCase, ISource->Id, DictionaryIndex, *pWordInfo, IsFirstLetterUpperCase );
			}
		}		
	}
}
///////////////////////////////////////////////////////////////////////////////

void CSmartDictionary::FillWordInfoFromNewWords(
      const wchar_t* wcWord,
      const wchar_t* wcWordOriginalCase,
      bool IsFirstLetterUpperCase,
      const SPartOfWord  &IDEnding,
      TWIList &WIList
) const
{
   APL_ASSERT_PTR(wcWord);
   APL_ASSERT( IDEnding.PositionInWord == -1 || IDEnding.PositionInWord < static_cast<int>(std::wcslen(wcWord)) );

   TString Source( wcWord, wcWord + IDEnding.PositionInWord + 1 );   
   TString SourceOriginal( wcWordOriginalCase, wcWordOriginalCase + IDEnding.PositionInWord + 1 );   

   Detail::CNewWordsCache::CCursor Curs = m_NewWords.FindBySourceLower(SourceOriginal); 
   SWordInfo *pWordInfo;
   TMTList::iterator  IMTList;  
   TMTList	MorphoTypeListBuf;

   WIList.clear();

   for( ; !Curs.End(); Curs.Next() )
   {
      if( m_pMorphoTypesSearch->GetByForm(Curs.Get().GetIDType(), IDEnding.Id, &MorphoTypeListBuf) )
      {
         //Мы нашли необходимую комбинацию теперь необходимо её добавить в WIList
         for( IMTList = MorphoTypeListBuf.begin(); IMTList != MorphoTypeListBuf.end(); ++IMTList )
         {
            //Добавляем новый элемент
            WIList.push_back( TWIList::value_type() );
            WIList.back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );
            pWordInfo = &WIList.back().back();

            //Заполняем его данными
            FillSWordInfo( *IMTList, wcWordOriginalCase, Curs.Get().GetIDSource().GetId(), m_NewWordsDictionaryIndex, *pWordInfo, IsFirstLetterUpperCase, true);
         }
      }		
   }
}
///////////////////////////////////////////////////////////////////////////////

bool CSmartDictionary::GetWordInfoFromHypothesis(
	const wchar_t* wcWord, 
	const wchar_t* wcWordLower,
	bool IsFirstLetterUpperCase,
	SHypothesisInfo& HypothesisInfo,
	TWIList* p_l_WordInfo
	)
{
	SS_TRY
	{
		TMTList	 MorphoTypeListBuf;
		TMTList::iterator  IMTList;
		CSmartDictionary::SHypothesisInfo::TFormsInfoList::iterator I;
		SWordInfo* pWordInfo;

		p_l_WordInfo->clear();

		//по всем гипотезам
		for(I = HypothesisInfo.m_FormsInfoList.begin(); I != HypothesisInfo.m_FormsInfoList.end(); ++I)
		{
			//по всем морфотипам
			if( m_pMorphoTypesSearch->GetByForm(I->m_MorphoType.m_MorphoTypeInfo.m_IDType, I->m_MorphoType.m_IDEnding, &MorphoTypeListBuf) )
			{
				for( IMTList = MorphoTypeListBuf.begin(); IMTList != MorphoTypeListBuf.end(); ++IMTList )
				{
					//Добавляем новый элемент
					p_l_WordInfo->push_back( TWIList::value_type() );
					p_l_WordInfo->back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );
					pWordInfo = &p_l_WordInfo->back().back();

					//Заполняем его данными
					FillSWordInfo( *IMTList, wcWord, GetNextSourceID(), m_NewWordsDictionaryIndex, *pWordInfo, IsFirstLetterUpperCase, true);
				}
			}		
		}
	}
	SS_CATCH(L"")

	return !p_l_WordInfo->empty();
}

bool CSmartDictionary::GetWordInfo/*ByWord*/(
	const wchar_t* wcWord, 
   const wchar_t* wcWordOriginalCase,
   bool IsFirstLetterUpperCase,
	TWIList* p_l_WordInfo, bool UseMainDictionary /*= true*/, bool UseNewWordsDictionary /*= true*/
)
{
	APL_ASSERT_PTR( wcWord );
   APL_ASSERT_PTR( wcWordOriginalCase );
	APL_ASSERT_PTR( p_l_WordInfo );
   APL_ASSERT( CheckLoad() );
   APL_ASSERT( UseMainDictionary || UseNewWordsDictionary ); //Странный вызов - искать нигде

	SS_TRY
	{
		TWIList WIListTmp;
		TPOFVector::iterator IEnding;
	
		int SearchPos = static_cast<int>(std::wcslen( wcWord )) - 1;
		p_l_WordInfo->clear();
		
		APL_ASSERT( SearchPos >= 0 );

		//Ищем все возможные окончания, причём если найдём в основном словаре, 
      //то в новых словах можно не искать
		m_EndingVecBuf.clear();
      m_pEndingWS->GetByForm(wcWord, SearchPos, &m_EndingVecBuf, FALSE);

      
      //Попробуем найти сначала имена нарицательные и собственные
      if( UseMainDictionary )
      {
         for( IEnding = m_EndingVecBuf.begin(); IEnding != m_EndingVecBuf.end(); ++IEnding )
         {
            CorrectIDEnding(*IEnding, SearchPos );
            
            FillWordInfo( wcWord, wcWordOriginalCase, IsFirstLetterUpperCase, *IEnding, m_pNominalWS, m_pNominalSourceToMorpho, m_NominalDictionaryIndex, WIListTmp );
            p_l_WordInfo->splice( p_l_WordInfo->end(), WIListTmp );

            FillWordInfo( wcWord, wcWordOriginalCase, IsFirstLetterUpperCase, *IEnding, m_pProperWS, m_pProperSourceToMorpho, m_ProperDictionaryIndex, WIListTmp );
            p_l_WordInfo->splice( p_l_WordInfo->end(), WIListTmp );
         }

         if( !p_l_WordInfo->empty() )
            return true;
      }

      //Затем те которые находятся в словаре новых слов
      if( UseNewWordsDictionary )
      {
         if( UseMainDictionary )
         {
            for( IEnding = m_EndingVecBuf.begin(); IEnding != m_EndingVecBuf.end(); ++IEnding )
            {
               //Окончания мы уже откорректировали
               FillWordInfoFromNewWords( wcWord, wcWordOriginalCase, IsFirstLetterUpperCase, *IEnding, WIListTmp );
               p_l_WordInfo->splice( p_l_WordInfo->end(), WIListTmp );
            }
         }
         else
         {
            for( IEnding = m_EndingVecBuf.begin(); IEnding != m_EndingVecBuf.end(); ++IEnding )
            {
               //Окончания ещё не откорректированы
               CorrectIDEnding(*IEnding, SearchPos );

               FillWordInfoFromNewWords( wcWord, wcWordOriginalCase, IsFirstLetterUpperCase, *IEnding, WIListTmp );
               p_l_WordInfo->splice( p_l_WordInfo->end(), WIListTmp );
            }
         }
      }
  	}
	SS_CATCH(L"")

	return !p_l_WordInfo->empty();
}

///////////////////////////////////////////////////////////////////////////////

bool CSmartDictionary::GetWordInfo/*ByWordIndex*/(
   const SWordIndex* pWordIndex, 
   SWordInfo* pWordInfo
)
{
	APL_ASSERT_PTR( pWordIndex );
	APL_ASSERT_PTR( pWordInfo );
   APL_ASSERT( CheckLoad() );

	SS_TRY
	{
		TSourceToTypeConstPtr pCurSourceToType(0);
		TPartOfWordsSearchPtr pSourceWS(0);
      
      Detail::CSourceToType::TTypeIndex IDType;
      unsigned int IDSource = pWordIndex->m_DictIndex.GetId();


		if( pWordIndex->m_DictIndex.GetDictionaryNumber() == m_NominalDictionaryIndex)
		{
			pCurSourceToType = m_pNominalSourceToMorpho;
			pSourceWS = m_pNominalWS;
		}
		else if( pWordIndex->m_DictIndex.GetDictionaryNumber() == m_ProperDictionaryIndex)
		{
			pCurSourceToType = m_pProperSourceToMorpho;
			pSourceWS = m_pProperWS;
		}
      else if( pWordIndex->m_DictIndex.GetDictionaryNumber() == m_NewWordsDictionaryIndex )
      {
         //Со словарём новых слов к сожалению так просто не поступишь
         //И поэтому они обрабатываются отдельно
         const Detail::CNewWordsCache::CSourceData *pData = m_NewWords.FindByIDSource( pWordIndex->m_DictIndex ); 

         if( pData == 0 )
            return false;

         IDType = pData->GetIDType();

         //Находим SMorphoType
         SMorphoType MorphoType;

         if( !m_pMorphoTypesSearch->GetMorphoTypeByIDTypeAndIDForm(IDType, pWordIndex->m_IDForm, &MorphoType) )
            return false;

         //Находим само слово
         std::wstring wcSource, wcEnding, OutStr;

         wcSource = pData->GetSource();

         if(  !m_pEndingWS->GetFormById(MorphoType.m_IDEnding, &wcEnding) )
            SS_THROW(L"Ошибка в данных CPartOfWordsSearch или CMorphoTypesSearch"); //Если мы нашли данные в m_MorphoTypesSearch то они должны быть и здесь

         OutStr = wcSource;
         OutStr += wcEnding;
         Detail::CorrectWordCase( OutStr, pWordIndex->m_DictIndex );

         FillSWordInfo(	MorphoType, OutStr, IDSource, pWordIndex->m_DictIndex.GetDictionaryNumber(), *pWordInfo, pWordIndex->m_DictIndex.GetFirstLetterUpperCase() );
         
         return true;
      }
		
		APL_ASSERT( pCurSourceToType != 0 && pSourceWS != 0 ); //Должен быть выбран словарь который точно есть у этого класса	

		//Находим IDType
		IDType = pCurSourceToType->Find( IDSource );

		if( IDType == Detail::CSourceToType::EmptyIndex() )
			return false;
		
		//Находим SMorphoType
		SMorphoType MorphoType;

		if( !m_pMorphoTypesSearch->GetMorphoTypeByIDTypeAndIDForm(IDType, pWordIndex->m_IDForm, &MorphoType) )
			return false;

		//Находим само слово
		std::wstring wcSource, wcEnding, OutStr;
		
      if( !pSourceWS->GetFormById(IDSource, &wcSource) )
         SS_THROW(L"Ошибка в данных CPartOfWordsSearch или CMorphoTypesSearch"); //Если мы нашли данные в m_MorphoTypesSearch то они должны быть и здесь

      if( !m_pEndingWS->GetFormById(MorphoType.m_IDEnding, &wcEnding) )
         SS_THROW(L"Ошибка в данных CPartOfWordsSearch или CMorphoTypesSearch"); //Если мы нашли данные в m_MorphoTypesSearch то они должны быть и здесь

      OutStr = wcSource;
      OutStr += wcEnding;
      Detail::CorrectWordCase( OutStr, pWordIndex->m_DictIndex );

		FillSWordInfo(MorphoType, OutStr, IDSource, pWordIndex->m_DictIndex.GetDictionaryNumber(), *pWordInfo, pWordIndex->m_DictIndex.GetFirstLetterUpperCase()  );

      return true;
	}
	SS_CATCH(L"")

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CSmartDictionary::GetWord(
      const SWordIndex* pWordIndex,
      wchar_t* wcWord
)
{
   APL_ASSERT_PTR( pWordIndex );
   APL_ASSERT_PTR( wcWord );
   APL_ASSERT( CheckLoad() );

   SS_TRY
   {
      TSourceToTypeConstPtr pCurSourceToType(0);
      TPartOfWordsSearchPtr pSourceWS(0);

      Detail::CSourceToType::TTypeIndex IDType;
      unsigned int IDSource = pWordIndex->m_DictIndex.GetId();

      if( pWordIndex->m_DictIndex.GetDictionaryNumber() == m_NominalDictionaryIndex)
      {
         pCurSourceToType = m_pNominalSourceToMorpho;
         pSourceWS = m_pNominalWS;
      }
      else if( pWordIndex->m_DictIndex.GetDictionaryNumber() == m_ProperDictionaryIndex)
      {
         pCurSourceToType = m_pProperSourceToMorpho;
         pSourceWS = m_pProperWS;
      }
      else if( pWordIndex->m_DictIndex.GetDictionaryNumber() == m_NewWordsDictionaryIndex )
      {
         //Со словарём новых слов к сожалению так просто не поступишь
         //И поэтому они обрабатываются отдельно
         const Detail::CNewWordsCache::CSourceData *pData = m_NewWords.FindByIDSource( pWordIndex->m_DictIndex ); 

         if( pData == 0 )
            return false;

         IDType = pData->GetIDType();

         //Находим SMorphoType
         SMorphoType MorphoType;

         if( !m_pMorphoTypesSearch->GetMorphoTypeByIDTypeAndIDForm(IDType, pWordIndex->m_IDForm, &MorphoType) )
            return false;

         //Находим само слово
         std::wstring wcSource, wcEnding;

         wcSource = pData->GetSource();

         if( !m_pEndingWS->GetFormById(MorphoType.m_IDEnding, &wcEnding) )
            SS_THROW(L"Ошибка в данных CPartOfWordsSearch или CMorphoTypesSearch"); //Если мы нашли данные в m_MorphoTypesSearch то они должны быть и здесь

         APL_ASSERT( wcSource.length() + wcEnding.length() < Constants::MAX_CHARS_IN_WORD );

         //Соединяем в одно слово и копируем в массив
         std::memcpy( wcWord, wcSource.c_str(), wcSource.length() * sizeof(*wcWord) );
         std::memcpy( wcWord + wcSource.length(), wcEnding.c_str(), ( wcEnding.length() + 1 ) * sizeof(*wcWord) );   // "+ 1" для того чтобы сразу скопировать '\0'
         Detail::CorrectWordCase( wcWord, pWordIndex->m_DictIndex );

         return true;
      }

      APL_ASSERT( pCurSourceToType != 0 && pSourceWS != 0 ); //Должен быть выбран словарь который точно есть у этого класса	

      //Находим IDType
      IDType = pCurSourceToType->Find( IDSource );

      if( IDType == Detail::CSourceToType::EmptyIndex() )
         return false;

      //Находим SMorphoType
      SMorphoType MorphoType;

      if( !m_pMorphoTypesSearch->GetMorphoTypeByIDTypeAndIDForm(IDType, pWordIndex->m_IDForm, &MorphoType) )
         return false;

      //Находим само слово
      std::wstring wcSource, wcEnding;

      if( !pSourceWS->GetFormById(IDSource, &wcSource) ) 
         SS_THROW(L"Ошибка в данных CPartOfWordsSearch или CMorphoTypesSearch"); //Если мы нашли данные в m_MorphoTypesSearch то они должны быть и здесь
             
      if( !m_pEndingWS->GetFormById(MorphoType.m_IDEnding, &wcEnding) )
         SS_THROW(L"Ошибка в данных CPartOfWordsSearch или CMorphoTypesSearch"); //Если мы нашли данные в m_MorphoTypesSearch то они должны быть и здесь

      APL_ASSERT( wcSource.length() + wcEnding.length() < Constants::MAX_CHARS_IN_WORD );

      //Соединяем в одно слово и копируем в массив
      std::memcpy( wcWord, wcSource.c_str(), wcSource.length() * sizeof(*wcWord) );
      std::memcpy( wcWord + wcSource.length(), wcEnding.c_str(), ( wcEnding.length() + 1 ) * sizeof(*wcWord) );   // "+ 1" для того чтобы сразу скопировать '\0'
      Detail::CorrectWordCase( wcWord, pWordIndex->m_DictIndex );
      
      return true;
   }
   SS_CATCH(L"")

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CSmartDictionary::GetWordSource(
   const TDictionaryIndex &DictIndex,
   wchar_t* wcWordSource
)
{
   APL_ASSERT_PTR( wcWordSource );
   APL_ASSERT( CheckLoad() );

   SS_TRY
   {
      TPartOfWordsSearchPtr pSourceWS(0);

      if( DictIndex.GetDictionaryNumber() == m_NominalDictionaryIndex)
      {
         pSourceWS = m_pNominalWS;
      }
      else if( DictIndex.GetDictionaryNumber() == m_ProperDictionaryIndex)
      {
         pSourceWS = m_pProperWS;
      }
      else if( DictIndex.GetDictionaryNumber() == m_NewWordsDictionaryIndex )
      {
         //Со словарём новых слов к сожалению так просто не поступишь
         //И поэтому они обрабатываются отдельно
         const Detail::CNewWordsCache::CSourceData *pData = m_NewWords.FindByIDSource( DictIndex ); 

         if( pData == 0 )
            return false;

         APL_ASSERT( pData->GetSource().length() < Constants::MAX_CHARS_IN_WORD );

         //Соединяем в одно слово и копируем в массив
         std::memcpy( wcWordSource, pData->GetSource().c_str(), (pData->GetSource().length() + 1) * sizeof(*wcWordSource) ); // "+ 1" для того чтобы сразу скопировать '\0'
         Detail::CorrectWordCase( wcWordSource, DictIndex );

         return true;
      }

      APL_ASSERT( pSourceWS != 0 ); //Должен быть выбран словарь который точно есть у этого класса	
      
      std::wstring wcSource;

      if( !pSourceWS->GetFormById(DictIndex.GetId(), &wcSource) ) 
         return false;

      APL_ASSERT( wcSource.length() < Constants::MAX_CHARS_IN_WORD );

      //Соединяем в одно слово и копируем в массив
      std::memcpy( wcWordSource, wcSource.c_str(), (wcSource.length() + 1) * sizeof(*wcWordSource) ); // "+ 1" для того чтобы сразу скопировать '\0'
      Detail::CorrectWordCase( wcWordSource, DictIndex );

      return true;
   }
   SS_CATCH(L"")

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CSmartDictionary::GetWordFormsByIDSource(
   const TDictionaryIndex* pDictionaryIndex,
   std::list<TWordInfo>* p_l_WordInfo
)
{
   APL_ASSERT_PTR( pDictionaryIndex );
   APL_ASSERT_PTR( p_l_WordInfo );
   APL_ASSERT( CheckLoad() );

   SS_TRY
   {
      TSourceToTypeConstPtr pCurSourceToType(0);
      TPartOfWordsSearchPtr pSourceWS(0);

      Detail::CSourceToType::TTypeIndex IDType;
      unsigned int IDSource = pDictionaryIndex->GetId();
      TMTList MTList;
      std::wstring wcEnding, wcSource, wcWord;
      TMTList	MorphoTypeListBuf;
      TMTList::iterator  IMTList;  
      p_l_WordInfo->clear();

      if( pDictionaryIndex->GetDictionaryNumber() == m_NewWordsDictionaryIndex )
      {
         //Обрабатываем отдельно словарь новых слов
         const Detail::CNewWordsCache::CSourceData *pData = m_NewWords.FindByIDSource( *pDictionaryIndex ); 

         if( pData == 0 )
            return false;

         IDType = pData->GetIDType();

         if( !m_pMorphoTypesSearch->GetMorphoTypesListByIDType(IDType, &MorphoTypeListBuf) )
            return false;

         wcSource = pData->GetSource();        
      }
      else
      {
         if( pDictionaryIndex->GetDictionaryNumber() == m_NominalDictionaryIndex)
         {
            pCurSourceToType = m_pNominalSourceToMorpho;
            pSourceWS = m_pNominalWS;
         }
         else if( pDictionaryIndex->GetDictionaryNumber() == m_ProperDictionaryIndex)
         {
            pCurSourceToType = m_pProperSourceToMorpho;
            pSourceWS = m_pProperWS;
         }

         APL_ASSERT( pCurSourceToType != 0 && pSourceWS != 0 ); //Должен быть выбран словарь который точно есть у этого класса	

         //Находим IDType
         IDType = pCurSourceToType->Find( IDSource );

         if( IDType == Detail::CSourceToType::EmptyIndex() )
            return false;

         if( !m_pMorphoTypesSearch->GetMorphoTypesListByIDType(IDType, &MorphoTypeListBuf) )
            return false;

         if( !pSourceWS->GetFormById(IDSource, &wcSource) )	//Основу мы можем найти уже сейчас
            SS_THROW(L"Ошибка в данных CPartOfWordsSearch или CMorphoTypesSearch"); //Если мы нашли данные в m_MorphoTypesSearch то они должны быть и здесь
      }

      APL_ASSERT(!MorphoTypeListBuf.empty());

      //Заполняем p_l_WordInfo
      SWordInfo *pWordInfo;

      for( IMTList = MorphoTypeListBuf.begin(); IMTList != MorphoTypeListBuf.end(); ++IMTList )
      {
         if( !m_pEndingWS->GetFormById(IMTList->m_IDEnding, &wcEnding) )
            SS_THROW(L"Ошибка в данных CPartOfWordsSearch или CMorphoTypesSearch"); //Если мы нашли данные в m_MorphoTypesSearch то они должны быть и здесь

         wcWord = wcSource;
         wcWord += wcEnding;

         //Добавляем новый элемент
         p_l_WordInfo->push_back( TWIList::value_type() );
         p_l_WordInfo->back().push_back( SWordInfo() /*TWIList::value_type::value_type()*/ );
         pWordInfo = &p_l_WordInfo->back().back();

         Detail::CorrectWordCase( wcWord, *pDictionaryIndex );

         //Заполняем его данными
         FillSWordInfo(	*IMTList, wcWord, IDSource, pDictionaryIndex->GetDictionaryNumber(), *pWordInfo, pDictionaryIndex->GetFirstLetterUpperCase() );
      }

      return true;
   }
   SS_CATCH(L"")
    
   return false;
}
///////////////////////////////////////////////////////////////////////////////

void CSmartDictionary::AddNewWord( const TString &Source, const TString &SourceLower, const TIDType IDType, int EndingLen, TDictionaryIndex &DI )
{
   APL_ASSERT( SourceLower.size() == Source.size() );
   APL_ASSERT( EndingLen >= 0 );

   Detail::CNewWordsCache::CSourceData SourceData;

   SourceData.SetIDType(IDType);
   APL_ASSERT( EndingLen >= 0 && EndingLen <= (int)Source.size() );

   size_t TmpLen = Source.size() - EndingLen;

   SourceData.SetSourceWithSourceLower( TString(Source.begin(), Source.begin() + TmpLen), TString(SourceLower.begin(), SourceLower.begin() + TmpLen) );
   m_NewWords.Add( SourceData );

   DI = SourceData.GetIDSource();
}
///////////////////////////////////////////////////////////////////////////////

bool CSmartDictionary::ReLoad()
{
	SS_TRY
	{
      if( !m_pNominalWS->Load() )
         SS_THROW( L"Ошибка CPartOfWordSearch::Load() (NominalWS)" );

      if( !m_pProperWS->Load() )
         SS_THROW( L"Ошибка CPartOfWordSearch::Load() (ProperWS)" );

      if( !m_pEndingWS->Load() )
         SS_THROW( L"Ошибка CPartOfWordSearch::Load() (EndingWS)" );

      if( !m_pNominalSourceToMorpho->Load() )
         SS_THROW( L"Ошибка CMorphoTypesSearch::Load() (NominalSourceToMorpho)" );

      if( !m_pProperSourceToMorpho->Load() )
         SS_THROW( L"Ошибка CMorphoTypesSearch::Load() (ProperSourceToMorpho)" );

      if( !m_pMorphoTypesSearch->Load() )
         SS_THROW(L"Ошибка CMorphoTypesSearch::Load()");
      
      return true;
   }
	SS_CATCH(L"")

	return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CSmartDictionary::Load()
{
   SS_TRY
	{
      if( !m_pNominalWS->LoadIfNeed() )
         SS_THROW( L"Ошибка CPartOfWordSearch::LoadIfNeed() (NominalWS)" );
      
      if( !m_pProperWS->LoadIfNeed() )
         SS_THROW( L"Ошибка CPartOfWordSearch::LoadIfNeed() (ProperWS)" );
      
      if( !m_pEndingWS->LoadIfNeed() )
         SS_THROW( L"Ошибка CPartOfWordSearch::LoadIfNeed() (EndingWS)" );

      if( !m_pNominalSourceToMorpho->LoadIfNeed() )
         SS_THROW( L"Ошибка CMorphoTypesSearch::LoadIfNeed() (NominalSourceToMorpho)" );

      if( !m_pProperSourceToMorpho->LoadIfNeed() )
         SS_THROW( L"Ошибка CMorphoTypesSearch::LoadIfNeed() (ProperSourceToMorpho)" );

      if( !m_pMorphoTypesSearch->LoadIfNeed() )
         SS_THROW(L"Ошибка CMorphoTypesSearch::LoadIfNeed()");

      return true;
   }
	SS_CATCH(L"")

	return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CSmartDictionary::ToBinary()
{
   SS_TRY
   {
      if( !m_pNominalWS->Create() )
         SS_THROW( L"Ошибка CPartOfWordSearch::Create() (NominalWS)" );

      if( !m_pProperWS->Create() )
         SS_THROW( L"Ошибка CPartOfWordSearch::Create() (ProperWS)" );

      if( !m_pEndingWS->Create() )
         SS_THROW( L"Ошибка CPartOfWordSearch::Create() (EndingWS)" );

      if( !m_pNominalWS->Load() )
         SS_THROW( L"Ошибка CPartOfWordSearch::Load() (NominalWS)" );

      if( !m_pProperWS->Load() )
         SS_THROW( L"Ошибка CPartOfWordSearch::Load() (ProperWS)" );

      if( !m_pEndingWS->Load() )
         SS_THROW( L"Ошибка CPartOfWordSearch::Load() (EndingWS)" );

      return true;
   }
   SS_CATCH(L"")

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CSmartDictionary::Clear()
{
	SS_TRY
	{
      m_pMorphoTypesSearch->Clear();      
		m_pNominalWS->Clear(); 						
		m_pProperWS->Clear(); 						
		m_pEndingWS->Clear(); 						
		m_pNominalSourceToMorpho->Clear(); 						
		m_pProperSourceToMorpho->Clear(); 	
      m_NewWords.Clear();

      return true;
	}
	SS_CATCH(L"")

	return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CSmartDictionary::FindSubWordsBounds( 		
   const wchar_t* wcWord, 
   const wchar_t* wcWordOriginalCase,
   const SPartOfWord  &IDEnding,
   TPartOfWordsSearchPtr pSourseWS, 
   TSourceToTypeConstPtr pSourceToType,
   TDictNumber DictionaryIndex,
   TWordBounds &WordBounds
)
{
   //APL_ASSERT_PTR( wcWord );
   APL_ASSERT_PTR( wcWordOriginalCase );

   TMTList	 MorphoTypeListBuf;
   TPOFVector SourceVecBuf;
   TPOFVector EndingVecBuf;
   TPOFVector::reverse_iterator ISource;
   TPOFVector::reverse_iterator IEnding;
   TMTList::iterator  IMTList;  
  

   //Для каждого окончания ищем возможный источник
   SourceVecBuf.clear();
   pSourseWS->GetByForm(wcWord, IDEnding.PositionInWord, &SourceVecBuf, FALSE); //Ищем не до конца слова

   //Для каждого источника узнаём узнаём его IDType и затем проверяем есть ли такая комбинация IDEnding - IDTypes в 
   //m_MorphoTypesSearch
   //PartOfWord гарантирует, что результат будет отсортирован по длине слова
   //Поэтому перебирая список с конца, мы гарантированно рассмотри сначала самые длинные
   //совпадения
   for( ISource = SourceVecBuf.rbegin(); ISource != SourceVecBuf.rend(); ++ISource  )
   {
      CorrectIDEnding(*ISource, IDEnding.PositionInWord );

      Detail::CSourceToType::TTypeIndex TypeIndex = pSourceToType->Find(ISource->Id);

      APL_ASSERT( TypeIndex != Detail::CSourceToType::EmptyIndex() );

      if( m_pMorphoTypesSearch->GetByForm(TypeIndex, IDEnding.Id, &MorphoTypeListBuf) )
      {
         //Мы нашли слово которое может являтся частью составного слова
         for( IMTList = MorphoTypeListBuf.begin(); IMTList != MorphoTypeListBuf.end(); ++IMTList )
         {
            APL_ASSERT( TypeIndex == IMTList->m_MorphoTypeInfo.m_IDType );

            if( ISource->PositionInWord == -1  )
            {
               //WordBounds.push_back(0);
               
               //Дошли до конца слова можно заполнять морфоинформацию
               //WIList.push_back( TWordInfo::value_type() );
               //FillSWordInfo( *IMTList, std::wstring(wcWordOriginalCase, WordEndIndex + 1), ISource->Id, DictionaryIndex, WIList.back(), IsFirstLetterUpperCase );

               return true;
            }
            else
            {
               //Не дошли до конца слова.
               //Проверяем, можем ли мы найти остальные слова

               //Находим все возможные окончания и вызываем функцию рекурсивно для каждоги из них
               EndingVecBuf.clear();
               m_pEndingWS->GetByForm(wcWord, ISource->PositionInWord, &EndingVecBuf, FALSE);
               
               //PartOfWord гарантирует, что результат будет отсортирован по длине слова
               //Поэтому перебирая список с конца, мы гарантированно рассмотри сначала самые длинные
               //совпадения
               for( IEnding = EndingVecBuf.rbegin(); IEnding != EndingVecBuf.rend(); ++IEnding )
               {
                  CorrectIDEnding(*IEnding, ISource->PositionInWord );

                  //Случай пустая основа + пустое окончание не может быть в БД 

                  //Флаг "с большой буквы" ложен всегда кроме первого слова
                  if( FindSubWordsBounds(wcWord, wcWordOriginalCase, *IEnding, pSourseWS, pSourceToType, DictionaryIndex, WordBounds) )
                  {
                     WordBounds.push_back(ISource->PositionInWord + 1);

                     //Вниз по рекурсии было найдено одно или более слов, 
                     //добавляем морфоинформацию по текущему слову
                     //WIList.push_back( TWordInfo::value_type() );
                     //FillSWordInfo( *IMTList, std::wstring(wcWordOriginalCase + ISource->PositionInWord + 1, WordEndIndex - ISource->PositionInWord), ISource->Id, DictionaryIndex, WIList.back(), IsFirstLetterUpperCase );

                     return true;
                  }
               }
            }
         }
      }		
   }

   return false;
}
///////////////////////////////////////////////////////////////////////////////

void CSmartDictionary::FillWordInfo(
      const wchar_t* wcWord,
      const wchar_t* wcWordOriginalCase,
      int SearchPos,
      bool IsFirstLetterUpperCase,
      const SPartOfWord  &IDEnding,
      TPartOfWordsSearchPtr pSourseWS, 
      TSourceToTypeConstPtr pSourceToType,
      TDictNumber DictionaryIndex,
      TWordInfo &WordInfo
) const
{
	TMTList	 MorphoTypeListBuf;
	TPOFVector::const_iterator ISource;
	TMTList::iterator  IMTList;  
	
   //Для каждого окончания ищем возможный источник
	m_SourceVecBuf.clear();
	pSourseWS->GetByForm(wcWord, IDEnding.PositionInWord, &m_SourceVecBuf, TRUE);

	//Для каждого источника узнаём узнаём его IDType и затем проверяем есть ли такая комбинация IDEnding - IDTypes в 
   //m_MorphoTypesSearch
	for( ISource = m_SourceVecBuf.begin(); ISource != m_SourceVecBuf.end(); ++ISource  )
	{
      Detail::CSourceToType::TTypeIndex TypeIndex = pSourceToType->Find(ISource->Id);

      APL_ASSERT( TypeIndex != Detail::CSourceToType::EmptyIndex() );

      //++SSDStatistic::TotalFillWordInfo;
		if( m_pMorphoTypesSearch->GetByForm(TypeIndex, IDEnding.Id, &MorphoTypeListBuf) )
		{
			//++SSDStatistic::SuccessfulFillWordInfo;
         //Мы нашли необходимую комбинацию теперь необходимо её добавить в WIList
			for( IMTList = MorphoTypeListBuf.begin(); IMTList != MorphoTypeListBuf.end(); ++IMTList )
			{
				APL_ASSERT( TypeIndex == IMTList->m_MorphoTypeInfo.m_IDType );
				
				//Добавляем новый элемент
				WordInfo.push_back( TWordInfo::value_type() );

				//Заполняем его данными
            FillSWordInfo( *IMTList, std::wstring(wcWordOriginalCase, SearchPos + 1), ISource->Id, DictionaryIndex, WordInfo.back(), IsFirstLetterUpperCase );
			}
		}		
	}
}
///////////////////////////////////////////////////////////////////////////////


bool CSmartDictionary::GetWordInfoOnlyNominal(
	const wchar_t* wcWord, 
   const wchar_t* wcWordOriginalCase,
   int SearchPos,
   bool IsFirstLetterUpperCase,
   TWordInfo &WordInfo
) const
{
	APL_ASSERT_PTR( wcWord );
   APL_ASSERT_PTR( wcWordOriginalCase );
   APL_ASSERT( CheckLoad() );

	SS_TRY
	{
		TPOFVector::iterator IEnding;
	
      WordInfo.clear();
		
		APL_ASSERT( SearchPos >= 0 );

		//Ищем все возможные окончания, причём если найдём в основном словаре, 
      //то в новых словах можно не искать
		m_EndingVecBuf.clear();
      m_pEndingWS->GetByForm(wcWord, SearchPos, &m_EndingVecBuf, FALSE);

      
      //Попробуем найти сначала имена нарицательные и собственные
      for( IEnding = m_EndingVecBuf.begin(); IEnding != m_EndingVecBuf.end(); ++IEnding )
      {
         CorrectIDEnding(*IEnding, SearchPos );
         
         FillWordInfo( wcWord, wcWordOriginalCase, SearchPos, IsFirstLetterUpperCase, *IEnding, m_pNominalWS, m_pNominalSourceToMorpho, m_NominalDictionaryIndex, WordInfo );
      }

  	}
	SS_CATCH(L"")

	return !WordInfo.empty();
}

///////////////////////////////////////////////////////////////////////////////

bool CSmartDictionary::GetCompoundWordInfo( 		
   const wchar_t* wcWord, 
   const wchar_t* wcWordOriginalCase,
   bool IsFirstLetterUpperCase,
   TWIList &WIList
)
{
   APL_ASSERT_PTR( wcWord );
   APL_ASSERT_PTR( wcWordOriginalCase );
   APL_ASSERT( CheckLoad() );

   SS_TRY
   {
      TPOFVector::reverse_iterator IEnding;

      int SearchPos = static_cast<int>(std::wcslen( wcWord )) - 1;
      TWordBounds WordBounds;
      WIList.clear();

      APL_ASSERT( SearchPos >= 0 );

      //Ищем все возможные окончания
      m_EndingVecBuf.clear();
      m_pEndingWS->GetByForm(wcWord, SearchPos, &m_EndingVecBuf, FALSE);

      //PartOfWord гарантирует, что результат будет отсортирован по длине слова
      //Поэтому перебирая список с конца, мы гарантированно рассмотри сначала самые длинные
      //совпадения
      for( IEnding = m_EndingVecBuf.rbegin(); IEnding != m_EndingVecBuf.rend(); ++IEnding )
      {
         CorrectIDEnding(*IEnding, SearchPos );

         if( FindSubWordsBounds(wcWord, wcWordOriginalCase, *IEnding, m_pNominalWS, m_pNominalSourceToMorpho, m_NominalDictionaryIndex, WordBounds) )
            break;
      }

      if( !WordBounds.empty() )
      {
         //Мы нашли границы слова, опредиляем морфоинформацию каждого подслова
         //Добавляем последний индекс для упрощения алгоритма
         WordBounds.push_back(SearchPos + 1);
         int BeginPos = 0;

         for(TWordBounds::iterator I = WordBounds.begin(); I != WordBounds.end(); ++I )
         {
            WIList.push_back( TWIList::value_type() );

            APL_CHECK( GetWordInfoOnlyNominal(wcWord + BeginPos, wcWordOriginalCase + BeginPos, *I - BeginPos - 1, IsFirstLetterUpperCase, WIList.back()) );
            IsFirstLetterUpperCase = false;
            BeginPos = *I;
         }

         return true;
      }

   }
   SS_CATCH(L"")

   return false;
}

//BOOL CSmartDictionary::GetMorphoTypeByDictionaryIndexAndIDForm(
//   TDictionaryIndex DI, 
//   TIDForm IDForm,
//   OUT SS::Dictionary::Types::SMorphoType * pMorphoType
//)
//{
//   APL_ASSERT( CheckLoad() );
//
//   TSourceToTypeConstPtr pCurSourceToType = 0;
//
//   if( DI.GetDictionaryNumber() == m_NominalDictionaryIndex )
//      pCurSourceToType = m_pNominalSourceToMorpho;
//   else if( DI.GetDictionaryNumber() == m_ProperDictionaryIndex )
//      pCurSourceToType = m_pProperSourceToMorpho;
//   else
//      return FALSE;
//
//   TIDType IDType = pCurSourceToType->Find( DI.GetId() );
//
//   if( IDType == Detail::CSourceToType::EmptyIndex() )
//      return FALSE;
//
//   return m_pMorphoTypesSearch->GetMorphoTypeByIDTypeAndIDForm( IDType, IDForm, pMorphoType );
//}
//

///////////////////////////////////////////////////////////////////////////////
// class CStandartSmartDictionary
///////////////////////////////////////////////////////////////////////////////
void CStandartSmartDictionary::FillHypothesisInfo(
      const wchar_t* wcWord,
      const wchar_t* wcWordLower,
      bool IsFirstLetterUpperCase, 
      int WordLen,
      const SPartOfWord  &IDEnding,
      TPartOfWordsSearchPtr pSourseWS, 
      TSourceToTypeConstPtr pSourceToType,
      TDictNumber DictionaryIndex,
      SHypothesisInfo &HypothesisInfo,
      const IDeterminationWordCheck &DWC
) const
{
   TMTList	 MorphoTypeListBuf;
   THIVector::const_iterator IHypo;
   TMTList::iterator  IMTList;  
   int ConcordanceChars = 0; 
   SHypothesisInfo::WordInfo WordInfo;

   APL_ASSERT_PTR(wcWord);
   APL_ASSERT_PTR(wcWordLower);
   APL_ASSERT( 
      static_cast<int>(std::wcslen( wcWord )) == WordLen &&
      static_cast<int>(std::wcslen( wcWordLower )) == WordLen 
   );

   APL_ASSERT( WordLen > 0 );
   APL_ASSERT_PTR( m_pAMConverter  );
   
   const wchar_t *pSearchWord = wcWordLower;
   int SearchPos = IDEnding.PositionInWord;
   int EndingMatch = WordLen - SearchPos - 1; //Количество совпадений по окончанию

   //Минимальное количество совпадений с конца в словаре
   int MinMatchSymbols = 
      std::max(
         HypothesisInfo.m_ConcordanceChars, 
         SS::Constants::Dictionary::MIN_COINCIDENCE_FOR_MORPHO_RECOGNIZABLE 
      ) - EndingMatch;

   //Мы не учитывем слова состоящие только из окончания, поэтому минимальное количество совпадений в источнике всегда 1
   MinMatchSymbols = std::max( MinMatchSymbols, 1 );
   
   //Мы проведем следующую оптимизацию: Фактически мы можем один раз вызвать GetHypothesis c MinMatchSymbols,
   //но при этом количество гипотез может быть очень большим, а выберем мы из них только очень малое количество.
   //Например, для слова "meteing" максимальное совпадение будет пять букв, но количество гипотез для поиска по основам
   //при окнчании "ing" и MinMatchSymbols = 1 будет более 12500, что не очень быстро. А если сначала поискать с 
   //MinMatchSymbols = 2, то всего около 1800, что тоже конечно не фонтан, но в 6 раз меньше.
   //Поэтому будем вызывать GetHypothesis в цикле где Минимальное совпадение будет изменятся от длины основы до 
   //MinMatchSymbols. 
   for( 
      int CurMatchSymbols = WordLen - EndingMatch;  
      CurMatchSymbols >= MinMatchSymbols && CurMatchSymbols  >= HypothesisInfo.m_ConcordanceChars - EndingMatch;
      --CurMatchSymbols 
   )
   {
      //Для каждого окончания ищем все возможные источники
      m_HIVectorBuf.clear();
         
      if( !pSourseWS->GetHypothesis(pSearchWord, SearchPos, &m_HIVectorBuf, CurMatchSymbols ) )
         continue;

      //Для каждого источника проверяем пригодность его как возможной гипотезы 
      for( IHypo = m_HIVectorBuf.begin(); IHypo != m_HIVectorBuf.end(); ++IHypo  )
      {
         Detail::CSourceToType::TTypeIndex TypeIndex = pSourceToType->Find(IHypo->Id);

         APL_ASSERT( TypeIndex != Detail::CSourceToType::EmptyIndex() );

         //Количество совпавших символов
         ConcordanceChars = IHypo->MatchSymbols + EndingMatch;

         //Отбрасываем все гипотезы длина совпадения в которых < HypothesisInfo.m_ConcordanceChars
         if( ConcordanceChars < HypothesisInfo.m_ConcordanceChars )
            continue;

         //++SSDStatistic::TotalFillHypothesisInfo;
         if( m_pMorphoTypesSearch->GetByForm(TypeIndex, IDEnding.Id, &MorphoTypeListBuf) )
         {
            //++SSDStatistic::SuccessfulFillHypothesisInfo;

            for( IMTList = MorphoTypeListBuf.begin(); IMTList != MorphoTypeListBuf.end(); ++IMTList )
            {
               if( !DWC.IsDeterminationWord(wcWord, wcWordLower, IsFirstLetterUpperCase, DictionaryIndex, IMTList->m_MorphoTypeInfo.m_MorphoInfo, IMTList->m_SpecialMorpho, m_pAMConverter) )
                  continue;

               //Если мы сюда добрались значит все проверки выполнились и мы можем добавлять
               if( HypothesisInfo.m_ConcordanceChars < ConcordanceChars ) //Возможно мы нашли больше совпадений
               {
                  HypothesisInfo.m_FormsInfoList.clear();
                  HypothesisInfo.m_ConcordanceChars = ConcordanceChars;
               }

               WordInfo.m_MorphoType = *IMTList;
               WordInfo.m_EndingLen = WordLen - IDEnding.PositionInWord - 1;
               WordInfo.m_DictIndex.SetId( IHypo->Id );
               WordInfo.m_DictIndex.SetDictionaryNumber( DictionaryIndex );
               WordInfo.m_DictIndex.SetFirstLetterUpperCase( IsFirstLetterUpperCase );

               HypothesisInfo.m_FormsInfoList.push_back(WordInfo);
            }
         }		
      }
   }
}
///////////////////////////////////////////////////////////////////////////////

bool CStandartSmartDictionary::DeterminationWordInfo(
      const wchar_t* wcWord,
      const wchar_t* wcWordLower,
      bool IsFirstLetterUpperCase,
      SHypothesisInfo &HypothesisInfo,
      const IDeterminationWordCheck &DWC
)
{
   APL_ASSERT_PTR( wcWord );
   APL_ASSERT_PTR( wcWordLower );
   APL_ASSERT( CheckLoad() );

   SS_TRY
   {
      TWIList WIListTmp;
      TPOFVector::iterator IEnding;
      TMTList::iterator  IMTList;  

      int WordLen = static_cast<int>(std::wcslen( wcWordLower ));
      int SearchPos = WordLen - 1;

      APL_ASSERT( SearchPos >= 0 );

      HypothesisInfo.Clear();

      // Если длина слова меньше некоторого числа символов то не опредиляем его автоматически 
      if( WordLen < GetMinRecognizableWordLen() )
         return false;

      //Ищем все возможные окончания
      m_EndingVecBuf.clear();
      m_pEndingWS->GetByForm(wcWordLower, SearchPos, &m_EndingVecBuf, FALSE);

      for( IEnding = m_EndingVecBuf.begin(); IEnding != m_EndingVecBuf.end(); ++IEnding )
      {
         CorrectIDEnding(*IEnding, SearchPos );

         FillHypothesisInfo( wcWord, wcWordLower, IsFirstLetterUpperCase, WordLen, *IEnding, m_pNominalWS, m_pNominalSourceToMorpho, m_NominalDictionaryIndex, HypothesisInfo, DWC );
      }
   }
   SS_CATCH(L"")

   return !HypothesisInfo.Empty();
}

///////////////////////////////////////////////////////////////////////////////
// class CMinimalHypothesisTreeSmartDictionary
///////////////////////////////////////////////////////////////////////////////
void CMinimalHypothesisTreeSmartDictionary::Init( 
      DBMS::IDataBase *pDataBase,
      IAMConverterMorpho *pAMConverter, const TCHAR *DBPath, 
      TDictNumber NominalDictionaryIndex,
      TDictNumber ProperDictionaryIndex, 
      TDictNumber NewWordsDictionaryIndex,
      int MinRecognizableWordLen
)
{
   CSmartDictionary::Init( pDataBase, pAMConverter, DBPath, NominalDictionaryIndex, ProperDictionaryIndex, NewWordsDictionaryIndex, MinRecognizableWordLen ); 

   //Дополнительно необходимо открыть минимальное дерево поиска
   //Формируем пути к файлам с даными и инициализуем словари
   TString InitPath(DBPath);

   InitPath += _T("\\");    InitPath += szNewWordsDeductionTable;   InitPath += _T("\\");

   //Открыем таблицы для CPartOfWordsSearch
   if( m_pNewWordsDeductionDT.Open(pDataBase, szNewWordsDeductionTable) != S_OK )
      SS_THROW(L"Open NewWordsDeduction");


   m_pNewWordsDeductionWS = CPartOfWordsSearchCreateManager::Create()->Construct(InitPath.c_str(), m_pNewWordsDeductionDT.Get());
}

///////////////////////////////////////////////////////////////////////////////

bool CMinimalHypothesisTreeSmartDictionary::DeterminationWordInfo(
      const wchar_t* wcWord,
      const wchar_t* wcWordLower,
      bool IsFirstLetterUpperCase, 
      SHypothesisInfo &HypothesisInfo,
      const IDeterminationWordCheck &DWC
)
{
   APL_ASSERT_PTR( wcWord );
   APL_ASSERT_PTR( wcWordLower );
   APL_ASSERT( CheckLoad() );

   SS_TRY
   {
      TMTList MorphoTypeListBuf;
      TPOFVector::reverse_iterator IIDTypes;
      TMTList::iterator  IMTList, IMTMaxLen;  
      TMTList MTList;
      std::wstring Ending;
      SHypothesisInfo::WordInfo WordInfo;
      int MatchSymbol = 0; //Индекс совпавшего символа
      int EndingLen, MaxEndingLen;

      typedef std::reverse_iterator<const wchar_t*> TRevWcharIter;

      int WordLen = static_cast<int>(std::wcslen( wcWordLower ));
      int SearchPos = WordLen - 1;

      TRevWcharIter IWordBegin( wcWordLower + WordLen), IWordEnd(wcWordLower);

      APL_ASSERT( SearchPos >= 0 );

      HypothesisInfo.Clear();

      // Если длина слова меньше некоторого числа символов то не опредиляем его автоматически 
      if( WordLen < GetMinRecognizableWordLen() )
         return false;

      //Ищем все возможные окончания. Мы находим максимальную совпадающую часть слова в дереве,
      //при этом эта часть всегда есть (кроме тех случаев когда нет слова оканчивающегося на последнюю 
      //букву в wcWordLower). 
      //Константа SS::Constants::Dictionary::MIN_COINCIDENCE_FOR_MORPHO_RECOGNIZABLE не имеет
      //смысла т.к. мы работаем с оптимизированным деревом и не знаем сколько именно символов с конца совпало 
      //со строками по которым это дерево построено.
      m_SourceVecBuf.clear();
      m_pNewWordsDeductionWS->GetByForm(wcWordLower, SearchPos, &m_SourceVecBuf, FALSE);

      //PartOfWord гарантирует, что m_SourceVecBuf будет отсортирован по длине слова
      //Поэтому перебирая список с конца, мы гарантированно рассмотри сначала самые длинные
      //совпадения
      for( 
         IIDTypes = m_SourceVecBuf.rbegin(); 
         IIDTypes != m_SourceVecBuf.rend() && 
         ( HypothesisInfo.Empty() || IIDTypes->PositionInWord == MatchSymbol );   //Нам необходимо собрать все IDType с одной и той-же длиной совпадения
         ++IIDTypes 
      )
      {
         APL_ASSERT( IIDTypes->PositionInWord >= MatchSymbol );
         MatchSymbol = IIDTypes->PositionInWord;

         APL_CHECK( m_pMorphoTypesSearch->GetMorphoTypesListByIDType(IIDTypes->Id, &MorphoTypeListBuf) );
            
         //Находим максимально длинное, подходящие окончание
         MaxEndingLen = -1; //Ищем хотя-бы пустое окончание
         for( IMTList = MorphoTypeListBuf.begin(); IMTList != MorphoTypeListBuf.end(); ++IMTList )
         {
            //Подходит ли найденая морфоинформация
            if( !DWC.IsDeterminationWord(wcWord, wcWordLower, IsFirstLetterUpperCase, m_NominalDictionaryIndex, IMTList->m_MorphoTypeInfo.m_MorphoInfo, IMTList->m_SpecialMorpho, m_pAMConverter) )
               continue;

            //Подходит ли окончание
            APL_CHECK( m_pEndingWS->GetFormById(IMTList->m_IDEnding, &Ending) );
            if( Mismatch( IWordBegin, IWordEnd, Ending.rbegin(), Ending.rend()).second == Ending.rend() )
            {
               EndingLen = (int)Ending.length();
               
               //Нашли окончание
               //Если мы сюда добрались значит все проверки выполнились и мы можем добавлять
               if( MaxEndingLen < EndingLen ) 
               {
                  MaxEndingLen = EndingLen;
                  IMTMaxLen = IMTList;
               }
            }
         }
         
         if( MaxEndingLen > -1 )
         {
            if( MaxEndingLen > HypothesisInfo.m_ConcordanceChars )
               HypothesisInfo.m_ConcordanceChars = MaxEndingLen;

            WordInfo.m_MorphoType = *IMTMaxLen;
            WordInfo.m_EndingLen = MaxEndingLen;
            WordInfo.m_DictIndex.SetId( 0 );
            WordInfo.m_DictIndex.SetDictionaryNumber( m_NominalDictionaryIndex );
            WordInfo.m_DictIndex.SetFirstLetterUpperCase( IsFirstLetterUpperCase );

            HypothesisInfo.m_FormsInfoList.push_back(WordInfo);
         }
      }
   }
   SS_CATCH(L"")

   return !HypothesisInfo.Empty();
}

///////////////////////////////////////////////////////////////////////////////
// struct TStandartDeterminationWordCheck
///////////////////////////////////////////////////////////////////////////////
bool TStandartDeterminationWordCheck::IsDeterminationWord( const wchar_t* wcWord, const wchar_t* wcWordLower, bool IsFirstLetterUpperCase, TDictNumber DictionaryIndex, TMorphoInfo MorphoInfo, TSpecialMorpho SpecialMorpho, IAMConverterMorpho *pAMConverter ) const
{
   CMorphoFeature MorphoFeature;

   pAMConverter->EnCode( DictionaryIndex, MorphoInfo, SpecialMorpho, &MorphoFeature );

   if( IsFirstLetterUpperCase ) //Первый символ верхний регистер
   {
      //Только существительное
      if( !MorphoFeature.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postNoun) )
         return false;
   }
   else
   {
      //Не принимаем в расчёт служебные части речи
      if(
         MorphoFeature.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postOfficial) ||
         MorphoFeature.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postAdverb)
      )
         return false;
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////
// struct TOnlyNounDeterminationWordCheck
///////////////////////////////////////////////////////////////////////////////
bool TOnlyNounDeterminationWordCheck::IsDeterminationWord( const wchar_t* wcWord, const wchar_t* wcWordLower, bool IsFirstLetterUpperCase, TDictNumber DictionaryIndex, TMorphoInfo MorphoInfo, TSpecialMorpho SpecialMorpho, IAMConverterMorpho *pAMConverter ) const
{
   CMorphoFeature MorphoFeature;

   pAMConverter->EnCode( DictionaryIndex, MorphoInfo, SpecialMorpho, &MorphoFeature );

   return MorphoFeature.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postNoun);
}

} //namespace Dictionary
} //namespace SS
