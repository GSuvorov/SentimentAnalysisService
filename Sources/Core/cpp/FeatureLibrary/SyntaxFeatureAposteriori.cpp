#include "StdAfx.h"
#include ".\syntaxfeatureaposteriori.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			CSyntaxFeatureAposteriori::CSyntaxFeatureAposteriori(void)
				: m_SyntaxCategories(Values::SyntaxCategories.Undefined)
				, m_SyntaxCategoryPower(0)
				, m_IdParent(0)
				, m_DependencyPower(0)
			{
				Init();
			}

			CSyntaxFeatureAposteriori::~CSyntaxFeatureAposteriori(void)
			{
			}
			void CSyntaxFeatureAposteriori::Init()
			{
				m_FeatureCollectionType = Base::fctSyntaxFeatureAposteriori;

				AddFeatureToCollection(&m_SyntaxCategories);
				AddFeatureToCollection(&m_SyntaxCategoryPower);
				AddFeatureToCollection(&m_IdParent);
				AddFeatureToCollection(&m_DependencyPower);
			};
		}
	}
}