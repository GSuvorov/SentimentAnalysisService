#pragma once

#include ".\data_unit.h"

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

///работа с таблицами базы DBMS
class CTableStorage : public CDataUnit, 
	public SS::Interface::Core::NdxSE::NdxDataStorage::INdxTableStorage
{
friend class CDataUnitFactory;
friend class CDataStorageFactory;

protected:
   CTableStorage(CDataUnitFactory* pDataStorageFactory, SS::Interface::Core::DBMS::IDataBase* pDBMSBase);
   virtual ~CTableStorage(void);

public:
   	//-----------IBase
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	//-----------Eof-IBase

	//-----------INdxDataUnit
	///открывает юнит данных в текущем режиме
	bool Open(const wchar_t* pszPath);
	///закрывает юнит данных 
	bool Close(void);
	///очищает  юнит данных 
	void Clear(void);
	///сбрасывает юнит данных 
	bool Flush(void);
	///возвращает размер юнита данных 
	unsigned int GetDataUnitSize(void);
	//-----------Eof-INdxDataUnit

	//-----------INdxTableStorage
	///добавляет поле в таблицу
	HRESULT AddField(SS::Interface::Core::DBMS::TDataTableFieldDescription* pDataTableFieldDescription);
	///возвращает коллекцию полей таблицы
	TTableFieldCollection* GetFieldsCollection(void){return &m_TTableFieldCollection;};

	///инициализирует процесс поиска в базе
	HRESULT InitFind(wchar_t* pKeyName, void* pKeyValue, void* ArrayOfPointersToData[], unsigned short* pusFindId = 0);
	///ищет в таблице несколько раз по неуникальным значениям пока поиск не закончится неудачей
	HRESULT Find(unsigned int uiFindId = 0);
	///ищет в таблице один раз по уникальному индексу
	HRESULT FindOne(unsigned int uiFindId = 0);

	///инициализирует процесс интервального поиска в базе
	HRESULT SpanFindInit(wchar_t* pKeyName, void* pLowerKeyValue, void* pUpperKeyValue, 
		std::set<unsigned int>* pSetOfRecordsIndexes, unsigned short* pusFindId = 0);
	///ищет в таблице записи удовлетворяющий SpanFindInit
	HRESULT SpanFind(unsigned int uiFindId = 0) ;

	///возвращает индекс записи таблицы найденой перед вызовом данного метода
	unsigned int GetFoundRecordIndex(unsigned int uiFindId = 0);
	///удаляет запись таблицы по индексу
	void DeleteRecord(unsigned int uiRecordIndex);

	///инициализирует процесс сканирования таблицы
	HRESULT InitScan(long nStartRecordIdx, void* ArrayOfPointersToData[]);
	///возвращает очередную запись таблицы
	HRESULT Scan(void);
	///возвращает запись таблицы по индексу
	HRESULT ScanOne(unsigned int uiRecordIndex);

	///инициализирует процесса добавления данных в таблицу.
	HRESULT InitAdd(void* ArrayOfPointersToData[]);
	///добавляет запись к таблице
	HRESULT Add(void);

	///инициализирует процесса изменения всех полей таблицы.
	HRESULT InitModify(void* ArrayOfPointersToData[]);
	///находит и изменяет запись в таблице
	HRESULT FindOneAndChange(void);
	//-----------Eof-INdxTableStorage
  
protected:

	///интерфейс на таблицу DBMS
	SS::Interface::Core::DBMS::IDataTable* m_pDBMSTable;
	///интерфейс на базу DBMS, с ее помощью создается открывается и закрывается таблица
	SS::Interface::Core::DBMS::IDataBase* m_pDBMSBase;
	///коллекция полей таблицы
	TTableFieldCollection m_TTableFieldCollection;
	///коллекция названий полей таблицы
	vector<wchar_t*> m_TableFieldNamesCollection;

	///Последний просмотренный индекс таблицы функцией DBMS::Scan .
	unsigned long m_LastAccessIndex;
};

//--------------------------------------------------------------------//

#pragma warning( default : 4250 )

}
}
}
}

