#pragma once
#include "./smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Russian
			{

				///выделение подлежащих и объектов
				class CSMSubjectR :
					public CSMSentenceExpert
				{
				public:
					CSMSubjectR(void);
					~CSMSubjectR(void);
					bool AnalizeSentence();
				};

			}
		}
	}
}