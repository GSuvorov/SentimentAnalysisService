#include "StdAfx.h"
#include ".\SMExSimpleQueryParserR.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Russian
			{


				CSMExSimpleQueryParserR::CSMExSimpleQueryParserR(void)
				{
					m_bInit = false;
					m_AnalyseLanguage=SS::Core::Types::ealRussian;
					m_sDescription=_T(__FUNCTION__);
				}

				CSMExSimpleQueryParserR::~CSMExSimpleQueryParserR(void)
				{
				}

				bool CSMExSimpleQueryParserR::Init(IText * pIText)
				{
					if(!CSMExSimpleQueryParser::Init(pIText)) return false;

					if(!m_bInit)
					{
						m_bInit = true;

						m_Words.clear();
						m_Words.insert(wstring(_T("где")));
						m_Words.insert(wstring(_T("когда")));
						m_Words.insert(wstring(_T("откуда")));
						m_Words.insert(wstring(_T("куда")));
						m_Words.insert(wstring(_T("как")));
						m_Words.insert(wstring(_T("почему")));
					}

					return true;
				}

			}
		}
	}
}