#pragma once

#include "stdafx.h"
#include "../ASSInterface/IBlackBoxItems.h"
#include "../ASSInterface/IBlackBoxUnit.h"

class CDigestCitation
{
public:	
	list<SS::Interface::Core::BlackBox::IUnit*> citUnits;
	SS::Interface::Core::BlackBox::IUnit* pSubjectUnit;
	SS::Interface::Core::BlackBox::ISentence* pSent;
	int toneNeg;
	int tonePos;

};