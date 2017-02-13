#include "StdAfx.h"
#include ".\smcausemanner.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace IndexationExperts
			{


CSMCauseManner::CSMCauseManner(void)
{
	m_sDescription=_T(__FUNCTION__);
}

CSMCauseManner::~CSMCauseManner(void)
{
}


void CSMCauseManner::AddMannerCategory()
{
	//m_Unit->m_MorphoInfo.m_SynonimsCollection.insert((SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24) + 
	//	Values::SyntaxCategories.scAjunctManner.GetSingleValue());
	AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctManner);
}
void CSMCauseManner::AddCauseCategory()
{
	//m_Unit->m_MorphoInfo.m_SynonimsCollection.insert((SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24) + 
	//	Values::SyntaxCategories.scAjunctCause.GetSingleValue());
	AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctCause);
}

bool CSMCauseManner::IsIndexExist(IIndex * pIndex, UINT uiSourceIndex)
{
	if(!pIndex) return false;

	UINT m_uiTest;

	int iSub=0;

	m_uiTest = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex() & 0x7FFFFFFF;

	if(uiSourceIndex==m_uiTest)
		return true;
	iSub=uiSourceIndex-m_uiTest/*pIndex->GetDictionaryIndex()->GetFirst()*/;

	if(iSub>=-2 && iSub<=1)
	{
		//itM++;
		if(pIndex=m_pCurrentUnit->GetNextIndex())
		{
			do{
				m_uiTest = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex() & 0x7FFFFFFF;

				if(uiSourceIndex==m_uiTest/*pIndex->GetDictionaryIndex()->GetFirst()*/)
					return true;

			}while(pIndex=m_pCurrentUnit->GetNextIndex());
		}
	}

	return false;
}

			}
		}
	}
}