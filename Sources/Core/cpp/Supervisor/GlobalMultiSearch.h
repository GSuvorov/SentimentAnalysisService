//GlobalMultiSearch.h
#pragma once
#include "..\ASSInterface\ISupervisor.h"
using namespace SS::Interface::Supervisor;
#include "GlobalSearchEngine.h"

namespace GlobalSearch
{
	///Многомашинный поиск
	class CGlobalMultiSearch : public IGlobalMultiSearch,
							   public CGlobalSearchEngine
	{
	public:
		CGlobalMultiSearch(){};
		~CGlobalMultiSearch(){};
		///Поиск выполняемый на сервере
		void ServerSearch(const wchar_t* wzQuery, std::wstring & wPluginXml);
		///Поиск выполняемый на клиентах (sub-серверах)
		void ClientSearch(const wchar_t* wzQuery,
						  const wchar_t* wzPluginXml, ISearchResult** ppSearchResult);
		///Получить список плагинов
		void GetPluginList(std::list<IGSPlugin*> & lstPlugins);
	};
}