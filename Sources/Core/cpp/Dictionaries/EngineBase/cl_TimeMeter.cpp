//////////////////////////////////////////////////////////////////
//	Definition of class <CSysMeter> that collects system statictic
//	NOTE: All time results are in MilliSeconds (1/1000 sec)
///////////////////////////

#include "StdAfx.h"
#include "cl_TimeMeter.h"

///////////////////////////////////
//	!!! See comments in header file
///////////////////////////////////


CSysMeter::CSysMeter()   {

	m_ErrFlag       = FALSE; // Optional line
	m_TStartedFlag  = FALSE;
	m_TActivityFlag = TRUE;

	m_TGlobal.QuadPart          = 0i64;
	m_GoodMeteringsNum.QuadPart = 0i64;
	m_BadMeteringsNum.QuadPart  = 0i64;
	m_TStart.QuadPart           = 0i64;
	m_TStop.QuadPart            = 0i64;
	m_TLastOper.QuadPart        = 0i64;

	if(!QueryPerformanceFrequency(&m_TFrequency) || m_TFrequency.QuadPart==0i64)   { // Err or not supported
		m_TActivityFlag = FALSE;
	}
}


CSysMeter::CSysMeter(LONGLONG nInitialGlobalTime,
								LONGLONG nGoodMeteringsNum,
								LONGLONG nBadMeteringsNum)   {
	this->CSysMeter::CSysMeter();
	Init(nInitialGlobalTime,nGoodMeteringsNum,nBadMeteringsNum);
}


void CSysMeter::Init(LONGLONG nInitialGlobalTime,
								LONGLONG nGoodMeteringsNum,
								LONGLONG nBadMeteringsNum)   {

	m_TGlobal.QuadPart          = nInitialGlobalTime;
	m_GoodMeteringsNum.QuadPart = nGoodMeteringsNum;
	m_BadMeteringsNum.QuadPart  = nBadMeteringsNum;
}


void CSysMeter::_Start(void)   {

	if(m_TActivityFlag)   {
		m_ErrFlag=FALSE;
		if(!m_TStartedFlag)   {
			if(QueryPerformanceCounter(&m_TStart))   {
				m_TStartedFlag  = TRUE;
			} else   {
				m_ErrFlag=TRUE;
			}
		}
	}
}

void CSysMeter::_Stop(void)   {
	if(m_TActivityFlag)   {
		if(m_ErrFlag)   {
			m_BadMeteringsNum.QuadPart++;
		} else   {
			if(m_TStartedFlag)   {
				if(QueryPerformanceCounter(&m_TStop))   {
					m_GoodMeteringsNum.QuadPart++;
					m_TLastOper.QuadPart = (m_TStop.QuadPart-m_TStart.QuadPart)/* *1000i64/m_TFrequency.QuadPart*/;
					m_TGlobal.QuadPart  += m_TLastOper.QuadPart;
					m_TStartedFlag  = FALSE;
				} else   {
					m_BadMeteringsNum.QuadPart++;
				}
			}
		}
	}
}

LONGLONG CSysMeter::GetGlobalTime(void)   {
	LONGLONG llTime;
	BOOL     bTStartedFlag = m_TStartedFlag;

	_Stop();
	llTime=m_TGlobal.QuadPart*1000i64/m_TFrequency.QuadPart;
	if(bTStartedFlag)   _Start();

	return(llTime);
}

LONGLONG CSysMeter::GetLastOperTime(void)   {
	LONGLONG llTime;
	BOOL     bTStartedFlag = m_TStartedFlag;

	_Stop();
	llTime=m_TLastOper.QuadPart*1000i64/m_TFrequency.QuadPart;
	if(bTStartedFlag)   _Start();

	return(llTime);
}

CHAR * CSysMeter::GetGlobalTimeStr(void)   {
	static char szStr[64];
	LONGLONG llTime = GetGlobalTime();
	int h,m,s,ms;

	ms = INT(llTime%1000);   llTime/=1000;
	s  = INT(llTime%60  );  llTime/=60;
	m  = INT(llTime%60  );  llTime/=60;
	h  = INT(llTime     );

	sprintf(szStr,"%ldh %2.2ldm %2.2lds %3.3ldms",h,m,s,ms);

	return(szStr);
}

CHAR * CSysMeter::GetLastOperTimeStr(void)   {
	static char szStr[64];
	LONGLONG llTime = GetLastOperTime();
	int h,m,s,ms;

	ms = INT(llTime%1000);   llTime/=1000;
	s  = INT(llTime%60  );  llTime/=60;
	m  = INT(llTime%60  );  llTime/=60;
	h  = INT(llTime     );

	sprintf(szStr,"%ldh %2.2ldm %2.2lds %3.3ldms",h,m,s,ms);

	return(szStr);
}
