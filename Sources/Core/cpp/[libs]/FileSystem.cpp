#include "stdafx.h"
#include "FileSystem.h"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi") //подлинкуем shlwapi.lib для PathFileExists

namespace SS { namespace Lib {

const BYTE g_UnicodeFilePrefix[] = { 0xFF, 0xFE };

///////////////////////////////////////////////////////////////////////////////
// class CDirectoryFileList
///////////////////////////////////////////////////////////////////////////////
bool CDirectoryFileList::Next()
{
   bool IsFind;
   
   //Мы будем пыться найти первый подходящий файл в текущем каталоге (последнем в m_FindHandles) и
   //во всех его подкаталогах
   for(;;)
   {
      if( m_FindHandles.empty() )
         return false;

      if( m_FindHandles.back().Handle == INVALID_HANDLE_VALUE )
      {
         //Обрезаем строку с имененем текущего каталога
         m_CurDir.erase( m_CurDir.size() - m_FindHandles.back().DirNameSize );
         m_FindHandles.pop_back();

         continue;
      }

      //m_FindHandles.back() - Содержит какой то файл

      //Если это каталог попубуем залесть внутрь
      if( m_FindHandles.back().FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
      {
         if( 
            m_RrecurSearch && 
            _tcscmp(m_FindHandles.back().FindData.cFileName, _T("..")) != 0 && 
            _tcscmp(m_FindHandles.back().FindData.cFileName, _T(".")) != 0
         )
         {
            SDirItem DirItem;

            DirItem.DirNameSize = m_CurDir.size();

            m_CurDir += m_FindHandles.back().FindData.cFileName;
            m_CurDir += _T('\\');

            DirItem.DirNameSize = m_CurDir.size() - DirItem.DirNameSize;

            FindFirst( DirItem ); 

            //Находим в предыдущем каталоге следующий файл 
            FindNextOrClose( m_FindHandles.back() );

            //Добавляем уровень каталога 
            m_FindHandles.push_back(DirItem);
         }
         else
         {
            FindNextOrClose( m_FindHandles.back() );
         }

         continue;
      }

      //Мы успешно нашли какой-то файл, проверим его расширение
      IsFind = false;

      if( !m_Extensions.empty() )
      {
         CharT *BeginPtr = m_FindHandles.back().FindData.cFileName;
         CharT *EndPtr = BeginPtr + _tcslen( BeginPtr );
         CharT *CurPtr = EndPtr - 1;
         
         while( CurPtr != BeginPtr - 1 && *CurPtr != _T('.') ) --CurPtr;

         StringT Ext( CurPtr + 1, EndPtr );

         if( CurPtr != BeginPtr - 1 )
         {
            Ext.assign( CurPtr + 1, EndPtr );   
         }
         
         IsFind = std::binary_search( m_Extensions.begin(), m_Extensions.end(), Ext );
      }
      else
      {
         IsFind = true;
      }
     
      if( IsFind )
      {
         m_FindData = m_FindHandles.back().FindData;
         m_CurFile = m_FindData.cFileName;
      }

      FindNextOrClose( m_FindHandles.back() );

      if( IsFind )
         return true;
   }

   return false;
}
///////////////////////////////////////////////////////////////////////////////

void CDirectoryFileList::Clear()
{
   for( TFindHandles::iterator I = m_FindHandles.begin(); I != m_FindHandles.end(); ++I )
   {
      if(I->Handle != INVALID_HANDLE_VALUE )
         APL_CHECK( FindClose(I->Handle) );
   }

   m_FindHandles.clear();
   m_CurFile.clear();
   m_CurDir.clear();   
   m_Mask.clear();
   m_Extensions.clear();
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// class CUnicodeFileReader
///////////////////////////////////////////////////////////////////////////////

void CUnicodeFileReader::CorrectBuffer()
{ 
   TChar *pCurRead = m_pBuffBegin, *pCurWrite = m_pBuffBegin;

   for( ; pCurRead  != m_pBuffEnd; ++pCurRead )
   {
      if( *(pCurRead) != _T('\r') )
      {
         *pCurWrite = *pCurRead;
         ++pCurWrite;
      }  
   }

   m_pBuffEnd = pCurWrite;
}
///////////////////////////////////////////////////////////////////////////////

void CUnicodeFileReader::FillBuffer()
{
   DWORD BytesRead;

   if( m_IsEof ) //Достигли конца файла и обработали все данные
   {
      m_pBuffBegin = m_pBuffEnd = 0;
      return;
   }

   m_pBuffBegin =  m_Buff;

   if( m_IsUnicode )
   {
      BOOL Result = ::ReadFile( m_hFile, m_Buff, BuffSize * sizeof(*m_Buff), &BytesRead, NULL );

      if( Result == FALSE )
         APL_THROW( _T("Ошибка при чтении файла") );

      if( BuffSize * sizeof(*m_Buff) != BytesRead )
         m_IsEof = true;
      
      m_pBuffEnd = m_pBuffBegin + BytesRead / sizeof(*m_Buff);
   }
   else
   {
      //Необходимо выполнить преобразование
      BOOL Result = ::ReadFile( m_hFile, &m_AuxBuff[0], BuffSize * sizeof(m_AuxBuff[0]), &BytesRead, NULL );

      if( Result == FALSE )
         APL_THROW( _T("Ошибка при чтении файла") );

      if( BuffSize * sizeof(m_AuxBuff[0]) != BytesRead )
         m_IsEof = true;

      MultiByteToWideChar(CP_ACP, 0, &m_AuxBuff[0], BytesRead, m_Buff, BytesRead);

      m_pBuffEnd = m_pBuffBegin + BytesRead / sizeof(m_AuxBuff[0]);
   }

   CorrectBuffer();

   APL_ASSERT( BytesRead == 0 || m_pBuffBegin != m_pBuffEnd );
}
///////////////////////////////////////////////////////////////////////////////

CUnicodeFileReader::EOpenResult CUnicodeFileReader::Open( LPCTSTR szFileName )
{
   APL_ASSERT_PTR( szFileName );
   
   Close();
 
   m_hFile = ::CreateFile( szFileName,                // open MYFILE.TXT 
                           GENERIC_READ,              // open for reading 
                           FILE_SHARE_READ,           // share for reading 
                           NULL,                      // no security 
                           OPEN_EXISTING,             // existing file only 
                           FILE_ATTRIBUTE_NORMAL,     // normal file 
                           NULL);                     // no attr. template   


   if( m_hFile == INVALID_HANDLE_VALUE )
      return EORCantOpen;

   //Читаем первые несколько байт для определения типа файла
   TByte Prefix[ APL_ARRSIZE(g_UnicodeFilePrefix) ];
   DWORD BytesRead;
   BOOL Result = ::ReadFile( m_hFile, Prefix, APL_ARRSIZE(g_UnicodeFilePrefix), &BytesRead, NULL );

   if( 
      Result == FALSE || 
      BytesRead != APL_ARRSIZE(g_UnicodeFilePrefix) ||
      std::memcmp( Prefix, g_UnicodeFilePrefix, APL_ARRSIZE(g_UnicodeFilePrefix)) != 0 
   )
   {
      //Файл не UNICODE
      m_IsUnicode = false;

      //Возвращаем курсор снова на начало файла
      APL_CHECK(::SetFilePointer( m_hFile, 0, NULL, FILE_BEGIN ) != INVALID_SET_FILE_POINTER );

      //Создаём дополнительный буфер
      m_AuxBuff.resize(BuffSize);
   }
   else
   {
      //Файл UNICODE
      m_IsUnicode = true;
   }

   m_IsEof = false;

   FillBuffer();

   return EOROk;
}

///////////////////////////////////////////////////////////////////////////////

DWORD CopyDirectory( const std::basic_string<TCHAR> &From, const std::basic_string<TCHAR> &To )
{ 
   if( PathIsDirectory(From.c_str()) == FALSE )
      return GetLastError();
   
   if( PathIsDirectory(To.c_str()) == FALSE && CreateDirectory(To.c_str(), NULL) == FALSE )
      return GetLastError();
      
   WIN32_FIND_DATA FindData;
   TAutoFindClose AFC( FindFirstFile( (From + _T("\\*")).c_str(), &FindData ) );

   if( AFC.hFind == INVALID_HANDLE_VALUE )
      return GetLastError();
   
   do 
   {
      //Если это каталог то вызываем рекурсивно
      if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
      {
         if( 
            _tcscmp(FindData.cFileName, _T("..")) != 0 && 
            _tcscmp(FindData.cFileName, _T(".")) != 0
         )
         {
            //Вызываем рекурсивно
            DWORD Rez = CopyDirectory( From + _T("\\") + FindData.cFileName, To + _T("\\") + FindData.cFileName );
            
            if( Rez != ERROR_SUCCESS )
               return Rez;
         }
      }
      else
      {
         //Это файл копируем его
         if( CopyFile((From + _T("\\") + FindData.cFileName).c_str(), (To + _T("\\") + FindData.cFileName).c_str(), FALSE) == FALSE )
            return GetLastError();
      }

   } while( FindNextFile(AFC.hFind, &FindData) != FALSE );

   return ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////

DWORD DeleteDirectory( const std::basic_string<TCHAR> &Dir )
{
   if( PathFileExists(Dir.c_str()) == FALSE )
      return GetLastError();

   WIN32_FIND_DATA FindData;

   {
      TAutoFindClose AFC( FindFirstFile( (Dir + _T("\\*")).c_str(), &FindData ) );

      if( AFC.hFind == INVALID_HANDLE_VALUE )
         return GetLastError();

      //Удаляем вложенные файлы и каталоги

      do 
      {
         //Если это каталог то вызываем рекурсивно
         if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
         {
            if( 
               _tcscmp(FindData.cFileName, _T("..")) != 0 && 
               _tcscmp(FindData.cFileName, _T(".")) != 0
               )
            {
               //Вызываем рекурсивно
               DWORD Rez = DeleteDirectory( Dir + _T("\\") + FindData.cFileName );

               if( Rez != ERROR_SUCCESS )
                  return Rez;
            }
         }
         else
         {
            //Это файл, удаляем его
            if( DeleteFile((Dir + _T("\\") + FindData.cFileName).c_str()) == FALSE )
               return GetLastError();
         }

      } while( FindNextFile(AFC.hFind, &FindData) != FALSE );
   }

   APL_ASSERT( PathIsDirectoryEmpty(Dir.c_str()) == TRUE );
      
   if( RemoveDirectory( Dir.c_str() ) == FALSE )
      return GetLastError();

   return ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////

DWORD CreateFullPath( const std::basic_string<TCHAR> &Dir )
{
   std::vector<TCHAR> CurPath( Dir.size() + 1 ); // "+ 1" место под '\0'
   TCHAR *pBegin = &CurPath[0], *pCur = pBegin;
   TCHAR CharTmp;

   std::copy( Dir.begin(), Dir.end(), CurPath.begin() );
   CurPath.back() = _T('\0'); //Не обязательно т.к. конструктор вектора уже заполнил его нулями

   do
   {
      pCur = PathFindNextComponent( pCur );
      APL_ASSERT_PTR(pCur);
      
      CharTmp = *pCur;
      *pCur = _T('\0');

      //Если нет такого каталога нет, попробуем создать
      if( !PathIsDirectory(pBegin) && !CreateDirectory(pBegin, NULL) )
            return GetLastError();
      
      *pCur = CharTmp;
   }
   while( *pCur != _T('\0') );

   return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// class CWordReader
///////////////////////////////////////////////////////////////////////////////
bool CFileWordReader::Get( std::string &Str )
{
   std::string::iterator BeginWord;

   for(;;)
   {
      while( m_Cur == m_Buf.end() )
      {
         if( !getline(m_flIn, m_Buf) )
            return false;

         m_Cur = m_Buf.begin();
      }

      while( m_Cur != m_Buf.end() && !ValidChar(*m_Cur) ) ++m_Cur;

      if( m_Cur != m_Buf.end() )
      {
         BeginWord = m_Cur;

         while( m_Cur != m_Buf.end() && ValidChar(*m_Cur) ) ++m_Cur;

         Str.assign( BeginWord, m_Cur );

         return true;
      }
   }

   return false;
}
///////////////////////////////////////////////////////////////////////////////

DWORD ReadUnicodeFile( const std::basic_string<TCHAR> &FileName, std::basic_string<WCHAR> &Str )
{  
   Str.clear();

   TAutoFileClose AFC = ::CreateFile(  FileName.c_str(),          // open MYFILE.TXT 
                                       GENERIC_READ,              // open for reading 
                                       FILE_SHARE_READ,           // share for reading 
                                       NULL,                      // no security 
                                       OPEN_EXISTING,             // existing file only 
                                       FILE_ATTRIBUTE_NORMAL,     // normal file 
                                       NULL);                     // no attr. template   

   if( AFC.hFile == INVALID_HANDLE_VALUE )
      return GetLastError();

   //Читаем первые несколько байт для определения типа файла
   BYTE Prefix[ APL_ARRSIZE(g_UnicodeFilePrefix) ];
   DWORD BytesRead;
   BOOL Result = ::ReadFile( AFC.hFile, Prefix, APL_ARRSIZE(g_UnicodeFilePrefix), &BytesRead, NULL );

   if( 
      Result == FALSE || 
      BytesRead != APL_ARRSIZE(g_UnicodeFilePrefix) ||
      std::memcmp( Prefix, g_UnicodeFilePrefix, APL_ARRSIZE(g_UnicodeFilePrefix)) != 0 
   )
      return ERROR_INVALID_DATA;

   //Определяем размер файла
   DWORD FileSize = GetFileSize( AFC.hFile, NULL );

   if( FileSize == INVALID_FILE_SIZE )
      return GetLastError();

   APL_ASSERT( FileSize >= APL_ARRSIZE(g_UnicodeFilePrefix) );
   FileSize -= APL_ARRSIZE(g_UnicodeFilePrefix); //Префикс считывать не нужно
   
   Str.resize( FileSize / sizeof(WCHAR) + FileSize % sizeof(WCHAR) );

   APL_ASSERT( Str.size() * sizeof(WCHAR) >= FileSize );

   Result = ::ReadFile( AFC.hFile, const_cast<WCHAR *>(Str.c_str()), FileSize, &BytesRead, NULL );

   if( !Result )
      return GetLastError();

   APL_ASSERT( FileSize == BytesRead );

   return ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////

DWORD WriteUnicodeFile( const std::basic_string<TCHAR> &FileName, const std::basic_string<WCHAR> &Str )
{
   BOOL Result;
   DWORD BytesWrite;

   TAutoFileClose AFC = ::CreateFile(  FileName.c_str(),             // create MYFILE.TXT 
                                       GENERIC_WRITE,                // open for writing 
                                       0,                            // do not share 
                                       NULL,                         // no security 
                                       CREATE_ALWAYS,                // overwrite existing 
                                       FILE_ATTRIBUTE_NORMAL,        // normal file 
                                       NULL );                       // no attr. template 

   if( AFC.hFile == INVALID_HANDLE_VALUE )
      return GetLastError();

   Result = ::WriteFile( AFC.hFile, g_UnicodeFilePrefix, APL_ARRSIZE(g_UnicodeFilePrefix), &BytesWrite, NULL );

   if( !Result || BytesWrite != APL_ARRSIZE(g_UnicodeFilePrefix) )
      return GetLastError();
   
   Result = ::WriteFile( AFC.hFile, Str.c_str(), static_cast<DWORD>(Str.size() * sizeof(WCHAR)), &BytesWrite, NULL );

   if( !Result || BytesWrite != static_cast<DWORD>(Str.size() * sizeof(WCHAR)) )
      return GetLastError();

   return ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////

DWORD WriteAnsiFile( const std::basic_string<TCHAR> &FileName, const std::basic_string<WCHAR> &Str )
{
   BOOL Result;
   DWORD BytesWrite;

   TAutoFileClose AFC = ::CreateFile(  FileName.c_str(),             // create MYFILE.TXT 
                                       GENERIC_WRITE,                // open for writing 
                                       0,                            // do not share 
                                       NULL,                         // no security 
                                       CREATE_ALWAYS,                // overwrite existing 
                                       FILE_ATTRIBUTE_NORMAL,        // normal file 
                                       NULL );                       // no attr. template 

   if( AFC.hFile == INVALID_HANDLE_VALUE )
      return GetLastError();

   if( Str.length() == 0 )
      return ERROR_SUCCESS; //Файл создан и он пустой - то что нужно

   std::vector<CHAR> Buf(Str.length());

   APL_CHECK( WideCharToMultiByte(CP_ACP, 0, Str.c_str(), (int)Str.length(), &Buf[0], (int)Buf.size(), NULL, NULL) );

   Result = ::WriteFile( AFC.hFile, &Buf[0], static_cast<DWORD>(Buf.size() * sizeof(CHAR)), &BytesWrite, NULL );

   if( !Result || BytesWrite != static_cast<DWORD>(Buf.size() * sizeof(CHAR)) )
      return GetLastError();

   return ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////

DWORD ReadUnicodeAnsiFile( const std::basic_string<TCHAR> &FileName, std::basic_string<WCHAR> &Str )
{
   Str.clear();

   TAutoFileClose AFC = ::CreateFile(  FileName.c_str(),          // open MYFILE.TXT 
                                       GENERIC_READ,              // open for reading 
                                       FILE_SHARE_READ,           // share for reading 
                                       NULL,                      // no security 
                                       OPEN_EXISTING,             // existing file only 
                                       FILE_ATTRIBUTE_NORMAL,     // normal file 
                                       NULL);                     // no attr. template   

   if( AFC.hFile == INVALID_HANDLE_VALUE )
      return GetLastError();

   //Определяем размер файла
   DWORD FileSize = GetFileSize( AFC.hFile, NULL );

   if( FileSize == INVALID_FILE_SIZE )
      return GetLastError();

   if( FileSize == 0 )
     return ERROR_SUCCESS;

   //Читаем первые несколько байт для определения типа файла
   BYTE Prefix[ APL_ARRSIZE(g_UnicodeFilePrefix) ];
   DWORD BytesRead;
   BOOL Result = ::ReadFile( AFC.hFile, Prefix, APL_ARRSIZE(g_UnicodeFilePrefix), &BytesRead, NULL );
    
   if( 
      Result == FALSE || 
      BytesRead != APL_ARRSIZE(g_UnicodeFilePrefix) ||
      std::memcmp( Prefix, g_UnicodeFilePrefix, APL_ARRSIZE(g_UnicodeFilePrefix)) != 0 
   )
   {
      //Мы работаем с ANSI файлом
      
      //Возвращаем курсор снова на начало файла
      APL_CHECK(::SetFilePointer( AFC.hFile, 0, NULL, FILE_BEGIN ) != INVALID_SET_FILE_POINTER );
      
      std::vector<CHAR> Buf(FileSize);

      Result = ::ReadFile( AFC.hFile, &Buf[0], FileSize, &BytesRead, NULL );

      if( !Result )
         return GetLastError();

      APL_ASSERT( FileSize == BytesRead );

      //Выполняем преобразование
      Str.resize( FileSize );

      MultiByteToWideChar(CP_ACP, 0, &Buf[0], BytesRead, const_cast<WCHAR *>(Str.c_str()), BytesRead);
      
      return ERROR_SUCCESS;
   }
   else
   {
      //Мы работаем с UNICODE файлом

      APL_ASSERT( FileSize >= APL_ARRSIZE(g_UnicodeFilePrefix) );
      FileSize -= APL_ARRSIZE(g_UnicodeFilePrefix); //Префикс считывать не нужно
      
      Str.resize( FileSize / sizeof(WCHAR) + FileSize % sizeof(WCHAR) );

      APL_ASSERT( Str.size() * sizeof(WCHAR) >= FileSize );

      Result = ::ReadFile( AFC.hFile, const_cast<WCHAR *>(Str.c_str()), FileSize, &BytesRead, NULL );

      if( !Result )
         return GetLastError();

      APL_ASSERT( FileSize == BytesRead );

      return ERROR_SUCCESS;
   }
}



}} //namespace SS::Lib 
