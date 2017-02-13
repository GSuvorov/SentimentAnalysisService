#include "StdAfx.h"
#include "cl_DiskFile.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_DiskFile.cpp"

//	!!! See comments in header file

DiskFileC::DiskFileC() :
		m_FHandle(INVALID_HANDLE_VALUE)   {
/*
	if(GetDiskFreeSpace("C:\\",         // LPCTSTR lpRootPathName,
								NULL,           // LPDWORD lpSectorsPerCluster,
								&m_FSectorSize, // LPDWORD lpBytesPerSector,
								NULL,           // LPDWORD lpNumberOfFreeClusters,
								NULL            // LPDWORD lpTotalNumberOfClusters
								))   {
		std::cout << "SIZE: " << m_FSectorSize << "\n";
	} else   {
		m_FSectorSize=512; // ???
	}
*/
/*
Platform                   High-order bit |  Next 7 bits | Low-order byte
Windows NT 3.51            0                 Build number   3
Windows NT 4.0             0                 Build number   4
Windows 2000 or Windows XP 0                 Build number   5
Windows 95, 98, or Me      1                 Reserved       4
Win32s with Windows 3.1    1                 Build number   3
*/
	m_CoolOsFlag = !(GetVersion() & 0x8000);
}


DiskFileC::~DiskFileC()   {
	Close();
	m_FHandle=INVALID_HANDLE_VALUE;
}


void DiskFileC::SetPathAndName(CONST STLSTR * FilePath, CONST _TCHAR * FileName)   {

	m_FPath=*FilePath;
	m_FName= FileName;

	if(!m_FPath.empty() && *(m_FPath.rbegin())!='\\')   m_FPath+='\\';

	m_FFullPath=m_FPath+m_FName;
}


BOOL DiskFileC::ChkDirExistence(void)   {
/*
	WIN32_FIND_DATA FindData;
	HANDLE          FindHandle;

	if(m_FFullPath.empty())   return(FALSE);

	FindHandle=FindFirstFile(m_FFullPath.c_str(),&FindData);
	FindClose(FindHandle);

	return(FindHandle!=INVALID_HANDLE_VALUE);
*/
	return(GetFileAttributes(m_FFullPath.c_str())!=INVALID_FILE_ATTRIBUTES);
}

HRESULT DiskFileC::Open(VOID)   {
	if(m_FHandle!=INVALID_HANDLE_VALUE)   LogMess(_T("File <%s> is still OPEN (was not closed)"),m_FFullPath.c_str());
	Close();

	UINT32 iAccessMode, iShareMode;
	if(!g_bItIsCDROM)   {
		SetFileAttributes(m_FFullPath.c_str(),FILE_ATTRIBUTE_ARCHIVE);
		iAccessMode = GENERIC_READ | GENERIC_WRITE, iShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	} else   {
		iAccessMode = GENERIC_READ                , iShareMode = FILE_SHARE_READ;
	}
/*
	if(strstr(m_FFullPath.c_str(),"Records"))   {
		LogMess("### MkFileIn DIR      (\"%s\")",m_FFullPath.c_str());
		if(GetFileAttributes(m_FPath.c_str())==INVALID_FILE_ATTRIBUTES)   LogMess("!!! No DIR DIR !!! (\"%s\")",m_FPath.c_str());
	}
*/
	m_FHandle=CreateFile(m_FFullPath.c_str(),
							iAccessMode, // | FILE_READ_DATA | FILE_WRITE_DATA  ?  GENERIC_READ | GENERIC_WRITE, //FILE_ALL_ACCESS,
							iShareMode , // | FILE_SHARE_DELETE,
							NULL, // LPSECURITY_ATTRIBUTES
							OPEN_ALWAYS, // OPEN_EXISTING CREATE_ALWAYS OPEN_ALWAYS

							// FILE_FLAG_WRITE_THROUGH |
							// FILE_FLAG_SEQUENTIAL_SCAN |
							// FILE_ATTRIBUTE_TEMPORARY |
							// FILE_ATTRIBUTE_NORMAL |

							FILE_ATTRIBUTE_ARCHIVE  |
							FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,

							NULL // hTemplateFile
							);

	if(m_FHandle!=INVALID_HANDLE_VALUE)   {
		return(S_OK);
	} else   {
		return(LogFail(_F,0,_T("Error in CreateOrOpenFile(\"%s\")"),m_FFullPath.c_str()));
		Sleep(3000);
	}
}

#define DiskFileC_Delete_MAX_TRY_NUM 100

HRESULT DiskFileC::Delete(VOID)   {
	if(!g_bItIsCDROM)   {
		if(m_FHandle!=INVALID_HANDLE_VALUE)   LogMess(_T("File <%s> was NOT CLOSED before deleteing."),m_FFullPath.c_str());
		Close();
		SetFileAttributes(m_FFullPath.c_str(),FILE_ATTRIBUTE_ARCHIVE);
		DeleteFile(m_FFullPath.c_str());
		
		int iTryCnt=0;
		while(GetFileAttributes(m_FFullPath.c_str())!=INVALID_FILE_ATTRIBUTES && iTryCnt++<DiskFileC_Delete_MAX_TRY_NUM)   Sleep(10);
		if(iTryCnt<DiskFileC_Delete_MAX_TRY_NUM)   {
			return(S_OK);
		} else   {
			return(LogFail(_F,0,_T("File <%s> was not deleted"),m_FFullPath.c_str()));
		}
	} else   {
		return(S_OK);
	}
}

/// Сохранить с именем ~*.*
HRESULT DiskFileC::RenameBack(VOID)   {
	if(!g_bItIsCDROM)   {
		if(m_FHandle!=INVALID_HANDLE_VALUE)   LogMess(_T("File <%s> was NOT CLOSED before renaming."),m_FFullPath.c_str());
		Close();
		SetFileAttributes(m_FFullPath.c_str(),FILE_ATTRIBUTE_ARCHIVE);
		WCHAR wszNewName[1024];
		wcscpy(wszNewName,m_FFullPath.c_str());
		WCHAR * pCh = wcsrchr(wszNewName,L'\\');
		if(pCh)   {
			pCh++;
			*pCh=L'~';
		//	DeleteFile(wszNewName);
		//	int iTryCnt=0;
		//	while(GetFileAttributes(wszNewName)!=INVALID_FILE_ATTRIBUTES && iTryCnt++<DiskFileC_Delete_MAX_TRY_NUM)   Sleep(10);
			CopyFile(m_FFullPath.c_str(),wszNewName,FALSE);
			return(Delete());
		}
		return(S_FALSE);
	} else   {
		return(S_OK);
	}
}


/// Переименовать из *.* в *.*_tmp
HRESULT DiskFileC::RenameTmp(VOID)   {
	if(!g_bItIsCDROM)   {
		if(_tcsstr(m_FFullPath.c_str(),_T("_tmp")))   {
			STLSTR sNewName = m_FFullPath;
			sNewName.resize(sNewName.length()-4);
			Move(sNewName.c_str());
			return(S_OK);
		} else   {
			return(LogFail(_F,0,_T("Try to rename not TMP file \"%s\""),m_FFullPath.c_str()));
		}
	} else   {
		return(S_OK);
	}
}


HRESULT DiskFileC::Move(CONST _TCHAR * sNewFileName)   {
	if(!g_bItIsCDROM)   {
		if(m_FHandle!=INVALID_HANDLE_VALUE)   LogMess(_T("File <%s> was NOT CLOSED before moveing."),m_FFullPath.c_str());
		Close();
		SetFileAttributes(m_FFullPath.c_str(),FILE_ATTRIBUTE_ARCHIVE);

		if(MoveFile(m_FFullPath.c_str(),sNewFileName))   {
			#define DiskFileC_Move_MAX_TRY_NUM 100
			int iTryCnt=0;
			while(GetFileAttributes(m_FFullPath.c_str())!=INVALID_FILE_ATTRIBUTES && iTryCnt++<DiskFileC_Move_MAX_TRY_NUM)   Sleep(10);
			if(iTryCnt<DiskFileC_Move_MAX_TRY_NUM)   {
				return(S_OK);
			}
		}
		return(LogFail(_F,0,_T("File <%s> was not moved"),m_FFullPath.c_str()));
	} else   {
		return(S_OK);
	}
}


HRESULT DiskFileC::Flush(VOID)   {
	if(!g_bItIsCDROM)   {
		return(FlushFileBuffers(m_FHandle)?S_OK:LogFail(_F,0,_T("FlushFileBuffersError")));
	} else   {
		return(S_OK);
	}
}

HRESULT DiskFileC::Close(BOOL bClearAfterClose)   {
	if(m_FHandle!=INVALID_HANDLE_VALUE)   {
		if(bClearAfterClose)   Clear();
		BOOL RetVal=CloseHandle(m_FHandle);
		if(!g_bItIsCDROM)   SetFileAttributes(m_FFullPath.c_str(),FILE_ATTRIBUTE_ARCHIVE/* | FILE_ATTRIBUTE_READONLY*/);
		m_FHandle=INVALID_HANDLE_VALUE;
		return(RetVal?S_OK:LogFail(_F,0,_T("CloseHandle()Error. File <%s>"),m_FFullPath.c_str()));
	} else   {
		return(S_OK); // LogFail(_F,0,"File <%s> already closed.",m_FFullPath.c_str());
	}
}

HRESULT DiskFileC::Clear(VOID)   {
	if(!g_bItIsCDROM)   {
		if(m_FHandle!=INVALID_HANDLE_VALUE)   {
			if( SetPosition(0,FILE_BEGIN)!=S_OK)   return(LogFail(_F,0,_T("SetFilePosition()Error. File <%s>"),m_FFullPath.c_str()));
			if(!SetEndOfFile(m_FHandle))   return(LogFail(_F,0,_T("SetEndOfFile()Error. File <%s>"),m_FFullPath.c_str()));
			if( Flush()!=S_OK)   return(LogFail(_F,0,_T("FlushFileBuffers()Error. File <%s>"),m_FFullPath.c_str()));
			return(S_OK);
		} else   {
			LogFail(_F,0,_T("File <%s> is closed."),m_FFullPath.c_str());
			return(E_FAIL);
		}
	} else   {
		return(S_OK);
	}
}

HRESULT DiskFileC::Fill(UINT32 nBytesNum32, BYTE nFillValue)   {
	if(!g_bItIsCDROM)   {
		if(m_FHandle!=INVALID_HANDLE_VALUE)   {
			BYTE *  Buffer;
			int     BufferLen = 128L*1024L;

			if(SetPosition(0L,FILE_END)!=S_OK)   return(LogFail(_F,0,_T("")));
			if((Buffer=(BYTE *)malloc(BufferLen))==NULL)    return(LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("NoMemoryForTempBuffer")));

			memset(Buffer,nFillValue,BufferLen);

			UINT32 i32, iFSize32;

			i32 = iFSize32 = GetSize();

			iFSize32  += nBytesNum32;
			do   {
				if(_Write(Buffer,BufferLen)!=S_OK)   return(LogFail(_F,0,_T("")));
				i32+=BufferLen;
			} while(i32 < iFSize32);

			free(Buffer);

			if(SetPosition(iFSize32,FILE_BEGIN)!=S_OK)   return(LogFail(_F,0,_T("")));
			if(!SetEndOfFile(m_FHandle))   return(LogFail(_F,0,_T("")));

			Flush();

			return(S_OK);
		} else   {
			LogFail(_F,0,_T("File <%s> is closed."),m_FFullPath.c_str());
			return(E_FAIL);
		}
	} else   {
		return(S_OK);
	}
}

HRESULT DiskFileC::fprintf(CHAR * FormatStr, ...)   {
	if(!g_bItIsCDROM)   {
		va_list argList;
		INT32   NWrite;
		DWORD   NWtitten;
		CHAR    Str[5*1024];

		va_start(argList,FormatStr);
		NWrite=vsprintf(Str,FormatStr,argList);
		if(NWrite<0)   {
			LogFail(_F,0,_T("File <%s> possibly has too smale buffer for <fprintf()>."),m_FFullPath.c_str());
		} else   {
			WriteFile(m_FHandle,Str,NWrite,&NWtitten,NULL);
		}
		va_end(argList);
	}

	return(S_OK);
}

HRESULT DiskFileC::SetPosition(UINT32 nBytesNum32, DWORD nFromPosition /*= FILE_BEGIN*/)   {
	//LARGE_INTEGER FPos64;
	//FPos64.HighPart = 0;
	//FPos64.LowPart  = nBytesNum32;

	//if(SetFilePointer(m_FHandle,FPos64.LowPart,&FPos64.HighPart,nFromPosition)!=INVALID_SET_FILE_POINTER)   {
	//   return(S_OK);
	//} else   {
	//   return(E_FAIL);
	//}

	if(SetFilePointer(m_FHandle,nBytesNum32,NULL,nFromPosition)!=INVALID_SET_FILE_POINTER)   {
		return(S_OK);
	} else   {
		return(E_FAIL);
	}
}

UINT32 DiskFileC::GetPosition(VOID)   {
	return(SetFilePointer(m_FHandle,0,NULL,FILE_CURRENT));
}

UINT32 DiskFileC::GetSize(VOID)   {
	UINT32 iFSize32;

	if((iFSize32=GetFileSize(m_FHandle,NULL))==INVALID_FILE_SIZE)   {
		return(0);
	} else   {
		return(iFSize32);
	}
}

HRESULT DiskFileC::ReadAt(UINT32 nBeginOffset, LPVOID pData, INT32 nDataLen)   {

	if(/*nDataLen==0 || */pData==NULL)   return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("InvalidParametersFor<ReadAt()>")));

	if(SetPosition(nBeginOffset)!=S_OK)   return(LogFail(_F,0,_T("1")));
	if(_Read(pData,nDataLen)!=S_OK)   return(LogFail(_F,0,_T("2")));

	return(S_OK);
}

HRESULT DiskFileC::Read(LPVOID pData, DWORD nDataLen)   {

	if(/*nDataLen==0 || */pData==NULL)   return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("InvalidParametersFor<Read()>")));

	// if(SetPosition(nBeginOffset)!=S_OK)   return(LogFail(_F,0,""));
	if(_Read(pData,nDataLen)!=S_OK)   return(LogFail(_F,0,_T("1")));

	return(S_OK);
}

HRESULT DiskFileC::WriteAt(UINT32 nBeginOffset, LPCVOID pData, DWORD nDataLen)   {
	if(!g_bItIsCDROM)   {
		if(nDataLen==0 || pData==NULL)   return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("InvalidParametersFor<WriteAt()>")));

		if(SetPosition(nBeginOffset)!=S_OK)   return(LogFail(_F,0,_T("1")));
		if(_Write(pData,nDataLen)!=S_OK)   return(LogFail(_F,0,_T("2")));
	}

	return(S_OK);
}

HRESULT DiskFileC::WriteAtBackOffset(LPCVOID pData, INT32 nDataLen)   {
	if(!g_bItIsCDROM)   {
		if(nDataLen==0 || pData==NULL)   return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("InvalidParametersFor<WriteAt()>")));

		if(SetPosition(-nDataLen,FILE_CURRENT)!=S_OK)   return(LogFail(_F,0,_T("1")));
		if(_Write(pData,nDataLen)!=S_OK)   return(LogFail(_F,0,_T("2")));
	}

	return(S_OK);
}

HRESULT DiskFileC::WriteAtEnd(LPCVOID pData, DWORD nDataLen)   {
	if(!g_bItIsCDROM)   {
		if(nDataLen==0 || pData==NULL)   return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("InvalidParametersFor<WriteAtEnd()>")));

		if(SetPosition(0i64,FILE_END)!=S_OK)   return(LogFail(_F,0,_T("1")));
		if(_Write(pData,nDataLen)!=S_OK)   return(LogFail(_F,0,_T("2")));
	}

	return(S_OK);
}


/*
HRESULT DiskFileC::ReadAt(UINT32 nBeginOffset, LPVOID pData, DWORD nDataLen)   {

	if(nDataLen==0 || pData==NULL)   return(LogFail(_F,ERROR_INVALID_PARAMETER,"InvalidParametersFor<ReadAt()>"));

	if(SetPosition(nBeginOffset)!=S_OK)   return(LogFail(_F,0,""));
	if(_Read(pData,nDataLen)!=S_OK)   return(LogFail(_F,0,""));

	return(S_OK);
}

HRESULT DiskFileC::Read(LPVOID pData, DWORD nDataLen)   {

	if(nDataLen==0 || pData==NULL)   return(LogFail(_F,ERROR_INVALID_PARAMETER,"InvalidParametersFor<Read()>"));

	// if(SetPosition(nBeginOffset)!=S_OK)   return(LogFail(_F,0,""));
	if(_Read(pData,nDataLen)!=S_OK)   return(LogFail(_F,0,""));

	return(S_OK);
}

HRESULT DiskFileC::WriteAt(UINT32 nBeginOffset, LPCVOID pData, DWORD nDataLen)   {

	if(nDataLen==0 || pData==NULL)   return(LogFail(_F,ERROR_INVALID_PARAMETER,"InvalidParametersFor<WriteAt()>"));

	if(SetPosition(nBeginOffset)!=S_OK)   return(LogFail(_F,0,""));
	if(_Write(pData,nDataLen)!=S_OK)   return(LogFail(_F,0,""));

	return(S_OK);
}

HRESULT DiskFileC::WriteAtEnd(LPCVOID pData, DWORD nDataLen)   {

	if(nDataLen==0 || pData==NULL)   return(LogFail(_F,ERROR_INVALID_PARAMETER,"InvalidParametersFor<WriteAtEnd()>"));

	if(SetPosition(0i64,FILE_END)!=S_OK)   return(LogFail(_F,0,""));
	if(_Write(pData,nDataLen)!=S_OK)   return(LogFail(_F,0,""));
	m_FSize32+=nDataLen;

	return(S_OK);
}
*/

/*
HRESULT DiskFileC::_Write(LPCVOID pData, DWORD nDataLen)   {
	DWORD WrittenDataLen;

	if(!WriteFile(m_FHandle,pData,nDataLen,&WrittenDataLen,NULL) ||  nDataLen!=WrittenDataLen)   {
		return(LogFail(_F,0));
	} else   {
		return(S_OK);
	}
}

HRESULT DiskFileC::_Read(LPVOID pData, DWORD nDataLen)   {
	DWORD ReaddenDataLen; // Bad english

	if(!ReadFile(m_FHandle,pData,nDataLen,&ReaddenDataLen,NULL) || nDataLen!=ReaddenDataLen)   {
		return(LogFail(_F,0));
	} else   {
		return(S_OK);
	}
}

void DiskFileC::_UpdateRealSize(void)      {

	LARGE_INTEGER FileSize64;

	if(GetFileSizeEx(m_FHandle,&FileSize64))   {
		m_FSize32=FileSize64.QuadPart;
	} else   {
		m_FSize32=0;
	}

#if 0
	DWORD FileSize64A;
	DWORD FileSize64B;

	if(FileSize64A=GetFileSize(m_FHandle,&FileSize64B))   {
		m_FSize32=0;
	} else   {
		m_FSize32=0;
	}
#endif
}
*/

