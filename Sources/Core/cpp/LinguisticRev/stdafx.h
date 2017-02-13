#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include "..\ASCInterface\defines.h"
USES_LOAD_MANAGER;


#include <assert.h>

#include "..\ASSInterface\IBlackBoxQuery.h"
using namespace SS::Interface::Core::BlackBox::Query;

#include "..\ASSInterface\IBlackBoxUnit.h"
using namespace SS::Interface::Core::BlackBox;

#include "..\ASSInterface\IAMConverter.h"
using namespace SS::Interface::Core::AMConverter;

#include "..\ASCInterface\IInterface.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\FeatureLibrary\define.h"
using namespace SS::Core::Features::Types;
using namespace SS::Core::Features;

//#define COUNT_SKIP_UNITS
#include "UnitHelper.h"
