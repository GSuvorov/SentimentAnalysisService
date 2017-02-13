#include "StdAfx.h"
#include "PerformanceMeasure.h"

#ifdef _SS_UNITTESTS
#include <psapi.h>
#endif

#include "..\ASCInterface\defines.h"

USES_LOAD_MANAGER;

namespace SS
{
namespace Core
{
namespace CommonServices
{


//--------------------------------------------------------------------//

CPerformanceMeasure::CPerformanceMeasure()
{

}

CPerformanceMeasure::~CPerformanceMeasure()
{

}

void CPerformanceMeasure::StartCalculation(const wchar_t* szID)
{
	TTickMeasures::iterator itMeasure=m_TickMeasures.find(szID);
	if(wcscmp(szID, L"set_Server") == 0)
	{
		int i=0;
	};
	
	if(itMeasure!=m_TickMeasures.end()){
	}else{
		itMeasure=m_TickMeasures.insert(TTickMeasures::value_type(szID, CTickMeasure())).first;
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&itMeasure->second.m_iStartTick);
	itMeasure->second.m_iStopTick=0;
	itMeasure->second.m_uiStartsCount++;
}

void CPerformanceMeasure::ClearCalculation(const wchar_t* szID)
{
	TTickMeasures::iterator itMeasure=m_TickMeasures.find(szID);
	
	if(itMeasure!=m_TickMeasures.end())
	{
		m_TickMeasures.erase(itMeasure);		
	}
}

void CPerformanceMeasure::ClearCalculations()
{
	m_TickMeasures.clear();
};

void CPerformanceMeasure::StopCalculation(const wchar_t* szID)
{
	TTickMeasures::iterator itMeasure=m_TickMeasures.find(szID);
	
	if(wcscmp(szID, L"set_Server") == 0)
	{
		int i=0;
	};

	if(itMeasure!=m_TickMeasures.end()){
		QueryPerformanceCounter((LARGE_INTEGER*)&itMeasure->second.m_iStopTick);
		itMeasure->second.m_iSumTick+=itMeasure->second.m_iStopTick-itMeasure->second.m_iStartTick;
		itMeasure->second.m_iStartTick=0;
		itMeasure->second.m_iStopTick=0;
	}
}

double CPerformanceMeasure::GetCalculation(const wchar_t* szID)
{
	TTickMeasures::iterator itMeasure=m_TickMeasures.find(szID);
	double	dbTime=0;

	if(itMeasure!=m_TickMeasures.end()){
		__int64 iFrequency;
		QueryPerformanceFrequency((LARGE_INTEGER*)&iFrequency);
		dbTime=(double)itMeasure->second.m_iSumTick/(double)iFrequency;
	}

	return dbTime;
}

void CPerformanceMeasure::ViewCalculation(const wchar_t* szID)
{
//	ToConsole(szID, GetTicks(szID));
	wchar_t buf[1024];
	swprintf(buf, L"%s: %f", szID, GetCalculation(szID));
	SAVE_LOG(buf);
}

void CPerformanceMeasure::ViewCalculations(void)
{
	wchar_t buf[500];
	for(TTickMeasures::iterator itMeasure=m_TickMeasures.begin(); itMeasure!=m_TickMeasures.end(); itMeasure++){
		swprintf(buf, L"%s: %f - %u", itMeasure->first.c_str(), GetCalculation(itMeasure->first.c_str()), itMeasure->second.m_uiStartsCount);
		SAVE_LOG(buf);
		//сбрасываем
		itMeasure->second.m_uiStartsCount=0;
		itMeasure->second.m_iSumTick=0;
		itMeasure->second.m_iStartTick=0;
		itMeasure->second.m_iStopTick=0;
	}
}


HRESULT CPerformanceMeasure::QueryInterface(REFIID pIID, void** pBase)
{
	*pBase = NULL;

	if (pIID == IID_PerformanceMeasure)
		*pBase = (SS::Interface::Core::CommonServices::IPerformanceMeasure*) this;
	else if (pIID == IID_Base)
		*pBase = (SS::Interface::IBase*) this;

	return (*pBase) ? S_OK : S_FALSE;
}; 

ULONG CPerformanceMeasure::Release()
{
	delete this;
	return NULL;
};

}

//--------------------------------------------------------------------//

}
}

