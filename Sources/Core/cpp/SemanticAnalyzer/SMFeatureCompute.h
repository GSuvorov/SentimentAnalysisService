#pragma once
#include "./smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{

			/// вычисляет характеристики юнита
			class CSMFeatureCompute :
				public CSMSentenceExpert
			{
			public:
				CSMFeatureCompute(void);
				~CSMFeatureCompute(void);

				bool AnalizeSentence();
				bool Init(IText * pIText);

			private:
				/// естина, если текущее слово - much,many
				bool m_bMuchMany;
				/// семантика 
				//long m_semAgent;
				long m_semLastName;
				long m_semFirstName;
				long m_semPlaceIndf;
				/// обработка характеристик английского 
				void ProcessEngFeature(SS::Core::Features::CMorphoFeature & oMorphoFeature);
				/// обработка характеристик русского 
				void ProcessRusFeature(SS::Core::Features::CMorphoFeature & oMorphoFeature);
				void Up2SymbolNextLastName();
			};


		}
	}
}