#pragma once
#include "./smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{

			/// базовый класс для определения обстоятельств места , времени
			class CSMAdverbialModifier :
				public CSMSentenceExpert
			{
			public:
				CSMAdverbialModifier(void);
				~CSMAdverbialModifier(void);
			protected:

				/// объединение группы юнитов, выделение синтаксической категории
				/** 
					\param l_Unit - группа
					\param pPrep - предлог
					\param bTime - истна - категории времени, ложь - места
					\return 				bool - истина - если объединение удачно
				*/
				bool ProcessPreposition(TBoxUnits &  l_Unit , IUnit * pPrep, bool bTime);
			};

		}
	}
}