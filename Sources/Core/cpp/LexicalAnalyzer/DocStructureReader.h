#pragma once

#import "..\..\..\[libraries]\MSXML4.0\msxml4.dll" rename_namespace("xml2")
#include "RangeList.h"


namespace SS
{
namespace LexicalAnalyzer
{

class CDocStructureReader
{
public:
	CDocStructureReader();
	~CDocStructureReader();

public:
	bool ParseXml(BSTR bstrXml, CRangeList* pRangeList, BSTR bstrFile = NULL);

private:
	bool ReadHeaders();
	bool ReadStyles();
	bool ReadEpigraphs();

	void ReadStyleTable();
	int GetIntAttribute(xml2::IXMLDOMNamedNodeMapPtr pAttr, BSTR bstrName);
	_bstr_t GetBstrAttribute(xml2::IXMLDOMNamedNodeMapPtr pAttr, BSTR bstrName);

private:
	CRangeList* m_pRangeList;
	xml2::IXMLDOMDocument2Ptr m_pDoc;

	typedef map<int, int> TIntMap;
	TIntMap m_mStyles;

	BSTR m_BSTR__int;
	BSTR m_BSTR__bstr;
	BSTR m_BSTR__begin;
	BSTR m_BSTR__end;
	BSTR m_BSTR__level;
	BSTR m_BSTR__number;
	BSTR m_BSTR__font;
	BSTR m_BSTR__bold;
};

}
}
