#include "StdAfx.h"
#include ".\index_stat_storage.h"
#include ".\index_stat_cash.h"
#include <math.h>

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

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;
using namespace SS::Interface::Core::CommonContainers;

//--------------------------------------------------------------------//
CIndexStatisticStorage::CIndexStatisticStorage(INdxStorageWithStatistic* pNdxStorage)
:CIndexStorage(pNdxStorage),
m_pIndexFrequencyStorage(NULL), m_pDocNormStorage(NULL)
{
	if(ISNULL(m_pNdxStorage)) return;
	
	m_pIndexFrequencyStorage=new CIndexFrequencyStorage(pNdxStorage->GetIndexFrequencyStorage());
	m_pDocNormStorage=new CDocNormStorage(pNdxStorage->GetDocumentNormStorage());
}

CIndexStatisticStorage::~CIndexStatisticStorage(void)
{
	if(m_pIndexFrequencyStorage) delete m_pIndexFrequencyStorage;
	if(m_pDocNormStorage) delete m_pDocNormStorage;
}

void CIndexStatisticStorage::CreateIndexCash(void)
{
	if(m_pIndexCash){
		TO_CONSOLE(L"IndexCash already created");
		return;
	}

	//создаем кэш индексов
	m_pIndexCash=new CIndexStatisticCash(m_pContainersFactory, m_uiCashMaxSize, m_uiCashIndexMinFreq);
}

CIndexStatisticCash*  CIndexStatisticStorage::GetIndexCash(void)
{
	return static_cast<CIndexStatisticCash*>(CIndexStorage::GetIndexCash());
}

void CIndexStatisticStorage::AddIndexes(SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut)
{
	if(ISNULL(pTextBlockCut)) return;

	if(pTextBlockCut->m_IndexesType==SS::Core::Types::IndexationSearch::itAdjustment){
		//если тип индексов блокката "корректирующий" кладем индексы в статистический кэш
		IIndexAndPosition* pIndexAndPosition=pTextBlockCut->GetFirstIndexAndPosition();
		while(pIndexAndPosition){
			//добавляем индексную часть
			IIndexationIndex* pIndexationIndex=pIndexAndPosition->GetIndexationIndex();
			if(pIndexationIndex!=NULL){
				unsigned int uiIndex=pIndexationIndex->GetFirstIndex();
				if(ISNULL(uiIndex)) return;
				while(uiIndex){
					//ToConsole(L"uiIndex", uiIndex);
					//если индекс есть добавляем его в кэш
					GetIndexCash()->AddFrequentIndex(uiIndex, pTextBlockCut->m_IndexesPercentage);
					//получаем следующий индекс на данной позиции
					uiIndex=pIndexationIndex->GetNextIndex();
				}
			}else{
				ISNULL(pIndexationIndex);
			}

			pIndexAndPosition=pTextBlockCut->GetNextIndexAndPosition();
		}

	}else{
		//для обычных индексов выполняем обычные действия		
		CIndexStorage::AddIndexes(pTextBlockCut);
	}
}

void CIndexStatisticStorage::CompleteDocumentIndexing(void)
{
	if(m_pIndexCash){
		//сбрасываем счетчик индексов, чтоб для следующего документа считать заново
		GetIndexCash()->CompleteDocumentIndexing(m_CoordinateCounter.GetCoordinate(SNdxLevelInfo::ictTextAbsNumber));
	}

	if(ISNULL(m_pDocNormStorage)) return;
	//добавляем в хранилище норм, количество слов в документе оно будет являться нормой документа
	m_pDocNormStorage->AddDocNorm(m_CoordinateCounter.GetCoordinate(SNdxLevelInfo::ictWordInTextNumber)+1);

	CIndexStorage::CompleteDocumentIndexing();
}

bool CIndexStatisticStorage::PreComputeDocumentNorm(unsigned int uiDocumentsNumber, TFrequencyCollection& IndexesDocumentFreqCollection)
{
	if (!uiDocumentsNumber) return false;
	
	m_IndexInverseDocFreqCollection.clear();

	float fTemp;
	float fLogDocumentNumber=log((float)uiDocumentsNumber+1);
	TFrequencyCollection::iterator it=IndexesDocumentFreqCollection.begin();
	while(it!=IndexesDocumentFreqCollection.end()){
		fTemp=log((float)it->second);
		fTemp=fLogDocumentNumber-fTemp;

		if(fTemp<0.0001 || fTemp>1e15){
			ToConsole(L"pre compute", fTemp);
			ToConsole(L"df", (it->second));
		}

		m_IndexInverseDocFreqCollection.insert(TIndexFreqCollection::value_type(it->first, fTemp));
		it++;
	}

	return true;
}

unsigned int CIndexStatisticStorage::ComputeDocumentNorm(TFrequencyCollection& IndexesFreqInTextCollection)
{
	if(IndexesFreqInTextCollection.empty()) return 0;

	float fNormKoef=0.0;
	float fIndexInverseDocFreq=0.0;
	for(TFrequencyCollection::iterator itIndexFreqInText=IndexesFreqInTextCollection.begin(); itIndexFreqInText!=IndexesFreqInTextCollection.end(); itIndexFreqInText++) 
	{
		fIndexInverseDocFreq=m_IndexInverseDocFreqCollection[itIndexFreqInText->first];
		fNormKoef+=(itIndexFreqInText->second)*fIndexInverseDocFreq;

		//ToConsole(L"Index", (itIndexFreqInText->first));
		//ToConsole(L"TF", (unsigned int)fIndexInverseDocFreq);
		//ToConsole(L"IDF", (unsigned int)itIndexFreqInText->second);

		if(fNormKoef<0.0001 || fNormKoef>1e15){
			ToConsole(L"Index", (itIndexFreqInText->first));
			ToConsole(L"IndexInverseDocFreq", fIndexInverseDocFreq);
			ToConsole(L"Norm", fNormKoef);

			ToConsole(L"TF", (unsigned int)fIndexInverseDocFreq);
			ToConsole(L"IDF", (unsigned int)itIndexFreqInText->second);
		}
	}

	fNormKoef=sqrt(fNormKoef);
	//ToConsole(L"Norm", (unsigned int)fNormKoef);

	return (unsigned int)fNormKoef;
}

void CIndexStatisticStorage::ComputeDocumentsNorm(void)
{
	if(ISNULL(m_pIndexEntryPointTable)) return;
	if(ISNULL(m_pIndexFrequencyStorage)) return;
	if(ISNULL(m_pDocNormStorage)) return;
	if(ISNULL(m_pTextsBinaryStorage)) return;
	
	unsigned int uiDocumentsNumber=m_pTextsBinaryStorage->GetTextsNumber();

	if(!uiDocumentsNumber) return;
	//if(ISNULL(uiDocumentsNumber)) return;

	//ToConsole(L"//-------ComputeDocumentsNorm");

	//коллекция для хранения индексов и их частот в проиндексированной коллекции
	TFrequencyCollection IndexesDocumentFreqCollection;
	unsigned int uiIndex=0, uiDocumentFrequncy=0;

	//просмотрим всю таблицу что бы получить все идексы и их частоты в проиндексированной коллекции
	if(!m_pIndexEntryPointTable->InitScanOffsetDF()) return; 
	while(m_pIndexEntryPointTable->MoveNext(&uiIndex, &uiDocumentFrequncy)){
		//ToConsole(L"uiIndex", uiIndex);
		//ToConsole(L"DocumentFrequncy", uiDocumentFrequncy);
		IndexesDocumentFreqCollection.insert(TFrequencyCollection::value_type(uiIndex, uiDocumentFrequncy));
	}

	//выполняем необходимые предвычисления
	if (!PreComputeDocumentNorm(uiDocumentsNumber, IndexesDocumentFreqCollection)) return;

	//коллекция хранит индексы и их частоты в данном тексте
	TFrequencyCollection IndexesFreqInTextCollection;

	//просмотрим по порядку все тексты
	unsigned int uiDocNormKoef=0;
	unsigned int uiCurrTextNumber=0;
	while(uiCurrTextNumber<uiDocumentsNumber){
		//получаем индексы с их частотами в данном документе
		m_pIndexFrequencyStorage->ReadIndexFreqsForText(uiCurrTextNumber, IndexesFreqInTextCollection);
				
		//вычисляем нормирующий коэффициент, для данного текста
		uiDocNormKoef=ComputeDocumentNorm(IndexesFreqInTextCollection);
		//ToConsole(L"DocumentNorm", uiDocNormKoef);
		
		//добавляем его в хранилище норм
		m_pDocNormStorage->AddDocNorm(uiDocNormKoef);
		uiCurrTextNumber++;
	}

	//ToConsole(L"//---Eof-ComputeDocumentsNorm");
}

//--------------------------------------------------------------------//

}
}
}
}
}