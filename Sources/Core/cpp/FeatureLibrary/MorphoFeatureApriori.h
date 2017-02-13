#pragma once
#include "morphofeature.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			///набор морфологических характеристик дл€ детектировани€ объекта конструкции
			class CMorphoFeatureApriori :
				public CMorphoFeature
			{
			public:
				CMorphoFeatureApriori(void);
				~CMorphoFeatureApriori(void);
				CMorphoFeatureApriori& operator = (const CMorphoFeatureApriori& value)
				{
					Assign(&value);
					return *this;
				}
				CMorphoFeatureApriori(const CMorphoFeatureApriori& value){
					Init();
					*this = value;
				};

				///вернет строковое им€ характеристики
				const char* GetFeatureName() const {return "MorphoFeatureApriori";};
				///вернет строковое описание характеристики
				const char* GetFeatureDescription() const {return "набор морфологических характеристик дл€ детектировани€ объекта конструкции";};

			//ƒополнительные характеристики
				/// уникалькальность данной морфологической категории
				Types::Uniqueness					m_Uniqueness;
				/// ƒолжна ли данна€ характеристика присутствовать в предложении или	должна отсутствовать
				Types::ExistenceMod				m_ExistenceMod;
				/// режим анализа морфологических характеристик
				Types::TDetectWordAnalyzeMode		m_DetectWordAnalyzeMode;
			private:
				void Init();
			};

		}
	}
}