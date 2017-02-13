#include "stdafx.h"

#include "Source.h"

namespace SS{ 
namespace Dictionary{
namespace Edit{	
///////////////////////////////////////////////////////////////////////////////
// class CSource
///////////////////////////////////////////////////////////////////////////////
void CSource::Init( 
          DBMS::IDataBase *pDataBase, 
          TDictNumber CurInitDictNumber, 
          const wchar_t *SourceFormTableName, 
          const wchar_t *SourceToMorphoTableName
)
{
	APL_ASSERT(pDataBase != 0);
	APL_ASSERT(SourceFormTableName != 0 && SourceToMorphoTableName != 0);


	m_CurInitDictNumber = CurInitDictNumber;
	
	m_pDataBase = pDataBase;

	wcscpy(m_wzSourceFormTableName, SourceFormTableName);
	wcscpy(m_wzSourceToMorphoTableName, SourceToMorphoTableName);
	wcscpy(m_wzMorphoTypeToMorphoInfoTable, L"MorphoTypeToMorphoInfo");

	Open();

}
///////////////////////////////////////////////////////////////////////////////
void CSource::Open() const
{
	if( m_pSourceForm.Open(m_pDataBase, m_wzSourceFormTableName) != S_OK)
		SS_THROW( L"Open SourceForm" );

	if( m_pSourceToMorpho.Open(m_pDataBase, m_wzSourceToMorphoTableName) != S_OK)
		SS_THROW( L"Open SourceToMorpho" );

	if( m_pMorphoTypeToMorphoInfo.Open(m_pDataBase, m_wzMorphoTypeToMorphoInfoTable) != S_OK )
		SS_THROW( L"Open MorphoTypeToMorphoInfo" );
}

void CSource::Close() const
{
	m_pSourceForm.Close();
	m_pSourceToMorpho.Close();
	m_pMorphoTypeToMorphoInfo.Close();
}
	
bool CSource::Get( TIDSource IDSource, SSource &Source ) const
{
	SS_TRY
	{
		Open();

		APL_ASSERT( m_pSourceForm.IsOpen() && m_pSourceToMorpho.IsOpen() && m_pMorphoTypeToMorphoInfo.IsOpen() );

		INT32 dbIDSource = IDSource;
		std::wstring dbPartOfWord;
		INT32 dbIDType = 0;
		INT32 dbMorphoInfo = 0;

		void *RecMapSourceForm[2] = { 0, &dbPartOfWord };
		void *RecMapSourceToMorpho[2] = { 0, &dbIDType };
		void *RecMapMorphoTypeToMorphoInfo[2] = { 0, &dbMorphoInfo };
		CFindHelper FindHelperSourceForm(m_pSourceForm);
		CFindHelper FindHelperSourceToMorpho(m_pSourceToMorpho);
		CFindHelper FindHelperMorphoTypeToMorphoInfo(m_pMorphoTypeToMorphoInfo);

		//Сначала попытаемся найти PartOfWord
		if( FindHelperSourceForm.Init(_T("ID"), &dbIDSource, RecMapSourceForm) != S_OK )
			SS_THROW( L"Ошибка при инициализации поиска в SourceForm по ID" );

		if( FindHelperSourceForm.FindOne() != S_OK )
			return false;

		//Теперь IDType
		if( FindHelperSourceToMorpho.Init(_T("IDSource"), &dbIDSource, RecMapSourceToMorpho) != S_OK )
			SS_THROW( L"Ошибка при инициализации поиска в SourceToMorpho по IDSource" );

		if( FindHelperSourceToMorpho.FindOne() != S_OK )
			SS_THROW( L"В таблице SourceToMorpho нет записи для IDSource" );

		//Ну и MorphoInfo, раз на то пошло :)
		if( FindHelperMorphoTypeToMorphoInfo.Init(_T("IDType"), &dbIDType, RecMapMorphoTypeToMorphoInfo) != S_OK )
			SS_THROW( L"Ошибка при инициализации поиска в MorphoTypeToMorphoInfo по IDType" );

		if( FindHelperMorphoTypeToMorphoInfo.FindOne() != S_OK )
			SS_THROW( L"В таблице MorphoTypeToMorphoInfo нет записи для IDType" );

		//Заполним структуру
		Source.m_DictIndex.SetId(IDSource);
		Source.m_DictIndex.SetDictionaryNumber( m_CurInitDictNumber );
		Source.m_MorphoTypeInfo.m_IDType = dbIDType;
		Source.m_MorphoTypeInfo.m_MorphoInfo  = dbMorphoInfo;
		Source.m_wSource = dbPartOfWord;


		Close();
		Open();

		return true;
	}
	SS_CATCH(L"")
	return false;
}
///////////////////////////////////////////////////////////////////////////////

void CSource::Add( const SSource &Source, TIDSource &IDSource )
{
	Open();

	APL_ASSERT( m_pSourceForm.IsOpen() && m_pSourceToMorpho.IsOpen() && m_pMorphoTypeToMorphoInfo.IsOpen() );
	APL_ASSERT( Source.m_DictIndex.GetDictionaryNumber() == m_CurInitDictNumber );

	INT32 dbIDSource = 0;
	std::wstring dbPartOfWord = Source.m_wSource;
	void *RecMapSourceForm[2] = { &dbIDSource, &dbPartOfWord };

	//Т.к. перед GetUserValue Add и SetUserValue к этой таблицы могут обратиться другие объекты этого класса
	//необходимо им всем запретить к ней доступ
	//Конечно, мы пренибрегаем тем что в таблицу можно добавлять из вне этого класса
	{
		TMutex::Lock Guard(m_Mutex); 

		IDSource = dbIDSource = m_pSourceForm->GetUserValue(0);

		if( m_pSourceForm->AddInit(RecMapSourceForm) != S_OK )
			SS_THROW(L"Ошибка при инициализации добавсления в SourceForm");

		if( m_pSourceForm->Add() != S_OK )
			SS_THROW(L"Ошибка при добавслении в SourceForm");

		if( m_pSourceForm->SetUserValue(0, dbIDSource + 1) != S_OK )
			SS_THROW(L"Ошибка SetUserValue таблицы SourceForm");
	}

	//Добавляем в SourceToMorpho
	INT32 IDType = Source.m_MorphoTypeInfo.m_IDType;
	void *RecMapSourceToMorpho[2] = { &dbIDSource, &IDType };

	if( m_pSourceToMorpho->AddInit(RecMapSourceToMorpho) != S_OK )
		SS_THROW(L"Ошибка при инициализации добавсления в SourceToMorpho");

	if( m_pSourceToMorpho->Add() != S_OK )
		SS_THROW(L"Ошибка при добавслении в SourceToMorpho");

	Close();
	Open();

}
///////////////////////////////////////////////////////////////////////////////

void CSource::Delete( TIDSource IDSource )
{
	Open();

	APL_ASSERT( m_pSourceForm.IsOpen() && m_pSourceToMorpho.IsOpen() && m_pMorphoTypeToMorphoInfo.IsOpen() );

	INT32 dbIDSource = IDSource;
	void *RecMapSourceForm[2] = {};

	CFindHelper FindHelperSourceForm(m_pSourceForm);

	if( FindHelperSourceForm.Init(_T("ID"), &dbIDSource, RecMapSourceForm) != S_OK )
		SS_THROW( L"Ошибка при инициализации поиска в SourceForm по ID" );

	while( FindHelperSourceForm.Find() == S_OK )
	{
		if( m_pSourceForm->DeleteRecord(FindHelperSourceForm.GetFoundIdx()) != S_OK )
		{
			SS_THROW(L"Ошибка удаления записи из SourceForm");
		}	
	}

	void *RecMapSourceToMorpho[2] = {};

	CFindHelper FindHelperSourceToMorpho(m_pSourceToMorpho);

	if( FindHelperSourceToMorpho.Init(_T("IDSource"), &dbIDSource, RecMapSourceToMorpho) != S_OK )
		SS_THROW( L"Ошибка при инициализации поиска в SourceToMorpho по IDSource" );

	while( FindHelperSourceToMorpho.Find() == S_OK )
	{
		if( m_pSourceToMorpho->DeleteRecord(FindHelperSourceToMorpho.GetFoundIdx()) != S_OK )
		{
			SS_THROW(L"Ошибка удаления записи из SourceToMorpho");
		}	
	}

	Close();
	Open();

}
///////////////////////////////////////////////////////////////////////////////
// CSourceAdapter
///////////////////////////////////////////////////////////////////////////////
unsigned int CSourceAdapter::SetDictionaryIndex(SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex)
{
	SS_TRY
	{
		CSource *pCurSource = m_Sources.Get( oDictionaryIndex.GetDictionaryNumber() );

		if( pCurSource == 0 )
			return false;
			//SS_THROW( L"Ошибка при поиске зарегистрированного DictionaryNumber" );

		m_pCurSource = pCurSource;
		m_CurIDSource = oDictionaryIndex.GetId();
	}
	SS_CATCH(L"")

	return true;
}
///////////////////////////////////////////////////////////////////////////////

unsigned int CSourceAdapter::Get(SS::Dictionary::Types::SSource* pSource)
{
   APL_ASSERT_PTR( m_pCurSource );
   APL_ASSERT_PTR( pSource );
   
   SS_TRY
   {
      return m_pCurSource->Get( m_CurIDSource, *pSource );
   }
   SS_CATCH(L"")

   return false;
}
///////////////////////////////////////////////////////////////////////////////

void CSourceAdapter::Add(SS::Dictionary::Types::SSource* pSource)
{
   APL_ASSERT_PTR( m_pCurSource );
   APL_ASSERT_PTR( pSource );

   SS_TRY
   {
      CSource::TIDSource IDSource;
      m_pCurSource->Add( *pSource, IDSource );
      pSource->m_DictIndex.SetId( IDSource );
      m_CurIDSource = IDSource;
   }
   SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

void CSourceAdapter::Delete()
{
   APL_ASSERT_PTR( m_pCurSource );

   SS_TRY
   {
      m_pCurSource->Delete( m_CurIDSource );
   }
   SS_CATCH(L"")
}

}
} //namespace Dictionary
} //namespace SS
					  