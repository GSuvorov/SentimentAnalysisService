#include "stdafx.h"
#include "XMLHelper.h"

#include <msxml2.h>
#pragma comment(lib, "MsXml2.lib")


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CXMLHelper::CXMLHelper()
{
}

CXMLHelper::~CXMLHelper()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CXMLHelper interface

bool CXMLHelper::CreateDocument()
{
	HRESULT hr = m_pDoc.CreateInstance(CLSID_DOMDocument2);
	if(FAILED(hr))
		return false;
	m_pDoc->Putasync(false);
	return true;
}

void CXMLHelper::SetXML(LPCWSTR szXML)
{
	m_pDoc->loadXML(szXML);
}

void CXMLHelper::GetXML(wstring* pResult)
{
	*pResult = m_pDoc->Getxml();
}

/////////////////////////////////////////////////////////////////////////////////////
// CXMLHelper interface (writing test file)

bool CXMLHelper::SetXMLAttr(xml2::IXMLDOMNodePtr pNode, BSTR bstrName, BSTR bstrType, VARIANT vtValue)
{
	xml2::IXMLDOMNodePtr pndAttr = m_pDoc->createNode(NODE_ATTRIBUTE, bstrName, L"");
	if(pndAttr == NULL)
		return false;

	pndAttr->PutdataType(bstrType);
	pndAttr->PutnodeTypedValue(vtValue);

	xml2::IXMLDOMNamedNodeMapPtr pMap = pNode->Getattributes();
	if(pMap == NULL)
		return false;

	xml2::IXMLDOMNodePtr pndNew = pMap->setNamedItem(pndAttr);
	return pndNew != NULL;
}

xml2::IXMLDOMNodePtr CXMLHelper::AddNode(xml2::IXMLDOMNodePtr pParent, BSTR bstrName, BSTR bstrText)
{
	xml2::IXMLDOMNodePtr pNode = m_pDoc->createNode(NODE_ELEMENT, bstrName, L"");
	pParent->appendChild(pNode);
	if(bstrText)
		pNode->put_text(bstrText);
	return pNode;
}

void CXMLHelper::AddCDataSection(xml2::IXMLDOMNodePtr pParent, BSTR bstrName, BSTR bstrText)
{
	xml2::IXMLDOMNodePtr pElement = AddNode(pParent, bstrName);

	xml2::IXMLDOMNodePtr pCDataSection = m_pDoc->createNode(NODE_CDATA_SECTION, L"", L"");
	pElement->appendChild(pCDataSection);
	pCDataSection->put_text(bstrText);
}

/////////////////////////////////////////////////////////////////////////////////////
// CXMLHelper interface (reading test file)

int CXMLHelper::GetIntAttribute(xml2::IXMLDOMNodePtr pParent, BSTR bstrName, int iDefault)
{
	xml2::IXMLDOMNamedNodeMapPtr pAttr = pParent->Getattributes();
	if(pAttr == NULL)
		return iDefault;

	xml2::IXMLDOMNodePtr pNode = pAttr->getNamedItem(bstrName);
	if(pNode == NULL)
		return iDefault;

	pNode->put_dataType(L"int");
	return pNode->nodeTypedValue;
}

_bstr_t CXMLHelper::GetBstrAttribute(xml2::IXMLDOMNodePtr pParent, BSTR bstrName)
{
	xml2::IXMLDOMNamedNodeMapPtr pAttr = pParent->Getattributes();
	if(pAttr == NULL)
		return _bstr_t(L"");

	xml2::IXMLDOMNodePtr pNode = pAttr->getNamedItem(bstrName);
	if(pNode == NULL)
		return _bstr_t(L"");

	pNode->put_dataType(L"bstr");
	return _bstr_t(pNode->nodeTypedValue);
}

_bstr_t CXMLHelper::GetTagText(BSTR bstrPath)
{
	xml2::IXMLDOMNodeListPtr pList = m_pDoc->selectNodes(bstrPath);
	if(pList == NULL)
		return L"";

	xml2::IXMLDOMNodePtr pNode = pList->nextNode();
	if(pNode == NULL)
		return L"";

	return pNode->Gettext();
}

int CXMLHelper::GetTagInt(BSTR bstrPath, int iDefault)
{
	xml2::IXMLDOMNodeListPtr pList = m_pDoc->selectNodes(bstrPath);
	if(pList == NULL)
		return iDefault;

	xml2::IXMLDOMNodePtr pNode = pList->nextNode();
	if(pNode == NULL)
		return iDefault;

	pNode->put_dataType(L"int");
	return pNode->nodeTypedValue;
}
