#pragma once
#include "smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace PreAnalysisExperts
		{
			class CSMWordGroupper :
				public CSMSentenceExpert
			{
			public:
				CSMWordGroupper(void);
				~CSMWordGroupper(void);
				bool AnalizeSentence();
			};

		}
	}
}

