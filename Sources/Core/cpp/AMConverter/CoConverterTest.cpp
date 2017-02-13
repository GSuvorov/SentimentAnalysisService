#include "StdAfx.h"
#include ".\CoConverterTest.h"


#include ".\SemanticConverter.h"


#ifdef _SS_UNITTESTS


void SS::UnitTests::AMConverter::CCoConverterTest::TestMorpho(void)
{
	SS_TRY
	{

		SS::Core::Features::CMorphoFeature oMorphoFeature;

		oMorphoFeature.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postPronoun;
		oMorphoFeature.m_GenderType = SS::Core::Features::Values::GenderType.gtMasculine;

		SS::Core::Features::CCoConverter oCoConverter;
		int iMorphoInfo, iPartOfSpeech;
		oCoConverter.Code( 0, &oMorphoFeature, &iMorphoInfo, &iPartOfSpeech);
		oCoConverter.EnCode(0,iMorphoInfo, iPartOfSpeech, &oMorphoFeature);

		int i = 0;

	}
	SS_CATCH(L"")
	return;

}


bool SS::UnitTests::AMConverter::CCoConverterTest::TestSemanticSetXML(void)
{
	SS_TRY
	{
		std::wstring wXML ;
			//L"<?xml version=\"1.0\" encoding=\"windows-1251\"?> \
			//<SemanticFeature> \
			//<TSemanticType group=\"Semantic types\"> \
			//	<smtTimeInterval type=\"Time Interval Type\">0</smtTimeInterval> \
			//	<smtPlace type=\"Place Type\">0</smtPlace> \
			//	<smtMeasure type=\"Measure Type\">0</smtMeasure> \
			//	<smtPerson type=\"Person Type\">1</smtPerson> \
			//	<smtOrganization type=\"Organization Type\">0</smtOrganization> \
			//	<smtVerb type=\"Verb Type\">0</smtVerb> \
			//	<smtService type=\"Service Type\">0</smtService> \
			//</TSemanticType> \
			//<TSTimeIntervalType group=\"Time Interval Type\"> \
			//	<semtitDateYear type=\"Year\">0</semtitDateYear> \
			//	<semtitDateMonth type=\"Month\">0</semtitDateMonth> \
			//	<semtitDateDay type=\"Date day\">0</semtitDateDay> \
			//	<semtitDayTime type=\"Day time\">0</semtitDayTime> \
			//	<semtitCentury type=\"Century\">0</semtitCentury> \
			//	<semtitDateNumber type=\"Date number\">0</semtitDateNumber> \
			//</TSTimeIntervalType>
			//<TSPlaceType group=\"Place Type\">
			//	<pltCity type=\"City\">0</pltCity>
			//	<pltCountry type=\"Country\">0</pltCountry>
			//	<pltContinent type=\"Continent\">0</pltContinent>
			//	<pltState type=\"State\">0</pltState>
			//	<pltProvince type=\"Province\">0</pltProvince>
			//	<pltHotel type=\"Hotel\">0</pltHotel>
			//	<pltIsland type=\"Island\">0</pltIsland>
			//</TSPlaceType>
			//<TSMeasureType group=\"Measure Type\">
			//	<msrtTemperature type=\"Temperature\">0</msrtTemperature>
			//	<msrtVolume type=\"Volume\">0</msrtVolume>
			//	<msrtWeight type=\"Weight\">0</msrtWeight>
			//	<msrtLength type=\"Lenght\">0</msrtLength>
			//	<msrtTime type=\"Time\">0</msrtTime>
			//	<msrtSurface type=\"Surface\">0</msrtSurface>
			//	<msrtPrice type=\"Price\">0</msrtPrice>
			//	<msrtSpeed type=\"Speed\">0</msrtSpeed>
			//	<msrtWidth type=\"Width\">0</msrtWidth>
			//	<msrtHeight type=\"Height\">0</msrtHeight>
			//	<msrtDepth type=\"Depth\">0</msrtDepth>
			//	<msrtAge type=\"Age\">0</msrtAge>
			//	<msrtNumber type=\"Number\">0</msrtNumber>
			//</TSMeasureType>
			//<TSPersonType group=\"Person Type\">
			//	<semprstFirstName type=\"Fisrt name\">0</semprstFirstName>
			//	<semprstLastName type=\"Last name\">1</semprstLastName>
			//	<semprstMiddleName type=\"Middle name\">0</semprstMiddleName>
			//	<semprstProfession type=\"Profession\">0</semprstProfession>
			//	<semprstNationality type=\"Nationality\">0</semprstNationality>
			//	<semprstAgent type=\"Agent\">0</semprstAgent>
			//	<semprstName type=\"Name\">0</semprstName>
			//</TSPersonType>
			//<TSOrganizationType group=\"Organization Type\">
			//	<orgtCompany type=\"Company\">0</orgtCompany>
			//	<orgtBank type=\"Bank\">0</orgtBank>
			//	<orgtInternationalOrganization type=\"International organization\">0</orgtInternationalOrganization>
			//	<orgtFund type=\"Fund\">0</orgtFund>
			//	<orgtParty type=\"Party\">0</orgtParty>
			//</TSOrganizationType>
			//<TSVerbType group=\"Verb Type\"><semvtBe type=\"Existence\">0</semvtBe>
			//	<semvtAcquirePossess type=\"Acquisition\">0</semvtAcquirePossess>
			//	<semvtEndBegin type=\"Beginning termination\">0</semvtEndBegin>
			//	<semvtEnd type=\"End\">0</semvtEnd>
			//	<semvtBecome type=\"Becoming\">0</semvtBecome>
			//	<semvtWeigh type=\"Weghing\">0</semvtWeigh>
			//	<semvtLast type=\"Lasting\">0</semvtLast>
			//	<semvtCost type=\"Cost\">0</semvtCost>
			//</TSVerbType>
			//<TSVBeType group=\"Existence\">
			//	<tsvbtRepresent type=\"Representation\">0</tsvbtRepresent>
			//	<tsvbtLocate type=\"Location\">0</tsvbtLocate>
			//	<tsvbtOccur type=\"Occurence\">0</tsvbtOccur>
			//	<tsvbtExist type=\"Being\">0</tsvbtExist>
			//</TSVBeType>
			//<TSVAcquirePossessType group=\"Acquisition\">
			//	<tsvaptAcquire type=\"Acquire subtype\">0</tsvaptAcquire>
			//	<tsvaptPossess type=\"Posess subtype\">0</tsvaptPossess>
			//</TSVAcquirePossessType>
			//<TSVEndType group=\"End\">
			//	<tsveEliminate type=\"Eliminate subtype\">0</tsveEliminate>
			//	<tsveUse type=\"Use subtype\">0</tsveUse>
			//</TSVEndType>
			//<TSVAcquireType group=\"Acquire verb subtype\">
			//	<tsvatReceive type=\"Receive\">0</tsvatReceive>
			//</TSVAcquireType>
			//<TSServiceTypes group=\"Service Type\">
			//	<sstAcronymCitation type=\"Acronim or quotation\">0</sstAcronymCitation>
			//	<sstNominativeDefinition type=\"Nominative definition\">0</sstNominativeDefinition>
			//	<sstWine type=\"Wine\">0</sstWine>
			//</TSServiceTypes>
			//</SemanticFeature>";

		SS::Core::Features::CSemanticFeature oSemanticfeature1, oSemanticfeature2;

		oSemanticfeature1.m_SemanticType	= SS::Core::Features::Values::TSemanticType.smtPerson;
		oSemanticfeature1.m_SPersonType		= SS::Core::Features::Values::TSPersonType.semprstLastName;

		std::string sXML;
		oSemanticfeature1.GetXMLValue(sXML);
		oSemanticfeature2.SetXMLValue(sXML);

		if(!oSemanticfeature1.Equal(&oSemanticfeature2))
		{
			return false;
		}


		int i = 0;

	}
	SS_CATCH(L"")
	return true;

}


#endif
