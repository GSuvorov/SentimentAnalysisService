#include "StdAfx.h"
#include ".\connection.h"
#include <sqloledb.h>

namespace SS
{
namespace Core
{
namespace ResourceManagers
{


CConnection::CConnection(void) : m_IsOpen(false)
{
}

CConnection::~CConnection(void)
{
}

bool CConnection::Open(wchar_t* wszName, wchar_t* wszPassword, wchar_t* wszLocation, wchar_t* wszDataSource)
{

	HRESULT		hr;
	CDBPropSet	dbinit(DBPROPSET_DBINIT);

	if(wcslen(wszLocation)==0)
		return false;

	m_currentDB.Close();
	dbinit.AddProperty(DBPROP_AUTH_PASSWORD, wszPassword);
	dbinit.AddProperty(DBPROP_AUTH_PERSIST_SENSITIVE_AUTHINFO, false);
	dbinit.AddProperty(DBPROP_AUTH_USERID, wszName);
	dbinit.AddProperty(DBPROP_INIT_CATALOG, wszLocation);
	dbinit.AddProperty(DBPROP_INIT_LOCATION, wszLocation);
	dbinit.AddProperty(DBPROP_INIT_DATASOURCE, wszDataSource);  
	dbinit.AddProperty(DBPROP_INIT_LCID, (long)1049);
	dbinit.AddProperty(DBPROP_INIT_PROMPT, (short)4);
	dbinit.AddProperty(SSPROP_ENABLEFASTLOAD, true);

	hr = m_currentDB.Open(_T("SQLOLEDB.1"), &dbinit);

	SAVE_LOG(SS_MESSAGE AND L"Открываем базу " AND wszLocation AND L"; password=" AND wszPassword AND L";user=" AND wszName AND L"server=" AND wszDataSource);
	if (hr!=S_OK)
	{
		SAVE_LOG(SS_MESSAGE AND L"База " AND wszLocation AND L" не открывается");
		return false;
	};

	if(m_currentSession.Open(m_currentDB)==S_OK)
	{
		m_IsOpen=true;
		return m_IsOpen;
	}
	else return false;
};

void CConnection::Close()
{

	m_currentSession.Close();
	m_currentDB.Close();
	m_IsOpen=false;
};

bool CConnection::IsOpen()
{

	return m_IsOpen;
};

ATL::CSession* CConnection::GetSession()
{
	
//	SAVE_LOG(SS_MESSAGE, "CConnection::GetSession()", NULL);
	return &m_currentSession;
};

bool CConnection::ReOpenSession()
{
	
	m_currentSession.Close();
	m_IsOpen=false;

	if(m_currentSession.Open(m_currentDB)==S_OK)
	{
		m_IsOpen=true;
		return m_IsOpen;
	}
	else return false;
};

int CConnection::CountSessionRef()
{
	int iCountRef=NULL;
	if(m_currentSession.m_spOpenRowset.p)
	{
		iCountRef = m_currentSession.m_spOpenRowset.p->AddRef();
		m_currentSession.m_spOpenRowset.p->Release();
		return --iCountRef;
	}
	else return NULL;
};

}
}
}