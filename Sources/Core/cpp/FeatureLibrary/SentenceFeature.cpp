#include "StdAfx.h"
#include ".\sentencefeature.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			CSentenceFeature::CSentenceFeature(void)
				: m_SentenceType(Values::SentenceType.Undefined)
			{
				Init();
			}

			CSentenceFeature::~CSentenceFeature(void)
			{
			}
			void CSentenceFeature::Init()
			{
				m_FeatureCollectionType = fctSentenceFeature;

				AddFeatureToCollection(&m_SentenceType);
			};
		}
	}
}