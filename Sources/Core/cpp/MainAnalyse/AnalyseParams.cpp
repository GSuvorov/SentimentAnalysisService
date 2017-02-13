#include "stdafx.h"
#include "AnalyseParams.h"

namespace SS
{
namespace MainAnalyse
{
	CAnalyseParams::CAnalyseParams()
	{
	}

	ULONG CAnalyseParams::Release()
	{
		delete this;
		return NULL;
	};

	HRESULT CAnalyseParams::QueryInterface(REFIID pIID, void** pBase)
	{
		if(	IID_AnalyseParams == pIID)
			*pBase = (SS::Interface::Core::MainAnalyse::IAnalyseParams*) this;
		else if(IID_Base == pIID)
			*pBase = (SS::Interface::IBase*) this;
		else 
			*pBase = NULL;

		return S_OK;
	}; 
}
}
