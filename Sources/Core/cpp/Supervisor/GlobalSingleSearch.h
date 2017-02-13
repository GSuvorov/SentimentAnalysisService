//GlobalSingleSearch.h
#pragma once
#include "..\ASSInterface\ISupervisor.h"
using namespace SS::Interface::Supervisor;
#include "GlobalSearchEngine.h"

namespace GlobalSearch
{

///Одномашинный глобальный поиск
class CGlobalSingleSearch : public IGlobalSingleSearch,
							public CGlobalSearchEngine
{
public:
	CGlobalSingleSearch(){};
	~CGlobalSingleSearch(){};
	///Глобальный поиск в массиве текстов, результаты по каждому тексту группируются в текстблоки
	bool Search(LPCWSTR wzQuery, LPCWSTR* rszText, int iCount, ISearchResult** ppSearchResult);
	///Глобальный поиск в тексте
	bool Search(LPCWSTR wzQuery, LPCWSTR szText, ISearchResult** ppSearchResult);
	///Глобальный поиск по запросу с с++ плагинами
	bool Search(const wchar_t* wzQuery, ISearchResult** ppSearchResult);
	///Получить список плагинов
	void GetPluginList(std::list<IGSPlugin*> & lstPlugins);
	virtual void GetQueryKeywords(LPCWSTR szQuery, std::vector<std::wstring>* pKeywords, 
			std::vector<std::vector<std::wstring> >* pMorphoForms);
};

}
