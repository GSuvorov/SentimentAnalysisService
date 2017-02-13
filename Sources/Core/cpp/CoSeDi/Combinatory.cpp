#include "StdAfx.h"
#include <set>
#include ".\Combinatory.h"
#include "..\ASCInterface\IBaseManager.h"
#include "..\ASSInterface\TBaseNames.h"

using namespace SS::CoSeDi::DBMS;
using namespace SS::Dictionary::DATA_TYPE::NAMES;


// -------------------------------------------------------------
// Запись сообщения об ошибке в журнал системы
HRESULT SS::CoSeDi::DBMS::CombiErr(WCHAR * pFormatStr, ...)   {

	WCHAR   ErrStr[1024];
	va_list ArgList;
	int     Len=0;

	va_start(ArgList,pFormatStr);
		Len = swprintf( ErrStr,L"   COMBINATORY:  ");
		Len+=vswprintf( ErrStr+wcslen(ErrStr),pFormatStr,ArgList);
		#ifdef _DEBUG
			wprintf(L"%s\n",ErrStr);
			Beep(300,100);
		#endif
		SS_THROW(ErrStr);
	va_end(ArgList);

	return(E_FAIL);
}


// -------------------------------------------------------------
// Класс CCombinatory

void CCombinatory::SetLoadManager(ILoadManager * pLoadManager)   {
	zm_pLoadManager = pLoadManager;
}

BOOL CCombinatory::InitConnection(WCHAR * pwszDataTablesName, BOOL bExtraTables)   {
	SS_TRY;


	   m_bExtraTables = bExtraTables;

		zm_ConnectEng.InitConnection(zm_pLoadManager, SS::MANAGER::Constants::c_szCoSeDiEng, pwszDataTablesName);
		zm_ConnectRus.InitConnection(zm_pLoadManager, SS::MANAGER::Constants::c_szCoSeDiRus, pwszDataTablesName);

		IDataTable * pDataTableEng = zm_ConnectEng.GetDataTable();
		IDataTable * pDataTableRus = zm_ConnectRus.GetDataTable();

		memset(zm_apTableRecord,0,sizeof(zm_apTableRecord));
		zm_apTableRecord[1 ] = &zm_oCombinationInfo.HashKey; 
		zm_apTableRecord[2 ] = &zm_oCombinationInfo.HashKey_2; 
		zm_apTableRecord[5 ] = &zm_oCombinationInfo.NumOfMembers;								
		zm_apTableRecord[6 ] = &zm_oCombinationInfo.WordIndexes[0].IDSource;
		zm_apTableRecord[8 ] = &zm_oCombinationInfo.WordIndexes[1].IDSource;
		zm_apTableRecord[10] = &zm_oCombinationInfo.WordIndexes[2].IDSource;
		zm_apTableRecord[12] = &zm_oCombinationInfo.WordIndexes[3].IDSource;
		zm_apTableRecord[14] = &zm_oCombinationInfo.WordIndexes[4].IDSource;

		if(pDataTableEng->ScanInit(0,zm_apTableRecord)!=S_OK)   CombiErr(L"Ошибка инициализации сканирования в таблице ENG Combinatory по индексу комбинации");
		while(pDataTableEng->Scan()==S_OK)   {
			zm_setExistingHashKeys.insert(  zm_oCombinationInfo.HashKey);				
			zm_setExistingHashKeys_2.insert(zm_oCombinationInfo.HashKey_2);
			zm_setExistingFirstSourcesAndForms.insert(zm_oCombinationInfo.WordIndexes[0].IDSource | (zm_oCombinationInfo.NumOfMembers<<21));					
		}

		if(pDataTableRus->ScanInit(0,zm_apTableRecord)!=S_OK)   CombiErr(L"Ошибка инициализации сканирования в таблице RUS Combinatory по индексу комбинации");
		while(pDataTableRus->Scan()==S_OK)   {
			zm_setExistingHashKeys.insert(  zm_oCombinationInfo.HashKey);
			zm_setExistingHashKeys_2.insert(zm_oCombinationInfo.HashKey_2);
			zm_setExistingFirstSourcesAndForms.insert(zm_oCombinationInfo.WordIndexes[0].IDSource | (zm_oCombinationInfo.NumOfMembers<<21));					
			
		}
		
		zm_apTableRecord[0 ] = &zm_oCombinationInfo.ID; 
		zm_apTableRecord[1 ] = NULL; // &zm_oCombinationInfo.HashKey; 
		zm_apTableRecord[2 ] = NULL; // &zm_oCombinationInfo.HashKey_2; 
		zm_apTableRecord[3 ] = &zm_oCombinationInfo.CombiType; 
		zm_apTableRecord[4 ] = &zm_oCombinationInfo.IDType;
//		zm_apTableRecord[5 ] = &zm_oCombinationInfo.NumOfMembers;            // Redundant
//		zm_apTableRecord[6 ] = &zm_oCombinationInfo.WordIndexes[0].IDSource; // Redundant
		zm_apTableRecord[7 ] = &zm_oCombinationInfo.WordIndexes[0].IDForm;
//		zm_apTableRecord[8 ] = &zm_oCombinationInfo.WordIndexes[1].IDSource;
		zm_apTableRecord[9 ] = &zm_oCombinationInfo.WordIndexes[1].IDForm;
//		zm_apTableRecord[10] = &zm_oCombinationInfo.WordIndexes[2].IDSource;
		zm_apTableRecord[11] = &zm_oCombinationInfo.WordIndexes[2].IDForm;
//		zm_apTableRecord[12] = &zm_oCombinationInfo.WordIndexes[3].IDSource;
		zm_apTableRecord[13] = &zm_oCombinationInfo.WordIndexes[3].IDForm;
//		zm_apTableRecord[14] = &zm_oCombinationInfo.WordIndexes[4].IDSource;
		zm_apTableRecord[15] = &zm_oCombinationInfo.WordIndexes[4].IDForm;

		if(pDataTableEng->FindInit(L"ID", &zm_oCombinationInfo.ID, zm_apTableRecord, efmByID)!=S_OK)                  CombiErr(L"Ошибка инициализации поиска в таблице ENG Combinatory по индексу комбинации");
		if(pDataTableEng->FindInit(L"HashKey", &zm_oCombinationInfo.HashKey, zm_apTableRecord, efmByHashKey)!=S_OK)   CombiErr(L"Ошибка инициализации поиска в таблице ENG Combinatory по HASH-ключу");		
		if(pDataTableEng->FindInit(L"HashKey_2", &zm_oCombinationInfo.HashKey_2, zm_apTableRecord, efmByHashKey_2)!=S_OK)   CombiErr(L"Ошибка инициализации поиска в таблице ENG Combinatory по HASH-ключу N2");		

		if(pDataTableRus->FindInit(L"ID", &zm_oCombinationInfo.ID, zm_apTableRecord, efmByID)!=S_OK)                  CombiErr(L"Ошибка инициализации поиска в таблице RUS Combinatory по индексу комбинации");
		if(pDataTableRus->FindInit(L"HashKey", &zm_oCombinationInfo.HashKey, zm_apTableRecord, efmByHashKey)!=S_OK)   CombiErr(L"Ошибка инициализации поиска в таблице RUS Combinatory по HASH-ключу");
		if(pDataTableRus->FindInit(L"HashKey_2", &zm_oCombinationInfo.HashKey_2, zm_apTableRecord, efmByHashKey_2)!=S_OK)   CombiErr(L"Ошибка инициализации поиска в таблице ENG Combinatory по HASH-ключу N2");		


	SS_CATCH(L"");

	return(true);
}


bool CCombinatory::GetCombinationsInfo(
					/*in */ TListOfSetsOfSourcesAndFormsItr itrBegin_ListOfSetsOfSourcesAndForms,
					/*in */ TListOfSetsOfSourcesAndFormsItr itrEnd_ListOfSetsOfSourcesAndForms,
					/*out*/ std::list<SCombinationNewInfo> * plistCombinationInfo)   {

	UINT iNumOfWordsToProcess = 0;
	TListOfSetsOfSourcesAndFormsItr iClauseIt = itrBegin_ListOfSetsOfSourcesAndForms;
	for( ; iClauseIt!=itrEnd_ListOfSetsOfSourcesAndForms; iClauseIt++)   {
		iNumOfWordsToProcess++;
	}
	return(GetCombinationsInfo(itrBegin_ListOfSetsOfSourcesAndForms,iNumOfWordsToProcess,plistCombinationInfo));
}

bool CCombinatory::GetCombinationsInfo(
	/*in */ TListOfSetsOfSourcesAndFormsItr itrStartItr_In_ListOfSetsOfSourcesAndForms,
	/*in */ UINT iNumOfWordsToProcess,
	/*out*/ std::list<SCombinationNewInfo> * plistCombinationInfo)   {

	SS_TRY;

	zm_plistCombinationInfo = plistCombinationInfo;
	zm_plistCombinationInfo->clear();

	TListOfSetsOfSourcesAndFormsItr iClauseIt         = itrStartItr_In_ListOfSetsOfSourcesAndForms;

	iNumOfWordsToProcess--;

	for(zm_iPosInClauseCnt = 0;zm_iPosInClauseCnt < iNumOfWordsToProcess; ++zm_iPosInClauseCnt, ++iClauseIt)
	{ 
		zm_SubListForCheck.clear();			

		int iMembersNum = 0;
		TListOfSetsOfSourcesAndFormsItr iSubClauseItr = iClauseIt;
		for(UINT m = 0; m < MAX_NUM_OF_COMBINATION_MEMBERS; ++m, ++iSubClauseItr)
		{
			if((zm_iPosInClauseCnt + m)<=iNumOfWordsToProcess)
			{
				if((*iSubClauseItr).size())
				{
					zm_SubListForCheck.push_back(&(*iSubClauseItr));
					++iMembersNum;
				}
				else
					break;
			}
			else
				break;
		}

		if(iMembersNum > 1)
		{
			for(int m = iMembersNum - 1; m > 0; --m)
			{
				if(zm_fCheckCombination())
				{
					for(int i=0; i < m; ++i)
						++iClauseIt;
					zm_iPosInClauseCnt += m;
					break;
				}
				zm_SubListForCheck.pop_back();
			}
		}
	}

	SS_CATCH(L"");

	return(zm_plistCombinationInfo->size() != 0);
}

BOOL CCombinatory::zm_fCheckCombination(VOID)   {

	int iOldlistCombinationSize=INT(zm_plistCombinationInfo->size());

	zm_SubListForCheckSise  = INT(zm_SubListForCheck.size());
	zm_SubListForCheckItr   =     zm_SubListForCheck.begin();

	zm_fRecursiveFunc(0);

	return(iOldlistCombinationSize!=INT(zm_plistCombinationInfo->size()));
}

BOOL CCombinatory::zm_fRecursiveFunc(UINT iLevel)   {

	BOOL bRet=FALSE;

	std::set< UINT > oUsedIDSorcesSet;

	if(iLevel<zm_SubListForCheckSise)   {
		TSetOfSourcesAndFormsItr itrSetOfSourcesAndForms = (*zm_SubListForCheckItr)->begin();
		for(; itrSetOfSourcesAndForms!=(*zm_SubListForCheckItr)->end(); itrSetOfSourcesAndForms++)   {
			if(oUsedIDSorcesSet.find((*itrSetOfSourcesAndForms).IDSource)==oUsedIDSorcesSet.end())   {
				if((iLevel!=0 || 
					zm_setExistingFirstSourcesAndForms.find((*itrSetOfSourcesAndForms).IDSource | (zm_SubListForCheckSise<<21))!=zm_setExistingFirstSourcesAndForms.end()))   {
					zm_SubListIDSourceArr[iLevel]=((*itrSetOfSourcesAndForms).IDSource) & 0x7FFFFFFF;
					zm_SubListIDFormArr[  iLevel]=((*itrSetOfSourcesAndForms).IDForm);
					zm_SubListForCheckItr++;
					if(zm_fRecursiveFunc(iLevel+1))   {
						oUsedIDSorcesSet.insert((*itrSetOfSourcesAndForms).IDSource);
						bRet=TRUE;
					}
					zm_SubListForCheckItr--;
				}
			}
		}
	} else   {

		// Расчет HASH-ключа

		static UINT32 Crc32Tbl[256] = {
			0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,   0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,   0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,   0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,   0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,   0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,   0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,   0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,   0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
			0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,   0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,   0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,   0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,   0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,   0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,   0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,   0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,   0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
			0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,   0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,   0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,   0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,   0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,   0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,   0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,   0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,   0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
			0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,   0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,   0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,   0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,   0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,   0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,   0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,   0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,   0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
		};

		UINT32 iHashKey=0;
		unsigned int i = 0;
		unsigned int num = 0;
		for(i = 0, num=iLevel<<2; i<20; i++)   {
			if(i<num)   {
				iHashKey = (((iHashKey) >> 8) ^ Crc32Tbl[((iHashKey) ^ ((BYTE *)zm_SubListIDSourceArr)[i]) & 0xFF]);
			} else   {
				iHashKey = (((iHashKey) >> 8) ^ Crc32Tbl[iHashKey & 0xFF]);	
			}
		}

		//
		
		if(zm_setExistingHashKeys.find(iHashKey)!=zm_setExistingHashKeys.end())   {
			IDataTable * pDataTable;
			UINT32       iDictMask;
			switch(zm_SubListIDSourceArr[0]>>24)   {
				case ednAbbreviationsEng:
				case ednENominalName:
				case ednEProperName:
				case ednUnknownEng:
					pDataTable = zm_ConnectEng.GetDataTable();
					iDictMask  = (m_bExtraTables?(UINT32)ednExtraECombinatory:(UINT32)ednECombinatory) << 24;
				break;
				default:
					pDataTable = zm_ConnectRus.GetDataTable();
					iDictMask  = (m_bExtraTables?(UINT32)ednExtraRCombinatory:(UINT32)ednRCombinatory) << 24;
				break;
			}

			zm_oCombinationInfo.HashKey = iHashKey;
			while(pDataTable->Find(efmByHashKey)==S_OK)   {

				for(i=0; i<iLevel; i++)   {
					//// printf("CC: (%ld-%ld:%ld)\n",zm_SubListIDSourceArr[i] >> 24, zm_SubListIDSourceArr[i] & 0x00FFFFFF, zm_SubListIDFormArr[i]);
					//if(zm_SubListIDSourceArr[i]!=zm_oCombinationInfo.WordIndexes[i].IDSource)   {
					//// printf("CC: BAD IDSource\n");
					//	break;
					//} else   {
					//	if(zm_oCombinationInfo.WordIndexes[i].IDForm!=-1 && zm_oCombinationInfo.WordIndexes[i].IDForm!=zm_SubListIDFormArr[i])  {
					//	// printf("CC: BAD IDForm (%ld:%ld)\n",zm_oCombinationInfo.WordIndexes[i].IDForm,zm_SubListIDFormArr[i]);
					//		break;
					//	}
					//}

					if(zm_SubListIDSourceArr[i]!=zm_oCombinationInfo.WordIndexes[i].IDSource ||
						(zm_oCombinationInfo.WordIndexes[i].IDForm!=-1 && zm_oCombinationInfo.WordIndexes[i].IDForm!=zm_SubListIDFormArr[i]))   {
						break;
					}

				}
				if(i==iLevel)   { // Wanted combination !
					zm_oCombinationInfo.PosInClause  = zm_iPosInClauseCnt;
					zm_oCombinationInfo.PosOccupied  = iLevel;
					zm_oCombinationInfo.ID          |= iDictMask;
					zm_plistCombinationInfo->push_back(zm_oCombinationInfo);
					bRet=TRUE;
//					printf("CC: Level: %ld \n",iLevel);
//					break;
				}
			}
		}
	}

	return(bRet);
}

bool CCombinatory::GetCombinationsByIDSource(/*in*/int iIDSource, /*out*/ std::list<int> * p_listCombinationsIDs)   
{
	SS_TRY;

	p_listCombinationsIDs->clear();

	UINT iDictMask;
	
	IDataTable * pDataTable;
	switch(iIDSource >> 24)   {
		case ednAbbreviationsEng:
		case ednENominalName:
		case ednEProperName:
		case ednUnknownEng:
			pDataTable = zm_ConnectEng.GetDataTable();
			iDictMask  = (m_bExtraTables?(UINT32)ednExtraECombinatory:(UINT32)ednECombinatory) << 24;
		break;
		default:
			pDataTable = zm_ConnectRus.GetDataTable();
			iDictMask  = (m_bExtraTables?(UINT32)ednExtraRCombinatory:(UINT32)ednRCombinatory) << 24;
		break;
	}

	int    iID;
	void * apFindArr[16]; memset(apFindArr,0,sizeof(apFindArr)); apFindArr[0] =&iID;

	for(UINT m=0; m<MAX_NUM_OF_COMBINATION_MEMBERS; m++)   {
		WCHAR wszStr[32];
		wsprintf(wszStr,L"IDSource%ld",m);
		if(pDataTable->FindInit(wszStr,&iIDSource,apFindArr,efmByIDSource)!=S_OK)   {
			CombiErr(L"Ошибка FindInit() в функции <GetCombinationsByIDSource()>.");
		}
		while(pDataTable->Find(efmByIDSource)==S_OK)   {
			p_listCombinationsIDs->push_back(iID | iDictMask);
		}
		if(pDataTable->FindCancel(efmByIDSource)!=S_OK)   {
			CombiErr(L"Ошибка FindCancel() в функции <GetCombinationsByIDSource()>.");
		}
	}

	SS_CATCH(L"");

	return(p_listCombinationsIDs->size()!=0);
}


bool CCombinatory::GetCombinationInfo(/*in*/int iIDCombination, /*out*/ SCombinationNewInfo * p_CombinationInfo)   {	
	SS_TRY;
	
	zm_oCombinationInfo.ID = iIDCombination & 0x00FFFFFF;

	IDataTable * pDataTable;
	if((iIDCombination>>24)==ednECombinatory || (iIDCombination>>24)==ednExtraECombinatory)   {
		pDataTable = zm_ConnectEng.GetDataTable();
	} else    {
		pDataTable = zm_ConnectRus.GetDataTable();
	}

	if(pDataTable->FindOne(efmByID) == S_OK)   {
		memcpy(p_CombinationInfo,&zm_oCombinationInfo,sizeof(SCombinationNewInfo));
		p_CombinationInfo->ID = iIDCombination;
		return(true);
	}

	SS_CATCH(L"");

	return(false);
}


UINT32 Crc32Tbl[256] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,   0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,   0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,   0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,   0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,   0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,   0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,   0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,   0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,   0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,   0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,   0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,   0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,   0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,   0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,   0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,   0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,   0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,   0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,   0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,   0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,   0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,   0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,   0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,   0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,   0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,   0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,   0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,   0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,   0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,   0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,   0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,   0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

UINT32 _GetCrc32_20Bytes(UINT32 * paIDSourceArr)   {
	UINT32  Crc32 = 0;
	BYTE  * pByte = (BYTE *)paIDSourceArr;  
	for(int i=0; i<20; i++)   {
	    Crc32 = (((Crc32) >> 8) ^ Crc32Tbl[((Crc32) ^ *pByte++) & 0xFF]);
	}
	return(Crc32);
}

//UINT32 _GetCrc32_20Bytes_2(UINT32 * paIDSourceArr, UINT32 iMembersNum)   {
//	UINT32  Crc32 = iMembersNum << 24;
//	for(int i=0; i<iMembersNum; i++)   {
//	    Crc32 ^= paIDSourceArr[i] & 0x7FFFFFFF;
//	}
//	return(Crc32);
//}

BOOL IsItHyphen(UINT32 iIDSource);

UINT32 _GetCrc32_20Bytes_2(UINT32 * paIDSourceArr, UINT32 iMembersNum, UINT32 * piRealMembersNum = NULL)   {
	UINT32  Crc32 = 0;
	UINT32  iRealMembersNum = 0;
	for(UINT i=0; i<iMembersNum; i++)   {
//		if((paIDSourceArr[i]>>24)!=0)   printf(" !!!:%ld ", paIDSourceArr[i]>>24);
		if(!IsItHyphen(paIDSourceArr[i]))   {
			Crc32 ^= paIDSourceArr[i] & 0x7FFFFFFF;
			iRealMembersNum++;
		}
	}
	Crc32 ^= (iRealMembersNum << 24);

	if(piRealMembersNum)   *piRealMembersNum = iRealMembersNum;

	return(Crc32);
}


bool CCombinatory::CreateCombination(/*in out*/ SCombinationNewInfoForAdd * p_CombinationInfo)   {			
	SS_TRY;

	UINT32       iDictMask;
	IDataTable * pDataTable;

	switch((p_CombinationInfo->WordIndexes[0].IDSource >> 24) & 0x7F)   {
		case ednAbbreviationsEng:
		case ednENominalName:
		case ednEProperName:
		case ednUnknownEng:
			pDataTable = zm_ConnectEng.GetDataTable();
			iDictMask  = (m_bExtraTables?(UINT32)ednExtraECombinatory:(UINT32)ednECombinatory) << 24;
		break;
		default:
			pDataTable = zm_ConnectRus.GetDataTable();
			iDictMask  = (m_bExtraTables?(UINT32)ednExtraRCombinatory:(UINT32)ednRCombinatory) << 24;
		break;
	}

	int    iID = 1;
	void * apAddArr[16]; memset(apAddArr,0,sizeof(apAddArr));

	if(pDataTable->GetRecordsNumber())   {
		iID = pDataTable->GetUserValue(0);
	}

	UINT32 aIDSourceArr[MAX_NUM_OF_COMBINATION_MEMBERS];
	memset(aIDSourceArr,0,MAX_NUM_OF_COMBINATION_MEMBERS * sizeof(UINT32));
	for(UINT i=0;i<p_CombinationInfo->NumOfMembers && i<MAX_NUM_OF_COMBINATION_MEMBERS; i++)   {
		aIDSourceArr[i]=p_CombinationInfo->WordIndexes[i].IDSource;
	}

	UINT32 iHashKey  = _GetCrc32_20Bytes(aIDSourceArr);
	UINT32 iHashKey2 = _GetCrc32_20Bytes_2(aIDSourceArr,p_CombinationInfo->NumOfMembers);

	apAddArr[0 ] = &iID; 
	apAddArr[1 ] = &iHashKey;
	apAddArr[2 ] = &iHashKey2;
	apAddArr[3 ] = &p_CombinationInfo->CombiType; 
	apAddArr[4 ] = &p_CombinationInfo->IDType;
	apAddArr[5 ] = &p_CombinationInfo->NumOfMembers;								
	apAddArr[6 ] = &p_CombinationInfo->WordIndexes[0].IDSource;
	apAddArr[7 ] = &p_CombinationInfo->WordIndexes[0].IDForm;
	apAddArr[8 ] = &p_CombinationInfo->WordIndexes[1].IDSource;
	apAddArr[9 ] = &p_CombinationInfo->WordIndexes[1].IDForm;
	apAddArr[10] = &p_CombinationInfo->WordIndexes[2].IDSource;
	apAddArr[11] = &p_CombinationInfo->WordIndexes[2].IDForm;
	apAddArr[12] = &p_CombinationInfo->WordIndexes[3].IDSource;
	apAddArr[13] = &p_CombinationInfo->WordIndexes[3].IDForm;
	apAddArr[14] = &p_CombinationInfo->WordIndexes[4].IDSource;
	apAddArr[15] = &p_CombinationInfo->WordIndexes[4].IDForm;

	if(pDataTable->AddInit(apAddArr)!=S_OK)   {
		CombiErr(L"Ошибка AddInit() в функции <CreateCombination()>.");
	} else   {
		if(pDataTable->Add()!=S_OK)   {
			CombiErr(L"Ошибка Add() в функции <CreateCombination()>.");
		}
	}

	pDataTable->SetUserValue(0,iID+1);

	p_CombinationInfo->ID = iID | iDictMask;
	return(true);

	SS_CATCH(L"");

	return(false);
}


bool CCombinatory::DeleteCombination(/*in*/int iIDCombination)   {
	int iCnt=0;

	SS_TRY;

	IDataTable * pDataTable;
	if((iIDCombination>>24)==ednECombinatory || (iIDCombination>>24)==ednExtraECombinatory)   {
		pDataTable = zm_ConnectEng.GetDataTable();
	} else    {
		pDataTable = zm_ConnectRus.GetDataTable();
	}

	zm_oCombinationInfo.ID = iIDCombination & 0x00FFFFFF;

//	printf("  %lX  ",iIDCombination);
	while(pDataTable->Find(efmByID)==S_OK)   {
//		printf(" !!!> %lX <!!! ",iIDCombination);
		pDataTable->DeleteRecord(pDataTable->GetFoundIdx(efmByID));
		iCnt++;
	}

	SS_CATCH(L"");

	return(iCnt!=0);
}

// -------------------------------------------------------------
// Класс CSimpleConnect
// -------------------------------------------------------------

bool CSimpleConnect::InitConnection(ILoadManager * pLoadManager, const wchar_t * pwsBaseName, const wchar_t * pwsTableName)   {

	SS_TRY;

		wcscpy(m_wsBaseName, pwsBaseName);

		m_pDataTable = NULL;

		m_poLingvoBaseManager = (ILingvoBaseManager *) pLoadManager->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);
		if(m_poLingvoBaseManager)   
		{
			m_poLingvoBaseManager->CreateOpenAndRegister(pwsBaseName,&m_pDataBase);
			if(m_pDataBase)   {
				if(m_pDataBase->MoveToDataTable(pwsTableName)==S_OK)   {
					m_pDataTable=m_pDataBase->OpenDataTable();
					if(m_pDataTable)   {
						return(true);
					} else   {
						CombiErr(L"Таблица данных <%s> не открыта.",pwsTableName);
					}
				} else   {
					CombiErr(L"Переход к таблице данных <%s> не выполнен.",pwsTableName);
				}
			} else   {
				CombiErr(L"База данных <%s> не открыта.",pwsBaseName);
			}
		} else   {
			CombiErr(L"Не получен интерфейс <ILingvoBaseManager>.");
		}

	SS_CATCH(L"");

	return(false);
}

void CSimpleConnect::CloseConnection(void)   
{
	if(m_pDataTable)          { 
		m_pDataTable->Close();   
		m_pDataTable = NULL; 
	}
	if(m_poLingvoBaseManager) 
	{
		m_poLingvoBaseManager->UnRegisterDeleteAndClose(m_wsBaseName,&m_pDataBase);
	}
//	if(m_pDataBase)           { m_pDataBase->Close();    m_pDataBase           = NULL; }
//	if(m_poLingvoBaseManager) { m_poLingvoBaseManager    m_poLingvoBaseManager = NULL; }
}

/// Возвращает указатель на IDataTable
IDataTable * CSimpleConnect::GetDataTable(void)   {
	if(m_pDataTable==NULL)   {
		CombiErr(L"Таблица данных для комбинаторики не была открыта.");
	}
   return(m_pDataTable);
}