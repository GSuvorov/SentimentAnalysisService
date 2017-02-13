#include "StdAfx.h"
#include "TypeChecker.h"


namespace SS
{
namespace TransformGenerator
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CTypeChecker::CTypeChecker() :
		m_pMorphoConverter(NULL),
		m_pMorphoFeature(NULL)
{
}

CTypeChecker::~CTypeChecker()
{
	if(m_pMorphoFeature)
		delete m_pMorphoFeature;
}

/////////////////////////////////////////////////////////////////////////////////////
// CWeightTable interface

void CTypeChecker::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	SS_TRY;

	if(GetLoadManager())
		return;

	CBaseCoreClass::SetLoadManager(pLoadManager);
	m_pMorphoFeature = new CMorphoFeature;
	m_pMorphoConverter = (IAMConverterMorpho*)
		pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);

	SS_CATCH(L"");
}

bool CTypeChecker::IsSuperlativeForm(IIndex* pIndex)
{
	SS_TRY;

	TDictionaryIndex oInd = pIndex->GetDictionaryIndex()->GetFirst();
	for(IFeature* pFeature = pIndex->GetFirstFeature(); pFeature; pFeature = pIndex->GetNextFeature())
	{
		m_pMorphoFeature->Undefine();
		m_pMorphoConverter->EnCode(oInd.GetDictionaryNumber(), pFeature->GetMorphoInfo(), 
			pFeature->GetPartOfSpeech(), m_pMorphoFeature);

		if(	m_pMorphoFeature->m_PartOfSpeechTypes.IsIntersectsWith(&Values::PartOfSpeechTypes.postAdjective) ||
			m_pMorphoFeature->m_PartOfSpeechTypes.IsIntersectsWith(&Values::PartOfSpeechTypes.postAdverb))
		{
			if(m_pMorphoFeature->m_DegreeType.IsIntersectsWith(&Values::DegreeType.dtSuperlative))
				return true;
		}
	}

	SS_CATCH(L"");
	return false;
}

}
}
