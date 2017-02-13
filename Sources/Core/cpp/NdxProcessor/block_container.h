#pragma once

#include ".\list_containers.h"
#include ".\basis_containers.h"
#include ".\pool_client.h"

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

///блок данных с возможностью последовательного добавления и выброса данных
///размер блока не больше 0xff 
#pragma pack(push,1)  // Fix packing size 

template <unsigned char DataSize>
class CBinaryBlock : public CUnidirListItem< CBinaryBlock<DataSize> >
	, public SS::Core::NdxSE::NdxProcessor::MemoryManagement::CPoolClient
{
public:
	CBinaryBlock(void):m_ucCurrentPos(0){};
		
	///добавляет данные в конец блока, возвращает размер добавленных данных
	///блока может и не хватить на все сразу
	unsigned int PushBack(unsigned char* pucData, unsigned int uiDataSize)
	{
		if(ISNULL(pucData)) return 0;
		if(ISNULL(uiDataSize)) return 0;

		unsigned int uiAvailableSize=DataSize-m_ucCurrentPos;
		if(uiDataSize<uiAvailableSize)	uiAvailableSize=uiDataSize;
		
		memcpy(m_arrData+m_ucCurrentPos, pucData, uiAvailableSize);
		m_ucCurrentPos+=uiAvailableSize;

		return uiAvailableSize;
	};

	///выкидывает данные с конца блока, возвращает размер выкинутых данных
	///в блоке может не содержаться всех данных
	unsigned int PopBack(unsigned char* pucData, unsigned int uiDataSize)
	{
		if(ISNULL(pucData)) return 0;
		if(ISNULL(uiDataSize)) return 0;

		unsigned int uiAvailableSize=m_ucCurrentPos;
		if(uiDataSize<uiAvailableSize)	uiAvailableSize=uiDataSize;
		
		memcpy(pucData, m_arrData+(m_ucCurrentPos-uiAvailableSize), uiAvailableSize);
		m_ucCurrentPos-=uiAvailableSize;

		return uiAvailableSize;
	}

	///проверяет на пустоту
	inline bool IsEmpty(void){ return (m_ucCurrentPos != 0); };
	///проверяет на заполненность
	inline bool IsFull(void){ return (m_ucCurrentPos>=DataSize); };
	///возвращает свободный размер
	unsigned int GetFreeSize(void){return (DataSize - m_ucCurrentPos);};
	///возвращает заполненый размер
	unsigned char GetFilledSize(void){return m_ucCurrentPos;};
	///устанавливает заполненый размер
	void SetFilledSize(unsigned char ucValue){m_ucCurrentPos=ucValue;};
	///возвращает указатель на данные
	unsigned char* GetDataPtr(void){return m_arrData;};
	
protected:
	///буфер с данными
	unsigned char m_arrData[DataSize];	
	///текущая позиция в буфере
	unsigned char m_ucCurrentPos;
};
#pragma pack(pop)

//--------------------------------------------------------------------//

///блочный буфер хранит содержимое списком из блоков, заданного размра
template <unsigned char BlockSize>
class CBlockBuffer : 
	public CBinaryContainer
	//,	public SS::Core::NdxSE::NdxProcessor::MemoryManagement::CPoolClient
{
public:
	CBlockBuffer(void):m_BinaryBlocksNumber(0), m_pTailBinaryBlock(NULL){};
	~CBlockBuffer(void){Reset();};

	///возвращает размер бинарного буфера
	unsigned int GetBinaryBufferSize(void)
	{
		if(!m_pTailBinaryBlock) return 0;
		return (m_BinaryBlocksNumber-1)*BlockSize+m_pTailBinaryBlock->GetFilledSize();
	};

	///добавляет данные в конец
	void PushBack(unsigned int uiValue)
	{
		unsigned int uiTemp=uiValue;
		PushBack((unsigned char*)&uiTemp, sizeof(unsigned int));
	};
	

	///добавляет данные в конец
	virtual void PushBack(unsigned char* pucData, unsigned int uiDataSize)
	{
		if(ISNULL(pucData)) return;
		if(ISNULL(uiDataSize)) return;
		
		if(!m_pTailBinaryBlock) AddBinaryBlock();

		//в цикле добавляем данные, 
		//если блоков не хватает добавляем блок и продолжаем с оставшимися данными 
		unsigned int uiTempSize=0;
		while(true){
			uiTempSize+=m_pTailBinaryBlock->PushBack(pucData+uiTempSize, uiDataSize-uiTempSize);

			if(uiTempSize<uiDataSize)
				AddBinaryBlock();
			else
				break;
		}
	};

	///выкидывает данные с конца
	virtual void PopBack(unsigned char* pucData, unsigned int uiDataSize)
	{
		if(ISNULL(pucData)) return;
		if(ISNULL(uiDataSize)) return;
		if(!m_pTailBinaryBlock) return;

		if(uiDataSize>(m_BinaryBlocksNumber-1)*BlockSize+m_pTailBinaryBlock->GetFilledSize()) return;
		
		//в цикле выкидываем данные, если крайний блок пустее убираем и его 
		unsigned int uiTempSize=0;
		unsigned int uiPopedSize=0;
		unsigned char* pucCurr=pucData+uiDataSize;
		while(true){
			uiTempSize=m_pTailBinaryBlock->GetFilledSize();
			if(uiTempSize>uiDataSize-uiPopedSize) uiTempSize=uiDataSize-uiPopedSize;

			pucCurr-=uiTempSize;
			uiPopedSize+=m_pTailBinaryBlock->PopBack(pucCurr, uiTempSize);

			if(uiPopedSize>=uiDataSize){
				break;
			}else{
				PopBinaryBlock();
			}
		}

		if(m_pTailBinaryBlock->GetFilledSize()==0) PopBinaryBlock();
	};

	///выкидывает байт с конца
	virtual unsigned char PopByte(void)
	{
		while(m_pTailBinaryBlock && m_pTailBinaryBlock->IsEmpty())	
				PopBinaryBlock();
		
		if(!m_pTailBinaryBlock) return UC_EMPTY;

		unsigned char ucTemp=0;
		m_pTailBinaryBlock->PopBack(&ucTemp, sizeof(unsigned char));

		return ucTemp;
	};

	///сжимает 4 байта и кладет в коллекцию блоков контайнера
	virtual void PushBackEncoded(unsigned int uiValue)
	{
		//кладем 7-ми битки в вектор, выставля всем кроме последней значащий бит
		unsigned char ucBytesNumber=m_pByteAlignCompress->GetNeededBytesNumber(uiValue);
		unsigned char i=0;
		unsigned char ucTemp;
		while(i<ucBytesNumber){
			ucTemp=(uiValue&arrMasks[i])>>arrOffsets[i];
			if(!(i==ucBytesNumber-1)){
				//очередная
				ucTemp=m_pByteAlignCompress->m_cucSignBit|ucTemp;
			}
			PushBack(&ucTemp, sizeof(unsigned char));
			i++;
		}
	};

	///сбрасывает содержимое
	virtual void Reset(void)
	{
		CBinaryBlock<BlockSize>* pTempBinaryBlock=NULL;
		while(m_pTailBinaryBlock){
			pTempBinaryBlock=m_pTailBinaryBlock->GetNextItem();
			delete m_pTailBinaryBlock;
			m_pTailBinaryBlock=pTempBinaryBlock;
		}
		m_pTailBinaryBlock=NULL;
		m_BinaryBlocksNumber=0;
	};

	///пишет содержимое контайнера в файл
	unsigned int Write(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage)
	{
		if(ISNULL(pNdxStorage)) return 0;
		
		unsigned int uiWritedBytes=0;
		//пишем количество блоков
		uiWritedBytes+=WriteCompressed(pNdxStorage, m_BinaryBlocksNumber);

		//пишем размер крайнего блока
		CBinaryBlock<BlockSize>* pBinaryBlock=GetTailBinaryBlock();
		unsigned char ucFilledSize=pBinaryBlock->GetFilledSize();
		pNdxStorage->Append((unsigned char*)&ucFilledSize, sizeof(unsigned char));
		uiWritedBytes+=sizeof(unsigned char);
		while(pBinaryBlock){
			//пишем очередной блок с боксами
			unsigned char* pBuffer=pBinaryBlock->GetDataPtr();
			pNdxStorage->Append(pBuffer, ucFilledSize);
			uiWritedBytes+=ucFilledSize;
			ucFilledSize=BlockSize;
			pBinaryBlock=pBinaryBlock->GetNextItem();
		}

		return uiWritedBytes;
	};

	///читает содержимое контайнера из файла
	void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage)
	{
		if(ISNULL(pNdxStorage)) return;

		//читаем количество блоков
		m_BinaryBlocksNumber=ReadCompressed(pNdxStorage);
		
		//читаем размер крайнего блока
		unsigned char ucFilledSize; 
		pNdxStorage->Read((unsigned char*)&ucFilledSize, sizeof(unsigned char));
		
		//читаем блоки с боксами, формируем список блоков
		CBinaryBlock<BlockSize>* pBinaryBlock=NULL;
		unsigned int i=m_BinaryBlocksNumber;
		while(i){
			if(pBinaryBlock){ 
				//если блок не первый добавляем к крайнему в списке
				pBinaryBlock->SetNextItem(new CBinaryBlock<BlockSize>());
				pBinaryBlock=pBinaryBlock->GetNextItem();
			}else{
				m_pTailBinaryBlock=new CBinaryBlock<BlockSize>();
				pBinaryBlock=GetTailBinaryBlock();
			}
			
			//читаем содержимое блока
			pNdxStorage->Read(pBinaryBlock->GetDataPtr(), ucFilledSize);
			pBinaryBlock->SetFilledSize(ucFilledSize);

			ucFilledSize=BlockSize;
			--i;
		}
	};

	///заполняется по буферу
	unsigned int FromBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize)
	{
		if(ISNULL(pBuffer)) return 0;
		if(ISNULL(uiBufferSize)) return 0;
		
		//читаем количество блоков
		unsigned int uiReadedBytesNumber=0;
		unsigned int uiWorkedBytesNumber;
		m_BinaryBlocksNumber=m_pByteAlignCompress->Decode(pBuffer, uiBufferSize, 0, &uiWorkedBytesNumber);
		pBuffer+=uiWorkedBytesNumber;	
		uiReadedBytesNumber+=uiWorkedBytesNumber;

		//читаем размер крайнего блока
		unsigned char ucFilledSize; 
		memcpy((unsigned char*)&ucFilledSize, pBuffer, sizeof(unsigned char));
		pBuffer+=sizeof(unsigned char);
		uiReadedBytesNumber+=sizeof(unsigned char);

		//читаем блоки, формируем список блоков
		CBinaryBlock<BlockSize>* pBinaryBlock=NULL;
		unsigned int i=m_BinaryBlocksNumber;
		while(i){

			if(pBinaryBlock){ 
				//если блок не первый добавляем к крайнему в списке
				pBinaryBlock->SetNextItem(new CBinaryBlock<BlockSize>());
				pBinaryBlock=pBinaryBlock->GetNextItem();
			}else{
				m_pTailBinaryBlock=new CBinaryBlock<BlockSize>();
				pBinaryBlock=GetTailBinaryBlock();
			}
			
			//читаем содержимое блока
			memcpy(pBinaryBlock->GetDataPtr(), pBuffer, ucFilledSize);
			pBuffer+=ucFilledSize;
			pBinaryBlock->SetFilledSize(ucFilledSize);
			uiReadedBytesNumber+=ucFilledSize;

			ucFilledSize=BlockSize;
			--i;
		}
		return 	uiReadedBytesNumber;
	}

	///пишется в буфер
	void ToBinaryBuffer(CByteAlignCompress::TBytesBuffer* pBytesBuffer)
	{
		if(ISNULL(pBytesBuffer)) return;
		
		//пишем количество блоков
		m_pByteAlignCompress->EncodeToBuffer(m_BinaryBlocksNumber, pBytesBuffer);

		//пишем размер крайнего блока
		CBinaryBlock<BlockSize>* pBinaryBlock=GetTailBinaryBlock();
		unsigned char ucFilledSize=pBinaryBlock->GetFilledSize();
		pBytesBuffer->push_back(ucFilledSize);
		unsigned int uiTemp=0;
		while(pBinaryBlock){
			//пишем очередной блок с боксами
			unsigned char* pBuffer=pBinaryBlock->GetDataPtr();

			uiTemp=(unsigned int)pBytesBuffer->size();
			pBytesBuffer->resize(uiTemp+ucFilledSize);
			memcpy(&pBytesBuffer->front()+uiTemp, pBuffer, ucFilledSize);

			ucFilledSize=BlockSize;
			pBinaryBlock=pBinaryBlock->GetNextItem();
		}
	}

protected:
	///возвращает хвостовой блок
	CBinaryBlock<BlockSize>* GetTailBinaryBlock(void){return m_pTailBinaryBlock;};
	///добавляет CPosBinaryBlock в конец
	virtual void AddBinaryBlock(void)
	{
		CBinaryBlock<BlockSize>* pTempBinaryBlock=new CBinaryBlock<BlockSize>();
		pTempBinaryBlock->SetNextItem(m_pTailBinaryBlock);
		m_pTailBinaryBlock=pTempBinaryBlock;
		m_BinaryBlocksNumber++;
	};

	///выкидывает CPosBinaryBlock с конца
	virtual void PopBinaryBlock(void)
	{
		CBinaryBlock<BlockSize>* pTempBinaryBlock=m_pTailBinaryBlock;
		m_pTailBinaryBlock=m_pTailBinaryBlock->GetNextItem();
		delete pTempBinaryBlock;
		--m_BinaryBlocksNumber;
	};

	///хвостовой блок
	CBinaryBlock<BlockSize>* m_pTailBinaryBlock;
	///колчество блоков которые содержит хранилище позиций 
	unsigned int m_BinaryBlocksNumber;
};

//--------------------------------------------------------------------//

///блочный буфер хранит содержимое списком из блоков, заданного размера, 
///имеет возможность преобразовываться в прямой буфер за счет указателя на головной блок
///отсутствует возможность PopBack, можно только добавлять
///отсутствует возможность чтения самого себя может только писать себя переводясь прямой буфер
template <unsigned char BlockSize>
class CBlockBufferPusher : 
	public CBlockBuffer<BlockSize>
{
public:
	CBlockBufferPusher(void):m_pHeadBinaryBlock(NULL){};
	
	//-----------------------------
	//эти функции не поддерживаются
	///выкидывает данные с конца
	void PopBack(unsigned char* pucData, unsigned int uiDataSize){TO_CONSOLE(L"Proxed");};
	///выкидывает байт с конца
	unsigned char PopByte(void){TO_CONSOLE(L"Proxed"); return 0;};
	///читает содержимое контайнера из файла
	void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage){TO_CONSOLE(L"Proxed");};
	///заполняется по буферу
	unsigned int FromBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize){TO_CONSOLE(L"Proxed"); return 0;};
	//-----------------------------

	///сбрасывает содержимое
	void Reset(void)
	{
		CBinaryBlock<BlockSize>* pTempBinaryBlock=NULL;
		while(m_pHeadBinaryBlock){
			pTempBinaryBlock=m_pHeadBinaryBlock->GetNextItem();
			delete m_pHeadBinaryBlock;
			m_pHeadBinaryBlock=pTempBinaryBlock;
		}

		m_pTailBinaryBlock=NULL;
		m_pHeadBinaryBlock=NULL;
		m_BinaryBlocksNumber=0;
	};

	///пишет содержимое контайнера в файл
	unsigned int Write(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage)
	{
		if(ISNULL(pNdxStorage)) return 0;

		unsigned int uiWritedBytes=0;
		CBinaryBlock<BlockSize>* pBinaryBlock=GetHeadBinaryBlock();
		while(pBinaryBlock){
			//пишем очередной блок с боксами
			pNdxStorage->Append(pBinaryBlock->GetDataPtr(), pBinaryBlock->GetFilledSize());
			uiWritedBytes+=pBinaryBlock->GetFilledSize();
			pBinaryBlock=pBinaryBlock->GetNextItem();
		}

		return uiWritedBytes;
	};

	///пишется в буфер
	void ToBinaryBuffer(CByteAlignCompress::TBytesBuffer* pBytesBuffer)
	{
		if(ISNULL(pBytesBuffer)) return;
		
		CBinaryBlock<BlockSize>* pBinaryBlock=GetHeadBinaryBlock();
		unsigned int uiTemp=0;
		while(pBinaryBlock){
			//пишем очередной блок с боксами
			uiTemp=(unsigned int)pBytesBuffer->size();
			pBytesBuffer->resize(uiTemp+pBinaryBlock->GetFilledSize());
			memcpy(&pBytesBuffer->front()+uiTemp, pBinaryBlock->GetDataPtr(), pBinaryBlock->GetFilledSize());

			pBinaryBlock=pBinaryBlock->GetNextItem();
		}

	}

protected:
	///возвращает головной блок
	CBinaryBlock<BlockSize>* GetHeadBinaryBlock(void){return m_pHeadBinaryBlock;};

	///добавляет CPosBinaryBlock в конец
	void AddBinaryBlock(void)
	{
		CBinaryBlock<BlockSize>* pTempBinaryBlock=new CBinaryBlock<BlockSize>();
		if(!m_pHeadBinaryBlock){
			m_pHeadBinaryBlock=m_pTailBinaryBlock=pTempBinaryBlock;
		}else{
			m_pTailBinaryBlock->SetNextItem(pTempBinaryBlock);
			m_pTailBinaryBlock=pTempBinaryBlock;
		}

		m_BinaryBlocksNumber++;
	};
	///выкидывает CPosBinaryBlock с конца
	void PopBinaryBlock(void){};

	///головной блок
	CBinaryBlock<BlockSize>* m_pHeadBinaryBlock;
};

//--------------------------------------------------------------------//

}
}
}
}
}