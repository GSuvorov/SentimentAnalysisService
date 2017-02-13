#pragma once

#include ".\basefeaturecollection.h"
#include "define.h"
namespace SS
{
	namespace Core
	{
		namespace Features
		{
			/// характеристики предложения
			class CSentenceFeature :
				public SS::Core::Features::CBaseFeatureCollection
			{
			public:
				CSentenceFeature(void);
				~CSentenceFeature(void);
				CSentenceFeature& operator = (const CSentenceFeature& value)
				{
					Assign(&value);
					return *this;
				}
				CSentenceFeature(const CSentenceFeature& value){
					Init();
					*this = value;
				};

				///вернет строковое имя характеристики
				const char* GetFeatureName() const {return "SentenceFeature";};
				///вернет строковое описание характеристики
				const char* GetFeatureDescription() const {return "характеристики предложения";};

				/// тип предложения
				Types::SentenceType		m_SentenceType;
			private:
				void Init();
			};
		}
	}
}