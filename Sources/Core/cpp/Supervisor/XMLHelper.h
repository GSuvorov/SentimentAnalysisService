#pragma once

#import "..\..\..\[libraries]\MSXML4.0\msxml4.dll" rename_namespace("xml2")

#include <string>
using namespace std;


class CXMLHelper
{
public:
	CXMLHelper();
	~CXMLHelper();

public:
	bool CreateDocument();
	void SetXML(LPCWSTR szXML);
	void GetXML(wstring* pResult);

// writing file
public:
	bool SetXMLAttr(xml2::IXMLDOMNodePtr pNode, BSTR bstrName, BSTR bstrType, VARIANT vtValue);
	xml2::IXMLDOMNodePtr AddNode(xml2::IXMLDOMNodePtr pParent, BSTR bstrName, BSTR bstrText = NULL);
	void AddCDataSection(xml2::IXMLDOMNodePtr pParent, BSTR bstrName, BSTR bstrText);

// reading file
public:
	int GetIntAttribute(xml2::IXMLDOMNodePtr pParent, BSTR bstrName, int iDefault = -1);
	_bstr_t GetBstrAttribute(xml2::IXMLDOMNodePtr pParent, BSTR bstrName);
	_bstr_t GetTagText(BSTR bstrPath);
	int GetTagInt(BSTR bstrPath, int iDefault = -1);

private:
	xml2::IXMLDOMDocument2Ptr m_pDoc;
};
