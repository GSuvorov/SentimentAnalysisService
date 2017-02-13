#pragma once

#include "../ASDInterface/INdxDataStorage.h"
#include "../ASCInterface/ICommonContainers.h"
#include ".\int_containers.h"
#include ".\basis_storage.h"
#include ".\memory_management.h"

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

///базовый класс для хранилищ отображений текстов
class CTextsBinaryStorage : public CBasisStorage
{
public:
	///добавляет индекс в хранилище
	virtual void AddIndex(SS::Interface::Core::CommonContainers::IServiceIndex* pServiceIndex, unsigned int uiCharPosition) = 0;
	///устанавливает окончание блока текста, сбрасывает накопленный буфер в хранилище
	virtual unsigned int  SetTextBlockComplete(void) = 0;
	///устанавливает окончание текста
	virtual unsigned int SetTextComplete(void) = 0;
	///заполняет блок текста по интервалу
	virtual SS::Interface::Core::CommonContainers::ITextBlockCut* FillTextBlock(SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock, 
			SS::Core::NdxSE::NdxProcessor::Containers::CInterval* pInterval,
			wstring* pwsIndexStorageName) = 0;
	///возвращает количество текстов
	virtual unsigned int GetTextsNumber(void) = 0;

};

//--------------------------------------------------------------------//

///работает с бинарной информацией по текстам
class CTextsBinaryStorageSimple : public CTextsBinaryStorage
{
public:
	typedef vector<unsigned char> TTextBuffer;

	CTextsBinaryStorageSimple(SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* pNdxVectorStorage);
	virtual ~CTextsBinaryStorageSimple(void);
	
	///осуществляет подготовку хранилища индексацию
	void PrepareIndexing(void){};
	///выполняет действия хранилища по окончании индексации
	void CompleteIndexing(void){};
	///выполняет действия хранилища по окончании индексации документа
	void CompleteDocumentIndexing(void){};

	///добавляет индекс в хранилище
	void AddIndex(SS::Interface::Core::CommonContainers::IServiceIndex* pServiceIndex, unsigned int uiCharPosition);
	///устанавливает окончание блока текста, сбрасывает накопленный буфер в хранилище
	unsigned int  SetTextBlockComplete(void);
	///устанавливает окончание текста
	unsigned int SetTextComplete(void);
	///заполняет блок текста по интервалу
	SS::Interface::Core::CommonContainers::ITextBlockCut* FillTextBlock(SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock, 
			SS::Core::NdxSE::NdxProcessor::Containers::CInterval* pInterval,
			wstring* pwsIndexStorageName);
	///возвращает количество текстов
	unsigned int GetTextsNumber(void){return m_pNdxVectorStorage->GetIndexesNumber();};

protected:
	///добавляет сервисную информацию
	void AddServiceInfo(unsigned char* pServiceInfo, unsigned int uiServiceInfoSize);

	///буфер текущей сервисной информации
	MemoryManagement::CBuffer m_ServiceInfoBuffer;
	///буфер индексов текста
	TTextBuffer m_TextBuffer;
	///размер текущего буфера сервисного индекса
	unsigned int m_uiServiceIndexBufferSize;
	///текущее количество добавленных блоков текста
	unsigned int uiCurrentTextBlockNumber;

	///физическое векторное хранилище данных
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* m_pNdxVectorStorage;
};

//--------------------------------------------------------------------//

}
}
}
}
}