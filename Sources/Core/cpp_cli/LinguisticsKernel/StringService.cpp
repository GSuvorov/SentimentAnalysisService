#include "StdAfx.h"
#include ".\stringservice.h"
#using <mscorlib.dll>

#include <string>
#include <list>

#include "..\..\cpp\FeatureLibrary\define.h"
#include "..\..\cpp\ASSInterface\IAMConverter.h"
#include "..\..\cpp\ASSInterface\TDictionaryManager.h"

namespace LinguisticsKernel
{
	extern SS::Interface::Core::AMConverter::IAMConverterSyntaxAposteriori*	m_pAMConverterSyntaxAposteriori;
	extern SS::Interface::Core::AMConverter::IAMConverterSemantic*			m_pAMConverterSemantic;

	CStringService::CStringService(void)
	{
	}

	CStringService::~CStringService(void)
	{
	}

	///выдаст стринговое описание синтаксического индекса
	System::String^ CStringService::SynonymIndexToString(System::UInt32 index)
	{
		System::String^ s;

		unsigned char uc = *((unsigned char*)(&index) + 3);
		s = System::String::Concat("[", uc.ToString(), "] " ,(index & 0x00ffffff).ToString());

		return s;
	}

	System::String^ CStringService::LanguageIndexToString(System::UInt32 index)
	{
		SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex;
		oDictionaryIndex.SetDictionaryIndex(index);

		if (oDictionaryIndex.GetLanguage()==SS::Core::Types::ealEnglish) return "en"; else
			return "ru";
	}

	System::String^ CStringService::DictionaryIndexToString1(System::UInt32 index)
	{
		SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex;
		oDictionaryIndex.SetDictionaryIndex(index);

		System::String^ s;
		switch(oDictionaryIndex.GetDictionaryIndexWithoutFlags())
		{
			case	SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
				s = "Nominal";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
				s = "Proper";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednNumeric:
				s = "Nominal";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednUnknown:
				s = "Unknown";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSymbols:
				s = "Nominal";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
				s = "Nominal";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
				s = "Proper";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
				s = "Abbr";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
				s = "Abbr";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednDates:
				s = "Nominal";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSyntax:
				s = "Nominal";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSemantic:
				s = "Nominal";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
				s = "Combi";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
				s = "Combi";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus:
				s = "Unknown";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng:
				s = "Unknown";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
				s = "Nominal";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
				s = "Nominal";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednRomanNumeric:
				s = "Nominal";
				break;
			default:
				s = "Unknown";
				break;
		}
		return s;
	}

	///выдаст стринговое описание номера словаря
	System::String^ CStringService::DictionaryIndexToString(System::UInt32 index)
	{
		SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex;
		oDictionaryIndex.SetDictionaryIndex(index);

		System::String^ s;
		switch(oDictionaryIndex.GetDictionaryIndexWithoutFlags())
		{
			case	SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
				s = "[Rus] нарицат";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
				s = "[Rus] Собств";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednNumeric:
				s = "[all] 12345";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednUnknown:
				s = "[all] х.з.";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSymbols:
				s = "[all] символы";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
				s = "[Eng] nominal";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
				s = "[Eng] proper";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
				s = "[Rus] АББРВАЛ";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
				s = "[Eng] АББРВАЛ";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednDates:
				s = "[all] Даты";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSyntax:
				s = "[all] Синтакс";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSemantic:
				s = "[all] Семантика";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
				s = "[Rus] ТКС";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
				s = "[Eng] ТКС";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus:
				s = "[Rus] х.з.";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng:
				s = "[Eng] х.з.";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
				s = "[Rus] синоним";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
				s = "[Eng] синоним";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednRomanNumeric:
				s = "[all] XXIIV";
				break;
			default:
				s = "[all] х.з. в квадрате";
				break;
		}
		return s;
	}
	///выдаст краткое стринговое описание номера словаря
	System::String^ CStringService::DictionaryIndexToShortString(System::UInt32 index)
	{
		System::String^ s;
		switch(index)
		{
			case	SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
				s = "RNom";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
				s = "RProp";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednUnknown:
				s = "~хз";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
				s = "ENom";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
				s = "EProp";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
				s = "RАББР";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
				s = "EАББР";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednDates:
				s = "RДат";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
				s = "RТКС";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
				s = "EТКС";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus:
				s = "Rхз";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng:
				s = "Eхз";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus:
				s = "RSy";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng:
				s = "ESy";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSymbols:
				s = "~симв";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSyntax:
				s = "~Sx";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednSemantic:
				s = "~Sm";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednNumeric:
				s = "~123";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednRomanNumeric:
				s = "~XIV";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednFloatPointNumeric:
				s = "~0.00x";
				break;
			case 	SS::Dictionary::DATA_TYPE::NAMES::ednOrderingEnglish:
				s = "~1st";
				break;
			case	SS::Dictionary::DATA_TYPE::NAMES::ednOrderingRussian:
				s = "~1-й";
				break;

			default:
				s = "~хз 2й степени";
				break;
		}
		return s;
	}

	///выдаст стринговое описание морфологического индекса
	System::String^ CStringService::MorphoIndexToString(System::UInt32 index)
	{
		SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex;
		oDictionaryIndex.SetDictionaryIndex(index);

		unsigned char uc = (unsigned char)oDictionaryIndex.GetDictionaryNumber()/**((unsigned char*)(&index) + 3)*/;		

		System::String^ s;

		try{
			s = DictionaryIndexToString(uc);
			s = s->Concat("<", s, "> ", (oDictionaryIndex.GetId()).ToString());
			s = s->Concat(s, " {", (oDictionaryIndex.GetDictionaryIndexWithoutFlags()).ToString(), "}");
		}
		catch(...){
			s = System::String::Concat("[", uc.ToString(), "] " , DictionaryIndexToString(index));
		}

		return s;
	}
	///выдаст краткое стринговое описание морфологического индекса
	System::String^ CStringService::MorphoIndexToShortString(System::UInt32 index)
	{
		unsigned char uc = *((unsigned char*)(&index) + 3);
		System::String^ s;
		try{
			s = DictionaryIndexToShortString(uc);
			s = s->Concat("<", s, "> ", (index & 0x00ffffff).ToString());
			s = s->Concat(s, " {", index.ToString(), "}");
		}
		catch(...){
			s = System::String::Concat("[", uc.ToString(), "] " ,(index & 0x00ffffff).ToString());
		}
		return s;
	}

	///выдаст стринговое описание семантического индекса
	System::String^ CStringService::SemanticIndexToString(System::UInt32 index)
	{
		System::String^ s;

		try
		{
			SS::Core::Features::CSemanticFeature oSemanticFeature;
			if(!m_pAMConverterSemantic)
			{
				throw (gcnew System::Exception(gcnew System::String(L"!m_pAMConverterSemantic")));
			}

			SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex(index);

			m_pAMConverterSemantic->EnCode(index, &oSemanticFeature);
			std::string sIndexInfo; 
			if(oDictionaryIndex.GetGenericMarker())
			{
				sIndexInfo = "<Generic>";
			}else
			{
				sIndexInfo = "<NotGeneric>";
			}
			sIndexInfo +=  oSemanticFeature.GetPrintableDataDescription().c_str();

			s = gcnew System::String( sIndexInfo.c_str() );
			
			
		}
		catch(...)
		{
			unsigned char uc = *((unsigned char*)(&index) + 3);
			s = System::String::Concat("[", uc.ToString(), "] " ,(index & 0x00ffffff).ToString());
		}
		return s;
	}

	///выдаст стринговое описание семантического индекса
	System::String^ CStringService::SyntaxIndexToString(System::UInt32 index)
	{
		System::String^ s;

		try{
			SS::Core::Features::Types::SyntaxCategories oSyntaxCategories;

			oSyntaxCategories.SetSingleValue(index & 0x000000ff);
			std::string ss = oSyntaxCategories.ToString();
			s = gcnew System::String(ss.c_str());
		}
		catch(...)
		{
			unsigned char uc = *((unsigned char*)(&index) + 3);
			s = System::String::Concat("[", uc.ToString(), "] " ,(index & 0x00ffffff).ToString());
		}

		return s;
	}

	///выдаст стринговое описание типа индекса
	System::String^ CStringService::IndexTypeToString(SS::Interface::Core::BlackBox::EIndexType indexType)
	{
		System::String^ s;
		switch(indexType){
			///неопределенный индекс
			case SS::Interface::Core::BlackBox::eitUndefinedIndex: 
				s = gcnew System::String("Undefined");
				break;
			///морфологический индекс
			case SS::Interface::Core::BlackBox::eitMorphoIndex:
				s = gcnew System::String("Morpho");
				break;
			///синтаксический индекс
			case SS::Interface::Core::BlackBox::eitSyntaxIndex:
				s = gcnew System::String("Syntax");
				break;
			///семантический индекс
			case SS::Interface::Core::BlackBox::eitSemanticIndex:
				s = gcnew System::String("Semantic");
				break;
			///индекс синонима
			case SS::Interface::Core::BlackBox::eitSynonymIndex:
				s = gcnew System::String("Synonym");
				break;
			default:
				s = gcnew System::String("???");
				break;
		}

		return s;
	}
	///выдаст стринговое описание типа трансформации
	System::String^ CStringService::TransformationTypeToString(SS::Interface::Core::BlackBox::Query::ETransformationType transformationType)
	{
		System::String^ s;

		switch(transformationType){
			///неопределен
			case SS::Interface::Core::BlackBox::Query::ettUndefined:
				s = gcnew System::String("неопределенная");
				break;
			///простая
			case SS::Interface::Core::BlackBox::Query::ettSimple:
				s = gcnew System::String("простая");
				break;
			///транзитивная
			case SS::Interface::Core::BlackBox::Query::ettTransitive:
				s = gcnew System::String("транзитивная");
				break;
			///нетранзитивная
			case SS::Interface::Core::BlackBox::Query::ettIntransitive:
				s = gcnew System::String("нетранзитивная");
				break;
			///определительная
			case SS::Interface::Core::BlackBox::Query::ettDefinitive:
				s = gcnew System::String("определительная");
				break;
			default:
				s = gcnew System::String("???");
		}

		return s;
	}

	//System::String^ GetMorphoString(SS::Core::Features::CMorphoFeature& oMorphoFeature)
	//{
	//	System::Text::StringBuilder *sb = new System::Text::StringBuilder();

	//	if (oMorphoFeature.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postNoun))
	//		sb->Append(L"Noun,");
	//	else
	//	if (oMorphoFeature.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postAdverb))
	//		sb->Append(L"Adverb,");
	//	else 
	//	if (oMorphoFeature.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postAdjective))
	//		sb->Append(L"Adjective,");
	//	else 
	//	if (oMorphoFeature.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postNumeric))
	//		sb->Append(L"Numeric,");
	//	else
	//	if (oMorphoFeature.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postOfficial))
	//		sb->Append(L"Official,");
	//	else
	//	if (oMorphoFeature.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postPronoun))
	//		sb->Append(L"Pronoun,");
	//	else
	//	if (oMorphoFeature.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postVerb))
	//		sb->Append(L"Verb,");
	//
	//	
	//	if (oMorphoFeature.m_GenderType.IsIntersectsWith(&SS::Core::Features::Values::GenderType.gtFemale))
	//		sb->Append(L"Female,");
	//	else
	//	if (oMorphoFeature.m_GenderType.IsIntersectsWith(&SS::Core::Features::Values::GenderType.gtMasculine))
	//		sb->Append(L"Masculine,");
	//	else
	//	if (oMorphoFeature.m_GenderType.IsIntersectsWith(&SS::Core::Features::Values::GenderType.gtNeutral))
	//		sb->Append(L"Neutral,");

	//	if (oMorphoFeature.m_NumberType.IsIntersectsWith(&SS::Core::Features::Values::NumberType.ntPlural))
	//		sb->Append(L"Plural,");
	//	else
	//	if (oMorphoFeature.m_NumberType.IsIntersectsWith(&SS::Core::Features::Values::NumberType.ntSingle))
	//		sb->Append(L"Single,");
	//	

	//	if (oMorphoFeature.m_CaseType.IsIntersectsWith(&SS::Core::Features::Values::CaseType.ctAccusative))
	//		sb->Append(L"Accusative,");
	//	else
	//	if (oMorphoFeature.m_CaseType.IsIntersectsWith(&SS::Core::Features::Values::CaseType.ctDative))
	//		sb->Append(L"Dative,");
	//	else
	//	if (oMorphoFeature.m_CaseType.IsIntersectsWith(&SS::Core::Features::Values::CaseType.ctGenitive))
	//		sb->Append(L"Genitive,");
	//	else
	//	if (oMorphoFeature.m_CaseType.IsIntersectsWith(&SS::Core::Features::Values::CaseType.ctInstrumental))
	//		sb->Append(L"Instrumental,");
	//	else
	//	if (oMorphoFeature.m_CaseType.IsIntersectsWith(&SS::Core::Features::Values::CaseType.ctLocative))
	//		sb->Append(L"Locative,");
	//	else
	//	if (oMorphoFeature.m_CaseType.IsIntersectsWith(&SS::Core::Features::Values::CaseType.ctNominative))
	//		sb->Append(L"Nominative,");


	//	if (oMorphoFeature.m_AnimInAnimType.IsIntersectsWith(&SS::Core::Features::Values::AnimInAnimType.aiaAnimation))
	//		sb->Append(L"Anim,");
	//	else
	//	if (oMorphoFeature.m_AnimInAnimType.IsIntersectsWith(&SS::Core::Features::Values::AnimInAnimType.aiaInanimation))
	//		sb->Append(L"InAnim,");


	//	/*if (oMorphoFeature.m_s.IsIntersectsWith(&SS::Core::Features::Values::TSemanticType.smtPerson))
	//		sb->Append(L"Person,");
	//	else
	//	if (oMorphoFeature.m_AnimInAnimType.IsIntersectsWith(&SS::Core::Features::Values::TSemanticType.smtPerson))
	//		sb->Append(L"Person,");*/






	//	
	//


	//	




	//	return sb->ToString();

	//}

}