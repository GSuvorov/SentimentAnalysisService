#pragma once
#include ".\const.h"
#include ".\console.h"
#include "..\ASDInterface\INdxDataStorage.h"
#include ".\basisfile.h"
#include ".\file_cash.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{
class CDataUnitFactory;

//скрывает варнинг о наследовании
#pragma warning( disable : 4250 )

//--------------------------------------------------------------------//

///файл поддержкой кэширования добавляемых данных
class CACashedFile : public CBasisFile, public SS::Interface::Core::NdxSE::NdxDataStorage::INdxCashedFile
{
friend class CDataUnitFactory;

protected:
	CACashedFile(CDataUnitFactory* pDataStorageFactory);
	virtual ~CACashedFile(void);
	
public:
	//-----------IBase
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	//-----------Eof-IBase

	//-----------INdxDataUnit
	///открывает юнит данных в текущем режиме
	bool Open(const wchar_t* pszPath);
	///закрывает юнит данных 
	bool Close(void);
	///сбрасывает юнит данных 
	bool Flush(void);
	///очищает  юнит данных 
	void Clear(void);
	///возвращает размер юнита данных 
	unsigned int GetDataUnitSize(void);
	//-----------Eof-INdxDataUnit

	//-----------INdxFile
	///читает из файла по текущей позиции
	void Read(unsigned char* pBuffer, unsigned int uiSize);
	///пишет в файл по текущей позиции
	void Write(unsigned char* pBuffer, unsigned int uiSize);
	///добавляет к файлу
	void Append(unsigned char* pBuffer, unsigned int uiSize);
	///изменяет текущую позицию в файле
	void SeekPos(unsigned int uiOffset, unsigned int uiOrigin);
	///возвращает текущую позицию в файле
	unsigned int TellPos(void){return m_uiCurrOffset;};
	//-----------Eof-INdxFile


	//-----------INdxCashedFile
	///инициализирует параметры кэша
	void InitCash(unsigned int uiCashBufferSize, unsigned int uiCashMinSizeToFlush){TO_CONSOLE(L"Not supported");};
	///кэширует файл
	void Cashed(void){TO_CONSOLE(L"Not supported");};
	///сбрасывает на диск заполненные буфера кэша файла
	void FlushFilledBuffers(void);
	//-----------Eof-INdxCashedFile
	
	///отображение информации по файлу
	void	View(void);
	///отображение информации по текущей позиции в файле
	void	ViewFilePos(void);

protected:
	///файловый кэш режима индексации
	FileCash::CFileCash m_IndexModeFileCash;

	///текущее смещение в файле (абсолютное от начала)
	unsigned int m_uiCurrOffset;
	///минимальный размер кэша, который можно сбрасывать
	unsigned int m_cuiCashMinSizeToFlush;
};

//--------------------------------------------------------------------//

///файл поддержкой кэширования добавляемых и читаемых данных
class CRACashedFile : public CACashedFile
{
friend class CDataUnitFactory;
friend class CDataStorageFactory;

protected:
	CRACashedFile(CDataUnitFactory* pDataStorageFactory);
	virtual ~CRACashedFile(void);
	
public:
	///открывает файл в текущем режиме
	bool	Open(const wchar_t* pszPath);
	///закрывает файл
	bool	Close(void);
	/////читает данные из файла
	//void	Read(unsigned char* pBuffer, unsigned int uiSize);
	/////устанавливает текущую позицию в файле
	//void	SeekPos(unsigned int uiOffset, unsigned int uiOrigin);

protected:
	/////читает данные из кэша чтения
	//void ReadFromCash(unsigned char* pBuffer, unsigned int uiSize);

	///файловый кэш режима поиска
	FileCash::CReadFileCash m_ReadFileCash;
};

//--------------------------------------------------------------------//

#pragma warning( default : 4250 )

}
}
}
}
