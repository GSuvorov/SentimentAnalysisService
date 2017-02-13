#pragma once

#include "..\ASDInterface\INdxDataStorage.h"
#include "..\ASCInterface\ICommonContainers.h"
#include ".\iep_storage.h"
#include ".\texts_binary_storage.h"
#include ".\basis_storage.h"
#include ".\coordinate_counter.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{
	class CIndexCash;
	class CContainersFactory;
}
}
}
}
}

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
class CIndexStorage : public CBasisStorage
{
public:
	typedef vector<SS::Interface::Core::NdxSE::NdxDataStorage::SNdxLevelInfo const*> TCoordinateLevelInfo;

	CIndexStorage(SS::Interface::Core::NdxSE::NdxDataStorage::INdxStorage* pNdxStorage);
	virtual ~CIndexStorage(void);

	///возвращает имя индексного хранилища
	const wchar_t* GetIndexStorageName(void){return m_pNdxStorage->GetDataUnitName();};
	///возвращает таблицу точек входа индексов в координатные деревья
	CIndexEntryPointTable* GetIndexEntryPointTable(void){return m_pIndexEntryPointTable;};
	///возвращает хранилище бинарного отображения текста
	CTextsBinaryStorage* GetTextsBinaryStorage(void){return m_pTextsBinaryStorage;};
	///возвращает хранилище координат 
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* GetCoordinateStorage(void){return m_pCoordinateStorage;};

	///возвращает коллекцию хранилищ уровня координат по номеру
	TCoordinateLevelInfo* GetCoordinateLevelCollection(void){return &m_CoordinateLevelInfo;};
	///возвращает количество уровней
	unsigned int GetLevelStoragesNumber(void){return (unsigned int)m_CoordinateLevelInfo.size();};
	///возвращает номер уровня с контролем предложений
	unsigned int GetSentencesControlLevelNumber(void){return m_uiSentencesControlLevelNumber;};
	///возвращает номер уровня с контролем номеров слов
	unsigned int GetWordsControlLevelNumber(void){return m_uiWordsControlLevelNumber;};
	///возвращает размер битовой карты по номеру уровня
	unsigned int GetBitMapSizeByLevel(unsigned char ucLevel);
	///возвращает фабрику контайнеров данного индекса
	SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory* GetContainersFactory(void){return m_pContainersFactory;};

	///готовит к индексации
	void PrepareIndexing(void);
	///кладет в хранилище индекс с координатами
	void AddIndex(SS::Interface::Core::CommonContainers::IIndexAndPosition* pIndexAndPosition);
	virtual void AddIndexationIndex(SS::Interface::Core::CommonContainers::IIndexationIndex* pIndexationIndex);
	///кладет в хранилище индексы с координатами
	virtual void AddIndexes(SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut);
	///обновляет проиндексированные данные
	virtual void UpdateIndexedData(void);
	///обновляет проиндексированные данные для текста
	virtual void CompleteDocumentIndexing(void);
	///сбрасывает индексные данные в хранилища
	virtual void CompleteIndexing(void);

protected:
	///создает кэш индексов текста
	virtual void CreateIndexCash(void);
	///возвращает кэш индексов текста
	virtual SS::Core::NdxSE::NdxProcessor::Containers::CIndexCash*  GetIndexCash(void);

	///таблица точек входа индексов в координатные деревья
	CIndexEntryPointTable* m_pIndexEntryPointTable;
	///хранилище бинарных образов текстов
	CTextsBinaryStorage* m_pTextsBinaryStorage;
	///хранилище координат индексов
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* m_pCoordinateStorage;
	///коллекция уровневых хранилищ координатных деревьев
	TCoordinateLevelInfo m_CoordinateLevelInfo;

	///кэш индексов текста
	SS::Core::NdxSE::NdxProcessor::Containers::CIndexCash* m_pIndexCash;
	///фабрика контайнеров данного индекса
	SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory* m_pContainersFactory;

	///номер уровня с контролем предложений
	unsigned int m_uiSentencesControlLevelNumber;
	///номер уровня с контролем номеров слов
	unsigned int m_uiWordsControlLevelNumber;
	
	///счетчик координат
	SS::Core::NdxSE::NdxProcessor::Indexing::CCoordinateCounter m_CoordinateCounter;

	//параметры кэша
	///максимальный размер кэша
	unsigned int m_uiCashMaxSize;
	///минимальная частота при которой индекс остается в кэше
	unsigned int m_uiCashIndexMinFreq;

	///физическое хранилище индекса
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxStorage* m_pNdxStorage;
	
	///флаг ведения лога координат индексов
	bool m_bLogIndexCoordinates;
	///лог-файл координат индексов
	FILE* m_pLogIndexCoordinatesFileStream;
};

//--------------------------------------------------------------------//

}
}
}
}
}