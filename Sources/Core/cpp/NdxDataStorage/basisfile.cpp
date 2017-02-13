#include "StdAfx.h"
#include ".\basisfile.h"

#include <tchar.h>

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

#define BAD_HANDLE(x) \
	(x==INVALID_HANDLE_VALUE?true:false)

//--------------------------------------------------------------------//

CBasisFile::CBasisFile(CDataUnitFactory* pDataStorageFactory)
:CDataUnit(pDataStorageFactory), m_FHandle(INVALID_HANDLE_VALUE)
{
}

CBasisFile::~CBasisFile()
{
}

HRESULT CBasisFile::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID==IID_NdxFile){
		*pBase=(INdxFile*)this;
	}else if(pIID==IID_NdxDataUnit){
		*pBase=(INdxDataUnit*)this;
	}else{
		*pBase=NULL;
	}

	return S_OK;
}


/// Программа запущена с CD-ROM ( TRUE == Да )
bool CBasisFile::IsCDROM(VOID)   
{

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



bool	CBasisFile::Open(const wchar_t* pszPath)
{
	DWORD dwCreationDisposition=0;
	DWORD dwDesiredAccess=0;
	DWORD dwShareMode = 0;


	if(m_FHandle==INVALID_HANDLE_VALUE)
	{
		wstring szFilePath(pszPath);
		wstring szFileAttr;
		m_BackupSettings.ToString(szFileAttr);
	
		szFilePath.append(GetDataUnitName());
		szFilePath.append(szFileAttr);
		szFilePath.append(GetDataUnitExt());
		
		

		if(IsCDROM())
		{
			dwCreationDisposition=GENERIC_READ, 
			dwDesiredAccess=OPEN_EXISTING;
			dwShareMode = FILE_SHARE_READ;
		}
		else
		{
			SetFileAttributes(szFilePath.c_str(), FILE_ATTRIBUTE_NORMAL);
			dwCreationDisposition=GENERIC_READ|GENERIC_WRITE, 
			dwDesiredAccess=OPEN_ALWAYS;
			dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
		}
		////открытие и закрытие файла отрубает буферизацию системой, при загрузке процесса
		//m_FHandle=CreateFileW(szFilePath.c_str(),
		//				dwCreationDisposition, 
		//				FILE_SHARE_READ | FILE_SHARE_WRITE, 
		//				NULL,
		//				dwDesiredAccess, 
		//				FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED | FILE_FLAG_NO_BUFFERING,
		//				NULL);
		//CloseHandle(m_FHandle);

		m_FHandle=CreateFileW(szFilePath.c_str(),
						dwCreationDisposition, 
						dwShareMode, 
						NULL,
						dwDesiredAccess, 
						FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
						NULL);

		//CloseHandle(m_FHandle);
	}

	if(IsBad())
	{
		return false;
	}
	
	m_bIsOpen=true;
	return m_bIsOpen;
}

bool	CBasisFile::Close(void)
{
	//ToConsole("CBasisFile::Close");
	//View();

	if(m_FHandle!=INVALID_HANDLE_VALUE){
		bool bRetVal=(CloseHandle(m_FHandle)==TRUE?true:false);
		m_FHandle=INVALID_HANDLE_VALUE;
		return bRetVal;
	}

	m_bIsOpen=false;
	return !m_bIsOpen;
}

unsigned int CBasisFile::GetDataUnitSize()
{
	if(IsBad())	return 0;
	
	LARGE_INTEGER FileSize64;
	if((FileSize64.LowPart=::GetFileSize(m_FHandle,LPDWORD(&FileSize64.HighPart)))!=INVALID_FILE_SIZE){
		return FileSize64.LowPart;
	}else{
		return 0;
	}                 
}

bool CBasisFile::Rename(const wchar_t* pszNewName, const wchar_t* pszPath)
{
	wstring szFileAttr;
	m_BackupSettings.ToString(szFileAttr);

	wstring szOldPath(pszPath);
	szOldPath.append(GetDataUnitName());
	szOldPath.append(szFileAttr);
	szOldPath.append(GetDataUnitExt());

	wstring szNewPath(pszPath);
	szNewPath.append(pszNewName);
	szNewPath.append(szFileAttr);
	szNewPath.append(GetDataUnitExt());
	
	//удаляем существующий
	DeleteFileW(szNewPath.c_str());
	//закрываемся
	Close();
	//переименовываемся
	if(!MoveFileW(szOldPath.c_str(), szNewPath.c_str())){
		TO_CONSOLE(L"MoveFile failed");
	}
	//меняем имя
	m_wszDataUnitName.assign(pszNewName);
	//режим открытия без очищения файла
	//открываемся
	return Open(pszPath);
}

bool	CBasisFile::Flush(void)
{
	return FlushFileBuffers(m_FHandle)==TRUE?true:false;
}

void	CBasisFile::View(void)
{
	wchar_t buff[100];
	swprintf(buff, L"CBasisFile:\n\tName: %s, Size:%u", GetDataUnitName(), GetDataUnitSize());
	ToConsole(buff);
}

void	CBasisFile::ViewFilePos(void)
{
	wchar_t buff[256];
	swprintf(buff, L"CBasisFile position: %u", TellPos());
	ToConsole(buff);
}

void	CBasisFile::Read(unsigned char* pBuffer, unsigned int uiSize)
{
	if(IsBad())	return;
	
	DWORD ReaddenDataLen;
	if(!ReadFile(m_FHandle, pBuffer, uiSize, &ReaddenDataLen, NULL)){
		ERR_TO_CONSOLE(erBadHandle, GetDataUnitName());
	}
}

void	CBasisFile::Write(unsigned char* pBuffer, unsigned int uiSize)
{
	if(IsCDROM())
	{
		TO_CONSOLE(L"Попытка записи в режиме поиска")
		return;
	}

	if(IsBad())	return;

	DWORD WrittenDataLen;
	if(!WriteFile(m_FHandle, pBuffer, uiSize, &WrittenDataLen, NULL)){
		ERR_TO_CONSOLE(erBadHandle, GetDataUnitName());
	}
}

void	CBasisFile::Append(unsigned char* pBuffer, unsigned int uiSize)
{
	if(IsBad())	return;

	CBasisFile::SeekPos(0, SEEK_END);
	CBasisFile::Write(pBuffer, uiSize);
}

void	CBasisFile::SeekPos(unsigned int uiOffset, unsigned int uiOrigin)
{
	if(IsBad())	return;
	
	LARGE_INTEGER FPos64={0};
	long lOffset=(int)uiOffset;
	FPos64.QuadPart=lOffset;
	SetFilePointer(m_FHandle, FPos64.LowPart, &FPos64.HighPart, uiOrigin);
}

unsigned int CBasisFile::TellPos(void)
{
	if(IsBad())	return EMPTY_OFFSET;
	
	LARGE_INTEGER liCurrPosition={0};
	return SetFilePointer(m_FHandle, liCurrPosition.LowPart, NULL, FILE_CURRENT);
}

bool CBasisFile::IsBad(void)
{
	if(BAD_HANDLE(m_FHandle))
	{ 
		wstring wsErrorMessage;
		wchar_t wz[20];
		DWORD dw = GetLastError();
		_itow(dw, wz, 10);
		wsErrorMessage = L"Ошибка открытия файла (Bad file handle!!!):";
		wsErrorMessage += wz;
		wsErrorMessage += L", имя юнита ";
		wsErrorMessage += GetDataUnitName();
		TO_CONSOLE(wsErrorMessage.c_str());	
		return true;
	}else
		return false;
}

void CBasisFile::Reserve(unsigned int uiSize)
{

	if(IsBad())	return;

	unsigned char* pFF=(unsigned char*)malloc(uiSize);
	memset(pFF, 0xff, uiSize);
	CBasisFile::Write(pFF, uiSize);
	free(pFF);		
}

void CBasisFile::Clear(void)
{
	if(IsBad())	return;
	CBasisFile::SeekPos(0, SEEK_SET);
	SetEndOfFile(m_FHandle);
	CBasisFile::Flush();
}

//--------------------------------------------------------------------//

}
}
}
}