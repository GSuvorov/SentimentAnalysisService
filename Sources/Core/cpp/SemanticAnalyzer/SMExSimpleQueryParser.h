#pragma once
#include "smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{

			class CSMExSimpleQueryParser : public CSMSentenceExpert
			{
			public:
				CSMExSimpleQueryParser(void);
				~CSMExSimpleQueryParser(void);
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
			};

		}
	}
}

