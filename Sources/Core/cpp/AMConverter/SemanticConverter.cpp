#include "StdAfx.h"
#include ".\SemanticConverter.h"

namespace SS
{
namespace Core
{
namespace Features
{
CSemanticConverter::CSemanticConverter(void)
{
}

CSemanticConverter::~CSemanticConverter(void)
{
}

bool CSemanticConverter::Code(/*in*/CSemanticFeature *pSemanticFeature, /*out*/SS::Dictionary::Types::TDictionaryIndex* pDictionaryIndex)
{
	SS_TRY
	{
		bitfieldsstructures::SbfSemanticType	oSemanticType;

		oSemanticType.m_STLevelFirts	= pSemanticFeature->m_SemanticType.GetSingleValue();

		if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtTimeInterval))
		{
			oSemanticType.m_STLevelSecond	= pSemanticFeature->m_STimeIntervalType.GetSingleValue();
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtPlace))
		{
			oSemanticType.m_STLevelSecond	= pSemanticFeature->m_SPlaceType.GetSingleValue();
			if(pSemanticFeature->m_SPlaceType.Equal(Values::TSPlaceType.pltCity))
			{
				oSemanticType.m_STLevelThird = pSemanticFeature->m_SPCity.GetSingleValue();
				if(pSemanticFeature->m_SPCity.Equal(Values::TCityType.smtcityArea))
				{
					oSemanticType.m_STLevelFourth = pSemanticFeature->m_SPCArea.GetSingleValue();
					if(pSemanticFeature->m_SPCArea.Equal(Values::TAreaType.smtareaDistrict))
					{
						oSemanticType.m_STLevelFifth = pSemanticFeature->m_SPCADistrict.GetSingleValue();
						if(pSemanticFeature->m_SPCADistrict.Equal(Values::TDistrictType.smtDistrMetro))
						{
							oSemanticType.m_STLevelSixth = pSemanticFeature->m_SPCADMetro.GetSingleValue();
						}
					}
				}
			}
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtMeasure))
		{
			oSemanticType.m_STLevelSecond	= pSemanticFeature->m_SMeasureType.GetSingleValue();
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtPerson))
		{
			oSemanticType.m_STLevelSecond	= pSemanticFeature->m_SPersonType.GetSingleValue();
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtOrganization))
		{
			oSemanticType.m_STLevelSecond	= pSemanticFeature->m_SOrganizationType.GetSingleValue();
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtVerb))
		{
			oSemanticType.m_STLevelSecond	= pSemanticFeature->m_SVerbType.GetSingleValue();

			CodeVerbSemantics(oSemanticType, pSemanticFeature);
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtService))
		{
			oSemanticType.m_STLevelSecond	= pSemanticFeature->m_SServiceTypes.GetSingleValue();
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtInfo))
		{
			oSemanticType.m_STLevelSecond	= pSemanticFeature->m_SInfoTypes.GetSingleValue();
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtMaterial))
		{
			oSemanticType.m_STLevelSecond	= pSemanticFeature->m_SMaterial.GetSingleValue();
			CodeMaterial(oSemanticType, /*out*/pSemanticFeature);

		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtBuilding))
		{
			oSemanticType.m_STLevelSecond	= pSemanticFeature->m_SBuilding.GetSingleValue();
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtTransport))
		{
			oSemanticType.m_STLevelSecond	= pSemanticFeature->m_STransport.GetSingleValue();
		}

		
		pDictionaryIndex->SetId(*(unsigned int*)&oSemanticType);
		if(pSemanticFeature->m_SGeneric.Equal(Values::TGenericType.smtGeneric))
		{
			pDictionaryIndex->SetGenericMarker(true);
		}
	}
	SS_CATCH(L"")

	return true;
}

void CSemanticConverter::CodeMaterial(bitfieldsstructures::SbfSemanticType&	oSemanticType, /*out*/CSemanticFeature *pSemanticFeature)
{
	SS_TRY
	{
		/*1*/if(pSemanticFeature->m_SMaterial.Equal(Values::TSMaterialType.mtrlFood))
		{
			oSemanticType.m_STLevelThird	= pSemanticFeature->m_SMFood.GetSingleValue();
		}else /*2*/if(pSemanticFeature->m_SMaterial.Equal(Values::TSMaterialType.mtrlForFreeTime))
		{
			oSemanticType.m_STLevelThird	= pSemanticFeature->m_SMForFreeTime.GetSingleValue();
		}
	}
	SS_CATCH(L"")
}

void CSemanticConverter::EnCodeMaterial(bitfieldsstructures::SbfSemanticType	oSemanticType, /*out*/CSemanticFeature *pSemanticFeature)
{
	SS_TRY
	{
		/*1*/if(pSemanticFeature->m_SMaterial.Equal(Values::TSMaterialType.mtrlFood))
		{
			pSemanticFeature->m_SMFood = oSemanticType.m_STLevelThird;
		}else /*2*/if(pSemanticFeature->m_SMaterial.Equal(Values::TSMaterialType.mtrlForFreeTime))
		{
			pSemanticFeature->m_SMForFreeTime = oSemanticType.m_STLevelThird;
		}
	}
	SS_CATCH(L"")
}

void CSemanticConverter::CodeVerbSemantics(bitfieldsstructures::SbfSemanticType&	oSemanticType, /*out*/CSemanticFeature *pSemanticFeature)
{
	SS_TRY
	{

	
		/*2*/if(pSemanticFeature->m_SVerbType.Equal(Values::TSVerbType.semvtBe))
		{
			oSemanticType.m_STLevelThird	= pSemanticFeature->m_SVBeType.GetSingleValue();
		}else
		/*3*/if(pSemanticFeature->m_SVerbType.Equal(Values::TSVerbType.semvtAcquirePossess))
		{
			oSemanticType.m_STLevelThird	= pSemanticFeature->m_SVAcquirePossessType.GetSingleValue();
			if(pSemanticFeature->m_SVAcquirePossessType.Equal(Values::TSVAcquirePossessType.tsvaptAcquire))
			{
				oSemanticType.m_STLevelFourth  = pSemanticFeature->m_SVAcquireType.GetSingleValue();
			}
		}else
		/*5*/if(pSemanticFeature->m_SVerbType.Equal(Values::TSVerbType.semvtEnd))
		{
			oSemanticType.m_STLevelThird	= pSemanticFeature->m_SVEndType.GetSingleValue();
		}
	}
	SS_CATCH(L"")
}

void CSemanticConverter::EnCodeVerbSemantics(bitfieldsstructures::SbfSemanticType	oSemanticType, /*out*/CSemanticFeature *pSemanticFeature)
{
	SS_TRY;

		/*2*/if(pSemanticFeature->m_SVerbType.Equal(Values::TSVerbType.semvtBe))
		{
			pSemanticFeature->m_SVBeType = oSemanticType.m_STLevelThird;
		}else
		/*3*/if(pSemanticFeature->m_SVerbType.Equal(Values::TSVerbType.semvtAcquirePossess))
		{
			pSemanticFeature->m_SVAcquirePossessType = oSemanticType.m_STLevelThird;
			if(pSemanticFeature->m_SVAcquirePossessType.Equal(Values::TSVAcquirePossessType.tsvaptAcquire))
			{
				pSemanticFeature->m_SVAcquireType = oSemanticType.m_STLevelFourth ;
			}
		}else
		/*5*/if(pSemanticFeature->m_SVerbType.Equal(Values::TSVerbType.semvtEnd))
		{
			pSemanticFeature->m_SVEndType = oSemanticType.m_STLevelThird;
		}
				
	SS_CATCH(L"");


}

bool CSemanticConverter::EnCode(/*in*/SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex, /*out*/CSemanticFeature *pSemanticFeature)
{
	SS_TRY;

		pSemanticFeature->Undefine();

		if(oDictionaryIndex.GetGenericMarker())
		{
			pSemanticFeature->m_SGeneric = Values::TGenericType.smtGeneric;
		}else
		{
			pSemanticFeature->m_SGeneric = Values::TGenericType.smtNotGeneric;
		}

		bitfieldsstructures::SbfSemanticType	oSemanticType;
		oSemanticType	= oDictionaryIndex.GetId();

		pSemanticFeature->m_SemanticType = oSemanticType.m_STLevelFirts;

		if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtTimeInterval))
		{
			pSemanticFeature->m_STimeIntervalType	= oSemanticType.m_STLevelSecond;
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtPlace))
		{
			pSemanticFeature->m_SPlaceType	= oSemanticType.m_STLevelSecond;
			pSemanticFeature->m_SPCity		= oSemanticType.m_STLevelThird;
			pSemanticFeature->m_SPCArea		= oSemanticType.m_STLevelFourth;
			pSemanticFeature->m_SPCADistrict= oSemanticType.m_STLevelFifth;
			pSemanticFeature->m_SPCADMetro	= oSemanticType.m_STLevelSixth;
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtMeasure))
		{
			pSemanticFeature->m_SMeasureType	= oSemanticType.m_STLevelSecond;
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtPerson))
		{
			pSemanticFeature->m_SPersonType	= oSemanticType.m_STLevelSecond;
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtOrganization))
		{
			pSemanticFeature->m_SOrganizationType	= oSemanticType.m_STLevelSecond;
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtVerb))
		{
			pSemanticFeature->m_SVerbType	= oSemanticType.m_STLevelSecond;

			EnCodeVerbSemantics(oSemanticType, pSemanticFeature);
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtService))
		{
			pSemanticFeature->m_SServiceTypes	= oSemanticType.m_STLevelSecond;
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtInfo))
		{
			pSemanticFeature->m_SInfoTypes		= oSemanticType.m_STLevelSecond;
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtMaterial))
		{
			pSemanticFeature->m_SMaterial		= oSemanticType.m_STLevelSecond;
			EnCodeMaterial(oSemanticType, pSemanticFeature);
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtTransport))
		{
			pSemanticFeature->m_STransport		= oSemanticType.m_STLevelSecond;
		}else if(pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtBuilding))
		{
			pSemanticFeature->m_SBuilding		= oSemanticType.m_STLevelSecond;
		}

		
	SS_CATCH(L"");

	return true;
}

}
}
}