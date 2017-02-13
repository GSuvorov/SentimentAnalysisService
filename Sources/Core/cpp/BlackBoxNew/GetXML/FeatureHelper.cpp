#include "stdafx.h"
#include "FeatureHelper.h"

#include <assert.h>


namespace SS
{
namespace Core
{
namespace BlackBox
{
namespace XML
{

/////////////////////////////////////////////////////////////////////////////////////
// Constructors
/////////////////////////////////////////////////////////////////////////////////////

CFeatureHelper::CFeatureHelper() :
		m_pSemanticConverter(NULL),
		m_pMorphoConverter(NULL),
		m_pSyntaxConverter(NULL),
		m_pSemanticFeature(NULL),
		m_pMorphoFeature(NULL),
		m_pSyntaxFeature(NULL)
{
}

CFeatureHelper::~CFeatureHelper()
{
	if(m_pSemanticFeature)
		delete m_pSemanticFeature;
	if(m_pMorphoFeature)
		delete m_pMorphoFeature;
	if(m_pSyntaxFeature)
		delete m_pSyntaxFeature;
}

/////////////////////////////////////////////////////////////////////////////////////
// CFeatureHelper interface

void CFeatureHelper::Init(ILoadManager* pLoadManager)
{
	SS_TRY;

	// converters
	m_pSemanticConverter = (IAMConverterSemantic*)
		pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterSemantic, IID_AMConverterSemantic);
	m_pMorphoConverter = (IAMConverterMorpho*)
		pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);
	m_pSyntaxConverter = (IAMConverterSyntax*)
		pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterSyntax, IID_AMConverterSyntax);

	assert(!m_pSemanticFeature && !m_pMorphoFeature && !m_pSyntaxFeature);
	m_pSemanticFeature = new CSemanticFeature;
	m_pMorphoFeature = new CMorphoFeature;
	m_pSyntaxFeature = new CSyntaxFeatureApriori;

	m_oSyntaxCategories.InitValues();

	SS_CATCH(L"");
}

string CFeatureHelper::GetDescription(TDictionaryIndex oIndex)
{
	SS::Interface::Core::BlackBox::EIndexType eit = oIndex.GetIndexType();
	switch(eit)
	{
		case SS::Interface::Core::BlackBox::eitMorphoIndex:
			return "Morpho";

		case SS::Interface::Core::BlackBox::eitSemanticIndex:
			m_pSemanticFeature->Undefine();
			m_pSemanticConverter->EnCode(oIndex.GetDictionaryIndex(), m_pSemanticFeature);
			return m_pSemanticFeature->GetPrintableDataDescription();

		case SS::Interface::Core::BlackBox::eitSyntaxIndex:
		{
			LPCSTR sz = m_oSyntaxCategories.GetValueName(oIndex.GetId());
			return string(sz);
		}

		case SS::Interface::Core::BlackBox::eitSynonymIndex:
			return "Synonym";

		case SS::Interface::Core::BlackBox::eitCombinatoryIndex:
			return "Combi";

		default:
			return "Undefined";
	}
}

string CFeatureHelper::GetDescription(IFeature* pFeature, IIndex* pParentIndex)
{
	DictionariesNumber edn = pParentIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber();

	m_pMorphoFeature->Undefine();
	m_pMorphoConverter->EnCode(edn, pFeature->GetMorphoInfo(), pFeature->GetPartOfSpeech(), m_pMorphoFeature);
	return m_pMorphoFeature->GetPrintableDataDescription();
}

bool CFeatureHelper::IsTitleUnit(IUnit* pUnit)
{
	SS_TRY;

	if(	pUnit->GetSyntaxFeature() &&
		pUnit->GetSyntaxFeature()->GetSyntaxFeatureApriori())
	{
		return pUnit->GetSyntaxFeature()->GetSyntaxFeatureApriori()->
			m_SyntaxCategories.IsIntersectsWith(&Values::SyntaxCategories.scTitle);
	}

	SS_CATCH(L"");
	return false;
}

void CFeatureHelper::SetTitleUnit(IUnit* pUnit)
{
	pUnit->CreateSyntaxFeature()->GetSyntaxFeatureApriori()->
		m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scTitle);
}

}
}
}
}
