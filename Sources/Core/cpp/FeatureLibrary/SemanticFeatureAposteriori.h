#pragma once
#include "semanticfeature.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			///набор морфологических характеристик, для принятия решения
			class CSemanticFeatureAposteriori :
				public CSemanticFeature
			{
			public:
				CSemanticFeatureAposteriori(void);
				~CSemanticFeatureAposteriori(void);
				CSemanticFeatureAposteriori& operator = (const CSemanticFeatureAposteriori& value)
				{
					Assign(&value);
					return *this;
				}
				CSemanticFeatureAposteriori(const CSemanticFeatureAposteriori& value){
					Init();
					*this = value;
				};

				///вернет строковое имя характеристики
				const char* GetFeatureName() const {return "SemanticFeatureAposteriori";};
				///вернет строковое описание характеристики
				const char* GetFeatureDescription() const {return "набор семантических характеристик, для принятия решения";};

				///ДОПОЛНИТЕЛЬНЫЕ ХАРАКТЕРИСТИКИ
				///Выполняемая операция над текущей морфологической характеристикой
				Types::OperationMod	m_OperationMod;
				///Номер элемента от которого необходимо унаследовать семантику
				Types::TShiftSemantic m_ShiftSemantic;
				///Значение по умолчанию для ShiftSemantic
				enum { DEF_SHIFT = 1000 };

			private:
				void Init();
			};
		}
	}
}