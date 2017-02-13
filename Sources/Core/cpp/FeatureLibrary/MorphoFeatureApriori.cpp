#include "StdAfx.h"
#include ".\morphofeatureapriori.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			CMorphoFeatureApriori::CMorphoFeatureApriori(void)
				: m_Uniqueness(Values::Uniqueness.unqNotUnique)
				, m_ExistenceMod(Values::ExistenceMod.Undefined)
				, m_DetectWordAnalyzeMode(Values::TDetectWordAnalyzeMode.Undefined)
			{
				Init();
			}

			CMorphoFeatureApriori::~CMorphoFeatureApriori(void)
			{
			}

			void CMorphoFeatureApriori::Init()
			{
				m_FeatureCollectionType = Base::fctMorphoFeatureApriori;

				AddFeatureToCollection(&m_Uniqueness);
				AddFeatureToCollection(&m_ExistenceMod);
				AddFeatureToCollection(&m_DetectWordAnalyzeMode);
			};
			
		}
	}
}