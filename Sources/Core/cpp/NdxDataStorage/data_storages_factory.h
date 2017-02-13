#pragma once

#include "..\ASDInterface\INdxDataStorage.h"
#include "./basisfile.h"
#include "./cashed_files.h"
#include "./vector_storage.h"
#include "./index_storage.h"
#include "./index_stat_storage.h"
#include "./table_storage.h"


namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{

//--------------------------------------------------------------------//

class CDataUnitFactory
{
friend class CDataUnit;

public:
	CDataUnitFactory(void);
	virtual ~CDataUnitFactory(void);

	///создает базовый индексный файл
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* CreateBasisFile(void);
	///создает индексный файл поддерживающий кэширование чтение-записи
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxCashedFile* CreateCashedFile(void); 
	///создает табличное хранилище данных
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxTableStorage* CreateTableStorage(void);
	///создает коллекцию юнитов данных
	CFilesCollectionEx* CreateFilesCollectionEx(void);
	///создает хранилище обновляемых данных
	CUpdateFile* CreateUpdateFile(void);
	///создает файл с кэшированием добавляемых данных
	CACashedFile* CreateAppendingCashedFile(void);
	///создает индексного уровня
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* CreateLevelStorage(void);
	///создает векторное хранилище данных
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* CreateVectorStorage(void);

protected:
	///регистритрует юнит даннх в фабрике
	virtual void Register(SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* pNdxDataUnit){};
	///разрегистритрует юнит даннх в фабрике
	virtual void UnRegister(SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* pNdxDataUnit){};

	///менеджер баз DBMS
	SS::Interface::Core::DBMS::IDBManager* m_pDBMSManager;
	///текущая вирутальная база,в ней будут создаваться таблицы
	SS::Interface::Core::DBMS::IDataBase* m_pDBMSBase;
	
	///флаг регистрации создаваемых объектов
	bool m_bRegister;
};

//--------------------------------------------------------------------//

class CDataStorageFactory : public CDataUnitFactory,
	public SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataStorageFactory
{
friend class CDataUnit;

protected:
	typedef set<SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit*> TDataUnitCollection;

public:
	CDataStorageFactory(void);
	virtual ~CDataStorageFactory(void);

	//-----------IBase
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	///устанавливает указатель на лоад мэнеджер
	void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
	///освобождение
	ULONG Release(void);
	//-----------Eof-IBase

	//-----------INdxDataStorageFactory
	///создает базовый индексный файл
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* CreateNdxFile(void);
	///создает индексный файл поддерживающий кэширование чтение-записи
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxCashedFile* CreateNdxCashedFile(void); 
	///создает табличное хранилище данных
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxTableStorage* CreateNdxTableStorage(void);
	///создает векторное хранилище данных
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* CreateNdxVectorStorage(void);
	///создает хранилище индексных данных
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxStorage* CreateNdxStorage(void);
	///создает хранилище индексных данных с поддержкой статистики
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxStorageWithStatistic* CreateNdxStorageWithStatistic(void);
	//-----------Eof-INdxDataStorageFactory

protected:
	///регистритрует юнит даннх в фабрике
	void Register(SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* pNdxDataUnit);
	///разрегистритрует юнит даннх в фабрике
	void UnRegister(SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* pNdxDataUnit);
	///разрегистритрует все юниты в фабрики
	void UnRegisterAll(void);

	///коллекция юнитов создаваемых фабрикой
	TDataUnitCollection m_DataUnitCollection;
};

//--------------------------------------------------------------------//

}
}
}
}