#pragma once
#include "./smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{

			/// выделение в тексте уточнений места типа "город, страна" или "страна, континент"
			class CSMPlaceSpecification :
				public CSMSentenceExpert
			{
			public:
				CSMPlaceSpecification(void);
				~CSMPlaceSpecification(void);
				bool AnalizeSentence();
			private:
				void PlaceProcess(TBoxUnits & l_Units);
			};

		}
	}
}