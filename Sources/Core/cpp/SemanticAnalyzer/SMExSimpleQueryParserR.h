#pragma once
#include ".\SMExSimpleQueryParser.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Russian
			{
				///объединяет придаточные предложения под одним общим юнитом и выставляет ему Ajunct 
				class CSMExSimpleQueryParserR :
					public CSMExSimpleQueryParser
				{
				public:
					CSMExSimpleQueryParserR(void);
					~CSMExSimpleQueryParserR(void);

					bool Init(IText * pIText);

				};


			}

		}
	}
}