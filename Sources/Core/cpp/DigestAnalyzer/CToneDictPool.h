#pragma once

#include "stdafx.h"
#include "../ASSInterface/IDigestManager.h"

class CToneDictPool
{
public:	
	string dict_name;
	SS::Interface::Core::BlackBox::IObjectDescr* adjectivesUnits;
	SS::Interface::Core::BlackBox::IObjectDescr* nounsUnits;
	SS::Interface::Core::BlackBox::IObjectDescr* objectnounsUnits;
	SS::Interface::Core::BlackBox::IObjectDescr* nounsgenerativeUnits;
	SS::Interface::Core::BlackBox::IObjectDescr* verbsUnits;
	SS::Interface::Core::BlackBox::IObjectDescr* inversUnits;
	SS::Interface::Core::BlackBox::IObjectDescr* stopwordsUnits;
};