#pragma once
#include "./smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace English
			{
				/// выделение субъектов и объектов в тексте
				class CSMSubjectObjectE :
					public CSMSentenceExpert
				{
				public:
					CSMSubjectObjectE(void);
					~CSMSubjectObjectE(void);
					bool AnalizeSentence();
				};


			}

		}
	}
}