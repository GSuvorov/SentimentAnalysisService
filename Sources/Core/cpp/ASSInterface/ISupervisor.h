#pragma once

#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASSInterface\IGSPluginManager.h"

#include <string>

// {438C6FE7-5665-4d38-B3E6-74EE340F6FF3}
static const GUID CLSID_Supervisor = 
{ 0x438c6fe7, 0x5665, 0x4d38, { 0xb3, 0xe6, 0x74, 0xee, 0x34, 0xf, 0x6f, 0xf3 } };
// {6237394C-5968-4e9c-BAFA-9D28E5E44FC7}
static const GUID IID_Supervisor = 
{ 0x6237394c, 0x5968, 0x4e9c, { 0xba, 0xfa, 0x9d, 0x28, 0xe5, 0xe4, 0x4f, 0xc7 } };


namespace SS
{
namespace Interface
{
namespace Supervisor
{

class IIndexBase
{
public:
	///открывает базу, при этом предыдуща€ открыта€ база закрываетс€
	virtual bool Open() = 0;
	///закрывает базу
	virtual void Close() = 0;
	///удал€ет базу
	virtual void Remove() = 0;
	/// возвращает путь к базе данных
	virtual LPCWSTR GetPath() = 0;
	///возвращает уникальное им€ базы (напр. IndX7Z7N700OYALNO0X)
	virtual LPCWSTR GetName() = 0;
	///возвращает описание базы (неуникальное)
	virtual LPCWSTR GetDescription() = 0;
	///устанавливает описание базы
	virtual bool SetDescription(LPCWSTR szDescription) = 0;
	///провер€ет, €вл€етс€ ли база базой знаний
	virtual BOOL IsKnowledgeBase() = 0;
	///устанавливает флаг базы знаний
	virtual bool SetKnowledgeBase(BOOL bSet) = 0;
};

class IDatabaseList
{
public:
	///возвращает первую базу
	virtual IIndexBase* GetFirstBase() = 0;
	///возвращает следующую базу
	virtual IIndexBase* GetNextBase() = 0;
	///ищет базу по описанию
	virtual IIndexBase* FindBase(LPCWSTR szDescription) = 0;
	///создает новую базу
	virtual IIndexBase* CreateBase(LPCWSTR szDescription) = 0;
};

class IPluginInfo
{
public:
	///получить им€ разборщика
	virtual LPCWSTR GetName() = 0;
	///включить или выключить разборщик
	virtual void Select(bool bSet) = 0;
	///проверить включен ли разборщик
	virtual bool IsSelected() = 0;
};

class IPluginList
{
public:
	///получение первого разборщика текста
	virtual IPluginInfo* GetFirst() = 0;
	///получение следующего разборщика текста
	virtual IPluginInfo* GetNext() = 0;
};

class ILocalSearchEngine
{
public:
	///локальный поиск по выбранной базе
	virtual bool Search(LPCWSTR szQuery, 
		SS::Interface::Core::CommonContainers::ISearchResult** ppSearchResult) = 0;
	///загрузка системы и кеширование файлов
	virtual bool Load(LPCWSTR szQuery) = 0;
};

class IIndexationEngine
{
public:
	///проиндексировать файл или директорию
	virtual bool Indexate(LPCWSTR szPath) = 0;
};

//-----------------------------------------------------------√лобальный поиск-------------------------------------------------------//

///ќдномашинный глобальный поиск
class IGlobalSingleSearch
{
public:
	IGlobalSingleSearch(){};
	virtual ~IGlobalSingleSearch(){};
	///√лобальный поиск в массиве текстов, результаты по каждому тексту группируютс€ в текстблоки
	virtual bool Search(LPCWSTR wzQuery, LPCWSTR* rszText, int iCount,
						SS::Interface::Core::CommonContainers::ISearchResult** ppSearchResult) = 0;
	///√лобальный поиск в тексте
	virtual bool Search(LPCWSTR wzQuery, LPCWSTR szText, 
						SS::Interface::Core::CommonContainers::ISearchResult** ppSearchResult) = 0;
	///√лобальный поиск по запросу с с++ плагинами
	virtual bool Search(const wchar_t* wzQuery, 
						SS::Interface::Core::CommonContainers::ISearchResult** ppSearchResult) = 0;
	///ѕолучить список плагинов
	virtual void GetPluginList(std::list<SS::Interface::GSPlugins::IGSPlugin*> & lstPlugins) = 0;
	virtual void GetQueryKeywords(LPCWSTR szQuery, std::vector<std::wstring>* pKeywords, 
			std::vector<std::vector<std::wstring> >* pMorphoForms) = 0;
};

///ћногомашинный глобальный поиск
class IGlobalMultiSearch
{
public:
	IGlobalMultiSearch(){};
	virtual ~IGlobalMultiSearch(){};
	///ѕоиск выполн€емый на сервере
	virtual void ServerSearch(const wchar_t* wzQuery, std::wstring & wPluginXml) = 0;
	///ѕоиск выполн€емый на клиентах (sub-серверах)
	virtual void ClientSearch(const wchar_t* wzQuery, const wchar_t* wzPluginXml,
							  SS::Interface::Core::CommonContainers::ISearchResult** ppSearchResult) = 0;
	///ѕолучить список плагинов
	virtual void GetPluginList(std::list<SS::Interface::GSPlugins::IGSPlugin*> & lstPlugins) = 0;
};

//----------------------------------------------------------------—упервизор-------------------------------------------------------//

class ISupervisor : public virtual IBase
{
public:
	///возвращает список существующих индексных баз
	virtual IDatabaseList* GetBasesList() = 0;
	///получить список загруженных разборщиков текста
	virtual IPluginList* GetPluginList() = 0;
	///получить интерфейс локального поиска
	virtual ILocalSearchEngine* GetLocalSearchEngine() = 0;
	///получить интерфейс глобального одномашинного поиска
	virtual IGlobalSingleSearch* GetGlobalSingleSearch() = 0;
	///получить интерфейс глобального многомашинного поиска
	virtual IGlobalMultiSearch* GetGlobalMultiSearch() = 0;
	///получить интерфейс индексации
	virtual IIndexationEngine* GetIndexationEngine() = 0;
};

}
}
}
