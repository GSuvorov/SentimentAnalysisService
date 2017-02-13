#include "stdafx.h"
#include "InternalRepresentation.h"

#include "IndexLoader.h"


namespace SS
{
namespace LexicalAnalyzer
{
namespace IR
{

/////////////////////////////////////////////////////////////////////////////////////
// CIndex class

class CIndex
{
public:
	CIndex(IIndex* pIndex, IBlackBoxTextStorage* pStorage) : 
		m_pIndex(pIndex), 
		m_pStorage(pStorage)
	{
	}

public:
	IIndex* Clone()
	{
		SS_TRY;

		IIndex* pIndex = m_pStorage->CreateIndex();
		pIndex->GetDictionaryIndex()->AppendIndex(m_pIndex->GetDictionaryIndex()->GetFirst());

		for (IFeature* pFeature = m_pIndex->GetFirstFeature(); pFeature; pFeature = m_pIndex->GetNextFeature())
		{
			IFeature* pNewFeature = m_pStorage->CreateFeature();
			pNewFeature->SetMorphoIndex(pFeature->GetMorphoIndex());
			pNewFeature->SetMorphoInfo(pFeature->GetMorphoInfo());
			pNewFeature->SetPartOfSpeech(pFeature->GetPartOfSpeech());

			pIndex->AddFeature(pNewFeature);
		}

		return pIndex;

		SS_CATCH(L"");
		return NULL;
	}

private:
	IIndex* m_pIndex;
	IBlackBoxTextStorage* m_pStorage;
};

/////////////////////////////////////////////////////////////////////////////////////
// CUnit

CUnit::CUnit(IUnit* pUnit) : 
	m_pUnit(pUnit),
	m_uiTag(0)
{
	for (int i = 0; i < IR_MODES_COUNT; i++)
	{
		m_rModes[i] = (EWordType) -1;
	}
}

void CUnit::Reset(bool bReleaseIUnit)
{
	if (bReleaseIUnit && m_pUnit)
	{
		m_pUnit->ReleaseUnitHorizontal();
	}
	m_uiTag = 0;
	m_pUnit = NULL;
	for (int i = 0; i < IR_MODES_COUNT; i++)
	{
		m_rModes[i] = (EWordType) -1;
	}
}

void CUnit::SetWordType(EWordType eType, EWalkerMode eMode, bool bReplace)
{
	if (bReplace || m_rModes[eMode] != -1)
	{
		m_rModes[eMode] = eType;
	}
}

EWordType CUnit::GetType(EWalkerMode eMode)
{
	if (m_rModes[eMode] != -1)
	{
		return m_rModes[eMode];
	}
	if (m_rModes[wmDefault] != -1)
	{
		return m_rModes[wmDefault];
	}
	assert(false);
	return wtUndefined;
}

EWordType CUnit::GetTypeStrict(EWalkerMode eMode)
{
	if (m_rModes[eMode] != -1)
	{
		return m_rModes[eMode];
	}
	return wtUndefined;
}

void CUnit::ClearMode(EWalkerMode eMode)
{
	m_rModes[eMode] = (EWordType) -1;
}

void CUnit::SetUnit(IUnit* pUnit)
{
	m_pUnit = pUnit;
}

IUnit* CUnit::GetUnit()
{
	return m_pUnit;
}

CUnit::operator IUnit*()
{
	return m_pUnit;
}

void CUnit::RemoveChildrenIndexes(EIndexType eitRemove)
{
	for (IUnit* pChildUnit = m_pUnit->GetLeftChildUnit(); pChildUnit; pChildUnit = pChildUnit->GetRightUnit())
	{
		for (IIndex* pIndex = pChildUnit->GetFirstIndex(); pIndex; )
		{
			EIndexType eit = pIndex->GetDictionaryIndex()->GetFirst().GetIndexType();
			if (eit == eitRemove)
			{
				pIndex = pIndex->ReleaseIndex();
			}
			else
			{
				pIndex = pChildUnit->GetNextIndex();
			}
		}
	}
}

void CUnit::RemoveIndexes(EIndexType eitRemove)
{
	SS_TRY;

	for (IIndex* pIndex = m_pUnit->GetFirstIndex(); pIndex; )
	{
		EIndexType eit = pIndex->GetDictionaryIndex()->GetFirst().GetIndexType();
		if (eit == eitRemove)
		{
			pIndex = pIndex->ReleaseIndex();
		}
		else
		{
			pIndex = m_pUnit->GetNextIndex();
		}
	}

	SS_CATCH(L"");
}

void CUnit::RemoveIndexes(DictionariesNumber ednRemove)
{
	SS_TRY;

	for (IIndex* pIndex = m_pUnit->GetFirstIndex(); pIndex; )
	{
		DictionariesNumber edn = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber();
		if (edn == ednRemove)
		{
			pIndex = pIndex->ReleaseIndex();
		}
		else
		{
			pIndex = m_pUnit->GetNextIndex();
		}
	}

	SS_CATCH(L"");
}


bool CUnit::HasIndexes(DictionariesNumber ednCompare)
{
	SS_TRY;

	for (IIndex* pIndex = m_pUnit->GetFirstIndex(); pIndex; pIndex = m_pUnit->GetNextIndex())
	{
		DictionariesNumber edn = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber();
		if (edn == ednCompare)
		{
			return true;
		}
	}

	SS_CATCH(L"");
	return false;
}

void CUnit::CopyUnit(IUnit* pUnit, IBlackBoxTextStorage* pStorage)
{
	SS_TRY;

	m_pUnit->SetWord(pUnit->GetWord());
	m_pUnit->SetPosition(pUnit->GetPosition());
	m_pUnit->SetFirstCharPosition(pUnit->GetFirstCharPosition());

	for (IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{
		CIndex oInd(pIndex, pStorage);
		m_pUnit->AddIndex(oInd.Clone());
	}

	SS_CATCH(L"");
}

bool CUnit::HaveCoincidentIndexes(IUnit* pUnit, EIndexType eit)
{
	SS_TRY;

	for (IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{
		if (pIndex->GetDictionaryIndex()->GetFirst().GetIndexType() != eit)
		{
			continue;
		}

		for (IIndex* pIndexCmp = m_pUnit->GetFirstIndex(); pIndexCmp; pIndexCmp = m_pUnit->GetNextIndex())
		{
			if (pIndex->GetDictionaryIndex()->GetFirst() == pIndexCmp->GetDictionaryIndex()->GetFirst())
			{
				return true;
			}
		}
	}

	SS_CATCH(L"");
	return false;
}

void CUnit::CopyWordTypes(CUnit* pFrom)
{
	SS_TRY;

	for (int i = 0; i < IR_MODES_COUNT; i++)
	{
		m_rModes[i] = pFrom->m_rModes[i];
	}

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// CSentence functions

CSentence::CSentence()
{
	m_oUnitStorage.Reserve(MAX_WORDS_IN_SENTENCE * 2);
	Reset();
}

CUnit* CSentence::GetFirstUnit()
{
	SS_TRY
	{
#ifdef DEBUG
		if (m_itTop._Getcont() == 0)
		{
			return (NULL);
		}
#endif
		m_it = m_lUnits.begin();
		if (m_it == m_itTop)
		{
			return (NULL);
		}
		return (*m_it);
	}
	SS_CATCH(L"");

	return (NULL);
}

CUnit* CSentence::GetNextUnit()
{
	SS_TRY
	{
#ifdef DEBUG
		++m_it;
		if (m_itTop._Getcont() == 0 || m_it == m_itTop)
		{
			return (NULL);
		}
		return (*m_it);
#else
		if (++m_it == m_itTop)
		{
			return (NULL);
		}
		return (*m_it);
#endif
	}
	SS_CATCH(L"");

	return (NULL);
}

CUnit* CSentence::GetNextUnitWithoutMove()
{
	SS_TRY;
	{
		TUnitList::iterator it = m_it;
#ifdef DEBUG
		if (m_itTop._Getcont() == 0 || ++it == m_itTop)
		{
			return (NULL);
		}
		return (*it);
#else		
		if (++it == m_itTop)
		{
			return (NULL);
		}
		return (*it);		
#endif
	}
	SS_CATCH(L"");

	return (NULL);
}

CUnit* CSentence::GetPrevUnit()
{
	SS_TRY
	{
		if (m_it == m_lUnits.begin())
		{
			return (NULL);
		}
		return *(--m_it);
	}
	SS_CATCH(L"");

	return (NULL);
}

CUnit* CSentence::GetLastUnit()
{
	SS_TRY;
	{
		if (m_lUnits.empty())
		{
			return (NULL);
		}

#ifdef DEBUG
		if (m_itTop._Getcont() == 0)
		{
			return (NULL);
		}
#endif

		m_it = m_itTop;
		return *(--m_it);
	}
	SS_CATCH(L"");
	
	return (NULL);
}

void CSentence::RemoveUnit(CUnit* pUnit)
{
	SS_TRY;
	{
#ifdef DEBUG
	  if (m_itTop._Getcont() != 0)
#endif
		for (TUnitList::iterator it = m_lUnits.begin(); it != m_itTop; ++it)
		{
			if (*it == pUnit)
			{
				m_oUnitStorage.Release(pUnit, true);
				m_lUnits.erase(it);
				m_iUnitCount--;
				return;
			}
		}
	}
	SS_CATCH(L"");
}

void CSentence::RemoveUnits(int nToLeft, int nCount)
{
	SS_TRY;

	TUnitList::iterator itLast = m_it;
	for (int i = 0; i < nToLeft - 1; i++)
	{
		--itLast;
	}

	TUnitList::iterator itFirst = itLast;
	for (int i = 0; i < nCount; i++)
	{
		m_oUnitStorage.Release(*--itFirst, true);
	}

	m_lUnits.erase(itFirst, itLast);
	m_iUnitCount -= nCount;

	SS_CATCH(L"");
}

void CSentence::RemoveUnits(EWordType eType)
{
	SS_TRY;
	{
#ifdef DEBUG
	  if (m_itTop._Getcont() != 0)
#endif
		for (TUnitList::iterator it = m_lUnits.begin(); it != m_itTop; )
		{
			if ((*it)->GetType(wmDefault) == eType)
			{
				m_oUnitStorage.Release(*it, true);
				it = m_lUnits.erase(it);
				m_iUnitCount--;
			}
			else
			{
				++it;
			}
		}
	}
	SS_CATCH(L"");
}

void CSentence::Reset()
{
	m_Language = ealUndefined;
	m_iUnitCount = 0;
	m_itTop = m_lUnits.begin();
}

CUnit* CSentence::AddUnit()
{
	SS_TRY;

	m_iUnitCount++;	
#ifdef DEBUG
  if (m_itTop._Getcont() != 0)
#endif
	if (m_itTop != m_lUnits.end())
	{
		(*m_itTop)->Reset(false);
		return *(m_itTop++);
	}

	m_lUnits.push_back(m_oUnitStorage.Allocate());
	m_itTop = m_lUnits.end();
	return (m_lUnits.back());

	SS_CATCH(L"");
	return NULL;
}

int CSentence::GetUnitCount()
{
	return m_iUnitCount;
}

void CSentence::ReleaseSentence()
{
	delete this;
}

ISentence* CSentence::GetSentence(IBlackBoxTextStorage* pStorage, TUnitList::iterator& startIt, int maxUnitCount, bool bOnlySyntax)
{
	SS_TRY;
	{
		ISentence* pSent = pStorage->CreateSentence();
#ifdef DEBUG
	  if (m_itTop._Getcont() != 0)
#endif
		for (int i = 0; ((maxUnitCount == 0) || (i < maxUnitCount)) && (startIt != m_itTop); ++i, ++startIt)
		{
			// add units to ISentence and set syntax flag
			if ((*startIt)->GetTypeStrict(wmNonSyntax) != wtNonSyntax)
			{
				(*startIt)->GetUnit()->SetSyntaxFlag(true);
			}

			if (bOnlySyntax && !(*startIt)->GetUnit()->GetSyntaxFlag())
			{
				(*startIt)->GetUnit()->ReleaseUnitHorizontal();
			}
			else
			{
				pSent->AddUnit((*startIt)->GetUnit());
			}
		}

		if (m_Language != ealUndefined)
		{
			pSent->SetLanguage(m_Language);
		}

		return pSent;
	}
	SS_CATCH(L"");
	return NULL;
}

ISentence* CSentence::GetSentence(IBlackBoxTextStorage* pStorage, bool bOnlySyntax)
{
	TUnitList::iterator it = m_lUnits.begin();
	return GetSentence(pStorage, it, m_iUnitCount, bOnlySyntax);
}

vector<ISentence*> CSentence::GetSentences(IBlackBoxTextStorage* pStorage,  int maxUnitCountInSentence, bool bOnlySyntax)
{
	vector<ISentence*> result;
#ifdef DEBUG
  if (m_itTop._Getcont() != 0)
#endif
	for (TUnitList::iterator it = m_lUnits.begin(); it != m_itTop; )
	{
		result.push_back(GetSentence(pStorage, it, maxUnitCountInSentence, bOnlySyntax));
	}
	return (result);
}

typedef map<EAnalyseLanguage, int> TLangMap;

class CLangCompare
{
public:
	bool operator()(const TLangMap::value_type& t1, const TLangMap::value_type& t2)
	{
		return (t1.second < t2.second);
	}
};

void CSentence::SetLanguage()
{
	SS_TRY;
	{
		TLangMap languages;
#ifdef DEBUG
	  if (m_itTop._Getcont() != 0)
#endif
		for (TUnitList::iterator it = m_lUnits.begin(); it != m_itTop; ++it)
		{
			IIndex* pIndex = (*it)->GetUnit()->GetFirstIndex();
			if (!pIndex)
			{
				continue;
			}

			EAnalyseLanguage eal = pIndex->GetDictionaryIndex()->GetFirst().GetLanguage();
			if (eal == ealRussian) 
			{ 
				m_Language = eal; 
				return; 
			} // для русского достаточно и одного слова
			if (eal != ealUndefined)
			{
				pair<TLangMap::iterator, bool> pr = languages.insert(TLangMap::value_type(eal, 0));
				(*pr.first).second++;
			}
		}

		TLangMap::iterator itl = max_element(languages.begin(), languages.end(), CLangCompare());
		if (itl != languages.end())
		{
			m_Language = (*itl).first;

			// if there are languages with the same quantity, choose the one that coincides with first word's language
			for (TLangMap::iterator it = languages.begin(); it != languages.end(); ++it)
			{
				IIndex* pIndex = m_lUnits.front()->GetUnit()->GetFirstIndex();
				if (!pIndex)
				{
					continue;
				}

				if(	(*itl).second == (*it).second && 
					pIndex->GetDictionaryIndex()->GetFirst().GetLanguage() == (*it).first)
				{
					m_Language = (*it).first;
					break;
				}
			}
		}
	}
	SS_CATCH(L"");
}

EAnalyseLanguage CSentence::GetLanguage()
{
	return m_Language;
}

void CSentence::JoinUnits(CUnit* pUnit, int nCount)
{
	SS_TRY;
	{
		IUnit *pCurrent = pUnit->GetUnit();
		TUnitList::iterator it = m_it;
		for(int i = 0; i < nCount && it != m_lUnits.end(); ++i, ++it)
		{
			pCurrent->AddUnitVertical((*it)->GetUnit());
			pCurrent->AddUnitChild((*it)->GetUnit());
			m_oUnitStorage.Release(*it, false);
			m_iUnitCount--;
		}

		it = m_lUnits.erase(m_it, it);
		m_it = m_lUnits.insert(it, pUnit);
		m_iUnitCount++;
	}
	SS_CATCH(L"");
}

void CSentence::ReplaceUnits(CUnit** ppUnit, int iUnitCount, int nCount)
{
	SS_TRY;

	TUnitList::iterator it = m_it;
	for (int i = 0; i < nCount && it != m_lUnits.end(); ++i, ++it)
	{
		m_oUnitStorage.Release(*it, true);
		m_iUnitCount--;
	}

	m_it = m_lUnits.erase(m_it, it);
	for (int i = iUnitCount - 1; i >= 0; i--)
	{
		m_it = m_lUnits.insert(m_it, ppUnit[i]);
		m_iUnitCount++;
	}

	SS_CATCH(L"");
}

void CSentence::JoinUnits(CUnit* pUnit, int nCount, int nToLeft)
{
	SS_TRY;

	for (int i = nCount + nToLeft - 1; i; i--)
	{
		--m_it;
	}

	JoinUnits(pUnit, nCount);
	while (nToLeft--)
	{
		++m_it;
	}

	SS_CATCH(L"");
}

void CSentence::ReplaceUnits(CUnit** ppUnit, int iUnitCount, int nCount, int nToLeft)
{
	SS_TRY;

	for (int i = nCount + nToLeft - 1; i; i--)
	{
		--m_it;
	}

	ReplaceUnits(ppUnit, iUnitCount, nCount);
	while (nToLeft--)
	{
		++m_it;
	}

	SS_CATCH(L"");
}

void CSentence::JoinLastUnits(CUnit* pUnit, int nCount)
{
#ifdef DEBUG
  if (m_itTop._Getcont() != 0)
#endif
	m_it = m_itTop;
	for (int i = 0; i < nCount; i++)
	{
		--m_it;
	}

	JoinUnits(pUnit, nCount);
}

void CSentence::JoinSentence(CUnit* pUnit)
{
	m_it = m_lUnits.begin();
	JoinUnits(pUnit, m_iUnitCount);
}

CUnit* CSentence::GetUnit(int nToLeft)
{
	SS_TRY;
	{
		TUnitList::iterator it = m_it;
		for (int i = 0; i < nToLeft; i++)
		{
			--it;
		}
		return *it;
	}
	SS_CATCH(L"");

	return NULL;
}

CUnit* CSentence::GetUnit(int nCount, int nToLeft, EWalkerMode eMode, EWordType eType)
{
	SS_TRY;

	TUnitList::iterator it = m_it;
	for (int i = 0; i < nToLeft; i++)
	{
		--it;
	}

	for (int i = 0; i < nCount; i++)
	{
		EWordType eResult = (*it)->GetTypeStrict(eMode);
		if (eResult == eType)
		{
			return *it;
		}
		if (it!=m_lUnits.begin()) 
		{
			--it;
		}
	}

	SS_CATCH(L"");
	return NULL;
}

void CSentence::MoveUnits(CSentence* pSource, int nCount)
{
	TUnitList::iterator it = m_lUnits.end();
	if (nCount < 0)
	{
		nCount = pSource->m_iUnitCount;
		it = m_lUnits.begin();
	}
	else
	{
		for (int i = 0; i < nCount; i++)
		{
			--it;
		}
	}

	m_iUnitCount += nCount;
	pSource->m_iUnitCount = 0;
	m_lUnits.splice(pSource->m_lUnits.begin(), pSource->m_lUnits, it, m_lUnits.end());
}

bool CSentence::IsFirstUnit(CUnit* pUnit)
{
	if (m_lUnits.size() == 0) 
	{
		return false;
	}
	return (m_lUnits.front() == pUnit);
}

bool CSentence::HaveCoincidentIndexes(int nCount, int nToLeft, EIndexType eit)
{
	SS_TRY;

	TUnitList::iterator it = m_it;
	for (int i = 0; i < nToLeft; i++)
	{
		--it;
	}

	TUnitList::iterator itCompare = it;
	for (int i = 0; i < nCount - 1; i++)
	{
		if (!(*--it)->HaveCoincidentIndexes(**itCompare, eit))
		{
			return false;
		}
	}

	return true;

	SS_CATCH(L"");
	return false;
}

int CSentence::GetUnitCount(int nCount, int nToLeft, EIndexType eit)
{
	SS_TRY;

	TUnitList::iterator it = m_it;
	for (int i = 0; i < nToLeft - 1; i++)
	{
		--it;
	}

	int iCount = 0;
	for (int i = 0; i < nCount; i++)
	{
		if ((*--it)->GetUnit()->HasIndexType(eit))
		{
			iCount++;
		}
	}

	return iCount;

	SS_CATCH(L"");
	return 0;
}

bool CSentence::CheckRangeWordType(int nCount, int nToLeft, EWalkerMode eMode, EWordType eType, bool bAll)
{
	SS_TRY;

	TUnitList::iterator it = m_it;	
	for (int i = 0; i < nToLeft; i++)
	{
		--it;
	}

	for (int i = 0; i < nCount; i++)
	{
		if (it == m_lUnits.begin()) 
		{
			break;
		}
		EWordType eUnitType = (*it--)->GetType(eMode);
		if (bAll && eUnitType != eType || !bAll && eUnitType == eType)
		{
			return false;
		}
	}

	return true;

	SS_CATCH(L"");
	return false;
}

TUnitStorage* CSentence::GetUnitStorage()
{
	return &m_oUnitStorage;
}

}
}
}
