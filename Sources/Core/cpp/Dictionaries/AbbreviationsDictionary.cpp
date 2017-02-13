#include "stdafx.h"

namespace SS{                        
namespace Dictionary{

///////////////////////////////////////////////////////////////////////////////
// class AbbreviationsDictionaryBase
///////////////////////////////////////////////////////////////////////////////

void CAbbreviationsDictionaryBase::Init(
      DBMS::IDataBase *pMorphoDataBase, DBMS::IDataBase *pAbbreviationsDataBase, const TCHAR *TableName, 
      TDictNumber DictionaryIndex 
)
{
   APL_ASSERT( pMorphoDataBase != 0 );
   APL_ASSERT( pAbbreviationsDataBase != 0 );
   APL_ASSERT( TableName != 0 );

   if( m_pTable.Open(pAbbreviationsDataBase, TableName) != S_OK )
      SS_THROW( L"Ошибка при открытии таблицы AbbreviationsDictionary" );

   m_pMorphoTypesSearch = CMorphoTypesSearchCreateManager::Create()->Construct(pMorphoDataBase, NULL);
   m_DictIndex = DictionaryIndex;
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionaryBase::IsExistWord(/*in*/const wchar_t* wcWord)
{
   APL_ASSERT( wcWord != 0 );
	APL_ASSERT( m_pMorphoTypesSearch != 0 && m_pTable.IsOpen() );

   CFindHelper FindHelper(m_pTable);
   
   std::wstring FormLower = wcWord;        
   INT32 IDType = 0;   
   INT32 IDForm = 0;  
   TMorphoInfo MorphoInfo = 0;
   TSpecialMorpho SpecialMorpho = 0;

   void *RecMap[5] = {0, 0, 0, &IDType,  };

   if( FindHelper.Init(1, &FormLower, RecMap) != S_OK )
      SS_THROW( L"Ошибка при инициализации поиска в AbbreviationsDictionary по Form" );

   if( FindHelper.FindOne() != S_OK )
      return false;

   return m_pMorphoTypesSearch->GetMorphoInfoByIDType( IDType, 0, &MorphoInfo, &SpecialMorpho ) == TRUE;
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionaryBase::GetWordInfo/*ByWord*/(
	/*in*/const wchar_t* wcWord, 
   /*in*/const wchar_t* wcWordOriginalCase,
   /*in*/bool IsFirstLetterUpperCase,
	/*out*/TWIList* p_l_WordInfo
)
{
   APL_ASSERT( wcWord != 0 && p_l_WordInfo != 0 );
   APL_ASSERT( m_pMorphoTypesSearch != 0 && m_pTable.IsOpen() );

   CFindHelper FindHelper(m_pTable);

   INT32 IDSource = 0;      
   std::wstring FormLower = wcWord;        
   std::wstring Form;        
   INT32 IDType = 0;   
   INT32 IDForm = 0;  
   TMorphoInfo MorphoInfo = 0;
   TSpecialMorpho SpecialMorpho = 0;
   SWordInfo *pWordInfo = 0;
   void *RecMap[5] = { &IDSource, 0, &Form, &IDType, &IDForm };

   p_l_WordInfo->clear();

   if( FindHelper.Init(1, &FormLower, RecMap) != S_OK )
      SS_THROW( L"Ошибка при инициализации поиска в AbbreviationsDictionary по Form" );
   
   if( FindHelper.FindOne() != S_OK )
      return false;

   if( m_pMorphoTypesSearch->GetMorphoInfoByIDType( IDType, 0, &MorphoInfo, &SpecialMorpho ) != TRUE )
      return false;
   
   p_l_WordInfo->push_back( TWIList::value_type() );
   p_l_WordInfo->back().push_back( SWordInfo() );
   pWordInfo = &p_l_WordInfo->back().back();

   FillSWordInfo(	wcWordOriginalCase, IDSource, IDForm, IDType, SpecialMorpho, MorphoInfo, *pWordInfo, IsFirstLetterUpperCase );
   
   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionaryBase::GetWordInfo/*ByWordIndex*/(
	/*in*/SWordIndex* pWordIndex, 
	/*out*/SWordInfo* pWordInfo
)
{
	APL_ASSERT( pWordIndex != 0 );
	APL_ASSERT( pWordInfo != 0 );
   APL_ASSERT( m_pMorphoTypesSearch != 0 && m_pTable.IsOpen() );
   APL_ASSERT( pWordIndex->m_DictIndex.GetDictionaryNumber() == m_DictIndex );

   CFindHelper FindHelper(m_pTable);      
   
   INT32 IDSource = pWordIndex->m_DictIndex.GetId();      
   std::wstring Form;        
   INT32 IDType = 0;   
   INT32 IDForm = 0;  
   TMorphoInfo MorphoInfo = 0;
   TSpecialMorpho SpecialMorpho = 0;
   void *RecMap[5] = { 0, 0, &Form, &IDType, &IDForm };

   if( FindHelper.Init(0u, &IDSource, RecMap) != S_OK )
      SS_THROW( L"Ошибка при инициализации поиска в AbbreviationsDictionary по IDSource" );

   while( FindHelper.Find() == S_OK )
   {
      if( 
         IDForm == pWordIndex->m_IDForm &&
         m_pMorphoTypesSearch->GetMorphoInfoByIDType( IDType, 0, &MorphoInfo, &SpecialMorpho ) == TRUE
      )
      {
         Detail::CorrectWordCase( Form, pWordIndex->m_DictIndex );

         FillSWordInfo(	Form, IDSource, IDForm, IDType, SpecialMorpho, MorphoInfo, *pWordInfo, pWordIndex->m_DictIndex.GetFirstLetterUpperCase() );
         return true;
      }
   }
	
	return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionaryBase::GetWord(
	/*in*/SWordIndex* pWordIndex,
	/*out*/wchar_t* wcWord
)
{
	APL_ASSERT( pWordIndex != 0 );
	APL_ASSERT( wcWord != 0 );
   APL_ASSERT( m_pMorphoTypesSearch != 0 && m_pTable.IsOpen() );
   APL_ASSERT( pWordIndex->m_DictIndex.GetDictionaryNumber() == m_DictIndex );

   CFindHelper FindHelper(m_pTable);

   INT32 IDSource = pWordIndex->m_DictIndex.GetId();      
   std::wstring Form;        
   INT32 IDForm = 0;  
   SWordInfo *pWordInfo = 0;
   void *RecMap[5] = { 0, 0, &Form, 0, &IDForm };

   if( FindHelper.Init(0u, &IDSource, RecMap) != S_OK )
      SS_THROW( L"Ошибка при инициализации поиска в AbbreviationsDictionary по IDSource" );

   while( FindHelper.Find() == S_OK )
   {
      if( IDForm == pWordIndex->m_IDForm )
      {        
         APL_ASSERT( Form.length() < Constants::MAX_CHARS_IN_WORD );

         //Соединяем в одно слово и копируем в массив
         std::memcpy( wcWord, Form.c_str(), (Form.length() + 1 )* sizeof(*wcWord) ); // "+ 1" для того чтобы сразу скопировать '\0'

         return true;
      }
   }

	return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionaryBase::GetWordFormsByIDSource(
	/*in*/TDictionaryIndex* pDictionaryIndex,
	/*out*/std::list<TWordInfo>* p_l_WordInfo
)
{
   APL_ASSERT( pDictionaryIndex != 0 );
   APL_ASSERT( p_l_WordInfo != 0 );
   APL_ASSERT( m_pMorphoTypesSearch != 0 && m_pTable.IsOpen() );
   APL_ASSERT( pDictionaryIndex->GetDictionaryNumber() == m_DictIndex );

   CFindHelper FindHelper(m_pTable);

   INT32 IDSource = pDictionaryIndex->GetId();      
   std::wstring Form;        
   INT32 IDType = 0;   
   INT32 IDForm = 0;  
   TMorphoInfo MorphoInfo = 0;
   TSpecialMorpho SpecialMorpho = 0;
   SWordInfo *pWordInfo = 0;
   void *RecMap[5] = { 0, 0, &Form, &IDType, &IDForm };

   p_l_WordInfo->clear();

   if( FindHelper.Init(0u, &IDSource, RecMap) != S_OK )
      SS_THROW( L"Ошибка при инициализации поиска в AbbreviationsDictionary по IDSource" );

   while( FindHelper.Find() == S_OK )
   {
      if( m_pMorphoTypesSearch->GetMorphoInfoByIDType( IDType, 0, &MorphoInfo, &SpecialMorpho ) == TRUE )
      {
         p_l_WordInfo->push_back( TWIList::value_type() );
         p_l_WordInfo->back().push_back( SWordInfo() );
         pWordInfo = &p_l_WordInfo->back().back();
         
         Detail::CorrectWordCase( Form, *pDictionaryIndex );

         FillSWordInfo(	Form, IDSource, IDForm, IDType, SpecialMorpho, MorphoInfo, *pWordInfo, pDictionaryIndex->GetFirstLetterUpperCase() );
         return true;
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////
// class CAbbreviationsDictionaryBaseMemory
///////////////////////////////////////////////////////////////////////////////
bool CAbbreviationsDictionaryBaseMemory::Init(
	DBMS::IDataBase *pMorphoDataBase, DBMS::IDataBase *pAbbreviationsDataBase, const TCHAR *TableName, 
	TDictNumber DictionaryIndex 
)
{
   APL_ASSERT( pMorphoDataBase != 0 );
   APL_ASSERT( pAbbreviationsDataBase != 0 );
   APL_ASSERT( TableName != 0 );

   m_pMorphoDataBase = pMorphoDataBase;
   m_pAbbreviationsDataBase = pAbbreviationsDataBase;
   m_TableName = TableName;
   m_DictIndex = DictionaryIndex;

   return Reload();
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionaryBaseMemory::Reload()
{
   APL_ASSERT( m_pMorphoDataBase != 0 );
   APL_ASSERT( m_pAbbreviationsDataBase != 0 );
   APL_ASSERT( !m_TableName.empty() );

   CTablePtr pTable;

   if( pTable.Open(m_pAbbreviationsDataBase, m_TableName.c_str()) != S_OK )
      SS_THROW( L"Ошибка при открытии таблицы AbbreviationsDictionary" );

   m_pMorphoTypesSearch = CMorphoTypesSearchCreateManager::Create()->Construct(m_pMorphoDataBase, NULL);
   m_FormLowerToRecord.clear();
   m_DictionaryIndexToRecordPtr.clear();

   SRecord Record;
   void *RecMap[5] = { &Record.IDSource, &Record.FormLower, &Record.Form, &Record.IDType, &Record.IDForm };

   if( pTable->ScanInit(0, RecMap ) != S_OK )
      SS_THROW( L"Ошибка при инициализации сканирования таблицы Abbreviations" );

   TFormLowerToRecord::const_iterator I;

   while( pTable->Scan() == S_OK )
   {
      //DEBUG_MSG( ConvertToNarrow(TableName) << ": " << ConvertToNarrow(Record.FormLower) );

      I = m_FormLowerToRecord.insert( TFormLowerToRecord::value_type(Record.FormLower, Record) );
      m_DictionaryIndexToRecordPtr.insert( 
         TDictionaryIndexToRecordPtr::value_type( Record.IDSource, &I->second )
      );    
   }

   APL_ASSERT( m_FormLowerToRecord.size() == m_DictionaryIndexToRecordPtr.size() );

   return !m_FormLowerToRecord.empty();
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionaryBaseMemory::IsExistWord(/*in*/const wchar_t* wcWord) const
{
   APL_ASSERT_PTR( wcWord );
   APL_ASSERT( m_pMorphoTypesSearch != 0 /*&& !m_FormLowerToRecord.empty() && !m_DictionaryIndexToRecordPtr.empty()*/ );

   TMorphoInfo MorphoInfo = 0;
   TSpecialMorpho SpecialMorpho = 0;

   TFormToRecordIterPair IP = m_FormLowerToRecord.equal_range(wcWord);

   for( ; IP.first != IP.second; ++IP.first )
      if( m_pMorphoTypesSearch->GetMorphoInfoByIDType( IP.first->second.IDType, 0, &MorphoInfo, &SpecialMorpho ) == TRUE )
         return true;

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionaryBaseMemory::GetWordInfo/*ByWord*/(
	/*in*/const wchar_t* wcWord, 
   /*in*/const wchar_t* wcWordOriginalCase,
   /*in*/bool IsFirstLetterUpperCase,
	/*out*/std::list<TWordInfo>* p_l_WordInfo
) const
{
   APL_ASSERT_PTR( wcWord );
   APL_ASSERT_PTR( p_l_WordInfo );
   APL_ASSERT( m_pMorphoTypesSearch != 0 /*&& !m_FormLowerToRecord.empty() && !m_DictionaryIndexToRecordPtr.empty()*/ );

   p_l_WordInfo->clear();

   TFormToRecordIterPair IP = m_FormLowerToRecord.equal_range(wcWord);
   SWordInfo *pWordInfo = 0;
   TMorphoInfo MorphoInfo = 0;
   TSpecialMorpho SpecialMorpho = 0;

   for( ; IP.first != IP.second; ++IP.first )
      if( m_pMorphoTypesSearch->GetMorphoInfoByIDType( IP.first->second.IDType, 0, &MorphoInfo, &SpecialMorpho ) == TRUE )
      {
         p_l_WordInfo->push_back( TWIList::value_type() );
         p_l_WordInfo->back().push_back( SWordInfo() );
         pWordInfo = &p_l_WordInfo->back().back();

         FillSWordInfo(	wcWordOriginalCase, IP.first->second.IDSource, IP.first->second.IDForm, IP.first->second.IDType, SpecialMorpho, MorphoInfo, *pWordInfo, IsFirstLetterUpperCase );
      }

   return !p_l_WordInfo->empty();
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionaryBaseMemory::GetWordInfo/*ByWordIndex*/(
	/*in*/SWordIndex* pWordIndex, 
	/*out*/SWordInfo* pWordInfo
) const
{
   APL_ASSERT_PTR( pWordIndex );
   APL_ASSERT_PTR( pWordInfo );
   APL_ASSERT( m_pMorphoTypesSearch != 0 /*&& !m_FormLowerToRecord.empty() && !m_DictionaryIndexToRecordPtr.empty()*/ );
   APL_ASSERT( pWordIndex->m_DictIndex.GetDictionaryNumber() == m_DictIndex );

   TDIToRecordPtrIterPair IP = m_DictionaryIndexToRecordPtr.equal_range(pWordIndex->m_DictIndex.GetId());
   TMorphoInfo MorphoInfo = 0;
   TSpecialMorpho SpecialMorpho = 0;
   std::wstring Word;

   for( ; IP.first != IP.second; ++IP.first )
      if( 
         IP.first->second->IDForm == pWordIndex->m_IDForm &&
         m_pMorphoTypesSearch->GetMorphoInfoByIDType( IP.first->second->IDType, 0, &MorphoInfo, &SpecialMorpho ) == TRUE
      )
      {
         Word = IP.first->second->Form;
         Detail::CorrectWordCase( Word, pWordIndex->m_DictIndex );

         FillSWordInfo(	Word, IP.first->second->IDSource, IP.first->second->IDForm, IP.first->second->IDType, SpecialMorpho, MorphoInfo, *pWordInfo, pWordIndex->m_DictIndex.GetFirstLetterUpperCase() );
         return true;
      }
   
   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionaryBaseMemory::GetWord(
	/*in*/SWordIndex* pWordIndex,
	/*out*/wchar_t* wcWord
) const
{
   APL_ASSERT_PTR( pWordIndex );
   APL_ASSERT_PTR( wcWord );
   APL_ASSERT( m_pMorphoTypesSearch != 0 /*&& !m_FormLowerToRecord.empty() && !m_DictionaryIndexToRecordPtr.empty()*/ );
   APL_ASSERT( pWordIndex->m_DictIndex.GetDictionaryNumber() == m_DictIndex );

   TDIToRecordPtrIterPair IP = m_DictionaryIndexToRecordPtr.equal_range(pWordIndex->m_DictIndex.GetId());

   for( ; IP.first != IP.second; ++IP.first )
      if( IP.first->second->IDForm == pWordIndex->m_IDForm )
      {
         APL_ASSERT( IP.first->second->Form.length() < Constants::MAX_CHARS_IN_WORD );

         //Соединяем в одно слово и копируем в массив
         std::memcpy( wcWord, IP.first->second->Form.c_str(), (IP.first->second->Form.length() + 1 )* sizeof(*wcWord) ); // "+ 1" для того чтобы сразу скопировать '\0'
         Detail::CorrectWordCase( wcWord, pWordIndex->m_DictIndex );

         return true;
      }
   
   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionaryBaseMemory::GetWordFormsByIDSource(
	/*in*/TDictionaryIndex* pDictionaryIndex,
	/*out*/std::list<TWordInfo>* p_l_WordInfo
) const
{
   APL_ASSERT_PTR( pDictionaryIndex );
   APL_ASSERT_PTR( p_l_WordInfo );
   APL_ASSERT( pDictionaryIndex->GetDictionaryNumber() == m_DictIndex );
   APL_ASSERT( m_pMorphoTypesSearch != 0 /*&& !m_FormLowerToRecord.empty() && !m_DictionaryIndexToRecordPtr.empty()*/ );

   p_l_WordInfo->clear();

   TDIToRecordPtrIterPair IP = m_DictionaryIndexToRecordPtr.equal_range(pDictionaryIndex->GetId());
   SWordInfo *pWordInfo = 0;
   TMorphoInfo MorphoInfo = 0;
   TSpecialMorpho SpecialMorpho = 0;
   std::wstring Word;

   for( ; IP.first != IP.second; ++IP.first )
   {
	   if(!IP.first->second->Form.empty() &&
		  (m_pMorphoTypesSearch->GetMorphoInfoByIDType( IP.first->second->IDType, 0, &MorphoInfo, &SpecialMorpho ) == TRUE))
      {
         p_l_WordInfo->push_back( TWIList::value_type() );
         p_l_WordInfo->back().push_back( SWordInfo() );
         pWordInfo = &p_l_WordInfo->back().back();
         
         Word = IP.first->second->Form;
         Detail::CorrectWordCase( Word, *pDictionaryIndex );

         FillSWordInfo(	Word, IP.first->second->IDSource, IP.first->second->IDForm, IP.first->second->IDType, SpecialMorpho, MorphoInfo, *pWordInfo, pDictionaryIndex->GetFirstLetterUpperCase() );
      }
   }

   return !p_l_WordInfo->empty();
}

///////////////////////////////////////////////////////////////////////////////
// class CAbbreviationsDictionary
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionary::Reload()
{
   SS_TRY
   {
      bool RetVal = true;

      for( 
         TDictNumberToAbbreviation::iterator I = m_DictNumberToAbbreviation.begin();    
         I != m_DictNumberToAbbreviation.end();
         ++I 
      )
      {
         if( !I->second->Reload() )
            RetVal = false;
      }

      return RetVal;
   }
   SS_CATCH(L"")

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionary::IsExistWord(/*in*/const wchar_t* wcWord)
{
   APL_ASSERT_PTR( wcWord );

   SS_TRY
   {
      for( 
         TDictNumberToAbbreviation::const_iterator I = m_DictNumberToAbbreviation.begin();    
         I != m_DictNumberToAbbreviation.end();
         ++I 
      )
         {
            if( I->second->IsExistWord(wcWord) )
               return true;
         }
   }
   SS_CATCH(L"")

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionary::GetWordInfo/*ByWord*/(
                           /*in*/const wchar_t* wcWord, 
                           /*in*/const wchar_t* wcWordOriginalCase,
                           /*in*/bool IsFirstLetterUpperCase,
                           /*out*/std::list<TWordInfo>* p_l_WordInfo
                           )
{
   APL_ASSERT_PTR( wcWord );

   SS_TRY
   {
      p_l_WordInfo->clear();
      std::list<TWordInfo> BuffList;
      
      for( 
         TDictNumberToAbbreviation::const_iterator I = m_DictNumberToAbbreviation.begin();    
         I != m_DictNumberToAbbreviation.end();
         ++I 
      )
      {
          I->second->GetWordInfo(wcWord, wcWordOriginalCase, IsFirstLetterUpperCase, &BuffList);
          p_l_WordInfo->splice( p_l_WordInfo->end(), BuffList );
      }
   }
   SS_CATCH(L"")

   return !p_l_WordInfo->empty();
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionary::GetWordInfo/*ByWordIndex*/(
                                /*in*/SWordIndex* pWordIndex, 
                                /*out*/SWordInfo* pWordInfo
                                )
{
   APL_ASSERT_PTR( pWordIndex );
   APL_ASSERT_PTR( pWordInfo );
                
   SS_TRY
   {
      return Get( pWordIndex->m_DictIndex.GetDictionaryNumber() )->GetWordInfo(pWordIndex, pWordInfo);
   }
   SS_CATCH(L"")

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionary::GetWord(
             /*in*/SWordIndex* pWordIndex,
             /*out*/wchar_t* wcWord
             )
{
   APL_ASSERT_PTR( pWordIndex );
   APL_ASSERT_PTR( wcWord );

   SS_TRY
   {
      return Get( pWordIndex->m_DictIndex.GetDictionaryNumber() )->GetWord(pWordIndex, wcWord);
   }
   SS_CATCH(L"")

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionary::GetWordSource(
   /*in*/const TDictionaryIndex &DictIndex,
   /*out*/wchar_t* wcWordSource
)
{
   APL_ASSERT_PTR( wcWordSource );

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CAbbreviationsDictionary::GetWordFormsByIDSource(
                            /*in*/TDictionaryIndex* pDictionaryIndex,
                            /*out*/std::list<TWordInfo>* p_l_WordInfo
                            )
{
   APL_ASSERT_PTR( pDictionaryIndex );
   APL_ASSERT_PTR( p_l_WordInfo );

   SS_TRY
   {
      return Get( pDictionaryIndex->GetDictionaryNumber() )->GetWordFormsByIDSource(pDictionaryIndex, p_l_WordInfo);
   }
   SS_CATCH(L"")

   return false;
}

} //namespace Dictionary
} //namespace SS
