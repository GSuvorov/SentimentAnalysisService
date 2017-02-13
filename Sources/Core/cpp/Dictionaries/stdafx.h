// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define  _WIN32_WINNT 0x0403 
#define APP_ID "SSDict"
#define NWLIB_SS_FRAMEWORK

#ifndef NDEBUG
#define SHOW_DEBUG_MSG_LOG_LEVEL 4
#else
#define SHOW_DEBUG_MSG_LOG_LEVEL 2
#endif

//Отключить сортировку морфотипов
#define DISABLE_MORPHO_SORTING

#ifndef NDEBUG
   //Создавать ли дочерние юниты при детектировании дефисов
   //#define CREATE_CHILDREN_UNITS_IN_HYPEN_DETECTION 
#endif

// Windows Header Files:
#include <tchar.h>
#include "../[libs]/FirstHeader.h"
#include <windows.h>

#include <fstream>
#include <set>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>
#include <hash_map>  
#include <cstdlib>
#include <bitset>
#include <deque>
#include <iterator>
#include <sstream>
#include "resource.h"


//#define CURRENT_THREAD_DEBUG_INFO LockDebugInfo
#define CURRENT_THREAD_DEBUG_INFO NoDebugInfo

//#define NO_SYNCHRONIZATION

#ifndef NO_SYNCHRONIZATION
   #define OBJECT_LEVEL_LOCKABLE_SYNC_DEP ObjectLevelLockable
   #define CLASS_LEVEL_LOCKABLE_SYNC_DEP ClassLevelLockable
#else
   #define OBJECT_LEVEL_LOCKABLE_SYNC_DEP NoLockable
   #define CLASS_LEVEL_LOCKABLE_SYNC_DEP NoLockable
#endif


#include "../ASCInterface/defines.h"
USES_LOAD_MANAGER;

typedef std::basic_string<TCHAR>  STLSTR;

// TODO: reference additional headers your program requires here
#include "../ASSInterface/Constants.h"
#include "../ASSInterface/ICommon.h"
#include "../ASSInterface/IDictionaryManager.h"
#include "../ASCInterface/IInterface.h"
#include "../ASSInterface/TDictionaryNames.h"
#include "../ASCInterface/IBaseManager.h"
#include "../ASSInterface/ICoSeDi.h"
#include "../ASCInterface/IDBMS.h"
#include "../ASSInterface/ILingvoBaseManager.h"
#include "../ASSInterface/IBlackBox.h"

//Достали все 1831 предупреждения
#pragma warning( push )
#pragma warning( disable: 4715 4244 4266 4267 )
#include "../ASSInterface/IAMConverter.h"
#pragma warning( pop )

#include "../ASSInterface/TBaseNames.h"

namespace SS{ 
namespace Dictionary{
   namespace Virtual {}

	using namespace SS::Dictionary::Types;
	using namespace SS::Interface::Core::Dictionary;
   using namespace SS::Interface::Core::BlackBox;
	using namespace SS::Interface::Core;
	using namespace SS::Interface::Core::Common;
	using namespace SS::Interface::Core::ResourceManagers;
	using namespace SS::Interface::Core::AMConverter;
   using namespace SS::Core::Features;
   using namespace SS::Core::Types;
   using namespace SS::Dictionary::Virtual;
   using namespace SS::Interface::Core::CoSeDi;
   using namespace SS::Core::Features::Types;
} //namespace Dictionary
} //namespace SS

namespace SS{ 
namespace UnitTests{ 
namespace Dictionary{
	
   using namespace SS::Dictionary;

} //namespace Dictionary
} //namespace UnitTests
} //namespace SS


#include "../[libs]/StlAuxiliary.h"
#include "../[libs]/Auxiliary.h"
#include "../[libs]/Compose.hpp"
#include "../[libs]/MultipleCompare.hpp"
#include "../[libs]/LikePointer.hpp"
#include "../[libs]/Threads.h"
#include "../[libs]/AssocVector.h"
#include "../[libs]/FileSystem.h"
#include "../[libs]/DBMSAuxiliary.hpp"
#include "../[libs]/Timer.hpp"

using namespace SS::Lib;

#include "InitData.h"
#include "Detail.h"
#include "Statistics.h"
#include "EngineBase/CPartOfWordsSearch.h"
#include "EngineBase/CMorphoTypesSearch.h"
#include "CreateManagers.h"
#include "SmartDictionary.h"
#include "AbbreviationsDictionary.h"
#include "RomanNumberDictionary.h"
#include "NumberDictionary.h"
#include "NumericOrderingDictionary.h"
#include "OrderingRussian.h"
#include "SymbolsDictionary.h"
#include "MorphoTypesCollection.h"
#include "OneRoot.h"
#include "Source.h"
#include "NewWordsList.h"
#include "AdditionalAnalysis.h"
#include "MALanguagesManager.h"
