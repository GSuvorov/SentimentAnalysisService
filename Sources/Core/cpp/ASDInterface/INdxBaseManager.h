#pragma once
#include "..\ASCInterface\IBaseManager.h"
#include "INdxDataStorage.h"

//--------------------------------------------------------------------//

// {BE8CB1C6-4F37-46c6-AAAD-F5D771B9779C}
static const GUID IID_NdxBaseManager = 
{ 0xbe8cb1c6, 0x4f37, 0x46c6, { 0xaa, 0xad, 0xf5, 0xd7, 0x71, 0xb9, 0x77, 0x9c } };


//--------------------------------------------------------------------//

namespace SS
{
namespace Interface
{
namespace Core
{
namespace NdxSE
{

//--------------------------------------------------------------------//

///имена хранилищ обязательно входящих в состав базы проиндексированных документов
namespace ObligatoryStorageNames
{
	///хранилище информации о проиндексиованных текстах
	const wchar_t TextsInfoStorage[]=L"tinf";
	///хранилище путей к текстам в виде дерева путей
	const wchar_t TextsPathTreeStorage[]=L"tpinf";
	///хранилище полей текстов в виде таблицы DBMS
	const wchar_t TextsFieldsStorage[]=L"tfld";
}

///типы хранилищ входящих в состав индексной базы
enum ENdxStorageTypes
{
	///поисковый индекс текстов
	ensTextsIndex,
	///поисковый индекс текстов с хранением статистики
	ensTextsIndexWithStatistic,
	///поисковый индекс заголовков текстов
	ensHeadersIndex,
	///поисковый индекс авторов текстов
	ensAthorsIndex,
	///поисковый индекс дат текстов
	ensDataIndex,
	///хранилище бинарного отображения проиндексиованных текстов
	ensTextsBinary,
	///хранилище информации о проиндексиованных текстах
	ensTextsInfo,
	///хранилище путей к текстам в виде дерева путей
	ensTextsPathTree,
	///хранилище полей текстов в виде таблицы DBMS
	ensTextsFields,
};

//--------------------------------------------------------------------//

///типы конфигураций поисковых индексов
enum ENdxStorageConfig
{
	///неопределенная для хранилищ не являющихся индексами
	escUndefined,
	///индексирование по абс. номерам предложений
	escAbsSentence,
	///индексирование по абс. номерам предложений с подсчетом номеров слов 
	escAbsSentence_WC,
	///индексирование по номерам текстов с подсчетом номеров слов
	escText_WC,
	///индексирование по номерам текстов с подсчетом номеров слов, оптимизировано для больших текстов
	escText_WC_ForBig,
	///индексирование по номерам текстов с подсчетом номеров слов, оптимизировано для маленьких текстов
	escText_WC_ForSmall,
	///индексирование по номерам текстов
	escText,
	escText_WC_Huge,
};

//--------------------------------------------------------------------//

///интерфейс управления индексными базами индексного процессора NdxSE
class INdxBaseManager : public SS::Interface::Core::ResourceManagers::IBaseManager
{
public:
	///возвращает идентификатор текущей сессии индексной базы,
	///если у клиента не совпадает идентификатор сессии, необходимо обновить
	///свои структуры, так как все хранилищя уже были созданы заново
	virtual unsigned int GetCurrentSessionID(void) = 0;

	///сбрасывает структуру индексной базы
	virtual void ResetNdxBaseStructure(void) = 0;
	///добавляет хранилище к индексной базе
	virtual void AddNdxBaseUnit(const wchar_t* wszStorageName, ENdxStorageConfig StorageConfig) = 0;
	///возвращает хранилище индексной базы по имени
	virtual SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* 
		GetNdxBaseUnit(const wchar_t* wszStorageName) = 0;
	//для обхода по коллекции юнитов базы
	///возвращает первое в коллекции хранилище индексной базы
	virtual SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* 
		GetFirstNdxBaseUnit(void) = 0;
	///возвращает следующее за текущим в коллекции хранилище индексной базы, 
	//при достижении конца или без вызова GetFirstNdxBaseUnit вернет NULL
	virtual SS::Interface::Core::NdxSE::NdxDataStorage::INdxDataUnit* 
		GetNextNdxBaseUnit(void) = 0;
};

//--------------------------------------------------------------------//

}
}
}
}