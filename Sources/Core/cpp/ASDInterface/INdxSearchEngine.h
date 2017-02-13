#pragma once
#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASCInterface\TCoommonTypes.h"
#include "..\ASCInterface\ITextsInfoExtracting.h"
#include "..\ASDInterface\INdxQuery.h"
#include ".\TNdxSearchEngineTypes.h"
#include "IISContainer.h"
#include <vector>

//--------------------------------------------------------------------//

// {AA99ECEA-3DB7-4e86-AB00-E9BB9849E973}
static const GUID IID_NdxSearchEngine = 
{ 0xaa99ecea, 0x3db7, 0x4e86, { 0xab, 0x0, 0xe9, 0xbb, 0x98, 0x49, 0xe9, 0x73 } };

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

///интерфейс для работы с поисковым движком
class INdxSearchEngineFind
{
public:
	//----------------------------------------------
	//Поиск
	//----------------------------------------------
	///запуск сессии поиска, получает контайнер результатов поиска 
	//где по окончании поисковой сессии будут лежать результаты
	virtual HRESULT StartFindSession(
		///контайнер результатов поиска 
		SS::Interface::Core::IISContainerUM* pISContainer) = 0;

	///останов сессии поиска, записывает в контайнер поданный в StartFindSession результаты поиска 
	virtual HRESULT EndFindSession(void) = 0;

	///	Поиск в индексном хранилище по оптимизированному запросу
	virtual HRESULT Find(
		///имя индексного хранилища, где выполнять поиск
		const wchar_t* wszIndexStorageName,
		///контейнер оптимизированного поискового запроса
		SS::Interface::Core::NdxSE::IParametrizedQuery*	pPrmQueryResult,
		///общие настройки поиска
		SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams, 
		///настройки поиска поискового движка
		CNdxSearchSettings* pSearchSettings) = 0;

	///поиск в таблице мета полей по фильтру
	virtual HRESULT Find(
		///коллекция мета-полей документа (фильтр)
		TNdxMetaFieldCollection* pNdxMetaFieldCollection,
		///общие настройки поиска
		SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams, 
		///настройки поиска поискового движка
		CNdxSearchSettings* pSearchSettings) = 0;

	///останов сессии поиска, записывает в контайнер поданный в StartFindSession результаты поиска 
	///для формирования результатов поиска ТОЛЬКО по полям точного значения
	virtual HRESULT EndFindByFieldSession(void) = 0;

	//----------------------------------------------

};

//--------------------------------------------------------------------//

///интерфейс для работы с поисковым движком
class INdxSearchEngineIndexation
{
public:
	//----------------------------------------------
	//Индексация
	//----------------------------------------------
	///запуск сессии индексации коллекции документов
	virtual HRESULT StartIndexationSession(void) = 0;

	///останов сессии индексации коллекции документов
	virtual HRESULT EndIndexationSession(void) = 0;

	///запуск пакетной индексации документа
	virtual HRESULT StartDocumentIndexation(
		///контайнер индексов для индексации
		SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult) = 0;

	///индексация очередного куска документа
	virtual HRESULT IndexateDocumentChunk(
		///контайнер индексов для индексации
		SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult) = 0;

	///индексация лингвистической текстовой информация !!! вызывается один раз для документа
	virtual HRESULT IndexateTextFeature(
		///контайнер индексов для индексации
		SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult) = 0;

	///останов пакетной индексации документа
	virtual HRESULT EndDocumentIndexation(
		///коллекция мета-полей документа
		TNdxMetaFieldCollection* pNdxMetaFieldCollection,
		///контайнер индексов для индексации
		SS::Interface::Core::CommonContainers::IIndexationResult* pIndexationResult) = 0;
	//----------------------------------------------
};

//--------------------------------------------------------------------//

///работа с информацией по текстам
class INdxSearchEngineTextsInfo
{
public:				
	///возвращает количество текстов в базе
	virtual unsigned int GetTextsNumber(void) = 0;
	///возвращает путь текста по индексу
	virtual void GetTextPath(unsigned int uiTextIndex, std::wstring* pPath) = 0;
	///возвращает информацию по тексту
	virtual void GetTextInfo(unsigned int uiTextIndex, SS::Interface::Core::CommonContainers::ITextFeature* pTextFeature) = 0;
	///возвращает информацию по тексту
	virtual void GetTextInfo(
		///индекс текста
		unsigned int uiTextIndex, 
		///извлекаемое поле, у поля должен быть соблюдено название и тип поля
		CNdxMetaField* pNdxMetaField) = 0;
	///возвращает информацию по тексту
	virtual void GetTextInfo(
		///индекс текста
		unsigned int uiTextIndex, 
		///коллекция извлекаемых полей, у полей должны быть соблюдены названия и типы поля
		TNdxMetaFieldCollection* pNdxMetaFieldCollection) = 0;
	///возвращает размер текста по индексу
	virtual unsigned int GetTextSize(unsigned int uiTextIndex) = 0;
	///удаляет текст из базы
	virtual void DeleteText(unsigned int uiTextIndex) = 0;
	///проверяет удален ли текст из базы
	virtual unsigned int IsTextDeleted(unsigned int uiTextIndex) = 0;
	///заполняет структуру заголовков по индексу текста
	virtual void GetHeadersStructure(unsigned int uiTextIndex, std::wstring* pwsHeaderStructure) = 0;
	///заполняет структуру таблиц по индексу текста
	virtual void GetTablesStructure(unsigned int uiTextIndex, std::wstring* pwsTablesStructure) = 0;
};

//--------------------------------------------------------------------//

//работа с путями текстов
class INdxSearchEngineTextsPathsInfo
{
public:				
	//заполняет в pTextPathChunk по смещению узла информацию по узлу
	virtual void FillTextPathChunk(
		///узел дерева пути
		STextPathChunk* pTextPathChunk) = 0;
	//заполняет по pTextPathChunk смещения детей узла pChildsOffsetsCollection
	virtual void GetTextPathChunkChildsOffsets(
		STextPathChunk* pTextPathChunk, 
		std::vector<unsigned int>* pChildsOffsetsCollection) = 0;
	///заполняет по pTextPathChunk смещения родителя узла pParentOffset
	virtual void GetTextPathChunkParentOffset(
		STextPathChunk* pTextPathChunk, 
		unsigned int* pParentOffset) = 0;
	///удаляет узел вместе с детьми
	virtual void DeleteTextPathChunk(
		///узел дерева пути
		STextPathChunk* pTextPathChunk) = 0;
	///проверяет есть ли путь в дереве путей текстов
	virtual unsigned int IsTextPathExist(const wchar_t* pwcszTextPath) = 0;
	///удаляет текст по пути
	virtual void DeleteTextByPath(const wchar_t* pwcszTextPath) = 0;
};

//--------------------------------------------------------------------//

//работа с путями текстов
class INdxSearchEngine : public virtual SS::Interface::IBase
{
public:				
	///возвращает интерфейс для выполнения поиска
	virtual INdxSearchEngineFind* INdxSearchEngineFind(void) = 0;
	///возвращает интерфейс для выполнения индексации
	virtual INdxSearchEngineIndexation* INdxSearchEngineIndexation(void) = 0;
	///возвращает интерфейс для получения информации по проиндексированным текстам
	virtual INdxSearchEngineTextsInfo* INdxSearchEngineTextsInfo(void) = 0;
	///возвращает интерфейс для получения информации по путям проиндексированных текстов
	virtual INdxSearchEngineTextsPathsInfo* INdxSearchEngineTextsPathsInfo(void) = 0;
};

}
}
}
}