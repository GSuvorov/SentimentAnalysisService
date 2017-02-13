#include "StdAfx.h"
#include ".\table_storage.h"
#include "console.h"
#include "const.h"
#include <iterator>


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

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;
using namespace SS::Interface::Core::NdxSE;
using namespace SS::Interface::Core::DBMS;

//--------------------------------------------------------------------//

CTableStorageDataField::CTableStorageDataField(wchar_t* pwszFieldName, 
					   EFieldDataTypes FieldType, EFieldModificatorTypes FieldModificator) :
	CNdxMetaField(pwszFieldName, FieldType, FieldModificator), 
	m_usFindProgramId(UC_EMPTY), m_usSpanFindProgramId(UC_EMPTY)
{
}

CTableStorageDataField::CTableStorageDataField(const CTableStorageDataField& DataField) :
	CNdxMetaField(DataField), m_usFindProgramId(0), m_usSpanFindProgramId(UC_EMPTY)
{
}

void CTableStorageDataField::View(void) const
{
	ToConsole(L"//------DataField");

	wstring szValue;
	ToString(szValue);
	ToConsole(szValue.c_str());

	ToConsole(L"//------Eof-DataField");
}

//--------------------------------------------------------------------//

CTableStorage::CTableStorage(INdxTableStorage* pNdxTableStorage)
:m_pNdxTableStorage(pNdxTableStorage), m_uiKeyFieldIndex(EMPTY_VALUE)
{
	if(ISNULL(m_pNdxTableStorage)) return;

	if(!m_pNdxTableStorage->IsOpen()){
		TO_CONSOLE(L"NdxTableStorage not opened");
		ToConsole(m_pNdxTableStorage->GetDataUnitName());
		return;
	}

	FillTextFieldCollection();
}

CTableStorage::~CTableStorage(void)
{
	ClearTextFieldCollection();
}

void CTableStorage::FillTextFieldCollection(void)
{
	if(ISNULL(m_pNdxTableStorage)) return;

	INdxTableStorage::TTableFieldCollection* pTableFieldCollection=m_pNdxTableStorage->GetFieldsCollection();

	if(ISNULL(pTableFieldCollection)) return;
	ClearTextFieldCollection();
	//формируем коллекцию полей таблицы

	for(INdxTableStorage::TTableFieldCollection::iterator 
		itFD=pTableFieldCollection->begin(); itFD!=pTableFieldCollection->end(); itFD++)
	{
		if(itFD->etFieldType==TYPE_FINAL) break;
		m_TextFieldCollection.push_back(CTableStorageDataField(itFD->szFieldName, (EFieldDataTypes)itFD->etFieldType, (EFieldModificatorTypes)itFD->etFieldModificator));
	}
	
	for(unsigned int i=0; i<m_TextFieldCollection.size(); i++)
	{
		m_AppendValuesCollection.push_back(m_TextFieldCollection[i].GetValue());
		if(m_TextFieldCollection[i].GetFieldModificator()==MOD_IS_UNIC){
			m_uiKeyFieldIndex=i;
			m_FindKeysValuesCollection.push_back(m_TextFieldCollection[i].GetValue());
			m_FindByKeysValuesCollection.push_back(0);	
		}else{
			m_FindKeysValuesCollection.push_back(0);
			m_FindByKeysValuesCollection.push_back(m_TextFieldCollection[i].GetValue());
		}
	}

	//инитим необходимы для поиска и добавления данные
	unsigned short usFindProgramID;
	unsigned short usSpanFindProgramID;
	for(TDataFieldCollection::iterator itDataField=m_TextFieldCollection.begin(); itDataField!=m_TextFieldCollection.end(); itDataField++)
	{
		if(itDataField->GetFieldModificator()&MOD_INDEXED){
			//готовим поиск получаем ID программы поиска, для извлечения полей по ключу
			m_pNdxTableStorage->SpanFindInit((wchar_t*)itDataField->GetFieldName(), 
				itDataField->GetLowerValue(), itDataField->GetUpperValue(), &m_SpanFindCollection, &usSpanFindProgramID);
		
			//устанавливаем программу поиска по данному полю
			itDataField->SetSpanFindProgramId(usSpanFindProgramID);
		}

		if(itDataField->GetFieldModificator()==MOD_IS_UNIC){
			//готовим поиск получаем ID программы поиска, для извлечения полей по ключу
			m_pNdxTableStorage->InitFind((wchar_t*)itDataField->GetFieldName(), 
				itDataField->GetValue(), &m_FindByKeysValuesCollection[0], &usFindProgramID);
		}else{
			//готовим поиск получаем ID программы поиска, для извлечения ключей
			m_pNdxTableStorage->InitFind((wchar_t*)itDataField->GetFieldName(), 
				itDataField->GetValue(), &m_FindKeysValuesCollection[0], &usFindProgramID);
		}

		//устанавливаем программу поиска по данному полю
		itDataField->SetFindProgramId(usFindProgramID);
	}
		
	//готовим добавление
	if(m_pNdxTableStorage->InitAdd(&m_AppendValuesCollection[0])!=S_OK) return;
}

void CTableStorage::Scan(void)
{
	TValuesCollection m_ScanValuesCollection;
	for(TDataFieldCollection::iterator itDataField=m_TextFieldCollection.begin(); itDataField!=m_TextFieldCollection.end(); itDataField++)
	{
		m_ScanValuesCollection.push_back(itDataField->GetValue());
	}
	
	//обход таблицы
	if(m_pNdxTableStorage->InitScan(0, &m_ScanValuesCollection[0])!=S_OK) return;
	while(m_pNdxTableStorage->Scan()==S_OK)
	{
		for(unsigned int i=0; i<m_TextFieldCollection.size(); i++)
		{
			m_TextFieldCollection[i].View();
		}
	}

}

void CTableStorage::Append(SS::Interface::Core::NdxSE::TNdxMetaFieldCollection* pDataRow)
{
	if(ISNULL(pDataRow)) return;
	if(m_AppendValuesCollection.size()!=pDataRow->size()){
		TO_CONSOLE(L"Not correct row format...");
		return;
	}

	for(unsigned int i=0; i<m_TextFieldCollection.size(); i++)
	{
		m_TextFieldCollection[i].SetValue(pDataRow->at(i).GetValue(), pDataRow->at(i).GetFieldType());
	}

	//присваиваем значения массив для добавления
	//for(unsigned int i=0; i<pDataRow->size(); i++)
	//{
	//	m_AppendValuesCollection[i]=pDataRow->at(i).GetValue();
	//}

	//добавляем запись в таблицу
	m_pNdxTableStorage->Add();
}

void CTableStorage::ExtractKeys(SS::Interface::Core::NdxSE::TNdxMetaFieldCollection* pFilterDataRow, TDataFieldExtracts* pResultExtracts)
{
	if(ISNULL(pFilterDataRow)) return;
	if(ISNULL(pResultExtracts)) return;

	if(m_TextFieldCollection.size()!=pFilterDataRow->size()){
		TO_CONSOLE(L"Not correct row format...");
		return;
	}

	///коллекция идентификаторов программ поиска
	vector<unsigned short> FindProgramIDCollection;
	vector<unsigned short> SpanFindProgramIDCollection;
	//присваиваем значения полям используемым при поиске
	//формируем программу поиска
	for(unsigned int i=0; i<pFilterDataRow->size(); i++)
	{
		if(pFilterDataRow->at(i).IsEnable()){
			if(m_TextFieldCollection[i].GetFieldType()==pFilterDataRow->at(i).GetFieldType()){
				if(pFilterDataRow->at(i).IsSingleValue()){
					//присваиваем значение соответствующему ключу
					m_TextFieldCollection[i].SetValue(pFilterDataRow->at(i).GetValue(), pFilterDataRow->at(i).GetFieldType());

					//добавляем советствующую программу поиска
					FindProgramIDCollection.push_back(m_TextFieldCollection[i].GetFindProgramId());
				}else{
					//присваиваем значение соответствующему ключу
					m_TextFieldCollection[i].SetLowerValue(
						pFilterDataRow->at(i).GetLowerValue(), pFilterDataRow->at(i).GetFieldType());
					m_TextFieldCollection[i].SetUpperValue(
						pFilterDataRow->at(i).GetUpperValue(), pFilterDataRow->at(i).GetFieldType());
					
					//добавляем советствующую программу поиска
					SpanFindProgramIDCollection.push_back(m_TextFieldCollection[i].GetSpanFindProgramId());
				}
			}else{
				TO_CONSOLE(L"Not correct field format...");
				ToConsole(pFilterDataRow->at(i).GetFieldName());
			}
		}
	}
	
	//выполняем поиск по заданным программам поиска с фильтрцией результатов поиска
	TRecordIdCollection RecordIdCollection;
	if(!FindProgramIDCollection.empty()){
		ExtractRecords(&FindProgramIDCollection, &RecordIdCollection);
		if(RecordIdCollection.empty()) return;
	}

	SpanExtractRecords(&SpanFindProgramIDCollection, &RecordIdCollection);
	if(RecordIdCollection.empty()) return;

	TValuesCollection m_ScanValuesCollection;
	for(TDataFieldCollection::iterator itDataField=m_TextFieldCollection.begin(); itDataField!=m_TextFieldCollection.end(); itDataField++)
	{
		if(itDataField->GetFieldModificator()==MOD_IS_UNIC){
			m_ScanValuesCollection.push_back(itDataField->GetValue());
		}else{
			m_ScanValuesCollection.push_back(0);
		}
	}
	
	//обход таблицы
	if(m_pNdxTableStorage->InitScan(0, &m_ScanValuesCollection[0])!=S_OK) return;

	for(TRecordIdCollection::iterator itRecordID=RecordIdCollection.begin(); itRecordID!=RecordIdCollection.end(); itRecordID++)
	{
		if(m_pNdxTableStorage->ScanOne(*itRecordID)==S_OK)
		{
			//m_TextFieldCollection[m_uiKeyFieldIndex].View();
			pResultExtracts->insert(CTableStorageDataField(m_TextFieldCollection[m_uiKeyFieldIndex]));
		}
	}
	
}

void CTableStorage::ExtractRecords(vector<unsigned short>* pFindProgramIDCollection, TRecordIdCollection* pRecordIdCollection)
{
	if(ISNULL(pFindProgramIDCollection)) return;
	if(ISNULL(pRecordIdCollection)) return;

	//выполняем поиск по заданным программам поиска с фильтрцией результатов поиска
	TRecordIdCollection FindRecordIdCollection;
	for(vector<unsigned short>::iterator itFP=pFindProgramIDCollection->begin(); itFP!=pFindProgramIDCollection->end(); itFP++)
	{
		//очищаем поисквую выборку
		FindRecordIdCollection.clear();
		//выполняем поиск, наполняем выборку результатами
		while(m_pNdxTableStorage->Find(*itFP)==S_OK)
		{
			FindRecordIdCollection.insert(m_pNdxTableStorage->GetFoundRecordIndex(*itFP));
		}

		//если поисквая выборка пустая, результат точно будет пустым
		if(FindRecordIdCollection.empty()){
			pRecordIdCollection->clear();		
			return;
		}

		if(itFP==pFindProgramIDCollection->begin()){
			//для первого поиска просто свапируем результаты
			pRecordIdCollection->swap(FindRecordIdCollection);
			//ViewDataFieldExtracts(pResultExtracts);
		}
		else
		{
			//ViewDataFieldExtracts(&FindExtracts);
			//ViewDataFieldExtracts(pResultExtracts);
			
			TRecordIdCollection TempRecordIdCollection;//(pRecordIdCollection->begin(), pRecordIdCollection->end());

			//для последующих поисков определяем пересечение 
			//результирующей выборки, с поисковой, обновляем результирующую (фильтруем)
			//TRecordIdCollection::iterator itBound=
			set_intersection(FindRecordIdCollection.begin(), FindRecordIdCollection.end(), 
				pRecordIdCollection->begin(), pRecordIdCollection->end(),
				std::inserter(TempRecordIdCollection, TempRecordIdCollection.begin()));
			//TempRecordIdCollection.erase(itBound, TempRecordIdCollection.end());
			pRecordIdCollection->swap(TempRecordIdCollection);

			//ViewDataFieldExtracts(pResultExtracts);
		}
	}
}

void CTableStorage::SpanExtractRecords(vector<unsigned short>* pFindProgramIDCollection, TRecordIdCollection* pRecordIdCollection)
{
	if(ISNULL(pFindProgramIDCollection)) return;
	if(ISNULL(pRecordIdCollection)) return;

	//выполняем поиск по заданным программам поиска с фильтрцией результатов поиска
	for(vector<unsigned short>::iterator itFP=pFindProgramIDCollection->begin(); itFP!=pFindProgramIDCollection->end(); itFP++)
	{
		//очищаем поисквую выборку
		m_SpanFindCollection.clear();
		//выполняем поиск, наполняем выборку результатами
		m_pNdxTableStorage->SpanFind(*itFP);

		//если поисквая выборка пустая, результат точно будет пустым
		if(m_SpanFindCollection.empty()){
			pRecordIdCollection->clear();		
			return;
		}

		if(itFP==pFindProgramIDCollection->begin()){
			//для первого поиска просто свапируем результаты
			pRecordIdCollection->swap(m_SpanFindCollection);
			//ViewDataFieldExtracts(pResultExtracts);
		}
		else
		{
			//ViewDataFieldExtracts(&FindExtracts);
			//ViewDataFieldExtracts(pResultExtracts);
			
			TRecordIdCollection TempRecordIdCollection;//(pRecordIdCollection->begin(), pRecordIdCollection->end());

			//для последующих поисков определяем пересечение 
			//результирующей выборки, с поисковой, обновляем результирующую (фильтруем)
			//TRecordIdCollection::iterator itBound=
			set_intersection(m_SpanFindCollection.begin(), m_SpanFindCollection.end(), 
				pRecordIdCollection->begin(), pRecordIdCollection->end(),
				std::inserter(TempRecordIdCollection, TempRecordIdCollection.begin()));
			//TempRecordIdCollection.erase(itBound, TempRecordIdCollection.end());
			pRecordIdCollection->swap(TempRecordIdCollection);

			//ViewDataFieldExtracts(pResultExtracts);
		}
	}
}

void CTableStorage::ExtractByKey(TNdxMetaFieldCollection* pResultDataRow)
{
	if(ISNULL(pResultDataRow)) return;

	if(m_TextFieldCollection.size()!=pResultDataRow->size()){
		TO_CONSOLE(L"Not correct row format...");
		return;
	}

	//присваиваем значение ключу
	m_TextFieldCollection[m_uiKeyFieldIndex].SetValue(
		pResultDataRow->at(m_uiKeyFieldIndex).GetValue(), pResultDataRow->at(m_uiKeyFieldIndex).GetFieldType());

	//выполняем поиск, наполняем выборку результатами
	if(m_pNdxTableStorage->FindOne(m_TextFieldCollection[m_uiKeyFieldIndex].GetFindProgramId())!=S_OK)
	{
		TO_CONSOLE(L"Find not successed...");
		return;
	}
	
	//формируем результат
	for(unsigned int i=0; i<m_TextFieldCollection.size(); i++)
	{
		//m_TextFieldCollection[i].View();
		if(pResultDataRow->at(i).IsEnable()){
			//присваиваем найденные значения полям
			pResultDataRow->at(i).SetValue(
				m_TextFieldCollection[i].GetValue(), m_TextFieldCollection[i].GetFieldType());
		}
	}
}

void CTableStorage::ViewDataFieldExtracts(TDataFieldExtracts* pDataFieldExtracts)
{
	if(ISNULL(pDataFieldExtracts)) return;
	ToConsole(L"//-----------DataFieldExtracts:");
	for(CTableStorage::TDataFieldExtracts::iterator itDF=pDataFieldExtracts->begin(); itDF!=pDataFieldExtracts->end(); itDF++)
	{
		(itDF)->View();
	}
	ToConsole(L"//-----------Eof-DataFieldExtracts");
}

unsigned int CTableStorage::GetDataFieldIndex(const wchar_t* wszFieldName)
{
	wstring wsFieldName(wszFieldName);
	wcslwr((wchar_t*)wsFieldName.c_str());
	wstring wsTemp;
	for(unsigned int i=0; i<m_TextFieldCollection.size(); i++)
	{
		wsTemp.assign(m_TextFieldCollection[i].GetFieldName());
		wcslwr((wchar_t*)wsTemp.c_str());
		if(!wcscmp(wsTemp.c_str(), wsFieldName.c_str())){
			return i;
		}
	}

	return EMPTY_VALUE;
}

void CTableStorage::DeleteByKey(SS::Interface::Core::NdxSE::CNdxMetaField* pKeyField)
{
	if(ISNULL(pKeyField)) return;
	
	if(GetDataFieldIndex(pKeyField->GetFieldName())==EMPTY_VALUE && pKeyField->GetFieldModificator()!=MOD_IS_UNIC){
		TO_CONSOLE(L"Not correct key field...");
		return;
	}

	//присваиваем значение ключу
	m_TextFieldCollection[m_uiKeyFieldIndex].SetValue(
		pKeyField->GetValue(), pKeyField->GetFieldType());

	//выполняем поиск, наполняем выборку результатами
	if(m_pNdxTableStorage->FindOne(m_TextFieldCollection[m_uiKeyFieldIndex].GetFindProgramId())!=S_OK)
	{
		TO_CONSOLE(L"Find not successed...");
		return;
	}

	//получаем индекс записи
	unsigned int uiRowIndex=m_pNdxTableStorage->GetFoundRecordIndex(m_TextFieldCollection[m_uiKeyFieldIndex].GetFindProgramId());
	
	//удаляем запись
	m_pNdxTableStorage->DeleteRecord(uiRowIndex);
}

//--------------------------------------------------------------------//

}
}
}
}
}