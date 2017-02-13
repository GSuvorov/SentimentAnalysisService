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
				/// выделение пассивных конструкций
				class CSMPassiveE :
					public CSMSentenceExpert
				{
				public:
					CSMPassiveE(void);
					~CSMPassiveE(void);
					bool AnalizeSentence();
					bool Init(IText * pIText);
				private:

					SS::Core::Features::CMorphoFeature m_MorphoFeature;
					/// индекс основы глагола to be
					UINT m_uiBeSourceIndex;
					/// истина, если слово - форма глагола "be"
					bool IsBeForm(IUnit * pUnit);

				};

			}
		}
	}
}