#include "StdAfx.h"
#include ".\morphofeature.h"
//#include "AMMorphoFeature.h"
namespace SS
{
	namespace Core
	{
		namespace Features
		{
//			/// конвертер морфологических характеристик
//			Service::CAMMorphoFeature oAMMorphoFeature;

			CMorphoFeature::CMorphoFeature(void)
				: m_PartOfSpeechTypes(Values::PartOfSpeechTypes.Undefined)
				, m_AnimInAnimType(Values::AnimInAnimType.Undefined)
				, m_AspectType(Values::AspectType.Undefined)
				, m_AdverbType(Values::AdverbType.Undefined)
				, m_CaseType(Values::CaseType.Undefined)
				
				, m_CountType(Values::CountType.Undefined)
				, m_CommonType(Values::CommonType.Undefined)
				, m_DegreeType(Values::DegreeType.Undefined)
				, m_GenderType(Values::GenderType.Undefined)
				, m_NumberType(Values::NumberType.Undefined)
				
				
				, m_NumericType(Values::NumericType.Undefined)
				, m_NegativeType(Values::NegativeType.Undefined)
				, m_OfficialType(Values::OfficialType.Undefined)
				, m_PhaseVerbType(Values::PhaseVerbType.Undefined)
				, m_PersonType(Values::PersonType.Undefined)
				
				, m_PronounGroup(Values::PronounGroup.Undefined)
				, m_PronounClass(Values::PronounClass.Undefined)
				, m_ShortType(Values::ShortType.Undefined)
				, m_AdverbClass(Values::TAdverbClass.Undefined)
				, m_ConjunctionType(Values::TConjunctionType.Undefined)
				
				
				, m_DeterminerType(Values::TDeterminerType.Undefined)
				, m_TimeType(Values::TimeType.Undefined)
				, m_TransitionalType(Values::TransitionalType.Undefined)
				, m_VoiceType(Values::VoiceType.Undefined)
				, m_VerbForm(Values::VerbForm.Undefined)
				
				, m_VerbType(Values::VerbType.Undefined)
				, m_MoodType(Values::MoodType.Undefined)
				, m_iCountMorphoElement(27)
			{
				Init();
			}

			CMorphoFeature::~CMorphoFeature(void)
			{
			}

			void CMorphoFeature::Init()
			{
				m_FeatureCollectionType = Base::fctMorphoFeature;

				AddFeatureToCollection(&m_PartOfSpeechTypes);
				AddFeatureToCollection(&m_AnimInAnimType);
				AddFeatureToCollection(&m_AspectType);
				AddFeatureToCollection(&m_AdverbType);
				AddFeatureToCollection(&m_CaseType);
				
				AddFeatureToCollection(&m_CountType);
				AddFeatureToCollection(&m_CommonType);
				AddFeatureToCollection(&m_DegreeType);
				AddFeatureToCollection(&m_GenderType);
				AddFeatureToCollection(&m_NumberType);
				
				
				AddFeatureToCollection(&m_NumericType);
				AddFeatureToCollection(&m_NegativeType);
				AddFeatureToCollection(&m_OfficialType);
				AddFeatureToCollection(&m_PhaseVerbType);
				AddFeatureToCollection(&m_PersonType);
				
				AddFeatureToCollection(&m_PronounGroup);
				AddFeatureToCollection(&m_PronounClass);
				AddFeatureToCollection(&m_ShortType);
				AddFeatureToCollection(&m_AdverbClass);
				AddFeatureToCollection(&m_ConjunctionType);
				
				
				AddFeatureToCollection(&m_DeterminerType);
				AddFeatureToCollection(&m_TimeType);
				AddFeatureToCollection(&m_TransitionalType);
				AddFeatureToCollection(&m_VoiceType);
				AddFeatureToCollection(&m_VerbForm);
				
				AddFeatureToCollection(&m_VerbType);
				AddFeatureToCollection(&m_MoodType);
			};

			bool CMorphoFeature::IsIntersectsByCaseGenderNumber(const CMorphoFeature& value)
			{
				if (( m_GenderType.IsUndefined() || (m_GenderType.GetValue() & value.m_GenderType.GetValue())>1 ) &&
					( m_NumberType.IsUndefined() || (m_NumberType.GetValue() & value.m_NumberType.GetValue())>1 ) &&
					( m_CaseType.IsUndefined() || (m_CaseType.GetValue() & value.m_CaseType.GetValue())>1 ))
				{
					return true;
				}
				return false;
			}

			bool CMorphoFeature::IsIntersectsWith(const CMorphoFeature& value) const
            {
                if ((m_PartOfSpeechTypes.IsUndefined() || value.m_PartOfSpeechTypes.IsIntersectsWith(&m_PartOfSpeechTypes))&&
				    (m_GenderType.IsUndefined() || value.m_GenderType.IsIntersectsWith(&m_GenderType))&&
					(m_NumberType.IsUndefined() || value.m_NumberType.IsIntersectsWith(&m_NumberType))&&
					(m_AnimInAnimType.IsUndefined() || value.m_AnimInAnimType.IsIntersectsWith(&m_AnimInAnimType))&&
					(m_CaseType.IsUndefined() || value.m_CaseType.IsIntersectsWith(&m_CaseType)) && 
					(m_PronounClass.IsUndefined() || value.m_PronounClass.IsIntersectsWith(&m_PronounClass)) &&
					(m_CommonType.IsUndefined() || value.m_CommonType.IsIntersectsWith(&m_CommonType)) &&
					(m_VerbForm.IsUndefined() || value.m_VerbForm.IsIntersectsWith(&m_VerbForm)) &&
					(m_VoiceType.IsUndefined() || value.m_VoiceType.IsIntersectsWith(&m_VoiceType)) &&
					(m_ShortType.IsUndefined() || value.m_ShortType.IsIntersectsWith(&m_ShortType)) &&
					(m_TransitionalType.IsUndefined() || value.m_TransitionalType.IsIntersectsWith(&m_TransitionalType)) &&
					(m_OfficialType.IsUndefined() || value.m_OfficialType.IsIntersectsWith(&m_OfficialType)) &&
                    (m_VerbType.IsUndefined() || value.m_VerbType.IsIntersectsWith(&m_VerbType)) &&
                    (m_TimeType.IsUndefined() || value.m_TimeType.IsIntersectsWith(&m_TimeType)) &&
					(m_MoodType.IsUndefined() || value.m_MoodType.IsIntersectsWith(&m_MoodType)) &&
					(m_AspectType.IsUndefined() || value.m_AspectType.IsIntersectsWith(&m_AspectType)))
				{
					return true;
				}
                return false;
            }

            bool CMorphoFeature::IsIntersectsWith(const CMorphoFeature& value, SS::Core::Types::EAnalyseLanguage eLang) const
			{
				using namespace SS::Core::Types;

				if (eLang == ealUndefined) 
				{
					//SS_THROW(L"для IsIntersectsWith не определён язык разбора");
					eLang = ealEnglish;
				}
				
				if( !m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.Undefined))
				{
					if((m_PartOfSpeechTypes.GetValue() & value.m_PartOfSpeechTypes.GetValue()) == 0)
						return false;
				}
				else
					return true;

				
				if(value.m_PartOfSpeechTypes.GetValue() & Values::PartOfSpeechTypes.postOfficial.GetValue())
				{
					if( m_OfficialType.IsUndefined() || ( m_OfficialType.GetValue() & value.m_OfficialType.GetValue() ) )
						return true;
				}
				if(value.m_PartOfSpeechTypes.GetValue() & Values::PartOfSpeechTypes.postVerb.GetValue())
				{
					//добавлены проверки для русского языка на :
					//1 - ShortType
					//2 - CaseType
					//
					if (eLang == ealEnglish) 
					{
						if (
						    ( m_TransitionalType.IsUndefined() || (m_TransitionalType.GetValue() & value.m_TransitionalType.GetValue()) )
							&&( m_VerbType.IsUndefined() || (m_VerbType.GetValue() & value.m_VerbType.GetValue()) )
							&&( m_VerbForm.IsUndefined() || (m_VerbForm.GetValue() & value.m_VerbForm.GetValue()) )
							&&( m_TimeType.IsUndefined() || (m_TimeType.GetValue() & value.m_TimeType.GetValue()) )
							&&( m_PersonType.IsUndefined() || (m_PersonType.GetValue() & value.m_PersonType.GetValue()) )
							&&( m_NumberType.IsUndefined() || (m_NumberType.GetValue() & value.m_NumberType.GetValue()) )
							&&( m_VoiceType.IsUndefined() || (m_VoiceType.GetValue() & value.m_VoiceType.GetValue()) )
							&&( m_CommonType.IsUndefined() || (m_CommonType.GetValue() & value.m_CommonType.GetValue()) )
							//&&( m_CaseType.IsUndefined() || (m_CaseType.GetValue() & value.m_CaseType.GetValue()) )
							//&&( m_ShortType.IsUndefined() || (m_ShortType.GetValue() & value.m_ShortType.GetValue()) )
							)
							return true;
					}
					
					if (eLang == ealRussian)
					{
						if (
							( m_TransitionalType.IsUndefined() || (m_TransitionalType.GetValue() & value.m_TransitionalType.GetValue()) )
							&&( m_VerbType.IsUndefined() || (m_VerbType.GetValue() & value.m_VerbType.GetValue()) )
							&&( m_VerbForm.IsUndefined() || (m_VerbForm.GetValue() & value.m_VerbForm.GetValue()) )
							&&( m_TimeType.IsUndefined() || (m_TimeType.GetValue() & value.m_TimeType.GetValue()) )
							&&( m_PersonType.IsUndefined() || (m_PersonType.GetValue() & value.m_PersonType.GetValue()) )
							&&( m_NumberType.IsUndefined() || (m_NumberType.GetValue() & value.m_NumberType.GetValue()) )
							&&( m_VoiceType.IsUndefined() || (m_VoiceType.GetValue() & value.m_VoiceType.GetValue()) )
							&&( m_CaseType.IsUndefined() || (m_CaseType.GetValue() & value.m_CaseType.GetValue()) )
							&&( m_ShortType.IsUndefined() || (m_ShortType.GetValue() & value.m_ShortType.GetValue()) )
							&&( m_CommonType.IsUndefined() || (m_CommonType.GetValue() & value.m_CommonType.GetValue()) )
							)
							return true;					
					}

					
				}
				if(value.m_PartOfSpeechTypes.GetValue() & Values::PartOfSpeechTypes.postNoun.GetValue())
				{
					//bool bRetVal = m_AnimInAnimType.IsUndefined();
					//bRetVal = (m_AnimInAnimType.GetValue() & value.m_AnimInAnimType.GetValue());
					
					//bRetVal = m_GenderType.IsUndefined();
					//bRetVal = (m_GenderType.GetValue() & value.m_GenderType.GetValue());
					
					//bRetVal = m_CountType.IsUndefined();
					//bRetVal = (m_CountType.GetValue() & value.m_CountType.GetValue());
					
					//bRetVal = m_CommonType.IsUndefined();
					//bRetVal = (m_CommonType.GetValue() & value.m_CommonType.GetValue());
					//int i = m_CommonType.GetValue();
					//int j = value.m_CommonType.GetValue();
					
					//bRetVal =  m_NumberType.IsUndefined();
					//bRetVal = (m_NumberType.GetValue() & value.m_NumberType.GetValue());
					
					//bRetVal = m_CaseType.IsUndefined();
					//bRetVal =  (m_CaseType.GetValue() & value.m_CaseType.GetValue());
					
					if (eLang == ealEnglish) 
					{
						if (
							( m_AnimInAnimType.IsUndefined() || (m_AnimInAnimType.GetValue() & value.m_AnimInAnimType.GetValue()) )
							&&( m_GenderType.IsUndefined() || (m_GenderType.GetValue() & value.m_GenderType.GetValue()) )
							&&( m_CountType.IsUndefined()/*m_CountType.Equal(Values::CountType.cntNotCount)*/ || (m_CountType.GetValue() & value.m_CountType.GetValue()) )
							//&&( /*m_CommonType.IsUndefined()*/m_CommonType.Equal(Values::CommonType.cmtCommon) || (m_CommonType.GetValue() & value.m_CommonType.GetValue()) )
							&&( m_NumberType.IsUndefined() || (m_NumberType.GetValue() & value.m_NumberType.GetValue()) )
							&&( m_CaseType.IsUndefined() || (m_CaseType.GetValue() & value.m_CaseType.GetValue()) )
							&&( m_CommonType.IsUndefined() || (m_CommonType.GetValue() & value.m_CommonType.GetValue()) )
							)
							return true;
					}
					else if (eLang == ealRussian) 
					{
						if (
							( m_AnimInAnimType.IsUndefined() || (m_AnimInAnimType.GetValue() & value.m_AnimInAnimType.GetValue()) )
							&&( m_GenderType.IsUndefined() || (m_GenderType.GetValue() & value.m_GenderType.GetValue()) )
							&&( m_CountType.IsUndefined()/*m_CountType.Equal(Values::CountType.cntNotCount)*/ || (m_CountType.GetValue() & value.m_CountType.GetValue()) )
							&&( m_CommonType.IsUndefined()/*m_CommonType.Equal(Values::CommonType.cmtCommon)*/ || (m_CommonType.GetValue() & value.m_CommonType.GetValue()) )
							&&( m_NumberType.IsUndefined() || (m_NumberType.GetValue() & value.m_NumberType.GetValue()) )
							&&( m_CaseType.IsUndefined() || (m_CaseType.GetValue() & value.m_CaseType.GetValue()) )
							)
							return true;
					}
				}
				if(value.m_PartOfSpeechTypes.GetValue() & Values::PartOfSpeechTypes.postAdjective.GetValue())
				{
					//добавлены проверки на:
					//1 - ShortType
					//2 - CaseType
					if (eLang == ealEnglish)
					{
						if (
							( m_DegreeType.IsUndefined() || (m_DegreeType.GetValue() & value.m_DegreeType.GetValue()) )
							&&( m_CommonType.IsUndefined() || (m_CommonType.GetValue() & value.m_CommonType.GetValue()))
						    )
							return true;
					}
					if (eLang == ealRussian)
					{
						if (
							( m_DegreeType.IsUndefined() || (m_DegreeType.GetValue() & value.m_DegreeType.GetValue()) )
							&&( m_CaseType.IsUndefined() || (m_CaseType.GetValue() & value.m_CaseType.GetValue()) )
							&&( m_ShortType.IsUndefined() || (m_ShortType.GetValue() & value.m_ShortType.GetValue()) )
							&&( m_GenderType.IsUndefined() || (m_GenderType.GetValue() & value.m_GenderType.GetValue()) ) 
							&&( m_CommonType.IsUndefined() || (m_CommonType.GetValue() & value.m_CommonType.GetValue()))
						    )
							return true;
					}

				}
				if(value.m_PartOfSpeechTypes.GetValue() & Values::PartOfSpeechTypes.postNumeric.GetValue())
				{
					//добавлены проверки на:
					//1 - CaseType
					if (eLang == ealEnglish)
					{
						if (
							( m_NumericType.IsUndefined() || (m_NumericType.GetValue() & value.m_NumericType.GetValue()) )
							//&&( m_CaseType.IsUndefined() || (m_CaseType.GetValue() & value.m_CaseType.GetValue()) )
							)
							return true;
					}
					if (eLang == ealRussian)
					{
						if (
							( m_NumericType.IsUndefined() || (m_NumericType.GetValue() & value.m_NumericType.GetValue()) )
							&&( m_CaseType.IsUndefined() || (m_CaseType.GetValue() & value.m_CaseType.GetValue()) )
							)
							return true;
					}
					
				}
				if(value.m_PartOfSpeechTypes.GetValue() & Values::PartOfSpeechTypes.postAdverb.GetValue())
				{
					if
					(
					( m_DegreeType.IsUndefined() || (m_DegreeType.GetValue() & value.m_DegreeType.GetValue()) )
					&&( m_AdverbType.IsUndefined() || (m_AdverbType.GetValue() & value.m_AdverbType.GetValue()) )
					&&( m_AdverbClass.IsUndefined() || (m_AdverbClass.GetValue() & value.m_AdverbClass.GetValue()) )
					&&( m_CommonType.IsUndefined() || (m_CommonType.GetValue() & value.m_CommonType.GetValue()))
					)
						return true;
					
					
				}
				if(value.m_PartOfSpeechTypes.GetValue() & Values::PartOfSpeechTypes.postPronoun.GetValue())
				{
					//bool bRetVal = m_PronounGroup.IsUndefined();
					//bRetVal = (m_PronounGroup.GetValue() & value.m_PronounGroup.GetValue());

					//bRetVal = m_PronounClass.IsUndefined();
					//bRetVal = (m_PronounClass.GetValue() & value.m_PronounClass.GetValue());
								
					//bRetVal = m_GenderType.IsUndefined();
					//bRetVal = (m_GenderType.GetValue() & value.m_GenderType.GetValue());
					
					//bRetVal = m_PersonType.IsUndefined();
					//bRetVal = (m_PersonType.GetValue() & value.m_PersonType.GetValue());
					
					//bRetVal = m_CaseType.IsUndefined();
					//bRetVal = (m_CaseType.GetValue() & value.m_CaseType.GetValue());
					
					//bRetVal = m_NumberType.IsUndefined();
					//bRetVal = (m_NumberType.GetValue() & value.m_NumberType.GetValue());
          				
					if
					(
					( m_PronounGroup.IsUndefined() || (m_PronounGroup.GetValue() & value.m_PronounGroup.GetValue()) )
					&&( m_PronounClass.IsUndefined() || (m_PronounClass.GetValue() & value.m_PronounClass.GetValue()) )
					&&( m_GenderType.IsUndefined() || (m_GenderType.GetValue() & value.m_GenderType.GetValue()) )
					&&( m_PersonType.IsUndefined() || (m_PersonType.GetValue() & value.m_PersonType.GetValue()) )
					&&( m_CaseType.IsUndefined() || (m_CaseType.GetValue() & value.m_CaseType.GetValue()) )
					&&( m_NumberType.IsUndefined() || (m_NumberType.GetValue() & value.m_NumberType.GetValue()) )
					)
						return true;

														
				}	
			//SS_CATCH(L"");

				return false;
			};

			///условное равенство с другим набором морфологических характеристик
			bool CMorphoFeature::IsEqualUniformMember( const CMorphoFeature& value ) const
			{
				if( m_PartOfSpeechTypes.GetValue() != value.m_PartOfSpeechTypes.GetValue() )
					return false;						
				
				if( m_PartOfSpeechTypes.GetValue() == Values::PartOfSpeechTypes.postVerb.GetValue() )
				{
					return
						( ( m_VerbType.GetValue() == value.m_VerbType.GetValue() )
						&& ( m_TimeType.GetValue() == value.m_TimeType.GetValue() )
						&& ( m_NumberType.GetValue() == value.m_NumberType.GetValue() ) );
				}
				else if(m_PartOfSpeechTypes.GetValue() == Values::PartOfSpeechTypes.postNoun.GetValue())
				{
					return
						( m_CaseType.GetValue() == value.m_CaseType.GetValue() );
						
				}
				else if(m_PartOfSpeechTypes.GetValue() == Values::PartOfSpeechTypes.postAdjective.GetValue())
				{
					return
						( ( m_GenderType.GetValue() == value.m_GenderType.GetValue() ) &&
						( m_CaseType.GetValue() == value.m_CaseType.GetValue() ) );
				}
				else if(m_PartOfSpeechTypes.GetValue() == Values::PartOfSpeechTypes.postPronoun.GetValue())
				{
					return
						( ( m_PronounClass.GetValue() == value.m_PronounClass.GetValue() )
						&& ( m_CaseType.GetValue() == value.m_CaseType.GetValue() ) );
				}	

				return true;
			}

			//Присваивание набора морфологических характеристик базовой части
			void CMorphoFeature::AssignMorphoFeature(const CMorphoFeature* pValue)
			{
				/*if(pValue->m_FeatureCollectionType != fctMorphoFeature){
				char bff1[20];
				char bff2[20];
				SS_THROW(L"несовпадение типов характеристик при операции, текущий тип = " AND ltoa(m_FeatureCollectionType, bff1, 10) AND "; пришедший тип = " AND ltoa(pValue->m_FeatureCollectionType, bff2, 10));
				}*/
				if( !pValue ){
					//throw std::invalid_argument( "NULL pointer!" );
					return;
				}
				unsigned int baseFeatureCount = 27;
				unsigned int i = 0;
				while(i < baseFeatureCount){
					m_ppFeatures[i]->Assign(pValue->m_ppFeatures[i]);
					i++;
				}
			};	

			///Оператор присваивает из pValue характеристики которые не Undefind
			void CMorphoFeature::AssignValidMorphoFeature(const CMorphoFeature* pValue)
			{
				if (!pValue) { SS_THROW(L"Нулевой указатель."); }
				unsigned int iBaseFeatureCount = 27;
				unsigned int i = 0;
				while (i < iBaseFeatureCount)
				{
					if (!pValue->m_ppFeatures[i]->IsUndefined()) 
					{ m_ppFeatures[i]->Assign(pValue->m_ppFeatures[i]); }
					++i;
				}
			}

			///// Выполняет кодирование набора морфологических характеристик в последовательность чисел
			//void MakeBack( 
			//	///индекс основы слова, морфологические характеристики которого необходимо закодировать
			//	const int iIDSource,
			//	///выходной параметр, содержащий часть закодированной морфологической информации.
			//	int* piMorphoInfo, 
			//	///выходной параметр, содержащий часть закодированной морфологической информации.
			//	int* piPartOfSpeech, 
			//	///выходной параметр, содержащий часть закодированной морфологической информации.
			//	int* piSpecialMorpho
			//	) const
			//{
			//	oAMMorphoFeature.MakeBack(
			//		iIDSource,
			//		piMorphoInfo, 
			//		piPartOfSpeech, 
			//		piSpecialMorpho
			//		);
			//}

			///// Выполняет декодирование последовательности чисел в набор морфологических характеристик 
			//void CMorphoFeature::MakeConvert(
			//	/// индекс основы слова, морфологические характеристики которого необходимо получить
			//	const int iIDSource,
			//	/// входной параметр, содержащий часть закодированной морфологической информации
			//	const int iMorphoInfo, 
			//	/// входной параметр, содержащий часть закодированной морфологической информации
			//	const int iPartOfSpeech, 
			//	/// входной параметр, содержащий часть закодированной морфологической информации.
			//	const int iSpecialMorpho
			//	)
			//{
			//	oAMMorphoFeature.MakeConvert(
			//		iIDSource,
			//		iMorphoInfo, 
			//		iPartOfSpeech, 
			//		iSpecialMorpho
			//		);
			//}

			///// Выполняет кодирование набора морфологических характеристик в последовательность чисел
			//void CMorphoFeature::Code(
			//	///номер словаря для которого необходимо выполнить кодирование морфологических характеристик
			//	const unsigned char uchDictionaryNumber,
			//	///выходной параметр, содержащий часть закодированной морфологической информации.
			//	int* piMorphoInfo, 
			//	///выходной параметр, содержащий часть закодированной морфологической информации.
			//	int* piPartOfSpeech
			//	) const;
			//{
			//	oAMMorphoFeature.Code(
			//		uchDictionaryNumber,
			//		piMorphoInfo, 
			//		piPartOfSpeech
			//		);
			//}
			//
			///// Выполняет декодирование последовательности чисел в набор морфологических характеристик 
			//void CMorphoFeature::EnCode(
			//	///номер словаря для которого необходимо выполнить расшифровку морфологических характеристик
			//	const unsigned char uchDictionaryNumber, 
			//	///входной параметр, содержащий часть закодированной морфологической информации
			//	const int iMorphoInfo,
			//	///входной параметр, содержащий часть закодированной морфологической информации
			//	const int iPartOfSpeech
			//	);
 		//	{
			//	oAMMorphoFeature.EnCode(
			//		uchDictionaryNumber, 
			//		iMorphoInfo,
			//		iPartOfSpeech
			//		);
			//}

		}
	}
}