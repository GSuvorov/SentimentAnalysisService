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

				/// выделение обстоятельст причны и способа, базовый класс
				class CSMCauseManner :
					public CSMSentenceExpert
				{
				public:
					CSMCauseManner(void);
					~CSMCauseManner(void);
				protected:
					/// добавление тек слову категории способа
					void AddMannerCategory();
					/// добавление тек слову категории причины
					void AddCauseCategory();
					/// истина, если в индексе есть основа
					/**  
					\param pIndex - индекс 
					\param uiSourceIndex - основа
					*/
					bool IsIndexExist(IIndex * pIndex, UINT uiSourceIndex);

				};


			}
		}
	}
}