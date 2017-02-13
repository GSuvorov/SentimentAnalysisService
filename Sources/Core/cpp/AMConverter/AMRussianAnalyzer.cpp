#include "StdAfx.h"
#include ".\amrussiananalyzer.h"

namespace SS
{
namespace Core
{
namespace Features
{
CAMRussianAnalyzer::CAMRussianAnalyzer(void)
{
}

CAMRussianAnalyzer::~CAMRussianAnalyzer(void)
{
}

bool CAMRussianAnalyzer::Code(/*in*/const CMorphoFeature *pMorphoFeature, /*out*/int *MorphoInfo, /*out*/int *PartOfSpeech)
{
	SS_TRY
	{
		unsigned char uchBaseMI = 0;
		unsigned char uchAddMI  = 0;

		/////*	обратный анализ MorphoInfo	*/
		m_mi_BASE.NegativeType		= pMorphoFeature->m_NegativeType.GetSingleValue();
		m_mi_BASE.PartOfSpeechTypes	= pMorphoFeature->m_PartOfSpeechTypes.GetSingleValue();
		m_mi_BASE.SemanticsTypes	= 0;
		
		memcpy(&uchBaseMI, &m_mi_BASE, 1);
		*MorphoInfo =  uchBaseMI<<8;

	// Обратить внимание
	//	memcpy(&uchBaseMI, &m_mi_BASE, 1);	

		if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postOfficial)){	// служебные части речи		
			m_mi_OFFICIAL.OfficialType = pMorphoFeature->m_OfficialType.GetSingleValue();
			memcpy(&uchAddMI, &m_mi_OFFICIAL, 1);
			*MorphoInfo += uchAddMI;
			CodeOfficial(PartOfSpeech, pMorphoFeature);
		}
		else if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postVerb)){		// глаголы и формы
			m_mi_VERB.AspectType		= pMorphoFeature->m_AspectType.GetSingleValue();
			memcpy(&uchAddMI, &m_mi_VERB, 1);
			*MorphoInfo += uchAddMI;
			CodeVerbs(PartOfSpeech, pMorphoFeature);
		}
		else if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postNoun)){		// существительные
			m_mi_NOUNS.AnimInAnimType	= pMorphoFeature->m_AnimInAnimType.GetSingleValue();

			if ((pMorphoFeature->m_GenderType.IsIntersectsWith(&Values::GenderType.gtMasculine)) 
				&& (pMorphoFeature->m_GenderType.IsIntersectsWith(&Values::GenderType.gtFemale))) 
			{
				m_mi_NOUNS.GenderType = Values::GenderType.Undefined.GetSingleValue();
			}else 
			{
				m_mi_NOUNS.GenderType		= pMorphoFeature->m_GenderType.GetSingleValue();
			}	
			memcpy(&uchAddMI, &m_mi_NOUNS, 1);
			*MorphoInfo += uchAddMI;
			CodeNouns(PartOfSpeech, pMorphoFeature);
		}
		else if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postAdjective)){	// прилагательные
			m_mi_ADJECTIVES.Empty = 0;
			memcpy(&uchAddMI, &m_mi_ADJECTIVES, 1);
			*MorphoInfo += uchAddMI;
			CodeAdjectives(PartOfSpeech, pMorphoFeature);
		}
		else if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postNumeric)){	// числительные			
			m_mi_NUMERALS.NumericType	= pMorphoFeature->m_NumericType.GetSingleValue();
			memcpy(&uchAddMI, &m_mi_NUMERALS, 1);
			*MorphoInfo += uchAddMI;
			CodeNumbers(PartOfSpeech, pMorphoFeature);
		}
		else if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postAdverb)){	// наречия
			m_mi_ADVERB.AdverbType		= pMorphoFeature->m_AdverbType.GetSingleValue();
			memcpy(&uchAddMI, &m_mi_ADVERB, 1);
			*MorphoInfo += uchAddMI;
			CodeAdverbs(PartOfSpeech, pMorphoFeature);
		}
		else if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postPronoun)){	// местоимение
			m_mi_PRONOUNS.Empty = 0;
			memcpy(&uchAddMI, &m_mi_PRONOUNS, 1);
			*MorphoInfo += uchAddMI;
			CodePronouns(PartOfSpeech, pMorphoFeature);
		}
		else{
			*MorphoInfo = 0;
			*PartOfSpeech = 0;
		}

		return true;
	}
	SS_CATCH(L"")

	return false;
}
bool CAMRussianAnalyzer::EnCode(/*in*/int iMorphoInfo, /*in*/int iPartOfSpeech, /*out*/CMorphoFeature *pMorphoFeature)
{

	SS_TRY
	{
		//Initialization(pMorphoFeature);

		pMorphoFeature->Undefine();

		unsigned char uchBaseMI = iMorphoInfo>>8;
		unsigned char uchAddMI = (iMorphoInfo<<24)>>24;
		memcpy(&m_mi_BASE, &uchBaseMI, 1);		
		
		pMorphoFeature->m_NegativeType		= m_mi_BASE.NegativeType;
		pMorphoFeature->m_PartOfSpeechTypes = m_mi_BASE.PartOfSpeechTypes;

		if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postOfficial)){	// служебные части речи
			memcpy(&m_mi_OFFICIAL, &uchAddMI, 1);	
			pMorphoFeature->m_OfficialType = m_mi_OFFICIAL.OfficialType;
			EnCodeOfficial(iPartOfSpeech, pMorphoFeature);
		}
		else if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postVerb)){		// глаголы и формы
			memcpy(&m_mi_VERB, &uchAddMI, 1);	
			pMorphoFeature->m_AspectType		= m_mi_VERB.AspectType;
			EnCodeVerbs(iPartOfSpeech, pMorphoFeature);

			if (pMorphoFeature->m_VerbForm.IsIntersectsWith(&Values::VerbForm.vfParticiple)) 
			{
				PostAnalyseGenderType(pMorphoFeature);
			}
		}
		else if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postNoun)){		// существительные
			memcpy(&m_mi_NOUNS, &uchAddMI, 1);	
			pMorphoFeature->m_AnimInAnimType	= m_mi_NOUNS.AnimInAnimType;
			pMorphoFeature->m_GenderType		= m_mi_NOUNS.GenderType;
			EnCodeNouns(iPartOfSpeech, pMorphoFeature);
			PostAnalyseCaseType(pMorphoFeature);
			PostAnalyseNumberType(pMorphoFeature);
		}
		else if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postAdjective)){ // прилагательные
			//memcpy(&m_mi_ADJECTIVES, &uchAddMI, 1);
			EnCodeAdjectives(iPartOfSpeech, pMorphoFeature);	
			PostAnalyseGenderType(pMorphoFeature);
		}
		else if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postNumeric)){	// числительные
			memcpy(&m_mi_NUMERALS, &uchAddMI, 1);
			pMorphoFeature->m_NumericType = m_mi_NUMERALS.NumericType;
			EnCodeNumbers(iPartOfSpeech, pMorphoFeature);

			if (pMorphoFeature->m_NumericType.IsIntersectsWith(&Values::NumericType.ntCount)) 
			{
				PostAnalyseCaseType(pMorphoFeature);
			}
         else if( 
            pMorphoFeature->m_NumericType.IsIntersectsWith(&Values::NumericType.ntOrdering) &&
            pMorphoFeature->m_NumberType.IsIntersectsWith(&Values::NumberType.ntPlural) &&
            pMorphoFeature->m_GenderType.IsIntersectsWith(&Values::GenderType.Undefined)
        )
        {
            pMorphoFeature->m_GenderType.Assign(Values::GenderType.gtFemale);
            pMorphoFeature->m_GenderType.AssignOR(&Values::GenderType.gtNeutral);
            pMorphoFeature->m_GenderType.AssignOR(&Values::GenderType.gtMasculine);
        }

		}
		else if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postAdverb)){	// наречия
			memcpy(&m_mi_ADVERB, &uchAddMI, 1);
			pMorphoFeature->m_AdverbType	= m_mi_ADVERB.AdverbType;
			EnCodeAdverbs(iPartOfSpeech, pMorphoFeature);
		}
		else if(pMorphoFeature->m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postPronoun)){	// местоимения
			//memcpy(&m_mi_PRONOUNS, &uchAddMI, 1);
			EnCodePronouns(iPartOfSpeech, pMorphoFeature);

			if (pMorphoFeature->m_PronounGroup.IsIntersectsWith(&Values::PronounGroup.pngAdjective) &&
				(pMorphoFeature->m_NumberType.IsIntersectsWith(&Values::NumberType.ntPlural))) 
			{
				PostAnalyseGenderType(pMorphoFeature);
			}

		}
		else{
			pMorphoFeature->m_PartOfSpeechTypes.Undefine();
		}

		return true;
		
	}
	SS_CATCH(L"")

	return false;
}

void CAMRussianAnalyzer::EnCodePronouns(int iPartOfSpeech, CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		memcpy( &m_pos_PRONOUNS, &iPartOfSpeech, 4);
		pMorphoFeature->m_CaseType		= m_pos_PRONOUNS.CaseType;
		pMorphoFeature->m_GenderType	= m_pos_PRONOUNS.GenderType;
		pMorphoFeature->m_NumberType	= m_pos_PRONOUNS.NumberType;
		pMorphoFeature->m_PersonType	= m_pos_PRONOUNS.PersonType;
		pMorphoFeature->m_PronounClass	= m_pos_PRONOUNS.PronounClass;
		pMorphoFeature->m_PronounGroup	= m_pos_PRONOUNS.PronounGroup;
		pMorphoFeature->m_AnimInAnimType= m_pos_PRONOUNS.AnimInAnimType;
	}
	SS_CATCH(L"")
}

void CAMRussianAnalyzer::CodePronouns(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		m_pos_PRONOUNS.CaseType		= pMorphoFeature->m_CaseType.GetSingleValue();
		
		if ((pMorphoFeature->m_GenderType.IsIntersectsWith(&Values::GenderType.gtMasculine)) 
			&& (pMorphoFeature->m_GenderType.IsIntersectsWith(&Values::GenderType.gtFemale))) 
		{
			m_pos_PRONOUNS.GenderType = Values::GenderType.Undefined.GetSingleValue();
		}else 
		{
			m_pos_PRONOUNS.GenderType		= pMorphoFeature->m_GenderType.GetSingleValue();
		}	

		m_pos_PRONOUNS.NumberType	= pMorphoFeature->m_NumberType.GetSingleValue();
		m_pos_PRONOUNS.PersonType	= pMorphoFeature->m_PersonType.GetSingleValue();
		m_pos_PRONOUNS.PronounClass	= pMorphoFeature->m_PronounClass.GetSingleValue();
		m_pos_PRONOUNS.PronounGroup	= pMorphoFeature->m_PronounGroup.GetSingleValue();
		m_pos_PRONOUNS.AnimInAnimType = pMorphoFeature->m_AnimInAnimType.GetSingleValue();

		memcpy( iPartOfSpeech, &m_pos_PRONOUNS, 4);
	}
	SS_CATCH(L"")
}



void CAMRussianAnalyzer::EnCodeAdverbs(int iPartOfSpeech, CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		memcpy( &m_pos_ADVERB, &iPartOfSpeech, 4);
		pMorphoFeature->m_DegreeType	= m_pos_ADVERB.DegreeType;
	}
	SS_CATCH(L"")

}
void CAMRussianAnalyzer::EnCodeNumbers(int iPartOfSpeech, CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		memcpy( &m_pos_NUMERALS, &iPartOfSpeech, 4);
		pMorphoFeature->m_AnimInAnimType= m_pos_NUMERALS.AnimInAnimType;
		pMorphoFeature->m_CaseType		= m_pos_NUMERALS.CaseType;
		pMorphoFeature->m_GenderType	= m_pos_NUMERALS.GenderType;
		pMorphoFeature->m_NumberType	= m_pos_NUMERALS.NumberType;
	}
	SS_CATCH(L"")

	
}

void CAMRussianAnalyzer::EnCodeVerbs(int iPartOfSpeech, CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		memcpy( &m_pos_VERB, &iPartOfSpeech, 4);
		pMorphoFeature->m_TransitionalType	= m_pos_VERB.TransitionalType;
		pMorphoFeature->m_GenderType		= m_pos_VERB.GenderType;
		pMorphoFeature->m_NumberType		= m_pos_VERB.NumberType;
		pMorphoFeature->m_PersonType		= m_pos_VERB.PersonType;
		pMorphoFeature->m_TimeType			= m_pos_VERB.TimeType;
		pMorphoFeature->m_VerbForm			= m_pos_VERB.VerbForm;
		pMorphoFeature->m_VoiceType			= m_pos_VERB.VoiceType;
		pMorphoFeature->m_PhaseVerbType		= m_pos_VERB.PhaseVerbType;

		pMorphoFeature->m_AnimInAnimType	= m_pos_VERB.AnimInAnimType;
		pMorphoFeature->m_CaseType			= m_pos_VERB.CaseType;
		pMorphoFeature->m_ShortType			= m_pos_VERB.ShortType;	
	}
	SS_CATCH(L"")



}

void CAMRussianAnalyzer::EnCodeNouns(int iPartOfSpeech, CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		memcpy( &m_pos_NOUNS, &iPartOfSpeech, 4);

		pMorphoFeature->m_CaseType		= m_pos_NOUNS.CaseType;
		pMorphoFeature->m_NumberType	= m_pos_NOUNS.NumberType;
	}
	SS_CATCH(L"")

}

void CAMRussianAnalyzer::EnCodeOfficial(int iPartOfSpeech, CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		if(pMorphoFeature->m_OfficialType.Equal(Values::OfficialType.otPretext))
		{
			memcpy( &m_pos_PRETEXT, &iPartOfSpeech, 4);
			pMorphoFeature->m_CaseType	= m_pos_PRETEXT.CaseType;
		}	
	}
	SS_CATCH(L"")


}

void CAMRussianAnalyzer::BackInit()
{
	SS_TRY
	{
		m_mi_BASE.Empty	= 0;
		m_mi_OFFICIAL.Empty	= 0;
		m_mi_NOUNS.Empty	= 0;
		m_mi_NUMERALS.Empty	= 0;
		m_mi_ADJECTIVES.Empty	= 0;
		m_mi_PRONOUNS.Empty	= 0;
		m_mi_VERB.Empty	= 0;
		m_pos_PRETEXT.Empty	= 0;
		m_pos_NOUNS.Empty	= 0;
		m_pos_NUMERALS.Empty	= 0;
		m_pos_ADJECTIVE.Empty	= 0;
		m_pos_PRONOUNS.Empty	= 0;
		m_pos_ADVERB.Empty	= 0;
		m_pos_VERB.Empty	= 0;
	}
	SS_CATCH(L"")



}

//void CAMRussianAnalyzer::Initialization(CMorphoFeature *morpho_feature)
//{
//	SS_TRY
//	{
//		morpho_feature->m_PartOfSpeechTypes.Undefine(); // часть речи
//		morpho_feature->m_AnimInAnimType.Undefine();    // одушевленность
//		morpho_feature->m_AspectType.Undefine();        // вид глаголов
//		morpho_feature->m_AdverbType.Undefine();        // тип наречия
//		morpho_feature->m_CaseType.Undefine();          // падеж
//
//		morpho_feature->m_CountType.Undefine();         // исчисляемость
//		morpho_feature->m_CommonType.Undefine();        // нарицательность 
//		morpho_feature->m_DegreeType.Undefine();        // степень сравнения
//		morpho_feature->m_GenderType.Undefine();        // род
//		morpho_feature->m_NumberType.Undefine();        // число
//
//		morpho_feature->m_NumericType.Undefine();       // тип числительного
//		morpho_feature->m_NegativeType.Undefine();      // характеристика отрицания
//		morpho_feature->m_OfficialType.Undefine();      // тип служебной части речи
//		morpho_feature->m_PhaseVerbType.Undefine();     // фазисность
//		morpho_feature->m_PersonType.Undefine();        // лицо
//
//		morpho_feature->m_PronounGroup.Undefine();      // группа местоимений
//		morpho_feature->m_PronounClass.Undefine();      // класс местоимений
//		morpho_feature->m_ShortType.Undefine();         // краткость
//		morpho_feature->m_AdverbClass.Undefine();       // класс наречий
//		morpho_feature->m_ConjunctionType.Undefine();   // тип союза
//
//
//		morpho_feature->m_DeterminerType.Undefine();    // тип определителя
//		morpho_feature->m_TimeType.Undefine();          // время
//		morpho_feature->m_TransitionalType.Undefine();  // переходность
//		morpho_feature->m_VoiceType.Undefine();         // залог
//		morpho_feature->m_VerbForm.Undefine();          // форма глагола
//
//		morpho_feature->m_VerbType.Undefine();          // тип глагола
//		morpho_feature->m_MoodType.Undefine();          // наклонение глагола	
//	}
//	SS_CATCH(L"")
//
//}


void CAMRussianAnalyzer::CodeAdverbs(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature)
{	
	SS_TRY
	{
		m_pos_ADVERB.DegreeType = pMorphoFeature->m_DegreeType.GetSingleValue();
		memcpy( iPartOfSpeech, &m_pos_ADVERB, 4);
	}
	SS_CATCH(L"")


}
void CAMRussianAnalyzer::CodeNumbers(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		m_pos_NUMERALS.AnimInAnimType	= pMorphoFeature->m_AnimInAnimType.GetSingleValue();
		
		if( (pMorphoFeature->m_CaseType.IsIntersectsWith(&Values::CaseType.ctNominative))
			&& (pMorphoFeature->m_CaseType.IsIntersectsWith(&Values::CaseType.ctGenitive)) )
		{
			m_pos_NUMERALS.CaseType			= Values::CaseType.Undefined.GetSingleValue();
		}else
		{
			m_pos_NUMERALS.CaseType			= pMorphoFeature->m_CaseType.GetSingleValue();
		}

		m_pos_NUMERALS.GenderType		= pMorphoFeature->m_GenderType.GetSingleValue();
		m_pos_NUMERALS.NumberType		= pMorphoFeature->m_NumberType.GetSingleValue();
		memcpy( iPartOfSpeech, &m_pos_NUMERALS, 4);
	}
	SS_CATCH(L"")


}
void CAMRussianAnalyzer::CodeAdjectives(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		m_pos_ADJECTIVE.AnimInAnimType	= pMorphoFeature->m_AnimInAnimType.GetSingleValue();
		m_pos_ADJECTIVE.CaseType		= pMorphoFeature->m_CaseType.GetSingleValue();
		m_pos_ADJECTIVE.DegreeType		= pMorphoFeature->m_DegreeType.GetSingleValue();


		///*	Если род общий, то его преобразовать в мужской и женский	*/
		//if(pMorphoFeature->m_GenderType.Equal(Values::GenderType.gtGeneral))
		//{
		//	pMorphoFeature->m_GenderType.AssignOR(&Values::GenderType.gtFemale);
		//	pMorphoFeature->m_GenderType.AssignOR(&Values::GenderType.gtMasculine);
		//}
		//else
		//	/*	если род неопределен	*/
		//	if(pMorphoFeature->m_GenderType.Equal(Values::GenderType.Undefined))
		//	{
		//		pMorphoFeature->m_GenderType.AssignOR(&Values::GenderType.gtFemale);
		//		pMorphoFeature->m_GenderType.AssignOR(&Values::GenderType.gtMasculine);
		//		pMorphoFeature->m_GenderType.AssignOR(&Values::GenderType.gtNeutral);
		//	}


		if (pMorphoFeature->m_GenderType.IsIntersectsWith(&Values::GenderType.gtGeneral))
		{
			m_pos_ADJECTIVE.GenderType = Values::GenderType.gtGeneral.GetSingleValue();
		}
		else if ((pMorphoFeature->m_GenderType.IsIntersectsWith(&Values::GenderType.gtMasculine)) 
			&& (pMorphoFeature->m_GenderType.IsIntersectsWith(&Values::GenderType.gtFemale))) 
		{
			m_pos_ADJECTIVE.GenderType = Values::GenderType.Undefined.GetSingleValue();
		}else 
		{
			m_pos_ADJECTIVE.GenderType		= pMorphoFeature->m_GenderType.GetSingleValue();
		}	
		
		m_pos_ADJECTIVE.NumberType		= pMorphoFeature->m_NumberType.GetSingleValue();
		m_pos_ADJECTIVE.ShortType		= pMorphoFeature->m_ShortType.GetSingleValue();

		memcpy( iPartOfSpeech, &m_pos_ADJECTIVE, 4);
	}
	SS_CATCH(L"")
}

void CAMRussianAnalyzer::EnCodeAdjectives(int iPartOfSpeech, CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		memcpy( &m_pos_ADJECTIVE, &iPartOfSpeech, 4);
		pMorphoFeature->m_AnimInAnimType	= m_pos_ADJECTIVE.AnimInAnimType;
		pMorphoFeature->m_CaseType			= m_pos_ADJECTIVE.CaseType;
		pMorphoFeature->m_DegreeType		= m_pos_ADJECTIVE.DegreeType;
		pMorphoFeature->m_GenderType		= m_pos_ADJECTIVE.GenderType;
		pMorphoFeature->m_NumberType		= m_pos_ADJECTIVE.NumberType;
		pMorphoFeature->m_ShortType			= m_pos_ADJECTIVE.ShortType;
	}
	SS_CATCH(L"")
}


void CAMRussianAnalyzer::CodeVerbs(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		m_pos_VERB.VoiceType		= pMorphoFeature->m_VoiceType.GetSingleValue();
		m_pos_VERB.PhaseVerbType	= pMorphoFeature->m_PhaseVerbType.GetSingleValue();
		m_pos_VERB.TransitionalType	= pMorphoFeature->m_TransitionalType.GetSingleValue();
		
		m_pos_VERB.GenderType		= pMorphoFeature->m_GenderType.GetSingleValue();
		m_pos_VERB.NumberType		= pMorphoFeature->m_NumberType.GetSingleValue();
		m_pos_VERB.PersonType		= pMorphoFeature->m_PersonType.GetSingleValue();
		m_pos_VERB.TimeType			= pMorphoFeature->m_TimeType.GetSingleValue();
		m_pos_VERB.VerbForm			= pMorphoFeature->m_VerbForm.GetSingleValue();
		m_pos_VERB.VoiceType		= pMorphoFeature->m_VoiceType.GetSingleValue();
		

		m_pos_VERB.AnimInAnimType	= pMorphoFeature->m_AnimInAnimType.GetSingleValue();
		m_pos_VERB.CaseType			= pMorphoFeature->m_CaseType.GetSingleValue();
		m_pos_VERB.ShortType		= pMorphoFeature->m_ShortType.GetSingleValue();
		
		memcpy( iPartOfSpeech, &m_pos_VERB, 4);
	}
	SS_CATCH(L"")
	

}

void CAMRussianAnalyzer::CodeNouns(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		if ((pMorphoFeature->m_CaseType.IsIntersectsWith(&Values::CaseType.ctNominative)) 
			&& (pMorphoFeature->m_CaseType.IsIntersectsWith(&Values::CaseType.ctGenitive)))
		{
			m_pos_NOUNS.CaseType = Values::CaseType.Undefined.GetSingleValue();
		}else
		{
			m_pos_NOUNS.CaseType		= pMorphoFeature->m_CaseType.GetSingleValue();
		}
		
		if ((pMorphoFeature->m_NumberType.IsIntersectsWith(&Values::NumberType.ntPlural)) &&
			(pMorphoFeature->m_NumberType.IsIntersectsWith(&Values::NumberType.ntSingle)))
		{
			m_pos_NOUNS.NumberType		= Values::NumberType.Undefined.GetSingleValue();
		}else
		{
			m_pos_NOUNS.NumberType		= pMorphoFeature->m_NumberType.GetSingleValue();
		}

		

		memcpy( iPartOfSpeech, &m_pos_NOUNS, 4);

	}
	SS_CATCH(L"")

}

void CAMRussianAnalyzer::CodeOfficial(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		if(pMorphoFeature->m_OfficialType.Equal(Values::OfficialType.otPretext))
		{
			m_pos_PRETEXT.CaseType	 = pMorphoFeature->m_CaseType.GetSingleValue();
			memcpy( iPartOfSpeech, &m_pos_PRETEXT, 4);
		}else
		{
			*iPartOfSpeech = 0;
		}	
	}
	SS_CATCH(L"")
}

void CAMRussianAnalyzer::PostAnalyseCaseType(CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{

		if(
			(pMorphoFeature->m_CaseType.Equal(Values::CaseType.Undefined)) ||
			(pMorphoFeature->m_CaseType.Equal(Values::CaseType.ctCommon))
			)
		{
			pMorphoFeature->m_CaseType.AssignOR(&Values::CaseType.ctNominative);
			pMorphoFeature->m_CaseType.AssignOR(&Values::CaseType.ctGenitive);
			pMorphoFeature->m_CaseType.AssignOR(&Values::CaseType.ctDative);
			pMorphoFeature->m_CaseType.AssignOR(&Values::CaseType.ctAccusative);
			pMorphoFeature->m_CaseType.AssignOR(&Values::CaseType.ctInstrumental);
			pMorphoFeature->m_CaseType.AssignOR(&Values::CaseType.ctLocative);			
		}

	}
	SS_CATCH(L"")
}


void CAMRussianAnalyzer::PostAnalyseNumberType(CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		if(pMorphoFeature->m_NumberType.Equal(Values::NumberType.Undefined))
		{
			pMorphoFeature->m_NumberType.AssignOR(&Values::NumberType.ntSingle);
			pMorphoFeature->m_NumberType.AssignOR(&Values::NumberType.ntPlural);
		}
	}
	SS_CATCH(L"")
}

void CAMRussianAnalyzer::PostAnalyseGenderType(CMorphoFeature *pMorphoFeature)
{
	SS_TRY
	{
		/*	Если род общий, то его преобразовать в мужской и женский	*/
		if(pMorphoFeature->m_GenderType.Equal(Values::GenderType.gtGeneral))
		{
			pMorphoFeature->m_GenderType.AssignOR(&Values::GenderType.gtFemale);
			pMorphoFeature->m_GenderType.AssignOR(&Values::GenderType.gtMasculine);
		}
		else
			/*	если род неопределен	*/
			if(pMorphoFeature->m_GenderType.Equal(Values::GenderType.Undefined))
		{
			pMorphoFeature->m_GenderType.AssignOR(&Values::GenderType.gtFemale);
			pMorphoFeature->m_GenderType.AssignOR(&Values::GenderType.gtMasculine);
			pMorphoFeature->m_GenderType.AssignOR(&Values::GenderType.gtNeutral);
		}
	}
	SS_CATCH(L"")
}

}
}
}
