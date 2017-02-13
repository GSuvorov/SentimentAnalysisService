#include "stdafx.h"
#include "XmlFile.h"

#include <msxml2.h>
#pragma comment(lib, "MsXml2.lib")

#include "FileSaver.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{
namespace XML
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CXmlFile::CXmlFile()
{
}

CXmlFile::~CXmlFile()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CXmlFile interface

bool CXmlFile::CreateDocument()
{
	HRESULT hr = m_pDoc.CreateInstance(CLSID_DOMDocument2);
	if(FAILED(hr))
		return false;
	m_pDoc->Putasync(false);
	return true;
}

bool CXmlFile::Transform(LPCTSTR szXsl, BSTR bstrFileToSave)
{
	// create xsl
	xml2::IXMLDOMDocument2Ptr pXsl;
	HRESULT hr = pXsl.CreateInstance(CLSID_DOMDocument2);
	if(FAILED(hr))
		return false;

	pXsl->Putasync(VARIANT_FALSE);
	if(pXsl->load(_variant_t(szXsl)) == VARIANT_FALSE)
		return false;

	_bstr_t bstrText = m_pDoc->transformNode(pXsl);

	USES_CONVERSION;
	CFileSaver oFileSaver;
	oFileSaver.Create(W2CT(bstrFileToSave));
	oFileSaver.WriteString(W2CT(bstrText));

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// CXmlFile interface (writing test file)

bool CXmlFile::SetXMLAttr(xml2::IXMLDOMNodePtr pNode, BSTR bstrName, BSTR bstrType, VARIANT vtValue)
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

xml2::IXMLDOMNodePtr CXmlFile::AddNode(xml2::IXMLDOMNodePtr pParent, BSTR bstrName, BSTR bstrText)
{
	xml2::IXMLDOMNodePtr pNode = m_pDoc->createNode(NODE_ELEMENT, bstrName, L"");
	pParent->appendChild(pNode);
	if(bstrText)
		pNode->put_text(bstrText);
	return pNode;
}

void CXmlFile::AddCDataSection(xml2::IXMLDOMNodePtr pParent, BSTR bstrName, BSTR bstrText)
{
	xml2::IXMLDOMNodePtr pElement = AddNode(pParent, bstrName);

	xml2::IXMLDOMNodePtr pCDataSection = m_pDoc->createNode(NODE_CDATA_SECTION, L"", L"");
	pElement->appendChild(pCDataSection);
	pCDataSection->put_text(bstrText);
}

void CXmlFile::AddProcessingInstruction()
{
	xml2::IXMLDOMProcessingInstructionPtr pInstr = 
		m_pDoc->createProcessingInstruction(L"xml", L"version=\"1.0\" encoding=\"UTF-16\"");
	m_pDoc->appendChild(pInstr);
}

/////////////////////////////////////////////////////////////////////////////////////
// CXmlFile interface (reading test file)

int CXmlFile::GetIntAttribute(xml2::IXMLDOMNodePtr pParent, BSTR bstrName, int iDefault)
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

_bstr_t CXmlFile::GetBstrAttribute(xml2::IXMLDOMNodePtr pParent, BSTR bstrName)
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

_bstr_t CXmlFile::GetTagText(BSTR bstrPath)
{
	xml2::IXMLDOMNodeListPtr pList = m_pDoc->selectNodes(bstrPath);
	if(pList == NULL)
		return L"";

	xml2::IXMLDOMNodePtr pNode = pList->nextNode();
	if(pNode == NULL)
		return L"";

	return pNode->Gettext();
}

}
}
}
}
