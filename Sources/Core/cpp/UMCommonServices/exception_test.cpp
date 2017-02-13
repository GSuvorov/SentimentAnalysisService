#include "stdafx.h"
#include "exception_test.h"

#ifdef _SS_UNITTESTS
#include "exception.h"

#include "../../../Sources/Core/ASCInterface/defines.h"

USES_LOAD_MANAGER;

typedef HRESULT (*CREATE_INSTANCE)(const GUID* pGuid, void** ppBase);

using namespace SS::Interface::Core;
using namespace SS::Interface;
using namespace SS::Interface::UnitTests;

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::Core::ResourceManagers::UMCommonServices::CException_test);

namespace SS
{
namespace UnitTests
{
namespace Core
{
namespace ResourceManagers
{
namespace UMCommonServices
{

	void CException_test::testThrow()
	{

		m_pLoadManager = NULL;
		bool blIsThrow = false;
		SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager=NULL;
		wchar_t path[MAX_PATH];
		::GetCurrentDirectoryW(MAX_PATH, (LPWSTR)path);
		wcscat(path, L"\\LoadManager.dll");
		HMODULE hLoadManager=::LoadLibraryW(path);
		
		CREATE_INSTANCE pCreateInstance=(CREATE_INSTANCE)GetProcAddress(hLoadManager, "CreateInstance");


		if(pCreateInstance==NULL){
			wprintf(L"LoadManager entry point not found, error %u\n", GetLastError());
			CPPUNIT_ASSERT(false);
		}
		
		const GUID Guid=CLSID_LoadManager;
		(*pCreateInstance)(&Guid, (void**)&pLoadManager);

		SetLoadManager(pLoadManager);
		try
		{

			SS_TRY	
				SS_TRY
					SS_THROW(L"Hello");
				SS_CATCH (L"");
			SS_CATCH (L"");
		}
		catch (SS::Interface::Core::CommonServices::IException* e)
		{
			blIsThrow = true;;
		}

		CPPUNIT_ASSERT(blIsThrow);
	};


	void CException_test::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
	{
		m_pLoadManager = pLoadManager;
	};
	/// устанавливает указатель на лоад мэнеджер
	SS::Interface::Core::ResourceManagers::ILoadManager* CException_test::GetLoadManager()
	{
		return m_pLoadManager;
	};
}
}
}
}
}

#endif