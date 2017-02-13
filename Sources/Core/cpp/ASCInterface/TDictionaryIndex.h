#pragma once

#include <vector>
#include <cstring>
namespace SS
{

namespace Dictionary
{
	namespace DATA_TYPE
	{
		namespace NAMES
		{	
			enum DictionariesNumber
			{
				ednRNominalName	= 0,
				ednRProperName	= 1,
				ednNumeric		= 2,
				ednUnknown		= 3,
				ednSymbols		= 4,
				ednENominalName	= 5,
				ednEProperName	= 6,
				ednAbbreviationsRus = 7,
				ednAbbreviationsEng = 8,
				ednDates = 9,
				ednSyntax = 10,
				ednSemantic = 11,
				ednRCombinatory = 12,
				ednECombinatory = 13,
				ednUnknownRus	= 14,
				ednUnknownEng	= 15,
				ednSynonimsRus	= 16,
				ednSynonimsEng	= 17,
				ednRomanNumeric	= 18,
				ednFloatPointNumeric = 19,
				ednOrderingEnglish = 20,
				ednRSemanticClass = 21,
				ednESemanticClass = 22,
				ednOrderingRussian = 23,
				ednExtraRCombinatory = 24,
				ednExtraECombinatory = 25,
			};
		}	
	}
}

namespace Constants
{
	//часть DictionaryIndex-а, которую надо сравнивать в релевантности
	const unsigned int SIGNIFICANT_INDEX_PART = 0x7FFFFFFF;
}
}

namespace SS
{
namespace Core
{
namespace Types
{
	///€зык анализа
	enum EAnalyseLanguage
	{
		///неопределенный
		ealUndefined,
		///русский
		ealRussian,
		///английский
		ealEnglish
	};
}
}
}

namespace SS
{
namespace Interface
{
namespace Core
{
namespace BlackBox
{
	///тип индекса
	enum EIndexType
	{
		///неопределенный индекс
		eitUndefinedIndex, 
		///морфологический индекс
		eitMorphoIndex, 
		///синтаксический индекс
		eitSyntaxIndex, 
		///семантический индекс
		eitSemanticIndex, 
		///индекс синонима
		eitSynonymIndex,
		///комбинаторный индекс
		eitCombinatoryIndex 
	};
}
}
}

namespace Dictionary
{
namespace Types
{

/// индекс типа морфообразовани€
typedef unsigned short	TIDType;
/// закодированна€ морфологическа€ информаци€
typedef unsigned short	TMorphoInfo;
/// закодированна€ дополнительна€ морфологическа€ информаци€
typedef unsigned int	TSpecialMorpho;
/// индекс словоформы
typedef unsigned short	TIDForm;
/// индекс окончани€
typedef unsigned short	TIDEnding;
/// номер словар€
typedef SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber	TDictNumber;
///—писок ћорфоинформации
typedef std::vector<TIDType> TTypesList;

/// индеск основы (DictionaryIndex), включающий ID и DictionaryNumber
class TDictionaryIndex
{
protected:
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
	};

public:
	TDictionaryIndex(unsigned int id = 0) : 
			m_DictionaryIndex(id)
	{
	}

	TDictionaryIndex(unsigned int idSource, TDictNumber edn) :
			m_Id(idSource),
			m_DictionaryNumber(edn),
			m_FirstLetterUpperCase(0)
	{
	}

	unsigned int GetDictionaryIndex() const
	{
		return m_DictionaryIndex; 
	}

	void SetDictionaryIndex(unsigned int uiDictionaryIndex)
	{
		m_DictionaryIndex = uiDictionaryIndex;
	}

	unsigned int GetId() const
	{
		return m_Id;
	}

	void  SetId(unsigned int uiId)
	{
		m_Id = uiId;
	}

	TDictNumber GetDictionaryNumber() const
	{
		return static_cast<SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber>(m_DictionaryNumber);
	}

	void SetDictionaryNumber(TDictNumber eDictionaryNumber)
	{
		m_DictionaryNumber = eDictionaryNumber;
	}

	bool GetFirstLetterUpperCase() const
	{
		return m_FirstLetterUpperCase;
	}

	void SetFirstLetterUpperCase(bool Val)
	{
		m_FirstLetterUpperCase = Val;
	}

   bool GetGenericMarker() const { return GetFirstLetterUpperCase(); }
   void SetGenericMarker( bool Val ) { SetFirstLetterUpperCase(Val); }

	unsigned int GetDictionaryIndexWithoutFlags() const
	{
		return m_DictionaryIndex & Constants::SIGNIFICANT_INDEX_PART;
	}

	void SetDictionaryIndexWithoutFlags(unsigned int Val)
	{
		m_DictionaryIndex = Val & Constants::SIGNIFICANT_INDEX_PART;
	}
	//ћаксимальный Id который можно сохранить
	static unsigned int MaxId()
	{
		return /*2^24 - 1 = */ 16777215;
	}

	bool operator==(const TDictionaryIndex& oInd) const
	{
		return oInd.m_DictionaryIndex == m_DictionaryIndex;
	}

	bool operator!=(const TDictionaryIndex& oInd) const
	{
		return oInd.m_DictionaryIndex != m_DictionaryIndex;
	}

	bool operator<(const TDictionaryIndex& oInd) const
	{
		return oInd.m_DictionaryIndex < m_DictionaryIndex;
	}

	bool operator>(const TDictionaryIndex& oInd) const
	{
		return oInd.m_DictionaryIndex > m_DictionaryIndex;
	}

	bool operator<=(const TDictionaryIndex& oInd) const
	{
		return oInd.m_DictionaryIndex <= m_DictionaryIndex;
	}

	bool operator>=(const TDictionaryIndex& oInd) const
	{
		return oInd.m_DictionaryIndex >= m_DictionaryIndex;
	}

	///возвращает тип индекса
	SS::Interface::Core::BlackBox::EIndexType GetIndexType()
	{
		switch(m_DictionaryNumber)
		{
			case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednNumeric: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednFloatPointNumeric:
			case SS::Dictionary::DATA_TYPE::NAMES::ednRomanNumeric:
			case SS::Dictionary::DATA_TYPE::NAMES::ednUnknown: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednSymbols: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednOrderingEnglish:
			case SS::Dictionary::DATA_TYPE::NAMES::ednOrderingRussian:
				return SS::Interface::Core::BlackBox::eitMorphoIndex;

			case SS::Dictionary::DATA_TYPE::NAMES::ednSyntax: 
				return SS::Interface::Core::BlackBox::eitSyntaxIndex;

			case SS::Dictionary::DATA_TYPE::NAMES::ednDates: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednSemantic: 
				return SS::Interface::Core::BlackBox::eitSemanticIndex;

			case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng: 
				return SS::Interface::Core::BlackBox::eitSynonymIndex;

			case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednExtraECombinatory:
			case SS::Dictionary::DATA_TYPE::NAMES::ednExtraRCombinatory:
				return SS::Interface::Core::BlackBox::eitCombinatoryIndex;

			default:
				return SS::Interface::Core::BlackBox::eitUndefinedIndex;
		}
	}

	///возвращает тип индекса
	std::wstring GetIndexTypeW()
	{
		std::wstring wsIndexType;
		switch(m_DictionaryNumber)
		{
			case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednNumeric: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednFloatPointNumeric:
			case SS::Dictionary::DATA_TYPE::NAMES::ednRomanNumeric:
			case SS::Dictionary::DATA_TYPE::NAMES::ednUnknown: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednSymbols: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednOrderingEnglish:
			case SS::Dictionary::DATA_TYPE::NAMES::ednOrderingRussian:
				{
					wsIndexType = L"Morpho";
					return wsIndexType;
				}

			case SS::Dictionary::DATA_TYPE::NAMES::ednSyntax: 
				{

					wsIndexType = L"Syntax";
					return wsIndexType;
				}

			case SS::Dictionary::DATA_TYPE::NAMES::ednDates: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednSemantic: 
				{
					wsIndexType = L"Semantic";
					return wsIndexType;
				}

			case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsRus: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednSynonimsEng: 
				{
					wsIndexType = L"Synonims";
					return wsIndexType;
				}

			case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory: 
			case SS::Dictionary::DATA_TYPE::NAMES::ednExtraECombinatory:
			case SS::Dictionary::DATA_TYPE::NAMES::ednExtraRCombinatory:
				{
					wsIndexType = L"Combinatory";
					return wsIndexType;
				}

			default:
				{
					wsIndexType = L"Undefine";
					return wsIndexType;
				}
		}
	}

	//возвращает €зык индекса
	SS::Core::Types::EAnalyseLanguage GetLanguage()
	{
		switch(m_DictionaryNumber)
		{
			case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus:
			case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
			case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
			case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
			case SS::Dictionary::DATA_TYPE::NAMES::ednRCombinatory:
			case SS::Dictionary::DATA_TYPE::NAMES::ednOrderingRussian:
			case SS::Dictionary::DATA_TYPE::NAMES::ednExtraRCombinatory:
				return SS::Core::Types::ealRussian;

			case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng:
			case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
			case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
			case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng:
			case SS::Dictionary::DATA_TYPE::NAMES::ednECombinatory:
			case SS::Dictionary::DATA_TYPE::NAMES::ednOrderingEnglish:
			case SS::Dictionary::DATA_TYPE::NAMES::ednExtraECombinatory:
				return SS::Core::Types::ealEnglish;

			default:
				return SS::Core::Types::ealUndefined;
		}
	}
};

}
}
}
