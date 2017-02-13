#pragma once

#include ".\index_stat_storage.h"
#include ".\hdr_containers.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Storages
{

//--------------------------------------------------------------------//

///индексное хранилище
class CIndexStorageHeaders : public CIndexStatisticStorage
{
public:
	CIndexStorageHeaders(SS::Interface::Core::NdxSE::NdxDataStorage::INdxStorageWithStatistic* pNdxStorage);
	virtual ~CIndexStorageHeaders(void);

	///кладет в хранилище индексы с координатами
	void AddIndexes(SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut);
	///кладет в хранилище индекс с координатами
	void AddIndexationIndex(SS::Interface::Core::CommonContainers::IIndexationIndex* pIndexationIndex);
	
	///обновляет проиндексированные данные для текста
	void CompleteDocumentIndexing(void);
	///заполняет позиции заголовка
	void FillHeaderPositions(unsigned int uiDocumentIndex, 
		Containers::CLevelHeaderID* pLevelHeaderID, Containers::SHeaderPositions* pOutHeaderPositions);
	///заполняет блок текста по интервалу
	void FillHeadersToTextBlock(SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock, 
			Containers::CIntervalAndHeader* pInterval, unsigned int uiGroupID);
	///заполняет структуру загловков текста
	Containers::CDocumentHeadersInfo* FillHeadersInfo(unsigned int uiDocumentIndex);

protected:
	///идентификатор текущего заголовка
	Containers::CDocumentHeadersInfo m_DocumentHeadersInfo;
	
	//для извлечения информации по заголовкам
	///индекс последнего запрашиваемого документа
	unsigned int m_uiLastFilledDocumentIndex;

	///хранилище информации по заголовкам
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* m_pHeadersInfoStorage;
};

//--------------------------------------------------------------------//

}
}
}
}
}