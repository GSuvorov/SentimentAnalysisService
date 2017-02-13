#pragma once
#include "smadverbialmodifier.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Russian
			{
/// выделение в тексте обстоятельств места и времени по семантике 
class CSMAdverbialModifierR :
	public CSMAdverbialModifier
{
public:
	CSMAdverbialModifierR(void);
	~CSMAdverbialModifierR(void);
	bool AnalizeSentence();
protected:
	/// добавляет зависимые слово к главному
	void AddGenetiveRussian(TBoxUnits & l_Units);
};


			}
		}
	}
}