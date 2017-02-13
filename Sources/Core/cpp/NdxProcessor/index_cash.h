#pragma once
#include ".\index_container.h"
#include ".\pool_client.h"

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
	class CIndexStorage;
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
namespace Containers
{

//--------------------------------------------------------------------//

///аттрибуты индекса в кэше
class CIndexAttributes
{
public:
	CIndexAttributes(CContainersFactory* pContainersFactory)
		:m_uiIndexEntryPoint(EMPTY_OFFSET), m_uiFreqKoef(0), m_uiAppearedTextsNumber(0), m_uiFreqInCollection(0), m_BmTree(pContainersFactory){};
	virtual ~CIndexAttributes(void){};
	
	///возвращает хранилище координат индекса
	TCoordinateContainer* const GetCoordinatesContainer(void){return &m_BmTree;};

	///возвращает точку входа индекса в хранилище уровней
	unsigned int GetIndexEntryPoint(void){return m_uiIndexEntryPoint;};
	///устанавливает точку входа индекса в хранилище уровней
	void SetIndexEntryPoint(unsigned int uiValue){m_uiIndexEntryPoint=uiValue;};

	///возвращает частоту появления индекса в текстах
	unsigned int GetAppearedTextsNumber(void){return m_uiAppearedTextsNumber;};
	///устанавливает частоту появления индекса в текстах
	void SetAppearedTextsNumber(unsigned int uiValue){m_uiAppearedTextsNumber=uiValue;};
	///увеличивает счетчик текстов в которых встретился индекс
	void OneMoreText(void){m_uiAppearedTextsNumber++;};

	///возвращает частоту появления индекса в текстах
	unsigned int GetFreqInCollection(void){return m_uiFreqInCollection;};
	///устанавливает частоту появления индекса в текстах
	void SetFreqInCollection(unsigned int uiValue){m_uiFreqInCollection=uiValue;};
	

	///возвращает частоту появления индекса
	unsigned int GetFreqKoef(void){return m_uiFreqKoef;};
	///устанавливает частоту появления индекса
	void SetFreqKoef(unsigned int uiValue){m_uiFreqKoef=uiValue;};
	///увеличивает счетчик появления индекса
	void OneMoreIndex(void){m_uiFreqKoef++; m_uiFreqInCollection++;};

protected:
	///дерево координат
	TCoordinateContainer m_BmTree;
	///смещение на данные в хранилище координат (точка входа)
	unsigned int m_uiIndexEntryPoint;
	///частотный коэффицент, им определяется удалять или нет индекс из кэша
	unsigned int m_uiFreqKoef;
	///количество документов есть данный индекс (частота в документах)
	unsigned int m_uiAppearedTextsNumber;
	///абсолютная частота индекса во всей коллекции документов
	unsigned int m_uiFreqInCollection;
};

//--------------------------------------------------------------------//

///кэш индексов индексируемой коллекции документов
class CIndexCash : public CFactoryClient<CContainersFactory>
{
public:
	typedef map<unsigned int, CIndexAttributes*> TIndexCollection;

	CIndexCash(CContainersFactory* pContainersFactory, unsigned int uiCashMaxSize, unsigned int uiIndexFreqMinSize);
	virtual ~CIndexCash(void);
	
	///обновление кэша
	virtual bool Update(void);
	///сброс кэша на диск
	void Flush(void);
	///добавление координаты к индексу в кэше
	virtual bool Add(unsigned int uiIndex, unsigned int* pCoordBuffer, unsigned int uiCoordBufferSize);
	///отображение размера кэша
	void  View();
	///отображение количества удаленных индексов
	void  ViewRemovedSize();
	///очищает кэш
	void Clear(void);

protected:
	///подымает информацию по индексу с диска
	CIndexCash::TIndexCollection::iterator MoveUpIndexInfo(unsigned int uiIndex);
	///сбрасывает информацию по индексу на диск
	void MoveDownIndexInfo(CIndexCash::TIndexCollection::iterator itIndex);

	///кэш индексов
	TIndexCollection m_IndexCash;
	
	///максимальный размер кэша, при котором он не очищается
	unsigned int m_uiCashMaxSize;
	///минимальная частота индекса при которой он выбрасывается из кэша
	unsigned int m_uiIndexFreqMinSize;
	///коллекция для инициализации итераторов
	CIndexCash::TIndexCollection m_NullCollection;
};

//--------------------------------------------------------------------//

}
}
}
}
}
