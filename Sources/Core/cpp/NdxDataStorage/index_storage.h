#pragma once

#include ".\files_collections.h"
#include ".\level_storage.h"

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

///индексное хранилище
class CNdxStorage : public CFilesCollection, public virtual SS::Interface::Core::NdxSE::NdxDataStorage::INdxStorage
{
friend class CDataUnitFactory;
friend class CDataStorageFactory;

protected:
	CNdxStorage(CDataUnitFactory* pDataStorageFactory);
	virtual ~CNdxStorage(void);

	///типы хранилища индекса
	enum EIndexStorageUnits
	{
		///таблица точек входа
		eisuEPTable,
		///хранилище образов документов
		eisuTextImages,
		eisuTextImages_TextsOffsets,
		eisuTextImages_TextBlocksOffsets,
		eisuTextImages_TextBlocksContent,
		///хранилище информации о структуре уровней координат
		eisuLevelsInfo,
		///хранилище координат индексов
		eisuCoordinates,
		///таблица норм документов
		eisuNormTable,
		///хранилище частот индексов
		eisuFreqUnit
	};

public:


	//-----------IBase
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	//-----------Eof-IBase

	//-----------INdxDataUnit
	///создает юнит данных 
	void Create(const wchar_t* pszDataUnitName, const wchar_t* pszDataUnitExt);
	///открывает файл в текущем режиме
	bool Open(const wchar_t* pszPath);
	//-----------Eof-INdxDataUnit

	//-----------INdxStorage
	///возвращает хранилище точек входы в индекс
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxTableStorage* GetEntryPointsStorage(void);
	///возвращает хранилище образов документов
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* GetTextImagesStorage(void);
	///возвращает хранилища образов документов
	void GetTextImagesStorages(SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile** ppNdxTextsOffsets, 
		SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile** ppNdxTextBlocksOffsets, 
		SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage** ppNdxTextBlocksContent);
	///возвращает хранилище координат
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* GetCoordinateStorage(void);

	///добавляет информацию по координатному уровню
	void AddLevelInfo(SS::Interface::Core::NdxSE::NdxDataStorage::SNdxLevelInfo* pNdxLevelInfo);
	///возвращает информацию по координатному уровню по номеру
	SS::Interface::Core::NdxSE::NdxDataStorage::SNdxLevelInfo const* GetLevelInfo(unsigned int uiLevelNumber);
	///возвращает число координатных уровней
	unsigned int GetLevelsNumber(void){return (unsigned int)m_NdxLevelInfoCollection.size();};
	//-----------Eof-INdxStorage

protected:
	///генерирует имя файла коллекции
	void GenerateFileName(unsigned int uiFileIndex, wstring* pszFileName);
	///создает файл коллекции
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* CreateFile(unsigned int uiFileIndex);
	///создает таблицу точек входа
	void CreateEntryPointsStorage(SS::Interface::Core::NdxSE::NdxDataStorage::INdxTableStorage* pTableStorage);
	///обновляет информацию о структуре уровней
	void UpdateLevelsInfo(void);
	///сохраняет информацию о структуре уровней
	void SaveLevelsInfo(void);

	///информация о структуре уровней
	vector<SS::Interface::Core::NdxSE::NdxDataStorage::SNdxLevelInfo> m_NdxLevelInfoCollection;
};

//--------------------------------------------------------------------//

#pragma warning( default : 4250 )

}
}
}
}
