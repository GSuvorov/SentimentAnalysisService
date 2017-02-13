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
				namespace Russian
				{
					/// анафора в тексте
					class CSMExAnaphoraR :
						public CSMExAnaphora
					{
					public:
						CSMExAnaphoraR(void);
						~CSMExAnaphoraR(void);
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