#include "StdAfx.h"
#include "cl_Topo_DBMS.h"
#include "..\ASCInterface\IDBMS.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "cl_DManagerTopo.cpp"

/*
namespace DBMS   {
	DBManagerTopoC * g_pDBManager = NULL; // Main global pointer to DBMS
	LONG             g_nDBManagerReferences=0;
}

ULONG DBManagerTopoC::Release()   {
	if(g_pDBManager)   {
		if(InterlockedDecrement(&g_nDBManagerReferences)==0L)   {
			delete g_pDBManager;
			g_pDBManager = NULL;
		}
	}
	return(NULL);
}

HRESULT DBManagerTopoC::QueryInterface(REFIID pIID, VOID ** pBase)   {
	InterlockedIncrement(&g_nDBManagerReferences);
	*pBase=(IDBManager *)g_pDBManager;
	return(S_OK);
}
*/

ULONG DBManagerTopoC::Release()   {
	// VISIT_CRIT_SECT(m_oTopoCriticalSection);

	if(!IsOpen())   {
		// m_IsOpen=FALSE;
		delete this;
	}
	return(NULL);
}

HRESULT DBManagerTopoC::QueryInterface(REFIID pIID, VOID ** pBase)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);

	if(pIID==IID_DBManager)   {
		*pBase=(SS::Interface::Core::DBMS::IDBManager *)this;
		return(S_OK);
	} else if(pIID==IID_Base)   {
		*pBase=(SS::Interface::IBase *)this;
		return(S_OK);
	} else   {
		*pBase=NULL;
		return(LogFail(_F,ERROR_NOT_SUPPORTED,_T("Unsupported REFIID")));
	}
}


BOOL IsStrStartsWith(char * pCfgStr, char * pSubStr)   {
	return(strstr(pCfgStr,pSubStr)==pCfgStr);
}

char * GetSubStr(char * pCfgStr, char * pTargetStr, char * pFromStr, char * pToStr)   {
	if((pCfgStr=strstr(pCfgStr,pFromStr))==NULL)   return(NULL);
	pCfgStr+=strlen(pFromStr);
	pFromStr=pCfgStr;

	if(pToStr[0])   {

		if((pCfgStr=strstr(pCfgStr,pToStr))==NULL)   return(NULL);

		char Ch=*pCfgStr;
		*pCfgStr='\0';
		strcpy(pTargetStr,pFromStr);
		*pCfgStr=Ch;
	} else   {
		strcpy(pTargetStr,pFromStr);
	}
	return(pCfgStr);
}

size_t ProcessStr(char * pCfgStr)   {
	BYTE * pc, * pb;

	pc=(BYTE *)pCfgStr;
	while(*pc=='\t' || *pc>=' ')   {
		if(*pc=='/' && *(pc+1)=='/')   break;
		pc++;
	}
	*pc='\0';

	pc=pb=(BYTE *)pCfgStr;
	while(*pc)   {
		if(*pc!=' ' && *pc!='\t')   {
			if(pb!=pc)   {
				strcpy((char *)pb,(char *)pc);
				pc=pb;
			}
			pb++;
		}
		pc++;
	}
	if(pb!=pc) strcpy((char *)pb,(char *)pc);

	//_strupr(pCfgStr);

	return(strlen(pCfgStr));
}


#define CFG_STR_LIM 1024

#define ST_NONE         0
#define ST_DATA_MANAGER 1
#define ST_DATA_BASE    2
#define ST_DATA_TABLE   3
#define ST_DATA_FIELD   4

//	Reads specified INI file and checks/builds phisical image of DBMS
HRESULT DBManagerTopoC::Init(CONST _TCHAR * pFullPathToIniFile, CONST _TCHAR * pFullPathToDbmsBases)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);

	if(m_Initialized)   {
		LogMess(_T("DBMS method Init(PathToIniFile,PathToDbmsBases) or InitDynamic() was already called."));
		return(S_FALSE);
	} else   {
		if(InitReadTopoFile(pFullPathToIniFile,pFullPathToDbmsBases)!=S_OK)   return(LogFail(_F,0,_T("DBMS INI-file processing was unsuccessful.")));
		if(InitBuildPhysicalImage()!=S_OK)   return(LogFail(_F,0,_T("DBMS disk image building was unsuccessful.")));
		m_Initialized=TRUE;
		return(S_OK);
	}
}

HRESULT DBManagerTopoC::InitDynamic(OUT IDataBase ** ppIDataBase /* = NULL */)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);

	if(m_Initialized)   {
		LogMess(_T("DBMS method Init() or InitDynamic() was already called."));
		if(ppIDataBase)   {
			if(MoveToDataBase(UINT32(0))!=S_OK)   return(LogFail(_F,0,_T("MoveTo <DataBase(0)> error.")));
			*ppIDataBase = OpenDataBase();
			if(*ppIDataBase==NULL)                return(LogFail(_F,0,_T("Open <DataBase(0)> error.")));
		}
		return(S_OK);
	} else   {

		m_Name.assign(_T("")); // Manager Name
		AddChild(_T("DynamicDataBase"));

		if(ppIDataBase)   {
			if(MoveToDataBase(UINT32(0))!=S_OK)   return(LogFail(_F,0,_T("MoveTo <DynamicDataBase> error.")));
			*ppIDataBase = OpenDataBase();
			if(*ppIDataBase==NULL)                return(LogFail(_F,0,_T("Open <DynamicDataBase> error.")));
		}

		m_Initialized=TRUE;
		return(S_OK);
	}
}

//	Reads specified INI file and checks/builds phisical image of DBMS
HRESULT DBManagerTopoC::Switch(CONST _TCHAR * pFullPathToIniFile, CONST _TCHAR * pFullPathToDbmsBases)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);

	HRESULT HRet;

	if(m_Initialized)   {

		if((HRet=InitReadDbmsName(pFullPathToIniFile, pFullPathToDbmsBases))!=S_OK)   return(HRet);

		for(int i=0; i<m_ChildrenNum; i++)   {
			DBaseTopoC * pBase=FindChild(i);
			if(pBase==NULL)   return(LogFail(_F,ERROR_INVALID_ACCESS,_T("Expected DataBase was not found.")));
			for(int j=0; j<pBase->m_ChildrenNum; j++)   {
				DTableTopoC * pTable=pBase->FindChild(j);
				if(pTable==NULL)   return(LogFail(_F,ERROR_INVALID_ACCESS,_T("Expected DataTable was not found.")));
				if((HRet=pTable->Switch())!=S_OK)   return(HRet);
			}
		}
		return(S_OK);
	} else   {
		return(Init(pFullPathToIniFile, pFullPathToDbmsBases));
	}
}

//	Reads specified INI file for DbmsName
HRESULT DBManagerTopoC::InitReadDbmsName(CONST _TCHAR * pFullPathToIniFile, CONST _TCHAR * pFullPathToDbmsBases)   {
	HRESULT HRet=E_FAIL;
	FILE  * CfgFile;
	CHAR    CfgStr[CFG_STR_LIM+1];
	CHAR    HelpStr[1024];

	//LogMess(_T("SWC : %s"),pFullPathToIniFile);

	if((CfgFile=_tfopen(pFullPathToIniFile,_T("rt")))!=NULL)   {
		while(fgets(CfgStr,CFG_STR_LIM,CfgFile))   {
			if(ProcessStr(CfgStr))   {
				if(IsStrStartsWith(CfgStr,"#DBMS_NAME"))   {
					if(GetSubStr(CfgStr,HelpStr,"=\"","\"")==NULL)   { HRet=LogFail(_F,ERROR_BAD_FORMAT,_T("Bad format of DBMS_INI-File: <%s>"),pFullPathToIniFile); break; }
					m_Name.assign(pFullPathToDbmsBases);
					DTablePhysC::MkDir(pFullPathToDbmsBases);
					if(!m_Name.empty() && *(m_Name.rbegin())!='\\')   m_Name+='\\';
					TCHAR tcsStr[1024];
					m_Name += CHAR2TCHAR(HelpStr,tcsStr);
					HRet=S_OK;
					break;
				}
			}
		}
		fclose(CfgFile);
		if(HRet!=S_OK)   HRet=LogFail(_F,ERROR_BAD_FORMAT,_T("#DBMS_NAME was not found in DBMS_INI-File: <%s>"),pFullPathToIniFile);
	} else   {
		HRet=LogFail(_F,ERROR_FILE_NOT_FOUND,_T("Not found DBMS_INI-File <%s>"),pFullPathToIniFile);
	}

	return(HRet);
}


//	Checks/builds phisical image of DBMS
HRESULT DBManagerTopoC::InitBuildPhysicalImage(void)   {

	for(int i=0; i<m_ChildrenNum; i++)   {
		DBaseTopoC * pBase=FindChild(i);
		if(pBase==NULL)   return(LogFail(_F,ERROR_INVALID_ACCESS,_T("ExpectedDataBaseNotFound")));

		for(int j=0; j<pBase->m_ChildrenNum; j++)   {
			DTableTopoC * pTable=pBase->FindChild(j);
			if(pTable==NULL)   return(LogFail(_F,ERROR_INVALID_ACCESS,_T("ExpectedDataTableNotFound")));
			pTable->SetDTFilesPathsAndNames(); //
		}
	}

	return(S_OK);
}

//	Reads specified INI file
HRESULT DBManagerTopoC::InitReadTopoFile(CONST _TCHAR * pFullPathToIniFile, CONST _TCHAR * pFullPathToDbmsBases)   {

	g_bItIsCDROM = IsItCDROM();

	FILE  * CfgFile;
	CHAR    CfgOrg[CFG_STR_LIM+1];
	CHAR    CfgStr[CFG_STR_LIM+1];
	TCHAR   tcsStr[1024];
	LONG    CfgLine,CfgLPos;
	BOOL    bBadFormatFlag=FALSE;

	HRESULT HRet=S_OK;
	WORD    TopoState=ST_NONE;
	CHAR    HelpStr[1024];

	DBaseTopoC  * pDB;
	DTableTopoC * pDT;
	DFieldTopoC * pDF;

	CfgLine=CfgLPos=0L;

//	LogMess(_T("INI : %s"),pFullPathToIniFile);

	if((CfgFile=_tfopen(pFullPathToIniFile,_T("rt")))!=NULL)   {
		while(fgets(CfgOrg,CFG_STR_LIM,CfgFile))   {
			CfgLine++;
			strcpy(CfgStr,CfgOrg);
			if(ProcessStr(CfgStr))   {

				switch(TopoState)   {

					case ST_NONE:
						if(IsStrStartsWith(CfgStr,"#DBMS_NAME"))   {
							TopoState=ST_DATA_MANAGER;
							if(GetSubStr(CfgStr,HelpStr,"=\"","\"")==NULL)   { HRet=ERROR_BAD_FORMAT; bBadFormatFlag=TRUE; break; }
							m_Name.assign(pFullPathToDbmsBases);
							DTablePhysC::MkDir(pFullPathToDbmsBases);
							if(!m_Name.empty() && *(m_Name.rbegin())!='\\')   m_Name+='\\';
							m_Name += CHAR2TCHAR(HelpStr,tcsStr);

						}
					break;

					case ST_DATA_MANAGER: // MANAGER
						if(IsStrStartsWith(CfgStr,"#END_DBMS"))   {
							TopoState=ST_NONE;
						} else if(IsStrStartsWith(CfgStr,"#DATA_BASE_NAME"))   {
							TopoState=ST_DATA_BASE;
							if(GetSubStr(CfgStr,HelpStr,"=\"","\"")==NULL)     { HRet=ERROR_BAD_FORMAT; bBadFormatFlag=TRUE; break; }
							if((pDB=AddChild(CHAR2TCHAR(HelpStr,tcsStr)))==NULL)      { HRet=LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("Child DATA_BASE was not added")); break; }
//							if(strstr(CfgStr,"") || strstr(CfgStr,"") || strstr(CfgStr,"") || strstr(CfgStr,""))
						}
					break;

					case ST_DATA_BASE:
						if(IsStrStartsWith(CfgStr,"#END_DATA_BASE"))   {
							TopoState=ST_DATA_MANAGER;
						} else if(IsStrStartsWith(CfgStr,"#DATA_TABLE_NAME"))   {

							TopoState=ST_DATA_TABLE;
							if(GetSubStr(CfgStr,HelpStr,"=\"","\"")==NULL)     { HRet=ERROR_BAD_FORMAT; bBadFormatFlag=TRUE; break; }
							if((pDT=pDB->AddChild(CHAR2TCHAR(HelpStr,tcsStr)))==NULL) { HRet=LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("Child DATA_TABLE was not added")); break; }
							// DataTable Attributes
							if(GetSubStr(CfgStr,HelpStr,"START_SIZE(",")")==NULL)   strcpy(HelpStr,"0");
							pDT->SetStartSize(atol(HelpStr));
							if(GetSubStr(CfgStr,HelpStr,"SAVE_EVERY(",")")!=NULL)   pDT->SetSaveAfterValue(atol(HelpStr));
							if(GetSubStr(CfgStr,HelpStr,"GROW_","")==NULL)          strcpy(HelpStr,"EVEN");
							pDT->SetGrowModeS(CHAR2TCHAR(HelpStr,tcsStr));
							pDT->m_sTableIniStructure = CfgOrg;
							pDT->m_sTableIniStructure.resize(pDT->m_sTableIniStructure.size()-1);
							pDT->m_sTableIniStructure += "\r\n\r\n";

						}
					break;

					case ST_DATA_TABLE:
						
						if(IsStrStartsWith(CfgStr,"#END_DATA_TABLE"))   {
							pDT->m_sTableIniStructure += "\r\n";
							pDT->m_sTableIniStructure += CfgOrg;
							pDT->m_sTableIniStructure.resize(pDT->m_sTableIniStructure.size()-1);
							pDT->m_sTableIniStructure += "\r\n";
							TopoState=ST_DATA_BASE;
						} else if(IsStrStartsWith(CfgStr,"#DATA_FIELD_INFO"))   {
							pDT->m_sTableIniStructure += CfgOrg;
							pDT->m_sTableIniStructure.resize(pDT->m_sTableIniStructure.size()-1);
							pDT->m_sTableIniStructure += "\r\n";
							if(GetSubStr(CfgStr,HelpStr,"=\"","\"")==NULL)     { HRet=ERROR_BAD_FORMAT; bBadFormatFlag=TRUE; break; }
							if((pDF=pDT->AddChild(CHAR2TCHAR(HelpStr,tcsStr)))==NULL) { HRet=LogFail(_F,ERROR_NOT_ENOUGH_MEMORY,_T("Child DATA_FIELD was not added")); break; }
							// DataField Attributes
							if(GetSubStr(CfgStr,HelpStr,":","=")==NULL  &&
								GetSubStr(CfgStr,HelpStr,":","-")==NULL &&
								GetSubStr(CfgStr,HelpStr,":",":")==NULL &&
								GetSubStr(CfgStr,HelpStr,":","")==NULL )        { HRet=ERROR_BAD_FORMAT; bBadFormatFlag=TRUE; break; }
							
							if(!pDF->SetTypeInfo(CHAR2TCHAR(HelpStr,tcsStr)))         { HRet=ERROR_BAD_FORMAT; bBadFormatFlag=TRUE; break; }
							if(GetSubStr(CfgStr,HelpStr,"NO_FIND","")   !=NULL)   { pDF->m_NoFindFlag  = TRUE; }
							if(GetSubStr(CfgStr,HelpStr,"IS_UNIC","")   !=NULL)   { pDF->m_IsUnicFlag  = TRUE; }
							if(GetSubStr(CfgStr,HelpStr,"IS_DATA","")   !=NULL)   { pDF->m_IsDataFlag  = TRUE; }
							if(GetSubStr(CfgStr,HelpStr,"IS_INDEXED","")!=NULL)   { pDF->m_IndexedFlag = TRUE; }
						}
					break;

					default:
						if(IsStrStartsWith(CfgStr,"#"))   {
							// CfgLine
							fclose(CfgFile);
							HRet=ERROR_BAD_FORMAT; 
							bBadFormatFlag=TRUE;
						}
					break;
				}
			}

			if(bBadFormatFlag)   {
				bBadFormatFlag=FALSE;
				HRet=LogFail(_F,ERROR_BAD_FORMAT,_T("Bad format of DBMS_INI-File. Line N %ld (%S), File: <%s>"),CfgLine,CfgStr,pFullPathToIniFile);
			}

			CfgLPos=ftell(CfgFile);

			if(HRet!=S_OK)   break;

		}

		fclose(CfgFile);

	} else   {

		HRet=LogFail(_F,GetLastError(),_T("Not found DBMS_INI-File <%s>"),pFullPathToIniFile);

	}

	return(HRet);
}

SS::Interface::Core::DBMS::IDataBase * DBManagerTopoC::OpenDataBase(void)   {
	VISIT_CRIT_SECT(m_oTopoCriticalSection);

	DBaseTopoC * pDB;
	HRESULT       HR;

	if((HR=GetChild(&pDB))==S_OK)   {
		pDB->m_ReferenceCounter++;
		return(static_cast<SS::Interface::Core::DBMS::IDataBase *>(pDB));
	} else   {
		LogFail(_F,HR,_T("Pointer DataBase N %ld was not obtained"),m_ChildCurrent);
		return(NULL);
	}
}

/*
HRESULT DBManagerTopoC::UnGetDataBase(IDataBase * pDBase)   {
	DBaseTopoC * pBase = static_cast<DBaseTopoC *>(pDBase);

	for(int i=0; i<pBase->m_ChildrenNum; i++)   {
		DTableTopoC * pTable=pBase->FindChild(i);
		if(pTable==NULL)   return(LogFail(_F,ERROR_INVALID_ACCESS,"ExpectedDataTableNotFound"));
		pTable->UnGet(); //
	}
	return(S_OK);
}

HRESULT DBManagerTopoC::ClearDataBase(void)   {
	DBaseTopoC * pDB;
	HRESULT       HR=S_OK;

	if((HR=GetChild(&pDB))==S_OK)   {
		for(int i=0; i<pDB->m_ChildrenNum; i++)   {
			DTableTopoC * pTable=pDB->FindChild(i);
			if(pTable==NULL)   {
				HR=LogFail(_F,ERROR_INVALID_ACCESS,"ExpectedDataTableWasNotFound");
			} else   {
				if((HR=pTable->Clear())!=S_OK)  LogFail(_F,HR,"DataTableWasNotCleared");
			}
		}
	}

	if(HR==S_OK)   {
		return(S_OK);
	} else   {
		return(HR);
	}
}
*/
