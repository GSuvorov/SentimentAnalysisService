#include "StdAfx.h"
#include ".\morphofeatureaposteriori.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			CMorphoFeatureAposteriori::CMorphoFeatureAposteriori(void)
				: m_OperationMod(Values::OperationMod.Undefined)
			{
				Init();
			}

			CMorphoFeatureAposteriori::~CMorphoFeatureAposteriori(void)
			{
			}

			void CMorphoFeatureAposteriori::Init()
			{
				m_FeatureCollectionType = Base::fctMorphoFeatureAposteriori;

				AddFeatureToCollection(&m_OperationMod);
			};
		}
	}
}