#pragma once
#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\ILinguisticProcessor.h"
#include "..\ASCInterface\TCoommonTypes.h"
#include <vector>

// {2012AE65-87CC-41d5-BE2F-3CF57DCDF2A0}
static const GUID CLSID_ISContainer = 
{ 0x2012ae65, 0x87cc, 0x41d5, { 0xbe, 0x2f, 0x3c, 0xf5, 0x7d, 0xcd, 0xf2, 0xa0 } };

// {AA60A102-DFC6-4a4b-86C0-427F45A93DF1}
static const GUID IID_ISContainer = 
{ 0xaa60a102, 0xdfc6, 0x4a4b, { 0x86, 0xc0, 0x42, 0x7f, 0x45, 0xa9, 0x3d, 0xf1 } };


namespace SS
{
namespace Interface
{
namespace Core
{
///интерфейс для работы с поисковым движком
class IISContainerUM : public virtual SS::Interface::IBase
{
public:
	///получение котейнера анализа запроса, соответствующего имени поля wszFieldName
	virtual SS::Interface::Core::CommonContainers::IQueryResult* GetQueryResult(const wchar_t* wszFieldName) = 0;
	///получение котейнера результатов поиска, соответствующего имени поля wszFieldName
	virtual SS::Interface::Core::CommonContainers::ISearchResult* GetSearchResult(const wchar_t* wszFieldName) = 0;
	///получение лингвистического процессора, соответствующего имени поля wszFieldName
	virtual SS::Interface::Core::ILinguisticProcessor* GetLinguisticProcessor(const wchar_t* wszFieldName) = 0;
	/// очистка контейнеров результата поиска
	virtual void Clear() = 0;
};
}
}
}