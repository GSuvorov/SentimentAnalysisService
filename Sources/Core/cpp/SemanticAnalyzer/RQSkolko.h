#pragma once
#include "smquestionanalysisr.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{
				namespace Russian
				{

/// разбор вопроса "сколько"
class CRQSkolko :
	public CSMQuestionAnalysisR
{
public:
	CRQSkolko(void);
	~CRQSkolko(void);
	bool AnalyseQuestion(EQuestionType eQType);
};


				}
			}
		}
	}
}