#pragma once

#include "..\BlackBoxNew\GetXML\XmlFile.h"
using namespace SS::Core::BlackBox::XML;

#include "..\ASCInterface\ICommonContainers.h"
using namespace SS::Interface::Core::CommonContainers;

#include <string>
using namespace std;


namespace SS
{
namespace Core
{
namespace CommonContainers
{
namespace XML
{

class CSearchResultFile : protected CXmlFile
{
public:
	CSearchResultFile();
	~CSearchResultFile();

public:
	bool GetXML(ISearchResult* pResult, wstring* pXml, int iFirst = -1, int iLast = -1);

private:
	bool WriteSearchResult(ISearchResult* pResult, xml2::IXMLDOMNodePtr pRoot, int iFirst, int iLast);
	void WriteTextBlock(ITextBlock* pBlock, xml2::IXMLDOMNodePtr pRoot);
	void WriteBlockCut(ITextBlockCut* pCut, xml2::IXMLDOMNodePtr pRoot);
	void WriteSpan(LPCWSTR szText, int iHighlight, xml2::IXMLDOMNodePtr pRoot);
	///Понижение регистра
	void LowStr(std::wstring & wsWord);
};

}
}
}
}
