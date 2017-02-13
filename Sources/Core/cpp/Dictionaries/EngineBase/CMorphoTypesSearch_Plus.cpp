#include "stdafx.h"
#include "cl_TimeMeter.h"
#include "CMorphoTypesSearch.h"

using namespace SS::Dictionary::Core;
using namespace SS::Dictionary::Core::CMorphoTypesSearchDbg;

/// Запись сообщения об ошибке в журнал системы
HRESULT SS::Dictionary::Core::CMorphoTypesSearchDbg::LogFail(char * pFuncName, char * pFormatStr, ...)   {

	char    ErrStr[1024];
	va_list ArgList;
	int     Len=0;

	va_start(ArgList,pFormatStr);
		Len=sprintf(ErrStr,"   PartOfWordSearch ERROR: ");
		Len+=vsprintf(ErrStr+strlen(ErrStr),pFormatStr,ArgList);
		Len+=sprintf( ErrStr+strlen(ErrStr),". Method: %s().",pFuncName);

		#ifdef _DEBUG
			CharToOemA(ErrStr,ErrStr); 
			printf("%s\n",ErrStr);
			Beep(300,100);
		#endif

		wchar_t wcErrStr[1024];
		mbstowcs(wcErrStr, ErrStr, 1024);
		SS_THROW(wcErrStr);
	va_end(ArgList);

	return(E_FAIL);
}

/// Запись простого сообщения в журнал системы
VOID SS::Dictionary::Core::CMorphoTypesSearchDbg::LogMess(char * pFormatStr, ...)   {
	char    MStr[1024];
	va_list ArgList;
	int     Len=0;

	va_start(ArgList,pFormatStr);
		Len=sprintf(MStr,"PartOfWordSearch MESSAGE: ");
		Len+=vsprintf(MStr+strlen(MStr),pFormatStr,ArgList);

		#ifdef _DEBUG
			CharToOemA(MStr,MStr); 
			printf("%s\n",MStr);
			Beep(300,100);
		#endif

		wchar_t wcErrStr[1024];
		mbstowcs(wcErrStr, MStr, 1024);
		SAVE_LOG(SS_MESSAGE AND wcErrStr);
	va_end(ArgList);
}
