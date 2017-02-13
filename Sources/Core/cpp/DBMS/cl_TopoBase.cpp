#include "StdAfx.h"
#include "cl_Topo_DBMS.h"
#include "..\ASCInterface\IDBMS.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_DBaseTopo.cpp"

//ULONG DBaseTopoC::Release()   {
//	// VISIT_CRIT_SECT(m_oTopoCriticalSection);
//	Close();
//	// delete this;
//	return(NULL);
//}

IDataTable * DBaseTopoC::OpenDataTable(VOID)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);

	if(!IsOpen())   {
		LogFail(_F,ERROR_ACCESS_DENIED,_T("DataBase <%s> is closed"),m_Name.c_str());
		return(NULL);
	}

	DTableTopoC * pDT;
	HRESULT        HR;

	if((HR=GetChild(&pDT))==S_OK)   {
		if((HR=pDT->Open())==S_OK)   {
			return(static_cast<SS::Interface::Core::DBMS::IDataTable *>(pDT));
		} else  {
			LogFail(_F,HR,_T("DataTable <%s> was not opened"),pDT->m_Name.c_str());
		}
	} else  {
		LogFail(_F,HR,_T("Pointer for DataTable N %ld was not obtained"),m_ChildCurrent);
	}

	return(NULL);
}

HRESULT DBaseTopoC::Close(VOID)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);
	HRESULT       HR=S_OK;

	if(m_ReferenceCounter==1)   {
		DTableTopoC * pTable;
		for(int i=0; i<m_ChildrenNum; i++)   {
			if((pTable=FindChild(i))==NULL)   {
				HR=LogFail(_F,ERROR_INVALID_ACCESS,_T("DataTable N %ld was not found"),i);
			} else   {
				if(pTable->IsOpen())   {
					LogMess(_T("DataTable <%s> was not closed in proper time (Before DataBase<%s>::Close())"),pTable->m_Name.c_str(),m_Name.c_str());
					if(pTable->Close()!=S_OK)  LogFail(_F,0,_T("Unsuccessful try to close DataTable <%s>"),pTable->m_Name.c_str());
				} // else - OK - Table is closed
			}
		}
		m_ReferenceCounter=0;
	} else   {
		if(m_ReferenceCounter==0)   {
			LogMess(_T("The excess try to close DataBase <%s> (Reference_Counter is zero already)"),m_Name.c_str());
		} else   {
			m_ReferenceCounter--;
		}
	}

	return(HR);
}

HRESULT DBaseTopoC::Clear(BOOL bClearUserInitializations /* = TRUE */)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);

	// if(!IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("DataBase <%s> is closed"),    m_Name.c_str()));
	// if( IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("DataBase <%s> is not closed"),m_Name.c_str()));

	DTableTopoC * pTable;
	HRESULT       HR=S_OK;

	for(int i=0; i<m_ChildrenNum; i++)   {
		if((pTable=FindChild(i))==NULL)   {
			HR=LogFail(_F,ERROR_INVALID_ACCESS,_T("DataTable N %ld was not found"),i);
		} else   {
			if((HR=pTable->Clear(bClearUserInitializations))!=S_OK)  LogFail(_F,HR,_T("DataTable N %ld was not cleared"),i);
		}
	}

	return(HR);
}

HRESULT DBaseTopoC::TryClear(VOID)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);

	// if(!IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("DataBase <%s> is closed"),    m_Name.c_str()));
	// if( IsOpen())   return(LogFail(_F,ERROR_ACCESS_DENIED,_T("DataBase <%s> is not closed"),m_Name.c_str()));

	DTableTopoC * pTable;
	HRESULT       HR=S_OK;
	BOOL          bExistOpen = FALSE;

	for(int i=0; i<m_ChildrenNum; i++)   {
		if((pTable=FindChild(i))!=NULL)   {
			if(pTable->IsOpen())   {
				bExistOpen = TRUE;
				break;
			}
		} else   {
			HR=LogFail(_F,ERROR_INVALID_ACCESS,_T("DataTable N %ld was not found"),i);
		}
	}

	if(bExistOpen)   {
		HR = S_FALSE;
	} else   {
		HR = Clear();
	}

	return(HR);
}


IDataTable * DBaseTopoC::DynamicDataTableCreate(CONST _TCHAR * pPathToDTable, CONST _TCHAR * pDTableName, INT iInitialSize, ETableGrowTypes eTableGrowType, CONST TDataTableFieldDescription aoFieldDescription[], BOOL bOpenAfterCreation /* = FALSE*/)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);

	HRESULT HR=S_OK;

	DTableTopoC * pDT;
	DFieldTopoC * pDF;

	if((pDT=AddChild(pDTableName))!=NULL)  {

		pDT->SetStartSize(iInitialSize);
		pDT->SetGrowModeI(eTableGrowType);

		int i=0;
		while(aoFieldDescription[i].szFieldName        !=NULL &&
				aoFieldDescription[i].etFieldType        !=TYPE_FINAL &&
				aoFieldDescription[i].etFieldModificator !=MOD_FINAL)   {

			if((pDF=pDT->AddChild(aoFieldDescription[i].szFieldName))==NULL)   {
				HR=LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("Child DATA_FIELD <%s> was not added"),aoFieldDescription[i].szFieldName);
				break;
			}

			if(!pDF->SetTypeInfo(aoFieldDescription[i].etFieldType))   {
				HR=LogFail(_F,ERROR_BAD_FORMAT,_T("Child DATA_FIELD <%s> data type was not assigned"),aoFieldDescription[i].szFieldName);
				break;
			}

			UINT uiFieldModificator = aoFieldDescription[i].etFieldModificator;
			if((uiFieldModificator & MOD_NO_FIND)== MOD_NO_FIND)   { pDF->m_NoFindFlag  = TRUE; if(aoFieldDescription[i].etFieldType==TYPE_INT32 || aoFieldDescription[i].etFieldType==TYPE_UINT32)   pDF->m_IsDataFlag  = TRUE; }
			if((uiFieldModificator & MOD_IS_UNIC)== MOD_IS_UNIC)   pDF->m_IsUnicFlag  = TRUE;
			if((uiFieldModificator & MOD_IS_DATA)== MOD_IS_DATA)   pDF->m_IsDataFlag  = TRUE;
			if((uiFieldModificator & MOD_INDEXED)== MOD_INDEXED)   pDF->m_IndexedFlag = TRUE;

			i++;
		}

		if(HR==S_OK)   {
			if((HR=pDT->SetDynamicInfo(pPathToDTable))==S_OK)   {
				HR=pDT->SetDTFilesPathsAndNames();
			}
		}

	} else   {
		HR=LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("Child DATA_TABLE <%s> was not added"),pDTableName);
	}

	IDataTable * pIDataTable;

	if(HR==S_OK)   {
		pIDataTable = static_cast<SS::Interface::Core::DBMS::IDataTable *>(pDT);
		if(bOpenAfterCreation)   DynamicDataTableOpen(pIDataTable);
	} else   {
		pIDataTable = NULL;
	}

	return(pIDataTable);
}

HRESULT DBaseTopoC::DynamicDataTableOpen(IDataTable * pIDataTable)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);
	return(static_cast<DTableTopoC *>(pIDataTable)->Open());
}

HRESULT DBaseTopoC::DynamicDataTableRefuse(IDataTable * pDTable)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);

	if(pDTable)   {
		pDTable->Close();
		return(DeleteChild(static_cast<DTableTopoC *>(pDTable)));
	} else   {
		return(S_FALSE);
	}
}


HRESULT DBaseTopoC::GetDataTablesNames(IN CONST _TCHAR * pDirectoryToExplore, OUT std::list< std::wstring > * plstDataTablesNamesList)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);

	HANDLE          hFile;
	WIN32_FIND_DATA sFileData;
	BOOL            bFirst;
	STLSTR          sPath = pDirectoryToExplore;

	plstDataTablesNamesList->clear();

	while(sPath.length() && (sPath[sPath.length()-1]==_T('\\') || sPath[sPath.length()-1]==_T('/')))   sPath.resize(sPath.length()-1);

	if(GetFileAttributes(sPath.c_str())==INVALID_FILE_ATTRIBUTES)   {
		return(LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("Directory not found (\"%s\")"),sPath.c_str()));
	}

	sPath += _T("\\*.*");

	bFirst = TRUE;
	while(1)   {
		if(bFirst)   {
			if((hFile=FindFirstFile(sPath.c_str(),&sFileData))==INVALID_HANDLE_VALUE)   break;
			bFirst = FALSE;
		} else   {
         if(!FindNextFile(hFile,&sFileData))   break;
		}

		if((sFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && sFileData.cFileName[0]!=_T('.'))   {
			plstDataTablesNamesList->push_back(sFileData.cFileName);
		}
	}

	if(hFile!=INVALID_HANDLE_VALUE)   FindClose(hFile);

	return(S_OK);
}

/*
HRESULT DBaseTopoC::UnGetDataTable(IDataTable * pDTable)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);

	HRESULT HR;

	if((HR=static_cast<DTableTopoC *>(pDTable)->UnGet())==S_OK)   {
		return(S_OK);
	}

	return(HR);
}

HRESULT DBaseTopoC::ClearDataTable(VOID)   {
	DTableTopoC * pDT;
	HRESULT        HR;

	if((HR=GetChild(&pDT))==S_OK)   {
		if((HR=pDT->Clear())==S_OK)   {
			return(S_OK);
		}
	}

	return(HR);
}
*/
