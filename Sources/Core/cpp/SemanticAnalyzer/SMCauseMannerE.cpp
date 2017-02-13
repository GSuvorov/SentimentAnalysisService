#include "StdAfx.h"
#include ".\smcausemannere.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace IndexationExperts
			{
				namespace English
				{
				


CSMCauseMannerE::CSMCauseMannerE(void)
{
	m_bInit = false;
	m_uiBYSource=0;
	m_uiTOSource=0;
	m_AnalyseLanguage=SS::Core::Types::ealEnglish;
	m_sDescription=_T(__FUNCTION__);
}

CSMCauseMannerE::~CSMCauseMannerE(void)
{
}
bool CSMCauseMannerE::AnalizeSentence()
{
	SS_TRY
	{
		TBoxUnits l_Units;
		do{
			if(!IsSyntax())
			{
				if(IsContainFeature(efEVerbParticiplePresent))
				{
					Mem();
					if( GetPrevUnit() && IsIndexExist(m_pCurrentUnit->GetFirstIndex(),m_uiBYSource))
					{
						l_Units.clear();
						l_Units.push_front(m_pCurrentUnit);
						GotoMem();
						l_Units.push_back(m_pCurrentUnit);
						ProcessManner(l_Units);
					}else GotoMem();
				}else if (IsContainFeature(efEVerbInfinitive) )
				{
					Mem();
					if( GetPrevUnit())
					{
						if (IsIndexExist(m_pCurrentUnit->GetFirstIndex(),m_uiTOSource))
						{
							AddCauseCategory();
							GotoMem();
							AddCauseCategory();
						}		
					}
					GotoMem();
				}
			}	

		}while(GetNextUnit());
		return true;
	}
	SS_CATCH(L"")
}


void CSMCauseMannerE::ProcessManner(TBoxUnits & l_Units)
{
	if (l_Units.empty()) return;
	
	IUnit * pHead=JoinUpUnits(l_Units);
	if (pHead)
	{
		m_pCurrentUnit=pHead;
		AddMannerCategory();
		SetSyntax();
	}
}

bool CSMCauseMannerE::Init(IText * pIText)
{
	if(!CSMSentenceExpert::Init(pIText)) return false;

	if(!m_bInit)
	{
		m_bInit = true;

		list<UINT> l_Sources;
		if(!m_uiBYSource)
			if(GetWordSourceIndices(_T("by"),l_Sources))
				m_uiBYSource=l_Sources.front();
		l_Sources.clear();
		if(!m_uiTOSource)
			if(GetWordSourceIndices(_T("to"),l_Sources))
				m_uiTOSource=l_Sources.front();
	}

	return true;
}

				}
			}
		}
	}
}