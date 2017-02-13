#include "StdAfx.h"
#include ".\settings_service.h"

namespace SS
{
namespace CommonServices
{

//----------------------------------------------------------------//

HRESULT CSettingsService::QueryInterface(REFIID pIID, void** pBase) 
{
	if(pIID==IID_Base){
		*pBase=(SS::Interface::IBase*)this;
	}else if(pIID==IID_SettingsServer){
		*pBase=(SS::Interface::Core::CommonServices::ISettingsServer*)this;
	}else if(pIID==IID_SettingsServerEdit){
		*pBase=(SS::Interface::Core::CommonServices::ISettingsServerEdit*)this;
	}else{
		*pBase=NULL;
	}
	
	return S_OK;
}

ULONG CSettingsService::Release(void)
{
	delete this;
	return 0;
}


//----------------------------------------------------------------//

}
}
