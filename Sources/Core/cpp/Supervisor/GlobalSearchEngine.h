//GlobalSearchEngine.h
#pragma once
#include "GlobalSearchData.h"

namespace GlobalSearch
{
	///Функциональность глобального поиска
	class CGlobalSearchEngine : public CGlobalSearchData
	{
	protected:
		TSearchParams m_oSearchParams;
		TQueryParams  m_oQueryParams;

	public:
		CGlobalSearchEngine(){};
		~CGlobalSearchEngine(){};
		///Генерация запроса
		bool GenerateQuery(const wchar_t* wzQuery);
		///Возвращает строку содержащую xml полученый из плагинов
		void PluginSearch(std::wstring & wPluginXml);
		///Поиск по результатам плагинов
		void LinguisticSearch(const wchar_t* wzPluginXml, ISearchResult** ppSearchResult);
		///Получить список плагинов
		void GetPluginList(std::list<IGSPlugin*> & lstPlugins);
		///Поиск по кэшу
		void CacheSearch();
	};
}