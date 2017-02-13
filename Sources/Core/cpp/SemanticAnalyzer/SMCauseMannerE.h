#pragma once
#include "smcausemanner.h"

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
				
				/// обстоятельства причины и способа
				class CSMCauseMannerE :
					public CSMCauseManner
				{
				public:
					CSMCauseMannerE(void);
					~CSMCauseMannerE(void);
					bool AnalizeSentence();
					bool Init(IText * pIText);
				private:
					UINT m_uiBYSource;
					UINT m_uiTOSource;
					void ProcessManner(TBoxUnits & l_Units);

				};

				}
			}
		}
	}
}