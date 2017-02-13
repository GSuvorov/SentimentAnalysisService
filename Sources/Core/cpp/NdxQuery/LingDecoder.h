#pragma once
namespace LingDecoder
{

enum EIndexType
{
	///	морфологический индекс
	eitMorphoIndex,
	///	синтаксический индекс
	eitSyntaxIndex,
	///	семантический индекс
	eitSemanticIndex
};


static EIndexType get_index_type(unsigned int _index)
{
	enum DictionariesNumber
	{
		ednRNominalName			= 0,
		ednRProperName			= 1,
		ednNumeric				= 2,
		ednUnknown				= 3,
		ednSymbols				= 4,
		ednENominalName			= 5,
		ednEProperName			= 6,
		ednAbbreviationsRus		= 7,
		ednAbbreviationsEng		= 8,
		ednDates				= 9,
		ednSyntax				= 10,
		ednSemantic				= 11,
		ednRCombinatory			= 12,
		ednECombinatory			= 13,
		ednUnknownRus			= 14,
		ednUnknownEng			= 15,
		ednSynonimsRus			= 16,
		ednSynonimsEng			= 17,
		ednRomanNumeric			= 18,
		ednFloatPointNumeric	= 19,
		ednNumericOrdering		= 20
	};

	union
	{
		unsigned int m_DictionaryIndex; // Union member N 1 
		struct
		{                      // Union member N 2
			unsigned int 
				m_Id: 24,
				m_DictionaryNumber: 7,
				m_FirstLetterUpperCase: 1;
		};

	} splitter;

	splitter.m_DictionaryIndex = _index;

	switch(splitter.m_DictionaryNumber)
	{
	case ednRNominalName: 
	case ednRProperName: 
	case ednENominalName: 
	case ednEProperName: 
	case ednAbbreviationsRus: 
	case ednAbbreviationsEng: 
	case ednNumeric: 
	case ednFloatPointNumeric:
	case ednRomanNumeric:
	case ednUnknown: 
	case ednUnknownEng: 
	case ednUnknownRus: 
	case ednSymbols: 
	case ednNumericOrdering:
		return eitMorphoIndex;
	case ednSyntax: 
		return eitSyntaxIndex;
	case ednDates: 
	case ednSemantic: 
		return eitSemanticIndex;
	case ednSynonimsRus: 
	case ednSynonimsEng: 
		//return eitSynonymIndex;
		return eitMorphoIndex;
	case ednECombinatory: 
	case ednRCombinatory: 
		//return eitCombinatoryIndex;
		return eitMorphoIndex;
	default:
		//return eitUndefinedIndex;
		return eitMorphoIndex;
	}
}
}