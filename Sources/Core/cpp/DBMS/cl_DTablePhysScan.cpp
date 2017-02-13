#include "StdAfx.h"
#include "cl_Topo_DBMS.h"
#include "cl_DTablePhys.h"
#include "fn_HashFunc.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_DTablePhysScan.cpp"

HRESULT DTablePhysC::ScanInit(UINT32 nStartRecordIdx, VOID * ppArrayOfPointersToData[], WORD nScanId /* = 0*/)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(nScanId>=SCAN_LIM)      return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Scan ID <%d> is out of range. DataTable <%s>"),nScanId,m_Path.c_str()));

	if(ppArrayOfPointersToData==NULL)   return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("ppArrayOfPointersToData==NULL")));

	m_Scan_ppArrayOfPointersToData[nScanId] = ppArrayOfPointersToData;
	return(ScanStart(nStartRecordIdx,nScanId));
}

BOOL DTablePhysC::GetFreeScanId(WORD * pnScanId)   {
	for(int i=0; i<SCAN_LIM; i++)   {
		if(m_Scan_ppArrayOfPointersToData[i]==NULL)   {
			*pnScanId=i;
			return(TRUE);
		}
	}

	return(FALSE);
}


HRESULT DTablePhysC::ScanInitEx(UINT32 nStartRecordIdx, VOID * ppArrayOfPointersToData[], WORD * pnScanId)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())     return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(pnScanId==NULL)           return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Pointer to <nScanId> is NULL (DataTable <%s>)"),m_Path.c_str()));

	if(!GetFreeScanId(pnScanId)) return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("No free <ScanId> (DataTable <%s>)"),m_Path.c_str()));

	return(ScanInit(nStartRecordIdx,ppArrayOfPointersToData,*pnScanId));

}

HRESULT DTablePhysC::ScanCancel(WORD nScanId /*= 0*/)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,    _T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(nScanId>=SCAN_LIM)      return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Scan ID <%d> is out of range. DataTable <%s>"),nScanId,m_Path.c_str()));

	m_Scan_ppArrayOfPointersToData[nScanId] = NULL;
	return(S_OK);
}


HRESULT DTablePhysC::ScanStart(UINT32 nStartRecordIdx, WORD nScanId /* = 0*/)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(nScanId>=SCAN_LIM)      return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Scan ID <%d> is out of range. DataTable <%s>"),nScanId,m_Path.c_str()));

	if(m_Scan_ppArrayOfPointersToData[nScanId]==NULL)   return(LogFail(_F,ERROR_NOT_READY,_T("Method ScanInit() was not called. DataTable <%s>"),m_Path.c_str()));
	if((m_Scan_RecordIdx[nScanId]=(UINT32)nStartRecordIdx)>m_RecordsNum)   return(S_FALSE);
	return(S_OK);
}

HRESULT DTablePhysC::Scan(WORD nScanId /* = 0*/)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(nScanId>=SCAN_LIM)      return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Scan ID <%d> is out of range. DataTable <%s>"),nScanId,m_Path.c_str()));

	HRESULT HR;

	if(m_Scan_ppArrayOfPointersToData[nScanId]==NULL)   return(LogFail(_F,ERROR_NOT_READY,_T("Method ScanInit(ScanId:%ld) was not called. DataTable <%s>"),nScanId,m_Path.c_str()));

	while(1)   {

		if(m_Scan_RecordIdx[nScanId]>=m_RecordsNum)   return(S_FALSE);

		if((HR=fR.ReadAt(m_Scan_RecordIdx[nScanId]*m_RecordSize,m_RecordArr,m_RecordSize))!=S_OK)   {
			return(LogFail(_F,HR,_T("Record reading error. DataTable <%s>"),m_Path.c_str()));
		}

		if(((BYTE *)m_RecordArr)[m_RecordSize-1] == 0x00)   { // Extra byte
			if((HR=GetRecordData(m_Scan_ppArrayOfPointersToData[nScanId], nScanId, FALSE ))!=S_OK)   {
				return(LogFail(_F,HR,_T("Data reading error. DataTable <%s>"),m_Path.c_str()));
			} else   {
				m_Scan_RecordIdx[nScanId]++;
				return(S_OK);
			}
		} else   {
			m_Scan_RecordIdx[nScanId]++;
		}
	}
}

UINT32 DTablePhysC::GetScanedIdx(WORD nScanId /* = 0*/)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(nScanId>=SCAN_LIM)      return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Scan ID <%d> is out of range. DataTable <%s>"),nScanId,m_Path.c_str()));

	return(m_Scan_RecordIdx[nScanId]-1);
}

HRESULT DTablePhysC::ScanAndChange(WORD nScanId /* = 0*/)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(nScanId>=SCAN_LIM)      return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Scan ID <%d> is out of range. DataTable <%s>"),nScanId,m_Path.c_str()));

	HRESULT HR;

	if(m_Change_UserDataPointers==NULL)   return(LogFail(_F,ERROR_NOT_READY,_T("Method ChangeInit() was not called. DataTable <%s>"),m_Path.c_str()));

	while(1)   {

		if(m_Scan_RecordIdx[nScanId]>=m_RecordsNum)   return(S_FALSE);

		if((HR=fR.ReadAt(m_Scan_RecordIdx[nScanId]*m_RecordSize,m_RecordArr,m_RecordSize))!=S_OK)   {
			return(LogFail(_F,HR,_T("Record reading error. DataTable <%s>"),m_Path.c_str()));
		}

		if(((BYTE *)m_RecordArr)[m_RecordSize-1] == 0x00)   { // Extra byte
			if((HR=SetRecordData(m_Change_UserDataPointers, nScanId, FALSE))!=S_OK)   {
				return(LogFail(_F,HR,_T("Data changing error. DataTable <%s>"),m_Path.c_str()));
			} else   {
				m_Scan_RecordIdx[nScanId]++;
				return(S_OK);
			}
		} else   {
			m_Scan_RecordIdx[nScanId]++;
		}
	}
}


HRESULT DTablePhysC::ScanOne(UINT32 nRecordIdx, WORD nScanId /* = 0*/)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(nScanId>=SCAN_LIM)      return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Scan ID <%d> is out of range. DataTable <%s>"),nScanId,m_Path.c_str()));

	HRESULT HR;
	if((HR=ScanStart(nRecordIdx))!=S_OK)   return(HR);
	return(Scan());
}

HRESULT DTablePhysC::ScanOneAndChange(UINT32 nRecordIdx, WORD nScanId /* = 0*/)   {
	VISIT_CRIT_SECT(m_oCriticalSection);
	if(!m_TopoPtr->IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("Operation was not perfomed because DataTable <%s> is closed"),m_Path.c_str()));
	if(nScanId>=SCAN_LIM)      return(LogFail(_F,ERROR_INVALID_PARAMETER,_T("Scan ID <%d> is out of range. DataTable <%s>"),nScanId,m_Path.c_str()));

	HRESULT HR;
	if((HR=ScanStart(nRecordIdx))!=S_OK)   return(HR);
	return(ScanAndChange());
}

