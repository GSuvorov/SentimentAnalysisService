#include "stdafx.h"

#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\types.h"

#include "types.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{
namespace Types
{


CGuidShell::CGuidShell(GUID guid)
{

	m_guid.Data1=guid.Data1;
	m_guid.Data2=guid.Data2;
	m_guid.Data3=guid.Data3;
	memcpy((void*)m_guid.Data4, (void*)guid.Data4, 8);
}

bool CGuidShell::operator <(const CGuidShell& value) const
{

	if(m_guid.Data1<value.m_guid.Data1)
		return true;
	else if(m_guid.Data1==value.m_guid.Data1)
	{

		if(m_guid.Data2<value.m_guid.Data2)
			return true;
		else if(m_guid.Data2==value.m_guid.Data2)
		{

			if(m_guid.Data3<value.m_guid.Data3)
				return true;
			else if(m_guid.Data3==value.m_guid.Data3)
			{

				if(*(int*)&m_guid.Data4[0]<*(int*)&value.m_guid.Data4[0])
					return true;
				else if(*(int*)&m_guid.Data4[0]==*(int*)&value.m_guid.Data4[0])
				{

					return (*(int*)&m_guid.Data4[4]<*(int*)&value.m_guid.Data4[4]) ? 
						true : false;
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}
	else return false;

	return true;
}
CGuidShell::operator GUID()
{
	return m_guid;
}
CGuidShell::operator GUID() const
{
	return m_guid;
}

std::wstring CGuidShell::inToString()
{

	wchar_t buff[100];
	std::wstring sGuid=_ltow(m_guid.Data1,buff,16);
	sGuid+=L"-";
	sGuid+=_ltow(m_guid.Data2,buff,16);
	sGuid+=L"-";
	sGuid+=_ltow(m_guid.Data3,buff,16);
	sGuid+=L"-";

	int iTemp=m_guid.Data4[0];
	iTemp=iTemp<<8;
	iTemp=iTemp | m_guid.Data4[1];
	sGuid+=_ltow(iTemp,buff,16);

	iTemp=NULL;
	sGuid+=L"-";

	for(int i=2;i<6;i++)
	{
		iTemp=iTemp | m_guid.Data4[i];

		if(i!=5)
			iTemp=iTemp<<8;
	};
	sGuid+=_ltow(iTemp,buff,16);;

	iTemp=NULL;
	for(int i=6;i<8;i++)
	{
		iTemp=iTemp | m_guid.Data4[i];
		if(i!=7)
		iTemp=iTemp<<8;
	};
	sGuid+=_ltow(iTemp,buff,16);;
	return sGuid;
}

std::wstring CGuidShell::ToString() 
{

	return this->inToString();
}
std::wstring CGuidShell::ToString() const
{

	return ((CGuidShell*)this)->inToString();
}


}
}
}
}