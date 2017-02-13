#include "StdAfx.h"
#include "WeightTable.h"

#include <assert.h>


namespace SS
{
namespace TransformGenerator
{

/////////////////////////////////////////////////////////////////////////////////////
// Static members

static const float s_fDefaultWeight = 1.;

static const float s_rPerson[3][6] = 
{
	//	Actor		Prof.		N-ty		Agent		Name		Undef
	{	0.75,		0.75,		0.5,		0.25,		1.,			0.25,	},	// who def
	{	1.,			0.8,		0.75,		0.5,		0.25,		0.5,	},	// name def
	{	0.75,		0.75,		0.5,		0.25,		1.,			0.25,	},	// subj fin
};

static const float s_rTime[] =
{
	//	Year		Month		Number		Undef/Day/Time
		0.9,		0.5,		0.4,		0.25,
};

static const float s_rPlace[] =
{
	//	City		Country		State		Cont.		Undef
		1.,			0.75,		0.7,		0.5,		0.25,
};

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CWeightTable::CWeightTable() :
		m_eQueryType(qtNone),
		m_pSemanticConverter(NULL),
		m_pSemanticFeature(NULL)
{
}

CWeightTable::~CWeightTable()
{
	if(m_pSemanticFeature)
		delete m_pSemanticFeature;
}

/////////////////////////////////////////////////////////////////////////////////////
// CWeightTable interface

void CWeightTable::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	SS_TRY;

	if(GetLoadManager())
		return;

	CBaseCoreClass::SetLoadManager(pLoadManager);
	m_pSemanticFeature = new CSemanticFeature;
	m_pSemanticConverter = (IAMConverterSemantic*)
		pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterSemantic, IID_AMConverterSemantic);

	SS_CATCH(L"");
}

void CWeightTable::SetWeights(ITransformation* pTrans)
{
	SS_TRY;

	IQueryUnit* pQueryWord = GetQueryWord(pTrans);
	if(!pQueryWord)
		return;

	IQueryUnitIndexCollection* pColl = pQueryWord->GetQueryUnitIndexCollection(eitSemanticIndex);
	if(!pColl)
		return;

	m_eQueryType = GetQueryType(pQueryWord);
	if(m_eQueryType == qtNone)
		return;

	for(IQueryUnitIndex* pIndex = pColl->GetFirstQueryUnitIndex(); pIndex; pIndex = pIndex->GetNextQueryUnitIndex())
	{
		float fWeight = GetWeight(pIndex);
		pIndex->SetWeight(fWeight);
	}

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// CWeightTable functions

IQueryUnit* CWeightTable::GetQueryWord(ITransformation* pTrans)
{
	SS_TRY;

	for(IGroup* pGroup = pTrans->GetFirstGroup(); pGroup; pGroup = pGroup->GetNextGroup())
	{
		if(pGroup->GetSyntaxFeature()->IsQuestion())
			return pGroup->GetFirstQueryUnit();
	}

	SS_CATCH(L"");
	return NULL;
}

CWeightTable::EQueryType CWeightTable::GetQueryType(IQueryUnit* pUnit)
{
	SS_TRY;

	IQueryUnitIndexCollection* pColl = pUnit->GetQueryUnitIndexCollection(eitSyntaxIndex);
	if(!pColl)
		return qtNone;

	if(IsSyntaxIndexInColl(pColl, Values::SyntaxCategories.scNameDefinition))
		return qtNameDefinition;
	else if(IsSyntaxIndexInColl(pColl, Values::SyntaxCategories.scWhoDefinition))
		return qtWhoDefinition;
	else if(IsSyntaxIndexInColl(pColl, Values::SyntaxCategories.scSubjectFin) ||
			IsSyntaxIndexInColl(pColl, Values::SyntaxCategories.scObject))
		return qtSubjectFin;
	else if(IsSyntaxIndexInColl(pColl, Values::SyntaxCategories.scAjunctTimeFixedPoint) ||
			IsSyntaxIndexInColl(pColl, Values::SyntaxCategories.scAjunctTimeSource) ||
			IsSyntaxIndexInColl(pColl, Values::SyntaxCategories.scAjunctTimeDuration) ||
			IsSyntaxIndexInColl(pColl, Values::SyntaxCategories.scAjunctTimeEnd))
		return qtAdjunctTime;
	else if(IsSyntaxIndexInColl(pColl, Values::SyntaxCategories.scAjunctPlaceFixedPoint) ||
			IsSyntaxIndexInColl(pColl, Values::SyntaxCategories.scAjunctPlaceSource) ||
			IsSyntaxIndexInColl(pColl, Values::SyntaxCategories.scAjunctPlaceEnd))
		return qtAdjunctPlace;

	SS_CATCH(L"");
	return qtNone;
}

bool CWeightTable::IsSyntaxIndexInColl(IQueryUnitIndexCollection* pColl, const SyntaxCategories& sc)
{
	SS_TRY;

	for(IQueryUnitIndex* pIndex = pColl->GetFirstQueryUnitIndex(); pIndex; pIndex = pIndex->GetNextQueryUnitIndex())
	{
		TDictionaryIndex oInd(pIndex->GetIndex());
		if(oInd.GetId() == sc.GetSingleValue())
			return true;
	}

	SS_CATCH(L"");
	return false;
}

float CWeightTable::GetWeight(IQueryUnitIndex* pIndex)
{
	SS_TRY;

	m_pSemanticFeature->Undefine();
	TDictionaryIndex oInd(pIndex->GetIndex());
	m_pSemanticConverter->EnCode(oInd.GetId(), m_pSemanticFeature);
	if(m_pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtPerson))
	{
		if(m_eQueryType != qtWhoDefinition && m_eQueryType != qtNameDefinition && m_eQueryType != qtSubjectFin)
			return 0.;

		if(m_pSemanticFeature->m_SPersonType.Equal(Values::TSPersonType.semprstProfession))
			return s_rPerson[m_eQueryType - 1][0];
		else if(m_pSemanticFeature->m_SPersonType.Equal(Values::TSPersonType.semprstActor))
			return s_rPerson[m_eQueryType - 1][1];
		else if(m_pSemanticFeature->m_SPersonType.Equal(Values::TSPersonType.semprstNationality))
			return s_rPerson[m_eQueryType - 1][2];
		else if(m_pSemanticFeature->m_SPersonType.Equal(Values::TSPersonType.semprstAgent))
			return s_rPerson[m_eQueryType - 1][3];
		else if(m_pSemanticFeature->m_SPersonType.Equal(Values::TSPersonType.semprstFirstName) ||
				m_pSemanticFeature->m_SPersonType.Equal(Values::TSPersonType.semprstLastName) ||
				m_pSemanticFeature->m_SPersonType.Equal(Values::TSPersonType.semprstMiddleName) ||
				m_pSemanticFeature->m_SPersonType.Equal(Values::TSPersonType.semprstName))
			return s_rPerson[m_eQueryType - 1][4];
		else if(m_pSemanticFeature->m_SPersonType.Equal(Values::TSPersonType.Undefined))
			return s_rPerson[m_eQueryType - 1][5];
	}
	else if(m_pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtTimeInterval))
	{
		if(m_eQueryType != qtAdjunctTime)
			return 0.;

		if(m_pSemanticFeature->m_STimeIntervalType.Equal(Values::TSTimeIntervalType.semtitDateYear))
			return s_rTime[0];
		else if(m_pSemanticFeature->m_STimeIntervalType.Equal(Values::TSTimeIntervalType.semtitDateMonth))
			return s_rTime[1];
		else if(m_pSemanticFeature->m_STimeIntervalType.Equal(Values::TSTimeIntervalType.semtitDateNumber))
			return s_rTime[2];
		else if(m_pSemanticFeature->m_STimeIntervalType.Equal(Values::TSTimeIntervalType.Undefined) ||
				m_pSemanticFeature->m_STimeIntervalType.Equal(Values::TSTimeIntervalType.semtitDateDay) ||
				m_pSemanticFeature->m_STimeIntervalType.Equal(Values::TSTimeIntervalType.semtitDayTime))
			return s_rTime[3];
	}
	else if(m_pSemanticFeature->m_SemanticType.Equal(Values::TSemanticType.smtPlace))
	{
		if(m_eQueryType != qtAdjunctPlace)
			return 0.;

		if(	m_pSemanticFeature->m_SPlaceType.Equal(Values::TSPlaceType.pltCity))
			return s_rPlace[0];
		else if(m_pSemanticFeature->m_SPlaceType.Equal(Values::TSPlaceType.pltCountry))
			return s_rPlace[1];
		else if(m_pSemanticFeature->m_SPlaceType.Equal(Values::TSPlaceType.pltState) ||
				m_pSemanticFeature->m_SPlaceType.Equal(Values::TSPlaceType.pltProvince))
			return s_rPlace[2];
		else if(m_pSemanticFeature->m_SPlaceType.Equal(Values::TSPlaceType.pltContinent))
			return s_rPlace[3];
		else if(m_pSemanticFeature->m_SPlaceType.Equal(Values::TSPlaceType.Undefined))
			return s_rPlace[4];
	}

	SS_CATCH(L"");
	return s_fDefaultWeight;
}

}
}
