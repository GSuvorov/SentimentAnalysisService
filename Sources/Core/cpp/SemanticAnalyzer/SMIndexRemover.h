#pragma once
#include "SMSentenceExpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace PostAnalysisExperts
		{
			namespace PostSyntaxAnalysis
			{
				/// удаляет лишние индексы
				class CSMIndexRemover :
					public CSMSentenceExpert
				{
				public:
					CSMIndexRemover(void);
					~CSMIndexRemover(void);
					bool AnalizeSentence();
					bool Init(IText * pIText);
				private:
					void RemoveSubjectObjectIndices(IUnit * pUnit);
					void RemoveSemanticIndices(IUnit * pUnit);
					bool RemoveSemanticFromUnits(IUnit * pGroup);
					bool RemoveSyntIndexFromUnits(IUnit * pGroup,const Types::SyntaxCategories & oCategory);
					UINT m_uiObject;
					UINT m_uiSubject;

				};
			}
		}


	}
}