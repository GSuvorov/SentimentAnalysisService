#pragma once

#pragma warning( disable : 4290 )
#include <stdexcept> 

#include ".\basefeaturecollection.h"
#include "define.h"
#include <vector>
#include "../ASSInterface/TMainAnalyseTypes.h"
#include "../ASCInterface/TDictionaryIndex.h"


namespace SS
{
	namespace Core
	{
		namespace Features
		{
			class CMorphoFeatureApriori;
			class CMorphoFeatureAposteriori;

			/// набор морфологических характеристик
			class CMorphoFeature :
				public SS::Core::Features::Base::CBaseFeatureCollection
			{
				friend class SS::Core::Features::CMorphoFeatureApriori;
				friend class SS::Core::Features::CMorphoFeatureAposteriori;
			public:
				CMorphoFeature(void);
				virtual ~CMorphoFeature(void);

				CMorphoFeature& operator = (const CMorphoFeature& value)
				{
					Assign(&value);
					return *this;
				}
				CMorphoFeature(const CMorphoFeature& value){
					Init();
					*this = value;
				};
				///вернет строковое имя характеристики
				const char* GetFeatureName() const {return "MorphoFeature";};
				///вернет строковое описание характеристики
				const char* GetFeatureDescription() const {return "морфологические характеристики";};

				///пересечение с другим набором по роду числу и падежу
				bool IsIntersectsByCaseGenderNumber(const CMorphoFeature& value);							

                ///условное пересечение с другим набором морфологических характеристик
				bool IsIntersectsWith(const CMorphoFeature& value) const;

				///условное пересечение с другим набором морфологических характеристик
				bool IsIntersectsWith(const CMorphoFeature& value, SS::Core::Types::EAnalyseLanguage eLang) const;

				///условное равенство с другим набором морфологических характеристик
				bool IsEqualUniformMember( const CMorphoFeature& value ) const;

				///оператор присваивания набора морфологических характеристик
				void AssignMorphoFeature(const CMorphoFeature* pValue)throw( std::invalid_argument);

				///Оператор присваивает из pValue характеристики которые не Undefind
				void AssignValidMorphoFeature(const CMorphoFeature* pValue);
				
				Types::PartOfSpeechTypes	m_PartOfSpeechTypes;// часть речи
				Types::AnimInAnimType		m_AnimInAnimType; 	// одушевленность
				Types::AspectType			m_AspectType;		// вид глаголов
				Types::AdverbType			m_AdverbType;		// тип наречия
				Types::CaseType				m_CaseType;			// падеж

				Types::CountType			m_CountType;		// исчисляемость
				Types::CommonType			m_CommonType;		// нарицательность 
				Types::DegreeType			m_DegreeType;		// степень сравнения
				Types::GenderType			m_GenderType;		// род
				Types::NumberType			m_NumberType;		// число


				Types::NumericType			m_NumericType;		// тип числительного
				Types::NegativeType			m_NegativeType;		// характеристика отрицания
				Types::OfficialType			m_OfficialType;		// тип служебной части речи
				Types::PhaseVerbType		m_PhaseVerbType;	// фазисность
				Types::PersonType			m_PersonType;		// лицо

				Types::PronounGroup			m_PronounGroup;		// группа местоимений
				Types::PronounClass			m_PronounClass;		// класс местоимений
				Types::ShortType			m_ShortType;		// краткость		
				Types::TAdverbClass			m_AdverbClass;		// класс наречий
				Types::TConjunctionType		m_ConjunctionType;	// тип союза


				Types::TDeterminerType		m_DeterminerType;	// тип определителя
				Types::TimeType				m_TimeType;			// время
				Types::TransitionalType		m_TransitionalType;	// переходность
				Types::VoiceType			m_VoiceType;		// залог
				Types::VerbForm				m_VerbForm;			// форма глагола

				Types::VerbType				m_VerbType;			// тип глагола
				Types::MoodType				m_MoodType;			// наклонение глагола				


				unsigned int m_iCountMorphoElement ;

			private:
				void Init();

				///// Выполняет кодирование набора морфологических характеристик в последовательность чисел
				//void Code(
				//	///номер словаря для которого необходимо выполнить кодирование морфологических характеристик
				//	const unsigned char uchDictionaryNumber,
				//	///выходной параметр, содержащий часть закодированной морфологической информации.
				//	int* piMorphoInfo, 
				//	///выходной параметр, содержащий часть закодированной морфологической информации.
				//	int* piPartOfSpeech
				//	) const;
				//
				///// Выполняет декодирование последовательности чисел в набор морфологических характеристик 
				//void EnCode(
				//	///номер словаря для которого необходимо выполнить расшифровку морфологических характеристик
				//	const unsigned char uchDictionaryNumber, 
				//	///входной параметр, содержащий часть закодированной морфологической информации
				//	const int iMorphoInfo,
				//	///входной параметр, содержащий часть закодированной морфологической информации
				//	const int iPartOfSpeech
				//	);
			};
		}
	}
}