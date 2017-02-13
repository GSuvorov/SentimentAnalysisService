#pragma once

#ifdef _DEBUG

#include <stdlib.h>
#include <tchar.h>

///	помечает место где мы находимся
#define	M_IMPL(__F, __L)								\
{													\
TCHAR	buffer[512];								\
TCHAR	ext[16];									\
TCHAR*	fn = _T( __F );								\
::OutputDebugString(_T("\nMARKER: FILE( "));		\
_tsplitpath(fn, NULL, NULL, buffer, ext);			\
::OutputDebugString(buffer);						\
::OutputDebugString(ext);							\
::OutputDebugString(_T(" )	LINE( "));				\
::OutputDebugString(_itot( __L, buffer, 10));		\
::OutputDebugString(_T(" )\n"));					\
}

#define	M_F_IMPL( __F, __L )						\
{													\
TCHAR	buffer[512];								\
TCHAR	ext[16];									\
TCHAR*	fn = _T( __F );								\
TCHAR*	fu = _T( __FUNCTION__ );						\
::OutputDebugString(_T("\nMARKER: FILE( "));		\
_tsplitpath(fn, NULL, NULL, buffer, ext);			\
::OutputDebugString(buffer);						\
::OutputDebugString(ext);							\
::OutputDebugString(_T(" )	LINE( "));				\
::OutputDebugString(_itot( __L, buffer, 10));		\
::OutputDebugString(_T(" )	FUNC( "));				\
::OutputDebugString(fu);							\
::OutputDebugString(_T(" )\n"));						\
}

#define	MARKER		M_F_IMPL( __FILE__ , __LINE__ )
#define	MARKER_NOF	M_IMPL( __FILE__ , __LINE__)

#else
#define	MARKER
#define	MARKER_NOF
#endif	//	_DEBUG