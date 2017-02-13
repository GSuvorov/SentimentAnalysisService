// bitfieldsstructures
#pragma once

namespace SS
{
namespace MorphoFeaturesCoding
{

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*! \struct BASE_MORPHO_INFO
*  \brief     содержит базовый набор морфологических характеристик для кодирования
*/
struct  BASE_MORPHO_INFO{
unsigned char
	SemanticsTypes		:3, // семантические характеристики
	NegativeType		:NEGATIVETYPE_BIT_SIZE,	// отрицание				
	PartOfSpeechTypes	:PARTOFSPEECHTYPES_BIT_SIZE,// часть речи				
	Empty				:1; 
BASE_MORPHO_INFO() : SemanticsTypes(0),  NegativeType(0), PartOfSpeechTypes(0), Empty(0)
{}
} ;


namespace English
{
//////////////////////////////////////////////////////////////////////////

/*! \struct VERBS_MORPHO_INFO
*  \brief     содержит набор морфологических характеристик для кодирования информации о глаголах
*/
struct  VERBS_MORPHO_INFO{
	unsigned char
		Transitivity		:TRANSITIONALTYPE_BIT_SIZE, // семантические характеристики
		VerbType			:VERBTYPE_BIT_SIZE,	// тип глагола				
		Empty				:4;	// пустые биты				
	VERBS_MORPHO_INFO() : Transitivity(0), VerbType(0), Empty(0)
	{}
} ;

/*! \struct VERBS_PART_OF_SPEECH
*  \brief     содержит набор морфологических характеристик для кодирования информации о глаголах
*/
struct  VERBS_PART_OF_SPEECH{
	unsigned short
		VerbForm				:VERBFORM_BIT_SIZE,	// форма глагола
		TimeType 				:TIMETYPE_BIT_SIZE,	// время глагола
		PersonType				:PERSONTYPE_BIT_SIZE,	// лицо			
		NumberType				:NUMBERTYPE_BIT_SIZE,	// число
		VoiceType				:VOICETYPE_BIT_SIZE,	// залог
		Empty					:5;	// пустые биты		
	VERBS_PART_OF_SPEECH() : VerbForm(0), TimeType(0), PersonType(0), NumberType(0), Empty(0), VoiceType(0)
	{}
} ;


//////////////////////////////////////////////////////////////////////////


/*! \struct NOUNS_MORPHO_INFO
*  \brief     содержит набор морфологических характеристик для кодирования информации о существительных
*/			
struct  NOUNS_MORPHO_INFO{
	unsigned char
		AnimInAnimType		:ANIMINANIMTYPE_BIT_SIZE, // одушевленность
		GenderType			:GENDERTYPE_BIT_SIZE,	// род				
		CountType			:COUNTTYPE_BIT_SIZE,	// исчисляемость			
		Empty				:1;	// пустые биты				
	NOUNS_MORPHO_INFO() : Empty(0), CountType(0), GenderType(0), AnimInAnimType(0)
	{
	}
} ;

/*! \struct NOUNS_PART_OF_SPEECH
*  \brief     содержит набор морфологических характеристик для кодирования информации о существительных
*/			
struct  NOUNS_PART_OF_SPEECH{
	unsigned short
		NumberType			:NUMBERTYPE_BIT_SIZE,	// число
		CaseType			:CASETYPE_BIT_SIZE,		// падеж
		Empty				:11;					// пустые биты				
	NOUNS_PART_OF_SPEECH() : Empty(0), CaseType(0), NumberType(0)
	{}
} ;

//////////////////////////////////////////////////////////////////////////


/*! \struct NUMERALS_MORPHO_INFO
*  \brief     содержит набор морфологических характеристик для кодирования информации о числительных
*/			
struct  NUMERALS_MORPHO_INFO{
	unsigned char
		Empty				:8;	// пустые биты				
	NUMERALS_MORPHO_INFO() : Empty(0)
	{
	}
} ;

/*! \struct NUMERALS_PART_OF_SPEECH
*  \brief     содержит набор морфологических характеристик для кодирования информации о числительных
*/			
struct  NUMERALS_PART_OF_SPEECH{
	unsigned short
		NumericType			:NUMERICTYPE_BIT_SIZE, // тип числительного
		Empty				:14;// пустые биты				
	NUMERALS_PART_OF_SPEECH() : NumericType(0), Empty(0)
	{}
} ;
//////////////////////////////////////////////////////////////////////////

/*! \struct ADJECTIVES_MORPHO_INFO
*  \brief     содержит набор морфологических характеристик для кодирования информации о прилагательных
*/			
struct  ADJECTIVES_MORPHO_INFO{
	unsigned char
		Empty				:8;	// пустые биты				
	ADJECTIVES_MORPHO_INFO() : Empty(0)
	{}
} ;

/*! \struct ADJECTIVES_PART_OF_SPEECH
*  \brief     содержит набор морфологических характеристик для кодирования информации о прилагательных
*/			
struct  ADJECTIVES_PART_OF_SPEECH{
	unsigned short
		DegreeType			:DEGREETYPE_BIT_SIZE,	// степень сравнения
		Empty				:14;// пустые биты				
	ADJECTIVES_PART_OF_SPEECH() : DegreeType(0), Empty(0)
	{}
} ;

//////////////////////////////////////////////////////////////////////////


/*! \struct ADVERBS_MORPHO_INFO
*  \brief     содержит набор морфологических характеристик для кодирования информации о наречиях
*/			
struct  ADVERBS_MORPHO_INFO{
	unsigned char
		AdverbType			:ADVERBTYPE_BIT_SIZE, // тип наречия
		AdverbClass			:ADVERBCLASS_BIT_SIZE, // класс наречий
		Empty				:2;	// пустые биты							
	ADVERBS_MORPHO_INFO() : AdverbType(0), AdverbClass(0), Empty(0)
	{}
} ;

/*! \struct ADVERBS_PART_OF_SPEECH
*  \brief     содержит набор морфологических характеристик для кодирования информации о наречиях
*/			
struct  ADVERBS_PART_OF_SPEECH{
	unsigned short
		DegreeType			:DEGREETYPE_BIT_SIZE,	// степень сравнения
		Empty				:14;// пустые биты				
	ADVERBS_PART_OF_SPEECH(): DegreeType(0),  Empty(0)
	{}
} ;
//////////////////////////////////////////////////////////////////////////


/*! \struct PRONOUNS_MORPHO_INFO
*  \brief     содержит набор морфологических характеристик для кодирования информации о местоимениях
*/			
struct  PRONOUNS_MORPHO_INFO{
	unsigned char
		GenderType			:GENDERTYPE_BIT_SIZE,	// род				
		PersonType			:PERSONTYPE_BIT_SIZE,	// лицо				
		Empty				:sizeof(unsigned char) * CHAR_BIT - 
                    GENDERTYPE_BIT_SIZE - PERSONTYPE_BIT_SIZE;	// пустые биты
	PRONOUNS_MORPHO_INFO() : Empty(0), PersonType(0), GenderType(0)
	{}
} ;	

/*! \struct PRONOUNS_PART_OF_SPEECH
*  \brief     содержит набор морфологических характеристик для кодирования информации о местоимениях
*/			
struct  PRONOUNS_PART_OF_SPEECH{
	unsigned short
		PronounGroup		:PRONOUNGROUP_BIT_SIZE, // группа местоимений 
		PronounClass		:PRONOUNCLASS_BIT_SIZE,	// класс местоимений
		CaseType			:CASETYPE_BIT_SIZE,	// падеж	
		NumberType			:NUMBERTYPE_BIT_SIZE,	// число
		AnimInAnimType		:ANIMINANIMTYPE_BIT_SIZE,
		Empty				:sizeof(unsigned short) * CHAR_BIT - 
                    PRONOUNGROUP_BIT_SIZE - PRONOUNCLASS_BIT_SIZE - 
                    CASETYPE_BIT_SIZE - NUMBERTYPE_BIT_SIZE - ANIMINANIMTYPE_BIT_SIZE;// пустые биты				
	PRONOUNS_PART_OF_SPEECH() : PronounClass(0), CaseType(0), NumberType(0), AnimInAnimType(0), PronounGroup(0), Empty(0)
	{}
} ;
//////////////////////////////////////////////////////////////////////////


/*! \struct OFFICIAL_MORPHO_INFO
*  \brief     содержит набор морфологических характеристик для кодирования информации о служебных частях речи
*/			
struct  OFFICIAL_MORPHO_INFO{
	unsigned short
		OfficialType	:OFFICIALTYPE_BIT_SIZE,	// служебные части речи
		ConjunctionType :CONJUNCTIONTYPE_BIT_SIZE,	// тип союза
		DeterminerType	:DETERMINERTYPE_BIT_SIZE, // тип определителя
		Empty			:1;	// пустые биты
	OFFICIAL_MORPHO_INFO() : OfficialType(0), ConjunctionType(0), DeterminerType(0), Empty(0)
	{}
} ;

};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

//	namespace RUSSIAN
//	{
//		////////////////////////////////////////////////////////////////////////////////////
//		////////////////////////////////////////////////////////////////////////////////////
//		///*! \struct MORPHO_INFO
//		//*  \brief     содержит базовый набор морфологических характеристик для кодирования 
//		//*/			
//		//		struct  MORPHO_INFO {
//		//			unsigned short
//		//				PartOfSpeechTypes	:PARTOFSPEECHTYPES_BIT_SIZE,	// часть речи
//		//				SemanticsTypes		:3, 	// семантические характеристики
//		//				NegativeType		:NEGATIVETYPE_BIT_SIZE,	// отрицание
//		//				EmptyBits			:9;	// пустые биты
//		//		} ;

//		///*! \struct PART_OF_SPEECH_OFFICIAL
//		//*  \brief     содержит набор морфологических характеристик для кодирования информации о служебных частях речи
//		//*/			
//		//		struct  PART_OF_SPEECH_OFFICIAL{
//		//			unsigned short
//		//				OfficialType	:OFFICIALTYPE_BIT_SIZE,	// служебные части речи
//		//				EmptyBits		:12;	// пустые биты
//		//		} ;

//		///*! \struct SPECIAL_MORPHO_PRETEXT
//		//*  \brief     содержит набор морфологических характеристик для кодирования информации о предлогах
//		//*/			
//		//		struct  SPECIAL_MORPHO_PRETEXT{
//		//			unsigned short
//		//				CaseType		:CASETYPE_BIT_SIZE,	// Падеж
//		//				EmptyBits		:13;	// пустые биты
//		//		};

//		///*! \struct PART_OF_SPEECH_ADJECTIVE_NOUN_OTHER
//		//*  \brief     содержит набор морфологических характеристик для кодирования информации о существительных, прилагательных и др частях речи
//		//*/			
//		//		struct  PART_OF_SPEECH_ADJECTIVE_NOUN_OTHER{
//		//			unsigned short
//		//				CaseType		:CASETYPE_BIT_SIZE,	//Падеж
//		//				GenderType		:GENDERTYPE_BIT_SIZE,	//Род				
//		//				NumberType		:NUMBERTYPE_BIT_SIZE,	//число 
//		//				AnimInAnimType	:ANIMINANIMTYPE_BIT_SIZE,	//одушевленность	
//		//				EmptyBits		:6;	//пустые биты
//		//		} ;


//		///*! \struct SPECIAL_MORPHO_ADJECTIVE
//		//*  \brief     содержит набор морфологических характеристик для кодирования информации о прилагательных
//		//*/			
//		//		struct  SPECIAL_MORPHO_ADJECTIVE{
//		//			unsigned short
//		//			ShortType	:SHORTTYPE_BIT_SIZE,	//Краткость
//		//			DegreeType	:DEGREETYPE_BIT_SIZE,	//Степень сравнения
//		//			PhaseVerbType	:PHASEVERBTYPE_BIT_SIZE,	//фазисность
//		//			// NegativeType	:NEGATIVETYPE_BIT_SIZE,	//Отрицание
//		//			EmptyBits		:11;	// пустые биты
//		//		} ;

//		///*! \struct SPECIAL_MORPHO_PRONOUN
//		//*  \brief     содержит набор морфологических характеристик для кодирования информации о местоимениях
//		//*/				
//		//		struct  SPECIAL_MORPHO_PRONOUN{
//		//			unsigned short
//		//				PronounGroup	:PRONOUNGROUP_BIT_SIZE,	//группа местоимений
//		//				PronounClass	:PRONOUNCLASS_BIT_SIZE,	//разряд местоимений
//		//				PersonType	:PERSONTYPE_BIT_SIZE,	//лицо
//		//				EmptyBits		:8;	// пустые биты
//		//			SPECIAL_MORPHO_PRONOUN() : PronounGroup(0), PronounClass(0), PersonType(0), EmptyBits(0) {}
//		//		} ;

//		///*! \struct SPECIAL_MORPHO_NUMERAL
//		//*  \brief     содержит набор морфологических характеристик для кодирования информации о числительных
//		//*/				
//		//		struct  SPECIAL_MORPHO_NUMERAL{
//		//			unsigned short
//		//				NumericType		:NUMERICTYPE_BIT_SIZE,	//тип числительного
//		//				EmptyBits			:14;	// пустые биты
//		//		} ;

//		///*! \struct PART_OF_SPEECH_ADVERB
//		//*  \brief     содержит набор морфологических характеристик для кодирования информации о наречиях
//		//*/				
//		//		struct  PART_OF_SPEECH_ADVERB{
//		//			unsigned short
//		//				AdverbType	:ADVERBTYPE_BIT_SIZE,	//тип наречия 
//		//				EmptyBits		:12;	//пустые биты
//		//		} ;

//		///*! \struct PART_OF_SPEECH_VERB
//		//*  \brief     содержит набор морфологических характеристик для кодирования информации о глаголах
//		//*/				
//		//		struct  PART_OF_SPEECH_VERB{
//		//			unsigned short
//		//				VerbForm			:VERBFORM_BIT_SIZE,	// Форма глагола
//		//				AspectType			:ASPECTTYPE_BIT_SIZE,	// Вид глагола
//		//				TransitionalType	:1,	// переходность					 
//		//				VoiceType			:VOICETYPE_BIT_SIZE,	//Залог	
//		//				VerbType			:VERBTYPE_BIT_SIZE,	// Форма глагола
//		//				EmptyBits			:6;	//пустые биты					 
//		//		} ;

//		///*! \struct SPECIAL_MORPHO_VERB_PERSONAL
//		//*  \brief     содержит набор морфологических характеристик для кодирования информации о личных глаголах
//		//*/		
//		//		struct  SPECIAL_MORPHO_VERB_PERSONAL{
//		//			unsigned short
//		//				TimeType		:TIMETYPE_BIT_SIZE,	// время глагола
//		//				NumberType	:NUMBERTYPE_BIT_SIZE,	//Число 
//		//				GenderType	:GENDERTYPE_BIT_SIZE,	//Род				
//		//				PhaseVerbType	:PHASEVERBTYPE_BIT_SIZE,	//фазисность
//		//				PersonType	:PERSONTYPE_BIT_SIZE,	//лицо			
//		//				EmptyBits		:5;	//пустые биты					 
//		//		} ;

//		///*! \struct SPECIAL_MORPHO_VERB_IMPERSONAL
//		//*  \brief     содержит набор морфологических характеристик для кодирования информации о безличных глаголах
//		//*/		
//		//		struct  SPECIAL_MORPHO_VERB_IMPERSONAL{
//		//			unsigned short
//		//				TimeType		:TIMETYPE_BIT_SIZE,	//время глагола
//		//				NumberType	:NUMBERTYPE_BIT_SIZE,	//Число 
//		//				GenderType	:GENDERTYPE_BIT_SIZE,	//Род				
//		//				EmptyBits		:9;	//пустые биты					 
//		//		} ;

//		///*! \struct SPECIAL_MORPHO_PARTICIPLE
//		//*  \brief     содержит набор морфологических характеристик для кодирования информации о причастиях
//		//*/		
//		//		struct  SPECIAL_MORPHO_PARTICIPLE{
//		//			unsigned short
//		//				TimeType		:TIMETYPE_BIT_SIZE,	//время глагола
//		//				NumberType		:NUMBERTYPE_BIT_SIZE,	//Число 
//		//				GenderType		:GENDERTYPE_BIT_SIZE,	//Род	
//		//				CaseType		:CASETYPE_BIT_SIZE,	//Падеж
//		//				AnimInAnimType	:ANIMINANIMTYPE_BIT_SIZE,	//Одушевленность
//		//				ShortType		:SHORTTYPE_BIT_SIZE,	//Краткость
//		//				//NegativeType	:NEGATIVETYPE_BIT_SIZE,	//Отрицание
//		//				EmptyBits		:3;	//пустые биты					 
//		//		} ;
//		//

//		///*! \struct SPECIAL_MORPHO_IMPERATIVE
//		//*  \brief     содержит набор морфологических характеристик для кодирования информации о глаголах в императиве
//		//*/	
//		//		struct  SPECIAL_MORPHO_IMPERATIVE{
//		//			unsigned short
//		//				NumberType	:NUMBERTYPE_BIT_SIZE,	//Число 
//		//				PersonType	:PERSONTYPE_BIT_SIZE,	//лицо
//		//				EmptyBits		:12;	//пустые биты					 
//		//		} ;

//	};


//namespace NEW
//{
namespace Russian
{

	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	/*! \struct MORPHO_INFO_OFFICIAL
	*  \brief     содержит набор морфологических характеристик для кодирования информации о служебных частях речи
	*/	

			struct  MORPHO_INFO_OFFICIAL{
				unsigned char
					OfficialType		:OFFICIALTYPE_BIT_SIZE,	// служебные части речи
					Empty				:4;	// пустые биты
				MORPHO_INFO_OFFICIAL() : OfficialType(0), Empty(0)
				{}
			} ;

	
	/*! \struct PART_OF_SPEECH_PRETEXT
	*  \brief     содержит набор морфологических характеристик для кодирования информации о предлогах
	*/			
			struct  PART_OF_SPEECH_PRETEXT{
				unsigned int
					CaseType		:CASETYPE_BIT_SIZE,	// Падеж
					Empty			:29;	// пустые биты
				PART_OF_SPEECH_PRETEXT() : CaseType(0), Empty(0)
				{}
			};

	/*! \struct MORPHO_INFO_NOUNS
	*  \brief     содержит набор морфологических характеристик для кодирования информации о существительных
	*/			
			struct  MORPHO_INFO_NOUNS{
				unsigned char
					AnimInAnimType		:ANIMINANIMTYPE_BIT_SIZE, // одушевленность
					GenderType			:GENDERTYPE_BIT_SIZE,	// род				
					Empty				:3;	// пустые биты				
				MORPHO_INFO_NOUNS() : AnimInAnimType(0), GenderType(0), Empty(0)
				{}
			} ;

	/*! \struct PART_OF_SPEECH_NOUNS
	*  \brief     содержит набор морфологических характеристик для кодирования информации о существительных
	*/			
			struct  PART_OF_SPEECH_NOUNS{
				unsigned int
					CaseType			:CASETYPE_BIT_SIZE,		//Падеж
					NumberType		:NUMBERTYPE_BIT_SIZE,	//число 
					Empty				:27;	//пустые биты
				PART_OF_SPEECH_NOUNS() : CaseType(0), NumberType(0), Empty(0)
				{}
			} ;

	/*! \struct MORPHO_INFO_NUMERALS
	*  \brief     содержит набор морфологических характеристик для кодирования информации о числительных
	*/			
		struct  MORPHO_INFO_NUMERALS{
				unsigned char
					NumericType		:NUMERICTYPE_BIT_SIZE,	//тип числительного
					Empty			:6;	// пустые биты				
				MORPHO_INFO_NUMERALS() : NumericType(0), Empty(0)
				{}
			} ;

	/*! \struct PART_OF_SPEECH_NUMERALS
	*  \brief     содержит набор морфологических характеристик для кодирования информации о числительных
	*/			
			struct  PART_OF_SPEECH_NUMERALS{
				unsigned int
					CaseType		:CASETYPE_BIT_SIZE,	//Падеж
					GenderType		:GENDERTYPE_BIT_SIZE,	//Род				
					NumberType		:NUMBERTYPE_BIT_SIZE,	//число 
					AnimInAnimType	:ANIMINANIMTYPE_BIT_SIZE,	//одушевленность	
					Empty			:22;	//пустые биты
				PART_OF_SPEECH_NUMERALS() : CaseType(0), GenderType(0), NumberType(0), 
					AnimInAnimType(0), Empty(0)
				{}
			} ;

	/*! \struct MORPHO_INFO_ADJECTIVES
	*  \brief     содержит набор морфологических характеристик для кодирования информации о прилагательных
	*/			
		struct  MORPHO_INFO_ADJECTIVES{
				unsigned char
					Empty			:8;	// пустые биты		
				MORPHO_INFO_ADJECTIVES() : Empty(0)
				{}
			} ;

	/*! \struct MORPHO_INFO_PRONOUNS
	*  \brief     содержит набор морфологических характеристик для кодирования информации о местоимениях
	*/			
		struct  MORPHO_INFO_PRONOUNS{
				unsigned char
					Empty			:8;	// пустые биты		
				MORPHO_INFO_PRONOUNS() : Empty(0)
				{}
			} ;

	/*! \struct PART_OF_SPEECH_ADJECTIVE
	*  \brief     содержит набор морфологических характеристик для кодирования информации о прилагательных
	*/			
		struct  PART_OF_SPEECH_ADJECTIVE{
				unsigned int
					CaseType		:CASETYPE_BIT_SIZE,	//Падеж
					GenderType		:GENDERTYPE_BIT_SIZE,	//Род				
					NumberType		:NUMBERTYPE_BIT_SIZE,	//число 
					AnimInAnimType	:ANIMINANIMTYPE_BIT_SIZE,	//одушевленность	
					ShortType		:SHORTTYPE_BIT_SIZE,	//Краткость
					DegreeType		:DEGREETYPE_BIT_SIZE,	//Степень сравнения
					Empty			:19;	//пустые биты
				PART_OF_SPEECH_ADJECTIVE() : CaseType(0), GenderType(0), NumberType(0), 
					AnimInAnimType(0), ShortType(0), DegreeType(0), Empty(0)
				{}
			} ;

	/*! \struct PART_OF_SPEECH_PRONOUNS
	*  \brief     содержит набор морфологических характеристик для кодирования информации о местоимениях
	*/			
			struct  PART_OF_SPEECH_PRONOUNS{
				unsigned int
					CaseType		:CASETYPE_BIT_SIZE,	//Падеж
					GenderType		:GENDERTYPE_BIT_SIZE,	//Род				
					NumberType		:NUMBERTYPE_BIT_SIZE,	//число 
					AnimInAnimType	:ANIMINANIMTYPE_BIT_SIZE,	//одушевленность	
					PronounGroup	:PRONOUNGROUP_BIT_SIZE,	//группа местоимений
					PronounClass	:PRONOUNCLASS_BIT_SIZE,	//разряд местоимений
					PersonType		:PERSONTYPE_BIT_SIZE,	//лицо
					Empty			:14;	//пустые биты
				PART_OF_SPEECH_PRONOUNS() : CaseType(0), GenderType(0), NumberType(0),AnimInAnimType(0),
					PronounGroup(0), PronounClass(0), PersonType(0), Empty(0)
				{}
			} ;

	/*! \struct MORPHO_INFO_ADVERB
	*  \brief     содержит набор морфологических характеристик для кодирования информации о наречиях
	*/			
			struct  MORPHO_INFO_ADVERB{
			unsigned char
				AdverbType		:ADVERBTYPE_BIT_SIZE,	//тип наречия 
				Empty			:4;	//пустые биты
			MORPHO_INFO_ADVERB() : AdverbType(0), Empty(0)
			{}
		} ;

	/*! \struct PART_OF_SPEECH_ADVERB_R
	*  \brief     содержит набор морфологических характеристик для кодирования информации о наречиях
	*/			
			struct  PART_OF_SPEECH_ADVERB_R{
				unsigned int
					DegreeType	:DEGREETYPE_BIT_SIZE,	//степень сравнения
					Empty		:30;	//пустые биты
				PART_OF_SPEECH_ADVERB_R() : DegreeType(0), Empty(0)
				{}
			} ;


	/*! \struct MORPHO_INFO_VERB
	*  \brief     содержит набор морфологических характеристик для кодирования информации о глаголах
	*/			
			struct  MORPHO_INFO_VERB{
				unsigned char
					AspectType			:ASPECTTYPE_BIT_SIZE,	// Вид глагола
					Empty				:4;	//пустые биты					 
				MORPHO_INFO_VERB() : AspectType(0), Empty(0)
				{}
			} ;

	/*! \struct PART_OF_SPEECH_VERB_R
	*  \brief     содержит набор морфологических характеристик для кодирования информации о глаголах
	*/			
			struct  PART_OF_SPEECH_VERB_R{
				unsigned int
					TransitionalType	:TRANSITIONALTYPE_BIT_SIZE,	// переходность					 
					VerbForm			:VERBFORM_BIT_SIZE,	// Форма глагола
					TimeType			:TIMETYPE_BIT_SIZE,	// время глагола
					NumberType			:NUMBERTYPE_BIT_SIZE,	//Число 
					GenderType			:GENDERTYPE_BIT_SIZE,	//Род				
					VoiceType			:VOICETYPE_BIT_SIZE,	//Залог	
					PersonType			:PERSONTYPE_BIT_SIZE,	//лицо			
					CaseType			:CASETYPE_BIT_SIZE,	//Падеж
					AnimInAnimType		:ANIMINANIMTYPE_BIT_SIZE,	//Одушевленность
					ShortType			:SHORTTYPE_BIT_SIZE,	//Краткость
					PhaseVerbType		:PHASEVERBTYPE_BIT_SIZE,	//фазисность
					Empty				:8;	//пустые биты					 
				PART_OF_SPEECH_VERB_R() : VerbForm(0), TimeType(0), NumberType(0), GenderType(0), 
					VoiceType(0), PersonType(0), CaseType(0),  AnimInAnimType(0), ShortType(0), 
					TransitionalType(0), PhaseVerbType(0), Empty(0)
				{}
			} ;

};
//};

};
};
