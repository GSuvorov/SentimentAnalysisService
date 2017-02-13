#include "stdafx.h"

#include "MorphoTypesCollection.h"

namespace SS{ 
namespace Dictionary{
namespace Edit{

///////////////////////////////////////////////////////////////////////////////
// class CMorphoTypesCollection
///////////////////////////////////////////////////////////////////////////////

const wchar_t *CMorphoTypesCollection::szMorphoTypesTable = _T("MorphoTypes");
const wchar_t *CMorphoTypesCollection::szMorphoTypeToMorphoInfoTable = _T("MorphoTypeToMorphoInfo");
///////////////////////////////////////////////////////////////////////////////

void CMorphoTypesCollection::Init( DBMS::IDataBase *pDataBase)
{
	SS_TRY
	{
		m_pDataBase = pDataBase;

		Open();
	}
	SS_CATCH(L"")
}

void CMorphoTypesCollection::Open()
{
	SS_TRY
	{
		if( m_pMorphoTypes.Open(m_pDataBase, szMorphoTypesTable) != S_OK )
			SS_THROW(L"Open MorphoTypes");
		if( m_pMorphoTypeToMorphoInfo.Open(m_pDataBase, szMorphoTypeToMorphoInfoTable) != S_OK )
			SS_THROW(L"Open MorphoTypeToMorphoInfo");
	}
	SS_CATCH(L"")
}

void CMorphoTypesCollection::Close()
{
	SS_TRY
	{
		m_pMorphoTypes.Close();
		m_pMorphoTypeToMorphoInfo.Close();
	}
	SS_CATCH(L"")
}


///////////////////////////////////////////////////////////////////////////////

void CMorphoTypesCollection::DeleteType( const TIDType &IDType )
{
	SS_TRY
	{
		Open();

		APL_ASSERT( m_pMorphoTypes.IsOpen() && m_pMorphoTypeToMorphoInfo.IsOpen() );

		INT32 dbIDType = IDType;
		void *RecMapMorphoTypes[4] = {};

		CFindHelper MorphoTypesFind(m_pMorphoTypes);

		if( MorphoTypesFind.Init(_T("IDType"), &dbIDType, RecMapMorphoTypes) != S_OK )   
			SS_THROW( L"Ошибка при инициализации поиска в MorphoType по IDType" );

		while( MorphoTypesFind.Find() == S_OK )
		{
			if( m_pMorphoTypes->DeleteRecord(MorphoTypesFind.GetFoundIdx()) != S_OK )
			{
				SS_THROW(L"Ошибка удаления записи из MorphoTypes");
			}	
		}
		Close();
		Open();
	}
	SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

void CMorphoTypesCollection::GetElements( const TIDType &IDType, std::list<SMorphoType>* p_l_MorphoTypes )
{
	SS_TRY
	{
		Open();

		APL_ASSERT(p_l_MorphoTypes != 0);
		APL_ASSERT( m_pMorphoTypes.IsOpen() && m_pMorphoTypeToMorphoInfo.IsOpen() );

		INT32 dbIDForm;   
		INT32 dbIDType = IDType;
		INT32 dbIDEnding;    
		INT32 dbPartOfSpeech; 

		void *RecMapMorphoTypes[4] = {&dbIDForm, 0, &dbIDEnding, &dbPartOfSpeech};

		INT32 dbMorphoInfo;
		void *RecMapMorphoTypeToMorphoInfo[2] = { 0, &dbMorphoInfo };

		SMorphoType MorphoTypeRec;

		p_l_MorphoTypes->clear();

		CFindHelper MorphoTypesFind(m_pMorphoTypes);
		CFindHelper MorphoTypeToMorphoInfoFind(m_pMorphoTypeToMorphoInfo);

		if( MorphoTypesFind.Init(_T("IDType"), &dbIDType, RecMapMorphoTypes) != S_OK )   
			SS_THROW( L"Ошибка при инициализации поиска в MorphoType по IDType" );

		if( MorphoTypeToMorphoInfoFind.Init(_T("IDType"), &dbIDType, RecMapMorphoTypeToMorphoInfo) != S_OK )   
			SS_THROW( L"Ошибка при инициализации поиска в MorphoTypeToMorphoInfo по IDType" );

		while( MorphoTypesFind.Find() == S_OK )
		{
			MorphoTypeRec.m_IDEnding = dbIDEnding;
			MorphoTypeRec.m_IDForm  = dbIDForm;
			MorphoTypeRec.m_MorphoTypeInfo.m_IDType = dbIDType;
			MorphoTypeRec.m_SpecialMorpho = dbPartOfSpeech;

			APL_CHECK_S_OK( MorphoTypeToMorphoInfoFind.FindStop() );

			if( MorphoTypeToMorphoInfoFind.Find() != S_OK )
				SS_THROW( L"Невозможно найти ни одну запись в MorphoTypeToMorphoInfo для IDType" );
			do 
			{
				MorphoTypeRec.m_MorphoTypeInfo.m_MorphoInfo = dbMorphoInfo;	

				p_l_MorphoTypes->push_back(MorphoTypeRec);
			} 
			while(MorphoTypeToMorphoInfoFind.Find() == S_OK);	
		}

		Close();
		Open();
	}
	SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

HRESULT CMorphoTypesCollection::AddElements(std::list<SMorphoType>* p_l_MorphoTypes)
{
	SS_TRY
	{
		Open();

		APL_ASSERT(p_l_MorphoTypes != 0);
		APL_ASSERT( m_pMorphoTypes.IsOpen() && m_pMorphoTypeToMorphoInfo.IsOpen() );

		HRESULT RetVal = S_OK;

		Detail::CIDTypeReferences TypeReferences( m_pMorphoTypes.Get(), m_pMorphoTypeToMorphoInfo.Get() );
		Detail::CIDTypeReferences::TItem *pCurItem;
		bool IsAdd; //Удалось ли куда нибудь добавить

		INT32 dbIDForm = 0;   
		INT32 dbIDType = 0;
		INT32 dbIDEnding = 0;    
		INT32 dbPartOfSpeech = 0; 

		void *RecMapMorphoTypes[4] = {&dbIDForm, &dbIDType, &dbIDEnding, &dbPartOfSpeech};

		INT32 dbMorphoInfo = 0;
		void *RecMapMorphoTypeToMorphoInfo[2] = { &dbIDType, &dbMorphoInfo };

		if( m_pMorphoTypes->AddInit(RecMapMorphoTypes) != S_OK )   
			SS_THROW( L"Ошибка при инициализации добавления в MorphoType" );

		if( m_pMorphoTypeToMorphoInfo->AddInit(RecMapMorphoTypeToMorphoInfo) != S_OK )   
			SS_THROW( L"Ошибка при инициализации добавления в MorphoTypeToMorphoInfo" );

		for( std::list<SMorphoType>::const_iterator I = p_l_MorphoTypes->begin(); I != p_l_MorphoTypes->end(); ++I )
		{
			pCurItem = &TypeReferences.Find(I->m_MorphoTypeInfo.m_IDType);
			
			IsAdd = false;

			if( !pCurItem->IsExistsMorphoInfo(I->m_MorphoTypeInfo.m_MorphoInfo) )
			{
				//Добавляем запись в MorphoTypeToMorphoInfo
				dbIDType = I->m_MorphoTypeInfo.m_IDType;
				dbMorphoInfo = I->m_MorphoTypeInfo.m_MorphoInfo;
				
				if( m_pMorphoTypeToMorphoInfo->Add() != S_OK )
					SS_THROW( L"Ошибка при добавлении в MorphoTypeToMorphoInfo" );

 				pCurItem->AddMorphoInfo( I->m_MorphoTypeInfo.m_MorphoInfo );
				IsAdd = true;
			}

  			if( !pCurItem->IsExistsIDForm(I->m_IDForm) )
			{
				//Добавляем запись MorphoTypes
				dbIDForm = I->m_IDForm;   
				dbIDType = I->m_MorphoTypeInfo.m_IDType;
				dbIDEnding = I->m_IDEnding;    
				dbPartOfSpeech = I->m_SpecialMorpho; 

				if( m_pMorphoTypes->Add() != S_OK )
					SS_THROW( L"Ошибка при добавлении в MorphoType" );

				pCurItem->AddIDForm( I->m_IDForm );
				IsAdd = true;
			}

			if( !IsAdd )
				RetVal = E_INVALIDARG;
		}

		return RetVal;

		Close();
		Open();
	}
	SS_CATCH(L"")
	return E_FAIL;
}

///////////////////////////////////////////////////////////////////////////////

HRESULT CMorphoTypesCollection::CreateCollection(std::list<SMorphoType>* p_l_MorphoTypes)
{
	SS_TRY
	{
		Open();

		APL_ASSERT( p_l_MorphoTypes != 0 );
		APL_ASSERT( m_pMorphoTypes.IsOpen() && m_pMorphoTypeToMorphoInfo.IsOpen() );

		//Т.к. перед GetUserValue Add и SetUserValue к этой таблицы могут обратиться другие объекты этого класса
		//необходимо им всем запретить к ней доступ
		//Конечно, мы пренибрегаем тем что в таблицу можно добавлять из вне этого класса
		TMutex::Lock Guard(m_Mutex); 
		INT32 dbIDType = m_pMorphoTypes->GetUserValue(0) + 1; //В таблице не должно быть нулевых индексов 

		for( std::list<SMorphoType>::iterator I = p_l_MorphoTypes->begin(); I != p_l_MorphoTypes->end(); ++I )
			I->m_MorphoTypeInfo.m_IDType = dbIDType;

		HRESULT RetVal = AddElements(p_l_MorphoTypes);

		APL_CHECK_S_OK( m_pMorphoTypes->SetUserValue(0, dbIDType + 1) );

		return  RetVal;

		Close();
		Open();
	}
	SS_CATCH(L"")
	return E_FAIL;
}
///////////////////////////////////////////////////////////////////////////////

void CMorphoTypesCollection::RestoreCorrectIDFormOrder( const TIDType &IDType )
{
	APL_ASSERT( m_pMorphoTypes.IsOpen() && m_pMorphoTypeToMorphoInfo.IsOpen() );

	typedef std::map<TIDForm, UINT32> TFromToRecIndx;  //Отношение IDForm к индексу записи в таблице
	TFromToRecIndx FromToRecIndx;

	INT32 dbIDForm;
	INT32 dbIDType = IDType;

	void *RecMapMorphoTypes[4] = { &dbIDForm };

	//Сканируем таблицу и находим все IDForm
	CFindHelper MorphoTypesFind(m_pMorphoTypes);

	if( MorphoTypesFind.Init(_T("IDType"), &dbIDType, RecMapMorphoTypes) != S_OK )   
		APL_THROW( L"Ошибка при инициализации поиска в MorphoType по IDType" );

	while( MorphoTypesFind.Find() == S_OK )
		if( !FromToRecIndx.insert( std::make_pair(dbIDForm, m_pMorphoTypes->GetFoundIdx()) ).second )
			APL_THROW( _T("У IDType ") << IDType << " повторяются индексы IDForm" );

	if( FromToRecIndx.empty() )
		APL_THROW( _T("В таблице не найдена ни одна запись с IDType = ") << IDType );

	//Заменяем значения IDForm
	TIDForm CurIDForm = 0;

	for( TFromToRecIndx::iterator I = FromToRecIndx.begin(); I != FromToRecIndx.end(); ++I, ++CurIDForm )
	{
		APL_ASSERT( I->first >= CurIDForm );
	    
		if( I->first != CurIDForm )
			ReplaceIDForm( I->second, CurIDForm );
	}
}
///////////////////////////////////////////////////////////////////////////////

void CMorphoTypesCollection::ReplaceIDForm( UINT32 RecordID, TIDForm NewIDForm )
{
	//INT32 dbIDForm;
	INT32 dbIDType;
	INT32 dbIDEnding;
	INT32 dbPartOfSpeech;

	void *ReadRecMapMorphoTypesPrev[4] = { NULL, &dbIDType, &dbIDEnding, &dbPartOfSpeech };
	void *WriteRecMapMorphoTypesPrev[4] = { &NewIDForm, &dbIDType, &dbIDEnding, &dbPartOfSpeech };

	if( m_pMorphoTypes->ScanInit(0, ReadRecMapMorphoTypesPrev) != S_OK )
		APL_THROW( L"Ошибка при инициализации сканирования в таблице MorphoType" );

	if( m_pMorphoTypes->AddInit( WriteRecMapMorphoTypesPrev ) != S_OK )
		APL_THROW( L"Ошибка при инициализации добавления в таблицу MorphoType" );

	if( m_pMorphoTypes->ScanOne(RecordID) != S_OK )
		APL_THROW( L"Ошибка в целостности данных таблицы MorphoType" );

	if( m_pMorphoTypes->DeleteRecord(RecordID) != S_OK )
		APL_THROW( L"Ошибка в целостности данных таблицы MorphoType" );

	if( m_pMorphoTypes->Add() != S_OK )
		APL_THROW( L"Ошибка в целостности данных таблицы MorphoType" );
}
///////////////////////////////////////////////////////////////////////////////

HRESULT CMorphoTypesCollection::DeleteElements(std::list<SMorphoType>* p_l_MorphoTypes)
{
	SS_TRY
	{
		Open();
		APL_ASSERT(p_l_MorphoTypes != 0);
		APL_ASSERT( m_pMorphoTypes.IsOpen() && m_pMorphoTypeToMorphoInfo.IsOpen() );

		typedef std::set<TIDType> TIDTypesToRestore;

		HRESULT RetVal = S_OK;
		Detail::CIDTypeReferences TypeReferences( m_pMorphoTypes.Get(), 0, Detail::CIDTypeReferences::UTMorphoTypes );
		Detail::CIDTypeReferences::TItem *pCurItem;
		UINT32 CurRecordID;
		TIDTypesToRestore IDTypesToRestore;

		for( std::list<SMorphoType>::const_iterator I = p_l_MorphoTypes->begin(); I != p_l_MorphoTypes->end(); ++I )
		{
			pCurItem = &TypeReferences.Find(I->m_MorphoTypeInfo.m_IDType);

			//Проверяем есть ли IDForm и он не последний
			if( 
				pCurItem->IDFormCount() > 1 &&	
				(CurRecordID = pCurItem->GetIDFormRecordID(I->m_IDForm )) != Detail::CIDTypeReferences::TItem::UndefinedRecordID()	 
			)
			{
				//Удаляем запись
				if( m_pMorphoTypes->DeleteRecord(CurRecordID) != S_OK )
					SS_THROW(L"Ошибка удаления записи из MorphoTypes")	

				pCurItem->DeleteIDForm( I->m_IDForm	 );

				IDTypesToRestore.insert( I->m_MorphoTypeInfo.m_IDType ); 
			}
			else
				RetVal = E_INVALIDARG;
		}

		for( TIDTypesToRestore::iterator I = IDTypesToRestore.begin(); I != IDTypesToRestore.end(); ++I )
			RestoreCorrectIDFormOrder(*I);   

		return RetVal;
		Close();
		Open();
	}
	SS_CATCH(L"")
	return E_FAIL;
}

///////////////////////////////////////////////////////////////////////////////
// class CMorphoTypesCollectionAdapter
///////////////////////////////////////////////////////////////////////////////
void CMorphoTypesCollectionAdapter::SetDictionaryNumber(TDictNumber eDictionariesNumber)
{
   SS_TRY
   {
      CMorphoTypesCollection *pCurMorphoTypesCollection = m_MorphoTypesCollections.Get( eDictionariesNumber );

      if( pCurMorphoTypesCollection == 0 )
         SS_THROW( L"Ошибка при поиске зарегистрированного DictionaryNumber" );

      m_pCurMorphoTypesCollection = pCurMorphoTypesCollection;
   }
   SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

void CMorphoTypesCollectionAdapter::DeleteType( const SS::Dictionary::Types::TIDType &IDType )
{
   APL_ASSERT_PTR( m_pCurMorphoTypesCollection );

   SS_TRY
   {
      m_pCurMorphoTypesCollection->DeleteType( IDType );
   }
   SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

void CMorphoTypesCollectionAdapter::GetElements(
                 const SS::Dictionary::Types::TIDType &IDType,
                 std::list<SS::Dictionary::Types::SMorphoType>* p_l_MorphoTypes
                 )
{
   APL_ASSERT_PTR( m_pCurMorphoTypesCollection );
   APL_ASSERT_PTR( p_l_MorphoTypes );

   SS_TRY
   {
      m_pCurMorphoTypesCollection->GetElements( IDType, p_l_MorphoTypes );
   }
   SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

HRESULT CMorphoTypesCollectionAdapter::AddElements(std::list<SS::Dictionary::Types::SMorphoType>* p_l_MorphoTypes)
{
   APL_ASSERT_PTR( m_pCurMorphoTypesCollection );
   APL_ASSERT_PTR( p_l_MorphoTypes );

   SS_TRY
   {
      return m_pCurMorphoTypesCollection->AddElements( p_l_MorphoTypes );
   }
   SS_CATCH(L"")
   
   return E_FAIL;
}
///////////////////////////////////////////////////////////////////////////////

HRESULT CMorphoTypesCollectionAdapter::CreateCollection(std::list<SS::Dictionary::Types::SMorphoType>* p_l_MorphoTypes)
{
   APL_ASSERT_PTR( m_pCurMorphoTypesCollection );
   APL_ASSERT_PTR( p_l_MorphoTypes );

   SS_TRY
   {
      return m_pCurMorphoTypesCollection->CreateCollection( p_l_MorphoTypes );
   }
   SS_CATCH(L"")

   return E_FAIL;
}
///////////////////////////////////////////////////////////////////////////////

HRESULT CMorphoTypesCollectionAdapter::DeleteElements(std::list<SS::Dictionary::Types::SMorphoType>* p_l_MorphoTypes)
{
   APL_ASSERT_PTR( m_pCurMorphoTypesCollection );
   APL_ASSERT_PTR( p_l_MorphoTypes );

   SS_TRY
   {
      return m_pCurMorphoTypesCollection->DeleteElements( p_l_MorphoTypes );
   }
   SS_CATCH(L"")
      
   return E_FAIL;
}

} //namespace Edit
} //namespace Dictionary
} //namespace SS
