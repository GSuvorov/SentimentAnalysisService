#pragma once

#include "stdafx.h"
#include "../ASSInterface/IDigestManager.h"
#include "../ASSInterface/IDictionaryManager.h"
#include "../ASSInterface/IAMConverter.h"
#include "../assinterface/iblackbox.h"

class CDigestDictElem
{
public:
	CDigestDictElem();
	SS::Interface::Core::BlackBox::IObjectDescr* objDescr;
	int nMark;
};

class CDigestDict
{
public:
	CDigestDict();
	~CDigestDict();

	void Clear();

	list<CDigestDictElem*> dictElems;

};

typedef list<CDigestDictElem*>::iterator TDigestDictIT;