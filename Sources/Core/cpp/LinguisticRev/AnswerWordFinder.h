#pragma once

#include "..\ASSInterface\ICommon.h"
#include "..\ASSInterface\IBlackBoxUnit.h"
#include "..\ASSInterface\IBlackBoxQuery.h"

using namespace SS::Interface::Core::BlackBox::Query;
using namespace SS::Interface::Core::BlackBox;

#include "..\ASSInterface\Iterators.h"
using namespace SS::Iterators;

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

#include "AnswerWordInterval.h"


namespace SS
{
namespace LinguisticRev
{

class CAnswerWordFinder : 
		public CBaseCoreClass,
		public CAnalyseParams
{
public:
	CAnswerWordFinder();
	~CAnswerWordFinder();

public:
	virtual void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
	void Init(ISentence* pSent);
	/// searches for pTransform's answer word in the interval found
	bool FindAnswerWord(ITransformation* pTransform, ECompareMode eMode, bool bSequence);
	/// returns the maximal weight of all answer words' indexes
	float GetAnswerWordWeight(ITransformation* pTransform);

private:
	/// finds query word in a transformation
	IQueryUnit* GetQueryWord(ITransformation* pTrans);
	/// compares units according to the compare mode
	bool Compare(IUnit* pUnit, IQueryUnit* pQueryUnit, ECompareMode eMode);
	/// checks, if pUnit's subtree contains hltQueryWord units
	bool IsSubtreeHighlighted(IUnit* pUnit);

	void RecalcTimeWeight(IUnit* pUnit, float* pfWeight);

private:
	CAnswerWordInterval m_oInterval;
	ISentence* m_pSent;

	CSemanticFeature* m_pSemanticFeature;
	IAMConverterSemantic* m_pSemanticConverter;
};

}
}
