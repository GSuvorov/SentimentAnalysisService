#include "stdafx.h"
#include ".\filefinder.h"
using namespace std;

namespace SS
{
namespace Core
{
namespace CommonServices
{
//////////////////////////////////////////////////////////////////////////
//
CFileFinder::CFileFinder(void)
{
}

//////////////////////////////////////////////////////////////////////////
//
CFileFinder::~CFileFinder(void)
{
}


//////////////////////////////////////////////////////////////////////////
//

bool CFileFinder::GetFiles(wstring& sRootPath, wstring& sSubPath, vector<wstring>* vIgnoreExt, vector<wstring>* vRelPaths)
{
	if (vRelPaths == NULL)
		return false;
	
	try
	{
		WIN32_FIND_DATAW FindData;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		wstring s = sRootPath + sSubPath;

		DWORD dwAttr = ::GetFileAttributesW(s.c_str());
		if (dwAttr == INVALID_FILE_ATTRIBUTES)
			return false;

		if ((dwAttr & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
		{
				vRelPaths->push_back(sSubPath);
				return true;
		}
		s.append(L"*");
		hFind = FindFirstFileW(s.c_str(), &FindData);
		if (hFind == INVALID_HANDLE_VALUE) 
			return false;
		do
		{
			if (wcscmp(FindData.cFileName,  L".")==0 || wcscmp(FindData.cFileName, L"..")==0)
				continue;
			if ( (FindData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{			
				s = sSubPath; 
				s.append(FindData.cFileName);
				s.append(L"\\");
				GetFiles(sRootPath, s, vIgnoreExt, vRelPaths);
			}
			else
			{
				wchar_t szExt[_MAX_EXT];
				if (vIgnoreExt!=NULL)
				{
					bool bIgnore = false;
					_wsplitpath (FindData.cFileName, NULL, NULL, NULL, szExt);
					for (size_t i=0; i<vIgnoreExt->size(); i++)
					{
						if (lstrcmpiW(szExt, vIgnoreExt->at(i).c_str()) == 0)
						{
							bIgnore = true;
							break;
						}
					}
					if (bIgnore)
						continue;
				}
				s = sSubPath;
				s.append(FindData.cFileName);
				vRelPaths->push_back(s);
			}
		}
		while(FindNextFileW(hFind, &FindData) != 0);
		FindClose(hFind);
	}
	catch (...)
	{
	}
	return true;
}


HRESULT CFileFinder::QueryInterface(REFIID pIID, void** pBase)
{
	*pBase = NULL;

	if (pIID == IID_FileFinder )
		*pBase = (SS::Interface::Core::CommonServices::IFileFinder*) this;
	else if (pIID == IID_Base)
		*pBase = (SS::Interface::IBase*) this;

	return (*pBase) ? S_OK : S_FALSE;
}; 

ULONG CFileFinder::Release()
{
	delete this;
	return NULL;
};
}
}
}