#include "stdafx.h"

#include "OneRoot.h"

namespace SS{ 
namespace Dictionary{
namespace Edit{

///////////////////////////////////////////////////////////////////////////////
// class COneRootSimpleConvert
///////////////////////////////////////////////////////////////////////////////
void COneRootSimpleConvert::Get( TIDSource IDSource, TDIList &SourceList ) const
{
   TDictionaryIndex Index;

   SourceList.clear();
   Index.SetId( IDSource );
   Index.SetDictionaryNumber( m_DictNumber );
   SourceList.push_back(Index);
}

///////////////////////////////////////////////////////////////////////////////
// class CArabic2RomanConvert
///////////////////////////////////////////////////////////////////////////////
void CArabic2RomanConvert::Get( TIDSource IDSource, TDIList &SourceList ) const
{
   if( IDSource < 1 || IDSource > Detail::CRomanArabicConvertor::MaxRomanDigit() )
      return;
   
   TDictionaryIndex Index;

   SourceList.clear();
   Index.SetId( IDSource );
   Index.SetDictionaryNumber( DATA_TYPE::NAMES::ednRomanNumeric );
   SourceList.push_back(Index);
}

///////////////////////////////////////////////////////////////////////////////
// class CSymbolsOneRoot
///////////////////////////////////////////////////////////////////////////////
void CSymbolsOneRoot::AddData( const TSymbols &Symbols )
{
   for( TSymbols::const_iterator I = Symbols.begin(); I != Symbols.end(); ++I )
   {
      APL_CHECK( m_Storage.insert(std::make_pair(*I, Symbols)).second );
   }
}
///////////////////////////////////////////////////////////////////////////////

CSymbolsOneRoot::CSymbolsOneRoot()
{
   AddData( L"\"УФЂї" );
   AddData( L"'СТ" );

}
///////////////////////////////////////////////////////////////////////////////

void CSymbolsOneRoot::Get( TIDSource IDSource, TDIList &SourceList ) const
{
   SourceList.clear();

   TStorage::const_iterator IFind = m_Storage.find(IDSource);
   
   if( IFind == m_Storage.end() )
      return;
   
   TDictionaryIndex Index;

   for( TSymbols::const_iterator I = IFind->second.begin(); I != IFind->second.end(); ++I )
   {
      if( IDSource != *I )
      {
         Index.SetId( *I );
         Index.SetDictionaryNumber( DATA_TYPE::NAMES::ednSymbols );
         SourceList.push_back(Index);
      }
   }
}


///////////////////////////////////////////////////////////////////////////////
// class COneRoot
///////////////////////////////////////////////////////////////////////////////
void COneRoot::Init( DBMS::IDataBase *pDataBase, TDictNumber CurInitDictNumber, const wchar_t *OneRootTableName )
{
	SS_TRY
	{
		APL_ASSERT(pDataBase != 0);
		APL_ASSERT( OneRootTableName != 0 );
		m_CurInitDictNumber = CurInitDictNumber;

		wcscpy(m_wzOneRootTableName, OneRootTableName);
		m_pDataBase = pDataBase;

		Open();
	}
	SS_CATCH(L"")

}

void COneRoot::Open() const
{
	SS_TRY
	{
		if( m_pTable.Open(m_pDataBase, m_wzOneRootTableName) != S_OK)
		{
			SS_THROW( L"Open OneRootTableName" );
		}
	}
	SS_CATCH(L"")
}
void COneRoot::Close() const
{
	SS_TRY
	{
		m_pTable.Close();
	}
	SS_CATCH(L"")
}

///////////////////////////////////////////////////////////////////////////////

void COneRoot::Get( TIDSource IDSource, TDIList &SourceList ) const
{
	SS_TRY
	{
		Open();

		APL_ASSERT( m_pTable.IsOpen() );

		TDictionaryIndex CurIDSourceIndex;
		CurIDSourceIndex.SetDictionaryNumber(m_CurInitDictNumber);
		CurIDSourceIndex.SetId(IDSource);


		INT32 dbIDSource = CurIDSourceIndex.GetDictionaryIndexWithoutFlags();
		INT32 dbIDOneRoot = 0;

		void *RecMapOneRoot[2] = { 0, &dbIDOneRoot };

		CFindHelper FindHelper(m_pTable);

		if( FindHelper.Init(_T("IDSource"), &dbIDSource, RecMapOneRoot) != S_OK )
			SS_THROW( L"ќшибка при инициализации поиска в OneRoot по IDSource" );

		SourceList.clear();

		while( FindHelper.Find() == S_OK )
		{
			SourceList.push_back(TDictionaryIndex());
			SourceList.back().SetId(dbIDOneRoot);
			SourceList.back().SetDictionaryNumber(m_CurInitDictNumber);
		}

		Close();
		Open();
	}
	SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

void COneRoot::Add( TIDSource IDSource, const TDIList &SourceList )
{
	SS_TRY
	{
		Open();
		APL_ASSERT( m_pTable.IsOpen() );

		TDictionaryIndex CurIDSourceIndex;
		CurIDSourceIndex.SetDictionaryNumber(m_CurInitDictNumber);
		CurIDSourceIndex.SetId(IDSource);

		INT32 dbIDSource = CurIDSourceIndex.GetDictionaryIndexWithoutFlags();
		INT32 dbIDOneRoot = 0;

		void *RecMapOneRoot[2] = { &dbIDSource, &dbIDOneRoot };

		m_pTable->AddInit( RecMapOneRoot );

		for( TDIList::const_iterator I = SourceList.begin(); I != SourceList.end(); ++I )
		{
			APL_ASSERT( I->GetDictionaryNumber() );
			dbIDOneRoot = I->GetId();

			if( m_pTable->Add() != S_OK )
				SS_THROW( L"ќшибка при добавлении в OneRoot" );
		}
		Close();
		Open();
	}
	SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

void COneRoot::Delete( TIDSource IDSource, const TDIList &SourceList )
{
	SS_TRY
	{
		Open();

		APL_ASSERT( m_pTable.IsOpen() );

		TDictionaryIndex CurIDSourceIndex;
		CurIDSourceIndex.SetDictionaryNumber(m_CurInitDictNumber);
		CurIDSourceIndex.SetId(IDSource);

		INT32 dbIDSource = CurIDSourceIndex.GetDictionaryIndexWithoutFlags();
		INT32 dbIDOneRoot = 0;
		void *RecMapOneRoot[2] = {0 , &dbIDOneRoot};
		CFindHelper FindHelper(m_pTable);


		typedef std::map<unsigned int, UINT32> TIDOneRoot2RecordIdx;
		TIDOneRoot2RecordIdx IDOneRoot2RecordIdx;
		TIDOneRoot2RecordIdx::iterator FindI;

		if( FindHelper.Init(_T("IDSource"), &dbIDSource, RecMapOneRoot) != S_OK )   
			SS_THROW( L"ќшибка при инициализации поиска в NominalOneRoot по IDSource" );

		while( FindHelper.Find() == S_OK )
			IDOneRoot2RecordIdx.insert( TIDOneRoot2RecordIdx::value_type(dbIDOneRoot, FindHelper.GetFoundIdx()) );

		for( TDIList::const_iterator I = SourceList.begin(); I != SourceList.end(); ++I )
		{
			APL_ASSERT( I->GetDictionaryNumber() );
			FindI = IDOneRoot2RecordIdx.find(I->GetId());

			if( FindI != IDOneRoot2RecordIdx.end() )
			{
				//ћы нашли нужную запись удал€ем еЄ
				if( m_pTable->DeleteRecord(FindI->second) != S_OK )
					SS_THROW(L"ќшибка удалени€ записи из NominalOneRoot");
			}
		}

		Close();
		Open();
	}
	SS_CATCH(L"")
}

///////////////////////////////////////////////////////////////////////////////
// class COneRootAdapter
///////////////////////////////////////////////////////////////////////////////
unsigned int COneRootAdapter::SetDictionaryIndex(SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex)
{
   SS_TRY
   {
      IOneRootBasic *pCurOneRoot = m_OneRoots.Get( oDictionaryIndex.GetDictionaryNumber() );

      if( pCurOneRoot == 0 )
	  {
		  return false;
         SS_THROW( L"ќшибка при поиске зарегистрированного DictionaryNumber" );
	  }

      m_pCurOneRoot = pCurOneRoot;
      m_CurIDSource = oDictionaryIndex.GetId();
   }
   SS_CATCH(L"")

   return true;
}
///////////////////////////////////////////////////////////////////////////////

void COneRootAdapter::Get(std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_IDSource)
{
   APL_ASSERT_PTR( m_pCurOneRoot );
   APL_ASSERT_PTR( p_l_IDSource );

   SS_TRY
   {
      m_pCurOneRoot->Get( m_CurIDSource, *p_l_IDSource );
   }
   SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

void COneRootAdapter::Add(std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_IDSource)
{
   APL_ASSERT_PTR( m_pCurOneRoot );
   APL_ASSERT_PTR( p_l_IDSource );

   SS_TRY
   {
      m_pCurOneRoot->Add( m_CurIDSource, *p_l_IDSource );
   }
   SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

void COneRootAdapter::Delete(std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_IDSource)
{
   APL_ASSERT_PTR( m_pCurOneRoot );
   APL_ASSERT_PTR( p_l_IDSource );

   SS_TRY
   {
      m_pCurOneRoot->Delete( m_CurIDSource, *p_l_IDSource );
   }
   SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

} //namespace Edit
} //namespace Dictionary
} //namespace SS
