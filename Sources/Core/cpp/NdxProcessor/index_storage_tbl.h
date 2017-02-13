#pragma once

#include ".\index_stat_storage.h"
#include ".\tbl_containers.h"

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
class CIndexStorageTables : public CIndexStatisticStorage
{
public:
	CIndexStorageTables(SS::Interface::Core::NdxSE::NdxDataStorage::INdxStorageWithStatistic* pNdxStorage);
	virtual ~CIndexStorageTables(void){};
	
	///кладет в хранилище индексы с координатами
	void AddTableIndexes(SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult);
	///обновляет проиндексированные данные для текста
	void CompleteDocumentIndexing(void);
	///заполняет позиции таблицы по идентификатору
	void FillTablePositions(unsigned int uiDocumentIndex, 
		unsigned int uiTableIndex, Containers::STablePositions* pOutTablePositions, unsigned int uiHeadRowIndex = 0, unsigned int uiTailRowIndex = EMPTY_VALUE);
	///заполняет блок текста по интервалу
	void FillTablesToTextBlock(SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock, 
			Containers::CInterval* pInterval, unsigned int uiGroupID);
	///заполняет строковую структуру таблиц документа
	void FillTablesStructure(unsigned int uiTextIndex, std::wstring* pwsTablesStructure);

protected:
	typedef vector<Containers::STablePositions> TTablePositionsCollection;

	///возвращает число катов в блоке
	unsigned int GetTextBlockCutCount(SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock);
	///возвращает стартовую позицию ячейки в текущей строке
	unsigned int GetCellTailPosition(unsigned int uiCellIndex);

	///заполняет позиции ячеек строки таблицы документа
	void FillTablePositions(unsigned int uiDocumentIndex, unsigned int uiTableIndex, unsigned int uiRowIndex);
	///кладет в хранилище индекс с координатами
	void AddIndexationIndex(SS::Interface::Core::CommonContainers::IIndexationIndex* pIndexationIndex);

	///текущий TRC-идентификатор
	Containers::CTrcID m_CurrentTrcID;

	///коллекция смещения на данные по таблицам текущего документа
	vector<Containers::STableInfo> m_TablesOffsets;
	///коллекция позиций строк текущей таблицы
	vector<unsigned int> m_CellPositions;

	//для извлечения информации по заголовкам
	///индекс последнего запрашиваемого документа
	unsigned int m_uiLastFilledDocumentIndex;
	unsigned int m_uiLastFilledTableIndex;
	unsigned int m_uiLastFilledRowIndex;

	///хранилище информации по таблицам
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* m_pTablesInfoStorage;
};

//--------------------------------------------------------------------//

}
}
}
}
}