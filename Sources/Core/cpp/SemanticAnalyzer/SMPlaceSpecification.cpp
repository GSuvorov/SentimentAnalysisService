#include "StdAfx.h"
#include ".\smplacespecification.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{


CSMPlaceSpecification::CSMPlaceSpecification(void)
{
	m_sDescription=_T(__FUNCTION__);
}

CSMPlaceSpecification::~CSMPlaceSpecification(void)
{
}

bool CSMPlaceSpecification::AnalizeSentence()
{
	SS_TRY
	{
		if(!GetFirstUnit()) return true;
		TBoxUnits l_Units;
		do{
			bool bCityLeft, bCountryLeft;
			bCityLeft = m_pCurrentUnit->GetInfo()->_PlaceType.IsContain(Values::TSPlaceType.pltCity);
			bCountryLeft = m_pCurrentUnit->GetInfo()->_PlaceType.IsContain(Values::TSPlaceType.pltCountry);
			if (bCityLeft || bCountryLeft)
			{
				l_Units.push_back(m_pCurrentUnit);
				if (GetNextUnit() && m_LexemType.IsSComma() /*&& SpecialTypeEquals(ustUndefined)*/  )
				{
					l_Units.push_back(m_pCurrentUnit);
					if (GetNextUnit())
					{
						bool bCountryRight, bContinentRight;
						bCountryRight = m_pCurrentUnit->GetInfo()->_PlaceType.IsContain(Values::TSPlaceType.pltCountry);
						bContinentRight = m_pCurrentUnit->GetInfo()->_PlaceType.IsContain(Values::TSPlaceType.pltContinent);
						if(((bCityLeft && bCountryRight) || (bCountryLeft && bContinentRight)))
						{
							PlaceProcess(l_Units);
						}
					}
				}
				l_Units.clear();
			}
			
		}while(GetNextUnit());
		return true;
	}
	SS_CATCH(L"")
}

void CSMPlaceSpecification::PlaceProcess(TBoxUnits & l_Units)
{
	list<SS::Dictionary::Types::TDictionaryIndex> l_Semantic;
	l_Units.push_back(m_pCurrentUnit);
	IUnit * pHead=JoinUpUnits(l_Units);
	pHead->ClearInfo();
	pHead->GetInfo()->_PlaceType=Values::TSPlaceType.pltCountry;
	AddFeature(efcSemanticCommon);
	AddFeature(efcNoun);
	RemoveSemanticIndicesFromUnit(l_Semantic,l_Units.back());
	RemoveSemanticIndicesFromUnit(l_Semantic,l_Units.front());
	KeepSpecifedSemanticTypes(Values::TSemanticType.smtPlace,l_Semantic);
	SetSemanticIndices(l_Semantic, pHead);
}
		


		}
	}
}