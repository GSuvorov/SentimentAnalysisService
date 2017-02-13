#include "stdafx.h"
#include "Unit.h"

#include "Index.h"

#include "..\ASSInterface\Iterators.h"
#include <locale>
using namespace SS::Iterators;




namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CUnit::CUnit() :
		m_pSyntaxFeature(NULL),
		m_uiPosition(UINT_MAX),
		m_uiFirstCharPosition(UINT_MAX),
		m_uiLastCharPosition(-1),
		m_pInfo(NULL)//,
		//m_oIndexParent(this)
{
	SetDefaultValues();
}

CUnit::~CUnit()
{
	delete m_pSyntaxFeature;
	delete m_pInfo;
	//if (pCitata!=NULL)
	//	delete pCitata;
}

/////////////////////////////////////////////////////////////////////////////////////
// IUnit interface

void CUnit::SetTransPosition(unsigned char uchTransPosition)
{
	m_uchTransPosition = uchTransPosition;
}

unsigned char CUnit::GetTransPosition()
{
	return m_uchTransPosition;
}

void CUnit::SetPosition(unsigned int uiPosition)
{
	m_uiPosition = uiPosition;
}

unsigned int CUnit::GetPosition()				
{
	if (m_uiPosition == -1)
	{
		IUnit* unit = GetLeftChildUnit();
		while(unit)
		{
			if (unit->GetPosition() != -1)
                return unit->GetPosition();
			unit = unit->GetLeftChildUnit();
		}		
	}
	return m_uiPosition;
}

unsigned int CUnit::GetFirstCharPosition()
{
	return m_uiFirstCharPosition;
}

void CUnit::SetFirstCharPosition(unsigned int uiPosition)
{
	m_uiFirstCharPosition = uiPosition;
}

EHighlightType CUnit::GetHighlightType()
{
	return m_eHighlight;
}

void CUnit::SetHighlightType(EHighlightType hlt)
{
	m_eHighlight = hlt;
}

bool CUnit::GetIsInAnswerSentence()
{
	return m_bIsInAnswerSentence;
}

void CUnit::SetIsInAnswerSentence(bool bIn)
{
	m_bIsInAnswerSentence = bIn;
}

const wchar_t* CUnit::GetWord()
{
	return m_strWord.c_str();
}

void CUnit::SetWord(const wchar_t* wcWord)
{
	m_strWord = wcWord;
}

void CUnit::SetSyntaxFlag(bool bl)
{
	m_bSyntaxFlag = bl;
}

bool CUnit::GetSyntaxFlag()
{
	return m_bSyntaxFlag;
}

ISyntaxFeature* CUnit::GetSyntaxFeature()
{
	return m_pSyntaxFeature;
}

ISyntaxFeature* CUnit::CreateSyntaxFeature()
{
	if(!m_pSyntaxFeature)
		m_pSyntaxFeature = new CSyntaxFeature();
	return m_pSyntaxFeature;
}

void CUnit::AddIndex(IIndex* pIndex)
{
	m_oIndexParent.AddChild(dynamic_cast<CIndex*>(pIndex));
}

/*void CUnit::AddSemanticIndex(IIndex* pMorphoIndex, IIndex* pSemanticIndex)
{
	AddIndex(pSemanticIndex);

	TMorphoSemanticMap::iterator it;
	pair<TMorphoSemanticMap::iterator, bool> pr;

	it = m_MorphoSemanticMap.find(pMorphoIndex);
	if(it == m_MorphoSemanticMap.end())
	{
		pr = m_MorphoSemanticMap.insert(TMorphoSemanticMap::value_type(pMorphoIndex, 0));
		if(pr.second)
		{
			pr.first->second.push_back(pSemanticIndex);
		}
	}
	else
	{
		it->second.push_back(pSemanticIndex);
	}
}

void CUnit::OnPopItem(CLeafItem* pItem)
{
	for(TMorphoSemanticMap::iterator it = m_MorphoSemanticMap.begin(); it != m_MorphoSemanticMap.end(); it++)
	{
		if(pItem == (CLeafItem*)(dynamic_cast<CIndex*>(it->first)))
		{
			for(int i=0; i < it->second.size(); i++)
			{
				IIndex* pSementic = it->second[i];
				((CLeafItem*)(dynamic_cast<CIndex*>(pSementic)))->ReleaseItem();
			}
				m_MorphoSemanticMap.erase(it);
			break;
		}
	}
}*/

IIndex* CUnit::GetFirstIndex()
{
	return dynamic_cast<CIndex*>(m_oIndexParent.GetFirstChild());
}

IIndex* CUnit::GetNextIndex()
{
	return dynamic_cast<CIndex*>(m_oIndexParent.GetNextChild());
}

void CUnit::AddUnitVertical(IUnit* pUnit)
{
	AddChild(dynamic_cast<CUnit*>(pUnit));
}

void CUnit::AddUnitHorizontal(IUnit* pUnit)
{
	InsertItem(dynamic_cast<CUnit*>(pUnit));
}

void CUnit::InsertUnitVertical(IUnit* pUnit)
{
	CMiddleItem* pItem = dynamic_cast<CUnit*>(pUnit);
	InsertItem(pItem);
	PopItem();
	pItem->AddChild(this);
}

void CUnit::JoinUpInterval(IUnit* pLeft, IUnit* pRight)
{
	dynamic_cast<CUnit*>(pRight)->InsertItem(this);
	for(IUnit* pUnit = pLeft; pUnit != this; )
	{
		IUnit* pTemp = pUnit->Pop();
		AddChild(dynamic_cast<CUnit*>(pUnit));
		pUnit = pTemp;
	}
}

IUnit* CUnit::Pop()
{
	return dynamic_cast<CUnit*>(PopItem());
}

IUnit* CUnit::ReleaseUnitHorizontal()
{
	return dynamic_cast<CUnit*>(ReleaseItem());
}

IUnit* CUnit::ReleaseUnitVertical()
{
	CUnit* pParent = dynamic_cast<CUnit*>(GetParent());
	ReleaseItem();
	return pParent;
}

void CUnit::ClearUnit()
{
	ReleaseChildren();
	m_oIndexParent.ReleaseChildren();
	m_strWord.clear();
	ClearInfo();
	SetDefaultValues();

	delete m_pSyntaxFeature;
	m_pSyntaxFeature = NULL;
}

IUnit* CUnit::GetParentUnit()
{
	return dynamic_cast<CUnit*>(GetParent());
}

IUnit* CUnit::GetLeftChildUnit()
{
	return dynamic_cast<CUnit*>(GetFirstChild());
}

IUnit* CUnit::GetRightChildUnit()
{
	return dynamic_cast<CUnit*>(GetLastChild());
}

IUnit* CUnit::GetLeftUnit()
{
	return dynamic_cast<CUnit*>(GetLeft());
}

IUnit* CUnit::GetRightUnit()
{
	return dynamic_cast<CUnit*>(GetRight());
}

unsigned int CUnit::GetChildCount()
{
	int iCount = 0;
	for(IUnit* p = GetLeftChildUnit(); p; p = p->GetRightUnit())
		iCount++;
	return iCount;
}

bool CUnit::HasChilds()
{
	return GetLeftChildUnit() != NULL;
}

bool CUnit::HasIndexType(EIndexType oIndexType, bool bIsAnalyseChild)
{
	if(bIsAnalyseChild)
	{
		IUnit* pRightUnit = GetRightUnit();
		for(CSentenceIterator it = this; *it != pRightUnit; ++it)
		{
			if(HasIndexType(*it, oIndexType))
				return true;
		}
	}
	else
	{
		return HasIndexType(this, oIndexType);
	}

	return false;
}

SUnitInfo* CUnit::GetInfo()
{
	if(!m_pInfo)
		m_pInfo = new SUnitInfo;
	return m_pInfo;
}

void CUnit::ClearInfo()
{
	if(m_pInfo)
		m_pInfo->Clear();
}

/////////////////////////////////////////////////////////////////////////////////////
// CUnit functions

void CUnit::SetDefaultValues()
{
	m_eHighlight = hltNone;
	m_bIsInAnswerSentence = false;
	m_bSyntaxFlag = false;
	m_uchTransPosition = 0;
	m_nIsTargetObjectUnit = 0;
	pCitata = NULL;
	pReferenceUnit = NULL;
	m_nIsTargetObjectUnitFormChilds = 0;
	m_nInversion = 1;
	m_nTone = 0;
	m_nTermNumber = 0;
	m_nStopUnit = 0;
	m_nRealTone = 0;
	m_bRealInversion = false;
	m_bInverted = false;
	m_bRealToneCancel = false;
	m_bFreeForTone = true;
	m_bUnknownWord = false;
	m_strPatternName = L"";
	m_iHashKey = 0;
	m_bEvristicLN = false;
	m_nToneTermCount = 0;
	m_nNumber = 0;
	m_bNominalOnly = false;
//	nPossibleCitSubjectUnit = 0;
}

bool CUnit::HasIndexType(IUnit* pUnit, EIndexType oIndexType)
{
	for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{
		IDictionaryIndex* pDictInd = pIndex->GetDictionaryIndex();

		TDictionaryIndex oInd;
		for(bool b = pDictInd->GetFirst(&oInd); b; b = pDictInd->GetNext(&oInd))
		{
			EIndexType eit = oInd.GetIndexType();
			if(eit == oIndexType)
				return true;
		}
	}

	return false;
}



int CUnit::GetIsTargetObjectUnit()
{
//	if (pReferenceUnit!=NULL) return pReferenceUnit->GetIsTargetObjectUnit();
//	else
		return m_nIsTargetObjectUnit;
}

void CUnit::SetIsTargetObjectUnit(int nIsTargetObjectUnit)
{
	m_nIsTargetObjectUnit = nIsTargetObjectUnit;
}

unsigned int CUnit::GetFirstCharPositionNew()
{
	if (m_uiFirstCharPosition == -1)
	{
		IUnit* child = this->GetLeftChildUnit();
		if (child)
			return child->GetFirstCharPositionNew();
	}
	return m_uiFirstCharPosition;
}

int CUnit::GetLastCharPositionNew(const wchar_t *pText)
{
    wstring word = GetWord();
    int endPosition = m_uiLastCharPosition;
    if((word.length() > 0) && (m_uiLastCharPosition < 0))
    {
        unsigned int wordPos = 0;
        /// ÔÂ‚Û˛ ·ÛÍ‚Û Ò˜ËÚ‡˛ ‡ÔËÓË ÒÓ‚Ô‡‰‡˛˘ÂÈ
        for(endPosition = GetFirstCharPositionNew() + 1, wordPos = 1; wordPos < word.length(); ++endPosition)
        {
            if(pText[endPosition] == word[wordPos])
                ++wordPos;
        }
    }
    return endPosition;
}

void CUnit::SetCitation(TCitation *cit)
{
	pCitata = cit;
}

TCitation* CUnit::GetCitation()
{
	return pCitata;
}

bool CUnit::IsVisual()
{
	wstring strchild;

	IUnit* child = this->GetLeftChildUnit();
	while (child!=NULL)
	{
		wstring str = child->GetWordFull();	
		strchild+=str;
		child = child->GetRightUnit();							
	}

	if (!HasChilds() || (strchild!=m_strWord && m_strWord.size()>0))  return true;
	return false;
}

wstring CUnit::GetWordFull()
{
	wstring res;
	wstring strchild;

	IUnit* child = this->GetLeftChildUnit();
	while (child)
	{
		wstring str = child->GetWordFull();
		res += str;		
		strchild += str;
		child = child->GetRightUnit();					
		if (child)
            res += L" ";
	}

	if (strchild!=m_strWord && strchild.size()!=0)
		res = m_strWord + L" " + res;
	else
        res = m_strWord;

	return res;
	/*if (m_strWord.size()==0)
	{
		IUnit* child = this->GetLeftChildUnit();
		while (child!=NULL)
		{
			res += child->GetWordFull();
			res += L" ";
			child = child->GetRightUnit();			
		}
		return res;
	}else
		return m_strWord;*/
	//return res;
}

IUnit* CUnit::GetReferenceUnit()
{
	return pReferenceUnit;
}

void CUnit::SetReferenceUnit(IUnit* unit)
{
	pReferenceUnit = unit;
}

int CUnit::GetIsTargetObjectUnitFromParent()
{
	IUnit* parent = this->GetParentUnit();
	while(parent!=NULL)
	{
		if (parent->GetIsTargetObjectUnitFromChilds()==1) return 1;
		parent = parent->GetParentUnit();
	}
	return 0;
}


void CUnit::SetIsTargetObjectUnitFromChilds(int bIsTargetObjectUnit)
{
	m_nIsTargetObjectUnitFormChilds = bIsTargetObjectUnit;
}

int CUnit::GetIsTargetObjectUnitFromChilds()
{
	return m_nIsTargetObjectUnitFormChilds;
}

void CUnit::SetTone(int nTone)
{
	if (nTone!=0)
	 m_nTone = nTone;
}

void CUnit::SetToneStrong(int nTone)
{
	 m_nTone = nTone;
}

int CUnit::GetTone()
{
	return m_nTone;
}

void CUnit::SetRealTone(int nTone)
{
	//if (nTone!=0)
	if (!m_bRealToneCancel)
	{
	 m_nRealTone = nTone;
	 if (nTone==0)
	 {
		 m_bRealToneCancel = true;
	 }
	}
}

int CUnit::GetRealTone()
{
	return m_nRealTone;
}

void CUnit::SetRealInversion(bool bInv)
{
	m_bRealInversion = bInv;
}

bool CUnit::GetRealInversion()
{
	return m_bRealInversion;
}

void CUnit::SetInversion(int inv)
{
	m_nInversion = inv;
}


int CUnit::GetInversion()
{
	return m_nInversion;
}

void CUnit::SetInverted(bool bInv)
{
	m_bInverted = true;
	
}

bool CUnit::GetInverted()
{
	return m_bInverted;
}


void CUnit::SetTermNumber(int nTermNumber)
{
	m_nTermNumber = nTermNumber;
}

int CUnit::GetTermNumber()
{
	return m_nTermNumber;
}

void CUnit::SetStopWordUnit(int val)
{
	m_nStopUnit = val;
}

int CUnit::GetStopWordUnit()
{
	return m_nStopUnit;
}

void CUnit::SetToneTermCount(int val)
{
	m_nToneTermCount = val;
}

int CUnit::GetToneTermCount()
{
	return m_nToneTermCount;
}



void CUnit::SetUnitToneDict(wchar_t* val)
{
  m_unitToneDict = val;
}


wchar_t* CUnit::GetUnitToneDict()
{
	return (wchar_t*)m_unitToneDict.c_str();
}

bool CUnit::IsRusInitial()
{	
	wstring str = GetWord();
	wstring s1 = L".";
	/// ÒÌ‡˜‡Î‡ Ë‰ÛÚ ‡Ì„ÎËÈÒÍËÂ ·ÛÍ‚˚, Ì‡ÔËÒ‡ÌËÂÏ ÔÓıÓÊËÂ Ì‡ ÛÒÒÍËÂ
	wstring s0 = L"ABCEHKMOPTXY¿¡¬√ƒ≈®∆«» ÀÃÕŒœ–—“”‘’÷◊ÿŸ⁄€‹›ﬁﬂ";
	if (str.size()==2)
	{				
		int i1 = s1.find(str[1]);
		int i2 = s0.find(str[0]);
		if (i1>=0 && i2>=0)
		  return true;
	}
	return false;
}

bool CUnit::IsInitial()
{
	wstring str = GetWord();
	wstring s0 = L"¿¡¬√ƒ≈®∆«» ÀÃÕŒœ–—“”‘’÷◊ÿŸ⁄€‹›ﬁﬂABCDEFGHIJKLMNOPQRSTUVWXYZ";
	//std::isupper
	if (!str.empty() && str.size()<=3 && str.find_last_of(L".")==str.size()-1 && iswupper(str[0]))
		return true;
	return false;
}

void CUnit::SetFreeForTone(bool val)
{
  m_bFreeForTone = val;
}

bool CUnit::GetFreeForTone()
{
	return m_bFreeForTone;
}

void CUnit::SetUnknownWord(bool val)
{
	this->m_bUnknownWord = val;
}

bool CUnit::GetUnknownWord()
{
	return this->m_bUnknownWord;
}

void CUnit::SetPatternName(wstring val)
{
	m_strPatternName = val;
}

wstring CUnit::GetPatternName()
{
	return m_strPatternName;
}

void CUnit::SetHashKey(__int64 val)
{
	m_iHashKey = val;
}

__int64 CUnit::GetHashKey()
{		
	return m_iHashKey;
}

bool CUnit::IsEvristicAbbr()
{
	wstring str = GetWord();
	if (str.size() < 2)
		return false;
	int countLower = 0;
	int countDigit = 0;
	int count = 0;
	for(unsigned int i = 0; i < str.size(); ++i)
	{
		if (iswalnum(str[i]))
		{
			++count;
			if (iswdigit(str[i]))
				++countDigit;
			if (iswlower(str[i]))
				++countLower;
		}
	}
	if (count)
	{
		if (!countDigit && (!countLower || ((countLower == 1) && (str.size() > 3))))
			return true;

		if ((countLower < str.size() - countLower) && (str.size() < 6) && (countDigit < str.size() - countDigit))
			return true;
	}
	return false;
}


void CUnit::SetEvristicLN(bool val)
{
	m_bEvristicLN = val;
}

bool CUnit::GetEvristicLN()
{
	return m_bEvristicLN;
}

void CUnit::SetNumber(int val)
{
	m_nNumber = val;
}

int CUnit::GetNumber()
{
	return m_nNumber;
}

void  CUnit::SetIsNominalOnly(bool val)
{
	m_bNominalOnly = val;
}

bool CUnit::GetIsNominalOnly()
{
	return m_bNominalOnly;
}

void CUnit::AddUnitChild(IUnit *pUnit)
{
	m_Childs.push_back(pUnit);
}

std::list<IUnit*> CUnit::GetChildList()
{
	return m_Childs;
}


}
}
}
