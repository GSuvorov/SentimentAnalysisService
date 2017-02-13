#pragma once

#include ".\files_collections.h"

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

///хранение данных по векторному индексу, поддерживает произвольное чтение и добавление в конец
class CVectorStorage : public CFilesCollection, 
	public SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage
{
friend class CDataUnitFactory;
friend class CDataStorageFactory;

protected:
	CVectorStorage(CDataUnitFactory* pDataStorageFactory);
	virtual ~CVectorStorage(void);

	///типы файлов хранилища
	enum EVectorStorageFiles
	{
		///индексный файл
		evsfIndex,
		///файл данных
		evsfData
	};

public:
	//-----------IBase
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	//-----------Eof-IBase

	//-----------INdxDataUnit
	///открывает юнит данных в текущем режиме
	bool Open(const wchar_t* pszPath);
	//-----------Eof-INdxDataUnit

	//---чтение данных
	///смещается на начало данных, возвращает их размер
	void	MoveToAndGetDataSize(unsigned int uiIndex, unsigned int* puiDataSizeInFile);
	///читает все данные находящиеся по индексу
	void	ReadWholeData(unsigned char* pData, unsigned int uiDataSize);
	///читает кусок данных находящихся по индексу
	void	ReadDataChunk(unsigned char* pData, unsigned int uiDataSize);
	///смещает внутри данных находящихся по индексу, смещение считается от начала
	void	SeekInData(unsigned int uiOffset);
	

	//---добавление данных
	///добавляет данные в хранилище, возвращает индекс, по которому будут находятся данные
	void	AppendWholeData(unsigned char* pData, unsigned int uiDataSize, unsigned int* puiIndex);
	///добавляет пакет данных, запускает сценарий пакетного добавления
	void	AppendDataChunk(unsigned char* pData, unsigned int uiDataSize);
	///завершает сценарий пакетного добавления, возвращает индекс, по которому будут находятся данные
	void	AppendDataChunkComplete(unsigned int* puiIndex);
	
	//---удаление данных
	///удаляет данные по индексу
	void	Delete(unsigned int uiIndex);
	///проверяет удалены ли данные по индексу
	bool	IsDeleted(unsigned int uiIndex);

	///возвращает количество индексов
	unsigned int GetIndexesNumber(void);

protected:
	///генерирует имя файла коллекции
	void GenerateFileName(unsigned int uiFileIndex, wstring* pszFileName);
	///создает файл коллекции
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* CreateFile(unsigned int uiFileIndex);
	///возвращает индексный файл 
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* GetIndexFile(void){return m_FilesCollection.empty()?NULL:dynamic_cast<SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile*>(m_FilesCollection[evsfIndex]);};
	///возвращает файл данных
	CFilesCollectionEx* GetDataFile(void){return m_FilesCollection.empty()?NULL:dynamic_cast<CFilesCollectionEx*>(m_FilesCollection[evsfData]);};

	///структура которая лежит в индексном файле
	struct SDataInfo
	{
		///смещение в файле	
		unsigned int m_uiOffsetInFile;
		///размер данных
		unsigned int m_uiDataSize;
		///индекс файла
		unsigned char m_ucFileIndex;
		///флаг удаления данных
		bool m_bDeleted;
		
		///сбрасывает поля структуры
		void Reset(void)
		{
			m_uiOffsetInFile=EMPTY_OFFSET;
			m_uiDataSize=0;
			m_ucFileIndex=UC_EMPTY;
			m_bDeleted=false;
		};

		///проверяет на пустоту
		bool IsEmpty(void)
		{
			if(	m_uiOffsetInFile==EMPTY_OFFSET &&
				m_uiDataSize==0 && 
				m_ucFileIndex==UC_EMPTY &&
				m_bDeleted==false) 
				return true;
			else
				return false;
		};
	};

	///текущие данные
	SDataInfo m_CurrDataInfo;
	///текущая информация по добавляемым данным
	SDataInfo m_CurrAppendedDataInfo;
	///текущее смещение внутри данных
	unsigned int m_uiCurrOffsetInData;
	///текущий размер добавляемых данных
	unsigned int m_uiCurrDataSize;
	///максимальный размер одного файла данных
	unsigned int m_uiMaxDataFileSize;
	///максимальное количество файлов данных в коллекции
	unsigned int m_uiMaxDataFilesNumber;

};

//--------------------------------------------------------------------//

#pragma warning( default : 4250 )

}
}
}
}