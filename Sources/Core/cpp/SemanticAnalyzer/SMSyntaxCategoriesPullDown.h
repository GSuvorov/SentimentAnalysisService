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
				/// перевод коллекции категорий в синтаксические индексы
				class CSMSyntaxCategoriesPullDown :
					public CSMSentenceExpert
				{
				public:
					CSMSyntaxCategoriesPullDown(void);
					~CSMSyntaxCategoriesPullDown(void);
					bool AnalizeSentence();
				private:
					/// текущее значение
					UINT m_uiValue;
					/// текущий номер бита
					UINT m_uiCurrentBitNumber;
					/// первый идекс из коллекции
					UINT GetFirstCategoryValue();
					/// очередной индекс коллекции
					UINT GetNextCategoryValue();

					bool IsCategory(UINT uiSingleValue);
				};
			}
		}
	}
}