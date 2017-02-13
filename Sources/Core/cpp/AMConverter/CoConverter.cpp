// CoConvertor.cpp : Implementation of CCoConvertor

#include "stdafx.h"



#include "CoConverter.h"

namespace SS
{
namespace Core
{
namespace Features
{

void AddAllCaseType( CMorphoFeature &MF )
{  
   MF.m_CaseType.AssignOR(&Values::CaseType.ctNominative);
   MF.m_CaseType.AssignOR(&Values::CaseType.ctGenitive);
   MF.m_CaseType.AssignOR(&Values::CaseType.ctDative);
   MF.m_CaseType.AssignOR(&Values::CaseType.ctAccusative);
   MF.m_CaseType.AssignOR(&Values::CaseType.ctInstrumental);
   MF.m_CaseType.AssignOR(&Values::CaseType.ctLocative);
   MF.m_CaseType.AssignOR(&Values::CaseType.ctCommon);
}

void AddAllNumberType( CMorphoFeature &MF )
{  
   MF.m_NumberType.AssignOR(&Values::NumberType.ntSingle);
   MF.m_NumberType.AssignOR(&Values::NumberType.ntPlural);
}

/// Выполняет кодирование набора морфологических характеристик в последовательность чисел
void CCoConverter::Code(
	///номер словаря для которого необходимо выполнить кодирование морфологических характеристик
	const unsigned char uchDictionaryNumber,
	///cодержит набор морфологических характеристик, которые необходимо закодировать.
	const CMorphoFeature *pMorphoFeature,
	///выходной параметр, содержащий часть закодированной морфологической информации.
	int* piMorphoInfo, 
	///выходной параметр, содержащий часть закодированной морфологической информации.
	int* piPartOfSpeech
	)
{
	SS_TRY;

		switch(uchDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName :
		case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus :
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus :
		case SS::Dictionary::DATA_TYPE::NAMES::ednOrderingRussian:
			m_oAMRussianAnalyzer.Code(pMorphoFeature, piMorphoInfo, piPartOfSpeech);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName :
			m_oAMRussianAnalyzer.Code(pMorphoFeature, piMorphoInfo, piPartOfSpeech);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednSymbols:
		case SS::Dictionary::DATA_TYPE::NAMES::ednNumeric:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName :
		case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng :
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng :
		case SS::Dictionary::DATA_TYPE::NAMES::ednRomanNumeric:
      case SS::Dictionary::DATA_TYPE::NAMES::ednFloatPointNumeric:
      case SS::Dictionary::DATA_TYPE::NAMES::ednOrderingEnglish:
			m_oAMEnglishAnalyzer.Code(pMorphoFeature, piMorphoInfo, piPartOfSpeech);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName :
			m_oAMEnglishAnalyzer.Code(pMorphoFeature, piMorphoInfo, piPartOfSpeech);
			break;
		}
	
	SS_CATCH(L"");
}

/// Выполняет декодирование последовательности чисел в набор морфологических характеристик 
void CCoConverter::EnCode(
	///номер словаря для которого необходимо выполнить расшифровку морфологических характеристик
	const unsigned char uchDictionaryNumber, 
	///входной параметр, содержащий часть закодированной морфологической информации
	const int iMorphoInfo,
	///входной параметр, содержащий часть закодированной морфологической информации
	const int iPartOfSpeech,
	///cодержит набор морфологических характеристик, которые необходимо закодировать
	CMorphoFeature *pMorphoFeature
	)
{
	SS_TRY
	{
		switch(uchDictionaryNumber)
		{
		case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName :		
		case SS::Dictionary::DATA_TYPE::NAMES::ednOrderingRussian:
			m_oAMRussianAnalyzer.EnCode(iMorphoInfo, iPartOfSpeech, pMorphoFeature);
			pMorphoFeature->m_CommonType.Assign(Values::CommonType.cmtCommon);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus :
			//SS::Dictionary::Types::TDictionaryIndex oDictInd = (SS::Dictionary::Types::TDictionaryIndex)iMorphoInfo;
			//oDictIndex
			m_oAMRussianAnalyzer.EnCode(iMorphoInfo, iPartOfSpeech, pMorphoFeature);
			/*if ((iMorphoInfo >> 31)==1) 
			  pMorphoFeature->m_CommonType.Assign(Values::CommonType.cmtProper);
			else*/
			pMorphoFeature->m_CommonType.Assign(Values::CommonType.cmtCommon);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus :
			m_oAMRussianAnalyzer.EnCode(iMorphoInfo, iPartOfSpeech, pMorphoFeature);
			pMorphoFeature->m_CommonType.Assign(Values::CommonType.cmtCommon);
			AddAllCaseType(*pMorphoFeature);
			AddAllNumberType(*pMorphoFeature);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName :
			m_oAMRussianAnalyzer.EnCode(iMorphoInfo, iPartOfSpeech, pMorphoFeature);
			pMorphoFeature->m_CommonType.Assign(Values::CommonType.cmtProper);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednSymbols:
		case SS::Dictionary::DATA_TYPE::NAMES::ednNumeric:
		case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName :
		case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng :
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng :
		case SS::Dictionary::DATA_TYPE::NAMES::ednRomanNumeric :
		case SS::Dictionary::DATA_TYPE::NAMES::ednFloatPointNumeric:
		case SS::Dictionary::DATA_TYPE::NAMES::ednOrderingEnglish:
			m_oAMEnglishAnalyzer.EnCode(iMorphoInfo, iPartOfSpeech, pMorphoFeature);
			if (uchDictionaryNumber==SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory) 
				pMorphoFeature->m_CommonType.Assign(Values::CommonType.cmtProper);
			else
			  pMorphoFeature->m_CommonType.Assign(Values::CommonType.cmtCommon);
			break;
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName :
			m_oAMEnglishAnalyzer.EnCode(iMorphoInfo, iPartOfSpeech, pMorphoFeature);
			pMorphoFeature->m_CommonType.Assign(Values::CommonType.cmtProper);
			break;
		}
	}
	SS_CATCH(L"")
}


void CCoConverter::Code(/*in*/SS::Core::Features::Types::PartOfSpeechTypes oPartOfSpeechTypes,
	/*in*/SS::Core::Features::CSyntaxFeatureApriori *pCombinationFeature, /*out*/unsigned int* piSyntaxInfo)
{
	SS_TRY
	{

		m_oSyntaxAnalyzer.Code(oPartOfSpeechTypes, pCombinationFeature, piSyntaxInfo);
	}
	SS_CATCH(L"")
}

void CCoConverter::EnCode(/*in*/SS::Core::Features::Types::PartOfSpeechTypes oPartOfSpeechTypes,
	/*in*/unsigned int iSyntaxInfo,	/*out*/SS::Core::Features::CSyntaxFeatureApriori *pCombinationFeature)
{
	SS_TRY
	{

		m_oSyntaxAnalyzer.EnCode(oPartOfSpeechTypes, iSyntaxInfo, pCombinationFeature);
	}
	SS_CATCH(L"")
}

void CCoConverter::Code(/*in*/SS::Core::Features::CSemanticFeature *pSemanticFeature, /*out*/SS::Dictionary::Types::TDictionaryIndex* pDictionaryIndex)
{
	SS_TRY
	{

		CSemanticConverter oSemanticConverter;
		oSemanticConverter.Code(pSemanticFeature, pDictionaryIndex);
	}
	SS_CATCH(L"")
}

void CCoConverter::EnCode(/*in*/SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex, /*out*/SS::Core::Features::CSemanticFeature *pSemanticFeature)
{
	SS_TRY
	{
	
		CSemanticConverter oSemanticConverter;
		oSemanticConverter.EnCode(oDictionaryIndex, pSemanticFeature);
	}
	SS_CATCH(L"")
}


void CCoConverter::Code(
	/*in*/SS::Core::Features::CSyntaxFeatureAposteriori *pSyntaxFeatureAposteriori, 
	/*out*/unsigned int* piSyntaxInfo)
{
	SS_TRY
	{
		CSyntaxAposterioriAnalyzer oSyntaxAposterioriAnalyzer;
		oSyntaxAposterioriAnalyzer.Code(pSyntaxFeatureAposteriori, piSyntaxInfo);
	}
	SS_CATCH(L"")
}

	/*	декодирование апосториорной синтаксической информации	*/
void  CCoConverter::EnCode(
	/*in*/unsigned int iSyntaxInfo,	
	/*out*/SS::Core::Features::CSyntaxFeatureAposteriori *pSyntaxFeatureAposteriori) 
{
	SS_TRY
	{
		CSyntaxAposterioriAnalyzer oSyntaxAposterioriAnalyzer;
		oSyntaxAposterioriAnalyzer.EnCode(iSyntaxInfo, pSyntaxFeatureAposteriori);
	}
	SS_CATCH(L"")
}



/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CCoConverter::QueryInterface(REFIID refIID, void** pBase)
{
	if(refIID == IID_AMConverterMorpho)
		*pBase = (SS::Interface::Core::AMConverter::IAMConverterMorpho*)this;
	else
		if(refIID == IID_AMConverterSyntax)
		*pBase = (SS::Interface::Core::AMConverter::IAMConverterSyntax*)this;
	else
		if(refIID == IID_AMConverterSemantic)
			*pBase = (SS::Interface::Core::AMConverter::IAMConverterSemantic*)this;
	else
		if(refIID == IID_AMConverterSyntaxAposteriori)
			*pBase = (SS::Interface::Core::AMConverter::IAMConverterSyntaxAposteriori*)this;
	else
		if (refIID==IID_Base )
			*pBase = (SS::Interface::IBase*)this;
	else
		*pBase = NULL;

	return S_OK;
}

ULONG CCoConverter::Release()
{
	delete this;
	return 0;
}
}
}
}