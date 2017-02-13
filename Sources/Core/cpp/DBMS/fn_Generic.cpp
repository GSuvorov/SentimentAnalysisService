#include "StdAfx.h"
#include "fn_Generic.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "fn_Generic.cpp"

#ifndef USES_IBASE
	#define SS_THROW_CD(z) { \
		if(!(!spLoadManager)) \
		{ \
			SS::Interface::Core::CommonServices::IException* pException = NULL;\
			spLoadManager->Create(L"./CommonServices/UMCommonServices.dll", CLSID_Exception, IID_Exception, (void**)&pException); \
			SS_FILL_EXCEPTION(SS::Core::CommonServices::eteCritical); \
			pException->GetCurrent()->SetMessage(z, NULL); \
			throw pException; \
		} \
	}
#else
	#define SS_THROW_CD(z) { \
		SS::Interface::Core::CommonServices::IException* pException = \
		(SS::Interface::Core::CommonServices::IException*) \
		GetLoadManager()->GetInterface(L"./CommonServices/UMCommonServices.dll", CLSID_Exception, IID_Exception); \
		pException->Clear(); \
		SS_FILL_EXCEPTION(SS::Core::CommonServices::eteCritical); \
		pException->GetCurrent()->SetMessage(z, NULL); \
		throw pException;}
#endif


//	Запись сообщения об ошибке в журнал системы
HRESULT LogFail(CHAR * pFuncName, HRESULT nFailCode, _TCHAR * pFormatStr, ...)   {

	if(nFailCode==0)   nFailCode=GetLastError();

	SetLastError(nFailCode);

	if(FAILED(nFailCode))   {

		_TCHAR  ErrStr[1024];
		va_list ArgList;
		int     Len=0;

		va_start(ArgList,pFormatStr);
			Len =_stprintf( ErrStr,_T("   DBMS: "));
			Len+=_vstprintf(ErrStr+_tcslen(ErrStr),pFormatStr,ArgList);
			_TCHAR tcsStr[1024];
			Len+=_stprintf( ErrStr+_tcslen(ErrStr),_T(". Method: %s(), FailCode: 0x%lX."),CHAR2TCHAR(pFuncName,tcsStr),nFailCode);
			#ifdef _DEBUG
				_tprintf(_T("%s\n"),ErrStr);
				Beep(300,100);
			#endif
			WCHAR wcsStr[1024];
			if(g_bItIsCDROM)   {
				SS_THROW_CD(TCHAR2WCHAR(ErrStr,wcsStr));
			} else   {
				SS_THROW(   TCHAR2WCHAR(ErrStr,wcsStr));
			}
		va_end(ArgList);

		return(E_FAIL);
	} else   {
		return(S_OK);
	}
}

//	Запись простого сообщения в журнал системы
VOID LogMess(_TCHAR * pFormatStr, ...)   {
	_TCHAR  MStr[1024];
	va_list ArgList;
	int     Len=0;

	va_start(ArgList,pFormatStr);
		Len =_stprintf( MStr,_T("   DBMS: "));
		Len+=_vstprintf(MStr+_tcslen(MStr),pFormatStr,ArgList);
		#ifdef _DEBUG
			_tprintf(_T("%s\n"),MStr);
			Beep(300,100);
		#endif
		WCHAR wcsStr[1024];
		if(!g_bItIsCDROM)   {
			SAVE_LOG(SS_MESSAGE AND TCHAR2WCHAR(MStr,wcsStr));
		}
	va_end(ArgList);
}

BOOL g_bItIsCDROM = FALSE;

/// Программа запущена с CD-ROM ( TRUE == Да )
BOOL IsItCDROM(VOID)   {

	_TCHAR szModuleFileName[1024], * pCh;

	// Retrieves the fully qualified path for the specified module ( NULL == current).
	GetModuleFileName(NULL, szModuleFileName, 1024);

	// Make path like "F:\"
	if((pCh=_tcschr(szModuleFileName,_T('\\')))!=NULL)   *(pCh+1) = _T('\0'); 

	// Parameter : Pointer to a null-terminated string that specifies the root directory of the disk to return information about. A trailing backslash is required. If this parameter is NULL, the function uses the root of the current directory. 
	return(GetDriveType(szModuleFileName)==DRIVE_CDROM);

	// Returns :
	// DRIVE_UNKNOWN     - The drive type cannot be determined. 
	// DRIVE_NO_ROOT_DIR - The root path is invalid. For example, no volume is mounted at the path. 
	// DRIVE_REMOVABLE   - The disk can be removed from the drive. 
	// DRIVE_FIXED       - The disk cannot be removed from the drive. 
	// DRIVE_REMOTE      - The drive is a remote (network) drive. 
	// DRIVE_CDROM       - The drive is a CD-ROM drive. 
	// DRIVE_RAMDISK     - The drive is a RAM disk. 
}


/////	Перевод CHAR-строки в TCHAR-строку
//_TCHAR * CHAR2TCHAR(CONST CHAR * pszStr)   {
//	#ifdef _UNICODE
//		static  UINT  MAL_Idx=0;
//		static _TCHAR MAL_Str[3][1024];
//
//		MAL_Idx++;
//		MAL_Idx%=3;
//
//		mbstowcs(MAL_Str[MAL_Idx],pszStr,1024);
//		return(MAL_Str[MAL_Idx]);
//	#else
//		return((_TCHAR *)pszStr);
//	#endif
//}
//
//WCHAR * TCHAR2WCHAR(CONST _TCHAR * pszStr)   {
//	#ifdef _UNICODE
//		return((WCHAR *)pszStr);
//	#else
//		static  UINT  MAL_Idx=0;
//		static  WCHAR MAL_Str[3][1024];
//
//		MAL_Idx++;
//		MAL_Idx%=3;
//
//		mbstowcs(MAL_Str[MAL_Idx],pszStr,1024);
//		return(MAL_Str[MAL_Idx]);
//	#endif
//}

///	Перевод CHAR-строки в TCHAR-строку
_TCHAR * CHAR2TCHAR(CONST CHAR * pszStr, _TCHAR * pszStrDst)   {
	#ifdef _UNICODE
		mbstowcs(pszStrDst,pszStr,1024);
		return(pszStrDst);
	#else
		return((_TCHAR *)pszStr);
	#endif
}

WCHAR * TCHAR2WCHAR(CONST _TCHAR * pszStr, WCHAR * pszStrDst)   {
	#ifdef _UNICODE
		return((WCHAR *)pszStr);
	#else
		mbstowcs(pszStrDst,pszStr,1024);
		return(pszStrDst);
	#endif
}


BOOL DeleteFiles(CONST _TCHAR * pszPath, CONST _TCHAR * pszBackSlashAndMask)   {
	if(!g_bItIsCDROM)   {
		STLSTR sFullMask = pszPath;
		sFullMask       += pszBackSlashAndMask;
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		BOOL   bFound = TRUE;
		hFind = FindFirstFile(sFullMask.c_str(), &FindFileData);
		while(bFound && hFind!=INVALID_HANDLE_VALUE)   {
			CONST _TCHAR * p1 = _tcsrchr(pszBackSlashAndMask,'.');
			CONST _TCHAR * p2 = _tcsrchr(FindFileData.cFileName,'.');
			if(p1==p2 || _tcsicmp(p1,p2)==0)   {
				sFullMask = pszPath;
				sFullMask+=_T("\\");
				sFullMask+=FindFileData.cFileName;
			
				SetFileAttributes(sFullMask.c_str(),FILE_ATTRIBUTE_ARCHIVE);
				DeleteFile(       sFullMask.c_str());
			}
			bFound=FindNextFile(hFind, &FindFileData);
		}
		FindClose(hFind);

		return(hFind!=INVALID_HANDLE_VALUE);
	} else   {
		return(TRUE);
	}
}

BOOL BakFiles_Save(CONST _TCHAR * pszPath)   {
	if(!g_bItIsCDROM)   {

		DeleteFiles(pszPath,_T("\\~T_*.*"));

		STLSTR sDstFile;
		STLSTR sFullMask = pszPath;
		sFullMask       += _T("\\DT_*.*");
		
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		BOOL   bFound = TRUE;
		hFind = FindFirstFile(sFullMask.c_str(), &FindFileData);
		while(bFound && hFind!=INVALID_HANDLE_VALUE)   {
			sFullMask = pszPath;
			sFullMask+=_T("\\");
			sDstFile  = sFullMask;
			sFullMask+=FindFileData.cFileName;
			FindFileData.cFileName[0]=_T('~');
			sDstFile +=FindFileData.cFileName; 

			CopyFile(sFullMask.c_str(),sDstFile.c_str(),FALSE);

			bFound=FindNextFile(hFind, &FindFileData);
		}
		FindClose(hFind);

		return(hFind!=INVALID_HANDLE_VALUE);
	} else   {
		return(TRUE);
	}
}

UINT32 GetFileSize(CONST _TCHAR * pszPath)   {

	HANDLE hFHandle=CreateFile(pszPath,
		GENERIC_READ , // FILE_READ_DATA | FILE_WRITE_DATA
		//GENERIC_READ | GENERIC_WRITE, //FILE_ALL_ACCESS,
		FILE_SHARE_READ , /* | FILE_SHARE_DELETE,*/ //
		NULL, // LPSECURITY_ATTRIBUTES
		OPEN_EXISTING, // OPEN_EXISTING CREATE_ALWAYS OPEN_ALWAYS

		// FILE_FLAG_WRITE_THROUGH |
		// FILE_FLAG_SEQUENTIAL_SCAN |
		// FILE_ATTRIBUTE_TEMPORARY |
		// FILE_ATTRIBUTE_NORMAL |
		0,
		//FILE_ATTRIBUTE_ARCHIVE  |
		//FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,

		NULL // hTemplateFile
	);

	if(hFHandle!=INVALID_HANDLE_VALUE)   {
		UINT32 iSize = GetFileSize(hFHandle,NULL);
		CloseHandle(hFHandle);
		return(iSize);
	} else   {
		return(0);
	}
}

BOOL BakFiles_Restore(CONST _TCHAR * pszPath)   {
	if(!g_bItIsCDROM)   {
		DeleteFiles(pszPath,_T("\\DT_*.*"));

		STLSTR sDstFile;
		STLSTR sFullMask = pszPath;
		sFullMask       += _T("\\~T_*.*");
		
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		BOOL   bFound = TRUE;
		hFind = FindFirstFile(sFullMask.c_str(), &FindFileData);
		while(bFound && hFind!=INVALID_HANDLE_VALUE)   {
			sFullMask = pszPath;
			sFullMask+=_T("\\");
			sDstFile  = sFullMask;
			sFullMask+=FindFileData.cFileName;
			FindFileData.cFileName[0]=_T('D');
			sDstFile +=FindFileData.cFileName; 

			if(!g_bItIsCDROM)   {
				SetFileAttributes(sDstFile.c_str(),FILE_ATTRIBUTE_ARCHIVE);
				if(!CopyFile(sFullMask.c_str(),sDstFile.c_str(),FALSE))   {
					LogFail(_F,ERROR_INVALID_PARAMETER,_T("ERROR: File was not copied (%s -> %s)"),sFullMask.c_str(),sDstFile.c_str());
				}
			}

			UINT32 iSz1 = GetFileSize(sFullMask.c_str());
			UINT32 iSz2 = GetFileSize(sDstFile.c_str());
		
			int iTryCnt=0;
			while(iSz1!=iSz2 && iTryCnt++<20)   {
				Sleep(50);
				iSz2 = GetFileSize(sDstFile.c_str());
			}

			//Sleep(10);
			//int iTryCnt=0;
			//while(GetFileAttributes(sDstFile.c_str())==INVALID_FILE_ATTRIBUTES && iTryCnt++<20)   Sleep(50);

			bFound=FindNextFile(hFind, &FindFileData);
		}
		FindClose(hFind);

		return(hFind!=INVALID_HANDLE_VALUE);
	} else   {
		return(TRUE);
	}
}

BOOL BakFiles_Exist(CONST _TCHAR * pszPath)   {
	STLSTR sFullMask = pszPath;
	sFullMask       += _T("\\~T_RecordsArr.dta");
	
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(sFullMask.c_str(), &FindFileData);
	FindClose(hFind);

	return(hFind!=INVALID_HANDLE_VALUE);
}


/*
#define _S DBMS_SRC_FILE
#define _F __FUNCTION__
#define _L __LINE__

HRESULT LogFail(char * pFileName, char * pFuncName, int nLineNum, HRESULT nFailCode, char * pCommentStr);
#define LogFail(_F,nFailCode,pCommentStr) LogFail(DBMS_SRC_FILE,__FUNCTION__,__LINE__,nFailCode,pCommentStr)

HRESULT DBMS::LogFail(char * pFileName, char * pFuncName, int nLineNum, HRESULT nFailCode, char * pCommentStr)   {

	if(nFailCode==0)   nFailCode=GetLastError();

	SetLastError(nFailCode);

	if(nFailCode)   {
		char ErrStr[1024];
		sprintf(ErrStr,"  DBMS: %s (0x%.2lX) in %s/%ld %s()",pCommentStr,nFailCode,pFileName,nLineNum,pFuncName);

		#ifdef _DEBUG
			printf("%s\n",ErrStr);
			Beep(300,100);
		#endif

		SAVE_LOG(SS_ERROR,ErrStr,NULL);
		return(E_FAIL);
	} else   {
		return(S_OK);
	}
}
*/

