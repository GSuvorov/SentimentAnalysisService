#include "StdAfx.h"
#include ".\constructionfeature.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			namespace Base
			{
				CConstructionFeature::CConstructionFeature(void)
					: m_SentenceType(Values::SentenceType.Undefined)
					//, m_ChildrenAnalysisDepth(Values::ChildrenAnalysisDepth.Undefined)
				{
					Init();
				}

				CConstructionFeature::~CConstructionFeature(void)
				{
				}
				void CConstructionFeature::Init()
				{
					m_FeatureCollectionType = fctSentenceFeature;

					AddFeatureToCollection(&m_SentenceType);
					//AddFeatureToCollection(&m_ChildrenAnalysisDepth);
				};
			}
		}
	}
}