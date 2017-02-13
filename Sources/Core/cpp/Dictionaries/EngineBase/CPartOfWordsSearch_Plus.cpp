#include "stdafx.h"
#include "cl_TimeMeter.h"
#include "CPartOfWordsSearch.h"

using namespace SS::Dictionary::Core;
using namespace SS::Dictionary::Core::PartOfWordsSearchDbg;

//BOOL CPartOfWordsSearch::m_fReadMorphoData(CHAR * pszSqlBaseName)   {
//	int          n=0;
//	Logf("Read data from <%s>: ",pszSqlBaseName);
//	try   {
//		//	::SetThreadLocale(MAKELCID(0x419,SORT_DEFAULT));
//		CPrepRusNominalInfo           oRsOneMInfo;
//		CSqlConnection                oSqlCon(pszSqlBaseName); //
//
//		if(oRsOneMInfo.Open(oSqlCon.m_oSession)==S_OK)   {
//			if(oRsOneMInfo.MoveFirst()==S_OK)   {
//				do   {
//					if((n++%10000)==0)   Logf("*");
//					wchar_t wcsStr[128];
//					mbstowcs(wcsStr,oRsOneMInfo.m_Source,128);
//					m_oLfVecor.push_back(wcsStr);
//					m_oIdVecor.push_back(oRsOneMInfo.m_IDSource & 0x00FFFFFF);
//
//					//g_oLfVecor.push_back(wcsStr);
//					//g_oIdVecor.push_back(oRsOneMInfo.m_IDSource & 0x00FFFFFF);
//
//				} while(oRsOneMInfo.MoveNext()==S_OK);
//			}
//			oRsOneMInfo.Close();
//		}
//		Logf(" OK(%ld).\n",n);
//		return(TRUE);
//	} catch(...)   {
//		Logf("\nEXCEPTION\n");
//		DebugBreak();
//		return(FALSE);
//	}
//}

//extern std::vector<std::wstring> g_oLfVecor;
//extern std::vector<UINT32>       g_oIdVecor;

#if 1 //Версия для широких строк
BOOL CPartOfWordsSearch::m_fReadData(VOID)   {
	UINT32       iID;
	std::wstring  sPartOdWord;

	void * apScanArr[] = {&iID, &sPartOdWord };

	m_pIDataTable->ScanInit(0,apScanArr);

	while(m_pIDataTable->Scan()==S_OK)   {
		m_oLfVecor.push_back(sPartOdWord);
		m_oIdVecor.push_back(iID & 0x00FFFFFF);
	}

	return(TRUE);
}
#else
BOOL CPartOfWordsSearch::m_fReadData(VOID)   {
   UINT32       iID;
   std::string  sPartOdWord;

   void * apScanArr[] = {&iID, &sPartOdWord };

   m_pIDataTable->ScanInit(0,apScanArr);

   while(m_pIDataTable->Scan()==S_OK)   {
      wchar_t wcsStr[1024];
      mbstowcs(wcsStr,sPartOdWord.c_str(),1024);
      m_oLfVecor.push_back(wcsStr);
      m_oIdVecor.push_back(iID & 0x00FFFFFF);
   }

   return(TRUE);
}
#endif

VOID CPartOfWordsSearch::m_fMakeTreeBranchRecurFunc(UINT iCurNumOfLeters, std::vector<UINT> & roVectOfAllowedIdxs)   {

	// Make set of letters
	std::set<wchar_t> oSetOfLeters;
	for(UINT i=0; i<roVectOfAllowedIdxs.size(); i++)   {
		int ii=roVectOfAllowedIdxs[i];
		if(m_oLfVecor[ii].length()>=iCurNumOfLeters)   {
			oSetOfLeters.insert(m_oLfVecor[ii].c_str()[m_oLfVecor[ii].length()-iCurNumOfLeters]);
		}
	}

	UINT16 iNumOfLetters = UINT16(oSetOfLeters.size());
	SET_MEM_UINT16_AND_INC(m_pTPtr,iNumOfLetters);

	BYTE * pLettersBegin = m_pTPtr;
	BYTE * pLinksBegin   = m_pTPtr + (iNumOfLetters * sizeof(UINT16)); // m_pTPtr + (iNumOfLetters * sizeof(UINT16));

	m_pTPtr += iNumOfLetters * (sizeof(UINT16) + sizeof(UINT32)); // iNumOfLetters * ( LetterSize + LinkSize)
	
	std::set<wchar_t>::iterator iLetterItr = oSetOfLeters.begin();
	for(UINT iLetterCnt=0; iLetterItr!=oSetOfLeters.end(); iLetterItr++, iLetterCnt++)   {

		std::vector<UINT> oNewVectOfAllowedIdxs; // .clear();

		SET_MEM_UINT16_AND_INC(pLettersBegin, UINT16(*iLetterItr));
		SET_MEM_UINT32_AND_INC(pLinksBegin,   UINT32(m_pTPtr-m_pTreeData));

		UINT   iIdsNum = 0;
		BYTE * pIdsNum = m_pTPtr;
		m_pTPtr+=sizeof(UINT16);

		for(UINT i=0; i<roVectOfAllowedIdxs.size(); i++)   {
			int ii=roVectOfAllowedIdxs[i];
			if(m_oLfVecor[ii].length()>=iCurNumOfLeters &&
				*iLetterItr==m_oLfVecor[ii].c_str()[m_oLfVecor[ii].length()-iCurNumOfLeters])   {

				if(m_oLfVecor[ii].length()==iCurNumOfLeters)   {
					SET_MEM_UINT32_AND_INC(m_pTPtr,m_oIdVecor[ii]);
					iIdsNum++;
				} else   {
					oNewVectOfAllowedIdxs.push_back(ii);
				}
			}
		}

		MEM_UINT16(pIdsNum)=iIdsNum;
		if(oNewVectOfAllowedIdxs.size())   {
			m_fMakeTreeBranchRecurFunc(iCurNumOfLeters+1, oNewVectOfAllowedIdxs);
		} else   {
			SET_MEM_UINT16_AND_INC(m_pTPtr,0);
		}
	}
}

VOID CPartOfWordsSearch::m_fRectifyTreePtrs_RecurFunc(BYTE * pMem)   {
	#pragma warning( disable : 4311 ) 
	UINT iIdsNum = MEM_UINT16(pMem);
	pMem += sizeof(UINT16) + ( iIdsNum << 2 ); // + iNumEmpty * sizeof(UINT32);

	UINT iLinksNum = MEM_UINT16(pMem);
	if(iLinksNum)   {
		UINT32 * pLinksArr = (UINT32 *)(pMem + sizeof(UINT16) + (iLinksNum << 1));
		for(UINT i=0; i<iLinksNum; i++,pLinksArr++)   {
			MEM_UINT32(pLinksArr)+=UINT32(m_pTreeData);
			m_fRectifyTreePtrs_RecurFunc((BYTE *)NULL+MEM_UINT32(pLinksArr));
		}
	}
	#pragma warning( default : 4311 )
}

BOOL CPartOfWordsSearch::m_fRectifyIdArrPtrs_IterFunc(UINT32 * pIdArr)   {
	#pragma warning( disable : 4311 ) 
	#pragma warning( disable : 4312 ) 
	UINT32 iInc = (UINT32)(m_pIdData + sizeof(UINT32) + m_iIdNum * sizeof(UINT32))-1;
	VOID * pLim = m_pIdData + m_iIdDataSize;
	for(UINT i=0; i<m_iIdNum; i++)   {
		if(pIdArr[i])   {
			pIdArr[i]+=iInc;
			if((VOID *)pIdArr[i]>=pLim)   {
				LogFail(_F,"Массив данных для поиска испорчен.");
				return(FALSE);
			}
		}
	}
	return(TRUE);
	#pragma warning( default : 4311 )
	#pragma warning( default : 4312 )
}

INT CPartOfWordsSearch::m_fFindLetterIdx(wchar_t wcLetter, wchar_t * pLettersArr, UINT16 iLettersNum)   {

	for(int i=0; i<iLettersNum; i++)   if(pLettersArr[i]==wcLetter)   return(i);
	return(-1);

	//UINT    iIdx;
	//UINT    iIdxL=0;
	//#define iIdxR iLettersNum // UINT iIdxR=iLettersNum;

	//while(1)   {

	//	iIdx=(iIdxL+iIdxR)>>1;

	//	if(pLettersArr[iIdx]==wcLetter)   {
	//		return(iIdx);
	//	} else if(pLettersArr[iIdx]>wcLetter)   {
	//		if((iIdxR=iIdx)==iIdxL)   return(-1);
	//	} else   {
	//		if((iIdxL=iIdx+1)==iIdxR)   return(-1);
	//	}
	//}
}

/// Запись сообщения об ошибке в журнал системы
HRESULT SS::Dictionary::Core::PartOfWordsSearchDbg::LogFail(char * pFuncName, char * pFormatStr, ...)   {

	char    ErrStr[1024];
	va_list ArgList;
	int     Len=0;

	va_start(ArgList,pFormatStr);
		Len=sprintf(ErrStr,"   PartOfWordSearch ERROR: ");
		Len+=vsprintf(ErrStr+strlen(ErrStr),pFormatStr,ArgList);
		Len+=sprintf( ErrStr+strlen(ErrStr),". Method: %s().",pFuncName);

		#ifdef _DEBUG
			CharToOemA(ErrStr,ErrStr); 
			printf("%s\n",ErrStr);
			Beep(300,100);
		#endif

		wchar_t wcErrStr[1024];
		mbstowcs(wcErrStr, ErrStr, 1024);
		SS_THROW(wcErrStr);
	va_end(ArgList);

	return(E_FAIL);
}

/// Запись простого сообщения в журнал системы
VOID SS::Dictionary::Core::PartOfWordsSearchDbg::LogMess(char * pFormatStr, ...)   {
	char    MStr[1024];
	va_list ArgList;
	int     Len=0;

	va_start(ArgList,pFormatStr);
		Len=sprintf(MStr,"PartOfWordSearch MESSAGE: ");
		Len+=vsprintf(MStr+strlen(MStr),pFormatStr,ArgList);

		#ifdef _DEBUG
			CharToOemA(MStr,MStr); 
			printf("%s\n",MStr);
			Beep(300,100);
		#endif

		wchar_t wcErrStr[1024];
		mbstowcs(wcErrStr, MStr, 1024);
		SAVE_LOG(SS_MESSAGE AND wcErrStr);
	va_end(ArgList);
}

