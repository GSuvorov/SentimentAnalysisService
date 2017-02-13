#include "StdAfx.h"
#include ".\SMExClauseTimePlaceE.h"

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

					CSMExClauseTimePlaceE::CSMExClauseTimePlaceE(void)
					{
						m_bInit = false;
						m_AnalyseLanguage=SS::Core::Types::ealEnglish;
						m_sDescription=_T(__FUNCTION__);
					}

					CSMExClauseTimePlaceE::~CSMExClauseTimePlaceE(void)
					{
					}

					bool CSMExClauseTimePlaceE::Init(IText * pIText)
					{
						if(!CSMExClauseTimePlace::Init(pIText)) return false;

						if(!m_bInit)
						{
							m_bInit = true;
							
							m_Words.clear();
							m_Words.insert(wstring(_T("when")));
							m_Words.insert(wstring(_T("where")));
							m_Words.insert(wstring(_T("as long as")));
							m_Words.insert(wstring(_T("until")));
							m_Words.insert(wstring(_T("while")));
							m_Words.insert(wstring(_T("as soon as")));
							m_Words.insert(wstring(_T("since")));

						}
						
						return true;
					}

				}
			}
		}
	}
}