#pragma once

#include ".\console.h"
#include ".\const.h"
#include ".\basisfile.h"
#include ".\file_cash_members.h"


namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{
namespace FileCash
{

class SS::Core::NdxSE::NdxDataStorage::CBasisFile;

//--------------------------------------------------------------------//

///файловый кэш
class CFileCash
{
public:
	typedef CCircularBuffer<CBuffer> TFileCashBuffers;

	CFileCash(unsigned int uiBufferSize, unsigned int uiBuffersMaxNumber);
	virtual ~CFileCash(void){Clear();};

	///инициализация кэша
	void Init(SS::Core::NdxSE::NdxDataStorage::CBasisFile* pCashedFile){if(!m_pCashedFile) m_pCashedFile=pCashedFile;};

	///добавляет данные в буфер
	virtual void AddData(unsigned char* pucData, unsigned int uiSize);
	///читает данные по текущей позиции
	virtual void Read(unsigned char* pucData, unsigned int uiSize);
	///читает данные по текущей позиции, без сдвига позиции
	virtual void Look(unsigned char* pucData, unsigned int uiSize);
	///пишет данные по текущей позиции
	virtual void Write(unsigned char* pucData, unsigned int uiSize);

	///перемещеает текущую позицию в кэше
	void SeekPos(unsigned int uiOffset, unsigned int uiOrigin);
	///получение текущей позиции
	unsigned int TellPos(void);

	///устанавливает смещение в файле
	void	SetFileOffset(unsigned int uiValue){m_uiFileOffset=uiValue;};
	///возвращает смещение в файле
	unsigned int	GetFileOffset(void){return m_uiFileOffset;};
	///возвращает размер данных в кэше
	unsigned int	GetFilledSize(void);

	///сбрасывает данные в файл
	void FlushToFile(void);
	///сбрасывает данные из заполненных буферов кэша файла в файл
	void FlushFilledBuffersToFile(void);

	///очищает кэш освобождает память
	void Clear(void);
	
	///отображает содержимое объекта
	void View(void);

protected:
	///добавляет буфер
	CBuffer* AddBuffer(void);

	///текущая позиция в кэше
	CCashPosition m_CurrentCashPosition;
	///колекция буферов кэша
	TFileCashBuffers m_FileCashBuffers;
	///файл, который обслуживается кэшем
	CBasisFile* m_pCashedFile;
	///размер буфера коллекции
	const unsigned int m_uiBufferSize;
	///текущее смещение в файле откуда начинается кэш
	unsigned int m_uiFileOffset;
};

//--------------------------------------------------------------------//

///файловый кэш чтения
class CReadFileCash : public CFileCash
{
public:
	CReadFileCash(unsigned int uiBufferSize);
	virtual ~CReadFileCash(void);

	///читает файл в буфер
	void	FillFromFile(void);

protected:
	///добавляет данные в буфер
	void AddData(unsigned char* pucData, unsigned int uiSize){TO_CONSOLE(L"Proxed");};
	///пишет данные по текущей позиции
	void Write(unsigned char* pucData, unsigned int uiSize){TO_CONSOLE(L"Proxed");};
	///сбрасывает данные в файл
	void	FlushToFile(CBasisFile* pFile){TO_CONSOLE(L"Proxed");};
	///сбрасывает данные из заполненных буферов кэша файла в файл
	void	FlushFilledBuffersToFile(CBasisFile* pFile){TO_CONSOLE(L"Proxed");};
};

//--------------------------------------------------------------------//

}
}
}
}
}