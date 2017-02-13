#include "StdAfx.h"
#include ".\smgenetive.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace English
			{
			


CSMGenetive::CSMGenetive(void)
{
	m_sDescription=_T("обработка родительного падежа в английском");
	m_AnalyseLanguage=SS::Core::Types::ealEnglish;
}

CSMGenetive::~CSMGenetive(void)
{
}

bool CSMGenetive::AnalizeSentence()
{
	SS_TRY
	{
		if(!GetFirstUnit()) return true;

		do{
			if (CheckGenetiveRecursive(m_pCurrentUnit)) continue;
			Mem();
			if ( m_LexemType.IsWord() && IsContainFeature(efECaseGenetive)
				/*GetNextUnit() &&
				m_LexemType.IsSApostrophe() &&
				GetNextUnit() &&
				m_LexemType.WordEqualsTo(_T("s"))*/
				)
			{
				Mem1();

				if(IsQuestionMode())
				{
					if(HideCurrentUnit())
						GetPrevUnit();
					HideCurrentUnit();
				}
				GotoMem();
				SetNounGenetiveCase(m_pCurrentUnit);
				//AddFeature(efECaseGenetive);
				if(!IsQuestionMode()) GotoMem1();
			}else GotoMem();

		}while(GetNextUnit());
		return true;
	}
	SS_CATCH(L"")
}

bool CSMGenetive::CheckGenetiveRecursive(IUnit * pUnit)
{
	SS_TRY
	{
		if (!pUnit) return false;

		if(pUnit->HasChilds())
		{
			IUnit * pUnit2=pUnit->GetLeftChildUnit();
			if(pUnit2->HasChilds())
			{
				if (CheckGenetiveRecursive(pUnit2))
					return true;
			}
			else
			{
				do{
					if(GenetiveProcess(pUnit2))
						return true;
				}while(pUnit2=pUnit2->GetRightUnit());
			}
		}
		return false;
	}
	SS_CATCH(L"")
}

bool CSMGenetive::GenetiveProcess(IUnit * pUnit)
{
	SS_TRY
	{
		if (!pUnit) return false;

		IUnit * pMem = NULL;
		//wstring str = pUnit -> GetWord();
		pMem = pUnit;
		if ( m_LexemType.IsWord(0,pUnit) && IsContainFeature(efECaseGenetive)
			/*(pUnit = pUnit -> GetRightUnit()) &&
			m_LexemType.IsSApostrophe(pUnit) &&
			(pUnit = pUnit -> GetRightUnit()) &&
			 m_LexemType.WordEqualsTo(_T("s"),pUnit)*/
			)
		{
			SetNounGenetiveCase(pMem);
			//AddFeature(efECaseGenetive);
		}
	}
	SS_CATCH(L"")
}

bool CSMGenetive::SetNounGenetiveCase(IUnit * pUnit)
{
//	TMorphoInfoCollection::iterator it;
//	CMorphoInfo oMT;
//	TMorphoInfoCollection oTemp;

	list<IFeature*> l_Features;
	list<IFeature*>::iterator it;

	if(pUnit==NULL) return false;

	int iMorphoInfo=0,iPartOfSpeech=0;
	IIndex * pIndex=pUnit->GetFirstIndex();
	if(!pIndex) return false;

	IFeature * pFeature, *pF2;
	UINT uiDicNumber;

	do{
		pFeature=pIndex->GetFirstFeature();
		if(!pFeature) continue;
		uiDicNumber=/*(*/pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*GetFirst())>>24*/;
		do{
			m_pIAMCMorpho->EnCode(uiDicNumber,pFeature->GetMorphoInfo(),pFeature->GetPartOfSpeech(),&m_MorphoFeature);
			if ( m_MorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postNoun) || m_MorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postPronoun) )
			{
				m_MorphoFeature.m_CaseType=Values::CaseType.ctGenitive;			
				
				m_pIAMCMorpho->Code(uiDicNumber,&m_MorphoFeature,&iMorphoInfo,&iPartOfSpeech);

				if (  iMorphoInfo>=0 && iPartOfSpeech>=0 )
				{
					pF2=m_pIBlackBoxFactory->CreateFeature();
					//pF2->SetIndexSynonymsClass(pFeature->GetIndexSynonymsClass());
					pF2->SetMorphoIndex(pFeature->GetMorphoIndex());
					pF2->SetMorphoInfo(iMorphoInfo);
					pF2->SetPartOfSpeech(iPartOfSpeech);
					l_Features.push_back(pF2);
				}
				m_MorphoFeature.m_CaseType=Values::CaseType.ctNominative;
				m_pIAMCMorpho->Code(uiDicNumber,&m_MorphoFeature,&iMorphoInfo,&iPartOfSpeech);
				if (  iMorphoInfo>=0 && iPartOfSpeech>=0 )
				{
					pF2=m_pIBlackBoxFactory->CreateFeature();
					//pF2->SetIndexSynonymsClass(pFeature->GetIndexSynonymsClass());
					pF2->SetMorphoIndex(pFeature->GetMorphoIndex());
					pF2->SetMorphoInfo(iMorphoInfo);
					pF2->SetPartOfSpeech(iPartOfSpeech);
					l_Features.push_back(pF2);
				}
			}
		}while((pFeature=pIndex->GetNextFeature()));
		if (!l_Features.empty())
		{
			for(it=l_Features.begin(); it!=l_Features.end();it++)
			{
				pIndex->AddFeature(*it);
			}
			l_Features.clear();
		}
	}while((pIndex=pUnit->GetNextIndex()));

	return true;

}

			}
		}
	}
}