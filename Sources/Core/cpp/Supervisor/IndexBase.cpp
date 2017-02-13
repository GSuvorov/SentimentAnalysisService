#include "stdafx.h"
#include "IndexBase.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "BasesList.h"


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CIndexBase::CIndexBase(CBasesList* pParent, INdxBaseManager* pNdxBaseManager, IBaseManager* pBaseManager,
					   LPCWSTR szDatabasePath, LPCWSTR szName) :
		m_pParent(pParent),
		m_pNdxBaseManager(pNdxBaseManager),
		m_pBaseManager(pBaseManager),
		m_strDatabasePath(szDatabasePath),
		m_strName(szName),
		m_bKnBase(false)
{
}

CIndexBase::~CIndexBase()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CIndexBase interface

void CIndexBase::Init(LPCWSTR szDescr, BOOL bKnBase)
{
	SS_TRY;

	m_strDescr = szDescr;
	m_bKnBase = bKnBase;

	SS_CATCH(L"");
}

bool CIndexBase::Open()
{
	SS_TRY;

	m_pParent->OnDatabaseOpen(this);

	m_pBaseManager->SetBasePath(m_strDatabasePath.c_str());
	if(FAILED(m_pBaseManager->OpenBase(m_strName.c_str())))
		return false;

	m_pNdxBaseManager->SetBasePath(m_strDatabasePath.c_str());
	if(FAILED(m_pNdxBaseManager->OpenBase(m_strName.c_str())))
		return false;

	SS_CATCH(L"");
	return true;
}

void CIndexBase::Close()
{
	SS_TRY;

	if(m_pParent->OnDatabaseClose(this))
	{
		m_pBaseManager->CloseBase();
		m_pNdxBaseManager->CloseBase();
	}

	SS_CATCH(L"");
}

void CIndexBase::Remove()
{
	SS_TRY;

	wchar_t szPath[MAX_PATH];
	wcscpy(szPath, m_strDatabasePath.c_str());
	PathAppendW(szPath, m_strName.c_str());

	RemoveDirectoryW(szPath);
	m_pParent->OnDatabaseRemoved(this);

	SS_CATCH(L"");
}

LPCWSTR CIndexBase::GetPath()
{
	SS_TRY;

	//---COMMENTED--- wchar_t szPath[MAX_PATH];
	wchar_t* szPath = new wchar_t[ MAX_PATH ];
	wcscpy(szPath, m_strDatabasePath.c_str());
	PathAppendW(szPath, m_strName.c_str());
	return szPath;

	SS_CATCH(L"");
}

LPCWSTR CIndexBase::GetName()
{
	return m_strName.c_str();
}

LPCWSTR CIndexBase::GetDescription()
{
	return m_strDescr.c_str();
}

bool CIndexBase::SetDescription(LPCWSTR szDescription)
{
	SS_TRY;

	wchar_t szIni[MAX_PATH];
	wcscpy(szIni, m_strDatabasePath.c_str());
	PathAppendW(szIni, m_strName.c_str());
	PathAppendW(szIni, L"ind.ini");

	if(WritePrivateProfileString(L"Properties", L"Description", szDescription, szIni))
	{
		m_strDescr = szDescription;
		return true;
	}

	SS_CATCH(L"");
	return false;
}

BOOL CIndexBase::IsKnowledgeBase()
{
	return m_bKnBase;
}

bool CIndexBase::SetKnowledgeBase(BOOL bSet)
{
	SS_TRY;

	wchar_t sz[16], szIni[MAX_PATH];
	wsprintf(sz, L"%d", bSet ? 1 : 0);

	wcscpy(szIni, m_strDatabasePath.c_str());
	PathAppendW(szIni, m_strName.c_str());
	PathAppendW(szIni, L"ind.ini");

	if(WritePrivateProfileString(L"Properties", L"KnBase", sz, szIni))
	{
		m_bKnBase = bSet;
		return true;
	}

	SS_CATCH(L"");
	return false;
}
