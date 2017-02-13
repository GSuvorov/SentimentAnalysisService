#pragma once

#include "..\ASDInterface\IISContainer.h"
using namespace SS::Interface::Core;
using namespace SS::Interface::Core::CommonContainers;

#include "..\ASDInterface\IBasesManager.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASCInterface\IInterface.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASCInterface\ICommonContainers.h"
using namespace SS::Interface::Core::CommonContainers;

#include "..\ASCInterface\TCoommonTypes.h"
using namespace SS::Core::Types::IndexationSearch;

#include "..\ASCInterface\ICommonServices.h"
using namespace SS::Interface::Core::CommonServices;

#include <hash_map>
using namespace stdext;

/// хэш-мап LoadManager-ов
typedef hash_map<std::wstring, ILoadManager*> TLoadManagerMap;
/// хэш-мап QueryResults-ов
typedef hash_map<std::wstring, IQueryResult*> TQueryResultsMap;
/// хэш-мап SearchResults-ов
typedef hash_map<std::wstring, ISearchResult*> TSearchResultsMap;
/// хэш-мап LinguisticProcessor-ов
typedef hash_map<std::wstring, ILinguisticProcessor*> TLinguisticProcessorMap;


class CISContainer : public IISContainerUM
{
public:
	CISContainer();
	virtual ~CISContainer();

public:
	virtual HRESULT QueryInterface(REFIID refIID, void** pBase);
	virtual ULONG Release();

public:
	virtual void SetLoadManager(ILoadManager* pLoadManager);
	///получение котейнера анализа запроса, соответствующего имени поля wszFieldName
	virtual IQueryResult* GetQueryResult(const wchar_t* wszFieldName);
	///получение котейнера результатов поиска, соответствующего имени поля wszFieldName
	virtual ISearchResult* GetSearchResult(const wchar_t* wszFieldName);
	///получение лингвистического процессора, соответствующего имени поля wszFieldName
	virtual ILinguisticProcessor* GetLinguisticProcessor(const wchar_t* wszFieldName);
	/// очистка контейнеров результата поиска
	virtual void Clear();

private:
	void SetBase(const wchar_t* wsz);
	ILoadManager* GetNewLoadManager(const wchar_t* wszFieldName);

private:
	TLinguisticProcessorMode m_oLinguisticMode;
	std::wstring m_wszBase;

	TQueryResultsMap m_QRMap;
	TSearchResultsMap m_SRMap;
	TLinguisticProcessorMap m_LPMap;
	TLoadManagerMap m_LMMap;
};
