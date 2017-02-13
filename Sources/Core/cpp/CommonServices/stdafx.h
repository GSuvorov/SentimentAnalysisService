// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#define  _WIN32_WINNT 0x0403 
#define APP_ID "SSCS"
#define NWLIB_SS_FRAMEWORK

#include <tchar.h>
#include "../[libs]/FirstHeader.h"
#include <windows.h>
//#include "..\ASCInterface\cmm.h"


#include "..\ASCInterface\defines.h"

USES_LOAD_MANAGER;

#include "..\ASSInterface\IAutorization.h"
#include "..\ASSInterface\ITextManager.h"
#include "..\ASSInterface\IXML.h"

#include "../[libs]/Auxiliary.h"
#include "../[libs]/FileSystem.h"
#include "../[libs]/DBMSAuxiliary.hpp"
#include "../[libs]/DBMSCreateManager.h"
#include "../[libs]/StlAuxiliary.h"
#include "../[libs]/Threads.h"


namespace SS {
namespace CommonServices {
namespace UserTextStorage {

   using namespace Interface::Core::DBMS;
   using namespace Autorization::Types;
   using namespace Interface::Autorization;
   using namespace Interface::Core::ResourceManagers;
   using namespace Interface::TextsMangers;
   using namespace TextsMangers::Types;
   using namespace Interface::Common;

   typedef std::basic_string<TCHAR> TString;

} //namespace UserTextStorage
} //namespace CommonServices
} //namespace SS


namespace SS {
namespace UnitTests{ 
namespace CommonServices {
namespace UserTextStorage {

   using namespace SS::CommonServices::UserTextStorage;
   using namespace SS::Interface::UnitTests;

} //namespace UserTextStorage 
} //namespace CommonServices 
} //namespace UnitTests 
} //namespace SS 

using namespace SS::Lib;

#include "UserTextStorageDetail.h"
#include "UserTextStorage.h"
#include "XMLOperation.h"



