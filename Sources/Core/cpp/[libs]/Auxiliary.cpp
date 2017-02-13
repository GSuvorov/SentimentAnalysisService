#include "stdafx.h"
#include "Auxiliary.h"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi") //подлинкуем shlwapi.lib для PathFindFileName

namespace SS { namespace Lib {

std::wstring &Convert(const std::string &In, std::wstring &Out)
{
#if 0
   int Rez = static_cast<int>(In.length()); 

   std::vector<WCHAR> Vec(Rez);

   APL_CHECK( MultiByteToWideChar(CP_ACP, 0, In.c_str(), Rez, &Vec[0], Rez) == Rez );

   Out.assign( Vec.begin(), Vec.end() );

   return Out;
#else
   Out.resize( In.size() );

   APL_CHECK( MultiByteToWideChar(CP_ACP, 0, In.c_str(), (int)In.size(), const_cast<WCHAR *>(Out.c_str()), (int)Out.size()) == Out.size() );
   return Out;
#endif
}
///////////////////////////////////////////////////////////////////////////////

std::string &Convert(const std::wstring &In, std::string &Out)
{
#if 0
   int Rez = static_cast<int>(In.length()); 

   std::vector<CHAR> Vec(Rez);

   APL_CHECK( WideCharToMultiByte(CP_ACP, 0, In.c_str(), Rez, &Vec[0], Rez, NULL, NULL) == Rez );

   Out.assign( Vec.begin(), Vec.end() );

   return Out;
#else
   Out.resize( In.size() );
   APL_CHECK( WideCharToMultiByte(CP_ACP, 0, In.c_str(), (int)In.size(), const_cast<CHAR *>(Out.c_str()), (int)Out.size(), NULL, NULL) == Out.size() );

   return Out;
#endif
}
///////////////////////////////////////////////////////////////////////////////

std::wstring &ToLower( const std::wstring &Src, std::wstring &Dst )
{
#if 0 
   std::vector<wchar_t> Buff( Src.begin(), Src.end() ); 
   CharLowerBuffW( &Buff[0], static_cast<DWORD>(Buff.size()) );
   Dst.assign( Buff.begin(), Buff.end() );

   return Dst;
#else
   Dst = Src;
   CharLowerBuffW( const_cast<WCHAR *>(Dst.c_str()), static_cast<DWORD>(Dst.size()) );
   return Dst;
#endif
}
///////////////////////////////////////////////////////////////////////////////

std::basic_string<TCHAR> GetExeDirPath()
{
   TCHAR Buf[MAX_PATH];

   APL_CHECK( ::GetModuleFileName(NULL, Buf, MAX_PATH) );

   //TCHAR *pRez = _tcsrchr( Buf, _T('\\') );

   //if( pRez != NULL )
   //   *(pRez + 1) = _T('\0'); //Сохраняем  '\'

   *PathFindFileName(Buf) = _T('\0');

   return Buf;
}
///////////////////////////////////////////////////////////////////////////////

std::basic_string<TCHAR> GetTimeStampString()
{
   SYSTEMTIME ST;
   const size_t BuffSize = 10;
   TCHAR Buff[BuffSize];

   GetLocalTime( &ST );

   APL_CHECK( GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | LOCALE_NOUSEROVERRIDE, &ST, NULL, Buff, BuffSize) );

   return Buff;
}
///////////////////////////////////////////////////////////////////////////////

std::basic_string<TCHAR> GetHRErrorInfo( HRESULT hr )
{
	LPTSTR msg = 0;
	std::basic_string<TCHAR> Str;

	DWORD Rez = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM, 0, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&msg, 0, 0 );

	if( Rez )
	{
		//В зависимости от того определён UNICODE Нам необходимо преобразовать
		//msg в обычную строку
		try
		{
			Str = msg;

			//Часто функция FormatMessage вставляет символы перевода строки и точки,
			//а нам это не надо
			std::basic_string<TCHAR>::reverse_iterator I;

			for( I = Str.rbegin();
				I != Str.rend() && (*I == _T('\n') || *I == _T('\r') || *I == _T('.') );
				++I
				) { ; }

		   Str.erase( I.base(), Str.end() );

         LocalFree( msg );	
		}
		catch(...)
		{
			LocalFree( msg );
			throw;
		}
	}
	else
		Str = _T("Невозможно получить описание ошибки");


	return Str;
}

///////////////////////////////////////////////////////////////////////////////

}} //namespace SS::Lib 

#ifdef NWLIB_SS_FRAMEWORK
#include "SSFramework/SSAuxiliary.cpp"
#endif

