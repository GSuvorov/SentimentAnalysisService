#pragma once

#include ".\index_storage.h"

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

///индексное хранилище с подержжкой хранилищ статистики
class CStatisticNdxStorage : public CNdxStorage, public SS::Interface::Core::NdxSE::NdxDataStorage::INdxStorageWithStatistic
{
friend class CDataUnitFactory;
friend class CDataStorageFactory;

protected:
	CStatisticNdxStorage(CDataUnitFactory* pDataStorageFactory);
	virtual ~CStatisticNdxStorage(void);

public:
	//-----------IBase
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	//-----------Eof-IBase

	//-----------INdxDataUnit
	///создает юнит данных 
	void Create(const wchar_t* pszDataUnitName, const wchar_t* pszDataUnitExt);
	//-----------Eof-INdxDataUnit

	//-----------INdxStorageWithStatistic
	///возвращает хранилище частот индексов
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* GetIndexFrequencyStorage(void);
	///возвращает хранилище норм документов индекса
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* GetDocumentNormStorage(void);
	//-----------Eof-INdxStorageWithStatistic

protected:
	///генерирует имя файла коллекции
	void GenerateFileName(unsigned int uiFileIndex, wstring* pszFileName);
	///создает файл коллекции
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* CreateFile(unsigned int uiFileIndex);
};

//--------------------------------------------------------------------//

#pragma warning( default : 4250 )

}
}
}
}
