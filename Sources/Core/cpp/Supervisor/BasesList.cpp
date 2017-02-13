#define RPC_USE_NATIVE_WCHAR
#include "stdafx.h"
#include "BasesList.h"

#include <rpc.h>
#pragma comment(lib, "Rpcrt4.lib")

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CBasesList::CBasesList() :
		m_pWorkRegister(NULL),
		m_pNdxBaseManager(NULL),
		m_pBaseManager(NULL),
		m_pCurrBase(NULL)
{
}

CBasesList::~CBasesList()
{
	for(TBasesCollection::iterator it = m_vBases.begin(); it != m_vBases.end(); ++it)
		delete *it;
	m_vBases.erase(m_vBases.begin(), m_vBases.end());
}

/////////////////////////////////////////////////////////////////////////////////////
// IDatabaseList interface

void CBasesList::SetLoadManager(ILoadManager* pLoadManager)
{
	SS_TRY;

	CBaseCoreClass::SetLoadManager(pLoadManager);

	// base managers
	m_pNdxBaseManager = (INdxBaseManager*) 
		GetLoadManager()->GetInterface(L"./core/NdxBaseManager.dll", CLSID_NdxBaseManager, IID_NdxBaseManager);
	m_pBaseManager = (IBaseManager*) 
		GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_BaseManager);

	// database path
	m_pWorkRegister = (IWorkRegister*)
		GetLoadManager()->GetInterface(L"./core/LoadManager.dll", CLSID_WorkRegister, IID_WorkRegister);
	m_strDatabasePath = m_pWorkRegister->GetParameter(L"PathDataBase");

	SS_CATCH(L"");
}

IIndexBase* CBasesList::GetFirstBase()
{
	SS_TRY;

	m_itCurrBase = m_vBases.begin();
	if(m_itCurrBase != m_vBases.end())
		return *m_itCurrBase;

	SS_CATCH(L"");
	return NULL;
}

IIndexBase* CBasesList::GetNextBase()
{
	SS_TRY;

	if(++m_itCurrBase == m_vBases.end())
		return *m_itCurrBase;

	SS_CATCH(L"");
	return NULL;
}

IIndexBase* CBasesList::FindBase(LPCWSTR szName)
{
	SS_TRY;

	for(TBasesCollection::iterator it = m_vBases.begin(); it != m_vBases.end(); ++it)
	{
		if(wcscmp((*it)->GetDescription(), szName) == 0)
			return *it;
	}

	SS_CATCH(L"");
	return NULL;
}

IIndexBase* CBasesList::CreateBase(LPCWSTR szDescription)
{
	SS_TRY;

	// get unique database name
	UUID guid;
	UuidCreate(&guid);
	wchar_t szName[64] = { 0 },
			*szGuid;
	if(UuidToStringW(&guid, (wchar_t**) &szGuid) != RPC_S_OK)
		return NULL;
	swprintf(szName, L"Ind%s", szGuid);
	RpcStringFreeW(&szGuid);

	// create database directory
	wchar_t szDir[MAX_PATH] = { 0 };
	wcscpy(szDir, m_strDatabasePath.c_str());
	PathAppendW(szDir, szName);
	CreateDirectoryW(szDir, NULL);

	// create new IndexBase object
	CIndexBase* pBase = new CIndexBase(
		this, m_pNdxBaseManager, m_pBaseManager, m_strDatabasePath.c_str(), szName);
	m_vBases.push_back(pBase);
	pBase->SetDescription(szDescription);
	pBase->SetKnowledgeBase(FALSE);
	pBase->Open();
	return pBase;

	SS_CATCH(L"");
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////
// CBasesList callback functions

void CBasesList::OnDatabaseOpen(CIndexBase* pBase)
{
	if(m_pCurrBase)
		m_pCurrBase->Close();
	m_pCurrBase = pBase;
}

bool CBasesList::OnDatabaseClose(CIndexBase* pBase)
{
	if(pBase != m_pCurrBase)
		return false;
	m_pCurrBase = NULL;
	return true;
}

void CBasesList::OnDatabaseRemoved(CIndexBase* pBase)
{
	if(pBase == m_pCurrBase)
		m_pCurrBase = NULL;

	for(TBasesCollection::iterator it = m_vBases.begin(); it != m_vBases.end(); ++it)
	{
		if(*it == pBase)
		{
			m_vBases.erase(it);
			delete *it;
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// CBasesList interface

void CBasesList::Clear()
{
	SS_TRY;

	if(m_pCurrBase)
		m_pCurrBase->Close();
	m_pCurrBase = NULL;

	for(TBasesCollection::iterator it = m_vBases.begin(); it != m_vBases.end(); ++it)
		delete *it;
	m_vBases.erase(m_vBases.begin(), m_vBases.end());

	SS_CATCH(L"");
}

void CBasesList::FillDatabaseList()
{
	SS_TRY;

	Clear();
	WIN32_FIND_DATAW oData;

	HANDLE hFind = FindFirstFileW((m_strDatabasePath + L"*").c_str(), &oData);
	for(BOOL b = (hFind != INVALID_HANDLE_VALUE); b; b = FindNextFileW(hFind, &oData))
	{
		if(!(oData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && oData.cFileName[0] != L'.'))
			continue;

		wstring strInternalDir = oData.cFileName;

		HANDLE h = FindFirstFileW((m_strDatabasePath + strInternalDir + L"\\ind.ini").c_str(), &oData);
		if(h != INVALID_HANDLE_VALUE)
		{
			auto_ptr<CIndexBase> pBase(new CIndexBase
				(this, m_pNdxBaseManager, m_pBaseManager, m_strDatabasePath.c_str(), strInternalDir.c_str()));
			GetDatabaseParams(m_strDatabasePath.c_str(), strInternalDir.c_str(), (LPCWSTR) oData.cFileName, pBase.get());
			m_vBases.push_back(pBase.release());
		}

		FindClose(h);
	}

	FindClose(hFind);

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// CBasesList functions

void CBasesList::GetDatabaseParams(LPCWSTR szBaseDir, LPCWSTR szDir, LPCWSTR szFileName, CIndexBase* pBase)
{
	SS_TRY;

	WCHAR szIniFile[MAX_PATH];
	swprintf(szIniFile, L"%s%s\\%s", szBaseDir, szDir, szFileName);

	WCHAR szBuff[256];
	int iCount = GetPrivateProfileStringW(L"Properties", L"Description", L"", szBuff, 256, szIniFile);
	if(iCount > 0)
	{
		BOOL bKnBase = GetPrivateProfileIntW(L"Properties", L"KnBase", 0, szIniFile);
		pBase->Init(szBuff, bKnBase);
	}

	SS_CATCH(L"");
}
