#pragma once

#include <tchar.h>


namespace SS
{
namespace Core
{
namespace BlackBox
{
namespace XML
{

class CFileSaver
{
public:
	CFileSaver() : m_hFile(INVALID_HANDLE_VALUE)
	{
	}

	~CFileSaver()
	{
		if(m_hFile != INVALID_HANDLE_VALUE)
			CloseHandle(m_hFile);
	}

public:
	void Close()
	{
		if(m_hFile != INVALID_HANDLE_VALUE)
			CloseHandle(m_hFile);

		m_hFile = INVALID_HANDLE_VALUE;
	}

	bool Create(LPCTSTR szFile, bool bRewrite = true)
	{
		Close();

		m_hFile = CreateFile(szFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, 
			bRewrite ? CREATE_ALWAYS : OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(m_hFile == INVALID_HANDLE_VALUE)
			return false;

		if(!bRewrite)
			SetFilePointer(m_hFile, 0, 0, FILE_END);

#ifdef UNICODE
		// create UNICODE text file
		USHORT shBuf = 0xfeff;
		DWORD dw;
		WriteFile(m_hFile, &shBuf, 2, &dw, NULL);
#endif

		return true;
	}

	bool WriteLine(LPCTSTR szLine)
	{
		DWORD dw;
		if(!WriteFile(m_hFile, szLine, (DWORD) _tcslen(szLine) * sizeof(TCHAR), &dw, NULL))
			return false;

		WriteFile(m_hFile, _T("\r\n"), 2 * sizeof(TCHAR), &dw, NULL);
		return true;
	}

	bool WriteString(LPCTSTR szString)
	{
		DWORD dw;
		if(!WriteFile(m_hFile, szString, (DWORD) _tcslen(szString) * sizeof(TCHAR), &dw, NULL))
			return false;

		return true;
	}

private:
	HANDLE m_hFile;
};

}
}
}
}
