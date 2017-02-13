// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "WorkRegister.h"
#include <algorithm>

#include "shlobj.h"
#include "shlwapi.h"

const wchar_t* c_szIsNoKeyExist = L"NO_KEY_EXIST";
const int c_iSizeBuffer = 512;
const wchar_t* c_szINIFileName = L"ss.ini";

using namespace std;

namespace SS
{
namespace Core
{
namespace CommonServices
{

CWorkRegister::CWorkRegister() : csPath(L"SOFTWARE\\SS\\SS")
{
	m_blParametersHadBeenLoaded = false;
}

CWorkRegister::~CWorkRegister()
{
}

///пересчитает при необходимости из относительного пути в нормальный
void CWorkRegister::UpdateForRelativePath(wstring* pString, const wchar_t* wszPath)
{

	wchar_t* sz = (wchar_t*)pString->c_str();
	if(sz[0] != '.' && sz[0] != '\\' && sz[0] != '/') return;

	wstring sPath = wszPath;

	wstring sNew;

	if(sz[0] == '.' && (sz[1] == '\\' || sz[1] == '/')){
		//тогда путь внутри текущей директории
		sNew = sPath + pString->substr(1, pString->length() - 1).c_str();
	}
	else if((sz[0] == '\\' || sz[0] == '/') && (sz[1] == '\\' || sz[1] == '/')){
		//тогда путь внутри текущей директории
		sNew = sPath + pString->c_str();
	}
	else if(sz[0] == '.' && sz[1] == '.' && (sz[2] == '\\' || sz[2] == '/')){
		//теперь считаем кол-во ходов
		unsigned int n = 1;
		unsigned int i = 3;

		while(sz[i] != 0){
			while(sz[i] == '\\' || sz[i] == '/'){
				i++;
			}
			if(sz[i] == '.' && sz[i + 1] == '.' && (sz[i + 2] == '\\' || sz[i + 2] == '/')){
				i = i + 3;
				n++;
			}
			else{
				break;
			}
		}

		//теперь собираем конечный путь (он состоит из исходной строки с позиции i при бавленной к текущему пути,
		// рубленному на n директорий

		unsigned int ii = (unsigned int )(sPath.length() - 1);
		while(sPath.c_str()[ii] == '\\' || sPath.c_str()[ii] == '/'){
			if(ii == 0) break;
			ii--;
		}
		while(n && ii){
			if(sPath.c_str()[ii] == '\\' || sPath.c_str()[ii] == '/'){
				n--;
				while(sPath.c_str()[ii] == '\\' || sPath.c_str()[ii] == '/'){
					if(ii == 0) break;
					ii--;
				}
			}
			else{
				ii--;
			}
		}

		sNew = sPath.substr(0 ,ii + 1) + L'\\' + pString->substr(i, pString->length() - i);

	}
	else{
		//тогда путь внутри текущей директории, но без слэша
		sNew = sPath + L'\\' + pString->c_str();
	}

	if(sNew.c_str()[sNew.length() - 1] != L'\\' && sNew.c_str()[sNew.length() - 1] != L'/'){
		sNew+=L'\\';
	}
	(*pString) = sNew;
	//MessageBox(NULL,"outstr", pString->c_str(), 0);
}

wstring CWorkRegister::GetParameter(wstring sParametrName, const wchar_t* wszPath)
{
	CCriticalSection oCriticalSection(&m_CriticalSection);		
	CheckAndLoadParameters(wszPath);

	//by viellsky
	try
    {
		if(sParametrName == L"PathDataBase")
			return m_sPathDataBase;
		else if(sParametrName == L"DataSource")
			return m_sDataSource;
		else if(sParametrName == L"PathWordsystem")
			return m_sPathWordsystem;
		else if(sParametrName == L"Password")
			return m_sPassword;
		else if(sParametrName == L"PathCore")
			return m_sPathCore;
		else if(sParametrName == L"WorkPath")
			return m_sWorkPath;	
		else if(sParametrName == L"PathTemporaryFiles")
			return m_sPathTemporaryFiles;
		else if(sParametrName == L"Server")
			return m_sServer;
		else if(sParametrName == L"DocPluginsPath")
			return m_sDocPluginsPath;
		else
		{
			wstring sRet = LoadParameterFromINIFile(sParametrName, wszPath);
			if (sRet.length() == NULL)
				sRet = LoadParameter(sParametrName);

			if (sRet.length() != NULL)
				UpdateForRelativePath(&sRet, wszPath);

			return sRet;
		}
	}
	//by viellsky
	catch(...){
		throw;
	}
	return L"";
}


void CWorkRegister::SetWorkDir(std::wstring wPathToDir)
{
	CCriticalSection oCriticalSection(&m_CriticalSection);		
	try
	{
		SetCurrentDirectoryW(wPathToDir.c_str());
		
		m_blParametersHadBeenLoaded = false;
		CheckAndLoadParameters(wPathToDir.c_str());

	}
	catch(...)
	{
		throw;
	}
}


void CWorkRegister::SaveParameter(wstring sNameParameter, wstring sValue)
{
	CreateNewKey(HKEY_LOCAL_MACHINE, csPath);
	SetKeyValue(HKEY_LOCAL_MACHINE, csPath, sNameParameter, sValue, REG_SZ);
}

wstring CWorkRegister::LoadParameter(wstring sNameParameter)
{
	CreateNewKey(HKEY_LOCAL_MACHINE, csPath);
	return QueryValue(HKEY_LOCAL_MACHINE, csPath, sNameParameter);
}

// ¬озвращает значени€ записанные в ключе
wstring CWorkRegister::QueryValue(HKEY hPredefinedKey, wstring sKeyName, wstring sValueName)
{
	DWORD dwSize;
	DWORD dwType;
	HKEY hKey;
	wstring sValue;
	wchar_t szVal[500] = { 0 };	
	dwSize = sizeof(szVal);
	RegOpenKeyEx(hPredefinedKey, sKeyName.c_str(), 0, KEY_READ, &hKey);
	RegQueryValueEx(hKey, sValueName.c_str(), NULL, &dwType, (BYTE*)szVal, &dwSize);
	RegCloseKey (hKey);
	sValue = szVal;
	return sValue;
}



//'—оздание нового ключа (подключа)
void CWorkRegister::CreateNewKey(HKEY hPredefinedKey, wstring sNewKeyName)
{
	HKEY hNewKey = NULL;
	long lErr = RegCreateKey(hPredefinedKey, sNewKeyName.c_str(), &hNewKey);
	if (lErr == ERROR_SUCCESS) RegCloseKey(hNewKey);
}


//' «апись данных в ключ
void CWorkRegister::SetKeyValue(HKEY hPredefinedKey, wstring sKeyName,
								wstring sValueName, wstring sValue, long lValueType)
{
	HKEY hKey; 
	RegOpenKeyEx(hPredefinedKey, sKeyName.c_str(), 0, KEY_ALL_ACCESS, &hKey);
	RegSetValueEx(hKey, sValueName.c_str(), NULL, lValueType, (BYTE*)sValue.c_str(), (DWORD)(sValue.size()+1));
	RegCloseKey(hKey);
}

wstring	CWorkRegister::LoadParameterFromINIFile(wstring sNameParameter, const wchar_t* wszPath)
{
	if(sNameParameter == L"PathTemporaryFiles")
	{
		TCHAR szAppData[MAX_PATH];
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szAppData);

		if(SUCCEEDED(hr))
		{
			PathAppend(szAppData, L"SS\\SS");
			PathAppend(szAppData, sNameParameter.c_str());
			wcscat(szAppData, L"\\");
			return szAppData;
		}				
	}
	else
	{
		wchar_t szReturnedString[512];
		wstring sPathINIFile = wszPath;
		sPathINIFile += L"\\";
		sPathINIFile += c_szINIFileName;

		::GetPrivateProfileString(L"SS", sNameParameter.c_str(), c_szIsNoKeyExist, szReturnedString, c_iSizeBuffer, sPathINIFile.c_str());

		//если в текущей директории ss.ini нет, пытаемс€ найти его в рабочей папке

		if(!wcscmp(szReturnedString, c_szIsNoKeyExist))
		{

			sPathINIFile = LoadParameter(L"WorkPath");
			sPathINIFile += c_szINIFileName;
			::GetPrivateProfileString(L"SS", sNameParameter.c_str(), c_szIsNoKeyExist, szReturnedString, c_iSizeBuffer, sPathINIFile.c_str());
		}
		return (!wcscmp(szReturnedString, c_szIsNoKeyExist)) ? 
			L"" : szReturnedString;
	}

	return L"";
};

void CWorkRegister::CheckAndLoadParameters(const wchar_t* wszPath)
{
	if (m_blParametersHadBeenLoaded)
		return;
	
	m_sPathDataBase = LoadParameterFromINIFile(L"PathDataBase", wszPath);
	if(m_sPathDataBase.length() == NULL)
	{
		m_sPathDataBase = LoadParameter(L"PathDataBase");
		m_sPathWordsystem = LoadParameter(L"PathWordsystem");
		m_sPassword=LoadParameter(L"Password");
		m_sPathCore=LoadParameter(L"PathCore");
		m_sWorkPath=LoadParameter(L"WorkPath");
		m_sPathTemporaryFiles=LoadParameter(L"PathTemporaryFiles");

		m_sDataSource=LoadParameter(L"DataSource");
		m_sServer=LoadParameter(L"Server");
	}
	else
	{
		m_sPathWordsystem = LoadParameterFromINIFile(L"PathWordsystem", wszPath);
		m_sPassword=LoadParameterFromINIFile(L"Password", wszPath);
		m_sPathCore=LoadParameterFromINIFile(L"PathCore", wszPath);
		m_sWorkPath=LoadParameterFromINIFile(L"WorkPath", wszPath);
		m_sPathTemporaryFiles=LoadParameterFromINIFile(L"PathTemporaryFiles", wszPath);	
		m_sDataSource=LoadParameterFromINIFile(L"DataSource", wszPath);
		m_sServer=LoadParameterFromINIFile(L"Server", wszPath);
		m_sDocPluginsPath = LoadParameterFromINIFile(L"DocPluginsPath", wszPath);
	}

	UpdateForRelativePath(&m_sPathDataBase, wszPath);
	UpdateForRelativePath(&m_sPathWordsystem, wszPath);
	UpdateForRelativePath(&m_sPathCore, wszPath);
	UpdateForRelativePath(&m_sWorkPath, wszPath);
	UpdateForRelativePath(&m_sPathTemporaryFiles, wszPath);
	UpdateForRelativePath(&m_sDocPluginsPath, wszPath);

	m_blParametersHadBeenLoaded = true;
}

}
}
}