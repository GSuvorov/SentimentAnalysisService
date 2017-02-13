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

/// разбор вопроса "кто"
class CRQKto :
	virtual public CSMQuestionAnalysisR
{
public:
	CRQKto(void);
	~CRQKto(void);
	bool AnalyseQuestion(EQuestionType eQType);

};


				}
			}
		}
	}
}