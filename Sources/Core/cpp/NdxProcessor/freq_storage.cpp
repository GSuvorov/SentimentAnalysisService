#include "StdAfx.h"
#include ".\freq_storage.h"
#include ".\const.h"
#include ".\console.h"

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

//--------------------------------------------------------------------//

CIndexFrequencyStorage::CIndexFrequencyStorage(INdxVectorStorage* pNdxVectorStorage)
:m_pNdxVectorStorage(pNdxVectorStorage)
{
}

CIndexFrequencyStorage::~CIndexFrequencyStorage()
{
}

void CIndexFrequencyStorage::WriteIndexFreqs(vector<SIndexTF>& v_tf)
{
	if(!m_pNdxVectorStorage->IsOpen()){ 
		TO_CONSOLE(L"Storage not opened!!!");
		return;
	}


	unsigned int uiIndexesNumber=(unsigned int)v_tf.size();
	
	//добавляем размер буфера 
	m_pNdxVectorStorage->AppendDataChunk((unsigned char*)&uiIndexesNumber, sizeof(unsigned int));

	if(uiIndexesNumber==0) return;

	//добавляем накопленый буфер
	m_pNdxVectorStorage->AppendDataChunk((unsigned char*)&v_tf[0], uiIndexesNumber*sizeof(SIndexTF));
}

void CIndexFrequencyStorage::SetTextComplete(void)
{
	if(!m_pNdxVectorStorage->IsOpen()){ 
		TO_CONSOLE(L"Storage not opened!!!");
		return;
	}

	unsigned int uiTextIndex=EMPTY_VALUE;

	//сигнализируем об окончании сессии добавления для текста
	m_pNdxVectorStorage->AppendDataChunkComplete(&uiTextIndex);
}

unsigned int CIndexFrequencyStorage::ReadIndexFreqsBuffer(vector<SIndexTF>& IndexFreqsBuffer)
{
	if(ISNULL(m_pNdxVectorStorage)) return 0;

	if(!m_pNdxVectorStorage->IsOpen()){ 
		TO_CONSOLE(L"Storage not opened!!!");
		return 0;
	}

	unsigned int uiReadedDataSize=0;
	unsigned int uiIndexesNumber=0;
	// считаем количество индексов в документе 
	m_pNdxVectorStorage->ReadDataChunk((unsigned char*)&uiIndexesNumber, sizeof(unsigned int));
	uiReadedDataSize+=sizeof(unsigned int);

	//изменим размер коллекции
	IndexFreqsBuffer.resize(uiIndexesNumber);

	if(uiIndexesNumber==0) return uiReadedDataSize;

	//считываем коллекцию
	m_pNdxVectorStorage->ReadDataChunk((unsigned char*)&IndexFreqsBuffer[0], uiIndexesNumber*sizeof(SIndexTF));

	uiReadedDataSize+=uiIndexesNumber*sizeof(SIndexTF);
	
	return uiReadedDataSize;
}

void CIndexFrequencyStorage::ReadIndexFreqsForText(unsigned int uiTextNumber, map<unsigned int,unsigned int>& IndexFreqsCollection)
{
	if(uiTextNumber==EMPTY_VALUE){
		TO_CONSOLE(L"uiTextNumber is empty!!!");
		return;
	}
	
	//предварительно чистим коллекцию
	IndexFreqsCollection.clear();

	unsigned int uiTextBufferSize=0;
	//узнаем требуемый размер буфера
	m_pNdxVectorStorage->MoveToAndGetDataSize(uiTextNumber, &uiTextBufferSize);

	if(uiTextBufferSize){
		vector<SIndexTF> IndexFreqsBuffer;
		map<unsigned int,unsigned int>::iterator itIndexFreq;
		unsigned int uiReadedDataSize=0;
		while(uiReadedDataSize<uiTextBufferSize){
			IndexFreqsBuffer.clear();
			uiReadedDataSize+=ReadIndexFreqsBuffer(IndexFreqsBuffer);
			
			//обновляем индексы в коллекции
			for(vector<SIndexTF>::iterator itIndexTF=IndexFreqsBuffer.begin(); itIndexTF!=IndexFreqsBuffer.end(); itIndexTF++){
				itIndexFreq=IndexFreqsCollection.find(itIndexTF->m_uiIndex);
				if(itIndexFreq==IndexFreqsCollection.end()){
					IndexFreqsCollection.insert(
						map<unsigned int,unsigned int>::value_type(itIndexTF->m_uiIndex, itIndexTF->m_IndexFrequencyInText));
				}else{
					//если инлекс есть обновляем его частоту
					itIndexFreq->second+=itIndexTF->m_IndexFrequencyInText;
				}
			}
		}

	}else{
		TO_CONSOLE(L"TextBufferSize is empty!!!");
		return;
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}