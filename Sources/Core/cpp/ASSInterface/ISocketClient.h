//ISocketClient.h
#pragma once
#include "../ASCInterface/IInterface.h"
#include "DataArray.h"

// {2346BF28-962C-4ba2-94EC-7E1B99C57527}
static const GUID CLSID_SocketClient = 
{ 0x2346bf28, 0x962c, 0x4ba2, { 0x94, 0xec, 0x7e, 0x1b, 0x99, 0xc5, 0x75, 0x27 } };

// {F9980B3B-0B69-4d39-9C12-AB5519FCBEEB}
static const GUID IID_SocketClient = 
{ 0xf9980b3b, 0xb69, 0x4d39, { 0x9c, 0x12, 0xab, 0x55, 0x19, 0xfc, 0xbe, 0xeb } };

namespace SS
{
namespace Interface
{
namespace GlobalSearch
{
namespace SocketClient
{
	///Сервер для данного клиента
	class IServer
	{
	public:
		IServer(){};
		virtual ~IServer(){};
		///Возвращает название сервера
		virtual const wchar_t* GetName() = 0;
		///Возвращает IP-адрес сервера
		virtual const wchar_t* GetIPAdress() = 0;
		///Отправляет сообщение серверу
		virtual void SendData(const char* szData, unsigned int uiSize) = 0;
		///Принимает сообщение от сервера
		virtual void ReceiveData(SS::Helpers::CDataArray<char>* pData) = 0;
	};

	///sub-сервер
	class ISocketClient : public virtual SS::Interface::IBase
	{
	public:
		ISocketClient(){};
		virtual ~ISocketClient(){};
		///Выполняет соединение с сервером
		virtual void Connect() = 0;
		///Возвращает текущий сервер
		virtual IServer* GetServer() = 0;
	};
}
}
}
}