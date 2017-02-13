#pragma once
#include "smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace IndexationExperts
			{
				class CSMExClauseTimePlace : public CSMSentenceExpert
				{
				public:
					CSMExClauseTimePlace(void);
					~CSMExClauseTimePlace(void);
					bool AnalizeSentence();
					bool Init(IText * pIText);
				protected:
					/// слова, что ищем в тексте  () 
					set<wstring> m_Words;
					/// определяет что слово найдено
					/** 
					\param sWord - слово 
					\return 		bool - истина, если найдено
					*/
					bool IsWord(const TCHAR * sWord);

					void SetSintaxCategoryToUnit(TBoxUnits l_units);
				};
			}
		}
	}
}

