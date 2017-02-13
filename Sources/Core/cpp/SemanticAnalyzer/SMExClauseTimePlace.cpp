#include "StdAfx.h"
#include ".\SMExClauseTimePlace.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace IndexationExperts
			{

				CSMExClauseTimePlace::CSMExClauseTimePlace(void)
				{
					m_AnalyseLanguage=SS::Core::Types::ealUndefined;
					m_sDescription=_T(__FUNCTION__);
				}

				CSMExClauseTimePlace::~CSMExClauseTimePlace(void)
				{
				}

				bool CSMExClauseTimePlace::AnalizeSentence()
				{
					TBoxUnits l_units;
					UINT uiWordCountInSentence=0;
					wstring sz_Word;

					do{
						uiWordCountInSentence++;
						sz_Word=m_pCurrentUnit->GetWord();
						if (IsWord(sz_Word.c_str()) /*&& (uiWordCountInSentence > 2)*/)
						{
							l_units.push_back(m_pCurrentUnit);
						}
						if(sz_Word == wstring(_T("?")))
						{
							l_units.clear();
						}
					}while(GetNextUnit());

					SetSintaxCategoryToUnit(l_units);
					l_units.clear();

					return true;
				}

				bool CSMExClauseTimePlace::Init(IText * pIText)
				{
					if (!CSMExpert::Init(pIText)) return false;

					return true;

				}

				void CSMExClauseTimePlace::SetSintaxCategoryToUnit(TBoxUnits l_units)
				{
					UINT uiSentence=0;
					wstring sz_Word;
					TBoxUnits::iterator i;
					list<SS::Dictionary::Types::TDictionaryIndex> l_sem;

					SS::Dictionary::Types::TDictionaryIndex lVal=0;
					SS::Core::Features::CSemanticFeature oSemanticFeature;

					for (i=l_units.begin(); i!=l_units.end() ; i++) {
						sz_Word=(*i)->GetWord();
						m_StringService.LowerSz((TCHAR*)sz_Word.c_str());	
						if(sz_Word==wstring(_T("где")) || (sz_Word==wstring(_T("where"))))
						{
							RemoveSemanticIndicesFromUnit(l_sem,(*i));
							oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPlace;
							oSemanticFeature.m_SPlaceType=Values::TSPlaceType.Undefined;
							m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
							lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
							AddSemanticIndex(lVal,(*i));
							AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctPlaceFixedPoint,(*i));
							//SetSyntaxCategory((*i),Values::SyntaxCategories.scAjunctPlaceFixedPoint);
						}
						else
						{
							RemoveSemanticIndicesFromUnit(l_sem,(*i));
							oSemanticFeature.m_SemanticType=Values::TSemanticType.smtTimeInterval;
							oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.Undefined;
							m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
							lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
							AddSemanticIndex(lVal,(*i));
							AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctTimeFixedPoint,(*i));
							//SetSyntaxCategory((*i),Values::SyntaxCategories.scAjunctTimeFixedPoint);
							
						}	

					}

				}


				bool CSMExClauseTimePlace::IsWord(const TCHAR * sWord)
				{
					wstring sz_Word(sWord);
					m_StringService.LowerSz((TCHAR*)sz_Word.c_str());
					set<wstring>::iterator i=m_Words.find(sz_Word.c_str());
					return (i!=m_Words.end());
				}

			}
		}
	}
}