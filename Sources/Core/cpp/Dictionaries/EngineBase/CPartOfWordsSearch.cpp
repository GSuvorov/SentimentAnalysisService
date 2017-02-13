#include "stdafx.h"
#include "cl_TimeMeter.h"
#include "CPartOfWordsSearch.h"

using namespace SS::Dictionary::Core;
using namespace SS::Dictionary::Core::PartOfWordsSearchDbg;

/// Конструктор 
CPartOfWordsSearch::CPartOfWordsSearch() : 
	m_pTreeData(NULL),
	m_pIdData(NULL),
	m_pIDataTable(NULL)
//	m_sPathToDataFiles("")
	{
}

/// Конструктор вызывающий Init()
CPartOfWordsSearch::CPartOfWordsSearch(CONST _TCHAR * pszPathToDataFiles, DBMS::IDataTable * pIDataTable) : 
	m_pTreeData(NULL),
	m_pIdData(NULL),
	m_pIDataTable(NULL)   {

	Init(pszPathToDataFiles, pIDataTable);
}

/// Инициализация путь к файлам данных и указатель на интерфейс таблицы IDataTable (для вызова Create())
BOOL CPartOfWordsSearch::Init(CONST _TCHAR * pszPathToDataFiles, DBMS::IDataTable * pIDataTable)   {
	m_pIDataTable      = pIDataTable;

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

/// Деструктор
CPartOfWordsSearch::~CPartOfWordsSearch()   {
	Clear();
}

/// Cоздает файлы, содержащие дерево и массив поиска
BOOL CPartOfWordsSearch::Create(VOID)   {

   DEBUG_MSG_LOG2( "Подготавливаем данные " << Convert(m_sPathToDataFiles, std::string()) );

   
   if(m_pIDataTable==NULL)   {
		LogFail(_F,"Не вызывался метод Init().");
		return(FALSE);
	}

	m_fReadData();

	Clear();
	if((m_pTreeData=(BYTE *)malloc(TREE_MAX_SIZE))==NULL)   {
		LogFail(_F,"Не выделена память для данных дерева поиска (%ld).",TREE_MAX_SIZE);
		return(FALSE);
	}

	BOOL        bRet;
	HANDLE      hFHandle;
	STLSTR      sFullPath;
	

	m_pTPtr = m_pTreeData;

	typedef std::pair<std::wstring,UINT32> TPair;

	std::set<TPair>           oPairSet;
	std::vector<UINT>         oVectOfAllowedIdxs;
	UINT                      iNumOfEmptyLfs = 0; 
	UINT                      iLfSumLen      = 0; 
	UINT                      iIdNum         = 0; 
	BYTE                    * pNumOfEmptyLfs = m_pTPtr;

	m_pTPtr+=sizeof(UINT16);

	for(UINT i=0; i<m_oLfVecor.size(); i++)   {

		iIdNum     =     std::max(iIdNum,m_oIdVecor[i]);
		iLfSumLen += 2 * UINT(m_oLfVecor[i].length() + 1);

		oPairSet.insert(std::make_pair(m_oLfVecor[i].c_str(),m_oIdVecor[i]));

		CharLower((wchar_t *)m_oLfVecor[i].c_str());
		
		if(m_oLfVecor[i].length()==0)   {
			SET_MEM_UINT32_AND_INC(m_pTPtr,m_oIdVecor[i]);
			iNumOfEmptyLfs++;
		} else   {
			oVectOfAllowedIdxs.push_back(i);
		}
	}

	if(iIdNum)   iIdNum++;

	MEM_UINT16(pNumOfEmptyLfs)=iNumOfEmptyLfs;

	m_fMakeTreeBranchRecurFunc(1, oVectOfAllowedIdxs);

	m_oLfVecor.clear();
	m_oIdVecor.clear();

	//	wprintf(L"\n OK(%ld)\n\n",UINT(m_pTPtr-m_pTreeData));

	// ------------------------------------------------------------------------------------------
	// Save "LF_to_ID.dta"
	bRet = FALSE;
	sFullPath = m_sPathToDataFiles + _T("LF_to_ID.dta");
   
   DEBUG_MSG_LOG2( "Создаём файл " << Convert(sFullPath, std::string()) );
	
   hFHandle=CreateFile(sFullPath.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, NULL);
	if(hFHandle==INVALID_HANDLE_VALUE)   {
		LogFail(_F,"Ошибка создания файла данных LF_to_ID.dta.");
	} else   {
		DWORD iTreeSize = DWORD(m_pTPtr-m_pTreeData);	
		DWORD iWritten;
		if(!WriteFile(hFHandle,m_pTreeData,iTreeSize,&iWritten,NULL) || iTreeSize!=iWritten)   {
			LogFail(_F,"Ошибка записи в файл данных LF_to_ID.dta.");
		} else   {
			bRet=TRUE; // OK
		}
		CloseHandle(hFHandle);
	}

	if(!bRet)	{
		Clear();
		return(FALSE);
	}

	// ------------------------------------------------------------------------------------------
	// Make & Save "ID_to_LF.dta"

	bRet = FALSE;
	BYTE * pMem;
	m_iIdDataSize = sizeof(UINT32) + iIdNum*sizeof(UINT32) + iLfSumLen;
	if((pMem = (BYTE *)calloc(1, m_iIdDataSize))==NULL)   {
		LogFail(_F,"Не выделена память для данных массива поиска (%ld).",m_iIdDataSize);
	} else   {
		std::set<TPair>::iterator oPairItr    = oPairSet.begin();
		std::wstring              sLastLf     = L"#@-+|";
		UINT32                    iLastId;

		UINT32                  * pIdArr      = (UINT32 *)(pMem + sizeof(UINT32));
		BYTE                    * pLfPtrStart = (BYTE   *)(pMem + sizeof(UINT32) + iIdNum*sizeof(UINT32));
		BYTE                    * pLfPtr      = pLfPtrStart;

		MEM_UINT32(pMem) = iIdNum;

		for( ; oPairItr!=oPairSet.end(); oPairItr++)   {
			if(sLastLf == (*oPairItr).first)   {
				pIdArr[(*oPairItr).second]=pIdArr[iLastId]; // wprintf(L"%ld %s\n",(*oPairItr).second,(*oPairItr).first.c_str());
			} else   {
				sLastLf = (*oPairItr).first;
				iLastId = (*oPairItr).second;

				UINT iLfLen = 2*UINT(sLastLf.length()+1);

				pIdArr[iLastId]=UINT32(pLfPtr-pLfPtrStart)+1;
				memcpy(pLfPtr,sLastLf.c_str(),iLfLen);
				pLfPtr+=iLfLen;
			}
		}

		sFullPath = m_sPathToDataFiles + _T("ID_to_LF.dta");

      DEBUG_MSG_LOG2( "Создаём файл " << Convert(sFullPath, std::string()) );

		hFHandle=CreateFile(sFullPath.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, NULL);
		if(hFHandle==INVALID_HANDLE_VALUE)   {
			LogFail(_F,"Ошибка создания файла данных ID_to_LF.dta.");
		} else   {
			DWORD iMemSize = DWORD(pLfPtr-pMem);	
			DWORD iWritten;
			if(!WriteFile(hFHandle,pMem,iMemSize,&iWritten,NULL) || iMemSize!=iWritten)   {
				LogFail(_F,"Ошибка записи в файл данных ID_to_LF.dta.");
			} else   {
				bRet=TRUE;
			}
			CloseHandle(hFHandle);
		}
		free(pMem);
	}

	if(!bRet)	{
		Clear();
		return(FALSE);
	} else   {
		return(TRUE);
	}
}

/// Загружает/перезагружает словарь
BOOL CPartOfWordsSearch::Load(VOID)   {
	BOOL        bRet;
	HANDLE      hFHandle;
	STLSTR      sFullPath;

	if(m_sPathToDataFiles.length()==0)   {
		LogFail(_F,"Не вызывался метод Init().");
		return(FALSE);
	}

	//if(m_pTreeData || m_pIdData)   {
	//	LogMess("Повторная загрузка данных для поиска.");
	//}

   Clear();

	// ------------------------------------------------------------------------------------------
	// Load "LF_to_ID.dta"

	bRet=FALSE;
	sFullPath = m_sPathToDataFiles + _T("LF_to_ID.dta");
	hFHandle=CreateFile(sFullPath.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, NULL);
	if(hFHandle!=INVALID_HANDLE_VALUE)   {
		DWORD iRead;
		m_iTreeDataSize=GetFileSize(hFHandle,NULL);
		if((m_pTreeData=(BYTE *)malloc(m_iTreeDataSize))!=NULL)   {
			if(!ReadFile(hFHandle,m_pTreeData,m_iTreeDataSize,&iRead,NULL) || m_iTreeDataSize!=iRead)   {
				LogFail(_F,"Ошибка чтения файла данных LF_to_ID.dta.");
			} else   {
				m_pTPtr = m_pTreeData;
				m_fRectifyTreePtrs_RecurFunc(m_pTPtr);
				bRet=TRUE; // OK
			}
		} else   {
			LogFail(_F,"Не выделена память для данных дерева поиска (%ld bytes).",m_iTreeDataSize);
		}
		CloseHandle(hFHandle);
	} else   {
		LogFail(_F,"Ошибка открытия файла данных LF_to_ID.dta (Err:%ld).",GetLastError());
	}

	if(!bRet)	{
		Clear();
		return(FALSE);
	}


	// ------------------------------------------------------------------------------------------
	// Load "ID_to_LF.dta"

	bRet=FALSE;
	sFullPath = m_sPathToDataFiles + _T("ID_to_LF.dta");
	hFHandle=CreateFile(sFullPath.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, NULL);
	if(hFHandle!=INVALID_HANDLE_VALUE)   {
		DWORD iRead;
		m_iIdDataSize=GetFileSize(hFHandle,NULL);
		if((m_pIdData=(BYTE *)malloc(m_iIdDataSize))!=NULL)   {
			if(!ReadFile(hFHandle,m_pIdData,m_iIdDataSize,&iRead,NULL) || m_iIdDataSize!=iRead)   {
				LogFail(_F,"Ошибка чтения файла данных ID_to_LF.dta.");
			} else   {
				m_iIdNum=MEM_UINT32(m_pIdData);
				m_pIdArr=(UINT32 *)(m_pIdData+sizeof(m_pIdData));
				bRet=m_fRectifyIdArrPtrs_IterFunc(m_pIdArr); // !!!
			}
		} else   {
			LogFail(_F,"Не выделена память для данных массива поиска (%ld).",m_iIdDataSize);
		}
		CloseHandle(hFHandle);
	} else   {
		LogFail(_F,"Ошибка открытия файла данных ID_to_LF.dta (Err:%ld).",GetLastError());
	}

	if(!bRet)	{
		Clear();
		return(FALSE);
	} else   {
		return(TRUE);
	}
}
///////////////////////////////////////////////////////////////////////////////

BOOL CPartOfWordsSearch::LoadIfFilesExists()
{
   STLSTR sFullPath1 = m_sPathToDataFiles + _T("LF_to_ID.dta");
   STLSTR sFullPath2 = m_sPathToDataFiles + _T("ID_to_LF.dta");

   if( !::PathFileExists(sFullPath1.c_str()) || !::PathFileExists(sFullPath2.c_str()) )
      if( !Create() )
         return false;
         
   return Load();
}

/// Загружает словарь только если он ещё не был загружен
BOOL CPartOfWordsSearch::LoadIfNeed()
{
   if( Empty() ) return Load();
   return TRUE;
}

// Поиск в словаре всех возможных гипотез о слове, при этом гипотезой считается любое слово, у которого с искомым словом совпадают несколько символов с конца.
BOOL CPartOfWordsSearch::GetHypothesis(
														IN CONST WCHAR * pwszWord,
														IN INT iBeginPosition,
														OUT std::vector< SHypothesisInfo > * p_v_HypothesisInfo,
														IN INT iMinMatchSymbols)   {

	#define MAX_LETTERS_IN_WORD      256
	BYTE            * aTreeBranchesArr[      MAX_LETTERS_IN_WORD];
	INT               aTreeBranchesLetterIdx[MAX_LETTERS_IN_WORD];
	INT               iTreeBranchesNum = 0;

	BYTE            * pTree = m_pTreeData;

	m_p_v_HypothesisInfo = p_v_HypothesisInfo;
	m_p_v_HypothesisInfo->clear();

	if(pTree==NULL)   {
		LogFail(_F,"Дерево поиска не было загружено.");
		return(FALSE);
	}

	pTree += sizeof(UINT16) + ( MEM_UINT16(pTree) << 2 ); // Num empty + iNumEmpty * sizeof(UINT32);

	for(int i=iBeginPosition; i>=0; i--)   {

		UINT iLettersNum = MEM_UINT16(pTree);	
		INT  iLetterIdx  = m_fFindLetterIdx(pwszWord[i],(wchar_t *)(pTree+sizeof(UINT16)),iLettersNum);

		if((iBeginPosition-i+1)>=iMinMatchSymbols)   {
			aTreeBranchesArr[      iTreeBranchesNum  ] = pTree;
			aTreeBranchesLetterIdx[iTreeBranchesNum++] = iLetterIdx;
		}

		if(iLetterIdx<0)   { // Letter was not found
			break;
		}

		BYTE * pIdBlock = (BYTE *)NULL+MEM_UINT32(pTree + sizeof(UINT16) + ( iLettersNum << 1 ) + ( iLetterIdx << 2 )); // BYTE * pIdBlock = MEM_UINT32(pLink) + m_pTreeData;
		UINT   iIdNum   = MEM_UINT16(pIdBlock); 
		
		pIdBlock+=sizeof(UINT16);	

		pTree = pIdBlock + ( iIdNum << 2 );
	}

	m_oHypTreeBranchSet.clear();
	for(int i=iTreeBranchesNum-1; i>=0; i--)   {
		m_iHypLetterIdx    = aTreeBranchesLetterIdx[i];
		m_iHypMatchSymbols = iMinMatchSymbols+i-1;

		if(i)   {
			m_fCollectIdsForHypothesis_RecurFunc(aTreeBranchesArr[i],0);
		} else   {
			m_fCollectIdsForHypothesis_RecurFunc_ForLast(aTreeBranchesArr[i]);
		}
	}
	m_oHypTreeBranchSet.clear();

	return(m_p_v_HypothesisInfo->size()!=0);
}

VOID CPartOfWordsSearch::m_fCollectIdsForHypothesis_RecurFunc(BYTE * pMem, INT iLevel)   {
	
	m_oHypTreeBranchPair = m_oHypTreeBranchSet.insert(pMem);
	if(!m_oHypTreeBranchPair.second)   return;

	UINT iLettersNum = MEM_UINT16(pMem);
	if(iLettersNum)   {
		UINT32 * pLinksArr = (UINT32 *)(pMem + sizeof(UINT16) + (iLettersNum << 1)); // Skip LettersNum & Letters
		for(UINT i=0; i<iLettersNum; i++,pLinksArr++)   {

			if(iLevel==0)   m_oHypothesisInfo.MatchSymbols = m_iHypMatchSymbols + (i==m_iHypLetterIdx?1:0);

			BYTE * pIdsArr = (BYTE *)NULL+MEM_UINT32(pLinksArr);
			UINT   iIdsNum = MEM_UINT16(pIdsArr); pIdsArr += sizeof(UINT16);
			for(UINT i=0; i<iIdsNum; i++)   {
				m_oHypothesisInfo.Id = MEM_UINT32(((UINT32 *)pIdsArr)+i);
				m_p_v_HypothesisInfo->push_back(m_oHypothesisInfo);
			}
			m_fCollectIdsForHypothesis_RecurFunc(/*(BYTE *)NULL+*/pIdsArr+(iIdsNum << 2),iLevel+1);
		}
	}
}

VOID CPartOfWordsSearch::m_fCollectIdsForHypothesis_RecurFunc_ForLast(BYTE * pMem)   {

	if(m_iHypLetterIdx>=0)   {
		m_oHypothesisInfo.MatchSymbols = m_iHypMatchSymbols + 1;

		m_oHypTreeBranchPair = m_oHypTreeBranchSet.insert(pMem);
		if(!m_oHypTreeBranchPair.second)   return;

		UINT iLettersNum = MEM_UINT16(pMem);
		if(iLettersNum)   {
			UINT32 * pLinksArr = (UINT32 *)(pMem + sizeof(UINT16) + (iLettersNum << 1) + m_iHypLetterIdx * sizeof(UINT32)) ; // Skip LettersNum & Letters
			BYTE   * pIdsArr   = (BYTE *)NULL+MEM_UINT32(pLinksArr);
			UINT   iIdsNum     = MEM_UINT16(pIdsArr); pIdsArr += sizeof(UINT16);
			for(UINT i=0; i<iIdsNum; i++)   {
				m_oHypothesisInfo.Id = MEM_UINT32(((UINT32 *)pIdsArr)+i);
				m_p_v_HypothesisInfo->push_back(m_oHypothesisInfo);
			}
			m_fCollectIdsForHypothesis_RecurFunc(/*(BYTE *)NULL+*/pIdsArr+(iIdsNum << 2),1);
		}
	}
}

/// Ищет все возможные пары частей слов и их индексов
BOOL CPartOfWordsSearch::GetByForm(
												IN CONST WCHAR                 * pwszWord,
												IN INT                           iBeginPosition,
												OUT std::vector< SPartOfWord > * p_v_PartOfWord,
												IN BOOL                          isCompleteWord /* = FALSE */)   {
	
	BYTE            * pTree = m_pTreeData;
	UINT32          * pIdPtr;
	UINT              iIdNum;
//	const wchar_t   * pwszWord = pwsPartOfWord->c_str();

   m_pTLastPtr = NULL;

	p_v_PartOfWord->clear();

	if(pTree==NULL)   {
		LogFail(_F,"Дерево поиска не было загружено.");
		return(FALSE);
	}

	// if(iBeginPosition >= UINT(wcslen(pwszWord)))   iBeginPosition = UINT(wcslen(pwszWord))-1);

	UINT iNumEmpty = MEM_UINT16(pTree);

	if(iBeginPosition<0 || !isCompleteWord)   {
		pIdPtr = (UINT32 *)(pTree + sizeof(UINT16));
		for(UINT n=0; n<iNumEmpty; n++)   {
			m_oPartOfWord.Id = pIdPtr[n];
			m_oPartOfWord.PositionInWord = -1;
			p_v_PartOfWord->push_back(m_oPartOfWord);
		}
		if(iBeginPosition<0 || isCompleteWord)   return(p_v_PartOfWord->size()!=0);
	}

	pTree += sizeof(UINT16) + ( iNumEmpty << 2 ); // + iNumEmpty * sizeof(UINT32);

	for(int i=iBeginPosition; i>=0; i--)   {

		UINT iLettersNum = MEM_UINT16(pTree);	
		INT  iLetterIdx  = m_fFindLetterIdx(pwszWord[i],(wchar_t *)(pTree+sizeof(UINT16)),iLettersNum);

		if(iLetterIdx<0)   { // Letter was not found
			return(p_v_PartOfWord->size()!=0);
		}

		BYTE * pIdBlock    = (BYTE *)NULL+MEM_UINT32(pTree + sizeof(UINT16) + ( iLettersNum << 1 ) + ( iLetterIdx << 2 )); // BYTE * pIdBlock = MEM_UINT32(pLink) + m_pTreeData;

		iIdNum = MEM_UINT16(pIdBlock); pIdBlock+=sizeof(UINT16);	
		pIdPtr = NULL;

		if(i==0)   { // Final letter

			pIdPtr = (UINT32 *)pIdBlock;

		} else   {  // NOT Final letter

			if(!isCompleteWord)   pIdPtr = (UINT32 *)pIdBlock;

		}

		pTree = pIdBlock + ( iIdNum << 2 );

		if(pIdPtr && iIdNum)   {
			for(UINT n=0; n<iIdNum; n++)   {
				m_oPartOfWord.Id = pIdPtr[n];
				m_oPartOfWord.PositionInWord = i; // 0
				p_v_PartOfWord->push_back(m_oPartOfWord);
			}
		}
	}

	m_pTLastPtr = pTree;

	return(p_v_PartOfWord->size()!=0);
}

/// Ищет часть слова по индексу
BOOL CPartOfWordsSearch::GetFormById(
												 IN UINT            iID, 
												 OUT std::wstring * pwsPartOfWord)   {

	if(m_pIdData==NULL)   {
		LogFail(_F,"Массив поиска не был загружен.");
		pwsPartOfWord->clear();
		return(FALSE);
	}

	iID &= 0x00FFFFFF;

	if(iID>=m_iIdNum)   {
		LogFail(_F,"Заданный ID превосходит размер массива поиска ( %ld >= %ld ).",iID,m_iIdNum);
		pwsPartOfWord->clear();
		return(FALSE);
	}

	#pragma warning( disable : 4312 ) 
	wchar_t * pwszStrPtr = (wchar_t *)(m_pIdArr[iID]);
	#pragma warning( default : 4312 )

	if(pwszStrPtr==NULL)   {
		LogFail(_F,"Заданный ID не существует ( %ld ).",iID);
		pwsPartOfWord->clear();
		return(FALSE);
	}

	pwsPartOfWord->assign(pwszStrPtr);

	return(TRUE);
}

BOOL CPartOfWordsSearch::Clear(VOID)   {
	if(m_pTreeData)   {
		free(m_pTreeData);
		m_pTreeData=NULL;
	}
	if(m_pIdData)   {
		free(m_pIdData);
		m_pIdData=NULL;
	}

	return(TRUE);
}
