#pragma once

#include "../ASDInterface/TNdxSearchEngineTypes.h"
#include "../ASDInterface/INdxDataStorage.h"
#include ".\basis_storage.h"
#include ".\consoleclient.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Storages
{

//--------------------------------------------------------------------//

///поле таблицы текста
class CTableStorageDataField : public SS::Interface::Core::NdxSE::CNdxMetaField
{
public:
	CTableStorageDataField(wchar_t* pwszFieldName, 
		SS::Interface::Core::DBMS::EFieldDataTypes FieldType,
		SS::Interface::Core::DBMS::EFieldModificatorTypes FieldModificator);
	CTableStorageDataField(const CTableStorageDataField& DataField);
	
	///устанавливает идентификатор программы поиска по данному полю
	void SetFindProgramId(unsigned short usFindProgramId){m_usFindProgramId=usFindProgramId;};
	///возвращает идентификатор программы поиска по данному полю
	unsigned short GetFindProgramId(void){return m_usFindProgramId;};
	///устанавливает идентификатор программы интервального поиска по данному полю
	void SetSpanFindProgramId(unsigned short usSpanFindProgramId){m_usSpanFindProgramId=usSpanFindProgramId;};
	///возвращает идентификатор программы интервального поиска по данному полю
	unsigned short GetSpanFindProgramId(void){return m_usSpanFindProgramId;};

	///отображает содержимое объекта класса
	void View(void) const;

	///оператор для сравнения и сортировки
	bool operator<(const CTableStorageDataField& RightDF) const {return m_SingleValue<RightDF.m_SingleValue;};

protected:
	///идентификатор программы поиска по данному полю
	unsigned short m_usFindProgramId;
	///идентификатор программы интервального поиска по данному полю
	unsigned short m_usSpanFindProgramId;
};

//--------------------------------------------------------------------//

///табличное хранилище
///ориентируясь на обязательное ключевое поле таблицы может:
///1. извлекать поля данных по ключевому полю
///2. извелкать ключевые поля по фильтру из данных других полей
class CTableStorage : public CBasisStorage
{
public:
	///выборка полей таблицы
	typedef set<CTableStorageDataField> TDataFieldExtracts;
	///коллекция указателей на значения полей таблицы
	typedef vector<void*> TValuesCollection;
	///коллекция указателей на значения полей таблицы
	typedef set<unsigned int> TRecordIdCollection;
	///коллекция указателей на значения полей таблицы
	typedef vector<CTableStorageDataField> TDataFieldCollection;

	CTableStorage(SS::Interface::Core::NdxSE::NdxDataStorage::INdxTableStorage* pNdxTableStorage);
	virtual ~CTableStorage(void); 
	
	///добавляет запись в таблицу
	void Append(SS::Interface::Core::NdxSE::TNdxMetaFieldCollection* pDataRow);
	///извлекает ключевые поля таблицы по фильтру
	void ExtractKeys(SS::Interface::Core::NdxSE::TNdxMetaFieldCollection* pFilterDataRow, TDataFieldExtracts* pResultExtracts);
	///извлекает поля таблицы по ключу
	void ExtractByKey(SS::Interface::Core::NdxSE::TNdxMetaFieldCollection* pResultDataRow);
	///отобрадает коллекцию
	void ViewDataFieldExtracts(TDataFieldExtracts* pDataFieldExtracts);
	///перебирает записи таблицы
	void Scan(void);
	///вовзвращает индекс поля в таблице
	unsigned int GetDataFieldIndex(const wchar_t* wszFieldName);
	///удаляет строку таблицы по ключу
	void DeleteByKey(SS::Interface::Core::NdxSE::CNdxMetaField* pKeyField);

protected:
	///заполняет коллекцию полей таблицы
	void FillTextFieldCollection(void);
	///очищает коллекцию полей таблицы
	void ClearTextFieldCollection(void){m_TextFieldCollection.clear();};
	///извлекает ключевые поля таблицы
	void ExtractRecords(vector<unsigned short>* pFindProgramIDCollection, TRecordIdCollection* pRecordIdCollection);
	///извлекает ключевые поля таблицы
	void SpanExtractRecords(vector<unsigned short>* pFindProgramIDCollection, TRecordIdCollection* pRecordIdCollection);

	///коллекция указателей на значения полей таблицы для добавления
	TValuesCollection m_AppendValuesCollection;
	///коллекция указателей на значения полей таблицы для поиска ключей
	TValuesCollection m_FindKeysValuesCollection;
	///коллекция указателей на значения полей таблицы для поиска по ключу
	TValuesCollection m_FindByKeysValuesCollection;
	///коллекция идентификаторв записей таблицы для поиска по интервалу значений поля
	TRecordIdCollection m_SpanFindCollection;
	///коллекция полей таблицы
	TDataFieldCollection m_TextFieldCollection;
	///номер поля данных, то которое нужно при поиске
	unsigned int m_uiKeyFieldIndex;

	///физическое табличное хранилище
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxTableStorage* m_pNdxTableStorage;
};

//--------------------------------------------------------------------//

}
}
}
}
}
