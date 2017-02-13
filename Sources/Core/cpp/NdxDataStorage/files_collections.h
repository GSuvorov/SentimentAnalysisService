#pragma once

#include ".\basisfile.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{

class CDataUnitFactory;

//--------------------------------------------------------------------//

///коллекция файлов
class CFilesCollection : public CDataUnit
{
friend class CDataUnitFactory;

protected:
	typedef vector<SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit*> TFiles;

	CFilesCollection(CDataUnitFactory* pDataStorageFactory);
	virtual ~CFilesCollection(void);

public:
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

	//---работа с коллекцией
	///удаляет все файлы из коллекции 
	void DeleteFiles(void);
	///добавляет файл к коллекции
	bool AddFile(void);
	///возвращает индекс текущего файла
	unsigned char GetCurrentFileIndex(void);
	///возвращает текущее количество файлов
	unsigned int GetFilesNumber(void){return (unsigned int)m_FilesCollection.size();};
	///переходит к первому файлу в коллекции
	bool MoveFirstFile(void);
	///переходит к файлу в коллекции
	bool MoveToFile(unsigned int uiFileIndex);
	///переходит к последнему файлу в коллекции
	bool MoveLastFile(void);
	///возвращает текущий файл в коллекции
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* GetCurrentFile(void);

protected:
	///генерирует имя файла коллекции
	virtual void GenerateFileName(unsigned int uiFileIndex, wstring* pszFileName);
	///создает файл коллекции
	virtual SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* CreateFile(unsigned int uiFileIndex);

	///коллекция файлов
	TFiles m_FilesCollection;
	///текущий файл
	TFiles::iterator m_itCurrentFile;
	///коллекция для инициализации итераторов
	TFiles m_NullCollection;
	///текущий рабочий путь
	wstring m_szCurrentFilePath;

	///максимальное количество файлов в коллекции
	unsigned int m_uiMaxFilesNumber;
};

//--------------------------------------------------------------------//

///мульти-файл, одновременное открытие файлов одной из коллекции
class CFilesCollectionEx : public CFilesCollection
{
friend class CDataUnitFactory;

protected:
	CFilesCollectionEx(CDataUnitFactory* pDataStorageFactory);
	virtual ~CFilesCollectionEx(void);
	
public:
	//-----------INdxDataUnit
	///открывает файл в текущем режиме
	bool Open(const wchar_t* pszPath);
	///закрывает юнит данных 
	bool Close(void);
	//-----------Eof-INdxDataUnit

	///возвращает текущий файл в коллекции
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* GetCurrentFile(void);

protected:
	///возвращает начальное количество юнитов
	unsigned int ReadDataUnitNumber(void);
	///устанавливает начальное количество юнитов
	void WriteDataUnitNumber(void);
};

//--------------------------------------------------------------------//

}
}
}
}