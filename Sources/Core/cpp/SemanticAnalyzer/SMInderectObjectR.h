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
			
/// выделение в тексте косвенных дополнений
class CSMInderectObjectR :
	public CSMSentenceExpert
{
public:
	CSMInderectObjectR(void);
	~CSMInderectObjectR(void);
	bool AnalizeSentence();
private:
};

			}
		}
	}
}