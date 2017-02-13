#include "StdAfx.h"
#include ".\moambresolver.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{
				namespace English
				{



CMoAmbResolver::CMoAmbResolver(void)
{
}

CMoAmbResolver::~CMoAmbResolver(void)
{
}
//void CMoAmbResolver::Analyse()
//{
//	//CExQuestionParser::TMorphoDescription::iterator pD,pD2,it3;
//	IUnit * pUnit, * pUnit2;
//	SMorphoDescription * pD, * pD2, *pD3;
//	//èçáàâëÿåìñÿ îò îìîíèìèè 
//	//char sz_Word[MAX_CHARS_IN_WORD + 1];
//	//char sz_Word2[MAX_CHARS_IN_WORD + 1];
//
//	if (GetFirstUnit())
//	{
//		do{
//			if(!m_pCurrentUnit->GetFirstIndex() || !m_pCurrentUnit->GetFirstIndex()->GetDictionaryIndex()||
//				((m_pCurrentUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst()>>24)==SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng)
//				)
//				continue;
//
//			pUnit=m_pCurrentUnit;
//			NextUnit(pUnit);
//			pD=GetUnitDescription();
//			pD2=GetUnitDescription(pUnit);
//
//			//1.	Det+Adjective || Noun –> Remove Verb Personal 
//			if( pD->_oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otDeterminers) )
//			{
//				pD->_ePOS=eposDet;
//				if ( pD2->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNoun) ||
//					pD2->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postAdjective)
//					)
//				{
//					pD2->_oMorphoFeature.m_PartOfSpeechTypes.Remove(Values::PartOfSpeechTypes.postVerb);
//				}		
//			}
//			//1.1	Det+!Verb&!Noun + Noun –> Remove Verb Personal 
//			if(pD->_oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otDeterminers))
//			{
//				if ( !pD2->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNoun) &&
//					!pD2->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb)
//					)
//				{
//					pUnit2=pUnit;
//					NextUnit(pUnit2);
//					pD3=GetUnitDescription(pUnit2);
//					if (pD3 && (pD3->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNoun)) )
//					{
//						pD3->_oMorphoFeature.m_PartOfSpeechTypes.Remove(Values::PartOfSpeechTypes.postVerb);
//					}
//				}		
//			}
//			//2.	Pronoun-adjective + Adjective || Noun –> Remove Verb Personal    
//
//			if( 
//				pD->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postPronoun) &&
//				pD->_oMorphoFeature.m_PronounGroup.IsContain(Values::PronounGroup.pngAdjective) &&
//				pD->_oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctGenitive) && 
//				pD->_oMorphoFeature.m_PronounClass.IsContain(Values::PronounClass.pncPossessive)
//				//		||	
//				//		(pD->_oMorphoFeature.m_PartOfSpeechTypes&postAdjective)!=postIndefinably 
//				)
//			{ 
//				if ( pD2->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNoun) ||
//					pD2->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postAdjective)
//					)
//				{
//					pD2->_oMorphoFeature.m_PartOfSpeechTypes.Remove(Values::PartOfSpeechTypes.postVerb);
//				}				
//			}	
//			//3.	Noun (CaseType== Genetive) + Adjective|| Noun –> Remove Verb Personal
//			if( pD->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNoun) &&
//				pD->_oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctGenitive) 
//				)
//			{
//				if ( pD2->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNoun) ||
//					pD2->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postAdjective)
//					)
//				{
//					pD2->_oMorphoFeature.m_PartOfSpeechTypes.Remove(Values::PartOfSpeechTypes.postVerb);
//				}				
//			}
//			////3.1	Noun (CaseType== Genetive)'s + Adjective|| Noun –> Remove Verb Personal
//			//if( pD->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNoun) &&
//			//	pD->_oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctGenitive) 
//			//	)
//			//{
//			//	pD2=pD; pD2++;
//			//	if (pD2==tDescription.end() ) break;
//			//	pD2->_Unit->GetWord(sz_Word);
//			//	if ( sz_Word[0]=='\'' ) 
//			//	{
//			//		pD2++;
//			//		if (pD2==tDescription.end() ) break;
//			//		pD2->_Unit->GetWord(sz_Word);
//			//		if ( sz_Word[0]=='s' && sz_Word[1]==0 )
//			//		{
//			//			pD2->_oMorphoFeature.m_PartOfSpeechTypes.Remove(Values::PartOfSpeechTypes.postVerb);
//			//			pD2++; 
//			//		}
//			//		if (pD2==tDescription.end() ) break;
//
//			//		pD2->_oMorphoFeature.m_PartOfSpeechTypes.Remove(Values::PartOfSpeechTypes.postVerb);
//			//	}
//			//}
//			//4.	Prep + Noun - > Remove Verb Personal
//			if( pD->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postAdverb) &&
//				pD->_oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otPretext)  ||
//				pD->_oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otConjunction) &&
//				pD->_oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otPretext)
//				)
//			{
//				if ( pD2->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNoun)
//					)
//				{
//					pD2->_oMorphoFeature.m_PartOfSpeechTypes.Remove(Values::PartOfSpeechTypes.postVerb);
//				}				
//			}
//			//7. Verb1(only & transitive) + Verb2  . -> kill Verb2
//			if(pD->_oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postVerb) && 
//				pD->_oMorphoFeature.m_TransitionalType.IsContain(Values::TransitionalType.tltTransitional) 
//				)
//			{
//				if( pD2->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb)
//					)
//				{
//					pD2->_oMorphoFeature.m_PartOfSpeechTypes.Remove(Values::PartOfSpeechTypes.postVerb);
//				}
//			}
//			//5.	Verb + Prep + “something Adjective Number Numeral Noun” ->  select Verb + Entity 
//			if(pD->_oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postVerb))
//			{
//				if( pD2->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postAdverb) &&
//					pD2->_oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otPretext)  ||
//					pD2->_oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otConjunction)&&
//					pD2->_oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otPretext)
//					)
//				{
//					pUnit2=pUnit;
//					NextUnit(pUnit2);
//					pD3=GetUnitDescription(pUnit2);
//					for ( ; pUnit2 && pD3 ; NextUnit(pUnit2), pD3=GetUnitDescription(pUnit2) )
//					{
//							pD3->_oMorphoFeature.m_PartOfSpeechTypes.Remove(Values::PartOfSpeechTypes.postVerb);
//					}				
//				}
//			}
//			//6. Verb1 + Verb2 ( only intransitive ) . -> kill Verb1
//			pUnit2=pUnit;
//			if (!GetUnitDescription(pUnit->GetRightUnit()))
//			{
//				if(pD->_oMorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postVerb))
//					if( pD2->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb) &&
//						(pD2->_oMorphoFeature.m_TransitionalType.Equal(Values::TransitionalType.tltNoTransitional))
//						)
//						pD->_oMorphoFeature.m_PartOfSpeechTypes.Remove(Values::PartOfSpeechTypes.postVerb);
//
//			}
//			// 7. prep(!verb & !noun ) + verb partic(present) -> kill verb
//			if(
//				!pD->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb)&&
//				!pD->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postNoun)&&
//				pD->_oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otPretext)
//				)
//			{
//				if( pD2->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb) &&
//					pD2->_oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple) &&
//					pD2->_oMorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPresent)
//					)
//				{
//					pD2->_oMorphoFeature.m_PartOfSpeechTypes.Remove(Values::PartOfSpeechTypes.postVerb);
//				}
//			}
//						
//		}while(GetNextUnit());
//	}
//
//	if (GetFirstUnit())
//	{
//		do{
//			pD=GetUnitDescription();
//			if (pD->_oMorphoFeature.m_OfficialType.IsContain(Values::OfficialType.otDeterminers) ||
//				pD->_oMorphoFeature.m_PronounGroup.IsContain(Values::PronounGroup.pngAdjective) &&
//				(pD->_oMorphoFeature.m_PronounClass.IsContain(Values::PronounClass.pncIndicative)||
//				pD->_oMorphoFeature.m_PronounClass.IsContain(Values::PronounClass.pncPossessive))
//				)
//			{
//				if(!(m_pCurrentUnit=m_pCurrentUnit->ReleaseUnitHorizontal()))
//					break;
//				
//			}else if(!GetNextUnit()) 
//				break;
//		}while(true);
//	}
//}


				}
			}
		}
	}
}