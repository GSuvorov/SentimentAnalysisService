#pragma once

#include ".\cashed_files.h"
#include ".\files_collections.h"

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

///файл обновляемых данных
class CUpdateFile : public CFilesCollection
{
friend class CDataUnitFactory;
 
protected:
	CUpdateFile(CDataUnitFactory* pDataStorageFactory);
	virtual ~CUpdateFile(void);

	///типы файлов хранилища
	enum EUpdateStorageFiles
	{
		///файл смещений
		eusfOffsets,
		///файл свободных смещений
		eusfFreeOffsets,
		///файл данных
		eusfData
	};

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
	//-----------Eof-INdxDataUnit

	///смещение в файле данных
	void	SeekPosInData(unsigned int uiOffset, unsigned int uiOrigin);
	///запись в файл данных
	void	WriteData(unsigned char* pBuffer, unsigned int uiSize);
	
	//чтение данных
	///смещается на начало данных, возвращает их размер
	void	StartRead(unsigned int uiOffset, unsigned int* puiDataSizeInFile);
	///читает кусок данных смещает позицию
	void	Read(unsigned char* pData, unsigned int uiDataSize);
	///завершает чтение возвращет размер считанных данных
	void	StopRead(unsigned int* puiReadedDataSize);

	//добавление данных
	///смещается на начало данных, возвращает их смещение
	void	StartAppend(unsigned int* puiOffset);
	///добавляет кусок данных смещает позицию
	void	Append(unsigned char* pData, unsigned int uiDataSize);
	///завершает добавление возвращет размер добавленных данных
	void	StopAppend(unsigned int* puiAppendedDataSize);

	///удаление данных
	void DeleteData(unsigned int uiOffset);
	
	///возвращает размер отработанных данных (считанных либо, добавленных)
	unsigned int GetWorkedDataSize(void){return m_uiWorkedDataSize;};
	
	///отображает позицию в файле
	void ViewFilePos(void);

	///шринткует, удаляет помеченные записи, дефрагментирует оставшиеся
	bool Shrink(void);

protected:
	///информация о данных
	struct SDataInfo
	{
		///смещение на данные
		unsigned int m_uiDataOffset;
		///размер данных
		unsigned int m_uiDataSize;
	};
	typedef vector<SDataInfo> TDataInfoCollection;
	
	///шринкует порцию данных по TDataInfoCollection, переписывает данные из старого 
	//файла данных в новый
	void ShrinkData(TDataInfoCollection* pDataInfoCollection, 
		SS::Interface::Core::NdxSE::NdxDataStorage::INdxCashedFile* pDataFile,
		SS::Interface::Core::NdxSE::NdxDataStorage::INdxCashedFile* pNewDataFile);
	///подменяет файл данных входным
	void ReplaceDataFile(SS::Interface::Core::NdxSE::NdxDataStorage::INdxCashedFile* pNewDataFile);

	///смещает позицию к свободному смещению
	unsigned int SeekToFreeOffset(void);
	///читает свободные смещения
	void MoveUpFreeOffsets(void);
	///сбрасывает на диск свободные смещения
	void MoveDownFreeOffsets(void);
	///добавляет свободное смещение
	void AddFreeOffset(unsigned int uiOffset);

	///генерирует имя файла коллекции
	void GenerateFileName(unsigned int uiFileIndex, wstring* pszFileName);
	///создает файл коллекции
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* CreateFile(unsigned int uiFileIndex);
	///файл смещений на данные содержит: смещение на данные|размер данных
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* GetOffsetsFile(void)
	{
		//return m_FilesCollection.empty()?NULL:dynamic_cast<SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile*>(m_FilesCollection[eusfOffsets]);
		if(!m_pOffsetFile){
			m_pOffsetFile=m_FilesCollection.empty()?NULL:dynamic_cast<SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile*>(m_FilesCollection[eusfOffsets]);
		}

		return m_pOffsetFile;
	};
	///файл свободных смещений
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* GetFreeOffsetsFile(void){return m_FilesCollection.empty()?NULL:dynamic_cast<SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile*>(m_FilesCollection[eusfFreeOffsets]);};
	///файл данных блоков 
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxCashedFile* GetDataFile(void)
	{
		//return m_FilesCollection.empty()?NULL:dynamic_cast<SS::Interface::Core::NdxSE::NdxDataStorage::INdxCashedFile*>(m_FilesCollection[eusfData]);

		if(!m_pDataFile){
			m_pDataFile=m_FilesCollection.empty()?NULL:dynamic_cast<SS::Interface::Core::NdxSE::NdxDataStorage::INdxCashedFile*>(m_FilesCollection[eusfData]);
		}

		return m_pDataFile;
	};
	///создает новый файл данных
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxCashedFile* CreateAndOpenNewDataFile(void);

	///указатель на файл данных
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxCashedFile* m_pDataFile;
	///указатель на файл смещений на данные
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* m_pOffsetFile;

	///массив свободных смещений
	vector<unsigned int> m_FreeOffsets;

	///максимально возможный размер отработанных данных (считанных либо, добавленных)
	unsigned int m_uiMaxWorkedDataSize;
	///размер отработанных данных (считанных либо, добавленных)
	unsigned int m_uiWorkedDataSize;
	///максимальный размер единовременно добавляемых в граничный файл данных
	const unsigned int m_cuiMaxWorkedDataSize;

	///размер файла при котором можно шринковать
	const unsigned int m_cuiDataSizeToShrink;
	///процент удаленных данных при котором можно шринковать
	const unsigned int m_cuiDeletedDataPercentageToShrink;
	///число удаленных данных
	unsigned int m_uiDeletedOffsetsNumber;
};

//--------------------------------------------------------------------//

}
}
}
}