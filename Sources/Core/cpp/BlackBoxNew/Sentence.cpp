#include "stdafx.h"
#include "Sentence.h"

#include "Unit.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CSentence::CSentence() :
		m_uiRelevance(0),
		m_eLanguage(ealUndefined),
		//m_bEnabled(true),
		//m_nTest(33),
		m_nEnabled(1),
		m_nDigest(0),
		m_nPreDigest(0)
{
	m_nTonePos = 0;
	m_nToneNeg = 0;
	m_pPrevSent = NULL;
	m_strMarkers = L"";
	m_processed = eProcessed::None;
}

CSentence::~CSentence()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// ISentence interface

void CSentence::AddUnit(IUnit* pUnit)
{
	AddChild(dynamic_cast<CUnit*>(pUnit));
}

ISentence* CSentence::GetNextSentence()
{
	return dynamic_cast<CSentence*>(GetRight());
}

ISentence* CSentence::GetPrevSentence()
{
	return dynamic_cast<CSentence*>(GetLeft());
}

IUnit* CSentence::GetFirstUnit()
{
	return dynamic_cast<CUnit*>(GetFirstChild());
}

IUnit* CSentence::GetLastUnit()
{
	return dynamic_cast<CUnit*>(GetLastChild());
}

void CSentence::ClearSentence()
{
	m_uiRelevance = 0;
	m_eLanguage = ealUndefined;

	ReleaseChildren();
}

ISentence* CSentence::ReleaseSentence()
{
	return dynamic_cast<CSentence*>(ReleaseItem());
}

void CSentence::SetLanguage(EAnalyseLanguage eLanguage)
{
	m_eLanguage = eLanguage;
}

EAnalyseLanguage CSentence::GetLanguage()
{
	return m_eLanguage;
}

void CSentence::SetRelevance(unsigned int uiRelevance)
{
	m_uiRelevance = uiRelevance;
}

unsigned int CSentence::GetRelevance()
{
	return m_uiRelevance;
}

//bool CSentence::isEnabled()
//{
//	return m_bEnabled;
//}
//
//void CSentence::SetEnabled(bool bEnabled)
//{
//	m_bEnabled = bEnabled;
//}
//
int CSentence::isEnabled()
{
	return m_nEnabled;
}

void CSentence::SetEnabled(int nEnabled)
{
	m_nEnabled = nEnabled;
}

int CSentence::IsDigest()
{
	return m_nDigest;
}

void CSentence::SetDigest(int val)
{
	m_nDigest = val;
}

int CSentence::IsPreDigest()
{ 
	return m_nPreDigest;
}

void CSentence::SetPreDigest(int val)
{
	m_nPreDigest = val;
}

int  CSentence::GetToneNeg()
{
	return m_nToneNeg;
}

 void  CSentence::SetToneNeg(int val)
{
	m_nToneNeg = val;
}

int CSentence::GetTonePos()
{
	return m_nTonePos;
}

void  CSentence::SetTonePos(int val)
{
	m_nTonePos = val;
}





bool SortByPosInSent(IUnit* u1, IUnit* u2)
{
	return u1->GetFirstCharPosition() < u2->GetFirstCharPosition();
}

bool SortByPosInSentFull(IUnit* u1, IUnit* u2)
{
	return u1->GetNumber() < u2->GetNumber();
}


void CSentence::GetLinearUnits(list<IUnit*>& allUnits)
{	
	IUnit* pUnit = GetFirstUnit();
	while(pUnit)
	{
		allUnits.push_back(pUnit);
		wstring str = pUnit->GetWord();	
		int off = pUnit->GetFirstCharPosition();
		AddChildUnits(pUnit, allUnits);
		pUnit = pUnit->GetRightUnit();
	}

	allUnits.sort(SortByPosInSent);
} 

void CSentence::GetLinearUnitsNonEmpty(list<IUnit*>& allUnits)
{	
	IUnit* pUnit = GetFirstUnit();
	while(pUnit)
	{
		wstring str = pUnit->GetWord();	
		if (str.size()>0)
		  allUnits.push_back(pUnit);		
		int off = pUnit->GetFirstCharPosition();
		AddChildUnits(pUnit, allUnits, false);
		pUnit = pUnit->GetRightUnit();
	}

	allUnits.sort(SortByPosInSent);
} 



void  CSentence::GetLinearUnitsFull(list<IUnit*>& allUnits)
{
	IUnit* pUnit = GetFirstUnit();	
	while(pUnit)
	{
		allUnits.push_back(pUnit);
		wstring str = pUnit->GetWord();	
		int off = pUnit->GetFirstCharPositionNew();
		AddChildUnits(pUnit, allUnits);
		pUnit = pUnit->GetRightUnit();
	}

	allUnits.sort(SortByPosInSentFull);
}


void CSentence::AddChildUnits(IUnit* unit, list<IUnit*>& findedUnits, bool isEmpty)
{
	IUnit* pChildUnit = unit->GetLeftChildUnit();
	while(pChildUnit)
	{
		AddChildUnits(pChildUnit, findedUnits);
		wstring str = pChildUnit->GetWord();
		if (!str.empty() || isEmpty)
			findedUnits.push_back(pChildUnit);

		pChildUnit = pChildUnit->GetRightUnit();
	}
}

ISentence* CSentence::GetPrevSentenceLinear()
{
	return m_pPrevSent;
}

void CSentence::SetPrevSentenceLinear(ISentence* val)
{
	m_pPrevSent = val;
}

wstring CSentence::GetMarkers()
{
	return m_strMarkers;
}

void CSentence::SetMarkers(wstring val)
{
	m_strMarkers = val;
}

eProcessed CSentence::GetProcessed()
{
	return m_processed;
}

void CSentence::SetProcessed(eProcessed val)
{
	m_processed = val;
}

}
}
}
