#pragma once

#include "..\ASDInterface\INdxBaseManager.h"
#include ".\index_storage.h"
#include ".\index_storage_hdr.h"
#include ".\index_storage_tbl.h"
#include ".\index_stat_storage.h"
#include ".\texts_info_storage.h"
#include ".\texts_binary_storage.h"
#include ".\texts_path_storage.h"
#include ".\texts_field_storage.h"

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

///управление логическими хранилищами текущей индексной базы
class CStorageManager
{
public:
	typedef map<wstring, CBasisStorage*> TStorages;

	CStorageManager(void);
	virtual ~CStorageManager(void);
	
	///инициализирует менеджер хранилищ
	void Init(SS::Interface::Core::NdxSE::INdxBaseManager* pNdxBaseManager);
	///переключает хранилища в режим индексации
	void SetIndexingMode(void);
	///проверяет на режим индексации
	bool IsIndexingMode(void);
	///переключает хранилища в режим извлечения-поиска
	void SetExtractingMode(void);
	///проверяет на режим индексации
	bool IsExtractingMode(void);
	
	///осуществляет подготовку хранилища индексацию
	void PrepareIndexing(void);
	///выполняет действия хранилища по окончании индексации
	void CompleteIndexing(void);
	///выполняет действия хранилища по окончании индексации документа
	void CompleteDocumentIndexing(void);

	///возвращает индексное хранилище индексов текстов
	CIndexStorage* GetIndexStorage(const wchar_t* wszIndexStorageName){return static_cast<CIndexStorage*>(GetStorage(wszIndexStorageName));};
	///возвращает хранилище информации о проиндексиованных текстах
	CTextsInfoStorage* GetTextsInfoStorage(void){return static_cast<CTextsInfoStorage*>(GetStorage(SS::Interface::Core::NdxSE::ObligatoryStorageNames::TextsInfoStorage));};
	///возвращает хранилище путей к текстам в виде дерева путей
	CTextsPathsTreeStorage* GetTextsPathsTreeStorage(void){return static_cast<CTextsPathsTreeStorage*>(GetStorage(SS::Interface::Core::NdxSE::ObligatoryStorageNames::TextsPathTreeStorage));};
	///возвращает хранилище полей текстов в виде таблицы DBMS
	CTextsFieldsStorage* GetTextsFieldsStorage(void){return static_cast<CTextsFieldsStorage*>(GetStorage(SS::Interface::Core::NdxSE::ObligatoryStorageNames::TextsFieldsStorage));};

	///обновляет хранилища по менеджеру базы
	bool Update(void);

protected:
	///добавляет хранилище менеджера
	void AddStorage(const wchar_t* wszStorageName, CBasisStorage* pStorage);
	///возвращает хранилище менеджера
	CBasisStorage* GetStorage(const wchar_t* wszStorageName);
	///удаляет хранилища менеджера
	void DeleteStorages(void);

	///коллекция хранилищ менеджера
	TStorages m_Storages;

	///интерфейс на менеджер индексной базы
	SS::Interface::Core::NdxSE::INdxBaseManager* m_pNdxBaseManager;
	///идентификатор текущей сессии индексной базы
	unsigned int m_uiCurrentNdxBaseSessionID;

};

//--------------------------------------------------------------------//

}
}
}
}
}