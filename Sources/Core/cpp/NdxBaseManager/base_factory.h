#pragma once

#include "..\ASDInterface\INdxDataStorage.h"
#include ".\ndx_configurator.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxBaseManager
{

//--------------------------------------------------------------------//
class CNdxBaseManager;

///базовая фабрика индексной базы
class CNdxBaseFactory
{
public:
	CNdxBaseFactory(void);
	virtual ~CNdxBaseFactory(void);

	///создает индексную базу
	virtual void CreateBase(CNdxBaseManager* pNdxBaseManager);

protected:
	///создает хранилище информации о проиндексиованных текстах
	void CreateTextInfo(CNdxBaseManager* pNdxBaseManager);
	///создает хранилище путей к текстам в виде дерева путей
	void CreateTextPathStorage(CNdxBaseManager* pNdxBaseManager);
	///создает хранилище полей текстов в виде таблицы DBMS
	void CreateTextFieldsStorage(CNdxBaseManager* pNdxBaseManager);
	///создает индексное хранилище
	void CreateIndexStorage(CNdxBaseManager* pNdxBaseManager, const wchar_t* wszIndexStorageName, 
		SS::Interface::Core::NdxSE::ENdxStorageConfig StorageConfig, bool bStorageWithStatistic);
	///создает коллекцию индексных хранилищ по системным настройкам
	void CreateIndexStoragesBySettings(CNdxBaseManager* pNdxBaseManager);
	
	///интерфейс с менеджером загрузки объектов
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataStorageFactory* m_pNdxDataStorageFactory;
	///конфигуратор индексных хранилищ
	CNdxConfigurator m_NdxConfigurator;
};

//--------------------------------------------------------------------//

}
}
}
}