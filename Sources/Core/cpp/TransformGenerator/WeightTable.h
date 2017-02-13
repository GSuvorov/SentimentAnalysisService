#pragma once

#include "..\ASSInterface\IAMConverter.h"
using namespace SS::Interface::Core::AMConverter;

#include "..\FeatureLibrary\SemanticFeature.h"
using namespace SS::Core::Features;

#include "..\ASSInterface\IBlackBoxQuery.h"
using namespace SS::Interface::Core::BlackBox::Query;

#include "..\ASSInterface\IBlackBoxDictionaryIndex.h"
using namespace SS::Interface::Core::BlackBox;

#include "..\ASSInterface\TDictionaryManager.h"
using namespace SS::Dictionary::Types;

#include "..\ASCInterface\services.h"
using namespace SS::Core::CommonServices;

#include "..\FeatureLibrary\define.h"
using namespace SS::Core::Features::Types;


namespace SS
{
namespace TransformGenerator
{

class CWeightTable : public CBaseCoreClass
{
public:
	CWeightTable();
	~CWeightTable();

public:
	virtual void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
	void SetWeights(ITransformation* pTrans);

private:
	enum EQueryType
	{
		qtNone,
		qtWhoDefinition,
		qtNameDefinition,
		qtSubjectFin,
		qtAdjunctTime,
		qtAdjunctPlace,
	};

	IQueryUnit* GetQueryWord(ITransformation* pTrans);
	EQueryType GetQueryType(IQueryUnit* pUnit);
	bool IsSyntaxIndexInColl(IQueryUnitIndexCollection* pColl, const SyntaxCategories& sc);
	float GetWeight(IQueryUnitIndex* pIndex);

private:
	EQueryType m_eQueryType;
	CSemanticFeature* m_pSemanticFeature;
	IAMConverterSemantic* m_pSemanticConverter;
};

}
}
