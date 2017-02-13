#include "StdAfx.h"
#include ".\text_binary_storage_ex.h"
#include "../../../[libraries]\ZLIB114DLL\zlib.h "
#include "..\ASCInterface\defines.h"

USES_LOAD_MANAGER;

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

void CTextBlock::AddIndex(IServiceIndex* pServiceIndex, unsigned int uiCharPosition)
{
	//	кладём позицию
	m_positions.push_back(uiCharPosition);
	//	кладём смещение на данные сервисного индекса
	m_si_offset.push_back((unsigned short)m_si_data.size());
	//	кладём данные сервисного индекса в контейнеры
	if (pServiceIndex == NULL) return;

	unsigned int	si_datasize		= 0;	//	размер данных сервисного индекса
	unsigned char*	si_buffer		= NULL;	//	указатель на буфер данных сервисного индекса
	pServiceIndex->GetInfoBuff(
		(char**)&si_buffer,
		&si_datasize);
	
	unsigned int old_size = (unsigned int)m_si_data.size();
	m_si_data.resize(old_size+si_datasize);
	memcpy((LPVOID)&m_si_data[old_size], (LPVOID)si_buffer, si_datasize);
}

unsigned int CTextBlock::AddToFile(INdxVectorStorage* pFile)
{
	if(ISNULL(pFile)) return 0;

	if(!pFile->IsOpen()){
		TO_CONSOLE(L"pFile not opened!!!");
		return 0;
	}

	/*
	{
		wchar_t	buf[512];
		swprintf(buf, L"\n\nm_positions.size() = %u\nm_si_offset.size() = %u\nnm_si_data.size() = %u\n\n",
			(unsigned int)m_positions.size(),
			(unsigned int)m_si_offset.size(),
			(unsigned int)m_si_data.size());
		ToConsole(buf);
	}
	*/
	//	проверка входных данных
	if (m_si_offset.size() != m_positions.size())	SS_THROW(L"input data sizes are not equal-1");

	//	возвращаемый результат
	unsigned int	result = 0;
	//	размер всего блока
	unsigned int	block_size		= 0;	//	пока не известен
	//	количество индексов в блоке
	unsigned int	count			= (unsigned int)m_si_offset.size();
	//	размер сегмента информации о позициях (сколько байт надо чтобы уместить в битах информацию)
	//	бит=0, => позиция хранится как 1 байт, бит=1, => позиция хранится как 4 байта
	unsigned int	size_pos_inf	= ((unsigned int)m_positions.size() >> 3)+((((unsigned int)m_positions.size() & (unsigned int)(7)) == 0)?(0):(1));
	//	размер сегмента позиций
	unsigned int	size_pos		= 0;	//	пока не известен
	//	размер сегмента размеров данных
	unsigned int	size_data_inf	= (unsigned int)m_si_offset.size()*sizeof(unsigned short);
	//	размер сегмента данных
	unsigned int	size_data		= (unsigned int)m_si_data.size();

	/*
	{
		wchar_t	buf[512];
		swprintf(buf, L"\n\n count = %u\n size_pos_inf = %u\n size_data_inf = %u\n size_data = %u\n\n",
			count,
			size_pos_inf,
			size_data_inf,
			size_data
			);
		ToConsole(buf);
	}
	*/


	//	временные буфера
	//	... под данные о абсолютности позиций
	unsigned char* buff_pos_inf = new unsigned char[size_pos_inf];
	//	заполнение нулями
	memset(buff_pos_inf, 0, size_pos_inf);
	// ToConsole(L"memset, size_pos_inf = ", size_pos_inf);
	//	... под сами позиции
	unsigned char* buff_pos = new unsigned char[m_positions.size() * sizeof(unsigned int) + 4];

	//	преобразуем позиции
	switch(m_positions.size())
	{
	case 0:
	{
		size_pos = 0;
		SAVE_LOG(L"TBSE: ERROR!!! m_positions.size() == 0");
	}; break;
	default:
	{
		//ToConsole(L"m_positions.size() != 0");
		//	первую позицию кладём всегда в абсолютном виде
		unsigned char*	p_pos = buff_pos;

		//	выставляем первый бит
		*buff_pos_inf	= 0x01;

		//	кладём абсолютную позицию
		*((unsigned int*)p_pos) = m_positions[0];	//	положили 4 байта
		p_pos		+= sizeof(unsigned int);
		size_pos	= sizeof(unsigned int);

		//	[i-1] есть всегда
		for(unsigned int i = 1, i_size = (unsigned int)m_positions.size(); i < i_size; ++i)
		{
			//	определяем маску для того бита который соответствует i
			unsigned char	mask = (unsigned char)(1 << (i & (unsigned int)7));
			unsigned int	index = (i >> 3);
			unsigned int	delta = m_positions[i];
			//	если разница меньше
			if (m_positions[i] < m_positions[i-1]+256)
			{
				//ToConsole(L"относительная");
				delta = m_positions[i] - m_positions[i-1];
				//	копируем 1 байт
				memcpy(p_pos, &delta, sizeof(unsigned char));
				p_pos += sizeof(unsigned char);
				size_pos += sizeof(unsigned char);
			}
			else
			{
				//ToConsole(L"абсолютная");
				buff_pos_inf[index] = buff_pos_inf[index] | mask;
				//	копируем 4 байта
				memcpy(p_pos, &delta, sizeof(delta));
				p_pos += sizeof(unsigned int);
				size_pos += sizeof(unsigned int);
			}
		}
	}
	}

	/*	отладочный вывод битов
	for (size_t i = 0, i_size = size_pos_inf; i < i_size; ++i)
		ToConsole(L"биты=", (unsigned int)buff_pos_inf[i]);
	*/

	//	вычисление размера блока
	block_size = 
		sizeof(count)+
		sizeof(size_pos_inf)+
		sizeof(size_pos)+
		sizeof(size_data_inf)+
		sizeof(size_data)+
		size_pos_inf+
		size_pos+
		size_data_inf+
		size_data;

	if(!CTextsBinaryStorageEx::m_bCompressTextBlocks)
	{
		//если нужно кладем без сжатия
		//	кладём размеры
		pFile->AppendDataChunk((unsigned char*)&count, sizeof(count));	//	число
		pFile->AppendDataChunk((unsigned char*)&size_pos_inf, sizeof(size_pos_inf));	//	размер информации о позициях
		pFile->AppendDataChunk((unsigned char*)&size_pos, sizeof(size_pos));	//	размер позиций
		pFile->AppendDataChunk((unsigned char*)&size_data_inf, sizeof(size_data_inf));	//	размер меток данных сервисных буферов
		pFile->AppendDataChunk((unsigned char*)&size_data, sizeof(size_data));	//	размер данных сервисных буферов
		//	кладём буфера
		pFile->AppendDataChunk((unsigned char*)buff_pos_inf, size_pos_inf);	//	информация о позициях
		pFile->AppendDataChunk((unsigned char*)buff_pos, size_pos);			//	позиции
		pFile->AppendDataChunk((unsigned char*)&m_si_offset[0], size_data_inf);
		pFile->AppendDataChunk((unsigned char*)&m_si_data[0], size_data);

		result = block_size;
	}
	else
	{
		//если нужно сжимаем и кладем
		//выделяем буфер под данные	
		unsigned int	uiTotalBufferSize	= block_size;
		unsigned char*	pTotalBuffer		= new unsigned char[uiTotalBufferSize]; 
		
		//	копируем данные в TotalBuffer
		//		копируем размеры
		unsigned char* pTemp = pTotalBuffer;
		memcpy(pTemp, (unsigned char*)&count, sizeof(count));
			pTemp+=sizeof(count);
		memcpy(pTemp, (unsigned char*)&size_pos_inf, sizeof(size_pos_inf));
			pTemp+=sizeof(size_pos_inf);
		memcpy(pTemp, (unsigned char*)&size_pos, sizeof(size_pos));
			pTemp+=sizeof(size_pos);
		memcpy(pTemp, (unsigned char*)&size_data_inf, sizeof(size_data_inf));
			pTemp+=sizeof(size_data_inf);
		memcpy(pTemp, (unsigned char*)&size_data, sizeof(size_data));
			pTemp+=sizeof(size_data);
		//		копируем буфера
		memcpy(pTemp, buff_pos_inf, size_pos_inf);
			pTemp+=size_pos_inf;
		memcpy(pTemp, buff_pos, size_pos);
			pTemp+=size_pos;
		memcpy(pTemp, (unsigned char*)&m_si_offset[0], size_data_inf);
			pTemp+=size_data_inf;
		memcpy(pTemp, (unsigned char*)&m_si_data[0], size_data);
			pTemp+=size_data;

		//	выделяем буфер под сжатые данные
		unsigned long	ulCompressedBuffer	= uiTotalBufferSize+512;
		unsigned char*	pCompressedBuffer	= new unsigned char[ulCompressedBuffer]; 

		//сжимаем
		compress(pCompressedBuffer, &ulCompressedBuffer, pTotalBuffer, uiTotalBufferSize);
		
		//	записываем сначала размер несжатого буфера
		pFile->AppendDataChunk((unsigned char*)&uiTotalBufferSize, sizeof(uiTotalBufferSize));
		//	записываем сжатый буфер
		pFile->AppendDataChunk(pCompressedBuffer, (unsigned int)ulCompressedBuffer);

		//удаляем буфера
		delete[] pTotalBuffer;
		delete[] pCompressedBuffer;
		
		result=sizeof(uiTotalBufferSize)+ulCompressedBuffer;
	}
	
	if (buff_pos_inf != NULL) delete[] buff_pos_inf;
	if (buff_pos     != NULL) delete[] buff_pos;

	//завершаем сценарий пакетного добавления, возвращаем индекс, по которому будут находятся данные
	pFile->AppendDataChunkComplete(&result);

	//	очищаем буфера текстового блока
	Reset();

	return result;
}

void CTextBlock::FillFromFile(INdxVectorStorage* pFile, unsigned int uiTextBlockBinarySize)
{
	Reset();

	if(ISNULL(pFile)) return;

	if(!pFile->IsOpen()){
		TO_CONSOLE(L"pFile not opened!!!");
		return;
	}

	//	размер несжатого блока
	unsigned int	block_size		= 0;
	//	количество элементов в блоке
	unsigned int	count			= 0;
	//	размер сегмента абсолютности позиций
	unsigned int	size_pos_inf	= 0;
	//	размер сегмента позиций
	unsigned int	size_pos		= 0;
	//	размер сегмента размеров данных
	unsigned int	size_data_inf	= 0;
	//	размер сегмента данных
	unsigned int	size_data		= 0;
	//	временные буфера
	//	... под данные о абсолютности позиций
	unsigned char*	buff_pos_inf	= NULL;
	//	... под сами позиции
	unsigned char*	buff_pos		= NULL;

	if(CTextsBinaryStorageEx::m_bCompressTextBlocks)
	{
		//ToConsole(L"\n\n\n\n                                                 сжатый блок");
		//	считываем размер несжатого блока
		pFile->ReadDataChunk((unsigned char*)&block_size, sizeof(block_size));
		//ToConsole(L"считываем размер несжатого блока", block_size);
		//ToConsole(L"uiTextBlockBinarySize = ", uiTextBlockBinarySize);
		
		//выделяем буфер под сжатые данные	
		unsigned int	uiTotalBufferSize		= uiTextBlockBinarySize;
		unsigned char*	pTotalBuffer			= new unsigned char[uiTotalBufferSize]; 
		//выделяем буфер под чистые данные
		unsigned long	ulUnCompressedBufferSize	= block_size;
		unsigned char*	pUnCompressedBuffer			= new unsigned char[ulUnCompressedBufferSize]; 

		//	читаем буфер из файла
		pFile->ReadDataChunk(pTotalBuffer, uiTotalBufferSize);
		//ToConsole(L"читаем буфер из файла");

		//	разжимаем
		uncompress(pUnCompressedBuffer, &ulUnCompressedBufferSize, pTotalBuffer, uiTotalBufferSize);
		//ToConsole(L"разжимаем");

		//	копируем данные
		unsigned char*	pTemp	= pUnCompressedBuffer;
		block_size				= 0;

		//	считываем размеры
		memcpy(&count, pTemp, sizeof(count));//	ToConsole(L"count = ", count);
			pTemp += sizeof(count);
		memcpy(&size_pos_inf, pTemp, sizeof(size_pos_inf));	//ToConsole(L"size_pos_inf = ", size_pos_inf);
			pTemp += sizeof(size_pos_inf);
		memcpy(&size_pos, pTemp, sizeof(size_pos));	//ToConsole(L"size_pos = ", size_pos);
			pTemp += sizeof(size_pos);
		memcpy(&size_data_inf, pTemp, sizeof(size_data_inf));	//ToConsole(L"size_data_inf = ", size_data_inf);
			pTemp += sizeof(size_data_inf);
		memcpy(&size_data, pTemp, sizeof(size_data));	//ToConsole(L"size_data = ", size_data);
			pTemp += sizeof(size_data);

		//	проверка размеров
		if (count != (size_data_inf >> 1))
		{
			ToConsole(L"TBSE: ERROR!!! count = ", count);
			ToConsole(L"TBSE: ERROR!!! (size_data_inf >> 1) = ", (unsigned int)(size_data_inf >> 1));
			SS_THROW(L"data sizes are not equal-1");
		}

		//	считываем позиции
		buff_pos_inf	= new unsigned char[size_pos_inf];//ToConsole(L"buff_pos_inf	= new unsigned char[size_pos_inf], = ", size_pos_inf);
		//for (size_t i = 0, i_size = size_pos_inf; i < i_size; ++i) ToConsole(L"bits= ", (unsigned int)buff_pos_inf[i]);
		buff_pos		= new unsigned char[size_pos];//ToConsole(L"buff_pos	= new unsigned char[size_pos], = ", size_pos);
		memcpy(buff_pos_inf, pTemp, size_pos_inf);	//ToConsole(L"считываем buff_pos_inf, ", size_pos_inf);
			pTemp += size_pos_inf;
		memcpy(buff_pos, pTemp, size_pos);	//ToConsole(L"считываем buff_pos, ", size_pos);
			pTemp += size_pos;
		//	считываем метки данных сервисных индексов
		m_si_offset.resize(size_data_inf >> 1);	//	unsigned short, из размера переводим в количество
		//ToConsole(L"m_si_offset.resize to ", (size_data_inf >> 1));
		memcpy((unsigned char*)&m_si_offset[0], pTemp, size_data_inf);//ToConsole(L"копирование m_si_offset");
			pTemp += size_data_inf;
		//	считываем данные сервисных индексов
		m_si_data.resize(size_data);
		//ToConsole(L"m_si_data.resize to ", size_data);
		memcpy(&m_si_data[0], pTemp, size_data);
			pTemp += size_data;

		delete[]	pTotalBuffer;
		delete[]	pUnCompressedBuffer;
	}
	else
	{
		//ToConsole(L"НЕ сжатый блок");
		//	чтение несжатого блока
		//	считываем размеры
		pFile->ReadDataChunk((unsigned char*)&count, sizeof(count));	//	число
		pFile->ReadDataChunk((unsigned char*)&size_pos_inf, sizeof(size_pos_inf));	//	размер информации о позициях
		pFile->ReadDataChunk((unsigned char*)&size_pos, sizeof(size_pos));	//	размер позиций
		pFile->ReadDataChunk((unsigned char*)&size_data_inf, sizeof(size_data_inf));	//	размер меток данных сервисных буферов
		pFile->ReadDataChunk((unsigned char*)&size_data, sizeof(size_data));	//	размер данных сервисных буферов
		//	считываем буфера
		buff_pos_inf	= new unsigned char[size_pos_inf];
		buff_pos		= new unsigned char[size_pos];
		pFile->ReadDataChunk((unsigned char*)buff_pos_inf, size_pos_inf);	//	информация о позициях
		pFile->ReadDataChunk((unsigned char*)buff_pos, size_pos);			//	позиции
		m_si_offset.resize(size_data_inf >> 1);	//	unsigned short, из размера переводим в количество
		pFile->ReadDataChunk((unsigned char*)&m_si_offset[0], size_data_inf);
		m_si_data.resize(size_data);
		pFile->ReadDataChunk((unsigned char*)&m_si_data[0], size_data);
	}

	//	декодируем позиции
	unsigned char*	p_pos						= buff_pos;
	m_positions.resize(count);
	for (unsigned int	i = 0; i < count; ++i)
	{
		//ToConsole(L"i = ", i);
		unsigned char	mask = (unsigned char)(1 << (i & (unsigned int)7));
		//ToConsole(L"mask = ", (unsigned int)mask);
		unsigned int	index = (i >> 3);
		//ToConsole(L"index = ", index);
		//ToConsole(L"buff_pos_inf[index] = ", (unsigned int)buff_pos_inf[index]);
		//	относительная или абсолютная позиция
		if ((buff_pos_inf[index] & mask) == 0)
		{
			//ToConsole(L"относительная, mask = ", (unsigned int)mask);
			//	относительная позиция
			if (i == 0)
			{
				ToConsole(L"ERROR!!! invalid i"); 
				SS_THROW(L"ERROR!!! first position must be absolute");
				//continue;  
			}
			m_positions[i] = m_positions[i-1] + (*p_pos);
			//ToConsole(L"m_positions[i] = ", m_positions[i]);
			p_pos += sizeof(unsigned char);
		}
		else
		{
			//	абсолютная позиция
			//ToConsole(L"__________абсолютная позиция, mask = ", (unsigned int)mask);
			m_positions[i] = *((unsigned int*)p_pos);
			//ToConsole(L"__________m_positions[i] = ", m_positions[i]);
			p_pos += sizeof(unsigned int);
		}
	}

	//	освобождаем буфера
	if (buff_pos_inf) delete[] buff_pos_inf;
	if (buff_pos)	  delete[] buff_pos;
}

unsigned int CTextBlock::FillTextBlockCut(ITextBlockCut* pTextBlockCut, unsigned int uiHeadPos, unsigned int uiTailPos)
{
	if(ISNULL(pTextBlockCut)) return 0;

	if ((uiHeadPos >= m_si_offset.size()) || (uiHeadPos >= m_positions.size()))
	{
		TO_CONSOLE(L"ERROR!!! Invalid HeadPos");
		ToConsole (L"ERROR!!! uiHeadPos", uiHeadPos);
		ToConsole (L"ERROR!!! WordsNumber", (unsigned int)m_si_offset.size());
		return 0;
	}
	
	//в цикле читаем инфу из буфера
	IIndexAndPosition*	pIndexAndPosition	= NULL;
	unsigned int		offset				= 0;
	unsigned int		si_datasize			= 0;
	unsigned int		pos					= 0;

	for	(pos = uiHeadPos;
		(pos <= uiTailPos) && (pos < m_si_offset.size());
		++pos)
	{
		if (m_si_offset.empty())
		{
			ToConsole(L"TBSE: ERROR!!! m_si_offset empty!!!");
			continue;
		}
		//	смещение данных сервисного индекса
		offset = m_si_offset[pos];
		//	определяем размер данных
		if (m_si_data.size() < offset) ToConsole(L"TBSE: ERROR!!! m_si_data.size() < offset");
		si_datasize = (pos != (m_si_offset.size()-1)) ? (m_si_offset[pos+1]-offset) : (m_si_data.size()-offset);
		//	добавляем сервисный индекс
		pIndexAndPosition = pTextBlockCut->AddIndexAndPosition();
		if (pIndexAndPosition == NULL)
		{
			ToConsole(L"TBSE: ERROR!!! pIndexAndPosition == NULL");
			continue;
		}
		//	задаём позицию
		pIndexAndPosition->SetFirstCharPosition(m_positions[pos]);
		//	выставляем данные сервисного индекса
		if (si_datasize == 0) continue;
		pIndexAndPosition->GetServiceIndex()->SetInfoBuff((char*)&m_si_data[offset], si_datasize);
	}

	return pos-uiHeadPos;
}

//--------------------------------------------------------------------//

CTextsBinaryStorageEx::CTextsBinaryStorageEx(INdxFile* pNdxTextsOffsets,
	INdxFile* pNdxTextBlocksOffsets, INdxVectorStorage* pNdxTextBlocksContent)
:m_pNdxTextsOffsets(pNdxTextsOffsets), m_pNdxTextBlocksOffsets(pNdxTextBlocksOffsets), 
m_pNdxTextBlocksContent(pNdxTextBlocksContent), m_uiCurrentTextBlocksNumber(0)
{

}

CTextsBinaryStorageEx::~CTextsBinaryStorageEx(void)
{
}

unsigned int CTextsBinaryStorageEx::GetTextsNumber(void)
{
	//возвращаем индекс текущего текста
	return (m_pNdxTextsOffsets->GetDataUnitSize()/(2*sizeof(unsigned int)));
}

void CTextsBinaryStorageEx::AddIndex(IServiceIndex* pServiceIndex, unsigned int uiCharPosition)
{
	if(ISNULL(m_pNdxTextsOffsets)) return;
	if(ISNULL(m_pNdxTextBlocksOffsets)) return;
	if(ISNULL(m_pNdxTextBlocksContent)) return;

	if(!m_pNdxTextsOffsets->IsOpen() || !m_pNdxTextBlocksOffsets->IsOpen() || !m_pNdxTextBlocksContent->IsOpen()){
		TO_CONSOLE(L"TextsInfo storages not opened!!!");
		return;
	}

	if(!m_CurrentTextBlock.GetCurrentWordsNumber() && !m_uiCurrentTextBlocksNumber){
		//если добавляется первый индекс текста, кладем ссылку на таблицу смещений блоков
		unsigned int uiTextBlocksOffset=m_pNdxTextBlocksOffsets->GetDataUnitSize();
		m_pNdxTextsOffsets->Append((unsigned char*)&uiTextBlocksOffset, sizeof(unsigned int));
	}

	//добавляем индекс в блок
	m_CurrentTextBlock.AddIndex(pServiceIndex, uiCharPosition);

	if(m_CurrentTextBlock.GetCurrentWordsNumber()>=m_uiMaxWordsNumberInTextBlock){
		MoveDownTextBlock();
	}
}

void CTextsBinaryStorageEx::MoveDownTextBlock(void)
{
	if(!m_CurrentTextBlock.GetCurrentWordsNumber()) return;

	//если блок заполнен, кладем его в хранилище
	unsigned int uiIndex=m_CurrentTextBlock.AddToFile(m_pNdxTextBlocksContent);

	//кладем ссылку на текстовый блок
	m_pNdxTextBlocksOffsets->Append((unsigned char*)&uiIndex, sizeof(unsigned int));
	
	//сбрасываем текстовый блок
	++m_uiCurrentTextBlocksNumber;
}

unsigned int CTextsBinaryStorageEx::SetTextComplete(void)
{
	if(ISNULL(m_pNdxTextsOffsets)) return 0;
	if(ISNULL(m_pNdxTextBlocksOffsets)) return 0;
	if(ISNULL(m_pNdxTextBlocksContent)) return 0;

	if(!m_pNdxTextsOffsets->IsOpen() || !m_pNdxTextBlocksOffsets->IsOpen() || !m_pNdxTextBlocksContent->IsOpen()){
		TO_CONSOLE(L"TextsInfo storages not opened!!!");
		return 0;
	}
	
	MoveDownTextBlock();
	
	//если текст пустой, кладем пустое смещение на блоки
	if(!m_uiCurrentTextBlocksNumber){
		unsigned int uiTextBlocksOffset=EMPTY_VALUE;
		m_pNdxTextsOffsets->Append((unsigned char*)&uiTextBlocksOffset, sizeof(unsigned int));
	}

	//кладем колличество текстовых блоков в тексте
	m_pNdxTextsOffsets->Append((unsigned char*)&m_uiCurrentTextBlocksNumber, sizeof(unsigned int));
	m_uiCurrentTextBlocksNumber=0;
	//возвращаем индекс текущего текста
	return GetTextsNumber()-1;
}

ITextBlockCut* CTextsBinaryStorageEx::FillTextBlock(ITextBlock* pTextBlock, CInterval* pInterval, wstring* pwsIndexStorageName)
{
	if(ISNULL(m_pNdxTextsOffsets)) return NULL;
	if(ISNULL(m_pNdxTextBlocksOffsets)) return NULL;
	if(ISNULL(m_pNdxTextBlocksContent)) return NULL;
	if(ISNULL(pTextBlock)) return NULL;
	if(ISNULL(pInterval)) return NULL;
	if(ISNULL(pwsIndexStorageName)) return NULL;
	
	if(!m_pNdxTextsOffsets->IsOpen() || !m_pNdxTextBlocksOffsets->IsOpen() || !m_pNdxTextBlocksContent->IsOpen()){
		TO_CONSOLE(L"TextsInfo storages not opened!!!");
		return NULL;
	}
	
	if(pInterval->m_uiHeadPos>pInterval->m_uiTailPos){
		TO_CONSOLE(L"Interval is corrupted, head after tail!!!");
		return NULL;
	}
	
	if(pTextBlock->GetTextFeature()->GetTextNumber()==427){
	ToConsole(L"TextNumber", pTextBlock->GetTextFeature()->GetTextNumber());
	pInterval->View();
	}
	//по интервалу вычисляем какие текстовые блоки необходимо поднять
	unsigned int uiTextIndex=pTextBlock->GetTextFeature()->GetTextNumber();
	ldiv_t dvHeadBlock=div((long)pInterval->m_uiHeadPos, (long)m_uiMaxWordsNumberInTextBlock); 
	ldiv_t dvTailBlock=div((long)pInterval->m_uiTailPos, (long)m_uiMaxWordsNumberInTextBlock); 

	//читаем информацию по блокам текста, смещение на таблицу и количество блоков
	STextBlockInfo TextInfo; 
	m_pNdxTextsOffsets->SeekPos(uiTextIndex*sizeof(STextBlockInfo), SEEK_SET);
	m_pNdxTextsOffsets->Read((unsigned char*)&TextInfo, sizeof(STextBlockInfo));
	
	//проверяем на количество блоков
	if(!TextInfo.m_uiTextBlockSize){
		TO_CONSOLE(L"Text has no text blocks!!!");
		ToConsole(L"uiTextIndex", uiTextIndex);
		return NULL;
	}

	if((unsigned int)dvHeadBlock.quot>=TextInfo.m_uiTextBlockSize){
		TO_CONSOLE(L"Invalid head block index!!!");
		ToConsole(L"TextIndex", pTextBlock->GetTextFeature()->GetTextNumber());
		ToConsole(L"HeadBlockIndex", (unsigned int)dvHeadBlock.quot);
		ToConsole(L"Blocks In Text", TextInfo.m_uiTextBlockSize);
		return NULL;
	}

	//позиционируемся в таблице на нужном блоке
	unsigned int uiTextBlockIndex=0; 
	unsigned int uiTextBlockSize=0; 
	m_pNdxTextBlocksOffsets->SeekPos(TextInfo.m_uiTextBlockOffset+dvHeadBlock.quot*sizeof(unsigned int), SEEK_SET);

	//теперь в цикле заполняем интервалом TextBlock
	ITextBlockCut* pTextBlockCut=pTextBlock->AddBlockCut();
	if(ISNULL(pTextBlockCut)) return NULL;
	//устанавливаем вес блока
	pTextBlockCut->SetRelevance(pInterval->m_uiWeight);
	//устанавливаем имя индексного хранилища в котором найден блок
	pTextBlockCut->SetIndexTypeName(pwsIndexStorageName->c_str());

    //в цикле подымаем блоки наполняем блоккат
	unsigned int uiReadedWords=0;
	unsigned int uiCurrentBlockIndex=dvHeadBlock.quot;
	unsigned int uiHeadPosInBlock=dvHeadBlock.rem;
	unsigned int uiTailPosInBlock=0xffffffff;
	while(uiReadedWords<pInterval->GetLength() && uiCurrentBlockIndex<TextInfo.m_uiTextBlockSize){
		//читаем информацию по блоку
		m_pNdxTextBlocksOffsets->Read((unsigned char*)&uiTextBlockIndex, sizeof(unsigned int));
		//позиционируемся на содержимом
		m_pNdxTextBlocksContent->MoveToAndGetDataSize(uiTextBlockIndex, &uiTextBlockSize);
		
		//подымаем блок
		m_CurrentTextBlock.FillFromFile(m_pNdxTextBlocksContent, uiTextBlockSize);
		
		//напоняем TextBlockCut определенным количеством слов
		if(uiCurrentBlockIndex==dvTailBlock.quot){
			uiTailPosInBlock=dvTailBlock.rem;
		}
		uiReadedWords+=m_CurrentTextBlock.FillTextBlockCut(pTextBlockCut, uiHeadPosInBlock, uiTailPosInBlock);
		
		//переходим к следующему блоку
		uiHeadPosInBlock=0;
		++uiCurrentBlockIndex;
	}
	return pTextBlockCut;
}

//--------------------------------------------------------------------//

}
}
}
}
}