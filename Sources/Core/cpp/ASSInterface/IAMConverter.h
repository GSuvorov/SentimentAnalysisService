#pragma once


#include "./TCoSeDiDataTypes.h"
#include "../FeatureLibrary/define.h"

// {3EC3B23C-0D80-4b0a-9FD1-F44A0F487B00}
static const GUID IID_AMConverterSemantic = 
{ 0x3ec3b23c, 0xd80, 0x4b0a, { 0x9f, 0xd1, 0xf4, 0x4a, 0xf, 0x48, 0x7b, 0x0 } };

// {2F51643E-61D5-41d6-90CB-2768DB9EA227}
static const GUID CLSID_AMConverterSemantic = 
{ 0x2f51643e, 0x61d5, 0x41d6, { 0x90, 0xcb, 0x27, 0x68, 0xdb, 0x9e, 0xa2, 0x27 } };


// {6C9D77F0-B88C-4e37-9D28-3EFA4B653448}
static const GUID CLSID_AMConverterSyntax = 
{ 0x6c9d77f0, 0xb88c, 0x4e37, { 0x9d, 0x28, 0x3e, 0xfa, 0x4b, 0x65, 0x34, 0x48 } };

// {1D972E62-2430-47aa-A94E-BDDA6CACC3A5}
static const GUID IID_AMConverterSyntax = 
{ 0x1d972e62, 0x2430, 0x47aa, { 0xa9, 0x4e, 0xbd, 0xda, 0x6c, 0xac, 0xc3, 0xa5 } };

// {ABC73A17-82E9-4acf-A736-6EDC3CCA90A5}
static const GUID IID_AMConverterSyntaxAposteriori = 
{ 0xabc73a17, 0x82e9, 0x4acf, { 0xa7, 0x36, 0x6e, 0xdc, 0x3c, 0xca, 0x90, 0xa5 } };


// {4332A9C7-622C-4e2d-8836-DAFD5B60A6AF}
static const GUID CLSID_AMConverterMorpho = 
{ 0x4332a9c7, 0x622c, 0x4e2d, { 0x88, 0x36, 0xda, 0xfd, 0x5b, 0x60, 0xa6, 0xaf } };
// {BE0933BD-679D-46df-A1F2-A036466D434D}
static const GUID IID_AMConverterMorpho = 
{ 0xbe0933bd, 0x679d, 0x46df, { 0xa1, 0xf2, 0xa0, 0x36, 0x46, 0x6d, 0x43, 0x4d } };



namespace SS
{
namespace Interface
{
namespace Core
{

namespace AMConverter
{

/// содержит набор методов для кодирования/декодирования морфологической информации
	class IAMConverterMorpho : public virtual SS::Interface::IBase
	{
	public:
		IAMConverterMorpho(void){};
		virtual ~IAMConverterMorpho(){};
	public:

		/// Выполняет кодирование набора морфологических характеристик в последовательность чисел
		virtual void Code(
			///номер словаря для которого необходимо выполнить кодирование морфологических характеристик
			const unsigned char uchDictionaryNumber,
			///cодержит набор морфологических характеристик, которые необходимо закодировать.
			const SS::Core::Features::CMorphoFeature* pMorphoFeature,
			///выходной параметр, содержащий часть закодированной морфологической информации.
			int* piMorphoInfo, 
			///выходной параметр, содержащий часть закодированной морфологической информации.
			int* piPartOfSpeech
			) = 0;
		
		/// Выполняет декодирование последовательности чисел в набор морфологических характеристик 
		virtual void EnCode(
			///номер словаря для которого необходимо выполнить расшифровку морфологических характеристик
			const unsigned char uchDictionaryNumber, 
			///входной параметр, содержащий часть закодированной морфологической информации
			const int iMorphoInfo,
			///входной параметр, содержащий часть закодированной морфологической информации
			const int iPartOfSpeech,
			///cодержит набор морфологических характеристик, которые необходимо закодировать
			SS::Core::Features::CMorphoFeature* pMorphoFeature
			) = 0;
	};
	
/// содержит набор методов для кодирования/декодирования синтаксической информации
	class IAMConverterSyntax : public virtual SS::Interface::IBase
	{
	public:
		IAMConverterSyntax(void){};
		virtual ~IAMConverterSyntax(){};
	public:
		//////////////////////////////////////////////////////////////////////////
		/// Кодирует набор априорных синтаксических харктеристик в число
		/** 
			\param oPartOfSpeechTypes - часть речи слова, для которого необходимо выполнить кодирование
			\param *pSyntaxFeature	- набора априорных синтаксических характеристик
			\param piSyntaxInfo	- число, которое содержит закодированные априорные синтаксические характеристики
			\return void
		*/
		virtual void Code(
			/*in*/SS::Core::Features::Types::PartOfSpeechTypes oPartOfSpeechTypes,
			/*in*/SS::Core::Features::CSyntaxFeatureApriori *pSyntaxFeature, 
			/*out*/unsigned int* piSyntaxInfo) = 0;

		/// Выполняет декодирование числа в набор априорных синтаксических характеристик 
		/** 
			\param oPartOfSpeechTypes - часть речи слова, синтаксические характеристики которого необходимо получить
			\param iSyntaxInfo - закодированная синтаксическая характеристика
			\param *pSyntaxFeature - набор синтаксических категорий 
			\return void
		*/
		virtual void EnCode(
		/*in*/ SS::Core::Features::Types::PartOfSpeechTypes oPartOfSpeechTypes,
		/*in*/unsigned int iSyntaxInfo,	
		/*out*/SS::Core::Features::CSyntaxFeatureApriori *pSyntaxFeature) = 0;
	};

	

/// интерфейс, содержит набор методов для кодирования/декодирования синтаксической информации
	class IAMConverterSyntaxAposteriori : public virtual SS::Interface::IBase
	{
	public:
		IAMConverterSyntaxAposteriori(void){};
		virtual ~IAMConverterSyntaxAposteriori(){};
	public:

		/// кодирует набор апосториорных синтаксических харктеристик в число
		/** 
			\param *pSyntaxFeatureAposteriori - набор апосториорных синтаксических характеристик
			\param piSyntaxInfo - число, которое содержит закодированные апосториорные синтаксические характеристики
		*/
		virtual void Code(
			/*in*/SS::Core::Features::CSyntaxFeatureAposteriori *pSyntaxFeatureAposteriori, 
			/*out*/unsigned int* piSyntaxInfo) = 0;

		/// Выполняет декодирование числа в набор априорных синтаксических характеристик 
		/** 
			\param iSyntaxInfo - число, которое содержит закодированные апосториорные синтаксические характеристики
			\param *pSyntaxFeatureAposteriori - набор апосториорных синтаксических характеристик
		*/
		virtual void EnCode(
			/*in*/unsigned int iSyntaxInfo,	
			/*out*/SS::Core::Features::CSyntaxFeatureAposteriori *pSyntaxFeatureAposteriori) = 0;
	};

				

/// интерфейс, который содержит набор методов для кодирования/декодирования семантической информации 
	class IAMConverterSemantic : public virtual SS::Interface::IBase
	{
	public:
		IAMConverterSemantic(void){};
		virtual ~IAMConverterSemantic(){};
	public:

		/// кодирует набор семантических характеристик в число
		/** 
			\param *pSemanticFeature - набор семантических характеристик
			\param pSemanticType - число, которое содержит закодированные семантические характеристики
			\return void
		*/
		virtual void Code(/*in*/SS::Core::Features::CSemanticFeature *pSemanticFeature, /*out*/SS::Dictionary::Types::TDictionaryIndex* pDictionaryIndex) = 0;

		/// декодирует число в набор семантических характеристик
		/** 
			\param SemanticType - число, которое содержит закодированные семантические характеристики
			\param *pSemanticFeature - набор семантических характеристик
			\return void
		*/
		virtual void EnCode(/*in*/SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex, /*out*/SS::Core::Features::CSemanticFeature *pSemanticFeature) = 0;
	};
}
}
}
}
