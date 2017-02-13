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

class CAnswerWordInterval
{
public:
	CAnswerWordInterval();
	~CAnswerWordInterval();

public:
	/// calculates interval where answer word must be located
	bool Init(ISentence* pSent, ITransformation* pTransform, bool bSequence);
	/// returns answer word query unit
	IQueryUnit* GetAnswerUnit();
	/// returns answer word position in query
	int GetTransPosition();

	/// returns the first unit of interval
	IUnit* GetFirstUnit();
	/// returns the next unit of interval
	IUnit* GetNextUnit();

private:
	/// searches for the first unit with correspondent trans-position
	CLeafIterator GetFirstUnit(int iPos);
	/// searches for the first unit with correspondent trans-position
	CLeafIterator GetLastUnit(int iPos);

private:
	ISentence* m_pSent;
	ITransformation* m_pTransform;

	IQueryUnit* m_pAnswerUnit;
	int m_iTransPosition;

	CLeafIterator m_itBegin;
	CLeafIterator m_itPos;
	CLeafIterator m_itEnd;
};

}
}
