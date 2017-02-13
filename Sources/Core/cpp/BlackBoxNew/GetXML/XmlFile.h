#pragma once

#import "..\..\..\..\[libraries]\MSXML4.0\msxml4.dll" rename_namespace("xml2")


namespace SS
{
namespace Core
{
namespace BlackBox
{
namespace XML
{

class CXmlFile
{
public:
	CXmlFile();
	~CXmlFile();

public:
	bool CreateDocument();
	bool Transform(LPCTSTR szXsl, BSTR bstrFileToSave);

// writing file
public:
	bool SetXMLAttr(xml2::IXMLDOMNodePtr pNode, BSTR bstrName, BSTR bstrType, VARIANT vtValue);
	xml2::IXMLDOMNodePtr AddNode(xml2::IXMLDOMNodePtr pParent, BSTR bstrName, BSTR bstrText = NULL);
	void AddCDataSection(xml2::IXMLDOMNodePtr pParent, BSTR bstrName, BSTR bstrText);
	void AddProcessingInstruction();

// reading file
public:
	int GetIntAttribute(xml2::IXMLDOMNodePtr pParent, BSTR bstrName, int iDefault = -1);
	_bstr_t GetBstrAttribute(xml2::IXMLDOMNodePtr pParent, BSTR bstrName);

	_bstr_t GetTagText(BSTR bstrPath);

protected:
	xml2::IXMLDOMDocument2Ptr m_pDoc;
};

}
}
}
}
