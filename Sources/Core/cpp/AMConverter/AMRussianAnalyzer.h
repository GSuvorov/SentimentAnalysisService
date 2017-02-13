#pragma once

#include ".\TAMConverterBitStructures.h"

using namespace SS::MorphoFeaturesCoding;
using namespace SS::MorphoFeaturesCoding::Russian;

namespace SS
{
namespace Core
{
namespace Features
{
/// Выполняет конвертацию морфологических характеристик для русского
class CAMRussianAnalyzer
{
public:
	/// конструктор
	CAMRussianAnalyzer(void);
	/// деструктор
	~CAMRussianAnalyzer(void);
public:
	/// Выполняет кодирование набора морфологических характеристик в последовательность чисел
	/** 
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\param *piMorphoInfo - выходной параметр, содержащий часть закодированной морфологической информации.
		\param *piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
	*/
	bool Code(/*in*/const CMorphoFeature *pMorphoFeature, /*out*/int *MorphoInfo, /*out*/int *PartOfSpeech);
	/// Выполняет декодирование последовательности чисел в набор морфологических характеристик 
	/** 
		\param iMorphoInfo - входной параметр, содержащий часть закодированной морфологической информации.
		\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
	*/
	bool EnCode(/*in*/int iMorphoInfo, /*in*/int iPartOfSpeech, /*out*/CMorphoFeature *pMorphoFeature);

private:
	/// инициализация вспомогательных структур данных
	void BackInit();
	///// инициализация морфологических характеристик
	///** 
	//	\param *morpho_feature - указатель на структуру, содержащую морфологические характеристики
	//*/
	//void Initialization(CMorphoFeature *morpho_feature);

	/// выполняет кодирование для местоимений
	/** 
		\param piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
	*/
	void CodePronouns(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature);
	/// выполняет кодирование для наречий
	/** 
		\param piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\return void
	*/
	void CodeAdverbs(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature);
	/// выполняет кодирование для числительных
	/** 
		\param piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\return void
	*/
	void CodeNumbers(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature);
	/// выполняет кодирование для прилагательных
	/** 
		\param piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\return void
	*/
	void CodeAdjectives(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature);
	/// выполняет кодирование для глаголов
	/** 
		\param piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\return void
	*/
	void CodeVerbs(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature);
	/// выполняет кодирование для существительных
	/** 
		\param piPartOfSpeech - выходной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\return void
	*/
	void CodeNouns(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature);
	/// Выполняет декодирование чисела в набор морфологических характеристик для служебных частей речи 
	/** 
		\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\return void
	*/
	void CodeOfficial(int* iPartOfSpeech, const CMorphoFeature *pMorphoFeature);


	/// Выполняет декодирование числа в набор морфологических характеристик для местоимений 
	/** 
		\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\return void
	*/
	void EnCodePronouns(int iPartOfSpeech, CMorphoFeature *pMorphoFeature);
	/// Выполняет декодирование числа в набор морфологических характеристик для наречий
	/** 
		\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\return void
	*/
	void EnCodeAdverbs(int iPartOfSpeech, CMorphoFeature *pMorphoFeature);
	/// Выполняет декодирование числа в набор морфологических характеристик для числительных 
	/** 
		\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\return void
	*/
	void EnCodeNumbers(int iPartOfSpeech, CMorphoFeature *pMorphoFeature);
	/// Выполняет декодирование числа в набор морфологических характеристик для прилагательных
	/** 
		\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\return void
	*/
	void EnCodeAdjectives(int iPartOfSpeech, CMorphoFeature *pMorphoFeature);
	/// Выполняет декодирование числа в набор морфологических характеристик для глаголов
	/** 
		\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\return void
	*/
	void EnCodeVerbs(int iPartOfSpeech, CMorphoFeature *pMorphoFeature);
	/// Выполняет декодирование числа в набор морфологических характеристик для существительных
	/** 
		\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\return void
	*/				
	void EnCodeNouns(int iPartOfSpeech, CMorphoFeature *pMorphoFeature);
	/// Выполняет декодирование числа в набор морфологических характеристик для служебных частей речи
	/** 
		\param iPartOfSpeech - входной параметр, содержащий часть закодированной морфологической информации.
		\param *pMorphoFeature - cодержит набор морфологических характеристик, которые необходимо закодировать.
		\return void
	*/				
	void EnCodeOfficial(int iPartOfSpeech, CMorphoFeature *pMorphoFeature);

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
	/*! \var m_mi_BASE
	*  \brief   содержит базовую часть морфоинформации
	*/
	BASE_MORPHO_INFO				m_mi_BASE;

	/*! \var m_mi_OFFICIAL
	*  \brief     содержит часть морфоинформации для служебных частей речи
	*/
	MORPHO_INFO_OFFICIAL			m_mi_OFFICIAL;
	/*! \var m_mi_NOUNS
	*  \brief     содержит часть морфоинформации для существительных 
	*/
	MORPHO_INFO_NOUNS				m_mi_NOUNS;
	/*! \var m_mi_NUMERALS
	*  \brief     содержит часть морфоинформации для числительных
	*/
	MORPHO_INFO_NUMERALS			m_mi_NUMERALS;

	/*! \var m_mi_ADJECTIVES
	*  \brief     содержит часть морфоинформации для прилагательных
	*/
	MORPHO_INFO_ADJECTIVES		m_mi_ADJECTIVES;

	/*! \var m_mi_PRONOUNS
	*  \brief     содержит часть морфоинформации для местоимений
	*/
	MORPHO_INFO_PRONOUNS			m_mi_PRONOUNS;

	/*! \var m_mi_ADVERB
	*  \brief     содержит часть морфоинформации для наречий
	*/
	MORPHO_INFO_ADVERB				m_mi_ADVERB;
	/*! \var m_mi_VERB
	*  \brief     содержит часть морфоинформации для глаголов
	*/
	MORPHO_INFO_VERB				m_mi_VERB;
	/*! \var m_pos_PRETEXT
	*  \brief     содержит часть морфоинформации для предлогов
	*/
	PART_OF_SPEECH_PRETEXT			m_pos_PRETEXT;
	/*! \var m_pos_NOUNS
	*  \brief     содержит часть морфоинформации для существительных
	*/
	PART_OF_SPEECH_NOUNS			m_pos_NOUNS;
	/*! \var m_pos_NUMERALS
	*  \brief     содержит часть морфоинформации для числительных
	*/
	PART_OF_SPEECH_NUMERALS			m_pos_NUMERALS;
	/*! \var m_pos_ADJECTIVE
	*  \brief     содержит часть морфоинформации для прилагательных
	*/
	PART_OF_SPEECH_ADJECTIVE		m_pos_ADJECTIVE;
	/*! \var m_pos_PRONOUNS
	*  \brief     содержит часть морфоинформации для местоимений
	*/
	PART_OF_SPEECH_PRONOUNS			m_pos_PRONOUNS;
	/*! \var m_pos_ADVERB
	*  \brief     содержит часть морфоинформации для наречий
	*/
	PART_OF_SPEECH_ADVERB_R			m_pos_ADVERB;
	/*! \var m_pos_VERB
	*  \brief     содержит часть морфоинформации для глаголов
	*/
	PART_OF_SPEECH_VERB_R			m_pos_VERB;

};
};
};
};
