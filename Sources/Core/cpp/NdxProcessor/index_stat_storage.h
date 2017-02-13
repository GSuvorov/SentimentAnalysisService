#pragma once

#include ".\index_storage.h"
#include ".\norm_storage.h"
#include ".\freq_storage.h"
#include ".\index_stat_cash.h"


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

///индексное хранилище с поддержкой хранения статистики
class CIndexStatisticStorage : public CIndexStorage 
{
public:
	typedef map<unsigned int,unsigned int> TFrequencyCollection;
	typedef map<unsigned int, float> TIndexFreqCollection;

	CIndexStatisticStorage(SS::Interface::Core::NdxSE::NdxDataStorage::INdxStorageWithStatistic* pNdxStorage);
	virtual ~CIndexStatisticStorage(void);

	///кладет в хранилище индексы с координатами
	void AddIndexes(SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut);

	///возвращает хранилище частоты вхождения индекса в документ
	CIndexFrequencyStorage* GetIndexFrequencyStorage(void){return m_pIndexFrequencyStorage;};
	///возвращает хранилище норм документов
	CDocNormStorage* GetDocNormStorage(void){return m_pDocNormStorage;};
	///обновляет проиндексированные данные для текста
	void CompleteDocumentIndexing(void);

protected:
	///создает кэш индексов текста
	void CreateIndexCash(void);
	///возвращает кэш индексов текста
	SS::Core::NdxSE::NdxProcessor::Containers::CIndexStatisticCash*  GetIndexCash(void);

	///расчёт нормирующих коэффициентов проиндексированных документов 
	void ComputeDocumentsNorm(void);
	///выполняет начальные предвычисления нормирующих коэффициентов
	bool PreComputeDocumentNorm(unsigned int uiNumberOfDocuments,  TFrequencyCollection& IndexesDocumentFreqCollection);
	///вычисляет нормирующий коэффициент для данного текста, по входным частотам его индексов
	unsigned int ComputeDocumentNorm(TFrequencyCollection& IndexesFreqInTextCollection);
	
	///хранилище частоты вхождения индекса в документ
	CIndexFrequencyStorage* m_pIndexFrequencyStorage;
	///хранилище норм документов
	CDocNormStorage* m_pDocNormStorage;
	///колекция обратных логарифмированных частот документов на индекс
	TIndexFreqCollection m_IndexInverseDocFreqCollection;
};

//--------------------------------------------------------------------//

}
}
}
}
}