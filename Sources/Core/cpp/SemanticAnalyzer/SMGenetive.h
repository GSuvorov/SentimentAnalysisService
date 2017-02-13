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

			/// определение генетива
			class CSMGenetive :
				public CSMSentenceExpert
			{
			public:
				CSMGenetive(void);
				~CSMGenetive(void);
				bool AnalizeSentence();
			private:			
				/// устанавливает родительный падеж существительному
				/** 
				\param pUnit - существительное
				\return bool
				*/
				bool SetNounGenetiveCase(IUnit * pUnit);
				/// рекурсивный поиск генетива
				bool CheckGenetiveRecursive(IUnit * pUnit);
				/// обработка родительного падежа
				bool GenetiveProcess(IUnit * pUnit);
			};

			}
		}
	}
}