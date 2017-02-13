#include "StdAfx.h"
#include ".\smadverbialmodifier.h"
#include ".\smmanager.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{



CSMAdverbialModifier::CSMAdverbialModifier(void)
{
	m_sDescription=_T(__FUNCTION__);
}

CSMAdverbialModifier::~CSMAdverbialModifier(void)
{
}

bool CSMAdverbialModifier::ProcessPreposition(TBoxUnits &  l_Unit, IUnit * pPrep, bool bTime)
{
	if(l_Unit.empty()|| !pPrep) return false;
	list<Types::SyntaxCategories>  l_Syntax;
	wstring str(pPrep->GetWord());
	list<SS::Dictionary::Types::TDictionaryIndex> l_Semantic;
	m_StringService.LowerSz((TCHAR*)str.c_str());

	if(str==wstring(_T("")) && pPrep->HasChilds())
	{
		IUnit * pUnit=pPrep->GetLeftChildUnit();
		str+=pUnit->GetWord();
		while(pUnit=pUnit->GetRightUnit())
		{
			str+=_T(" ");
			str+=pUnit->GetWord();
		}
	}
		
	if(m_pSMManager->GetOfficialVirtualSyntax()->GetByFormPrepositionSyntaxCategories(&str,&l_Syntax))
	{
		if(FilterSyntax(l_Syntax,bTime))
		{
			IUnit * pHead;
			l_Unit.push_front(pPrep);
			if(l_Unit.size()>1) 
				pHead=JoinUpUnits(l_Unit);
			else pHead=l_Unit.front();
			if(pHead)
			{
				m_pCurrentUnit=pHead;
				RemoveSemanticIndicesFromUnit(l_Semantic,l_Unit.back());
				if (bTime) KeepSpecifedSemanticTypes(Values::TSemanticType.smtTimeInterval,l_Semantic);
				else 
				{
					Types::TSemanticType oSemanticType=Values::TSemanticType.smtOrganization;
					oSemanticType.AssignOR(&Values::TSemanticType.smtPlace);
					KeepSpecifedSemanticTypes(oSemanticType,l_Semantic);
				}
				SetSemanticIndices(l_Semantic, pHead);
				AddSyntaxCategoryIndices(l_Syntax);
				SetSyntax();
				return true;
			}
		}
	}
	return false;
}


		}
	}
}