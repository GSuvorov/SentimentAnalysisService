#pragma once
#include "morphofeature.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			///набор морфологических характеристик, для принятия решения
			class CMorphoFeatureAposteriori :
				public CMorphoFeature
			{
			public:
				CMorphoFeatureAposteriori(void);
				~CMorphoFeatureAposteriori(void);
				CMorphoFeatureAposteriori& operator = (const CMorphoFeatureAposteriori& value)
				{
					Assign(&value);
					return *this;
				}
				CMorphoFeatureAposteriori(const CMorphoFeatureAposteriori& value){
					Init();
					*this = value;
				};

				///вернет строковое имя характеристики
				const char* GetFeatureName() const {return "MorphoFeatureAposteriori";};
				///вернет строковое описание характеристики
				const char* GetFeatureDescription() const {return "набор морфологических характеристик, для принятия решения";};

			//Дополнительные характеристики
				/// выполняемая операция над текущей морфологической характеристикой
				Types::OperationMod	m_OperationMod;
			private:
				void Init();
			};
		}
	}
}