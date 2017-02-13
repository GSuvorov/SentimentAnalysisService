#pragma once

// с помощью дефайнов мы определяем размер (в битах) для каждой из МХ
// данное значение будет использоваться как при хранениии, так и (ВНИМАНИЕ!!!)
// при использовании для контроля целостности
#define ANIMINANIMTYPE_BIT_SIZE	2	//m_AnimInAnimType 	// одушевленность
#define ASPECTTYPE_BIT_SIZE	2		//m_AspectType			// вид глаголов
#define ADVERBTYPE_BIT_SIZE 4		//m_AdverbType			// тип наречия
#define ADVERBCLASS_BIT_SIZE 2		//m_AdverbClass		// класс наречий
#define CASETYPE_BIT_SIZE 3			//m_CaseType			// падеж
#define COUNTTYPE_BIT_SIZE 2		//m_CountType			// исчисляемость
#define COMMONTYPE_BIT_SIZE	1		//m_CommonType			// нарицательность 
#define CONJUNCTIONTYPE_BIT_SIZE 2	//m_ConjunctionType	// тип союза
#define DEGREETYPE_BIT_SIZE	2		//m_DegreeType			// степень сравнения
#define DETERMINERTYPE_BIT_SIZE	1   //m_DeterminerType		// тип определителя
#define GENDERTYPE_BIT_SIZE 3		//m_GenderType			// род
#define MOODTYPE_BIT_SIZE 2			//m_MoodType
#define NUMBERTYPE_BIT_SIZE	2		//m_NumberType			// число
#define NUMERICTYPE_BIT_SIZE 2		//m_NumericType		// тип числительного
#define NEGATIVETYPE_BIT_SIZE 1		//m_NegativeType		// характеристика отрицания
#define OFFICIALTYPE_BIT_SIZE 4		//m_OfficialType		// тип служебной части речи
#define PARTOFSPEECHTYPES_BIT_SIZE 3 
#define PHASEVERBTYPE_BIT_SIZE 2	//m_PhaseVerbType		// фазисность
#define PERSONTYPE_BIT_SIZE 2		//m_PersonType			// лицо
#define PRONOUNGROUP_BIT_SIZE 2		//m_PronounGroup		// группа местоимений
#define PRONOUNCLASS_BIT_SIZE 4		//m_PronounClass		// класс местоимений
#define SHORTTYPE_BIT_SIZE 1		//m_ShortType			// краткость		
#define TIMETYPE_BIT_SIZE 2			//m_TimeType			// время
#define TRANSITIONALTYPE_BIT_SIZE 2	//m_TransitionalType	// переходность
#define VOICETYPE_BIT_SIZE 2		//m_VoiceType			// залог
#define VERBFORM_BIT_SIZE 3			//m_VerbForm			// форма глагола
#define VERBTYPE_BIT_SIZE 2			//m_VerbType			// тип глагола
#define Uniqueness_BIT_SIZE	1		// уникальность морфологической характеристики по части речи
#define OperationMod_BIT_SIZE		2	// выполняемая операция над морфологическими характеристиками
#define ExistenceMod_BIT_SIZE		2	// режим определения морфологических характеристик 
#define TDetectWordAnalyzeMode_BIT_SIZE	2	// режим анализа морфологических характеристик при синтаксическом анализе


//СЕМАНТИКА

#define SEMANTIC_TYPES_BIT_SIZE 	5
#define SemanticType_BIT_SIZE		3
#define ServiceTypes_BIT_SIZE		2
#define OrganizationType_BIT_SIZE	3
#define SemanticPersonType_BIT_SIZE	3
#define MeasureType_BIT_SIZE		4
#define PlaceType_BIT_SIZE			3
#define SemanticTimeType_BIT_SIZE	3
#define SemanticType_BIT_SIZE		3
#define SemanticVerbType_BIT_SIZE		5	
#define TSVBeType_BIT_SIZE				3
#define TSVAcquirePossessType_BIT_SIZE	2
#define TSVAcquireType_BIT_SIZE			2
#define TSVEndType_BIT_SIZE				2
#define TSVMoveType_BIT_SIZE			2
#define TSVEmotionType_BIT_SIZE			2
#define TSVExperiencedType_BIT_SIZE		2
#define TSVSearchType_BIT_SIZE			2

//СИНТАКСИС

#define VerbConstruction_BIT_SIZE				4 
#define AdjectiveConstruction_BIT_SIZE			2
#define NounConstruction_BIT_SIZE				2

#define TYPEOFENDINGOBJECTFORVERB_BIT_SIZE		5 
#define ObjectsOrder_BIT_SIZE					2
#define PositionToDetectedObject_BIT_SIZE		2
#define ConnectionsObligation_BIT_SIZE			1
#define ScopeOfTheRule_BIT_SIZE					2
#define SentenceType_BIT_SIZE					2
#define CombiType_BIT_SIZE						3
#define SyntaxCategories_BIT_SIZE				5
#define HeadDependentType_BIT_SIZE				2
#define AgreementType_BIT_SIZE					2
#define PositionInBlock_BIT_SIZE				2
