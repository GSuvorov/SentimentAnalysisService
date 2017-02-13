#pragma once


#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "..\ASSInterface\Constants.h"
using namespace SS::Constants;

#include <atlconv.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlhost.h>

#include <string>
#include <map>
#include <vector>
#include <set>
#include <list>
#include <deque>
#include <algorithm>
#include <stack>
using namespace std;

#include <tchar.h>
#include <assert.h>

#include "..\ASCInterface\defines.h"
USES_LOAD_MANAGER;


#include "..\ASSInterface\TDictionaryNames.h"
using namespace SS::Dictionary::DATA_TYPE::NAMES;

#include "..\ASSInterface\IBlackBoxIndex.h"
#include "..\ASSInterface\IBlackBoxUnit.h"
using namespace SS::Interface::Core::BlackBox;

#include "..\ASSInterface\TMainAnalyseTypes.h"
using namespace SS::Core::Types;

#include "../DigestAnalyzer/HelpFunc.h"
