#ifndef DRIVER

#ifndef _LIB
	#include "StdAfx.h"
#endif

#include "cl_SyncroObjects.h"

/*
CritSectC::CritSectC(void)    { memset(&CritSect,0,sizeof(CRITICAL_SECTION)); InitializeCriticalSection(&CritSect);   }
CritSectC::~CritSectC(void)   { DeleteCriticalSection(&CritSect); }
void CritSectC::Enter(void)   { EnterCriticalSection(&CritSect); }
void CritSectC::Leave(void)   { LeaveCriticalSection(&CritSect); }

VisitCritSectC::VisitCritSectC(CritSectC & rCritSect)   {
	CritSectPtr=&rCritSect;
	if(CritSectPtr)   CritSectPtr->Enter();
}

VisitCritSectC::VisitCritSectC(CritSectC * pCritSect)   {
	CritSectPtr= pCritSect;
	if(CritSectPtr)   CritSectPtr->Enter();
}


VisitCritSectC::~VisitCritSectC(void)   {
	if(CritSectPtr)   CritSectPtr->Leave();
}

void VisitCritSectC::AbortVisitCritSect(void)   {
	if(CritSectPtr)   CritSectPtr->Leave();
	CritSectPtr=NULL;
}
*/

SyncObjC::SyncObjC(void)   {
	Hnd=NULL;
}

SyncObjC::~SyncObjC(void)   {
	if(Hnd)   {
		CloseHandle(Hnd);
		Hnd=NULL;
	}
}


AMEventC::AMEventC(bool bManual) :
			SyncObjC()   {

	if((Hnd=CreateEvent(NULL,bManual,FALSE,NULL))==NULL) throw EXEPTION_NO_SYNC_OBJECT;

}

AMEventC::~AMEventC(void)   {
}

void AMEventC::Set(void)   {
	if(Hnd==NULL)   throw EXEPTION_NULL_OBJECT_OPERATION;
	SetEvent(Hnd);
}

void AMEventC::Reset(void)   {
	if(Hnd==NULL)   throw EXEPTION_NULL_OBJECT_OPERATION;
	ResetEvent(Hnd);
}

void AMEventC::Pulse(void)   {
	if(Hnd==NULL)   throw EXEPTION_NULL_OBJECT_OPERATION;
	PulseEvent(Hnd);
}


SemaphoreC::SemaphoreC(void) :
				SyncObjC()   {
	MaxCount=0;
}

SemaphoreC::~SemaphoreC(void)   {
}

void SemaphoreC::Make(long nMaxCount, long nInitCount)   {
	if(Hnd!=NULL)   throw EXEPTION_NOT_NULL_OBJECT_CREATION;

	MaxCount=nMaxCount;
	if((Hnd=CreateSemaphore(NULL,nInitCount,nMaxCount,NULL))==NULL) throw EXEPTION_NO_SYNC_OBJECT;
}

long SemaphoreC::GetCount(void)   {
	if(Hnd==NULL)   throw EXEPTION_NOT_NULL_OBJECT_CREATION;

	if(WaitForSingleObject(Hnd,0)==WAIT_OBJECT_0)   {
		LONG       OldCount;
		Release(1,&OldCount);
		return(OldCount+1);
	} else   {
		return(0);
	}
}

long SemaphoreC::Release(long lReleaseCount, long * lpPreviousCount)   {
	LONG OldCount,MaxRelCount;

	if(Hnd==NULL)   throw EXEPTION_NULL_OBJECT_OPERATION;

	if(lReleaseCount > (MaxRelCount=(MaxCount-GetCount())))   lReleaseCount = MaxRelCount;
	if(lReleaseCount > 0)   {
		ReleaseSemaphore(Hnd,lReleaseCount,&OldCount);
		if(lpPreviousCount)   *lpPreviousCount=OldCount;
		return(OldCount+lReleaseCount);
	} else   {
		if(lpPreviousCount)   *lpPreviousCount=MaxCount;
		return(MaxCount);
	}
}

void SemaphoreC::Restore(void)   {
	LONG OldCount,IncToFullCount;

	if(Hnd==NULL)   throw EXEPTION_NULL_OBJECT_OPERATION;

	WaitForSingleObject(Hnd,0);
	Release(1,&OldCount);
	if((IncToFullCount=MaxCount-(OldCount+1))!=0)   Release(IncToFullCount);
}

#endif // DRIVER is undefined
