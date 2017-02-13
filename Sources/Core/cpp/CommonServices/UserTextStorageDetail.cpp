#include "stdafx.h"

namespace SS {
namespace CommonServices {
namespace UserTextStorage {
namespace Detail {


const TCHAR g_DatabaseName[] = _T("UserTextStorage");
const TCHAR g_UsersTableName[] = _T("Users");

///////////////////////////////////////////////////////////////////////////////
// class TUserTextStorage
///////////////////////////////////////////////////////////////////////////////
void TUserTextStorage::Close()
{
   m_pTable.Close();

   if( m_pDataBase != 0 )
   {  
      CDBMSCreateManager::Create()->CloseDataBase( m_pDataBase );
      m_pDataBase = 0;
   }

   m_DataCatalogPath.clear();
}
///////////////////////////////////////////////////////////////////////////////
   
void TUserTextStorage::Open( ILoadManager *pLoadManager, const TString &IniFilePath, const TString &DataCatalogPath )
{
   APL_ASSERT( !DataCatalogPath.empty() );
   
   Close();
   m_DataCatalogPath = DataCatalogPath + _T('\\');
   m_pDataBase = CDBMSCreateManager::Create()->OpenDataBase( pLoadManager, IniFilePath, m_DataCatalogPath, g_DatabaseName );
   APL_ASSERT_PTR( m_pDataBase );
   m_pTable.Open( m_pDataBase, g_UsersTableName );
}
///////////////////////////////////////////////////////////////////////////////

void TUserTextStorage::AddUser( TPrivateUserInfo &oUserName )
{
   APL_ASSERT_PTR( m_pDataBase );
   APL_ASSERT( !m_DataCatalogPath.empty() );
   APL_ASSERT( m_pTable.IsOpen() );

   TRecord Rec;
   {
      TMutex::Lock Guard(m_Mutex); 

      Rec.UserID = m_pTable->GetUserValue(0) + 1;
      APL_CHECK_S_OK( m_pTable->SetUserValue(0, Rec.UserID) );
   }
   
   Rec.Name = oUserName.GetUserName();
   Rec.Description = oUserName.GetDescription();
   MakeUserCatalogName( Rec.Name, Rec.UserID, Rec.DirectoryName );

   if( !CreateDirectory((m_DataCatalogPath + Rec.DirectoryName).c_str(), NULL) )
      APL_THROW( _T("Ошибка при создании каталога ") << m_DataCatalogPath + Rec.DirectoryName  << _T(": ") << GetDWErrorInfo(GetLastError()) );

   if( m_pTable->AddInit(Rec.rgRecMap) != S_OK )
      APL_THROW(_T("Ошибка при инициализации добавсления в ") << g_DatabaseName << _T("->") << g_UsersTableName);

   if( m_pTable->Add() != S_OK )
      APL_THROW(_T("Ошибка при добавлении в ") << g_DatabaseName << _T("->") << g_UsersTableName);

   oUserName.SetUserID( Rec.UserID );
   oUserName.SetUserCatalog( Rec.DirectoryName );
}
///////////////////////////////////////////////////////////////////////////////

void TUserTextStorage::DeleteUser( TUserID UserID )
{
   APL_ASSERT_PTR( m_pDataBase );
   APL_ASSERT( !m_DataCatalogPath.empty() );
   APL_ASSERT( m_pTable.IsOpen() );

   TRecord Rec;   
   CFindHelper FH( m_pTable );
   
   if( FH.Init(0U, &UserID, Rec.rgRecMap) != S_OK )
      APL_THROW(_T("Ошибка при инициализации поиска в ") << g_DatabaseName << _T("->") << g_UsersTableName);

   if( FH.Find() != S_OK )
      APL_THROW(_T("Пользователь не найден. UserID = ") << UserID );
    
   if( m_pTable->DeleteRecord( FH.GetFoundIdx() ) != S_OK )
      APL_THROW(_T("Ошибка при удалении в ") << g_DatabaseName << _T("->") << g_UsersTableName);

   DWORD Rez = DeleteDirectory( m_DataCatalogPath + Rec.DirectoryName );

   if( Rez != ERROR_SUCCESS )
      APL_THROW( _T("Ошибка при удалении каталога ") << m_DataCatalogPath + Rec.DirectoryName  << _T(": ") << GetDWErrorInfo(Rez) );
}
///////////////////////////////////////////////////////////////////////////////

void TUserTextStorage::ChangeUserInfo( const TPrivateUserInfo &oUserName )
{
   APL_ASSERT_PTR( m_pDataBase );
   APL_ASSERT( !m_DataCatalogPath.empty() );
   APL_ASSERT( m_pTable.IsOpen() );

   TRecord Rec;
   CFindHelper FH( m_pTable );

   Rec.UserID = oUserName.GetUserID();

   if( FH.Init(0U, &Rec.UserID, Rec.rgRecMap) != S_OK )
      APL_THROW(_T("Ошибка при инициализации поиска в ") << g_DatabaseName << _T("->") << g_UsersTableName);

   if( FH.FindOne() != S_OK )
      APL_THROW(_T("Пользователь не найден. UserID = ") << Rec.UserID );

   if( m_pTable->DeleteRecord( FH.GetFoundIdx() ) != S_OK )
      APL_THROW(_T("Ошибка при удалении в ") << g_DatabaseName << _T("->") << g_UsersTableName);

   Rec.Name = oUserName.GetUserName();
   Rec.Description = oUserName.GetDescription();

   if( m_pTable->AddInit(Rec.rgRecMap) != S_OK )
      APL_THROW(_T("Ошибка при инициализации добавсления в ") << g_DatabaseName << _T("->") << g_UsersTableName);

   if( m_pTable->Add() != S_OK )
      APL_THROW(_T("Ошибка при добавлении в ") << g_DatabaseName << _T("->") << g_UsersTableName);
}
///////////////////////////////////////////////////////////////////////////////

void TUserTextStorage::GetUsersIDList( TUserIDList& oUserIDList ) const
{
   APL_ASSERT_PTR( m_pDataBase );
   APL_ASSERT( !m_DataCatalogPath.empty() );
   APL_ASSERT( m_pTable.IsOpen() );

   INT32 UserID = 0;
   void *rgRecMap[10] = { &UserID };

   oUserIDList.clear();
  
   if( m_pTable->ScanInit( 0, rgRecMap ) != S_OK )
      APL_THROW(_T("Ошибка при инициализации сканирования в ") << g_DatabaseName << _T("->") << g_UsersTableName);

   while( m_pTable->Scan() == S_OK )
      oUserIDList.push_back(UserID);
}
///////////////////////////////////////////////////////////////////////////////

void TUserTextStorage::GetPrivateUsersInfo( TUserID UserID, const std::wstring &Password, TPrivateUserInfo &oUserInfo ) const
{
   APL_ASSERT_PTR( m_pDataBase );
   APL_ASSERT( !m_DataCatalogPath.empty() );
   APL_ASSERT( m_pTable.IsOpen() );

   TRecord Rec;   
   CFindHelper FH( m_pTable );

   if( FH.Init(0U, &UserID, Rec.rgRecMap) != S_OK )
      APL_THROW(_T("Ошибка при инициализации поиска в ") << g_DatabaseName << _T("->") << g_UsersTableName);

   if( FH.FindOne() != S_OK )
      APL_THROW(_T("Пользователь не найден. UserID = ") << UserID );

   oUserInfo.SetUserID( Rec.UserID );
   oUserInfo.SetUserName( Rec.Name );
   oUserInfo.SetDescription( Rec.Description );
   oUserInfo.SetUserCatalog( Rec.DirectoryName );
}
///////////////////////////////////////////////////////////////////////////////

void TUserTextStorage::GetPublicUsersInfo( TUserID UserID, TPublicUserInfo &oUserInfo ) const
{
   APL_ASSERT_PTR( m_pDataBase );
   APL_ASSERT( !m_DataCatalogPath.empty() );
   APL_ASSERT( m_pTable.IsOpen() );

   TRecord Rec;   
   CFindHelper FH( m_pTable );

   if( FH.Init(0U, &UserID, Rec.rgRecMap) != S_OK )
      APL_THROW(_T("Ошибка при инициализации поиска в ") << g_DatabaseName << _T("->") << g_UsersTableName);

   if( FH.FindOne() != S_OK )
      APL_THROW(_T("Пользователь не найден. UserID = ") << UserID );

   oUserInfo.SetUserID( Rec.UserID );
   oUserInfo.SetUserName( Rec.Name );
}
///////////////////////////////////////////////////////////////////////////////

bool TUserTextStorage::FindUserIDByName( const std::wstring &Name, TUserIDList &oUserIDList ) const
{
   INT32 UserID = 0;
   void *rgRecMap[10] = { &UserID };
   CFindHelper FH( m_pTable );

   oUserIDList.clear();

   if( FH.Init(1U, (void *)&Name, rgRecMap) != S_OK )
      APL_THROW(_T("Ошибка при инициализации поиска в ") << g_DatabaseName << _T("->") << g_UsersTableName);

   while( FH.Find() == S_OK )
      oUserIDList.push_back(UserID);

   return !oUserIDList.empty();
}

///////////////////////////////////////////////////////////////////////////////
// class TText
///////////////////////////////////////////////////////////////////////////////
bool TText::GetFileText( const std::wstring &FileName,  std::wstring &Content ) const
{
   APL_ASSERT( !m_DirName.empty() );
   
   std::wstring FilePath( m_DirName + FileName );
   
   Content.clear();
   
   if( !PathFileExists(FilePath.c_str()) || PathIsDirectory(FilePath.c_str()) )
      return false;

   DWORD Rez = ReadUnicodeAnsiFile(FilePath, Content);

   if( Rez != ERROR_SUCCESS )
      APL_THROW( _T("Ошибка при чтении файла: ") << FilePath << _T(": ") << GetDWErrorInfo(Rez) );

   return true;
}
///////////////////////////////////////////////////////////////////////////////

void TText::SetFileTextUnicode( const std::wstring &FileName,  const std::wstring &Content )
{
   APL_ASSERT( !m_DirName.empty() );

   std::wstring FilePath( m_DirName + FileName );

   DWORD Rez = WriteUnicodeFile(FilePath, Content);

   if( Rez != ERROR_SUCCESS )
      APL_THROW( _T("Ошибка при записи файла: ") << FilePath << _T(": ") << GetDWErrorInfo(Rez) );
}
///////////////////////////////////////////////////////////////////////////////

void TText::SetFileTextAnsi( const std::wstring &FileName,  const std::wstring &Content )
{
   APL_ASSERT( !m_DirName.empty() );

   std::wstring FilePath( m_DirName + FileName );

   DWORD Rez = WriteAnsiFile(FilePath, Content);

   if( Rez != ERROR_SUCCESS )
      APL_THROW( _T("Ошибка при записи файла: ") << FilePath << _T(": ") << GetDWErrorInfo(Rez) );
}
///////////////////////////////////////////////////////////////////////////////

bool TText::DeleteFileText( const std::wstring &FileName )
{
   APL_ASSERT( !m_DirName.empty() );

   std::wstring FilePath( m_DirName + FileName );

   if( !PathFileExists(FilePath.c_str()) || PathIsDirectory(FilePath.c_str()) )
      return false;
   
   if( DeleteFile(FilePath.c_str()) == FALSE )
      APL_THROW( _T("Ошибка при удалении файла: ") << FilePath << _T(": ") << GetDWErrorInfo(GetLastError()) );
      
   return true;
}
///////////////////////////////////////////////////////////////////////////////

void TText::GetFileNames( TStringsList &FileNameList ) const
{
   APL_ASSERT( !m_DirName.empty() );

   WIN32_FIND_DATA FindData;

   FileNameList.clear();

   TAutoFindClose AFC( FindFirstFile( (m_DirName + _T("\\*")).c_str(), &FindData ) );

   if( AFC.hFind == INVALID_HANDLE_VALUE )
      APL_THROW( _T("Ошибка при просмотре каталога: ") << m_DirName << _T(": ") << GetDWErrorInfo(GetLastError()) );

   do 
   {
      if( ~FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) //Не каталог (т.е. файл)
      {
         FileNameList.push_back( FindData.cFileName );
      }

   } while( FindNextFile(AFC.hFind, &FindData) != FALSE );
}

///////////////////////////////////////////////////////////////////////////////
// class TTextsManager
///////////////////////////////////////////////////////////////////////////////
void TTextsManager::OpenDir( const std::wstring &DirName )
{
   APL_ASSERT(!DirName.empty()); 
   m_DirName = DirName; 
   m_DirName += _T('\\'); 

   if( !PathIsDirectory(DirName.c_str()) )
   {
      DWORD ErrorCode = CreateFullPath(DirName);

      if( ErrorCode != ERROR_SUCCESS )
         APL_THROW( _T("Ошибка при создании каталога ") << DirName  << _T(": ") << GetDWErrorInfo(ErrorCode) );
   }
}
///////////////////////////////////////////////////////////////////////////////

bool TTextsManager::AddSubDir( const std::wstring &DirName )
{
   APL_ASSERT( !m_DirName.empty() );
   APL_ASSERT( !DirName.empty() );

   std::wstring FilePath( m_DirName + DirName );

   if( PathIsDirectory(FilePath.c_str()) )
      return false;

   DWORD ErrorCode = CreateFullPath(FilePath);

   if( ErrorCode != ERROR_SUCCESS )
      APL_THROW( _T("Ошибка при создании каталога ") << FilePath  << _T(": ") << GetDWErrorInfo(ErrorCode) );

   return true;
}
///////////////////////////////////////////////////////////////////////////////

bool TTextsManager::IsExistSubDir( const std::wstring &DirName ) const
{
   APL_ASSERT( !m_DirName.empty() );
   APL_ASSERT( !DirName.empty() );

   std::wstring FilePath( m_DirName + DirName );

   return PathIsDirectory(FilePath.c_str()) != FALSE;
}
///////////////////////////////////////////////////////////////////////////////

bool TTextsManager::DeleteSubDir( const std::wstring &DirName )
{
   APL_ASSERT( !m_DirName.empty() );
   APL_ASSERT( !DirName.empty() );

   std::wstring FilePath( m_DirName + DirName );

   if( !PathIsDirectory(FilePath.c_str()) )
      return false;

   DWORD Rez = DeleteDirectory( FilePath );

   if( Rez != ERROR_SUCCESS )
      APL_THROW( _T("Ошибка при удалении каталога ") << FilePath  << _T(": ") << GetDWErrorInfo(Rez) );

   return true;
}
///////////////////////////////////////////////////////////////////////////////

void TTextsManager::GetSubDirNames( TStringsList &DirNameList ) const
{
   APL_ASSERT( !m_DirName.empty() );

   WIN32_FIND_DATA FindData;

   DirNameList.clear();

   TAutoFindClose AFC( FindFirstFile( (m_DirName + _T("\\*")).c_str(), &FindData ) );

   if( AFC.hFind == INVALID_HANDLE_VALUE )
      APL_THROW( _T("Ошибка при просмотре каталога: ") << m_DirName << _T(": ") << GetDWErrorInfo(GetLastError()) );

   do 
   {
      if( 
         FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && 
         _tcscmp(FindData.cFileName, _T("..")) != 0 && 
         _tcscmp(FindData.cFileName, _T(".")) != 0
      ) 
      {
         DirNameList.push_back( FindData.cFileName );
      }

   } while( FindNextFile(AFC.hFind, &FindData) != FALSE );


}
///////////////////////////////////////////////////////////////////////////////

} //namespace Detail 
} //namespace UserTextStorage
} //namespace CommonServices
} //namespace SS
