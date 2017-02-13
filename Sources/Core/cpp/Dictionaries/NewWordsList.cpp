#include "stdafx.h"

#include "NewWordsList.h"

namespace SS{ 
namespace Dictionary{

///////////////////////////////////////////////////////////////////////////////
// class CNewWordsListBasic
///////////////////////////////////////////////////////////////////////////////
   
void CNewWordsListBasic::Init( DBMS::IDataBase *pNewWordsDataBase, DBMS::IDataBase *pMorphoDataBase, TDictNumber DictNumber, const TCHAR *TableName )
{
   APL_ASSERT_PTR( pNewWordsDataBase );
   APL_ASSERT_PTR( pMorphoDataBase );
   APL_ASSERT_PTR( TableName );

   m_DictNumber = DictNumber;
   m_pMorphoTypesSearch = CMorphoTypesSearchCreateManager::Create()->Construct(pMorphoDataBase, NULL);

   if( m_Table.Open(pNewWordsDataBase, TableName) != S_OK )
      APL_THROW( _T("Ошибка при открыии таблицы ") << TableName );
}
///////////////////////////////////////////////////////////////////////////////

unsigned int CNewWordsListBasic::GetCount()
{
   return m_Table->GetRecordsNumber();
}
///////////////////////////////////////////////////////////////////////////////

bool CNewWordsListBasic::MoveFirst()
{
   if( m_Table->ScanInit(0, m_RecMap) != S_OK )
      APL_THROW( _T("Ошибка при инициализации поиска в таблице новых слов") );

   return m_Table->Scan() == S_OK;
}
///////////////////////////////////////////////////////////////////////////////

bool CNewWordsListBasic::MoveNext(TWordInfo &WordInfo)
{
   if( m_Table->Scan() != S_OK )
      return false;

   GetWord( WordInfo );

   return true;
}
///////////////////////////////////////////////////////////////////////////////

void CNewWordsListBasic::GetWord(TWordInfo &WordInfo)
{
   TMorphoInfo MorphoInfo = 0;
   TSpecialMorpho SpecialMorpho = 0;
   
   WordInfo.clear();
   
   if( m_pMorphoTypesSearch->GetMorphoInfoByIDType(m_dbIDType, 0, &MorphoInfo, &SpecialMorpho) != TRUE )
      APL_THROW( _T("Не найдена морфологическая информация для IDType ") << m_dbIDType );

   WordInfo.push_back( TWordInfo::value_type() );
   SWordInfo &WI = WordInfo.back();
   
   WI.m_wWord = m_dbSource;
   WI.m_SpecialMorpho = SpecialMorpho;
   WI.m_WordIndex.m_IDForm = 0; 
   WI.m_WordIndex.m_DictIndex.SetId(m_dbIDSource); 
   WI.m_WordIndex.m_DictIndex.SetDictionaryNumber(m_DictNumber); 
   WI.m_MorphoTypeInfo.m_IDType = m_dbIDType; 		
   WI.m_MorphoTypeInfo.m_MorphoInfo = MorphoInfo;
}

///////////////////////////////////////////////////////////////////////////////
// class CNewWordsList
///////////////////////////////////////////////////////////////////////////////
void CNewWordsList::SetDictionaryNumber( TDictNumber eDictionariesNumber )
{
   SS_TRY
   {
      CNewWordsListBasic *pCurNewWordsList = m_NewWordsLists.Get( eDictionariesNumber );

      if( pCurNewWordsList == 0 )
         APL_THROW( _T("Ошибка при поиске зарегистрированного DictionaryNumber ") << (int)eDictionariesNumber );

      m_pCurNewWordsList = pCurNewWordsList;
   }
   SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

unsigned int CNewWordsList::GetCount()
{
   APL_ASSERT_PTR( m_pCurNewWordsList );

   SS_TRY
   {
      return m_pCurNewWordsList->GetCount();
   }
   SS_CATCH(L"")

   return 0;
}
///////////////////////////////////////////////////////////////////////////////

unsigned int CNewWordsList::MoveFirst()
{
   APL_ASSERT_PTR( m_pCurNewWordsList );

   SS_TRY
   {
      return m_pCurNewWordsList->MoveFirst();
   }
   SS_CATCH(L"")

   return false;
}
///////////////////////////////////////////////////////////////////////////////

unsigned int CNewWordsList::MoveNext( TWordInfo * pWordInfo )
{
   APL_ASSERT_PTR( m_pCurNewWordsList );
   APL_ASSERT_PTR( pWordInfo );

   SS_TRY
   {
      return m_pCurNewWordsList->MoveNext(*pWordInfo);
   }
   SS_CATCH(L"")

   return false;
}
///////////////////////////////////////////////////////////////////////////////

void CNewWordsList::GetWord( TWordInfo * pWordInfo )
{
   APL_ASSERT_PTR( m_pCurNewWordsList );
   APL_ASSERT_PTR( pWordInfo );

   SS_TRY
   {
      m_pCurNewWordsList->GetWord(*pWordInfo);
   }
   SS_CATCH(L"")
}

} //namespace Dictionary
} //namespace SS
