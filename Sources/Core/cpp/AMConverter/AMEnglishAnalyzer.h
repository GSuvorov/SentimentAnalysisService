#pragma once

#include "stl.h"

#include ".\TAMConverterBitStructures.h"

using namespace SS::MorphoFeaturesCoding;
using namespace SS::MorphoFeaturesCoding::English;

namespace SS
{
namespace Core
{
namespace Features
{
	/// Выполняет конвертацию морфологических характеристик для английского			
	class CAMEnglishAnalyzer
	{
	public:
		/// конструктор
		CAMEnglishAnalyzer(void)
		{
		}
		/// деструктор
		virtual ~CAMEnglishAnalyzer(void)
		{
		}
	public:
		/// Выполняет кодирование набора морфологических характеристик в последовательность чисел
		/** 
			\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
			\param *piMorphoInfo - выходной параметр, содержащий часть закодированной морфологической информации.
			\param *piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
			\return void
		*/
		void Code(/*in*/const CMorphoFeature *pMorphoFeature, /*out*/int *MorphoInfo, /*out*/int *PartOfSpeech);

		/// Выполняет декодирование последовательности чисел в набор морфологических характеристик 
		/** 
			\param iMorphoInfo - входной параметр, содержащий часть закодированной морфологической информации.
			\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
			\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
			\return void
		*/	
		void EnCode(/*in*/int iMorphoInfo, /*in*/int iPartOfSpeech, /*out*/CMorphoFeature *pMorphoFeature);
	private:

		/// выполняет кодирование для местоимений
		/** 
			\param piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
			\param ppMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		*/				
		void CodePronouns(int* piPartOfSpeech, const CMorphoFeature *pMorphoFeature);
		/// выполняет кодирование для наречий
		/** 
			\param piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
			\param ppMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		*/				
		void CodeAdverbs(int* piPartOfSpeech, const CMorphoFeature *pMorphoFeature);
		/// выполняет кодирование для числительных
		/** 
			\param piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
			\param ppMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		*/				
		void CodeNumbers(int* piPartOfSpeech, const CMorphoFeature *pMorphoFeature);
		/// выполняет кодирование для прилагательных
		/** 
			\param piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
			\param ppMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		*/				
		void CodeAdjectives(int* piPartOfSpeech, const CMorphoFeature *pMorphoFeature);
		/// выполняет кодирование для глаголов
		/** 
			\param piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
			\param ppMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		*/				
		void CodeVerbs(int* piPartOfSpeech, const CMorphoFeature *pMorphoFeature);
		/// выполняет кодирование для существительных
		/** 
			\param piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
			\param ppMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		*/				
		void CodeNouns(int* piPartOfSpeech, const CMorphoFeature *pMorphoFeature);


		/// Выполняет декодирование чисел в набор морфологических характеристик для местоимений
		/** 
			\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
			\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
			\return void
		*/
		void EnCodePronouns(int iPartOfSpeech, CMorphoFeature *pMorphoFeature);
		/// Выполняет декодирование чисел в набор морфологических характеристик для наречий
		/** 
			\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
			\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
			\return void
		*/
		void EnCodeAdverbs(int iPartOfSpeech, CMorphoFeature *pMorphoFeature);
		/// Выполняет декодирование чисел в набор морфологических характеристик для числительных
		/** 
			\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
			\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
			\return void
		*/
		void EnCodeNumbers(int iPartOfSpeech, CMorphoFeature *pMorphoFeature);
		/// Выполняет декодирование чисел в набор морфологических характеристик для прилагательных
		/** 
			\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
			\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
			\return void
		*/
		void EnCodeAdjectives(int iPartOfSpeech, CMorphoFeature *pMorphoFeature);
		/// Выполняет декодирование чисел в набор морфологических характеристик для глаголов
		/** 
			\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
			\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
			\return void
		*/
		void EnCodeVerbs(int iPartOfSpeech, CMorphoFeature *pMorphoFeature);

		/// Выполняет декодирование чисел в набор морфологических характеристик для существительных
		/** 
			\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
			\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
			\return void
		*/
		void EnCodeNouns(int iPartOfSpeech, CMorphoFeature *ppMorphoFeature);
		///// инициализация морфологических характеристик
		///** 
		//	\param *pmorpho_feature - указатель на структуру, содержащую морфологические характеристики
		//*/
		//void Initialization(CMorphoFeature *pmorpho_feature);
		/// инициализация вспомогательных структур данных
		void BackInit();

		/// выполняет кодирование морфологических характеристик
		/** 
			\param *pMorphoFeature- cодержит набор морфологических характеристик, которые необходимо закодировать.
			\param *piMorphoInfo	- содержит часть закодированной информации
			\param *piPartOfSpeech	- содержит часть закодированной информации
		*/
		void MorphoInfoBackAnalyzer(const CMorphoFeature *ppMorphoFeature, int *piMorphoInfo, int *piPartOfSpeech);

		/// выполняет декодирование морфологических характеристик
		/** 
			\param iMorphoInfo		- содержит часть закодированной информации
			\param iPartOfSpeech	- содержит часть закодированной информации
			\param *pMorphoFeature	- cодержит набор морфологических характеристик, которые необходимо закодировать.
		*/
		void MorphoInfoAnalyzer(int iMorphoInfo, int iPartOfSpeech, CMorphoFeature *pMorphoFeature);

	private:
		/// выполняет дополнительный анализ числа
		/** 
			\param pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		*/
		void PostAnalyseNumberType(CMorphoFeature *pMorphoFeature);

		/// выполняет дополнительный анализ рода
		/** 
			\param pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		*/
		void PostAnalyseGenderType(CMorphoFeature *pMorphoFeature);

		/// выполняет дополнительный анализ падежа
		/** 
			\param pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		*/
		void PostAnalyseCaseType(CMorphoFeature *pMorphoFeature);

	private:
		/*! \var m_bMI
		*  \brief   содержит базовую часть морфоинформации
		*/
		BASE_MORPHO_INFO m_mi_Base;

		/*! \var m_verbMI
		*  \brief     содержит часть морфоинформации для глаголов
		*/
		VERBS_MORPHO_INFO m_mi_Verb;
		/*! \var m_nounMI
		*  \brief     содержит часть морфоинформации для существительных
		*/
		NOUNS_MORPHO_INFO m_mi_Noun;
		/*! \var m_numMI
		*  \brief     содержит часть морфоинформации для числительных
		*/
		NUMERALS_MORPHO_INFO m_mi_Numerals;
		/*! \var m_adjMI
		*  \brief     содержит часть морфоинформации для прилагательных
		*/
		ADJECTIVES_MORPHO_INFO m_mi_Adjective;
		/*! \var m_advMI
		*  \brief     содержит часть морфоинформации для наречий
		*/
		ADVERBS_MORPHO_INFO m_mi_Adverb;
		/*! \var m_prnMI
		*  \brief     содержит часть морфоинформации для местоимений
		*/
		PRONOUNS_MORPHO_INFO m_mi_Pronoun;
		/*! \var m_offMI
		*  \brief     содержит часть морфоинформации для служебных частей речи
		*/
		OFFICIAL_MORPHO_INFO m_mi_Official;


		/*! \var m_verbPOS
		*  \brief     содержит часть морфоинформации для глаголов
		*/
		VERBS_PART_OF_SPEECH		m_pos_Verb;
		/*! \var m_nounPOS
		*  \brief     содержит часть морфоинформации для существительных
		*/
		NOUNS_PART_OF_SPEECH		m_pos_Noun;
		/*! \var m_numPOS
		*  \brief     содержит часть морфоинформации для числительных
		*/
		NUMERALS_PART_OF_SPEECH		m_pos_Numeral;
		/*! \var m_adjPOS
		*  \brief     содержит часть морфоинформации для прилагательных
		*/
		ADJECTIVES_PART_OF_SPEECH	m_pos_Adjective;
		/*! \var m_advPOS
		*  \brief     содержит часть морфоинформации для наречий
		*/
		ADVERBS_PART_OF_SPEECH		m_pos_Adverb;
		/*! \var m_prnPOS
		*  \brief     содержит часть морфоинформации для местоимений
		*/
		PRONOUNS_PART_OF_SPEECH		m_pos_Pronoun;


	};
};
};
};
