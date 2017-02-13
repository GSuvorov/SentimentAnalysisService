#pragma once

#include "..\ASSInterface\IBlackBox.h"
using namespace SS::Interface::Core::BlackBox;


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CTextStorage : public IBlackBoxTextStorage
{
public:
	CTextStorage();
	~CTextStorage();

// IBase
public:
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	ULONG Release();

// IBlackBoxTextStorage
public:
	virtual IText* CreateText();
	virtual IParagraph* CreateParagraph();
	virtual ISentence* CreateSentence();
	virtual IUnit* CreateUnit();
	virtual IIndex* CreateIndex();
	virtual IFeature* CreateFeature();
	virtual ITextFeature* CreateTextFeature();
	virtual IObjectDescr* CreateObjectDescr();
};

}
}
}
