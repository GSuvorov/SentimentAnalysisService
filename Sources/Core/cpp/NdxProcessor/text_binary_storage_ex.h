#pragma once

#include "../ASDInterface/INdxDataStorage.h"
#include "../ASCInterface/ICommonContainers.h"
#include ".\texts_binary_storage.h"
#include ".\int_containers.h"
#include ".\basis_storage.h"
#include ".\memory_management.h"
#include ".\byte_align.h"

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

///текстовый блок, содержит сервисные буфера слов
class CTextBlock
{
public:
	///добавляет индекс в блок
	void AddIndex(SS::Interface::Core::CommonContainers::IServiceIndex* pServiceIndex, unsigned int uiCharPosition);
	///наполняет ITextBlockCut словами из текстового блока, елси не указывать параметры то наполнит всем содержимым блока
	///возвращает количество действительно прочитанных блоков
	unsigned int FillTextBlockCut(SS::Interface::Core::CommonContainers::ITextBlockCut* pTextBlockCut, unsigned int uiHeadPos = 0, unsigned int uiTailPos = 0xffffffff);
	///кладет блок в файл, возвращает количество записанных байт
	unsigned int AddToFile(SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* pFile);
	///подымает блок из файла
	void FillFromFile(SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* pFile, unsigned int uiTextBlockBinarySize);
	///возвращает текущее количество слов в блоке
	__inline unsigned int GetCurrentWordsNumber(void){return (unsigned int)m_si_offset.size();};
	
protected:
	///сбрасывает и очищает текстовый блок
	__inline void Reset(void)
	{
		m_positions.clear();
		m_si_offset.clear();
		m_si_data.clear();
	};

	///	функциональность для сжатия и расжатия методом ByteAlign
	//не нужна Containers::CByteAlignCompress	m_ByteAlignCompress;
	///	буфер позиций
	std::vector<unsigned int>		m_positions;
	///	буфер меток данных сервисных индексов
	std::vector<unsigned short>		m_si_offset;
	///	буфер данных сервисных индексов
	std::vector<unsigned char>		m_si_data;
};

//--------------------------------------------------------------------//

///работает с бинарной информацией по текстам
///Состоит из 3 хранилищ:
///	хранилище смещений на таблицу блоков текста
///	хранилище смещений на блоки текстов
///	хранилище блоков текстов
///Для хранения принято, что: 
//текст состоит из блоков, каждый блок, кроме последнего содержит N слов (сервисных буферов)
class CTextsBinaryStorageEx : public CTextsBinaryStorage
{
public:
	CTextsBinaryStorageEx(SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* pNdxTextsOffsets,
		SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* pNdxTextBlocksOffsets,
		SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* pNdxTextBlocksContent);
	virtual ~CTextsBinaryStorageEx (void);

	///осуществляет подготовку хранилища индексацию
	void PrepareIndexing(void){};
	///выполняет действия хранилища по окончании индексации
	void CompleteIndexing(void){};
	///выполняет действия хранилища по окончании индексации документа
	void CompleteDocumentIndexing(void){};

	///добавляет индекс в хранилище
	void AddIndex(SS::Interface::Core::CommonContainers::IServiceIndex* pServiceIndex, unsigned int uiCharPosition);
	///устанавливает окончание блока текста, сбрасывает накопленный буфер в хранилище
	//не используется
	unsigned int  SetTextBlockComplete(void){return 0;};
	///устанавливает окончание текста
	unsigned int SetTextComplete(void);
	///заполняет блок текста по интервалу
	SS::Interface::Core::CommonContainers::ITextBlockCut* FillTextBlock(SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock, 
			SS::Core::NdxSE::NdxProcessor::Containers::CInterval* pInterval,
			wstring* pwsIndexStorageName);
	///возвращает количество текстов
	unsigned int GetTextsNumber(void);
	
	//константы
	///максимальное количество слов в текстовом блоке
	static const unsigned int m_uiMaxWordsNumberInTextBlock=1000;
	///флаг сжатия текстовых блоков
	static const bool m_bCompressTextBlocks=true;

protected:
	///информация по текстовому блоку
	struct STextBlockInfo
	{
		///смещение на текстовый блок
		unsigned int m_uiTextBlockOffset;
		///размер текстового блока
		unsigned int m_uiTextBlockSize;
	};

	///кладет текстовый блок на диск
	void MoveDownTextBlock(void);

	///текущий блок текста
	CTextBlock m_CurrentTextBlock;
	///текущее количество блоков в текущем тексте
	unsigned int m_uiCurrentTextBlocksNumber;

	///хранилище смещений на таблицу блоков текста
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* m_pNdxTextsOffsets;
	///хранилище смещений на блоки текстов
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* m_pNdxTextBlocksOffsets;
	///хранилище блоков текстов
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* m_pNdxTextBlocksContent;
};

//--------------------------------------------------------------------//

}
}
}
}
}