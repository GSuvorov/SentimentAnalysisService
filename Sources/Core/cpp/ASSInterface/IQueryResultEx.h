//IQueryResultEx.h

#pragma once

#include "../ASCInterface/ICommonContainers.h"
#include "./IBlackBoxQuery.h"
#include "../ASCInterface/TCoommonTypes.h"

// {EC2A6A36-AD69-4193-83BD-6E2B964BDA8E}
static const GUID CLSID_QueryResultEx = 
{ 0xec2a6a36, 0xad69, 0x4193, { 0x83, 0xbd, 0x6e, 0x2b, 0x96, 0x4b, 0xda, 0x8e } };

// {6FF1591C-5599-4b7f-A7CE-CFC6E1EF65CF}
static const GUID IID_QueryResultEx = 
{ 0x6ff1591c, 0x5599, 0x4b7f, { 0xa7, 0xce, 0xcf, 0xc6, 0xe1, 0xef, 0x65, 0xcf } };


namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace CommonContainers
			{

				using SS::Interface::Core::BlackBox::Query::IQuery;
				using SS::Core::Types::IndexationSearch::TQueryParams;
				using SS::Core::Types::IndexationSearch::TLinguisticProcessorMode;

				/// реализует функциональность IQueryResult + его заполнение из IQuery
				class IQueryResultEx : virtual public IQueryResult 
				{
				public:
					///преобразует запрос (IQuery) в список CQueryIndex - ов
					virtual void SetIQuery(IQuery* pQuery) = 0;
					///возврат IQuery установленного с помошью SetIQuery
					virtual IQuery* GetIQuery() = 0;
					///установление параметров запроса
					virtual void SetQueryParams(TQueryParams* pQueryParams) = 0;
					///установление режима лингвистического процессора
					virtual void SetLinguisticProcessorMode(TLinguisticProcessorMode* pLinguisticProcessorMode) = 0;
				};			

			}
		}
	}
}