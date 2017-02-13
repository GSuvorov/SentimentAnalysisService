#include "StdAfx.h"
#include ".\smsemanticinserter.h"

namespace SS
{
	namespace Semantic
	{
		namespace PostAnalysisExperts
		{
			namespace PostSyntaxAnalysis
			{

		CSMSemanticInserter::CSMSemanticInserter(void) : m_lsemNumber(0)
		{
			m_bInit = false;
			m_AnalyseLanguage=SS::Core::Types::ealEnglish;
			m_sDescription=_T(__FUNCTION__);
		}

		CSMSemanticInserter::~CSMSemanticInserter(void)
		{
		}
		bool CSMSemanticInserter::AnalizeSentence()
		{
			SS_TRY
			{
				IUnit * pUnit;
				const TCHAR * str;
				list<SS::Dictionary::Types::TDictionaryIndex> l_temp;
				if (!GetFirstUnit()) return true;

				do{
					if (m_pCurrentUnit->GetSyntaxFeature() && m_pCurrentUnit->GetSyntaxFeature()->IsQuestion() )
					{
						if(m_pCurrentUnit->GetChildCount()>0)
						{
							if(pUnit=m_pCurrentUnit->GetLeftChildUnit())
							{
								str=pUnit->GetWord();
								m_StringService.LowerSz((TCHAR*)str);
								if (m_StringService.StrEqual(_T("when"),str))
								{
									RemoveSemanticIndicesFromUnit(l_temp);
									SetSemanticIndices(m_l_SemanticIndicesTime);
									break;
								}else if (m_StringService.StrEqual(_T("where"),str))
								{
									RemoveSemanticIndicesFromUnit(l_temp);
									SetSemanticIndices(m_l_SemanticIndicesPlace);
									SetSemanticIndices(m_l_SemanticIndicesOrganization);
									break;
								}else if (m_pCurrentUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(Values::SyntaxCategories.scSubjectFin)&&
									m_StringService.StrEqual(_T("how"),str))
								{
									/*l_temp.push_back(m_lsemNumber);
									SetSemanticIndices(l_temp);*/
									m_pCurrentUnit->GetSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Remove(Values::SyntaxCategories.scSubjectFin);
									break;
								}
							}
						}
					}
				}while(GetNextUnit());

				return true;
			}
			SS_CATCH(L"")

		}
		bool CSMSemanticInserter::Init(IText * pIText)
		{
			if ( !CSMExpert::Init(pIText)) return false;

			if(!m_bInit)
			{
				m_bInit = true;
				
				SS::Core::Features::CSemanticFeature oSemanticFeature;
				SS::Dictionary::Types::TDictionaryIndex lVal=0;

				/*oSemanticFeature.m_SemanticType=Values::TSemanticType.smtMeasure;
				oSemanticFeature.m_SMeasureType=Values::TSMeasureType.msrtNumber;
				m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
				m_lsemNumber=(lVal);*/


				UINT uitemp,uiCurrentBitNumber;
				if (m_l_SemanticIndicesTime.empty())
				{
					oSemanticFeature.Undefine();
					oSemanticFeature.m_SemanticType=Values::TSemanticType.smtTimeInterval;
					oSemanticFeature.m_STimeIntervalType.FillUp();
					//oSemanticFeature.m_STimeIntervalType.Remove(Values::TSTimeIntervalType.Undefined);
					uitemp=oSemanticFeature.m_STimeIntervalType.GetValue();
					uiCurrentBitNumber=0;
					do{
						if(uitemp&(1<<uiCurrentBitNumber))
						{
							oSemanticFeature.m_STimeIntervalType.SetSingleValue(uiCurrentBitNumber);
							m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
							/*SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex;
							oDictionaryIndex.SetId(lVal);*/
							lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
							m_l_SemanticIndicesTime.push_back(lVal.GetDictionaryIndex());
						}
					}while( uiCurrentBitNumber++<31);
				}
				if (m_l_SemanticIndicesPlace.empty())
				{
					oSemanticFeature.Undefine();
					oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPlace;
					oSemanticFeature.m_SPlaceType.FillUp();
					uitemp=oSemanticFeature.m_SPlaceType.GetValue();
					uiCurrentBitNumber=0;
					do{
						if(uitemp&(1<<uiCurrentBitNumber))
						{
							oSemanticFeature.m_SPlaceType.SetSingleValue(uiCurrentBitNumber);
							m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
							lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
							m_l_SemanticIndicesPlace.push_back(lVal.GetDictionaryIndex());
						}
					}while( uiCurrentBitNumber++<31);
				}
				if(m_l_SemanticIndicesOrganization.empty())
				{
					oSemanticFeature.Undefine();
					oSemanticFeature.m_SemanticType=Values::TSemanticType.smtOrganization;
					oSemanticFeature.m_SOrganizationType.FillUp();
					uitemp=oSemanticFeature.m_SOrganizationType.GetValue();
					uiCurrentBitNumber=0;
					do{
						if(uitemp&(1<<uiCurrentBitNumber))
						{
							oSemanticFeature.m_SOrganizationType.SetSingleValue(uiCurrentBitNumber);
							m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
							lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
							m_l_SemanticIndicesOrganization.push_back(lVal.GetDictionaryIndex());
						}
					}while( uiCurrentBitNumber++<31);
				}
			}

			return true;
		}
//////////////////////////////////////////////////////////////////////////		
}
}
}
}