#pragma once

#include "IBlackBoxQuery.h"

namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			/// отражает набор функций, требуемых от лингвистического процессора при сценарии индексации
			class ILinguisticQueryEx : public ILinguisticQuery
			{
			public:
				ILinguisticQueryEx(){}
				virtual ~ILinguisticQueryEx(){}
			public:
				///// данная функция используется при поиске для получения запроса к индексному процессору по разобранному запросу 
				///**
				//	используется для подачи запроса к индексным процессорам, принимающим на вход стандартный контейнер запроса SS
				//*/
				//virtual void GenerateQuery(
				//	/// параметры поиска
				//	SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
				//	/// параметры анализа запроса
				//	, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
				//	/// контейнер, который необходимо заполнить
				//	, SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult
				//	/// контейнер, который необходимо заполнить
				//	, SS::Interface::Core::CommonContainers::IQueryResult* pQueryResult
				//	/// критерий сохранения результата разбора текущего запроса
				//	, bool bIsRememberQuery					
				//	) = 0;

				/// возвращает последний результат анализа
				virtual SS::Interface::Core::BlackBox::Query::IQuery* GetLastQuery() = 0;
			};
		};
	}
}