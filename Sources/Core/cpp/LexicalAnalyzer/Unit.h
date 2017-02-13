#pragma once

#include "LexemeBoard.h"

#include "..\ASCInterface\TCoommonTypes.h"
using namespace SS::Core::Types::IndexationSearch;


namespace SS
{
namespace LexicalAnalyzer
{

class CParagraph
{
public:
	CParagraph() :
			m_pPara(NULL),
			m_pStorage(NULL),
			m_pText(NULL),
			m_iSentCount(0)
	{
	}

	~CParagraph()
	{
		if(m_pPara && !m_pPara->GetFirstSentence())
			m_pPara->ReleaseParagraph();
	}

public:
	/// sets IText to be filled
	void SetText(IText* pText)
	{
		m_pText = pText;
	}

	/// initializes the paragraph for a new text
	void Init(IBlackBoxTextStorage* pStorage)
	{
		m_lData.clear();

		m_pStorage = pStorage;
		m_iSentCount = 0;

		if(!m_pPara || m_pPara->GetFirstSentence())
			m_pPara = m_pStorage->CreateParagraph();
	}

	void AddSentence(ISentence* pSent)
	{
		m_iSentCount++;
		m_pPara->AddSentence(pSent);
		if(m_iSentCount == MAX_SENTS_IN_PARAGRAPH)
			Break();
	}

	void AddData(int iRev, int iNumber, int iType, int iLevel, LPCWSTR szIndexTypeName)
	{
		SParagraphData oData = { iRev, iNumber, iType, iLevel };
		oData.sIndexTypeName = szIndexTypeName;
		m_lData.push_back(oData);
	}

	/// is called if current lexem is paragraph breaker, creates new paragraph and adds the previous one to IText
	void Break(int iHeaderLevel = 0)
	{
		if(m_iSentCount > 0)
		{
			if(!m_lData.empty())
			{
				SParagraphData& oData = m_lData.front();
				m_pPara->SetRelevance(oData.iRev);
				m_pPara->SetNumber(oData.iNumber);
				m_pPara->SetLevel(oData.iLevel);
				m_pPara->SetType((ETextCutType) oData.iType);
				m_pPara->SetIndexTypeName(oData.sIndexTypeName.c_str());
				m_lData.pop_front();
			}

			if(iHeaderLevel)
			{
				m_pPara->SetType(tctHeader);
				m_pPara->SetLevel(iHeaderLevel - 1);
			}

			m_pText->AddParagraph(m_pPara);
			m_pPara = m_pStorage->CreateParagraph();
			m_iSentCount = 0;
		}
	}

	/// true if the paragraph doesn't contain sentences
	bool IsEmpty()
	{
		return (m_iSentCount == 0);
	}

	/// manages last paragraph, should be called after each block with false and after last block with true
	void Finalize(bool bFinal)
	{
		if (bFinal || !m_pText->GetFirstParagraph())
		{
			Break();
		}
	}

private:
	/// number of sentences in current paragraph
	int m_iSentCount;
	/// IText we are filling
	IText* m_pText;
	/// current paragraph
	IParagraph* m_pPara;
	/// object used to create BlackBoard elements
	IBlackBoxTextStorage* m_pStorage;
	/// list of relevance values to fill new IText
	struct SParagraphData
	{
		int iRev;
		int iNumber;
		int iType;
		int iLevel;
		wstring sIndexTypeName;
	};
	typedef std::list<SParagraphData> TDataList;
	TDataList m_lData;
};

}
}
