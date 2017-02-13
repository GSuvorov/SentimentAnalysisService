#pragma once

#include "..\ASCInterface\IBaseManager.h"

#include ".\DBMSManager.h"
#include ".\SQLManager.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{

class CBaseManager 
	: public SS::Interface::Core::ResourceManagers::IBaseManager
{
	std::wstring m_wszBaseName;
	std::wstring m_wszPath;
protected:
	CDBMSManager m_oDBMSManager;
	CSQLManager m_oSQLManager;
public:
	CBaseManager(void);
	~CBaseManager(void);
public:
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** ppBase); 
	///освобождение
	ULONG Release();

public:
	///устанавливает путь к индексным базам
	void SetBasePath(const wchar_t* wszPath);
	///открывает индексную базу
	HRESULT OpenBase(const wchar_t* wszBaseName);
	///очищает индексную базу
	HRESULT ClearBase(void);
	///зaкрывает индексную базу
	void CloseBase(void);
	///осуществляет резервное копирование индексной базы
	HRESULT BackUpBase(const wchar_t* wszPath);
	/// заполнит вектор путями к файлам базы (необходимо для бакапа)
	void GetDatabaseFilePathes(std::vector< std::pair<std::wstring, std::wstring> >* pvPathes, std::wstring& sBaseName);
};

}
}
}