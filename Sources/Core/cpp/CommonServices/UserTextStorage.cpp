#include "stdafx.h"

namespace SS {
namespace CommonServices {
namespace UserTextStorage {

///////////////////////////////////////////////////////////////////////////////
// class TUsersManager
///////////////////////////////////////////////////////////////////////////////   
void TUsersManager::Init( const std::wstring &IniFilePath, const std::wstring &DataCatalogPath )
{
   SS_TRY
   {
      m_UserTextStorage.Open( GetLoadManager(), IniFilePath, DataCatalogPath );
   }
   SS_CATCH( L"" )
}
///////////////////////////////////////////////////////////////////////////////

void TUsersManager::Init( const std::wstring &DataCatalogPath /* = L""*/ )
{
   //if( GetLoadManager() == pLM )
   //{
   //   IUsersManager::SetLoadManager(pLM);
   //   return;
   //}

   //IUsersManager::SetLoadManager(pLM);

   SS::Interface::Core::CommonServices::IWorkRegister* pWorkRegister =
      ((SS::Interface::Core::CommonServices::IWorkRegister*) 
      GetLoadManager()->GetInterface(L"./core/LoadManager.dll",CLSID_WorkRegister,IID_WorkRegister));

   if( pWorkRegister == 0 )
      APL_THROW( _T("Ошибка при получении WorkRegister") );

   std::wstring m_wsDataBasePath = pWorkRegister->GetParameter(L"PathDataBase");

   Init( m_wsDataBasePath + _T("\\UserTextStorageData.ini"), DataCatalogPath.empty() ? m_wsDataBasePath + _T("\\Users") : DataCatalogPath  );
}
///////////////////////////////////////////////////////////////////////////////

std::wstring TUsersManager::GetDataCatalogPath() const
{
   SS_TRY
   {
      return m_UserTextStorage.GetDataCatalogPath();
   }
   SS_CATCH( L"" )

   return std::wstring();
}
///////////////////////////////////////////////////////////////////////////////

void TUsersManager::AddUser( TPrivateUserInfo &UserInfo )
{
   SS_TRY
   {
      m_UserTextStorage.AddUser( UserInfo );
   }
   SS_CATCH( L"" )
}
///////////////////////////////////////////////////////////////////////////////

void TUsersManager::DeleteUser( TUserID UserID )
{
   SS_TRY
   {
      m_UserTextStorage.DeleteUser( UserID );
   }
   SS_CATCH( L"" )
}
///////////////////////////////////////////////////////////////////////////////

void TUsersManager::ChangeUserInfo( const TPrivateUserInfo &UserInfo )
{
   SS_TRY
   {
      m_UserTextStorage.ChangeUserInfo( UserInfo );
   }
   SS_CATCH( L"" )
}
///////////////////////////////////////////////////////////////////////////////

void TUsersManager::GetUsersIDList(TUserIDList& UserIDList) const
{
   SS_TRY
   {
      m_UserTextStorage.GetUsersIDList( UserIDList );
   }
   SS_CATCH( L"" )
}
///////////////////////////////////////////////////////////////////////////////

void TUsersManager::GetPrivateUsersInfo( TUserID UserID, const std::wstring &Password, TPrivateUserInfo &UserInfo ) const
{
   SS_TRY
   {
      m_UserTextStorage.GetPrivateUsersInfo( UserID, Password, UserInfo );
   }
   SS_CATCH( L"" )
}
///////////////////////////////////////////////////////////////////////////////

void TUsersManager::GetPublicUsersInfo( TUserID UserID, TPublicUserInfo &UserInfo ) const
{
   SS_TRY
   {
      m_UserTextStorage.GetPublicUsersInfo( UserID, UserInfo );

   }
   SS_CATCH( L"" )
}
///////////////////////////////////////////////////////////////////////////////

bool TUsersManager::FindUserIDByName( const std::wstring &Name, TUserIDList &UserIDList ) const
{
   SS_TRY
   {
      return m_UserTextStorage.FindUserIDByName( Name, UserIDList );
   }
   SS_CATCH( L"" )

   return false;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// class TText
///////////////////////////////////////////////////////////////////////////////
void TText::OpenDir( const std::wstring &DirName )
{
   SS_TRY
   {
      m_Text.OpenDir( DirName );
   }
   SS_CATCH( L"" )
}
///////////////////////////////////////////////////////////////////////////////

bool TText::GetTextPart( const std::wstring &PartName, std::wstring &Content ) const
{
   SS_TRY
   {
      return m_Text.GetFileText( PartName, Content );
   }
   SS_CATCH( L"" )

   return false;
}
///////////////////////////////////////////////////////////////////////////////

void TText::SetTextPart( const std::wstring &PartName,  const std::wstring &Content, EFileFormat FileFormat/* = FFUNICODE*/ )
{
   SS_TRY
   {
      switch(FileFormat)
      {
      case FFANSI:
         m_Text.SetFileTextAnsi( PartName, Content );
         break;

      case FFUNICODE:
         m_Text.SetFileTextUnicode( PartName, Content );
         break;

      default:
         APL_THROW( L"FileFormat имеет неправильное значение" );
      }
      
   }
   SS_CATCH( L"" )
}
///////////////////////////////////////////////////////////////////////////////

bool TText::DeleteTextPart( const std::wstring &PartName )
{
   SS_TRY
   {
      return m_Text.DeleteFileText( PartName );
   }
   SS_CATCH( L"" )

   return false;
}
///////////////////////////////////////////////////////////////////////////////

void TText::GetTextPartNames( TStringsList &PartNameList )
{
   SS_TRY
   {
      m_Text.GetFileNames( PartNameList );
   }
   SS_CATCH( L"" )
}

///////////////////////////////////////////////////////////////////////////////
// class TTextsManager
///////////////////////////////////////////////////////////////////////////////

void TTextsManager::Open( TUserID UserID, const std::wstring &Password, IUsersManager *pUsersManager )
{
   APL_ASSERT_PTR(pUsersManager);

   SS_TRY
   {
      TPrivateUserInfo PUI;
    
      pUsersManager->GetPrivateUsersInfo( UserID, Password, PUI );
      
      m_TextsManager.OpenDir( pUsersManager->GetDataCatalogPath() + PUI.GetUserCatalog() );
   }
   SS_CATCH( L"" )
}
///////////////////////////////////////////////////////////////////////////////

bool TTextsManager::AddText( const std::wstring &TextName, IText *pText )
{
   SS_TRY
   {
      return m_TextsManager.AddSubDir( TextName ) && GetText( TextName, pText );
   }
   SS_CATCH( L"" )

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool TTextsManager::GetText( const std::wstring &TextName, IText *pText ) const
{
   APL_ASSERT_PTR( pText );

   SS_TRY
   {
      if( !m_TextsManager.IsExistSubDir(TextName) )
         return false;

      TText *pCurText = APL_DYNAMIC_CAST( TText *, pText );

      pCurText->OpenDir( m_TextsManager.GetCurDir() + TextName );
      
      return true;
   }
   SS_CATCH( L"" )

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool TTextsManager::DeleteText( const std::wstring &TextName )
{
   SS_TRY
   {
      return m_TextsManager.DeleteSubDir( TextName );
   }
   SS_CATCH( L"" )

   return false;
}
///////////////////////////////////////////////////////////////////////////////

void TTextsManager::GetUserTextNames( TStringsList &TextNameList ) const
{
   SS_TRY
   {
      m_TextsManager.GetSubDirNames( TextNameList );
   }
   SS_CATCH( L"" )
}
///////////////////////////////////////////////////////////////////////////////

} //namespace UserTextStorage
} //namespace CommonServices
} //namespace SS
