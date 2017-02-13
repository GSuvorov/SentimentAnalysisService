#include "stdafx.h"
#include "SearchResultFile.h"

#include "..\ASCInterface\TCoommonTypes.h"
using namespace SS::Core::Types::IndexationSearch;


namespace SS
{
namespace Core
{
namespace CommonContainers
{
namespace XML
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CSearchResultFile::CSearchResultFile()
{
}

CSearchResultFile::~CSearchResultFile()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CSearchResultFile interface

bool CSearchResultFile::GetXML(ISearchResult* pResult, wstring* pXml, int iFirst, int iCount)
{
	SS_TRY;

	if(!CreateDocument())
		return false;

	xml2::IXMLDOMNodePtr pRoot = AddNode(m_pDoc, (BSTR) (void*) L"SearchResult");
	if(pRoot == NULL)
		return false;

	bool bRet = WriteSearchResult(pResult, pRoot, iFirst, iCount);
	*pXml = m_pDoc->Getxml();
	return bRet;

	SS_CATCH(L"");
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////
// CSearchResultFile functions

bool CSearchResultFile::WriteSearchResult(ISearchResult* pResult, xml2::IXMLDOMNodePtr pRoot, int iFirst, int iCount)
{
	SetXMLAttr(pRoot, (BSTR) (void*) L"QueryID", (BSTR) (void*) L"int", _variant_t(pResult->GetRequestID()));
	if(iFirst >= 0 && iCount > 0)
	{
		int iNum = 0;
		ITextBlock* pBlock = NULL;
		for(pBlock = pResult->GetTextBlock(iFirst); pBlock; pBlock = pResult->GetNextTextBlock())
		{
			if(iNum >= iCount)
				break;
			WriteTextBlock(pBlock, pRoot);
			iNum++;
		}

		SetXMLAttr(pRoot, (BSTR) (void*) L"MessageID", (BSTR) (void*) L"int", _variant_t(iFirst / iCount + 1));
		SetXMLAttr(pRoot, (BSTR) (void*) L"IsLastResult", (BSTR) (void*) L"int", _variant_t(pBlock ? 0 : 1));
		return iNum > 0;
	}
	else
	{
		for(ITextBlock* pBlock = pResult->GetFirstTextBlock(); pBlock; pBlock = pResult->GetNextTextBlock())
			WriteTextBlock(pBlock, pRoot);
		return true;
	}
}

void CSearchResultFile::WriteTextBlock(ITextBlock* pBlock, xml2::IXMLDOMNodePtr pRoot)
{
	xml2::IXMLDOMNodePtr pCutRoot = AddNode(pRoot, (BSTR) (void*) L"TextBlock");
	SetXMLAttr(pCutRoot, (BSTR) (void*) L"Rev", (BSTR) (void*) L"int", _variant_t(pBlock->GetRelevance()));
	SetXMLAttr(pCutRoot, (BSTR) (void*) L"DocID", (BSTR) (void*) L"int", _variant_t(pBlock->GetTextFeature()->GetTextNumber()));
	SetXMLAttr(pCutRoot, (BSTR) (void*) L"DocPath", (BSTR) (void*) L"string", _variant_t(pBlock->GetTextFeature()->GetTextPath()));
	SetXMLAttr(pCutRoot, (BSTR) (void*) L"DocName", (BSTR) (void*) L"string", _variant_t(pBlock->GetTextFeature()->GetTextName()));

	for(ITextBlockCut* pCut = pBlock->GetFirstBlockCut(); pCut; pCut = pBlock->GetNextBlockCut())
		WriteBlockCut(pCut, pCutRoot);
}

void CSearchResultFile::WriteBlockCut(ITextBlockCut* pCut, xml2::IXMLDOMNodePtr pRoot)
{
	xml2::IXMLDOMNodePtr pSpanRoot = AddNode(pRoot, (BSTR) (void*) L"TextCut");
	SetXMLAttr(pSpanRoot, (BSTR) (void*) L"Rev",(BSTR) (void*)  L"int", _variant_t(pCut->GetRelevance()));
	//Для глоб системы
	if (pCut->GetType() == SS::Core::Types::IndexationSearch::tctText)
	{
		SetXMLAttr(pSpanRoot, (BSTR) (void*) L"Type", (BSTR) (void*) L"string", _variant_t(L"Text"));
	}
	else if (pCut->GetType() == SS::Core::Types::IndexationSearch::tctHeader)
	{
		SetXMLAttr(pSpanRoot, (BSTR) (void*) L"Type", (BSTR) (void*) L"string", _variant_t(L"Header"));
		SetXMLAttr(pSpanRoot, (BSTR) (void*) L"PluginName", (BSTR) (void*) L"string", _variant_t(pCut->GetPluginName()));
	}
	else if (pCut->GetType() == SS::Core::Types::IndexationSearch::tctHttp)
	{
		SetXMLAttr(pSpanRoot, (BSTR) (void*) L"Type", (BSTR) (void*) L"string", _variant_t(L"Ref"));
	}
	//.

	wstring strText;
	int iHighlight = -1;
	for(IIndexAndPosition* pWord = pCut->GetFirstIndexAndPosition(); pWord; pWord = pCut->GetNextIndexAndPosition())
	{
		int iNewHighlight = pWord->GetHighlightType();
		
		//Убираем зеленую подсветку если http
		if ( ((pCut->GetType() == SS::Core::Types::IndexationSearch::tctHttp) || 
			  (pCut->GetType() == SS::Core::Types::IndexationSearch::tctHeader)) && 
			  (iNewHighlight == hltAnswerWord) )
		{ iNewHighlight = hltNone; }
		
		if (pCut->GetRelevance() > 400000000 && !pWord->GetIsInAnswerSentence() && iNewHighlight == hltAnswerWord)
		{ iNewHighlight = hltNone; }

		if (iHighlight != -1 && iHighlight != iNewHighlight)
		{
			//Понижение регистра если адрес
			if (pCut->GetType() == SS::Core::Types::IndexationSearch::tctHttp)
			{ LowStr(strText); }
			WriteSpan(strText.c_str(), iHighlight, pSpanRoot);
			strText.clear();
		}

		iHighlight = iNewHighlight;
		strText += pWord->GetWord();
		if (pCut->GetType() != SS::Core::Types::IndexationSearch::tctHttp)
		{ strText += L" "; }
	}
	if (iHighlight != -1)
	{ 
		//Понижение регистра если адрес
		if (pCut->GetType() == SS::Core::Types::IndexationSearch::tctHttp)
		{ LowStr(strText); }
		WriteSpan(strText.c_str(), iHighlight, pSpanRoot); 
	}
}

void CSearchResultFile::WriteSpan(LPCWSTR szText, int iHighlight, xml2::IXMLDOMNodePtr pRoot)
{
	xml2::IXMLDOMNodePtr pSpan = AddNode(pRoot, (BSTR) (void*) L"sp", (BSTR) (void*) szText);
	SetXMLAttr(pSpan, (BSTR) (void*) L"hl", (BSTR) (void*) L"int", _variant_t(iHighlight));
}

///Понижение регистра
void CSearchResultFile::LowStr(std::wstring & wsWord)
{
	unsigned int iLen = (unsigned int)wsWord.size();
	for (unsigned int i = 0; i < iLen; i++)
	{
		if (L'A' <= wsWord[i] && wsWord[i] <= L'Z')
		{ 
			wsWord[i] = wsWord[i] - L'A' + L'a';
			continue;
		}
		if (L'А' <= wsWord[i] && wsWord[i] <= L'Я') 
		{
			wsWord[i] = wsWord[i] - L'А' + L'а';
			continue;
		}
	}
}


}
}
}
}
