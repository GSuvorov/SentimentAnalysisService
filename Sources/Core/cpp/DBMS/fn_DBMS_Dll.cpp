#include "StdAfx.h"
#include "..\ASCInterface\IDBMS.h"
#include "fn_DBMS_Dll.h"
#include "cl_Topo_DBMS.h"
// #include "..\ASCInterface\GUID.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "fn_DBMS_Dll.cpp"


SET_LOAD_MANAGER_IMPLEMENTATION;


BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReasonForCall, LPVOID lpReserved)   {

	switch(dwReasonForCall)   {
		case DLL_PROCESS_ATTACH :
		case DLL_THREAD_ATTACH  :
		case DLL_THREAD_DETACH  :
		case DLL_PROCESS_DETACH :
		break;
	}

	return(TRUE);
}

HRESULT CreateInstance(const GUID * pGuid, void ** pBase)   {

	if(*pGuid==CLSID_DBMS)   {
/*
		if(g_pDBManager==NULL)   g_pDBManager = new DBManagerTopoC();
		if(g_pDBManager)   {
			*pBase=(SS::Interface::IBase *)g_pDBManager;
			return(S_OK);
		} else   {
			*pBase=NULL;
			return(LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,"CreateInstance()Failed new()"));
		}
*/
		#pragma warning( disable : 4291)
		DBManagerTopoC * pDBManager = new DBManagerTopoC();
		if(pDBManager)   {
			*pBase=(SS::Interface::IBase *)pDBManager;
			return(S_OK);
		} else   {
			*pBase=NULL;
			return(LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("CreateInstance()Failed new()")));
		}
		#pragma warning( default : 4291 )

	} else   {
		*pBase=NULL;
		return(LogFail(_F,ERROR_NOT_SUPPORTED,_T("CreateInstance()Unsupported GUID")));
	}
}

//HRESULT DBaseTopoC::QueryInterface(REFIID pIID, VOID ** pBase)   {
//	VISIT_CRIT_SECT(m_oTopoCriticalSection);
//
//	if(pIID==IID_DBManager)   {
//		*pBase=(SS::Interface::Core::DBMS::IDBManager *)m_ParentPtr;
//		return(S_OK);
//	} else if(pIID==IID_DataBase)   {
//		*pBase=(SS::Interface::Core::DBMS::IDataBase *)this;
//		return(S_OK);
//	} else if(pIID==IID_Base)   {
//		*pBase=(SS::Interface::IBase *)this;
//		return(S_OK);
//	} else   {
//		*pBase=NULL;
//		return(LogFail(_F,ERROR_NOT_SUPPORTED,_T("Request for unsupported interface")));
//	}
//}

