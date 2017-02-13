#include "stdafx.h"

#include "cl_TimeMeter.h"
#include "CMorphoTypesSearch.h"

using namespace SS::Dictionary::Core;
using namespace SS::Dictionary::Core::CMorphoTypesSearchDbg;

CMorphoTypesSearch::CMorphoTypesSearch(VOID) : m_pIDataBase(NULL)
{
	m_oIDTypeInfoEmpty.m_oMorphoInfo = 0;
	memset( &m_oMorFormInfoEmpty, 0, sizeof(TMorFormInfo));
}

CMorphoTypesSearch::~CMorphoTypesSearch(VOID)   {
	Clear();
}

/// Инициализация путь к файлам данных и указатель на интерфейс таблицы IDataBase (для вызова Load())
BOOL CMorphoTypesSearch::Init(
	CONST _TCHAR * pszPathToDataFiles,
	SS::Interface::Core::DBMS::IDataBase * pIDataBase)   {

	m_pIDataBase      = pIDataBase;

	if(pszPathToDataFiles==NULL)   {
		m_sPathToDataFiles = _T(".\\");
	} else   {
		m_sPathToDataFiles = pszPathToDataFiles;
	}
	if(m_sPathToDataFiles.length()==0)   {
		m_sPathToDataFiles = _T(".\\");
	} else   {
		if(m_sPathToDataFiles.c_str()[m_sPathToDataFiles.length()-1]!='\\')   {
			m_sPathToDataFiles += _T("\\");
		}
	}
	
	return(TRUE);
}

#pragma pack(push,1)
struct SMorphoTypesSearchInfo   {
	SS::Dictionary::Types::TIDType        oIDType;
	SS::Dictionary::Types::TMorphoInfo    oMorphoInfo;
	SS::Dictionary::Types::TIDForm        oIDForm;
	SS::Dictionary::Types::TIDEnding      oIDEnding;
	SS::Dictionary::Types::TSpecialMorpho oSpecialMorpho;
};
#pragma pack(pop)


/// Класс CTimeMeter
class CTimeMeter   {

	public:

		/// Конструктор
		CTimeMeter();

	public: //

		/// Начать/возобновить измерение времени
		void _Start(void);

		/// Закончить/приостановить измерение времени
		void _Stop(void);

		/// Очистить счетчики
		void _Clear(void);

		/// Вернуть значение счетчика в милли-секундах (1/1000 сек.)
		LONGLONG _GetTimeValue(void);

		/// Вернуть строку указатель на строку времени в формате "5h 07m 34s 025ms"
		CHAR *   _GetTimeString(void);

	private:

		LARGE_INTEGER m_TGlobal;      //
		LARGE_INTEGER m_TStart;       //
		LARGE_INTEGER m_TFrequency;   //
		BOOL          m_TStartedFlag; // 
		CHAR          m_TString[64];  //
};

CTimeMeter::CTimeMeter() {
	m_TStartedFlag     = FALSE;
	m_TGlobal.QuadPart = 0i64;
	m_TStart .QuadPart = 0i64;
	memset( m_TString, 0, sizeof(CHAR) * 64 );
	if( !QueryPerformanceFrequency(&m_TFrequency) || m_TFrequency.QuadPart == 0i64 )   {
		throw(1);
	}
}

void CTimeMeter::_Start(void)   {

	if(!m_TStartedFlag)   {
		if(QueryPerformanceCounter(&m_TStart))   {
			m_TStartedFlag  = TRUE;
		} else   {
			throw(1);
		}
	}
}

void CTimeMeter::_Stop(void)   {
	if(m_TStartedFlag)   {
		LARGE_INTEGER TStop;        //
		if(QueryPerformanceCounter(&TStop))   {
			m_TGlobal.QuadPart  += (TStop.QuadPart-m_TStart.QuadPart)/* *1000i64/m_TFrequency.QuadPart*/;
			m_TStartedFlag  = FALSE;
		} else   {
			throw(1);
		}
	}
}

// -------------------------------------------------------------------

//CHAR * MkFullFilePath(CONST CHAR * pszFileName)   {
//	static CHAR  szFullFilePath[MAX_PATH+128];
//	char       * pszEnd;
//
//	GetModuleFileNameA(0,szFullFilePath,MAX_PATH);
//	if((pszEnd=strrchr(szFullFilePath,'\\'))==NULL)   {
//		pszEnd=szFullFilePath;
//	} else    {
//		pszEnd++;
//	}
//
//	strcpy(pszEnd,pszFileName); 
//
//	return(szFullFilePath);
//}


void CTimeMeter::_Clear(void)   {
	m_TGlobal.QuadPart = 0i64;
}


LONGLONG CTimeMeter::_GetTimeValue(void)   {
	BOOL bTStartedFlag = m_TStartedFlag;

	_Stop();
	LONGLONG llTime=m_TGlobal.QuadPart*1000i64/m_TFrequency.QuadPart;

	if(bTStartedFlag)   _Start();

	return(llTime);
}


CHAR * CTimeMeter::_GetTimeString(void)   {
	LONGLONG llTime = _GetTimeValue();
	int h,m,s,ms;

	ms = INT(llTime%1000);  llTime/=1000;
	s  = INT(llTime%60  );  llTime/=60;
	m  = INT(llTime%60  );  llTime/=60;
	h  = INT(llTime     );

	sprintf(m_TString,"%ldh %2.2ldm %2.2lds %3.3ldms",h,m,s,ms);

	return(m_TString);
}


/// Загружает/перезагружает словарь
BOOL CMorphoTypesSearch::Load(VOID)   {
	UINT        i,n=0;
	HANDLE      hFHandle;
	STLSTR      sFullPath;

//	CTimeMeter  t1,t2,t3;
//
//printf("\nCMorphoTypesSearch START\n");
//t1._Start();
	if(m_sPathToDataFiles.length()==0)   {
		LogFail(_F,"Не вызывался метод Init().");
		return(FALSE);
	}

	//if(m_oIDTypeMap.size()!=0)   {
	//	LogMess("Повторная загрузка данных для поиска.");
	//}

//	FILE * pf = fopen("d:\\__DATA.dta","wb");
//	if(pf==NULL)   printf("!!!");

	Clear();

	sFullPath = m_sPathToDataFiles + _T("ID_TypeInfo.dta");
	hFHandle=CreateFile(sFullPath.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, NULL);
	if(hFHandle!=INVALID_HANDLE_VALUE)   {

//		printf("\nVARIANT N 1 (Fast)\n");		

		DWORD iRead;
		UINT   iDataSize=GetFileSize(hFHandle,NULL);
		BYTE * pData;
		if((pData=(BYTE *)malloc(iDataSize))!=NULL)   {
//			t3._Start();
			if(!ReadFile(hFHandle,pData,iDataSize,&iRead,NULL) || iDataSize!=iRead)   {
				LogFail(_F,"Ошибка чтения файла данных ID_TypeInfo.dta.");
			} else   {
//				t3._Stop();
				SMorphoTypesSearchInfo * pStruct = (SMorphoTypesSearchInfo *)pData;
				for(i=0, n=iDataSize/sizeof(SMorphoTypesSearchInfo); i<n; i++, pStruct++)   {
//					t2._Start();
					AddInfo(pStruct->oIDType,pStruct->oMorphoInfo,pStruct->oIDForm,pStruct->oIDEnding,pStruct->oSpecialMorpho);		
//					t2._Stop();
				}
			}
			free(pData);
		} else   {
			LogFail(_F,"Не выделена память для данных дерева поиска (%ld bytes).",iDataSize);
		}

		CloseHandle(hFHandle);
	} else   {

//		printf("\nVARIANT N 2 (SLOW)\n");		

		// LogFail(_F,"Ошибка открытия файла данных ID_TypeInfo.dta (Err:%ld).",GetLastError());

		//	FILE * pf = fopen("d:\\__DATA.dta","wb");
		//	if(pf==NULL)   printf("!!!");
		
			DBMS::IDataTable * poDataTable1;
			DBMS::IDataTable * poDataTable2;
		
			if(m_pIDataBase->MoveToDataTable(DBMS_TABLE_MorphoTypes)!=S_OK)   { 
				LogFail(_F,"Ошибка перехода к таблице <%s>.",DBMS_TABLE_MorphoTypes);
				return(FALSE);
			}
			if((poDataTable1=m_pIDataBase->OpenDataTable())==NULL)   { 
				LogFail(_F,"Ошибка открытия таблицы <%s>.",DBMS_TABLE_MorphoTypes);
				return(FALSE);
			}
		
			if(m_pIDataBase->MoveToDataTable(DBMS_TABLE_MorphoTypeToMorphoInfo)!=S_OK)   { 
				LogFail(_F,"Ошибка перехода к таблице <%s>.",DBMS_TABLE_MorphoTypeToMorphoInfo);
				return(FALSE);
			}
			if((poDataTable2=m_pIDataBase->OpenDataTable())==NULL)   { 
				LogFail(_F,"Ошибка открытия таблицы <%s>.",DBMS_TABLE_MorphoTypeToMorphoInfo);
				return(FALSE);
			}
		
			INT32 iIDForm;      
			INT32 iIDType;      
			INT32 iIDEnding;    
			INT32 iPartOfSpeech;
			INT32 iMorphoInfo;
		
			WORD  iFindId;
		
			void * apFindArr1[] = {&iIDForm, NULL, &iIDEnding, &iPartOfSpeech };
			void * apScanArr2[] = {&iIDType, &iMorphoInfo };
		
			if( poDataTable1->FindInitEx(_T("IDType"),&iIDType,apFindArr1,&iFindId) != S_OK )   {
				LogFail(_F,"Ошибка FindInitEx().",DBMS_TABLE_MorphoTypes);
				return(FALSE);
			}
		
			if( poDataTable2->ScanInit(0,apScanArr2) != S_OK )   {
				LogFail(_F,"Ошибка ScanInit().",DBMS_TABLE_MorphoTypes);
				return(FALSE);
			}
		
			while(poDataTable2->Scan()==S_OK)   {
				while(poDataTable1->Find(iFindId)==S_OK)   {
					AddInfo(iIDType,iMorphoInfo,iIDForm,iIDEnding,iPartOfSpeech);
		//			SMorphoTypesSearchInfo oInfo = {iIDType,iMorphoInfo,iIDForm,iIDEnding,iPartOfSpeech};
		//			fwrite(&oInfo,sizeof(SMorphoTypesSearchInfo),1,pf);
					n++;
				}
			}
		
			poDataTable1->FindCancel(iFindId);
		   poDataTable1->Close();
		   poDataTable2->Close();

		//	fclose(pf);
	}

//t1._Stop();
//printf("\nCMorphoTypesSearch STOP (%s,%s,%s)\n",t1._GetTimeString(),t2._GetTimeString(),t3._GetTimeString());

	return(n!=0);
}

///// Загружает/перезагружает словарь
//BOOL CMorphoTypesSearch::Load(VOID)   {
//	int n=0;
//printf(" 3");
//	if(m_sPathToDataFiles.length()==0)   {
//		LogFail(_F,"Не вызывался метод Init().");
//		return(FALSE);
//	}
//
//	//if(m_oIDTypeMap.size()!=0)   {
//	//	LogMess("Повторная загрузка данных для поиска.");
//	//}
//
//	Clear();
//
////	FILE * pf = fopen("d:\\__DATA.dta","wb");
////	if(pf==NULL)   printf("!!!");
//
//	DBMS::IDataTable * poDataTable1;
//	DBMS::IDataTable * poDataTable2;
//
//	if(m_pIDataBase->MoveToDataTable(DBMS_TABLE_MorphoTypes)!=S_OK)   { 
//		LogFail(_F,"Ошибка перехода к таблице <%s>.",DBMS_TABLE_MorphoTypes);
//		return(FALSE);
//	}
//	if((poDataTable1=m_pIDataBase->OpenDataTable())==NULL)   { 
//		LogFail(_F,"Ошибка открытия таблицы <%s>.",DBMS_TABLE_MorphoTypes);
//		return(FALSE);
//	}
//
//	if(m_pIDataBase->MoveToDataTable(DBMS_TABLE_MorphoTypeToMorphoInfo)!=S_OK)   { 
//		LogFail(_F,"Ошибка перехода к таблице <%s>.",DBMS_TABLE_MorphoTypeToMorphoInfo);
//		return(FALSE);
//	}
//	if((poDataTable2=m_pIDataBase->OpenDataTable())==NULL)   { 
//		LogFail(_F,"Ошибка открытия таблицы <%s>.",DBMS_TABLE_MorphoTypeToMorphoInfo);
//		return(FALSE);
//	}
//
//	INT32 iIDForm;      
//	INT32 iIDType;      
//	INT32 iIDEnding;    
//	INT32 iPartOfSpeech;
//	INT32 iMorphoInfo;
//
//	WORD  iFindId;
//
//	void * apFindArr1[] = {&iIDForm, NULL, &iIDEnding, &iPartOfSpeech };
//	void * apScanArr2[] = {&iIDType, &iMorphoInfo };
//
//	if( poDataTable1->FindInitEx(_T("IDType"),&iIDType,apFindArr1,&iFindId) != S_OK )   {
//		LogFail(_F,"Ошибка FindInitEx().",DBMS_TABLE_MorphoTypes);
//		return(FALSE);
//	}
//
//	if( poDataTable2->ScanInit(0,apScanArr2) != S_OK )   {
//		LogFail(_F,"Ошибка ScanInit().",DBMS_TABLE_MorphoTypes);
//		return(FALSE);
//	}
//
//	while(poDataTable2->Scan()==S_OK)   {
//		while(poDataTable1->Find(iFindId)==S_OK)   {
//			AddInfo(iIDType,iMorphoInfo,iIDForm,iIDEnding,iPartOfSpeech);
////			SMorphoTypesSearchInfo oInfo = {iIDType,iMorphoInfo,iIDForm,iIDEnding,iPartOfSpeech};
////			fwrite(&oInfo,sizeof(SMorphoTypesSearchInfo),1,pf);
//			n++;
//		}
//	}
//
//	poDataTable1->FindCancel(iFindId);
//   poDataTable1->Close();
//   poDataTable2->Close();
//
////	fclose(pf);
//printf("4 ");
//	return(n!=0);
//}

/// Загружает словарь только если он ещё не был загружен
BOOL CMorphoTypesSearch::LoadIfNeed()
{
   if( Empty() ) return Load();
   return TRUE;
}

/// Освобождает память
BOOL CMorphoTypesSearch::Clear(VOID)   {
	m_oIDTypeMap.clear();
	return(TRUE);
}

// добавляет часть информации для последующего поиска
BOOL CMorphoTypesSearch::AddInfo(
	IN  SS::Dictionary::Types::TIDType        oIDType,
	IN  SS::Dictionary::Types::TMorphoInfo    oMorphoInfo, 
	IN  SS::Dictionary::Types::TIDForm        oIDForm, 	
	IN  SS::Dictionary::Types::TIDEnding      oIDEnding, 
	IN  SS::Dictionary::Types::TSpecialMorpho oSpecialMorpho)   {

	TIDTypeInfo * pIDTypeInfo;

	m_oIDTypeItr = m_oIDTypeMap.find(oIDType);
	if(m_oIDTypeItr != m_oIDTypeMap.end())   { // Yet exist
		pIDTypeInfo = (TIDTypeInfo *)&((*m_oIDTypeItr).second);
	} else   { // Not exist
		m_oIDTypeRet = m_oIDTypeMap.insert(TIDTypePair(oIDType, m_oIDTypeInfoEmpty));

		if(!m_oIDTypeRet.second)   { // Error
			LogFail(_F,"Ошибка добавления нового элемента (std::map.insert(IDType:%ld,...))",oIDType);
			return(FALSE);
		} else   {
			pIDTypeInfo = (TIDTypeInfo *)&((*m_oIDTypeRet.first).second);
		}
	}

	// Now we must define the following fields :
	//
	// 1. pIDTypeInfo->m_oMorphoInfo = oMorphoInfo;
	// 2. pIDTypeInfo->m_oMorFormInfoVect
	// 3. pIDTypeInfo->m_oEndingsAndFormsMap

	// 1.
	pIDTypeInfo->m_oMorphoInfo = oMorphoInfo;

	// 2.
	TMorFormInfoVect * pMorFormInfoVect = &(pIDTypeInfo->m_oMorFormInfoVect);
	if(oIDForm >= pMorFormInfoVect->size())   pMorFormInfoVect->resize(oIDForm+1);
	m_oMorFormInfoEmpty.m_oIDEnding      = oIDEnding;
	m_oMorFormInfoEmpty.m_oSpecialMorpho = oSpecialMorpho;
	pMorFormInfoVect->at(oIDForm) = m_oMorFormInfoEmpty;

	// 3.
	TIDFormVect         * pIDFormVect;
	TEndingsAndFormsMap * pEndingsAndFormsMap = &(pIDTypeInfo->m_oEndingsAndFormsMap);

	m_oEndingsAndFormsItr = pEndingsAndFormsMap->find(oIDEnding);
	if(m_oEndingsAndFormsItr != pEndingsAndFormsMap->end())   { // Yet exist
		pIDFormVect = (TIDFormVect *)&((*m_oEndingsAndFormsItr).second);
	} else   { // Not exist
		m_oEndingsAndFormsRet = pEndingsAndFormsMap->insert(TEndingsAndFormsPair(oIDEnding,m_oIDFormVectEmpty));
		if(!m_oEndingsAndFormsRet.second)   { // Error
			LogFail(_F,"Ошибка добавления нового элемента (std::map.insert(IDEnding:%ld,...))",oIDEnding);
			return(FALSE);
		} else   {
			pIDFormVect = (TIDFormVect *)&((*m_oEndingsAndFormsRet.first).second);
		}
	}

	pIDFormVect->push_back(oIDForm);

	return(TRUE);
}

// осуществляет поиск информации об окончании по индексу литерального представления
BOOL CMorphoTypesSearch::GetByForm(
	IN  SS::Dictionary::Types::TIDType oIDType, 
	IN  SS::Dictionary::Types::TIDEnding oIDEnding, 
	OUT std::list<SS::Dictionary::Types::SMorphoType> * p_l_MorphoType)   {

	if(p_l_MorphoType)   p_l_MorphoType->clear();

	TIDTypeInfo * pIDTypeInfo;

	m_oIDTypeItr = m_oIDTypeMap.find(oIDType);
	if(m_oIDTypeItr != m_oIDTypeMap.end())   { // IDType exists

		pIDTypeInfo = (TIDTypeInfo *)&((*m_oIDTypeItr).second);

		SMorphoType oMorphoType;

		oMorphoType.m_MorphoTypeInfo.m_IDType     = oIDType;
		oMorphoType.m_MorphoTypeInfo.m_MorphoInfo = pIDTypeInfo->m_oMorphoInfo;

		TEndingsAndFormsMap * pEndingsAndFormsMap = &(pIDTypeInfo->m_oEndingsAndFormsMap);
		m_oEndingsAndFormsItr = pEndingsAndFormsMap->find(oIDEnding);
		if(m_oEndingsAndFormsItr != pEndingsAndFormsMap->end())   { // IDEnding exists	

			if(p_l_MorphoType)   { // Valid list ptr

				TIDFormVect * pIDFormVect = (TIDFormVect *)&((*m_oEndingsAndFormsItr).second);

				
				TMorFormInfoVect * pMorFormInfoVect = &(pIDTypeInfo->m_oMorFormInfoVect);

				int        i;
				int        Num = int(pMorFormInfoVect->size());
				TIDFormItr iItr = pIDFormVect->begin();
				TIDFormItr iEnd = pIDFormVect->end();
			
				while(iItr!=iEnd)   {
					i=(*iItr);

					if(i<Num)   {
						oMorphoType.m_IDForm        = i;
						oMorphoType.m_IDEnding      = pMorFormInfoVect->at(i).m_oIDEnding;
						oMorphoType.m_SpecialMorpho = pMorFormInfoVect->at(i).m_oSpecialMorpho;
						p_l_MorphoType->push_back(oMorphoType);
					} else   {
						LogFail(_F,"Неверная ссылка от <IDEnding:%ld> к <IDForm:%ld>",oIDEnding,i);
					}

					iItr++;
				}
			}
			return(TRUE);

		} else   { // IDEnding does not exist	
			return(FALSE);
		}

	} else   { // IDType does not exist
		return(FALSE);
	}
}

// осуществляет поиск информации об индексе литерального представления по индексу словоформы
BOOL CMorphoTypesSearch::GetIDEndingByIDTypeAndIDForm(
	IN  SS::Dictionary::Types::TIDType oIDType, 
	IN  SS::Dictionary::Types::TIDForm oIDForm, 
	OUT SS::Dictionary::Types::TIDEnding * pIDEnding)   {

	TIDTypeInfo * pIDTypeInfo;

	m_oIDTypeItr = m_oIDTypeMap.find(oIDType);
	if(m_oIDTypeItr != m_oIDTypeMap.end())   { // IDType exists
		pIDTypeInfo = (TIDTypeInfo *)&((*m_oIDTypeItr).second);
		if(oIDForm<pIDTypeInfo->m_oMorFormInfoVect.size())   { // IDForm exists
			if(pIDEnding)   {
				*pIDEnding = pIDTypeInfo->m_oMorFormInfoVect[oIDForm].m_oIDEnding;
			}
			return(TRUE);
		} else   { // IDForm does not exist
			return(FALSE);
		}
	} else   { // IDType does not exist
		return(FALSE);
	}
}

// осуществляет поиск морфологической информации по индексу окончания в типе морфообразования 
BOOL CMorphoTypesSearch::GetMorphoTypeByIDTypeAndIDForm(
	IN  SS::Dictionary::Types::TIDType oIDType, 
	IN  SS::Dictionary::Types::TIDForm oIDForm, 
	OUT SS::Dictionary::Types::SMorphoType * pMorphoType)   {

	TIDTypeInfo * pIDTypeInfo;

	m_oIDTypeItr = m_oIDTypeMap.find(oIDType);
	if(m_oIDTypeItr != m_oIDTypeMap.end())   { // IDType exists
		pIDTypeInfo = (TIDTypeInfo *)&((*m_oIDTypeItr).second);
		if(oIDForm<pIDTypeInfo->m_oMorFormInfoVect.size())   { // IDForm exists
			if(pMorphoType)   {

				TMorFormInfo * pMorFormInfo = &pIDTypeInfo->m_oMorFormInfoVect[oIDForm];

				pMorphoType->m_IDEnding      = pMorFormInfo->m_oIDEnding;
				pMorphoType->m_SpecialMorpho = pMorFormInfo->m_oSpecialMorpho;
				pMorphoType->m_IDForm        = oIDForm;
				pMorphoType->m_MorphoTypeInfo.m_IDType        = oIDType;
				pMorphoType->m_MorphoTypeInfo.m_MorphoInfo    = pIDTypeInfo->m_oMorphoInfo;
			}
			return(TRUE);
		} else   { // IDForm does not exist
			return(FALSE);
		}
	} else   { // IDType does not exist
		return(FALSE);
	}
}

// осуществляет поиск информации о всех окончаниях в типе морфообразования
BOOL CMorphoTypesSearch::GetMorphoTypesListByIDType(
	IN  SS::Dictionary::Types::TIDType oIDType, 
	OUT std::list<SS::Dictionary::Types::SMorphoType> * p_l_MorphoType)   {

	if(p_l_MorphoType)   p_l_MorphoType->clear();

	TIDTypeInfo * pIDTypeInfo;

	m_oIDTypeItr = m_oIDTypeMap.find(oIDType);
	if(m_oIDTypeItr != m_oIDTypeMap.end())   { // IDType exists

		if(p_l_MorphoType)   { // Valid list ptr

			pIDTypeInfo = (TIDTypeInfo *)&((*m_oIDTypeItr).second);

			SMorphoType oMorphoType;

			oMorphoType.m_MorphoTypeInfo.m_IDType     = oIDType;
			oMorphoType.m_MorphoTypeInfo.m_MorphoInfo = pIDTypeInfo->m_oMorphoInfo;


			int             i=0; 
			TMorFormInfoItr iItr = pIDTypeInfo->m_oMorFormInfoVect.begin();
			TMorFormInfoItr iEnd = pIDTypeInfo->m_oMorFormInfoVect.end();
		
			while(iItr!=iEnd)   {
				oMorphoType.m_IDForm        = i;
				oMorphoType.m_IDEnding      = (*iItr).m_oIDEnding;
				oMorphoType.m_SpecialMorpho = (*iItr).m_oSpecialMorpho;
				
				p_l_MorphoType->push_back(oMorphoType);

				i++;
				iItr++;
			}
		}

		return(TRUE);

	} else   { // IDType does not exist
		return(FALSE);
	}
}

// осуществляет поиск морфологической информации по типу морфообразования
BOOL CMorphoTypesSearch::GetMorphoInfoByIDType(
	IN  SS::Dictionary::Types::TIDType oIDType, 
	IN  SS::Dictionary::Types::TIDForm oIDForm, 
	OUT SS::Dictionary::Types::TMorphoInfo * pMorphoInfo, 
	OUT SS::Dictionary::Types::TSpecialMorpho * pSpecialMorpho)   {

	TIDTypeInfo * pIDTypeInfo;

	m_oIDTypeItr = m_oIDTypeMap.find(oIDType);
	if(m_oIDTypeItr != m_oIDTypeMap.end())   { // IDType exists
		pIDTypeInfo = (TIDTypeInfo *)&((*m_oIDTypeItr).second);
		if(oIDForm<pIDTypeInfo->m_oMorFormInfoVect.size())   { // IDForm exists
			if(pMorphoInfo)   {
				*pMorphoInfo = pIDTypeInfo->m_oMorphoInfo;
			}
			if(pSpecialMorpho)   {
				*pSpecialMorpho = pIDTypeInfo->m_oMorFormInfoVect[oIDForm].m_oSpecialMorpho;
			}
			return(TRUE);
		} else   { // IDForm does not exist
			return(FALSE);
		}
	} else   { // IDType does not exist
		return(FALSE);
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL CMorphoTypesSearch::GetMorphoInfoByIDType( 
      IN   SS::Dictionary::Types::TIDType oIDType,
      OUT SS::Dictionary::Types::TMorphoInfo *pMorphoInfo
)
{
   m_oIDTypeItr = m_oIDTypeMap.find(oIDType);
   
   if( m_oIDTypeItr != m_oIDTypeMap.end() )   
   {
      *pMorphoInfo = m_oIDTypeItr->second.m_oMorphoInfo;
      return TRUE;
   }

   return FALSE;
}



