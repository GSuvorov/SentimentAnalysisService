#pragma once

#include "..\ASDInterface\INdxBaseManager.h"
#include "..\ASDInterface\INdxDataStorage.h"
#include ".\base_factory.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxBaseManager
{

//--------------------------------------------------------------------//

///менеджер управляющий юнитами индексной базы
class CNdxBaseManager : public  SS::Interface::Core::NdxSE::INdxBaseManager
{
public:
	typedef map<wstring, SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit*> TNdxDataUnitCollection;
	typedef vector< pair<wstring, SS::Interface::Core::NdxSE::ENdxStorageConfig> > TNdxBaseStructCollection;

	CNdxBaseManager(void);
	virtual ~CNdxBaseManager(void);

	//-----------IBase
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	///освобождение
	ULONG Release(void);
	//-----------Eof-IBase

	//------------------IBaseManager
	///устанавливает путь к индексным базам
	void SetBasePath(const wchar_t* wszPath);
	///открывает индексную базу
	HRESULT OpenBase(const wchar_t* wszBaseName);
	///очищает индексную базу
	HRESULT ClearBase(void);
	///зaкрывает индексную базу
	void CloseBase(void);
	///осуществляет резервное копирование индексной базы
	HRESULT BackUpBase(const wchar_t* wszPath);
	/// заполнит вектор путями к файлам базы (необходимо для бакапа)
	void GetDatabaseFilePathes(std::vector< std::pair<std::wstring, std::wstring> >* vPathes, std::wstring& sBaseName);

	//------------------Eof-IBaseManager

	//------------------INdxBaseManager
	///сбрасывает структуру индексной базы
	void ResetNdxBaseStructure(void){m_BaseStructCollection.clear();};
	///возвращает юнит индексной базы по входному типу
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* 
		GetNdxBaseUnit(const wchar_t* wszStorageName);
	///добавляет юнит индексной базы к коллекции
	void AddNdxBaseUnit(const wchar_t* wszStorageName, 
		SS::Interface::Core::NdxSE::ENdxStorageConfig StorageConfig);
	///возвращает идентификатор текущей сессии индексной базы,
	///если у клиента не совпадает идентификатор сессии, необходимо обновить
	///свои структуры, так как все хранилищя уже были созданы заново
	unsigned int GetCurrentSessionID(void){return m_uiCurrentSessionID;};
	///возвращает первое в коллекции хранилище индексной базы
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* GetFirstNdxBaseUnit(void);
	///возвращает следующее за текущим в коллекции хранилище индексной базы, 
	//при достижении конца или без вызова GetFirstNdxBaseUnit вернет NULL
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* GetNextNdxBaseUnit(void);
	//------------------Eof-INdxBaseManager


	///удаляет юнит индексной базы из коллекции и на диске по имени
	void DeleteNdxBaseUnit(const wchar_t* wszStorageName);
	///возвращает структуру индексной базы
	TNdxBaseStructCollection* GetBaseStructure(void){return &m_BaseStructCollection;};
	///добавляет юнит индексной базы к коллекции
	void AddNdxBaseUnit(const wchar_t* wszStorageName, SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* pNdxDataUnit);
	///удаляет все дата юниты из коллекции
	void TotalRemoveAll(void);
	
protected:
	///переоткрывает индексную базу
	HRESULT ReOpenBase(void);

	///проверяет по структуре папку
	bool IsFolder(WIN32_FIND_DATA* pFD);
	///очищает папку
	bool ClearFolder(const wchar_t* wszPathFrom);
	
	///текущая база
	wstring m_wsCurrentBase;
	///путь к индексным базам на диск
	wstring m_wsBasePath;
	///флаг открытия базы
	bool m_bIsBaseOpen;
	///идентификатор текущей сессии индексной базы,
	unsigned int m_uiCurrentSessionID;

	///коллекция структуры индексной базы, определяет какие юниты должны быть в индексной базе, в какой конфигурации
	TNdxBaseStructCollection m_BaseStructCollection;
	///коллекция юнитов индексной базы
	TNdxDataUnitCollection m_DataUnitCollection;
	///итератор для возможности обхода юнитов индексной базы извне 
	TNdxDataUnitCollection::iterator m_itCurrentDataUnit;
	///коллекция для инициализации итераторов
	TNdxDataUnitCollection m_NullCollection;
	///фабрика индексной базы
	CNdxBaseFactory m_NdxBaseFactory;
};

//--------------------------------------------------------------------//

}
}
}
}