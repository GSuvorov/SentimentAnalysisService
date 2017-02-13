#pragma once
#include "smadverbialmodifier.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace English
			{
				/// обстоятельства места и времени в английском
				class CSMAdverbialModifierE :
					public CSMAdverbialModifier
				{
				public:
					CSMAdverbialModifierE(void);
					~CSMAdverbialModifierE(void);
					bool AnalizeSentence();
				private:
					/// добавляет зависимые слово к главному
					void AddPlaceUnit(TBoxUnits & l_Units);
				};
			}
		}
	}
}
