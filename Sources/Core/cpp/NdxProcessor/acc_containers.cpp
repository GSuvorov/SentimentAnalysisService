#include "StdAfx.h"
#include ".\acc_containers.h"
#include <float.h>

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

//--------------------------------------------------------------------//

CAccumulator::CAccumulator(unsigned int uiNumber)
{
	m_uinMean=0;
	m_uiNumberOfElements=uiNumber;
	m_isEmpty=false;
	if ( uiNumber>0) 
	{
		m_vfMax.assign(uiNumber,0.0);
		m_vfMin.assign(uiNumber,FLT_MAX);
		m_vfMean.assign(uiNumber,0.0);
	}
}
void CAccumulator::Init(int uiNumber)
{
	m_uinMean=0;
	m_isEmpty=false;
	if ( uiNumber>0 && 
		m_uiNumberOfElements != uiNumber) 
	{
		m_uiNumberOfElements=uiNumber;
		m_vfMean.resize(uiNumber,0.0);
		m_vfMin.resize(uiNumber,FLT_MAX);
		m_vfMax.resize(uiNumber,0.0);
	}else if ( uiNumber == 0 ||
		m_uiNumberOfElements == uiNumber )
	{
		m_vfMax.assign(m_uiNumberOfElements,0.0);
		m_vfMin.assign(m_uiNumberOfElements,FLT_MAX);
		m_vfMean.assign(m_uiNumberOfElements,0.0);
	}
}

CAccumulator& CAccumulator::operator=(const float fVal)
{
	unsigned int nMin=(unsigned int)m_vfMin.size(), nMax=(unsigned int)m_vfMax.size(), nMean=(unsigned int)m_vfMean.size();
	if ( nMax != nMin && 
		 nMin != nMean ) 
		 return *this;
	m_vfMax.assign(m_uiNumberOfElements,fVal);
	m_vfMin.assign(m_uiNumberOfElements,fVal);
	m_vfMean.assign(m_uiNumberOfElements,fVal);
	m_uinMean=(unsigned int)(fVal);
	return *this;
}

bool CAccumulator::isEmpty()
{
	return m_isEmpty;
}
bool CAccumulator::MergeAccumulators(CAccumulator& oAcc)
{
	unsigned int i,n=m_uiNumberOfElements;
	if ( n != oAcc.m_uiNumberOfElements || n==0 ) 
		return false;
	m_uinMean+=oAcc.m_uinMean;
	for ( i=0;i<n;i++) 
	{
		m_vfMean[i]=m_vfMean[i]+oAcc.m_vfMean[i];
		if ( m_vfMin[i] > oAcc.m_vfMin[i] ) 
			m_vfMin[i]=oAcc.m_vfMin[i];
		if ( m_vfMax[i] < oAcc.m_vfMax[i] ) 
			m_vfMax[i]=oAcc.m_vfMax[i];
	}
	return true;
}
bool CAccumulator::EvalA4ORFromAccumulator(float fBeta)
{
	unsigned int i;
	if ( m_uiNumberOfElements == 0 ) 
		return false;
	for ( i=0; i<m_uiNumberOfElements; i++) 
	{
		m_vfMean[i]=fBeta*(m_vfMax[i]) + (1-fBeta)*(m_vfMean[i])/m_uinMean;
		m_vfMin[i]=m_vfMean[i];
		m_vfMax[i]=m_vfMean[i];
	}
	m_uinMean=1;
	return true;
}
bool CAccumulator::EvalA4ANDFromAccumulator(float fBeta)
{
	unsigned int i;
	if ( m_uiNumberOfElements == 0 ) 
		return false;
	for ( i=0; i<m_uiNumberOfElements; i++) 
	{
		m_vfMean[i]=fBeta*(m_vfMin[i]) + (1-fBeta)*(m_vfMean[i])/m_uinMean;
		m_vfMax[i]=m_vfMean[i];
		m_vfMin[i]=m_vfMean[i];
	}
	m_uinMean=1;
	return true;
}

void CAccumulator::ToString(wstring& szValue)
{
	szValue.append(L"//-----CAccumulator:\n");

	wchar_t buf[255];

	for (unsigned int i=0; i<m_uiNumberOfElements; i++) 
	{
		if(m_vfMean[i]!=0.0){
			if(m_vfMin[i]==FLT_MAX){
				wsprintf(buf, L"%.3f %.3f %s", m_vfMax[i], m_vfMean[i], L"FLT_MAX");
			}else{
				wsprintf(buf, L"%.3f %.3f %.3f", m_vfMax[i], m_vfMean[i], m_vfMin[i]);
			}
			szValue.append(buf);
			szValue.append(L"\n");
		}
	}

	szValue.append(L"\n//-End-of-CAccumulator\n");
}

void CAccumulator::View(void)
{
	wstring szValue;
	ToString(szValue);
	ToConsole(szValue.c_str());
}

//--------------------------------------------------------------------//

}
}
}
}
}