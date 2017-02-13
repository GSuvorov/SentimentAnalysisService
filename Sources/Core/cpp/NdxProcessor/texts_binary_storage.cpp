#include "StdAfx.h"
#include ".\texts_binary_storage.h"
#include ".\const.h"
#include ".\console.h"
#include <windows.h>
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

using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Interface::Core::NdxSE::NdxDataStorage;
using namespace SS::Interface::Core::CommonContainers;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;

//--------------------------------------------------------------------//

CTextsBinaryStorageSimple::CTextsBinaryStorageSimple(INdxVectorStorage* pNdxVectorStorage) :
	m_pNdxVectorStorage(pNdxVectorStorage), m_ServiceInfoBuffer(8192),
	m_uiServiceIndexBufferSize(0), uiCurrentTextBlockNumber(0)

{
}

CTextsBinaryStorageSimple::~CTextsBinaryStorageSimple(void)
{
}

void CTextsBinaryStorageSimple::AddIndex(IServiceIndex* pServiceIndex, unsigned int uiCharPosition)
{
	if(ISNULL(m_pNdxVectorStorage)) return;

	if(!m_pNdxVectorStorage->IsOpen()){
		TO_CONSOLE(L"TextsInfo not opened!!!");
		return;
	}

	unsigned char* pServiceIndexBuffer=NULL;
	unsigned int uiServiceIndexBufferSize=0;
	unsigned char* arEmptyServiceIndex[5];

	if(pServiceIndex){
		//получаем буфер инфы по тексту
		//ToConsole(L"ServiceIndexSource", (unsigned int)pServiceIndex->GetViewIndex()->m_DictionaryIndex);
		//ToConsole(L"ServiceIndexForm", (unsigned int)pServiceIndex->GetViewIndex()->m_IDForm);
		pServiceIndex->GetInfoBuff((char**)&pServiceIndexBuffer, &uiServiceIndexBufferSize);
	}else{
		//если сервисный индекс пустой, кладем заплатку вместо него
		//ToConsole(L"ServiceIndexSource: 0xffffffff");
		uiServiceIndexBufferSize=5;
		memset(arEmptyServiceIndex, 0xffffffff, sizeof(char*) * uiServiceIndexBufferSize);
		pServiceIndexBuffer=(unsigned char*)arEmptyServiceIndex;
	}

	if(ISNULL(pServiceIndexBuffer)) return;
	if(ISNULL(uiServiceIndexBufferSize)) return;
	
	//уславливаемся что размер буфера сервисного 
	//индекса постоянный, хотя бы для одного текста
	//сохраняем этот размер для текста
	if(m_uiServiceIndexBufferSize==0){
		//создаем текущий буфер
		m_uiServiceIndexBufferSize=uiServiceIndexBufferSize;
		
		//добавляем сервисную информацию
		AddServiceInfo((unsigned char*)&uiServiceIndexBufferSize, sizeof(unsigned int));
	}else{
		//проверяем не изменился ли размер
		if(m_uiServiceIndexBufferSize!=uiServiceIndexBufferSize){
			TO_CONSOLE(L"ServiceIndexBufferSize has changed!!!")
		}
	}

	//добавляем буфер индекса в буфер текста
	unsigned int uiOldSize=(unsigned int)m_TextBuffer.size();
	
	//добавляем буфер сервисного индекса
	AddServiceInfo(pServiceIndexBuffer, m_uiServiceIndexBufferSize);
	//добавляем буфер для хранения позиции первого символа
	AddServiceInfo((unsigned char*)&uiCharPosition, sizeof(unsigned int));
}

void CTextsBinaryStorageSimple::AddServiceInfo(unsigned char* pServiceInfo, unsigned int uiServiceInfoSize)
{
	if(ISNULL(pServiceInfo)) return;
	if(ISNULL(uiServiceInfoSize)) return;
	
	if(uiServiceInfoSize>m_ServiceInfoBuffer.GetBufferSize()-m_ServiceInfoBuffer.GetFilledSize()){
		//добавляем накопленый буфер в файл
		m_pNdxVectorStorage->AppendDataChunk(m_ServiceInfoBuffer.GetHeadPtr(), m_ServiceInfoBuffer.GetFilledSize());
		//сбрасываем для добавляения новых данных
		m_ServiceInfoBuffer.Reset();
	}
	
	//кладем данные в буфер
	m_ServiceInfoBuffer.PutData(pServiceInfo, uiServiceInfoSize);
}

unsigned int CTextsBinaryStorageSimple::SetTextBlockComplete(void)
{
	//if(!m_TextBuffer.empty()){
	//	//добавляем накопленый буфер в файл
	//	m_pNdxVectorStorage->AppendDataChunk(&m_TextBuffer.front(), (unsigned int)m_TextBuffer.size());
	//	//очищаем буфер текста для следующего раза
	//	m_TextBuffer.clear();
	//}

	return ++uiCurrentTextBlockNumber;
}

unsigned int CTextsBinaryStorageSimple::SetTextComplete(void)
{
	unsigned int uiTextIndex=EMPTY_VALUE;
	
	if(!uiCurrentTextBlockNumber){
		//если блоков текста не было добавляем пустую сервисную информацию
		unsigned int uiEmptyServiceInfo=0;
		AddServiceInfo((unsigned char*)&uiEmptyServiceInfo, sizeof(unsigned int));
	}
	
	//добавляем накопленый буфер в файл
	if(m_ServiceInfoBuffer.GetFilledSize())
		m_pNdxVectorStorage->AppendDataChunk(m_ServiceInfoBuffer.GetHeadPtr(), m_ServiceInfoBuffer.GetFilledSize());
	//сигнализируем об окончании сессии добавления для текста
	m_pNdxVectorStorage->AppendDataChunkComplete(&uiTextIndex);

	//сбрасываем для добавляения новых данных
	m_ServiceInfoBuffer.Reset();
	//обнуляем размер буфера сервисного индекса
	m_uiServiceIndexBufferSize=0;
	uiCurrentTextBlockNumber=0;

	return uiTextIndex;
}

ITextBlockCut* CTextsBinaryStorageSimple::FillTextBlock(ITextBlock* pTextBlock, CInterval* pInterval, wstring* pwsIndexStorageName)
{
	if(ISNULL(pTextBlock)) return NULL;
	if(ISNULL(pInterval)) return NULL;
	if(ISNULL(pwsIndexStorageName)) return NULL;
	
	unsigned int uiTextBufferSize=0;
	
	if(pInterval->m_uiHeadPos>pInterval->m_uiTailPos){
		TO_CONSOLE(L"Interval is corrupted, head after tail!!!");
		return NULL;
	}

	//ToConsole(L"TextNumber", pTextBlock->GetTextFeature()->GetTextNumber());
	//pInterval->View();

	//узнаем требуемый размер буфера
	m_pNdxVectorStorage->MoveToAndGetDataSize(pTextBlock->GetTextFeature()->GetTextNumber(), &uiTextBufferSize);

	if(uiTextBufferSize){
		//если размер валидный читаем данные
		
		//определяем размер буфера сервисного индекса
		m_uiServiceIndexBufferSize=0;
		m_pNdxVectorStorage->ReadDataChunk((unsigned char*)&m_uiServiceIndexBufferSize, sizeof(unsigned int));	
		
		//Внимание!!!!
		//!!!Везде к буферу добавляет размер sizeof(unsigned int) так как помимо буфера сервисного
		//индекса хранится позиция первого символа слова!!!! 
		//Размер буфера сервисного буфера не равен сервисного индекса, ниже определяется настоящий размер
		unsigned int uiFullServiceBufferSize=m_uiServiceIndexBufferSize + sizeof(unsigned int);

		//получаем количество сервисных индексов
		unsigned int uiServiceIndexesNumber=
			(uiTextBufferSize-sizeof(unsigned int))/(uiFullServiceBufferSize);
		//ToConsole(L"ServiceIndexesNumber", uiServiceIndexesNumber);

		if(pInterval->m_uiHeadPos>=uiServiceIndexesNumber){
			TO_CONSOLE(L"Interval not correct, or t_img storage is corrupted!!! Couldn't extract TextBlockCut");
			ToConsole(L"TextNumber", pTextBlock->GetTextFeature()->GetTextNumber());
			pInterval->View();
			return  NULL;
		}

		if(pInterval->m_uiTailPos>=uiServiceIndexesNumber){
			//если интервал выходит за границы текста сдвигаем его
			pInterval->m_uiTailPos=uiServiceIndexesNumber-1;
		}

		//резервируем буфер под интервал
		m_TextBuffer.assign(uiFullServiceBufferSize*pInterval->GetLength(), 0);
		//смещаемся к интервалу
		m_pNdxVectorStorage->SeekInData(pInterval->m_uiHeadPos*uiFullServiceBufferSize);
		//читаем интервал в буфер
		m_pNdxVectorStorage->ReadDataChunk((unsigned char*)&m_TextBuffer.front(), (unsigned int)m_TextBuffer.size());	
		
		//теперь в цикле заполняем интервалом TextBlock
		ITextBlockCut* pTextBlockCut=pTextBlock->AddBlockCut();

		if(ISNULL(pTextBlockCut)) return NULL;
		IIndexAndPosition* pIndexAndPosition=NULL;
		
		//устанавливаем вес блока
		pTextBlockCut->SetRelevance(pInterval->m_uiWeight);
		//устанавливаем имя индексного хранилища в котором найден блок
		pTextBlockCut->SetIndexTypeName(pwsIndexStorageName->c_str());

		unsigned int uiCurrIndex=0;
		unsigned char* pPosInTextBuffer=(unsigned char*)&m_TextBuffer.front();
		unsigned int uiCurrCharPosition=EMPTY_VALUE;
		while(pPosInTextBuffer<=(unsigned char*)&m_TextBuffer.back()){
			//добавляем индекс 
			pIndexAndPosition=pTextBlockCut->AddIndexAndPosition();
			if(ISNULL(pIndexAndPosition)){
				return pTextBlockCut;
			}
			//даем ему буфер с данными
			pIndexAndPosition->GetServiceIndex()->SetInfoBuff((char*)pPosInTextBuffer, m_uiServiceIndexBufferSize);

			//считываем позицию первого символа слова
			memcpy((unsigned char*)&uiCurrCharPosition, 
				(pPosInTextBuffer+m_uiServiceIndexBufferSize), sizeof(unsigned int));
			pIndexAndPosition->SetFirstCharPosition(uiCurrCharPosition);

			//смещаемся дальше
			pPosInTextBuffer+=uiFullServiceBufferSize;
		}
		return pTextBlockCut;
	}else{
		TO_CONSOLE(L"TextBufferSize is empty!!!");
		return NULL;
	}
	

}

//--------------------------------------------------------------------//

}
}
}
}
}