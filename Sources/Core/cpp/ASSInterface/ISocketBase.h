#pragma once

#include "..\ASCInterface\IInterface.h"
#include "DataArray.h"
#include <winsock2.h>

// {C6EE5AF7-63C3-4bf0-BB20-99D864887BE5}
static const GUID CLSID_SocketFactory = 
{ 0xc6ee5af7, 0x63c3, 0x4bf0, { 0xbb, 0x20, 0x99, 0xd8, 0x64, 0x88, 0x7b, 0xe5 } };
// {50A0DEC5-D663-4892-ABA2-35521F16C893}
static const GUID IID_SocketFactory = 
{ 0x50a0dec5, 0xd663, 0x4892, { 0xab, 0xa2, 0x35, 0x52, 0x1f, 0x16, 0xc8, 0x93 } };


namespace SS
{
namespace Interface
{
namespace Sockets
{

struct SSender
{
	sockaddr addr;
	SOCKET socket;
};

class ISocketBase
{
public:
	virtual void Release() = 0;

	virtual bool Receive(SS::Helpers::CDataArray<char>* pData) = 0;
	virtual bool SendBack(const char* pData, int iSize) = 0;

	virtual bool Receive(SS::Helpers::CDataArray<char>* pData, SSender* pSender) = 0;
	virtual bool Send(const char* pData, int iSize, SSender* pSender) = 0;
};

class ISocketFactory : 
		public virtual SS::Interface::IBase
{
public:
	virtual bool Startup() = 0;
	virtual void Cleanup() = 0;

	virtual ISocketBase* CreateUDPSocket(USHORT shPortReceive, USHORT shPortSend) = 0;
	virtual ISocketBase* CreateTCPSocket(USHORT shPortReceive) = 0;
	virtual ISocketBase* CreateFromIni(LPCWSTR szIni, LPCWSTR szSection, LPCWSTR szDefProto, 
		int iDefRcvPort, int iDefSndPort) = 0;
};

}
}
}
