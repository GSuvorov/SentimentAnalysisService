#include "StdAfx.h"
#include ".\semanticfeatureapriori.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			CSemanticFeatureApriori::CSemanticFeatureApriori(void)
				: m_Uniqueness(Values::Uniqueness.unqNotUnique)
				, m_ExistenceMod(Values::ExistenceMod.Undefined)
			{
				Init();
			}

			CSemanticFeatureApriori::~CSemanticFeatureApriori(void)
			{
			}

			void CSemanticFeatureApriori::Init()
			{
				m_FeatureCollectionType = Base::fctSemanticFeatureApriori;

				AddFeatureToCollection(&m_Uniqueness);
				AddFeatureToCollection(&m_ExistenceMod);
			};
			
		}
	}
}