#pragma once
// Тута все макросы для МС++

#include "services.h"
#include <windows.h>
#include "iinterface.h"
#include "vcclr.h"
#include "_vcclrit.h"

/// обеспечивает инициализацию/деинициализацию CRT для модуля
#define SS_CRT_LOAD(MODULE_NAME)\
typedef HRESULT (*CREATE_INSTANCE)(const GUID* pGuid, void** ppBase); \
SS::Core::CommonServices::TSmartPointerOfIBase<SS::Interface::Core::ResourceManagers::ILoadManager> spLoadManager; \
\
namespace SS\
{\
	namespace MODULE_NAME\
	{\
		/* класс-загрузчик модуля*/\
		public __gc class Loader\
			: public SS::API::Loader::ILoader\
		{\
\
			static SS::Core::CommonServices::CManagerLifeTimeForCriticalSection* m_pmlt =\
				new SS::Core::CommonServices::CManagerLifeTimeForCriticalSection();\
		public:\
\
			__property   void set_bOn(bool bOn)\
			{\
				SS::Core::CommonServices::CCriticalSection oCritical(m_pmlt);\
				m_bOn = bOn;\
			}\
\
			__property   bool get_bOn()\
			{\
				return m_bOn;\
			}\
			/*Инициирует загрузку всех переменных*/\
			void Load() {\
				if(bOn) return;\
\
				bOn = true;\
\
				System::AppDomain::CurrentDomain->DomainUnload += new System::EventHandler(this, DomainUnload);\
				__crt_dll_initialize();\
				\
				const wchar_t __pin* path	= PtrToStringChars(System::AppDomain::CurrentDomain->BaseDirectory); \
				std::wstring wsPath = path; \
				wsPath += L"Core\\LoadManager.dll"; \
				HMODULE hLoadManager=::LoadLibraryW(wsPath.c_str()); \
				\
				if (!hLoadManager) \
				{ \
					System::String* pMessageException = new System::String(L"LoadManager not found, path="); \
					pMessageException = pMessageException->Concat(pMessageException, new System::String(wsPath.c_str())); \
					pMessageException = pMessageException->Concat(pMessageException, new System::String(L", assembly=")); \
					pMessageException = pMessageException->Concat(pMessageException, System::Reflection::Assembly::GetExecutingAssembly()->FullName); \
					throw new System::Exception(pMessageException);\
				} \
				CREATE_INSTANCE pCreateInstance=(CREATE_INSTANCE)GetProcAddress(hLoadManager, "CreateInstance"); \
				SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager = NULL; \
				\
				(*pCreateInstance)(&CLSID_LoadManager, (void**)&pLoadManager);\
				pLoadManager->SetWorkPath(path);\
				spLoadManager = pLoadManager; \
			}\
			/*Инициирует выгрузку всех переменных*/\
			static void DomainUnload(System::Object* sender, System::EventArgs* e)\
			{\
				__crt_dll_terminate();\
				delete m_pmlt; \
			};\
\
 \
		private:\
			static bool m_bOn = false;\
		};\
\
	}\
};
