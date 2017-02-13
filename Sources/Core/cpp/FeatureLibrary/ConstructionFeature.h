#pragma once

#include ".\basefeaturecollection.h"
#include "define.h"
namespace SS
{
	namespace Core
	{
		namespace Features
		{
			namespace Base
			{
				/// характеристики предложения
				class CConstructionFeature : public SS::Core::Features::Base::CBaseFeatureCollection
				{
				public:
					CConstructionFeature(void);
					~CConstructionFeature(void);
					CConstructionFeature& operator = (const CConstructionFeature& value)
					{
						Assign(&value);
						return *this;
					}
					CConstructionFeature(const CConstructionFeature& value){
						Init();
						*this = value;
					};

					///вернет строковое имя характеристики
					const char* GetFeatureName() const {return "SentenceFeature";};
					///вернет строковое описание характеристики
					const char* GetFeatureDescription() const {return "характеристики предложения";};

					/// тип предложения
					Types::SentenceType		m_SentenceType;
					///// глубина анализа детей
					//Types::ChildrenAnalysisDepth	m_ChildrenAnalysisDepth;		// по роду, числу, падежу или числу, лицу
					
				private:
					void Init();
				};
			}
		}
	}
}