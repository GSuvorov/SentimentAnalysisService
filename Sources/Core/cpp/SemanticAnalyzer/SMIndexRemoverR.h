#pragma once
#include "SMSentenceExpert.h"

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
					/// удаляет лишние индексы
					class CSMIndexRemoverR :
						public CSMSentenceExpert
					{
					public:
						CSMIndexRemoverR(void);
						~CSMIndexRemoverR(void);
						bool AnalizeSentence();
						bool Init(IText * pIText);
					private:
						void RemoveSubjectObjectIndices(IUnit * pUnit);
						void RemoveSemanticIndices(IUnit * pUnit);
						bool RemoveSemanticFromUnits(IUnit * pGroup);
						bool RemoveSyntIndexFromUnits(IUnit * pGroup,const Types::SyntaxCategories & oCategory);
						void RemoveSubjObjFromUnit(IUnit * pUnit);
						UINT m_uiObject;
						UINT m_uiSubject;

					};
				}
			}
		}


	}
}