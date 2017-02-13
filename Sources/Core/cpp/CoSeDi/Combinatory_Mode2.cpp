#include "StdAfx.h"
#include <set>
#include ".\Combinatory.h"
#include "..\ASCInterface\IBaseManager.h"
#include "..\ASSInterface\TBaseNames.h"

using namespace SS::CoSeDi::DBMS;
using namespace SS::Dictionary::DATA_TYPE::NAMES;

extern UINT32 _GetCrc32_20Bytes_2(UINT32 * paIDSourceArr, UINT32 iMembersNum, UINT32 * piRealMembersNum = NULL);

BOOL IsItHyphen(UINT32 iIDSource)   {
	if((iIDSource>>24) == ednSymbols)   {
		iIDSource &= 0x00FFFFFF;
		if(iIDSource==0x002d || iIDSource==0x005f || iIDSource==0x0096 || iIDSource==0x0097 || iIDSource==0x00ad ||
			iIDSource==0x2013 || iIDSource==0x2014 || iIDSource==0x0015 || iIDSource==0x2212)   {
			return(TRUE);
		}
	}
	return(FALSE);
}

//

bool CCombinatory::GetCombinationsInfo_2(
					/*in */ TListOfSetsOfSourcesAndFormsItr itrBegin_ListOfSetsOfSourcesAndForms,
					/*in */ TListOfSetsOfSourcesAndFormsItr itrEnd_ListOfSetsOfSourcesAndForms,
					/*out*/ std::list<SCombinationNewInfo> * plistCombinationInfo)   {

	UINT iNumOfWordsToProcess = 0;
	TListOfSetsOfSourcesAndFormsItr iClauseIt = itrBegin_ListOfSetsOfSourcesAndForms;
	for( ; iClauseIt!=itrEnd_ListOfSetsOfSourcesAndForms; iClauseIt++)   {
		iNumOfWordsToProcess++;
	}
	return(GetCombinationsInfo_2(itrBegin_ListOfSetsOfSourcesAndForms,iNumOfWordsToProcess,plistCombinationInfo));
}


bool CCombinatory::GetCombinationsInfo_2(
	/*in */ TListOfSetsOfSourcesAndFormsItr itrStartItr_In_ListOfSetsOfSourcesAndForms,
	/*in */ UINT iNumOfWordsToProcess,
	/*out*/ std::list<SCombinationNewInfo> * plistCombinationInfo)   {

	SS_TRY;

	zm_plistCombinationInfo = plistCombinationInfo;
	zm_plistCombinationInfo->clear();

	TListOfSetsOfSourcesAndFormsItr iClauseIt         = itrStartItr_In_ListOfSetsOfSourcesAndForms;

	iNumOfWordsToProcess--;

	zm_iPosInClauseCnt = 0;	

	for( ; zm_iPosInClauseCnt<iNumOfWordsToProcess; zm_iPosInClauseCnt++,iClauseIt++)   { 
		zm_SubListForCheck.clear();			
		int                             iMembersNum   = 0;
		TListOfSetsOfSourcesAndFormsItr iSubClauseItr = iClauseIt;
		for(UINT m=0; m<MAX_NUM_OF_COMBINATION_MEMBERS; m++,iSubClauseItr++)   {
			if((zm_iPosInClauseCnt+m)<=iNumOfWordsToProcess)   {
				if((*iSubClauseItr).size())   {
					zm_SubListForCheck.push_back(&(*iSubClauseItr));
					iMembersNum++;
				} else   {
					break;
				}
			} else  {
				break;
			}
		}

		if(iMembersNum>1)   {
			for(int m=iMembersNum-1; m>0; m--)   {
				if(zm_fCheckCombination_2())   {
					for(int i=0; i<m; i++)   iClauseIt++;
					zm_iPosInClauseCnt+=m;
					break;
				}
				zm_SubListForCheck.pop_back();
			}
		}
	}

	SS_CATCH(L"");

	return(zm_plistCombinationInfo->size()!=0);
}

BOOL CCombinatory::zm_fCheckCombination_2(VOID)   {

	int iOldlistCombinationSize=INT(zm_plistCombinationInfo->size());

	zm_SubListForCheckSise  = INT(zm_SubListForCheck.size());
	zm_SubListForCheckItr   =     zm_SubListForCheck.begin();

	zm_fRecursiveFunc_2(0);

	return(iOldlistCombinationSize!=INT(zm_plistCombinationInfo->size()));
}

BOOL CCombinatory::zm_fRecursiveFunc_2(UINT iLevel)   {

	BOOL bRet=FALSE;

	std::set< UINT > oUsedIDSorcesSet;

	if(iLevel<zm_SubListForCheckSise)   {
		TSetOfSourcesAndFormsItr itrSetOfSourcesAndForms = (*zm_SubListForCheckItr)->begin();
		for(; itrSetOfSourcesAndForms!=(*zm_SubListForCheckItr)->end(); itrSetOfSourcesAndForms++)   {
			if(oUsedIDSorcesSet.find((*itrSetOfSourcesAndForms).IDSource)==oUsedIDSorcesSet.end())   {
//				if((iLevel!=0 || zm_setExistingFirstSourcesAndForms.find((*itrSetOfSourcesAndForms).IDSource | (zm_SubListForCheckSise<<21))!=zm_setExistingFirstSourcesAndForms.end()))   {
					zm_SubListIDSourceArr[iLevel]=((*itrSetOfSourcesAndForms).IDSource) & 0x7FFFFFFF;
					zm_SubListIDFormArr[  iLevel]=((*itrSetOfSourcesAndForms).IDForm);
					zm_SubListForCheckItr++;
					if(zm_fRecursiveFunc_2(iLevel+1))   {
						oUsedIDSorcesSet.insert((*itrSetOfSourcesAndForms).IDSource);
						bRet=TRUE;
					}
					zm_SubListForCheckItr--;
//				}
			}
		}
	} else   {

		// Расчет HASH-ключа
		UINT32 iRealMembersNum;
		UINT32 iHashKey_2 = _GetCrc32_20Bytes_2(zm_SubListIDSourceArr, iLevel, &iRealMembersNum);

		//if(iRealMembersNum!=iLevel)   {
		//	printf(" # ");
		//}

		//
//		printf("\n iHashKey_2: %lX (Lev:%ld, Sz:%ld) ",iHashKey_2,iLevel,zm_setExistingHashKeys_2.size());		
		if(zm_setExistingHashKeys_2.find(iHashKey_2)!=zm_setExistingHashKeys.end())   {
//		printf("\n !!! iHashKey_2 Exist! ");		
			IDataTable * pDataTable;
			UINT32       iDictMask;
			switch(zm_SubListIDSourceArr[0]>>24)   {

				//case ednSymbols:

				//break;

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

			zm_oCombinationInfo.HashKey_2 = iHashKey_2;
			while(pDataTable->Find(efmByHashKey_2)==S_OK)   {

				if(iRealMembersNum==zm_oCombinationInfo.NumOfMembers)   {

					UINT32 TmpSubListIDSourceArr[MAX_NUM_OF_COMBINATION_MEMBERS], iCnt=0; 
					memcpy(TmpSubListIDSourceArr,zm_SubListIDSourceArr,sizeof(TmpSubListIDSourceArr));
					
					UINT j = 0;
					for(UINT i=0; i<iRealMembersNum; i++)   {
						for(j=0; j<iLevel; j++)   {
							if(zm_oCombinationInfo.WordIndexes[i].IDSource == TmpSubListIDSourceArr[j] /* &&	(zm_oCombinationInfo.WordIndexes[i].IDForm==-1 || zm_oCombinationInfo.WordIndexes[i].IDForm==zm_SubListIDFormArr[j])*/)   {
								TmpSubListIDSourceArr[j] = 0;
								iCnt++;
								break; // Good
							}
						}
						if(j==iLevel)   { // Member not found
							break;
						}
					}

					if(iCnt==iRealMembersNum)   { // Wanted combination !
						zm_oCombinationInfo.PosInClause  = zm_iPosInClauseCnt;
						zm_oCombinationInfo.PosOccupied  = iLevel;
						zm_oCombinationInfo.ID          |= iDictMask;
						zm_plistCombinationInfo->push_back(zm_oCombinationInfo);
						bRet=TRUE;
//						printf("CC: Level: %ld \n",iLevel);
//						break;
					}
				}
			}
		}
	}

	return(bRet);
}

