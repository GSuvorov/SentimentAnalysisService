#pragma once

#include "RootItem.h"
#include "TextFeature.h"

#include "..\ASSInterface\IBlackBox.h"
using namespace SS::Interface::Core::BlackBox;


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CText : 
		public CRootItem,
		public IText
{
public:
	CText(IBlackBoxTextStorage* pStorage);
	virtual ~CText();

// IText
public:
	virtual ITextFeature* GetTextFeature();
	virtual void AddParagraph(IParagraph* pParagraph);
	virtual IParagraph* GetFirstParagraph();
	virtual IParagraph* GetLastParagraph();
	virtual void ClearText();
	virtual void ReleaseText();
	virtual void Normalize();

// IXMLStorage
public:
	virtual void GetXML(std::wstring * pwXML);
	virtual void SetXML(const std::wstring * pwXML);

private:
	void CopyIndexesToChildren(IUnit* pUnit);
	IIndex* Clone(IIndex* pIndex);
	bool FindIndex(IUnit* pUnit, IIndex* pIndex);

private:
	CTextFeature* m_pTextFeature;
	IBlackBoxTextStorage* m_pStorage;
};

}
}
}
