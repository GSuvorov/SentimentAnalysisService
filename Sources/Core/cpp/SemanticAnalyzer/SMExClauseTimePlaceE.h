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
				namespace English
				{
					/// 
					class CSMExClauseTimePlaceE :
						public CSMExClauseTimePlace
					{
					public:
						CSMExClauseTimePlaceE(void);
						~CSMExClauseTimePlaceE(void);

						bool Init(IText * pIText);

					};


				}

			}
		}
	}
}