#include "StdAfx.h"
#include ".\smexanaphora.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
		
			namespace IndexationExperts
			{



CSMExAnaphora::CSMExAnaphora(void)
{
	m_sDescription=_T(__FUNCTION__);
	m_AnalyseLanguage=SS::Core::Types::ealUndefined;
}

CSMExAnaphora::~CSMExAnaphora(void)
{
}
bool CSMExAnaphora::AnalizeSentence()
{
	SS_TRY
	{
		UINT uiWordCountInSentence=0;
		wstring sz_Word;//
		TBoxUnits l_units;

		do{
			uiWordCountInSentence++;
			l_units.clear();
			
			sz_Word = m_pCurrentUnit->GetWord();
			/*if (uiWordCountInSentence<=5)
			{*/
				if(m_pCurrentUnit->HasChilds())
				{
					CollectListOfNameAgents(l_units);
					ReplaceProNoun(uiWordCountInSentence,m_pCurrentUnit,l_units);
				}
				else
				{
					if ( IsNameAgent()) 
					{
						sz_Word = m_pCurrentUnit->GetWord();
						ReplaceProNoun(uiWordCountInSentence,m_pCurrentUnit, l_units);
					}
				}
			/*} else break;*/

		}while(GetNextUnit());

		return true;
	}
	SS_CATCH(L"")
}

bool CSMExAnaphora::Init(IText * pIText)
{
	SS_TRY
	{
		if (!CSMExpert::Init(pIText)) return false;
	}
	SS_CATCH(L"")

	return true;
}


void CSMExAnaphora::ReplaceProNoun(UINT & uiWordCount, IUnit * poInfo, TBoxUnits l_units)
{
	SS_TRY
	{
		UINT uiSentence=0;
		UINT uiWordCountIn=0;
		wstring sz_Word;
		IUnit * pUnit;
		list<IUnit*>::iterator it;

		do
		{
			uiSentence++;
			//uiWordCount=0;
			if(!GetFirstUnit()) 
				continue;
			do 
			{
				uiWordCountIn++;
				if (uiWordCountIn<=uiWordCount) 
					continue;

				sz_Word = m_pCurrentUnit->GetWord();

				if (IsNameAgent()) 
				{
					pUnit = m_pCurrentUnit;
					if (GetPrevUnit())
					{
						uiWordCount = uiWordCountIn-1;
						return;
					}else
					{
						ReplaceProNoun(uiWordCountIn,pUnit,l_units);
						return;
					}	
				}

				sz_Word=m_pCurrentUnit->GetWord();
				if (IsAimWord(sz_Word.c_str()) && IsValidGender(poInfo))
				{
					CopyUnit(poInfo, false);
					if(l_units.empty() == false)
					{
						for(it=l_units.begin(); it!=l_units.end(); it++)
						{
							CopyUnit(*it, true);
						}
					}		
				}
			}while(GetNextUnit());

			uiWordCount=0;
			uiWordCountIn=0;
		}while (uiSentence<3 &&  GetNextSentence() );

		uiWordCount = uiWordCountIn;
		return;
	}
	SS_CATCH(L"")

}

bool CSMExAnaphora::IsAimWord(const TCHAR * sWord)
{
	SS_TRY
	{
		wstring sz_Word(sWord);
		m_StringService.LowerSz((TCHAR*)sz_Word.c_str());
		set<wstring>::iterator i=m_AimWords.find(sz_Word.c_str());
		return (i!=m_AimWords.end());
	}
	SS_CATCH(L"")
}

void CSMExAnaphora::CollectListOfNameAgents(TBoxUnits & l_units)
{
	SS_TRY
	{
		IUnit * pUnit;

		pUnit = m_pCurrentUnit->GetLeftChildUnit();
		do{
			if (IsSemanticName(pUnit))
			{
				l_units.push_back(pUnit);
			}
		}while(pUnit = pUnit->GetRightUnit());
		return;
	}
	SS_CATCH(L"")
}

bool CSMExAnaphora::IsSemanticName(IUnit * pUnit)
{
	SS_TRY
	{
		SS::Core::Features::CSemanticFeature oSemanticFeature;

		oSemanticFeature.Undefine();
		CollectSemanticFeature(oSemanticFeature,pUnit);
		if(oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtPerson))
			return true;
		return false;
	}
	SS_CATCH(L"");
}

void CSMExAnaphora::CopyUnit(IUnit * pUnit, bool bMorphoOnly)
{
	SS_TRY
	{
		if(!pUnit) return;

		//unsigned int iIndex;
		IFeature * pFeature, *pF2;
		int iMorphoInfo=0, iPartOfSpeech=0;
		list<IFeature*> l_Features;
		list<IFeature*>::iterator it;
		UINT uiDicNumber;
		IIndex * pIndex;
		IIndex * pIndex2;

		//if(!bMorphoOnly)
		//{
			//m_pCurrentUnit->ClearUnit();
			//m_pCurrentUnit->ClearIndexList();
		//}

		pIndex=pUnit->GetFirstIndex();
		if (pIndex)
		{
			do
			{
				//iIndex = pIndex->GetDictionaryIndex()->GetFirst();
				//uiDicNumber = pIndex->GetDictionaryIndex()->GetDictionaryNumber()/*iIndex>>24*/;
				//uiDicNumber = iIndex->GetDictionaryNumber();
				uiDicNumber = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber();
				if(bMorphoOnly && ((uiDicNumber) == SS::Dictionary::DATA_TYPE::NAMES::ednSemantic||
					(uiDicNumber) == SS::Dictionary::DATA_TYPE::NAMES::ednSyntax))
						continue;

				pFeature = pIndex->GetFirstFeature();
				if(!pFeature) 
				{
					continue;
				}
				do
				{
					m_pIAMCMorpho->EnCode(uiDicNumber,pFeature->GetMorphoInfo(),pFeature->GetPartOfSpeech(),&m_MorphoFeature);
					if ( m_MorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postNoun) || m_MorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postPronoun) )
					{
						m_pIAMCMorpho->Code(uiDicNumber,&m_MorphoFeature,&iMorphoInfo,&iPartOfSpeech);

						if (iMorphoInfo>=0 && iPartOfSpeech>=0)
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
					pIndex2 = m_pIBlackBoxFactory ->CreateIndex();
					pIndex2->GetDictionaryIndex()->AppendIndex(pIndex->GetDictionaryIndex()->GetFirst());
					pIndex2->SetIndexationFlag(true);
					m_pCurrentUnit->AddIndex(pIndex2);
					for(it=l_Features.begin(); it!=l_Features.end();it++)
					{
						pIndex2->AddFeature(*it);
					}
					l_Features.clear();
				}
			}while(pIndex=pUnit->GetNextIndex());
		}	
	}
	SS_CATCH(L"")
}

bool CSMExAnaphora::IsValidGender(IUnit * pAgent)
{
	SS_TRY
	{
		if(!pAgent) return false;
		SS::Core::Features::CMorphoFeature oMorphoFeature,oMorphoFeature2;
		CollectMorphoFeature(oMorphoFeature,pAgent);
		CollectMorphoFeature(oMorphoFeature2);
		if(oMorphoFeature.m_GenderType.IsContain(Values::GenderType.gtFemale) &&
			oMorphoFeature2.m_GenderType.IsContain(Values::GenderType.gtFemale))
			return true;
		else if(oMorphoFeature.m_GenderType.IsContain(Values::GenderType.gtMasculine) &&
			oMorphoFeature2.m_GenderType.IsContain(Values::GenderType.gtMasculine))
			return true;
		else return false;
	}
	SS_CATCH(L"")
}

//////////////////////////////////////////////////////////////////////////
			}
		}
	}
}