#pragma once

#include ".\const.h"
#include ".\console.h"
#include "..\ASDInterface\INdxDataStorage.h"

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

///абстрактный юнит индексной базы данных и функции по работе с ним
class CDataUnit : public virtual SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit
{
public:
	CDataUnit(CDataUnitFactory* pDataStorageFactory);
	virtual ~CDataUnit(void);
	
	//-----------IBase
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	///освобождение
	ULONG Release(void);
	//-----------Eof-IBase

	//-----------INdxDataUnit
	///создает юнит данных 
	void Create(const wchar_t* pszDataUnitName, const wchar_t* pszDataUnitExt);
	///проверяет открыт ли юнит данных 
	bool IsOpen(void);
	///возвращает имя юнита данных 
	const wchar_t*	GetDataUnitName(void);
	///возвращает расширение юнита данных 
	const wchar_t* GetDataUnitExt(void);
	///устанавливает настройки бакапирования
	void SetBackupSettings(
		SS::Interface::Core::NdxSE::NdxDataStorage::SBackupSettings::ECompressMode CompressMode, 
		SS::Interface::Core::NdxSE::NdxDataStorage::SBackupSettings::EWriteMode WriteMode);
	//-----------Eof-INdxDataUnit
	
	///возвращает фабрику хранилищ
	CDataUnitFactory* GetDataStorageFactory(void){return m_pDataStorageFactory;};

protected:
	///имя юнита данных 
	wstring	m_wszDataUnitName;
	///расширение юнита данных 
	wstring	m_wszDataUnitExt;
	///флаг открытия юнита данных
	bool m_bIsOpen;
	///фабрика создания хранилищ
	CDataUnitFactory* m_pDataStorageFactory;
	///параметры бакапирования
	SS::Interface::Core::NdxSE::NdxDataStorage::SBackupSettings m_BackupSettings;
};

//--------------------------------------------------------------------//

}
}
}
}