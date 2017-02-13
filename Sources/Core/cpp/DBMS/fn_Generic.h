#pragma once

//#include "..\containers\define.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "fn_Generic.h"

#define _F __FUNCTION__

///	Запись сообщения об ошибке в журнал системы
/**
	\param pFuncName  - имя функции в которой произошла ошибка
	\param nFailCode  - код ошибки
	\param pFormatStr - строка форматирования
	\param ...        - переменный список параметров
	\return HRESULT   - код ошибки nFailCode или код от GetLastError()

	Запись сообщения об ошибке в журнал системы
	*/
HRESULT LogFail(CHAR * pFuncName, HRESULT nFailCode, _TCHAR * pFormatStr, ...);

///	Запись простого сообщения в журнал системы
/**
	\param pFormatStr - строка форматирования
	\param ... - переменный список параметров
	\return VOID

	Запись простого сообщения в журнал системы
	*/
VOID LogMess(_TCHAR * pFormatStr, ...);

extern BOOL g_bItIsCDROM;

/// Программа запущена с CD-ROM ( TRUE == Да )
BOOL IsItCDROM(VOID);


///	Перевод CHAR-строки  в TCHAR-строку
/**
	\param   CONST CHAR *
	\return _TCHAR *

	Перевод CHAR-строки  в TCHAR-строку
	*/
//_TCHAR * CHAR2TCHAR(CONST    CHAR * pszStr);
_TCHAR * CHAR2TCHAR(CONST CHAR * pszStr, _TCHAR * pszStrDst);

///	Перевод TCHAR-строки  в WCHAR-строку
/**
	\param   CONST _TCHAR *
	\return  WCHAR *

	Перевод TCHAR-строки  в WCHAR-строку
	*/
//WCHAR * TCHAR2WCHAR(CONST _TCHAR * pszStr);
WCHAR * TCHAR2WCHAR(CONST _TCHAR * pszStr, WCHAR * pszStrDst);

/// Удаление файлов
/**
	\param   _TCHAR *

	Удаление файлов
	*/
BOOL DeleteFiles(CONST _TCHAR * pszPath, CONST _TCHAR * pszBackSlashAndMask);

/// Сохранение BACKUP-файлов
/**
	\param   _TCHAR *

	Сохранение BACKUP-файлов
	*/
BOOL BakFiles_Save(CONST _TCHAR * pszPath);

/// Восстановление BACKUP-файлов
/**
	\param   _TCHAR *

	Восстановление BACKUP-файлов
	*/
BOOL BakFiles_Restore(CONST _TCHAR * pszPath);

/// Сушествование BACKUP-файлов
/**
	\param   _TCHAR *

	Сушествование BACKUP-файлов
	*/
BOOL BakFiles_Exist(CONST _TCHAR * pszPath);


/// STL-контейнер 
class CContainer_HMap_Int_Int   {

	public:

		BOOL Add(INT iInt1, VOID * iInt2)   {
			if(Find(iInt1))   {
				return(TRUE);
			} else   {
				_oPair.first  = iInt1;
				_oPair.second = iInt2;
				_oRet=_oMap.insert(_oPair);
				_oItr=_oRet.first;	
				return(_oRet.second==TRUE);
			}
		}

		BOOL Find(INT iInt1)  {
			_oItr=_oMap.find(iInt1);
			return(_oItr!=_oMap.end());
		}

		VOID DeleteFound(VOID)  {
			_oItr=_oMap.erase(_oItr);
		}

		UINT Size(VOID)    { return((UINT)_oMap.size()); }
		VOID Clear(VOID)   {	_oMap.clear();	}
		VOID Start(VOID)   {	_oItr = _oMap.begin(); _iIdx=-1; }
		BOOL Get(VOID)     {	if(++_iIdx) _oItr++; return(_oItr != _oMap.end()); }
		INT  First(VOID)   { return((*_oItr).first); }
		VOID * Second(VOID)  { return((*_oItr).second); }


		typedef stdext::hash_map< INT , VOID * > TMap;
		typedef std::pair< INT , VOID * >        TPair;
		typedef TMap::iterator                TItr;
		typedef std::pair<TItr, BOOL>         TRet;

		TMap  _oMap;
		TPair _oPair;
		TItr  _oItr;
		TRet  _oRet; 
		INT   _iIdx;
};



