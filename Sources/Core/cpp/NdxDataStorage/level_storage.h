#pragma once

#include ".\update_file.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{

//скрывает варнинг о наследовании
#pragma warning( disable : 4250 )

//--------------------------------------------------------------------//

///коллекция файлов с кэшированием
class CLevelStorage : public CFilesCollectionEx, public SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage
{
friend class CDataUnitFactory;
 
protected:
	CLevelStorage(CDataUnitFactory* pDataStorageFactory);
	virtual ~CLevelStorage(void);

public:
	//-----------IBase
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	//-----------Eof-IBase

	//-----------INdxDataUnit
	///открывает файл в текущем режиме
	bool Open(const wchar_t* pszPath);
	//-----------Eof-INdxDataUnit

	//-----------INdxLevelStorage
	///перемещение по коллекции файлов
	void	SeekPos(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pOffset, unsigned int uiOrigin);
	///возвращает позицию в коллекции файлов
	void	TellPos(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pOffset);

	//сессионное чтение из коллекции файлов
	///начинает чтение по данной позиции
	bool	StartRead(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pOffset, unsigned int* puiDataSizeInFile);
	///читает в буфер
	void	Read(unsigned char* pBuffer, unsigned int uiSize);
	///заканчивает чтение, возвращает размер прочитанного
	void	StopRead(unsigned int* puiReadedDataSize);
	
	//сессионное добавление к коллекции файлов
	///начинает добавление завершенных данных
	void	StartCompleteDataAppend(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pOffset);
	///начинает добавление незавершенных данных
	void	StartUnCompleteDataAppend(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pOffset);
	///добавление буфера данных
	void	Append(unsigned char* pBuffer, unsigned int uiSize);
	///заканчивает добавление, возвращает размер добавленного
	void	StopAppend(unsigned int* puiAppendedDataSize);

	///возвращает размер добавленых изменяемых данных
	unsigned int GetAppendedUnCompleteDataSize(void);
	///позиционирует в добавленых изменяемых данных
	void	SeekPosInAppendedUnCompleteData(unsigned int uiOffset, unsigned int uiOrigin);
	///пишет буфер данных в добавленые изменяемых данных
	void	WriteInAppendedUnCompleteData(unsigned char* pBuffer, unsigned int uiSize);

	///удаление из коллекции файлов
	void	Delete(SS::Interface::Core::NdxSE::NdxDataStorage::SCompositeOffset* pOffset);
	
	///обновляет файлы коллекции
	void	UpdateData(void);
	//-----------Eof-INdxLevelStorage

protected:
	///генерирует имя файла коллекции
	void GenerateFileName(unsigned int uiFileIndex, wstring* pszFileName);
	///создает файл коллекции
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* CreateFile(unsigned int uiFileIndex);
	///возвращает файл обновляемых данных коллекции
	CUpdateFile* GetUpdateFile(void);

	///максимальный размер файла коллекции
	const unsigned int	m_cuiMaxFileSize;
	///максимальное количество файлов в коллекции
	const unsigned int	m_cuiMaxFilesNumber;
};

//--------------------------------------------------------------------//

#pragma warning( default : 4250 )

}
}
}
}