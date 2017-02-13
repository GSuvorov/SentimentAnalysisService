// ControlDynamicLibrary.h: interface for the CControlDynamicLibrary class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\types.h"
#include "PathMap.h"

///мап дл€ хранени€ и получени€ загруженных модулей, ключом €вл€етс€ им€ модул€ wstring
typedef std::map<std::wstring, HMODULE> MNameLibrary_HModule;


///класс дл€ загрузки модулей и создани€ классов
class  CControlDynamicLibrary  
{
private:
	///заполнение списка модулей с предопределенными id
	void Init();
	///список уже загруженных модулей
	MNameLibrary_HModule m_mNameLibrary_HModule;
	///получение библиотеки с заданным именем sNameLibrary
	HMODULE GetLibrary(std::wstring sNameLibrary, const wchar_t* wszPath);
	///выгрузка библиотеки с заданным именем sNameLibrary
	BOOL FreeLibrary(std::wstring sNameLibrary);
	///передаем в модуль hModule указатель на ILog
	void SetLog(HMODULE hModule, const wchar_t* wszPath);
	BOOL FileExists(const wchar_t* wszLibraryPath);

private:
	CPathMap m_oPathMap;

public:
	CControlDynamicLibrary();
	virtual ~CControlDynamicLibrary();

	///создает экземпл€р требуемого класса и возвращает указатель на его базовый интерфейс
	void CreateInstance(const GUID* pGuid, void** ppBase, TCoreLibrary CoreLibrary, const wchar_t* wszPath);
	///—оздает экземпл€р требуемого класса(по имени dll) и возвращает указатель на его базовый интерфейс 
	void CreateInstance(const GUID* pGuid, void** ppBase, const wchar_t* wszDllName, const wchar_t* wszPath);

	TCoreLibrary AddLibraryByPath(const wchar_t* wszPath);
};
