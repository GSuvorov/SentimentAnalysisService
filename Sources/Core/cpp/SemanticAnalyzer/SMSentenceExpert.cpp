#include "StdAfx.h"
#include ".\smsentenceexpert.h"

namespace SS
{
namespace Semantic
{

CSMSentenceExpert::CSMSentenceExpert(void)
{
}

CSMSentenceExpert::~CSMSentenceExpert(void)
{
}

bool CSMSentenceExpert::Analize()
{
	SS_TRY
	{
		if(!GetFirstParagraph()) 
			return true;

		do
		{
			if(m_pCurrentParagraph->GetType() == SS::Core::Types::IndexationSearch::tctTable ||
				!GetFirstSentence() || !IsExecuteExpert(m_pCurrentSentence->GetLanguage())) 
			{
				continue;
			}

			do
			{
				if(GetFirstUnit())
				{
					AnalizeSentence();
				}
			}while(GetNextSentence());

		}while(GetNextParagraph());

		return true;
	}
	SS_CATCH(L"")
}
bool CSMSentenceExpert::Init(IText * pIText)
{
	SS_TRY
	{
		if(!CSMExpert::Init(pIText)) return false;
		if(m_AdverbStrings.empty())
		{
			m_AdverbStrings.insert(wstring(_T("every")));
			m_AdverbStrings.insert(wstring(_T("each")));
		}
		if (m_PreDeterminerStrings.empty())
		{
			m_PreDeterminerStrings.insert(wstring(_T("all")));
			m_PreDeterminerStrings.insert(wstring(_T("such")));
			m_PreDeterminerStrings.insert(wstring(_T("quiet")));
		}
		if(m_StopPretexts.empty())
		{
			m_StopPretexts.insert(wstring(_T("as")));
			m_StopPretexts.insert(wstring(_T("than")));
		}
		return true;
	}
	SS_CATCH(L"")

}

IUnit * CSMSentenceExpert::IsPrepositionBeforeE(UINT uiDepth,IUnit * pUnit,bool bPlaceTime)
{
	SS_TRY
	{
		if(!pUnit) return NULL;

		Mem2();

		m_pCurrentUnit=pUnit;
		UINT uiCount=0;
		IUnit * pPrep=NULL;
		while( uiCount++<uiDepth && GetPrevUnit() )
		{
			if(m_LexemType.IsEmpty()) {
				uiCount--;
				continue;
			}
			if(IsSyntax())
				break;
			if(IsContainFeature(efcPretext)&&
				((!IsContainFeature(efcNoun)&&
				!IsContainFeature(efcVerb))||
				bPlaceTime) || IsStopPretextOrAimAdverb(false)
				)
			{
				if(!IsStopPretextOrAimAdverb(true) && !IsUpConjunction())
				{
					pPrep=m_pCurrentUnit;
					GotoMem2();
					return pPrep;
				}else uiCount--;
			}else if(uiDepth==1 && IsPredeterminer(m_pCurrentUnit))//if(m_pCurrentUnit->m_MorphoInfo.IsContainFeature(efVerb))
			{
				uiCount--;
			}else  if ( 
				IsContainFeature(efcAdverbAdjective)||
				IsContainFeature(efEDetPronounAdjective)||
				( IsContainFeature(efECaseGenetive) || SpecialTypeEquals(ustNumeral) ) &&
				/*( m_LexemType.IsEng() ||m_LexemType.IsENG()||m_LexemType.Iseng() ) */m_LexemType.IsWord()|| m_LexemType.IsSApostrophe() || m_LexemType.IsDigits()
				)
			{
				continue;
			}else
				break;
		}
		GotoMem2();
		return pPrep;
	}
	SS_CATCH(L"")

}

bool CSMSentenceExpert::IsUpConjunction()
{
	SS_TRY
	{
		if(m_LexemType.IsUpperCase() && IsConjunction(m_pCurrentUnit))
			return true;
		return false;
	}
	SS_CATCH(L"")
}

bool CSMSentenceExpert::IsPredeterminer(IUnit* pUnit)
{
	SS_TRY
	{
		if(!pUnit)return false;
		set<wstring>::iterator i;
		i=m_PreDeterminerStrings.find(pUnit->GetWord());
		return i!=m_PreDeterminerStrings.end();
	}
	SS_CATCH(L"")
}

bool CSMSentenceExpert::IsConjunction(IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit)return false;

		IIndex * pIndex;
		IFeature * pFeature;
		SS::Core::Features::CMorphoFeature oMorphoFeature;

		if (pIndex=pUnit->GetFirstIndex())
		{
			do{
				if ( pFeature=pIndex->GetFirstFeature() )
				{
					do{
						m_pIAMCMorpho->EnCode(pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber(),pFeature->GetMorphoInfo(),pFeature->GetPartOfSpeech(),&oMorphoFeature);
						if(oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otConjunction))
							return true;
					}while(pFeature=pIndex->GetNextFeature());
				}
			}while(pIndex=pUnit->GetNextIndex());
		}
		return false;
	}
	SS_CATCH(L"")
}

bool CSMSentenceExpert::IsPretext(IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit)return false;

		IIndex * pIndex;
		IFeature * pFeature;
		SS::Core::Features::CMorphoFeature oMorphoFeature;

		if (pIndex=pUnit->GetFirstIndex())
		{
			do{
				if ( pFeature=pIndex->GetFirstFeature() )
				{
					do{
						m_pIAMCMorpho->EnCode(pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber(),pFeature->GetMorphoInfo(),pFeature->GetPartOfSpeech(),&oMorphoFeature);
						if(oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otPretext))
							return true;
					}while(pFeature=pIndex->GetNextFeature());
				}
			}while(pIndex=pUnit->GetNextIndex());
		}
		return false;
	}
	SS_CATCH(L"")
}

bool CSMSentenceExpert::IsDeterminer(IUnit * pUnit)
{
	IIndex * pInd;
	IFeature * pFeature;
	SS::Core::Features::CMorphoFeature oMorphoFeature;

	if (!pUnit) return false;

	if (pInd=pUnit->GetFirstIndex())
	{
		do{
			if (pFeature=pInd->GetFirstFeature() )
			{
				do{
					m_pIAMCMorpho->EnCode(pInd->GetDictionaryIndex()->GetFirst().GetDictionaryNumber(),pFeature->GetMorphoInfo(),pFeature->GetPartOfSpeech(),&oMorphoFeature);
					if(oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otDeterminers))
						return true;
				}while(pFeature=pInd->GetNextFeature());
			}
		}while(pInd=pUnit->GetNextIndex());
	}
	return false;

}

IUnit * CSMSentenceExpert::IsPrepositionBeforeR(UINT uiDepth,IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) return NULL;

		Mem2();

		m_pCurrentUnit=pUnit;

		UINT uiCount=0;
		IUnit * pPrep=NULL;
		wstring str;
		bool bDigit = false;

		while( uiCount++<uiDepth && GetPrevUnit() )
		{
			if(m_LexemType.IsEmpty()) {
				uiCount--;
				continue;
			}
			if(/*SyntaxTypeEquals(usntDateSyntax)||
				SyntaxTypeEquals(usntComplexProperNameSyntax)*/IsSyntax() ||
				m_LexemType.IsSComma())
				break;
			if(IsContainFeature(efcPretext)	)
			{
				str = m_pCurrentUnit->GetWord();
				m_StringService.LowerSz((TCHAR*)str.c_str());
				if(!(m_StringService.StrEqual(_T("около"),str.c_str()) && 
					bDigit))
				{
					pPrep=m_pCurrentUnit;
					GotoMem2();
					return pPrep;
				}
			}else  if ( 
				IsContainFeature(efcAdverbAdjective)||
				SpecialTypeEquals(ustNumeral) || m_LexemType.IsDigits()
				)
			{
				bDigit = true;
				continue;
			}else if(IsContainFeature(efcVerb))
			{
				m_MorphoFeature.Undefine();
				CollectMorphoFeature(m_MorphoFeature);
				if(m_MorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple) && 
					!m_MorphoFeature.m_ShortType.IsContain(Values::ShortType.stShort) )
					continue;
				else break;
			}else break;
		}
		GotoMem2();
		return pPrep;
	}
	SS_CATCH(L"")

}

bool CSMSentenceExpert::FilterSyntax(list<Types::SyntaxCategories> & l_Syntax, bool bTime)
{
	SS_TRY
	{
		list<Types::SyntaxCategories>::iterator i;
		Types::SyntaxCategories oTime, oPlace;

		oTime=Values::SyntaxCategories.scAjunctTimeDuration;
		oTime.AssignOR(&Values::SyntaxCategories.scAjunctTimeFixedPoint);
		oTime.AssignOR(&Values::SyntaxCategories.scAjunctTimeEnd);
		oTime.AssignOR(&Values::SyntaxCategories.scAjunctTimeSource);

		oPlace=Values::SyntaxCategories.scAjunctPlaceFixedPoint;
		oPlace.AssignOR(&Values::SyntaxCategories.scAjunctPlaceSource);
		oPlace.AssignOR(&Values::SyntaxCategories.scAjunctPlaceEnd);

		for(i=l_Syntax.begin(); i!=l_Syntax.end(); )
		{
			if( bTime && !oTime.IsContain(*i) || !bTime && !oPlace.IsContain(*i) )
				i=l_Syntax.erase(i);
			else i++;
		}
		return !l_Syntax.empty();
	}
	SS_CATCH(L"")
}

bool CSMSentenceExpert::IsStopPretextOrAimAdverb(bool bStop)
{
	SS_TRY
	{
		wstring str = m_pCurrentUnit->GetWord();
		CStringService oStringService;
		oStringService.LowerSz((TCHAR*)str.c_str());
		if (bStop)
		{
			set<wstring>::iterator i=m_StopPretexts.find(str);
			return i!=m_StopPretexts.end();
		}
		else
		{
			 set<wstring>::iterator i=m_AdverbStrings.find(str);
			 return i!=m_AdverbStrings.end();
		}
	}SS_CATCH(L"")
}

//////////////////////////////////////////////////////////////////////////

}
}