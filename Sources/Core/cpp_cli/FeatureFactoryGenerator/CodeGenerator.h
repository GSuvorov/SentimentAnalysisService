#pragma once

#include "FeatureStorage.h"

namespace FeatureFactoryGenerator
{
	class CCodeGenerator
	{
	public:
		CCodeGenerator(void);
		~CCodeGenerator(void);

		///сгенерирует код в стрингу по переданному хранилищу характеристик
		System::String* Generate(CFeatureStorage* pFeatureStorage);
		///сгенерирует код в стрингу по переданной группе характеристик
		System::String* Generate(CFeatureGroup* pFeatureGroup);
		///сгенерирует код в стрингу по переданной характеристике
		System::String* Generate(CFeature* pFeature);

		///сгенерирует код в стрингу по переданной характеристике
		System::String* GenerateMap(CFeature* pFeature);
		///сгенерирует код в стрингу по переданной характеристике
		System::String* GenerateInteger(CFeature* pFeature);
		///сгенерирует код в стрингу по переданной характеристике
		System::String* GeneratePercent(CFeature* pFeature);
	};
};