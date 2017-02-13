#include "StdAfx.h"
#include ".\smnoundetectione.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace English
			{


CSMNounDetectionE::CSMNounDetectionE(void)
{
	m_AnalyseLanguage=SS::Core::Types::ealEnglish;
	m_sDescription=_T(__FUNCTION__);
}

CSMNounDetectionE::~CSMNounDetectionE(void)
{
}
bool CSMNounDetectionE::AnalizeSentence()
{
	SS_TRY
	{
		if(!GetFirstUnit()) return true;
		bool bGood;
		UINT uiCount;
		//TCHAR sz_buff[MAX_CHARS_IN_WORD + 1];
		bool bDeterminer=false;
		IUnit * pPrep;

		do{
			//wstring str = m_pCurrentUnit->GetWord();
			if ( IsSyntax()	) 
				continue;
			if ( IsContainFeature(efEDetPronounAdjective)||
				IsContainFeature(efECaseGenetive) && m_LexemType.IsWord() || 
				m_LexemType.IsDigits()
				)
			{
				if(IsContainFeature(efECaseGenetive) && 
					!IsContainFeature(efEDetPronounAdjective))
				{
					if (GetNextUnit())
					{
						if (m_LexemType.IsSApostrophe())
						{
							if (GetNextUnit())
							{
								//m_pCurrentUnit->GetWord(sz_buff);
								if (m_LexemType.GetLexemLength()>1 || !m_LexemType.WordEqualsTo(_T("s")))
								{
									GetPrevUnit();
								}
							}else continue;
						}else GetPrevUnit();
					}else continue;
				}
				uiCount=0;
				bGood=false;
				bDeterminer=IsDeterminer(m_pCurrentUnit);
				Mem();
				while(bGood==false && GetNextUnit() && m_LexemType.IsWord() &&
					!IsSyntax() && uiCount++<4)
				{
					if (
						IsContainFeature(efcNoun)&&
						!IsContainFeature(efECaseGenetive)&& 
						!IsContainFeature(efcDetectedNoun)&&
						(!IsContainFeature(efcAdverbAdjective)||
						bDeterminer)
						)
					{	
						Mem1();
						GotoMem();
						//wstring str = m_pCurrentUnit->GetWord();
						if (!(pPrep=IsPrepositionBeforeE(1,m_pCurrentUnit)) || (IsOneOf(4)) ||
							(pPrep && pPrep->GetSyntaxFeature() && pPrep->GetSyntaxFeature()->GetSyntaxFeatureApriori()->m_CombiType.IsContain(Values::CombiType.cmbtVerbWithParticle))
							)
						{

							GotoMem1();
							AddFeature(efEDNounNominative);
						}else
						{
							GotoMem1();
							//str = m_pCurrentUnit->GetWord();
							if(!IsVerbBeforeE(1))
							{
								/*if(IsContainFeature(efcVerbWithPrep,pPrep))*/ AddFeature(efEDNounPrep);
								RemoveFeature(efcSemanticCommon);
							}
						}
						bGood=true;
					}else if (IsContainFeature(efcAdverbAdjective))
					{
						continue;
					}else if (
						IsContainFeature(efcVerb)||
						IsContainFeature(efcPretext)
						)
					{
						break;
					}
				}
				if (!bGood) GotoMem();
			}else if ( 
				m_LexemType.IsWord() &&!IsContainFeature(efECaseGenetive)&& !IsContainFeature(efcDetectedNoun) &&
				(IsContainFeature(efEClearProper)||
				!IsContainFeature(efENotOnlyNoun) && 
				IsContainFeature(efcNoun)||
				IsContainFeature(efcSemanticCommon)&&
				!IsContainFeature(efcSemanticVerb))
				)
			{
				//str = m_pCurrentUnit->GetWord();

				if (!(pPrep=IsPrepositionBeforeE(4,m_pCurrentUnit)))
					AddFeature(efEDNounNominative);
				else /*if(IsContainFeature(efcVerbWithPrep,pPrep))*//*if(!IsVerbBeforeE(CalculateDepth(pPrep,1)))*/
				{
					AddFeature(efEDNounPrep);
					RemoveFeature(efcSemanticCommon);
				}
			}
		}while(GetNextUnit());
		return true;
	}
	SS_CATCH(L"")
}

UINT CSMNounDetectionE::CalculateDepth(IUnit * pPrep,const UINT uiLimit)
{
	if(!pPrep || !m_pCurrentUnit) return 0;

	UINT uiDepth;
	UINT uiNounPrep, uiPrep;
	IUnit * pUnit;

	if (m_pCurrentUnit->HasChilds())
	{
		pUnit = m_pCurrentUnit->GetLeftChildUnit();
		uiNounPrep = ((IUnit*)pUnit)->GetPosition();
	}else
	{
		uiNounPrep = ((IUnit*)m_pCurrentUnit)->GetPosition();
	}

	uiPrep = ((IUnit*)pPrep)->GetPosition();
	uiDepth = uiNounPrep - uiPrep - 1;

	if(uiDepth > uiLimit)
		return uiLimit;
	else
		return uiDepth;
}

bool CSMNounDetectionE::IsVerbBeforeE(UINT uiDepth)
{
	Mem2();
	
	UINT uiCount=0;

	while( uiCount++<uiDepth && GetPrevUnit() )
	{
		if(m_LexemType.IsEmpty()) {
			uiCount--;
			continue;
		}
		//wstring str=m_pCurrentUnit->GetWord();
		if(IsContainFeature(efcVerb))
		{
			GotoMem2();
			return true;
		}		
	}
	GotoMem2();
	return false;
}

bool CSMNounDetectionE::IsOneOf(UINT uiDepth)
{
	GotoMem1();
	//wstring str = m_pCurrentUnit ->GetWord();
	UINT uiCount=0;
	while(uiCount++<uiDepth && GetPrevUnit())
	{
		if(m_LexemType.IsEmpty()) {
			uiCount--;
			continue;
		}
		if(IsSyntax() || m_LexemType.IsSComma() || m_LexemType.IsSDot()) 
			break;
		if(m_StringService.StrEqual(m_pCurrentUnit->GetWord(),_T("of")) &&
			GetPrevUnit() && m_StringService.StrEqual(m_pCurrentUnit->GetWord(),_T("one")))
			return true;
	}
	return false;
}



			}
		}
	}
}