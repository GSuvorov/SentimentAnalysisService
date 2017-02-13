#pragma once
#include "./smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{
				namespace English
				{
					/// выделение субъектов и объектов в запросе
					class CSMQuestionSubjectObjectE :
						public CSMSentenceExpert
					{
					public:
						CSMQuestionSubjectObjectE(void);
						~CSMQuestionSubjectObjectE(void);
						bool AnalizeSentence();
					};
				}
			}

		}
	}
}