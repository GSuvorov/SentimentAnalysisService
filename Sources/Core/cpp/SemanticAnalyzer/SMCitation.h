#pragma once
#include "./smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			/// выставление индексов цитате в тексте
			class CSMCitation :
				public CSMSentenceExpert
			{
			public:
				CSMCitation(void);
				~CSMCitation(void);
				bool AnalizeSentence();
				bool Init(IText * pIText);
			private:
				SS::Dictionary::Types::TDictionaryIndex m_uiSemanticCitation;
			};

		}
	}
}
