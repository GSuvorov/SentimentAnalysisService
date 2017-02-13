#include "StdAfx.h"

#include ".\index_stat_cash.h"
#include ".\index_stat_storage.h"

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

using namespace SS::Core::NdxSE::NdxProcessor::Storages;
using namespace SS::Core::NdxSE::NdxProcessor::MemoryManagement;

//--------------------------------------------------------------------//

CIndexStatisticCash::CIndexStatisticCash(CContainersFactory* pContainersFactory, unsigned int uiCashMaxSize, unsigned int uiIndexFreqMinSize)
:CIndexCash(pContainersFactory, uiCashMaxSize, uiIndexFreqMinSize), m_uiAddedIndexesNumber(0)
{
}

CIndexStatisticCash::~CIndexStatisticCash(void)
{
}

void CIndexStatisticCash::AddFrequentIndex(unsigned int uiIndex, unsigned int uiFreqIncreaseKoef)
{
	TFrequentIndexCollection::iterator itFrequentIndex=m_FrequentIndexCollection.find(uiIndex);
	if(itFrequentIndex==m_FrequentIndexCollection.end()){
		itFrequentIndex=m_FrequentIndexCollection.insert(TFrequentIndexCollection::value_type(uiIndex, uiFreqIncreaseKoef)).first;
	}else{
		itFrequentIndex->second+=uiFreqIncreaseKoef;
	}
}

bool CIndexStatisticCash::Add(unsigned int uiIndex, unsigned int* pCoordBuffer, unsigned int uiCoordBufferSize)
{
	//увеличиваем счетчик индексов
	m_uiAddedIndexesNumber++;
	
	//выполняем добавление
	return CIndexCash::Add(uiIndex, pCoordBuffer, uiCoordBufferSize);
}

void CIndexStatisticCash::CompleteDocumentIndexing(unsigned int uiTextIndex)
{
	TFrequentIndexCollection::iterator itFrequentIndex=m_FrequentIndexCollection.begin();
	while(itFrequentIndex!=m_FrequentIndexCollection.end()){
		//достаем индекс с координатами их кэша
		TIndexCollection::iterator itIndex=m_IndexCash.find(itFrequentIndex->first);
		if(itIndex==m_IndexCash.end()){
			//если в кэше нет поднимаем с диска
			itIndex=MoveUpIndexInfo(itFrequentIndex->first);
			if(itIndex->second->GetIndexEntryPoint()==EMPTY_VALUE){
				delete itIndex->second;
				m_IndexCash.erase(itIndex);
				itIndex=m_NullCollection.end();
			}
		}

		if(itIndex!=m_NullCollection.end()){
			//обновляем коэффицент увеличения частоты
			unsigned int CoordBuffer[5];
			CoordBuffer[1]=uiTextIndex;
			//itIndex->second->GetCoordinatesContainer()->View();
			itIndex->second->GetCoordinatesContainer()->UpdateFreqIncreaseKoef(CoordBuffer, 5, itFrequentIndex->second);
			//itIndex->second->GetCoordinatesContainer()->View();
		}

		itFrequentIndex++;
	}
	
	//очищаем коллекцию
	m_FrequentIndexCollection.clear();

	//сбрасываем счетчик индексов добавленных в кэш
	ResetAddedIndexesNumber();
}

//--------------------------------------------------------------------//

}
}
}
}
}