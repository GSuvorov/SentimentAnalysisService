/*

=================== МОДУЛЬ ИНДЕКСНОГО ПРОЦЕССОРА NdxProcessor.dll =======================
-	Изменить тип второго параметра метода 
virtual HRESULT	INdxSearchEngineFind::Find(
	const wchar_t* wszIndexStorageName,
	SS::Interface::Core::CommonContainers::IQueryResult* pQueryResult, 
	SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams, 
	CNdxSearchSettings* pSearchSettings) = 0;
на "SS::Interface::Core::NdxSE::IParametrizedQuery*	pPrmQueryResult"



Файл NdxProcessor\search_factory.h, .cpp
-	Изменить конструктор класса CSearchFactory: поменять параметр 
"SS::Interface::Core::CommonContainers::IQueryResult* pQueryResult"
на 
"SS::Interface::Core::NdxSE::IParametrizedQuery* pPrmQueryResult"


Файл NdxProcessor\es_filler.h
-	Можно не менять шаблон es_filler.h\CESFillerEx, от которого наследуется CESFillerFromQuery


Файл NdxProcessor\es_filler_query.h, .cpp
-	Изменить конструктор класса CESFillerFromQuery, чтобы в качестве 
одного из аргументов он принимал IParametrizedQuery, 
станет так:
CESFillerFromQuery(CEvalFactory* pEvalFactory, 
	SS::Interface::Core::NdxSE::IParametrizedQuery* pPrmQueryResult,
	SS::Core::NdxSE::NdxProcessor::Storages::CIndexStorage* pIndexStorage, bool bFillOnlyObligatory);


r_logic.h, .cpp
-	Поменять параметр метода virtual void SetQuery(SS::Interface::Core::CommonContainers::IQueryResult* pQueryResult);


Файл hdr_logic.h, .cpp
-	Поменять тип параметра метода SetQuery с IQueryResult на IParametrizedQuery,
всё равно в этом методе ничего кроме проверки на NULL не происходит


=================== МОДУЛЬ ЗАГЛУШКИ NdxStub.dll =======================

Внутри метода CNdxStub::Find вставить преобразование запроса из IQueryResult в IParametrizedQuery,
с помощью модуля NDxQuery.dll
*/