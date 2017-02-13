#pragma once

#include "../ASDInterface/INdxDataStorage.h"

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

///таблица индексов и точек входа в хранилища уровней
class CIndexEntryPointTable
{
public:
   CIndexEntryPointTable(SS::Interface::Core::NdxSE::NdxDataStorage::INdxTableStorage* pNdxTableStorage);
   virtual ~CIndexEntryPointTable(void); 

	///возвращет смещение по заданной основе
	unsigned int GetIndexEntryPoint(unsigned int uiIndex);
	///добавляет смещение основы в базу данных, df - по умолчанию заностится равным 0.
	bool SetIndexEntryPoint(unsigned int uiIndex, unsigned int uiEntryPoint);
	///добавляет смещение основы в базу данных и  df 
	bool SetIndexEntryPoint(unsigned int uiIndex, unsigned int uiEntryPoint, unsigned int uiIndexDF);
	///открывает базу параметр - не используется 
	bool Open(string& szPath);
	///изменяет запись в БД, изменяя значение df. 
	bool ModifyIndexRecordDF(unsigned int uiIndex, unsigned int uiDF);
	///изменение данных по заданной основе. Индекс основы не меняется.
	bool ModifyIndexDataRecord(unsigned int uiIndex, unsigned int uiEntryPoint, unsigned int uiDF);
	///возвращает частоту основы в документах
	unsigned int GetIndexDF(unsigned int uiIndex);
	///возвращает по индексу основы точку входа в хранилище уровней и частоту основы в документах
	bool GetIndexData(unsigned int uiIndex, unsigned int* puiOffset, unsigned int* puiDF);
	///инициализация чтения таблицы
	bool InitScanOffsetDF();
	///переход к следющей основе
	bool MoveNext(unsigned int* puiSourceIndex, unsigned int* puiDF ) ; 

private:
	///инициализация процесса добавления записей в БД
	bool InitAdd(void);
	///инициализация поиска по индексу основы
	bool InitFind(void* pvOffset, void* pvDF);
	///инициализация изменения полей записи.
	bool InitModify(void* pvOffset, void* pvDF);

	///индекс 
	unsigned int m_uiIndex;
	///точка входа в координатное дерево
	unsigned int m_uiEntryPoint;
	///массив указателей для работы с DBMS. Добавление.
	void* m_pToDataAdd[3];
	///массив указателей для получения даных
	void* m_pToDataGet[3];
	///число докумнтов, в которых встречается основа.
	unsigned int m_uiDF;
	///массив указателей для изменения полей записи.
	void* m_pToDataModify[3];
	///массив указателей для сканирования полей записи.
	void* m_pToScanGet[3];
	
	///физическое табличное хранилище
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxTableStorage* m_pNdxTableStorage;
};

//--------------------------------------------------------------------//

}
}
}
}
}