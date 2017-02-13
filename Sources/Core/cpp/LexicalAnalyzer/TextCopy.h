#pragma once


namespace SS
{
namespace LexicalAnalyzer
{

/////////////////////////////////////////////////////////////////////////////////////
// CTextCopy class

class CTextCopy
{
public:
	CTextCopy(IBlackBoxTextStorage* pStorage) :
			m_pStorage(pStorage)
	{
	}

public:
	void AddSentence(ISentence* pSent, ISentence* pTo)
	{
		for(CLeafIterator it = pSent->GetFirstUnit(); *it; ++it)
			pTo->AddUnit(CopyUnit(*it));
	}

	ISentence* CopySentence(ISentence* pSent)
	{
		ISentence* pNewSent = m_pStorage->CreateSentence();
		for(CLeafIterator it = pSent->GetFirstUnit(); *it; ++it)
			pNewSent->AddUnit(CopyUnit(*it));
		return pNewSent;
	}

	IUnit* CopyUnit(IUnit* pUnit)
	{
		IUnit* pNewUnit = m_pStorage->CreateUnit();
		for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
			pNewUnit->AddIndex(CopyIndex(pIndex));
		pNewUnit->SetWord(pUnit->GetWord());
		pNewUnit->SetPosition(pUnit->GetPosition());
		return pNewUnit;
	}

	IIndex* CopyIndex(IIndex* pIndex)
	{
		IIndex* pNewIndex = m_pStorage->CreateIndex();
		pNewIndex->GetDictionaryIndex()->AppendIndex(pIndex->GetDictionaryIndex()->GetFirst());

		for(IFeature* pFeature = pIndex->GetFirstFeature(); pFeature; pFeature = pIndex->GetNextFeature())
		{
			IFeature* pNewFeature = m_pStorage->CreateFeature();
			pNewFeature->SetMorphoIndex(pFeature->GetMorphoIndex());
			pNewFeature->SetMorphoInfo(pFeature->GetMorphoInfo());
			pNewFeature->SetPartOfSpeech(pFeature->GetPartOfSpeech());

			pNewIndex->AddFeature(pNewFeature);
		}

		return pNewIndex;
	}

private:
	IBlackBoxTextStorage* m_pStorage;
};

}
}
