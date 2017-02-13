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
				namespace Russian
				{
					///удаляет юниты из группы
					class CSMWordRemoverR :
						public CSMSentenceExpert
					{
					public:
						CSMWordRemoverR(void);
						~CSMWordRemoverR(void);
						bool AnalizeSentence();
						bool Init(IText * pIText);
					private:
						set<wstring> m_Words;
						bool IsPartOfSpeech(IUnit * pUnit);
						bool IsLexem(IUnit * pUnit);
						bool IsDeleteCurrentUnit();
						bool IsAjunct(IUnit * pUnit);
						bool IsWord(const TCHAR * sWord);
					};
				}
			}
		}
	}
}