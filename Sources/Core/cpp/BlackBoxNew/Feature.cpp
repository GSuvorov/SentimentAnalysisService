#include "stdafx.h"
#include "Feature.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CFeature::CFeature()
{
	ClearFeature();
}

CFeature::~CFeature()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// IFeature interface

unsigned int CFeature::GetMorphoIndex()
{
	return m_uiMorphoIndex;
}

void CFeature::SetMorphoIndex(unsigned int uiValue)
{
	m_uiMorphoIndex = uiValue;
}

unsigned int CFeature::GetMorphoInfo()
{
	return m_uiMorphoInfo;
}

void CFeature::SetMorphoInfo(unsigned int uiValue)
{
	m_uiMorphoInfo = uiValue;
}

unsigned int CFeature::GetPartOfSpeech()
{
	return m_uiPartOfSpeech;
}

void CFeature::SetPartOfSpeech(unsigned int uiValue)
{
	m_uiPartOfSpeech = uiValue;
}

IFeature* CFeature::ReleaseFeature()
{
	return dynamic_cast<CFeature*>(ReleaseItem());
}

void CFeature::ClearFeature()
{
	m_uiMorphoIndex = 0;
	m_uiMorphoInfo = 0;
	m_uiPartOfSpeech = 0;
}

}
}
}
