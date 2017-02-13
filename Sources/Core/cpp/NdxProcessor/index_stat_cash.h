#pragma once

#include ".\index_cash.h"

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
	class CIndexStatisticStorage;
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

class CIndexStatisticCash : public CIndexCash
{
public:
	CIndexStatisticCash(CContainersFactory* pContainersFactory, unsigned int uiCashMaxSize, unsigned int uiIndexFreqMinSize);
	virtual ~CIndexStatisticCash(void);

	///добавляет "частый" индекс
	void AddFrequentIndex(unsigned int uiIndex, unsigned int uiFreqIncreaseKoef);
	///добавление координаты к индексу в кэше
	bool Add(unsigned int uiIndex, unsigned int* pCoordBuffer, unsigned int uiCoordBufferSize);
	///возвращает колличество индексов добавленных в кэш
	unsigned int GetAddedIndexesNumber(void){return m_uiAddedIndexesNumber;};
	///обновляет проиндексированные данные для текста
	void CompleteDocumentIndexing(unsigned int uiTextIndex);
	
protected:
	///сбрасывает колличество индексов добавленных в кэш
	void ResetAddedIndexesNumber(void){m_uiAddedIndexesNumber=0;};

	typedef map<unsigned int, unsigned int > TFrequentIndexCollection;
	///коллекция "частых" индексов
	TFrequentIndexCollection m_FrequentIndexCollection;
	///счетчик индексов добавленных в кэш
	unsigned int m_uiAddedIndexesNumber;
};

//--------------------------------------------------------------------//

}
}
}
}
}

