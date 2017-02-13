#pragma once

#include "XmlFile.h"
#include "FeatureHelper.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{
namespace XML
{

class CITextFile : protected CXmlFile
{
public:
	CITextFile(IBlackBoxTextStorage* pStorage);
	~CITextFile();

public:
	_bstr_t GetXML(IText* pText);
	void SetXML(IText* pText, BSTR bstrXML);

	void WriteTestFile(BSTR bstrTestFileName, IText* pText, LPCTSTR szXsl = NULL);
	void TransformResult(BSTR bstrTestFileName, LPCTSTR szXsl);
	IText* ReadTestFile(BSTR bstrTestFileName);

	void SetQuestion(BSTR bstrQuestion);
	void SetText(BSTR bstrText);
	void SetDocStructure(BSTR bstrDocStructure);

	BSTR GetQuestion();
	BSTR GetText();
	BSTR GetDocStructure();

// writing test file
private:
	void WriteText(IText* pText, xml2::IXMLDOMNodePtr pRoot);
	void WriteParagraph(IParagraph* pPara, xml2::IXMLDOMNodePtr pRoot);
	void WriteSentence(ISentence* pSent, xml2::IXMLDOMNodePtr pRoot);
	void WriteUnit(IUnit* pParentUnit, xml2::IXMLDOMNodePtr pRoot);
	void WriteIndex(IIndex* pIndex, xml2::IXMLDOMNodePtr pRoot);
	void WriteDictionaryIndex(IDictionaryIndex* pIndex, xml2::IXMLDOMNodePtr pRoot);
	void WriteFeature(IFeature* pFeature, IIndex* pParentIndex, xml2::IXMLDOMNodePtr pRoot);

// reading test file
private:
	void ReadText(IText* pText);
	void ReadParagraph(IParagraph* pPara, xml2::IXMLDOMNodePtr pTextNode);
	bool ReadSentence(ISentence* pSentParent, IUnit* pUnitParent, xml2::IXMLDOMNodePtr pTextNode);
	IUnit* ReadUnit(xml2::IXMLDOMNodePtr pTextNode);
	void ReadIndex(IUnit* pUnit, xml2::IXMLDOMNodePtr pTextNode);
	void ReadDictionaryIndex(IIndex* pIndex, xml2::IXMLDOMNodePtr pTextNode);
	void ReadFeature(IIndex* pIndex, xml2::IXMLDOMNodePtr pTextNode);

private:
	_bstr_t m_bstrQuestion;
	_bstr_t m_bstrText;
	_bstr_t m_bstrDocStructure;

	IBlackBoxTextStorage* m_pStorage;

	CFeatureHelper m_oFeatureHelper;
};

}
}
}
}
