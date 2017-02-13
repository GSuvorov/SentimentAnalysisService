#pragma once
#include ".\SMExClauseTimePlace.h"

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
					/// 
					class CSMExClauseTimePlaceR :
						public CSMExClauseTimePlace
					{
					public:
						CSMExClauseTimePlaceR(void);
						~CSMExClauseTimePlaceR(void);

						bool Init(IText * pIText);

					};


				}

			}
		}
	}
}