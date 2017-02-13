#include "StdAfx.h"
#include ".\SMExClauseTimePlaceR.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace IndexationExperts
			{
				namespace Russian
				{


					CSMExClauseTimePlaceR::CSMExClauseTimePlaceR(void)
					{
						m_bInit = false;
						m_AnalyseLanguage=SS::Core::Types::ealRussian;
						m_sDescription=_T(__FUNCTION__);
					}

					CSMExClauseTimePlaceR::~CSMExClauseTimePlaceR(void)
					{
					}

					bool CSMExClauseTimePlaceR::Init(IText * pIText)
					{
						if(!CSMExClauseTimePlace::Init(pIText)) return false;

						if(!m_bInit)
						{
							m_bInit = true;

							m_Words.clear();
							m_Words.insert(wstring(_T("где")));
							m_Words.insert(wstring(_T("когда")));
						}

						return true;
					}

				}
			}
		}
	}
}