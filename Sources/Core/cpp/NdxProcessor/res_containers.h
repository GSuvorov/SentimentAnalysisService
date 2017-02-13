#pragma once

#include ".\const.h"
#include ".\consoleclient.h"
#include ".\int_containers.h"
#include <functional>

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

//--------------------------------------------------------------------//

///контайнер результата поиска
class CFindResult : public CConsoleClient
{
public:
	///коллекции инервалов разбитые по хранилищам, в которых они были найдены
	typedef map<wstring, TIntervalCollection*> TIntervalByStorageCollection;
	///коллекция идентификаторов заголовков
	typedef vector<unsigned int> THIDCollection;
	///коллекция коллекций идентификаторов заголовков по индексам
	typedef map<unsigned int, THIDCollection*> TBlockHIDCollection;
	
	CFindResult(unsigned int uiTextIndex, unsigned int uiRelevance);
	virtual ~CFindResult(void);
	
	///возвращает идентификатор текста результата
	unsigned int GetTextIndex(void){return m_uiTextIndex;};
	///возвращает значение релевантности результата
	unsigned int GetRelevance(void){return m_uiRelevance;};
	///устанавливает значение релевантности результата
	void SetRelevance(unsigned int uiValue){m_uiRelevance=uiValue;};
	///добавляет коллекцию интервало в результат
	void MoveIntervalCollection(const wchar_t* wszIndexStorageName, TIntervalCollection* pIntervalCollection);
	///возвращет коллекции инервалов разбитые по хранилищам
	TIntervalByStorageCollection* GetIntervalCollection(void){return m_pIntervalByStorageCollection;};
	///возвращет коллекцию идентификаторов заголовков по индексу
	THIDCollection* GetHIDCollection(unsigned int uiIndex);
	///перебрасывает коллекции идентификаторов заголовков в результат
	void MoveBlockHIDCollection(TBlockHIDCollection* pBlockHIDCollection);

	///сериализация в строку
	void ToString(wstring& wszValue);
	///отображает содержимое объекта класса
	void View(void);

protected:
	///удаляет коллекцию инервалов разбитые по хранилищам, в которых они были найдены
	void DeleteIntervalByStorageCollection(void);
	void DeleteBlockHIDCollection(void);

	///идентификатор текста результата
	const unsigned int m_uiTextIndex;
	///значение релевантности результата
	unsigned int m_uiRelevance;
	///коллекции инервалов разбитые по хранилищам, в которых они были найдены
	TIntervalByStorageCollection* m_pIntervalByStorageCollection;
	///коллекции идентификаторов заголовков по блокам, доступ по индексу блока
	TBlockHIDCollection* m_pBlockHIDCollection;
};


//--------------------------------------------------------------------//

///контайнер-коллекция контайнеров результата поиска упорядоченных по релевантности
class CFindResultsByRelevanceCollection : public CConsoleClient
{
public:
	///коллекция контайнеров результата поиска упорядоченные по релевантности
	typedef list<CFindResult*> TFindResultsList;
	typedef map<unsigned int, TFindResultsList*, greater<int> > TFindResultsByRelevanceCollection;

	CFindResultsByRelevanceCollection(void);
	virtual ~CFindResultsByRelevanceCollection(void);

	///добавляет результат в коллекцию
	void AddFindResult(CFindResult* pFindResult);
	///удаляет результат из коллекции
	void RemoveFindResult(CFindResult* pFindResult);
	///очищает коллекицю
	void Clear(void);

	//обход по результатам в порядке убывания релевантности
	///позиционирование на первом результате
	bool MoveFirstFindResult(void);
	///позиционирование на очередном результате
	bool MoveNextFindResult(void);
	///возвращает текущий результат
	CFindResult* GetCurrentFindResult(void);

protected:	
	///коллекция контайнеров результата поиска упорядоченные по релевантности
	TFindResultsByRelevanceCollection m_FindResultsByRelevanceCollection;
	TFindResultsByRelevanceCollection::iterator m_itCurrFindResultsList;
	TFindResultsList::iterator m_itCurrFindResult;
	///коллекции для инициализации итераторов
	TFindResultsByRelevanceCollection m_NullCollection;
	TFindResultsList m_NullCollectionList;
};

//--------------------------------------------------------------------//

///контайнер-коллекция контайнеров результата поиска
class CFindResultCollection : public CConsoleClient
{
public:
	///коллекция контайнеров результата поиска
	typedef map<unsigned int, CFindResult*> TFindResultsCollection;
	
	///режимы добавления результатов в хранилище
	enum EAddingMode
	{
		///неопределен
		eamUndefined,
		///добавление только если результат уже есть
		eamIfExist,
		///добавление всегда
		eamAlways,
	};

	CFindResultCollection(void);
	virtual ~CFindResultCollection(void);
	
	///запускает сессию добавления результатов поиска в определнном режиме
	void StartFindResultAdding(EAddingMode AddingMode);
	///заврешает сессию добавления результатов поиска в определнном режиме
	void StopFindResultAdding(void);
	///добавляет коллекцию интервало в результат
	CFindResult* AddFindResult(unsigned int uiTextIndex, unsigned int uiRelevance);
	///возвращает результат по номеру текста
	CFindResult* GetFindResult(unsigned int uiTextIndex);
	///возвращет коллекции инервалов разбитые по хранилищам
	TFindResultsCollection* GetFindResultsCollection(void){return &m_FindResultsCollection;};
	///очищает коллекцию контайнеров результата поиска
	void Clear(void);
	///сбрасывает сессии по добавлению
	void ResetAddingSession(void){m_uiCurrentAddingSession=0;};
	///возвращет количество контайнеров результата поиска
	unsigned int Count(void){return (unsigned int)m_FindResultsCollection.size();};
	///проверяет на пустоту
	bool IsEmpty(void){return m_FindResultsCollection.empty();};
	///контайнер-коллекция контайнеров результата поиска упорядоченных по релевантности
	CFindResultsByRelevanceCollection* GetFindResultsByRelevanceCollection(void){return &m_FindResultsByRelevanceCollection;};

	///сериализация в строку
	void ToString(wstring& wszValue);
	///отображает содержимое объекта класса
	void View(void);

protected:
	///очищает коллекцию контайнеров результата поиска
	void Clear(TFindResultsCollection* pFindResultsCollection);
	///устанавливает текущий режим добавления результатов в хранилище
	void SetAddingMode(EAddingMode AddingMode){m_CurrentAddingMode=AddingMode;};

	///коллекция контайнеров результата поиска
	TFindResultsCollection m_FindResultsCollection;
	///вспомогательная коллекция контайнеров результата поиска
	TFindResultsCollection m_TempFindResultsCollection;
	///текущий режим добавления результатов в хранилище
	EAddingMode m_CurrentAddingMode;
	///текущач сессия
	unsigned int m_uiCurrentAddingSession;
	///контайнер-коллекция контайнеров результата поиска упорядоченных по релевантности
	CFindResultsByRelevanceCollection m_FindResultsByRelevanceCollection;
};

//--------------------------------------------------------------------//

}
}
}
}
}
