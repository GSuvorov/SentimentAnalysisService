#pragma once

#include "..\ASSInterface\ICommon.h"
#include "..\ASSInterface\IBlackBoxUnit.h"
#include "..\ASSInterface\IBlackBoxQuery.h"

using namespace SS::Interface::Core::BlackBox::Query;
using namespace SS::Interface::Core::BlackBox;

#include "..\ASSInterface\Iterators.h"
using namespace SS::Iterators;


namespace SS
{
namespace LinguisticRev
{

class CSequenceComparer : public CAnalyseParams
{
public:
	CSequenceComparer();
	~CSequenceComparer();

public:
	void Init(ISentence* pSent, int iParagraphNumber, bool bSynonyms);
	/// marks all units in pSent, that are coincident with pTransform; returns false if there aren't such units
	bool HasCoincidences(ITransformation* pTransform, ECompareMode eMode, bool bSequence);
	/// highlights pSent's units, that coincide with pTransform by morphology
	bool HighlightKeywords(ITransformation* pTransform);

private:
	/// marks the part of pSent (starting with itSent), that coincides with m_pTransform; in itSent is returned the unit that follows that part
	bool FindCoincidence(CLeafIterator& itSent, IUnit* pFirstUnit);
	/// find units, that are coincident with pQueryUnit, starting with pStartUnit
	IUnit* FindCoincidentUnit(IUnit* pStartUnit, IQueryUnit* pQueryUnit);
	/// compares indexes of pUnit and pQueryUnit
	bool Compare(IUnit* pUnit, IQueryUnit* pQueryUnit);

private:
	ISentence* m_pSent;
	ITransformation* m_pTransform;

	ECompareMode m_eMode;
	bool m_bSynonyms;
	bool m_bSequence;
	int m_iParagraphNumber;

	enum ESearchResult
	{
		srFound,
		srSkipped,
		srNotFound,
	};

	ESearchResult m_eResult;
};

}
}
