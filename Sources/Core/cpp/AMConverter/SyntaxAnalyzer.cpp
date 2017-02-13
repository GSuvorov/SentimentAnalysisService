#include "StdAfx.h"

#include "..\ASSInterface\IAMConverter.h"
#include "..\ASSInterface\TAMConverterSyntaxStructures.h"

#include ".\syntaxanalyzer.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			CSyntaxAnalyzer::CSyntaxAnalyzer(void)
			{
			}

			HRESULT CSyntaxAnalyzer::Code(
				/*in*/Types::PartOfSpeechTypes oPartOfSpeechTypes,
				/*in*/SS::Core::Features::CSyntaxFeatureApriori *pSyntaxFeature, 
				/*out*/unsigned int* iSyntaxInfo)
			{
				SS_TRY;
					

					//pSyntaxFeature->m_bf_ui_TypeOfEndingObjectForVerb
					SS::Syntax::DataTypes::S_bf_ui_SyntaxInfoVerb o_bf_ui_SyntaxInfoVerb;
					
					o_bf_ui_SyntaxInfoVerb.EmptyBits			 = 0;
					
					if(oPartOfSpeechTypes.IsUndefined()){
						return E_FAIL;
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postOfficial)){
						return E_FAIL;
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postVerb)){
						o_bf_ui_SyntaxInfoVerb.ConstructionType = pSyntaxFeature->m_VerbConstruction.GetSingleValue();
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postNoun)){

						o_bf_ui_SyntaxInfoVerb.ConstructionType = pSyntaxFeature->m_NounConstruction.GetSingleValue();
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postAdjective)){
						o_bf_ui_SyntaxInfoVerb.ConstructionType = pSyntaxFeature->m_AdjectiveConstruction.GetSingleValue();
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postNumeric)){
						return E_FAIL;
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postAdverb)){
						return E_FAIL;
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postPronoun)){
						return E_FAIL;
					}
					else{
						return E_FAIL;
					}

						o_bf_ui_SyntaxInfoVerb.Object = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoObject))?(1):(0);			
						o_bf_ui_SyntaxInfoVerb.Relatives = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoRelatives))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Adv = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoAdv))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Preposition = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoPreposition))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Inf =
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoInf))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.ToInf = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoToInf))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Ing = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoIng))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.That = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoThat))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Adjective = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoAdjective))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.PastPart = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoPastPart))?(1):(0);

						o_bf_ui_SyntaxInfoVerb.CaseDat = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoCaseDat))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.CaseGen = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoCaseGen))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.CaseInstr = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoCaseInstr))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Relatives1 = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoThatBoodto))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Relatives2 = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoThatBoodtoKakKakbiKakoy))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Relatives3 = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoThatEsliKogda))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Relatives4 = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoThatKakKakbi))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Relatives5 = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoThatKakKakoyNaskolkoSkolko))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Relatives6 = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoThatKakPochemu))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Relatives7 = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoThatChtobiBoodto))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Relatives8 = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoThatChtobiEsliKogda))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Relatives9 = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoThatChtobiKakKakbi))?(1):(0);
						o_bf_ui_SyntaxInfoVerb.Relatives10 = 
							(pSyntaxFeature->m_TypeOfEndingObject.IsIntersectsWith(&Values::TypeOfEndingObject.toeoThatChtobiKakKogdaKakoy))?(1):(0);

						

					*iSyntaxInfo = *((unsigned int *)&o_bf_ui_SyntaxInfoVerb);

					
				SS_CATCH(L"");

				return S_OK;
			}
			HRESULT CSyntaxAnalyzer::EnCode(
				/*in*/ Types::PartOfSpeechTypes oPartOfSpeechTypes,
				/*in*/unsigned int iSyntaxInfo,	
				/*out*/SS::Core::Features::CSyntaxFeatureApriori *pSyntaxFeature)
			{
				SS_TRY;
						
					pSyntaxFeature->m_VerbConstruction.SetValue(1);
					pSyntaxFeature->m_TypeOfEndingObject.SetValue(0);
					pSyntaxFeature->m_CombiType.SetValue(0);

					SS::Syntax::DataTypes::S_bf_ui_SyntaxInfoVerb o_bf_ui_SyntaxInfoVerb;
					o_bf_ui_SyntaxInfoVerb = iSyntaxInfo;


					if(o_bf_ui_SyntaxInfoVerb.Object)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoObject);}
					if(o_bf_ui_SyntaxInfoVerb.Relatives)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoRelatives);}
					if(o_bf_ui_SyntaxInfoVerb.Adv)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoAdv);}
					if(o_bf_ui_SyntaxInfoVerb.Preposition)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoPreposition);}
					if(o_bf_ui_SyntaxInfoVerb.Inf)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoInf);}
					if(o_bf_ui_SyntaxInfoVerb.ToInf)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoToInf);}
					if(o_bf_ui_SyntaxInfoVerb.Ing)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoIng);}
					if(o_bf_ui_SyntaxInfoVerb.That)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoThat);}
					if(o_bf_ui_SyntaxInfoVerb.Adjective)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoAdjective);}
					if(o_bf_ui_SyntaxInfoVerb.PastPart)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoPastPart);}

					if(o_bf_ui_SyntaxInfoVerb.CaseDat)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoCaseDat);}
					if(o_bf_ui_SyntaxInfoVerb.CaseGen)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoCaseGen);}
					if(o_bf_ui_SyntaxInfoVerb.CaseInstr)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoCaseInstr);}
					if(o_bf_ui_SyntaxInfoVerb.Relatives1)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoThatBoodto);}
					if(o_bf_ui_SyntaxInfoVerb.Relatives2)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoThatBoodtoKakKakbiKakoy);}
					if(o_bf_ui_SyntaxInfoVerb.Relatives3)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoThatEsliKogda);}
					if(o_bf_ui_SyntaxInfoVerb.Relatives4)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoThatKakKakbi);}
					if(o_bf_ui_SyntaxInfoVerb.Relatives5)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoThatKakKakoyNaskolkoSkolko);}
					if(o_bf_ui_SyntaxInfoVerb.Relatives6)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoThatKakPochemu);}
					if(o_bf_ui_SyntaxInfoVerb.Relatives7)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoThatChtobiBoodto);}
					if(o_bf_ui_SyntaxInfoVerb.Relatives8)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoThatChtobiEsliKogda);}
					if(o_bf_ui_SyntaxInfoVerb.Relatives9)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoThatChtobiKakKakbi);}
					if(o_bf_ui_SyntaxInfoVerb.Relatives10)
						{pSyntaxFeature->m_TypeOfEndingObject.AssignOR(&Values::TypeOfEndingObject.toeoThatChtobiKakKogdaKakoy);}


					
					
					if(oPartOfSpeechTypes.IsUndefined()){
						return E_FAIL;
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postOfficial)){
						return E_FAIL;
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postVerb)){
							pSyntaxFeature->m_VerbConstruction = o_bf_ui_SyntaxInfoVerb.ConstructionType;
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postNoun)){
						pSyntaxFeature->m_NounConstruction = o_bf_ui_SyntaxInfoVerb.ConstructionType;
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postAdjective)){
						pSyntaxFeature->m_AdjectiveConstruction = o_bf_ui_SyntaxInfoVerb.ConstructionType;			
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postNumeric)){
						return E_FAIL;
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postAdverb)){
						return E_FAIL;
					}
					else if(oPartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postPronoun)){
						return E_FAIL;
					}
					else{
						return E_FAIL;
					}
					

				SS_CATCH(L"");

				return S_OK;
			}
		}
	}
}