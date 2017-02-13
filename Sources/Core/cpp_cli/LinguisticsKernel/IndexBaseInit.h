#pragma once

#include "../../cpp/ASCInterface/ILinguisticProcessor.h"
#include "../../cpp/ASCInterface/IBaseManager.h"

const std::wstring wszDataBaseName = L"NewWordForms1";

class CIndexBaseInit : public SS::Core::CommonServices::CBaseCoreClass
{
	SS::Interface::Core::ResourceManagers::IBaseManager* m_pBaseManager;	
public:
	CIndexBaseInit(void) : m_pBaseManager(NULL)
	{
	}
	~CIndexBaseInit(void)
	{
		EndSession();
	}

public:
	void BeginSession()
	{
		SS_TRY
		{
			SS::Interface::Core::CommonServices::IWorkRegister* pWorkRegister =
				((SS::Interface::Core::CommonServices::IWorkRegister*) 
				GetLoadManager()->GetInterface(L"./core/LoadManager.dll",CLSID_WorkRegister,IID_WorkRegister));
			
			std::wstring m_wsDataBasePath = pWorkRegister->GetParameter(L"PathDataBase");		

			m_pBaseManager = (SS::Interface::Core::ResourceManagers::IBaseManager*)
				GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_BaseManager);
			if(!m_pBaseManager)
				SS_THROW(L"!m_pBaseManager");
			m_pBaseManager->SetBasePath(m_wsDataBasePath.c_str());
			m_pBaseManager->OpenBase(wszDataBaseName.c_str());
		}
		SS_CATCH(L"")
	}

protected:
	void EndSession()
	{
		SS_TRY
		{
			m_pBaseManager->CloseBase();
		}
		SS_CATCH(L"")
	}

};
