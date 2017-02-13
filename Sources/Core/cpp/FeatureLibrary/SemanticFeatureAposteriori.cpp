#include "StdAfx.h"
#include ".\semanticfeatureaposteriori.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			CSemanticFeatureAposteriori::CSemanticFeatureAposteriori(void)
				: m_OperationMod(Values::OperationMod.Undefined),
				  m_ShiftSemantic(DEF_SHIFT)
			{
				Init();
			}

			CSemanticFeatureAposteriori::~CSemanticFeatureAposteriori(void)
			{
			}

			void CSemanticFeatureAposteriori::Init()
			{
				m_FeatureCollectionType = Base::fctSemanticFeatureAposteriori;
				//Добавить в коллекцию
				AddFeatureToCollection(&m_OperationMod);
				AddFeatureToCollection(&m_ShiftSemantic);
			};
		}
	}
}