#include "StdAfx.h"
#include ".\Objectfeature.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			CObjectFeature::CObjectFeature(void)
				: m_ObjectsOrder(Values::ObjectsOrder.ooDiscontinuous)
				, m_PositionToDetectedObject(Values::PositionToDetectedObject.Undefined)
				, m_ObjectObligation(Values::ObjectObligation.ecoNotObligatory)
				, m_SameTypeObjectsNumber(0)
				, m_ScopeOfTheRule(Values::ScopeOfTheRule.Undefined)
				, m_PositionInBlock(Values::PositionInBlock.Undefined)
				, m_AgreementType(Values::AgreementType.Undefined)
				, m_ChildrenAnalysisDepth(Values::ChildrenAnalysisDepth.Undefined)
				, m_SyntaxParamsInheritanceMode(Values::SyntaxParamsInheritanceMode.Undefined)
				, m_SentenceNumber(0)
				, m_BlockPosition(0)
				, m_BlockSearchScope(0)
				, m_CreateUnit(Values::CreateUnit.cuDetect)
				, m_MoveType(Values::MoveType.mtDoNotMove)
			{
				Init();
			}

			CObjectFeature::~CObjectFeature(void)
			{
			}

			void CObjectFeature::Init()
			{
				m_FeatureCollectionType = Base::fctConstructionFeature;

				AddFeatureToCollection(&m_ObjectsOrder);
				AddFeatureToCollection(&m_PositionToDetectedObject);
				AddFeatureToCollection(&m_ObjectObligation);
				AddFeatureToCollection(&m_SameTypeObjectsNumber);
				AddFeatureToCollection(&m_ScopeOfTheRule);
				AddFeatureToCollection(&m_PositionInBlock);
				AddFeatureToCollection(&m_AgreementType);	
				AddFeatureToCollection(&m_ChildrenAnalysisDepth);	
				AddFeatureToCollection(&m_SyntaxParamsInheritanceMode);
				AddFeatureToCollection(&m_SentenceNumber);
				AddFeatureToCollection(&m_BlockPosition);
				AddFeatureToCollection(&m_BlockSearchScope);
				AddFeatureToCollection(&m_CreateUnit);
				AddFeatureToCollection(&m_MoveType);
			};

		}
	}
}