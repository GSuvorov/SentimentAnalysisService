#include "stdafx.h"
#include "CLogSingleton_test.h"

#ifdef _SS_UNITTESTS
#include "exception.h"

#include "../../../Sources/Core/ASCInterface/defines.h"

USES_LOAD_MANAGER;

typedef HRESULT (*CREATE_INSTANCE)(const GUID* pGuid, void** ppBase);



using namespace SS::Interface::Core;
using namespace SS::Interface;
using namespace SS::Interface::UnitTests;

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::Core::ResourceManagers::UMCommonServices::CLogSingleton_test);

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

	void CLogSingleton_test::testSAVE_LOG()
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
		
			((SS::Interface::Core::CommonServices::ILog*) 
			GetLoadManager()->GetInterface(L"./CommonServices/UMCommonServices.dll", CLSID_Log, IID_Log))->SaveLog(L"", NULL);
			SAVE_LOG(SS_MESSAGE AND L"Hello" AND L"Второй раз");
			SAVE_LOG(SS_ERROR AND L"Hello" AND L"Второй раз");
			SAVE_LOG(SS_EVENT AND L"Hello" AND L"Второй раз");
			SAVE_LOG(SS_WARNING AND L"Hello" AND L"Второй раз");

			SAVE_LOG(SS_MESSAGE AND L"Always" AND L"Второй раз");
			SAVE_LOG(SS_ERROR AND L"Always" AND L"Второй раз");
			SAVE_LOG(SS_EVENT AND L"Always" AND L"Второй раз");
			SAVE_LOG(SS_WARNING AND L"Always" AND L"Второй раз");
		}
		catch (SS::Interface::Core::CommonServices::IException* e)
		{
			blIsThrow = true;;
		}

		CPPUNIT_ASSERT(true);	
	};

	void CLogSingleton_test::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
	{
		m_pLoadManager = pLoadManager;
	};
	/// устанавливает указатель на лоад мэнеджер
	SS::Interface::Core::ResourceManagers::ILoadManager* CLogSingleton_test::GetLoadManager()
	{
		return m_pLoadManager;
	};
}
}
}
}
}

#endif