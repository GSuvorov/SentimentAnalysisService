#pragma once

#pragma once

#ifdef _SS_UNITTESTS

#include ".\unittestsusings.h"
#include "../ASCInterface/ICommonContainers.h"
#include "..\ASDInterface\TNdxSearchEngineTypes.h"
#include "..\ASDInterface\IISContainer.h"
#include "..\ASDInterface\INdxQuery.h"

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{

//--------------------------------------------------------------------//

class CCommonContainersUsing : public CUnitTestsUsings
{
public:
	CCommonContainersUsing(void):m_pIndexationResult(NULL), m_pSearchResult(NULL), m_pQueryResult(NULL), m_pPrmQueryResult(NULL), m_pISContainer(NULL){};

protected:
	///возвращает менеджер загрузки
	SS::Interface::Core::ResourceManagers::ILoadManager* GetLoadManager(void);
	///возвращает контайнер для индексации
	SS::Interface::Core::CommonContainers::IIndexationResult* GetIndexationResult(void){return m_pIndexationResult;};
	///возвращает контайнер для поиска
	SS::Interface::Core::CommonContainers::ISearchResult* GetSearchResult(void){return m_pSearchResult;};
	///возвращает контайнер для запроса
	SS::Interface::Core::CommonContainers::IQueryResult* GetQueryResult(void){return m_pQueryResult;};
	///возвращает контейнер для оптимизированного запроса (он же оптимизатор запроса)
	SS::Interface::Core::NdxSE::IParametrizedQuery* GetPrmQueryResult(void){return m_pPrmQueryResult;};
	///возвращает контайнер для индексации полей документов
	SS::Interface::Core::NdxSE::TNdxMetaFieldCollection* GetNdxMetaFieldCollection(void){return &m_NdxMetaFieldCollection;};
	///возвращает контайнер для поиска
	SS::Interface::Core::IISContainerUM* GetISContainer(void){return m_pISContainer;};

	//наполняет для индексации данными
	void StartFillIndexationResult(void);
	void FillIndexationResult(void);
	void FillIndexationResultTable(void);
	void InitMetaFieldCollection(void);
	void FillMetaFieldCollection(void);
	void ViewMetaFieldCollection(void);

	///определяет структуру документа
	void InitDocumentStructure(void);
	///заполянет контайнеры индексации в соответствиии со струкутрой документа
	void FillIndexationResultByStructure(unsigned int uiBlockNumber);

	void GetQueries(wstring& wsFilePath, vector<wstring>& Queries);
	void QueryResultToString(wstring* pwszQueryResult);

	//наполняет для запроса данными
	void FillQueryResult(void);
	void NextText(void){m_uiCurrTextNumber++; m_uiCurrPosition=0;};

	void SetCurrentQuery(const wchar_t* pwszQuery){if(pwszQuery) m_wszCurrentQuery.assign(pwszQuery);};
private:
	void MakeQueryFromString(wstring& wszValue);
	unsigned int GetTogetherCharsNumber(wchar_t* wszToken, wchar_t wsChar);
	void MakeQueryIndexFromString(wstring& wszValue, char chOperator);
	void FillTexBlockCut(SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut, unsigned int uiIndexesCount);

	///текущий номер текста
	unsigned int m_uiCurrTextNumber;
	///текущий номер текста
	unsigned int m_uiCurrPosition;
	///контайнер для индексации
	SS::Interface::Core::CommonContainers::IIndexationResult* m_pIndexationResult;
	///контайнер для поиска
	SS::Interface::Core::CommonContainers::ISearchResult* m_pSearchResult;
	///контайнер для запроса
	SS::Interface::Core::CommonContainers::IQueryResult* m_pQueryResult;
	///контайнер для оптимизированного запроса
	SS::Interface::Core::NdxSE::IParametrizedQuery* m_pPrmQueryResult;
	///контайнер для индексации полей документов
	SS::Interface::Core::NdxSE::TNdxMetaFieldCollection m_NdxMetaFieldCollection;
	///контайнер для поиска
	SS::Interface::Core::IISContainerUM* m_pISContainer;
	///строковое представление текущего запроса
	wstring m_wszCurrentQuery;
	///структуры документа
	vector<wstring> m_DocumentStructure;
};

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS
