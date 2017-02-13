#pragma once
#include "./smexanaphora.h"

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
					/// анафора в тексте
					class CSMExAnaphoraE :
						public CSMExAnaphora
					{
					public:
						CSMExAnaphoraE(void);
						~CSMExAnaphoraE(void);
						bool Init(IText * pIText);
					protected:
						/// истина, если слово - имя или деятель, для английского
						bool IsNameAgent();

					};


				}

			}
		}
	}
}