#include "stdafx.h"
#include ".\DocStructureReader.h"

#include "..\ASCInterface\DocStructures.h"

#include <msxml2.h>
#pragma comment(lib, "MsXml2.lib")


namespace SS
{
namespace LexicalAnalyzer
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CDocStructureReader::CDocStructureReader() : m_pRangeList(NULL)
{
	m_BSTR__int    = SysAllocString( L"int" );
	m_BSTR__bstr   = SysAllocString( L"bstr" );
	m_BSTR__begin  = SysAllocString( L"begin" );
	m_BSTR__end    = SysAllocString( L"end" );
	m_BSTR__level  = SysAllocString( L"level" );
	m_BSTR__number = SysAllocString( L"number" );
	m_BSTR__font   = SysAllocString( L"font" );
	m_BSTR__bold   = SysAllocString( L"bold" );
}

CDocStructureReader::~CDocStructureReader()
{
	if (m_BSTR__int)    { SysFreeString( m_BSTR__int ); m_BSTR__int = NULL; };
	if (m_BSTR__bstr)   { SysFreeString( m_BSTR__bstr ); m_BSTR__bstr = NULL; }
	if (m_BSTR__begin)  { SysFreeString( m_BSTR__begin ); m_BSTR__begin = NULL; }
	if (m_BSTR__end)    { SysFreeString( m_BSTR__end ); m_BSTR__end = NULL; }
	if (m_BSTR__level)  { SysFreeString( m_BSTR__level ); m_BSTR__level = NULL; }
	if (m_BSTR__number) { SysFreeString( m_BSTR__number ); m_BSTR__number = NULL; }
	if (m_BSTR__font)   { SysFreeString( m_BSTR__font ); m_BSTR__font = NULL; }
	if (m_BSTR__bold)   { SysFreeString( m_BSTR__bold ); m_BSTR__bold = NULL; }
}

/////////////////////////////////////////////////////////////////////////////////////
// CDocStructureReader interface

bool CDocStructureReader::ParseXml(BSTR bstrXml, CRangeList* pRangeList, BSTR bstrFile)
{
	SS_TRY;

	m_pRangeList = pRangeList;

	HRESULT hr = m_pDoc.CreateInstance(CLSID_DOMDocument2);
	if(FAILED(hr))
		return false;

	if(bstrFile)
		m_pDoc->load(bstrFile);
	else
		m_pDoc->loadXML(bstrXml);

	if(!ReadHeaders())
		return false;

	if(!ReadStyles())
		return false;

	if(!ReadEpigraphs())
		return false;

	return true;

	SS_CATCH(L"");
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////
// CDocStructureReader functions

bool CDocStructureReader::ReadHeaders()
{
	SS_TRY;

	WCHAR szPath[64] = { 0 };
	swprintf(szPath, L"%s/%s/%s", TAG_Document, TAG_Marking, TAG_Header);

	xml2::IXMLDOMNodeListPtr pList = m_pDoc->selectNodes(szPath);
	while(true)
	{
		xml2::IXMLDOMNodePtr pNode = pList->nextNode();
		if(pNode == NULL)
			break;

		xml2::IXMLDOMNamedNodeMapPtr pAttr = pNode->Getattributes();
		if(pAttr == NULL)
			return false;

		// read attributes
		int nBegin = GetIntAttribute(pAttr, m_BSTR__begin),
			nEnd   = GetIntAttribute(pAttr, m_BSTR__end),
			nLevel = GetIntAttribute(pAttr, m_BSTR__level);

		// insert header
		SStyle ss = { rtHeader, nLevel, -1, -1 };
		m_pRangeList->AddRange(nBegin, nEnd, ss);
	}

	return true;

	SS_CATCH(L"");
	return false;
}

bool CDocStructureReader::ReadStyles()
{
	SS_TRY;

	WCHAR szPath[64] = { 0 };
	swprintf(szPath, L"%s/%s/%s", TAG_Document, TAG_Marking, TAG_Style);

	xml2::IXMLDOMNodeListPtr pList = m_pDoc->selectNodes(szPath);
	while(true)
	{
		xml2::IXMLDOMNodePtr pNode = pList->nextNode();
		if(pNode == NULL)
			break;

		xml2::IXMLDOMNamedNodeMapPtr pAttr = pNode->Getattributes();
		if(pAttr == NULL)
			return false;

		// read attributes
		int nBegin  = GetIntAttribute(pAttr, m_BSTR__begin),
			nEnd    = GetIntAttribute(pAttr, m_BSTR__end),
			nNumber = GetIntAttribute(pAttr, m_BSTR__number);

		// insert style if is present in m_mStyles
		if(m_mStyles.find(nNumber) != m_mStyles.end())
		{
			SStyle ss = { rtStyle, 0 };
			m_pRangeList->AddRange(nBegin, nEnd, ss);
		}
	}

	return true;

	SS_CATCH(L"");
	return false;
}

bool CDocStructureReader::ReadEpigraphs()
{
	SS_TRY;

	WCHAR szPath[64] = { 0 };
	swprintf(szPath, L"%s/%s/%s", TAG_Document, TAG_Marking, TAG_Epigraph);

	xml2::IXMLDOMNodeListPtr pList = m_pDoc->selectNodes(szPath);
	while(true)
	{
		xml2::IXMLDOMNodePtr pNode = pList->nextNode();
		if(pNode == NULL)
			break;

		xml2::IXMLDOMNamedNodeMapPtr pAttr = pNode->Getattributes();
		if(pAttr == NULL)
			return false;

		// read attributes
		int nBegin = GetIntAttribute(pAttr, m_BSTR__begin),
			nEnd   = GetIntAttribute(pAttr, m_BSTR__end);

		// insert epigraph
		SStyle ss = { rtEpigraph, 0 };
		m_pRangeList->AddRange(nBegin, nEnd, ss);
	}

	return true;

	SS_CATCH(L"");
	return false;
}

void CDocStructureReader::ReadStyleTable()
{
	SS_TRY;

	m_mStyles.clear();

	WCHAR szPath[64] = { 0 };
	swprintf(szPath, L"%s/%s/%s", TAG_Document, TAG_DocStyle, TAG_Style);

	xml2::IXMLDOMNodeListPtr pList = m_pDoc->selectNodes(szPath);
	while(true)
	{
		xml2::IXMLDOMNodePtr pNode = pList->nextNode();
		if(pNode == NULL)
			break;

		xml2::IXMLDOMNamedNodeMapPtr pAttr = pNode->Getattributes();
		if(pAttr == NULL)
			return;

		// read attributes
		_bstr_t bstrFont = GetBstrAttribute(pAttr, m_BSTR__font);
		if(bstrFont != _bstr_t(L"large") || bstrFont != _bstr_t(L"largest"))
			continue;

		if(GetIntAttribute(pAttr, m_BSTR__bold) == 0)
			continue;

		// insert style, if it's bold and large
		int nStyle = GetIntAttribute(pAttr, m_BSTR__number);
		m_mStyles.insert(TIntMap::value_type(nStyle, 0));
	}

	SS_CATCH(L"");
}

int CDocStructureReader::GetIntAttribute(xml2::IXMLDOMNamedNodeMapPtr pAttr, BSTR bstrName)
{
	SS_TRY;

	xml2::IXMLDOMNodePtr pNode = pAttr->getNamedItem(bstrName);
	if(pNode == NULL)
		return -1;

	pNode->put_dataType( m_BSTR__int );
	return pNode->nodeTypedValue;

	SS_CATCH(L"");
	return -1;
}

_bstr_t CDocStructureReader::GetBstrAttribute(xml2::IXMLDOMNamedNodeMapPtr pAttr, BSTR bstrName)
{
	SS_TRY;

	xml2::IXMLDOMNodePtr pNode = pAttr->getNamedItem(bstrName);
	if(pNode == NULL)
		return _bstr_t(L"");

	pNode->put_dataType( m_BSTR__bstr );
	return _bstr_t(pNode->nodeTypedValue);

	SS_CATCH(L"");
	return _bstr_t(L"");
}

}
}
