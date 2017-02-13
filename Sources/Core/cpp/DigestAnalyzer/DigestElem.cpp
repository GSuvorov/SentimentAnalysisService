#include "stdafx.h"
#include "DictElems.h"

CDigestDictElem::CDigestDictElem()
{
	objDescr = NULL;
	nMark=0;
}

CDigestDict::CDigestDict()
{
}

CDigestDict::~CDigestDict()
{
	Clear();
}

void CDigestDict::Clear()
{
	for(TDigestDictIT it = dictElems.begin(); it!=dictElems.end(); ++it)
	{
		CDigestDictElem *elem = *it;
		delete elem;
	}
	dictElems.clear();
}