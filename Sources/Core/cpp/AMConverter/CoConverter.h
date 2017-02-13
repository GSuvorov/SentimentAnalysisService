#pragma once

#include "..\ASSInterface\TDictionaryNames.h"


#include "..\ASSInterface\IAMConverter.h"

#include ".\syntaxanalyzer.h"

#include ".\AMRussianAnalyzer.h"
#include ".\AMEnglishAnalyzer.h"

#include ".\SemanticConverter.h"
#include ".\syntaxAposterioriAnalyzer.h"


namespace SS
{
	namespace Core
	{
		namespace Features
		{
			/////////////////////////////////////////////////////////////////////////////
			// CCoConverter
			class CCoConverter 
				: public SS::Interface::Core::AMConverter::IAMConverterMorpho
				, public SS::Interface::Core::AMConverter::IAMConverterSyntax
				, public SS::Interface::Core::AMConverter::IAMConverterSyntaxAposteriori
				, public SS::Interface::Core::AMConverter::IAMConverterSemantic
			{
			public:
				/// конструктор
				CCoConverter()
				{		
				}
				/// деструктор
				virtual ~CCoConverter()
				{
				}
			public: //from SS::Interface::IBase	
				HRESULT QueryInterface(REFIID refIID, void** pBase);
				ULONG Release();
			public: //from SS::Core::SMISL::IAMConverterMorpho	
				/// Выполняет кодирование набора морфологических характеристик в последовательность чисел
				void Code(
					///номер словаря для которого необходимо выполнить кодирование морфологических характеристик
					const unsigned char uchDictionaryNumber,
					///cодержит набор морфологических характеристик, которые необходимо закодировать.
					const CMorphoFeature* pMorphoFeature,
					///выходной параметр, содержащий часть закодированной морфологической информации.
					int* piMorphoInfo, 
					///выходной параметр, содержащий часть закодированной морфологической информации.
					int* piPartOfSpeech
					);
				
				/// Выполняет декодирование последовательности чисел в набор морфологических характеристик 
				void EnCode(
					///номер словаря для которого необходимо выполнить расшифровку морфологических характеристик
					const unsigned char uchDictionaryNumber, 
					///входной параметр, содержащий часть закодированной морфологической информации
					const int iMorphoInfo,
					///входной параметр, содержащий часть закодированной морфологической информации
					const int iPartOfSpeech,
					///cодержит набор морфологических характеристик, которые необходимо закодировать
					CMorphoFeature* pMorphoFeature
					);

				/// Кодирует набор априорных синтаксических харктеристик в число
				/** 
					\param oPartOfSpeechTypes - часть речи слова, для которого необходимо выполнить кодирование
					\param *pSyntaxFeature	- набора априорных синтаксических характеристик
					\param piSyntaxInfo	- число, которое содержит закодированные априорные синтаксические характеристики
					\return void
				*/
				void Code(
					/*in*/SS::Core::Features::Types::PartOfSpeechTypes oPartOfSpeechTypes,
					/*in*/SS::Core::Features::CSyntaxFeatureApriori *pSyntaxFeature, 
					/*out*/unsigned int* piSyntaxInfo);

				/// Выполняет декодирование числа в набор априорных синтаксических характеристик 
				/** 
					\param oPartOfSpeechTypes - часть речи слова, синтаксические характеристики которого необходимо получить
					\param iSyntaxInfo - закодированная синтаксическая характеристика
					\param *pSyntaxFeature - набор синтаксических категорий 
					\return void
				*/
				void EnCode(
				/*in*/SS::Core::Features::Types::PartOfSpeechTypes oPartOfSpeechTypes,
				/*in*/unsigned int iSyntaxInfo,	
				/*out*/SS::Core::Features::CSyntaxFeatureApriori *pSyntaxFeature);

			public: //from SS::Core::SMISL::IAMConverterSemantic	
				/// кодирует набор семантических характеристик в число
				/** 
					\param *pSemanticFeature - набор семантических характеристик
					\param pSemanticType - число, которое содержит закодированные семантические характеристики
					\return void
				*/
				void Code(/*in*/SS::Core::Features::CSemanticFeature *pSemanticFeature, /*out*/SS::Dictionary::Types::TDictionaryIndex* pDictionaryIndex);

				/// декодирует число в набор семантических характеристик
				/** 
					\param SemanticType - число, которое содержит закодированные семантические характеристики
					\param *pSemanticFeature - набор семантических характеристик
					\return void
				*/
				void EnCode(/*in*/SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex, /*out*/SS::Core::Features::CSemanticFeature *pSemanticFeature);

			public: //from SS::Core::SMISL::IAMConverterSyntaxAposteriori
				/// Кодирует набор априорных синтаксических харктеристик в число
				/** 
					\param *pSyntaxFeature	- набора априорных синтаксических характеристик
					\param piSyntaxInfo	- число, которое содержит закодированные априорные синтаксические характеристики
				*/
				void Code(
					/*in*/SS::Core::Features::CSyntaxFeatureAposteriori *pSyntaxFeatureAposteriori, 
					/*out*/unsigned int* piSyntaxInfo) ;
					/*	декодирование апосториорной синтаксической информации	*/
				/// Выполняет декодирование числа в набор априорных синтаксических характеристик 
				/** 
					\param iSyntaxInfo - закодированная синтаксическая характеристика
					\param *pSyntaxFeature - набор синтаксических категорий 
				*/
				void EnCode(
					/*in*/unsigned int iSyntaxInfo,	
					/*out*/SS::Core::Features::CSyntaxFeatureAposteriori *pSyntaxFeatureAposteriori);
			private:
//RRR				CMorphoAnalyzerRus		m_MorphoAnalyzerRus;
				CAMEnglishAnalyzer		m_oAMEnglishAnalyzer;

				CSyntaxAnalyzer			m_oSyntaxAnalyzer;

				CAMRussianAnalyzer		m_oAMRussianAnalyzer;
			};
		}
	}
}