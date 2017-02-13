#pragma once
#include "IInterface.h"
#include <vector>
#include <string>
//--------------------------------------------------------------------//

// guid для интерфейса IBaseManager
static const GUID IID_BaseManager = 
{ 0xb191a2a8, 0xc2e9, 0x4b61, { 0xa6, 0x99, 0x4, 0x4, 0xb4, 0x98, 0xc2, 0xe6 } };

/// guid для класса, поддерживающего IBaseManager (модуль NdxBaseManager)
static const GUID CLSID_NdxBaseManager = 
{ 0x17bdada8, 0x5aba, 0x4786, { 0xa9, 0xb7, 0xe, 0xdd, 0x23, 0xb2, 0x2a, 0x77 } };

/// guid для класса, поддерживающего IBaseManager (модуль лингвистического анализа)
static const GUID CLSID_LinguoBaseManager = 
{ 0xf2441250, 0xdf53, 0x471c, { 0x8c, 0x4c, 0xe7, 0xb9, 0xbc, 0x86, 0x8d, 0xbd } };

//--------------------------------------------------------------------//

namespace SS
{
namespace Interface
{
namespace Core
{
namespace ResourceManagers
{

//--------------------------------------------------------------------//

///интерфейс управления индексными базами данных
class IBaseManager : public virtual IBase
{
public:
	///устанавливает путь к индексным базам
	virtual void SetBasePath(const wchar_t* wszPath) = 0;
	///открывает индексную базу
	virtual HRESULT OpenBase(const wchar_t* wszBaseName) = 0;
	///очищает индексную базу
	virtual HRESULT ClearBase(void) = 0;
	///зaкрывает индексную базу
	virtual void CloseBase(void) = 0;
	///осуществляет резервное копирование индексной базы
	virtual HRESULT BackUpBase(const wchar_t* wszPath) = 0;
	/// заполнит вектор путями к файлам базы (необходимо для бакапа)
	virtual void GetDatabaseFilePathes(std::vector< std::pair<std::wstring, std::wstring> >* vPathes, std::wstring& sBaseName) = 0;
};

//--------------------------------------------------------------------//
}
}
}
}