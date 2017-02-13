#pragma once
#include "semanticfeature.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			///набор морфологических характеристик дл€ детектировани€ объекта конструкции
			class CSemanticFeatureApriori :
				public CSemanticFeature
			{
			public:
				CSemanticFeatureApriori(void);
				~CSemanticFeatureApriori(void);
				CSemanticFeatureApriori& operator = (const CSemanticFeatureApriori& value)
				{
					Assign(&value);
					return *this;
				}
				CSemanticFeatureApriori(const CSemanticFeatureApriori& value){
					Init();
					*this = value;
				};

				///вернет строковое им€ характеристики
				const char* GetFeatureName() const {return "SemanticFeatureApriori";};
				///вернет строковое описание характеристики
				const char* GetFeatureDescription() const {return "набор семантических характеристик дл€ детектировани€ объекта конструкции";};

			   //ƒополнительные характеристики
				/// уникалькальность данной морфологической категории
				Types::Uniqueness					m_Uniqueness;
				/// ƒолжна ли данна€ характеристика присутствовать в предложении или	должна отсутствовать
				Types::ExistenceMod				m_ExistenceMod;
			private:
				void Init();
			};

		}
	}
}