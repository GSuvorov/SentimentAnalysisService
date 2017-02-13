//ISocketServer.h
#pragma once
#include "../ASCInterface/IInterface.h"
#include "DataArray.h"

// {8C9CEFAE-3BDD-40f5-A08B-19CC5091C7DF}
static const GUID CLSID_SocketServer = 
{ 0x8c9cefae, 0x3bdd, 0x40f5, { 0xa0, 0x8b, 0x19, 0xcc, 0x50, 0x91, 0xc7, 0xdf } };

// {5C9A49E8-8C6C-46e4-BFC7-1EE2A612F551}
static const GUID IID_SocketServer = 
{ 0x5c9a49e8, 0x8c6c, 0x46e4, { 0xbf, 0xc7, 0x1e, 0xe2, 0xa6, 0x12, 0xf5, 0x51 } };

namespace SS
{
namespace Interface
{
namespace GlobalSearch
{
namespace SocketServer
{
	///Sub-сервер
	class IClient
	{
	public:
		IClient(){};
		virtual ~IClient(){};
		///Возвращает название машины клиента
		virtual const wchar_t* GetName() = 0;
		///Возвращает IP-адрес машины клиента
		virtual const wchar_t* GetIPAdress() = 0;
		///Отправляет данные клиенту
		virtual void SendData(const char* szData, unsigned int uiSize) = 0;
		///Принимает данные от клиента
		virtual void ReceiveData(SS::Helpers::CDataArray<char>* pData) = 0;
	};

	///Хранилка для клиентов
	class IClientStorage
	{
	public:
		IClientStorage(){};
		virtual ~IClientStorage(){};
		///Возвращает первого клиета
		virtual IClient* GetFirstClient() = 0;
		///Возвращает последующего клиета
		virtual IClient* GetNextClient() = 0;
		///Возвращает колличество клиентов
		virtual unsigned int GetClientCount() = 0;
	};

	///Сервер
	class ISocketServer : public virtual IClientStorage,
						  public virtual SS::Interface::IBase
	{
	public:
		ISocketServer(){};
		virtual ~ISocketServer(){};
		virtual void StartServer() = 0;
	};
}
}
}
}