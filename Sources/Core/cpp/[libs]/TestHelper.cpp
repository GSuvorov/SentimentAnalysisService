#include "stdafx.h"
#include "TestHelper.h"
#include "CoutConvert.h"

#include <shellapi.h>

namespace SS { namespace Lib {

using namespace std;

ConsoleSynchronous Console;

///////////////////////////////////////////////////////////////////////////////

void ExecutePreRunFile( TCHAR *FilePath )
{
#if 0
   SHELLEXECUTEINFO ShExecInfo;
   
   TCHAR FullPath[MAX_PATH], FileDir[MAX_PATH];
   LPTSTR FileName;

   APL_ERROR( GetFullPathName(FilePath, MAX_PATH, FullPath, &FileName) != 0 );

   memcpy( FileDir, FullPath, (FileName - FullPath) * sizeof(*FileDir) );
   FileDir[FileName - FullPath] = _T('\0');

   ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
   ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
   ShExecInfo.hwnd = NULL;
   ShExecInfo.lpVerb = NULL;
   ShExecInfo.lpFile = FullPath;
   
#ifdef DEBUG
   ShExecInfo.lpParameters = _T( "DEBUG" );
#else
   ShExecInfo.lpParameters = _T( "RELEASE" );
#endif

   ShExecInfo.lpDirectory = FileDir;
   ShExecInfo.nShow = nShowCmd;
   ShExecInfo.hInstApp = NULL;

   cout << "Запуск файла " << FullPath;
   if( ShellExecuteEx(&ShExecInfo) != TRUE )
   {
      cout << "ERROR: " << GetDWErrorInfo( GetLastError() );
      throw TSilentException();
   }

   if( ShExecInfo.hProcess != NULL )
   {
      DWORD ExitCode;

      APL_ERROR( WaitForSingleObject(ShExecInfo.hProcess, INFINITE) != WAIT_FAILED );
      APL_ERROR( GetExitCodeProcess(ShExecInfo.hProcess, &ExitCode) );       
      APL_ERROR( CloseHandle(ShExecInfo.hProcess) != 0 );

      if( ExitCode != 0 )
      {
         cout << " ERROR ExitCode != 0!"<< endl;
         throw TSilentException();
      }
      else
         cout << " OK!" << endl;
   }
   else
      cout << " OK (no hProcess)!" << endl;
#else

   #ifdef DEBUG
      #define CMD_LINE _T( " DEBUG" )
   #else
      #define CMD_LINE _T( " RELEASE" )
   #endif

   TCHAR FullPath[MAX_PATH], FileDir[MAX_PATH];
   LPTSTR FileName;

   APL_ERROR( GetFullPathName(FilePath, MAX_PATH, FullPath, &FileName) != 0 );

   memcpy( FileDir, FullPath, (FileName - FullPath) * sizeof(*FileDir) );
   FileDir[FileName - FullPath] = _T('\0');
   
   lstrcat( FullPath, CMD_LINE );

   STARTUPINFO si;
   PROCESS_INFORMATION pi;

   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );


   cout << "Запуск файла: " << FullPath << endl << APL_LINE;
   // Start the child process. 
   if( !CreateProcess( NULL, // No module name (use command line). 
      FullPath,             // Command line. 
      NULL,             // Process handle not inheritable. 
      NULL,             // Thread handle not inheritable. 
      FALSE,            // Set handle inheritance to FALSE. 
      0,                // No creation flags. 
      NULL,             // Use parent's environment block. 
      FileDir,          // Use parent's starting directory. 
      &si,              // Pointer to STARTUPINFO structure.
      &pi )             // Pointer to PROCESS_INFORMATION structure.
      ) 
   {
      cout << "ERROR: " << GetDWErrorInfo( GetLastError() );
      throw TSilentException();
   }

   DWORD ExitCode;

   APL_ERROR( WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_FAILED );
   APL_ERROR( GetExitCodeProcess(pi.hProcess, &ExitCode) );       
   APL_ERROR( CloseHandle(pi.hProcess) );
   APL_ERROR( CloseHandle(pi.hThread) );
   cout << APL_LINE;

   if( ExitCode != 0)
   {
      cout << " ERROR ExitCode: " << ExitCode << endl;
      throw TSilentException();
   }
   else
      cout << "OK!" << endl << endl;

#endif
}
///////////////////////////////////////////////////////////////////////////////

/*
void CopyLoadManager()
{
#ifdef DEBUG
   std::basic_string<TCHAR> CopyFrom(GetExeDirPath() + _T("..\\..\\..\\..\\Bin.debug\\Core\\LoadManager.dll") );
#else
   std::basic_string<TCHAR> CopyFrom(GetExeDirPath() + _T("..\\..\\..\\..\\Bin\\Core\\LoadManager.dll") );
#endif

   std::basic_string<TCHAR> CopyTo(GetExeDirPath() + _T("LoadManager.dll") );

   cout << "Копируем\n\t" << CopyFrom << "\n\t" << CopyTo << endl << endl;
   if( CopyFile(CopyFrom.c_str(), CopyTo.c_str(), FALSE) == FALSE )
   {
      cout << GetDWErrorInfo(GetLastError());
      throw TSilentException();
   }
}
*/

///////////////////////////////////////////////////////////////////////////////
// class ConsoleSynchronous
///////////////////////////////////////////////////////////////////////////////
void ConsoleSynchronous::StdData()
{
   std::cout << GetTimeStampString() << " ";
}



}} //namespace SS::Lib 
