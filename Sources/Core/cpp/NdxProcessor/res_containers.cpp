#include "StdAfx.h"
#include ".\res_containers.h"

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

CFindResult::CFindResult(unsigned int uiTextIndex, unsigned int uiRelevance)
:m_uiTextIndex(uiTextIndex), m_uiRelevance(uiRelevance), 
m_pIntervalByStorageCollection(NULL), m_pBlockHIDCollection(NULL)
{
}

CFindResult::~CFindResult(void)
{
	DeleteIntervalByStorageCollection();
	DeleteBlockHIDCollection();

}

void CFindResult::DeleteBlockHIDCollection(void)
{
	if(m_pBlockHIDCollection){
		TBlockHIDCollection::iterator itHIDCollection=m_pBlockHIDCollection->begin();
		while(itHIDCollection!=m_pBlockHIDCollection->end()){
			if(itHIDCollection->second)
				delete itHIDCollection->second;
			itHIDCollection++;
		}
		delete m_pBlockHIDCollection;
		m_pBlockHIDCollection=NULL;
	}
}

void CFindResult::DeleteIntervalByStorageCollection(void)
{
	if(!m_pIntervalByStorageCollection) return;

	for(TIntervalByStorageCollection::iterator itInt=m_pIntervalByStorageCollection->begin(); itInt!=m_pIntervalByStorageCollection->end(); itInt++){
		if(itInt->second){
			for(unsigned int i=0; i<itInt->second->size(); i++){
				if(itInt->second->at(i)) delete itInt->second->at(i);
			}
			delete itInt->second;
		}
	}
	delete m_pIntervalByStorageCollection;
	m_pIntervalByStorageCollection=NULL;
}

void CFindResult::MoveIntervalCollection(const wchar_t* wszIndexStorageName, TIntervalCollection* pIntervalCollection)
{
	if(ISNULL(pIntervalCollection)) return; 
	if(pIntervalCollection->empty()) return; 

	if(!m_pIntervalByStorageCollection){
		m_pIntervalByStorageCollection=new TIntervalByStorageCollection();
	}
	
	TIntervalByStorageCollection::iterator itFind=m_pIntervalByStorageCollection->find(wszIndexStorageName);
	if(itFind==m_pIntervalByStorageCollection->end()){
		itFind=m_pIntervalByStorageCollection->insert(
			TIntervalByStorageCollection::value_type(wszIndexStorageName, new TIntervalCollection)).first;
	}
	
	//переносим интервалы и входной коллекции в свою, обнуляя указатели в исходной
	for(unsigned int i=0; i<pIntervalCollection->size(); i++){
		itFind->second->push_back((*pIntervalCollection)[i]);
		(*pIntervalCollection)[i]=NULL;
	}
}

CFindResult::THIDCollection* CFindResult::GetHIDCollection(unsigned int uiIndex)
{
	TBlockHIDCollection::iterator itHIDCollection=m_pBlockHIDCollection->find(uiIndex);
	if(itHIDCollection!=m_pBlockHIDCollection->end()){
		return itHIDCollection->second;
	}else{
		return NULL;
	}
}

void CFindResult::MoveBlockHIDCollection(TBlockHIDCollection* pBlockHIDCollection)
{
	if(ISNULL(pBlockHIDCollection)) return;

	DeleteBlockHIDCollection();

	if(!m_pBlockHIDCollection)
		m_pBlockHIDCollection=new TBlockHIDCollection();
	
	//переносим идентификаторы блоков из одной коллекции в другую
	m_pBlockHIDCollection->swap(*pBlockHIDCollection);
}

void CFindResult::ToString(wstring& wszValue)
{
	wchar_t buf[100];
	wsprintf(buf, L"TextId: %u Relevance: %u\n", m_uiTextIndex, m_uiRelevance);
	wszValue.assign(buf);
}

void CFindResult::View(void)
{
	ToConsole(L"//-------CFindResult");
	wstring wsValue;
	ToString(wsValue);
	ToConsole(wsValue.c_str());
	ToConsole(L"//-----Eof-CFindResult");
}

//--------------------------------------------------------------------//

CFindResultsByRelevanceCollection::CFindResultsByRelevanceCollection(void)
{
	m_itCurrFindResultsList = m_NullCollection.end();
	m_itCurrFindResult = m_NullCollectionList.end();
}

CFindResultsByRelevanceCollection::~CFindResultsByRelevanceCollection(void)
{
	Clear();
}

void CFindResultsByRelevanceCollection::Clear(void)
{

	for(TFindResultsByRelevanceCollection::iterator itFindResultList=m_FindResultsByRelevanceCollection.begin(); 
		itFindResultList!=m_FindResultsByRelevanceCollection.end(); itFindResultList++){
		//ToConsole(L"itFindResultList", itFindResultList->first);
		if(itFindResultList->second){
			delete itFindResultList->second;
		}
	}

	m_FindResultsByRelevanceCollection.clear();
}

void CFindResultsByRelevanceCollection::AddFindResult(CFindResult* pFindResult)
{
	if(ISNULL(pFindResult)) return;

	TFindResultsByRelevanceCollection::iterator itFindResultList=m_FindResultsByRelevanceCollection.find(pFindResult->GetRelevance());
	if(itFindResultList==m_FindResultsByRelevanceCollection.end()){
		itFindResultList=m_FindResultsByRelevanceCollection.insert(
			TFindResultsByRelevanceCollection::value_type(pFindResult->GetRelevance(), new TFindResultsList())).first;
	}

	itFindResultList->second->push_back(pFindResult);
}

void CFindResultsByRelevanceCollection::RemoveFindResult(CFindResult* pFindResult)
{
	if(ISNULL(pFindResult)) return;

	//ToConsole(L"CFindResultsByRelevanceCollection::RemoveFindResult");

	TFindResultsByRelevanceCollection::iterator itFindResultList=m_FindResultsByRelevanceCollection.find(pFindResult->GetRelevance());
	if(itFindResultList!=m_FindResultsByRelevanceCollection.end()){
		if(itFindResultList->second){
			for(TFindResultsList::iterator itFindResult=itFindResultList->second->begin(); itFindResult!=itFindResultList->second->end(); itFindResult++){
				if(*itFindResult==pFindResult){
					//нашли результат удаляем и уходим
					itFindResultList->second->erase(itFindResult);
					break;
				}
			}
			
			if(itFindResultList->second->empty()){
				delete itFindResultList->second;
				m_FindResultsByRelevanceCollection.erase(itFindResultList);
			}
		}
	}
}

bool CFindResultsByRelevanceCollection::MoveFirstFindResult(void)
{
	if(m_FindResultsByRelevanceCollection.empty()){
		m_itCurrFindResultsList=m_NullCollection.end();
		m_itCurrFindResult=m_NullCollectionList.end();
		return false;
	}else{
		m_itCurrFindResultsList=m_FindResultsByRelevanceCollection.begin();

		if(ISNULL(m_itCurrFindResultsList->second)){ 
			m_itCurrFindResultsList=m_NullCollection.end();
			m_itCurrFindResult=m_NullCollectionList.end();
			return false;
		}

		if(m_itCurrFindResultsList->second->empty()){
			while(m_itCurrFindResultsList->second->empty()){ 
				if(m_itCurrFindResultsList==m_FindResultsByRelevanceCollection.end()){
					m_itCurrFindResultsList=m_NullCollection.end();
					m_itCurrFindResult=m_NullCollectionList.end();
					return false;
				}
				m_itCurrFindResultsList++;
			}
		}

		m_itCurrFindResult=m_itCurrFindResultsList->second->begin();
		return true;
	}
}

bool CFindResultsByRelevanceCollection::MoveNextFindResult(void)
{
	if(m_itCurrFindResultsList==m_NullCollection.end() || 
	   m_itCurrFindResult==m_NullCollectionList.end()){
		TO_CONSOLE(L"Collection empty or MoveFirst was not called!!!");
		return false;
	}

	if(m_itCurrFindResult!=(--m_itCurrFindResultsList->second->end())){
		m_itCurrFindResult++;
		return true;
	}else{
		if(m_itCurrFindResultsList==(--m_FindResultsByRelevanceCollection.end())){
			m_itCurrFindResultsList=m_NullCollection.end();
			m_itCurrFindResult=m_NullCollectionList.end();
			return false;
		}else{
			m_itCurrFindResultsList++;

			if(ISNULL(m_itCurrFindResultsList->second)){ 
				m_itCurrFindResultsList=m_NullCollection.end();
				m_itCurrFindResult=m_NullCollectionList.end();
				return false;
			}

			if(m_itCurrFindResultsList->second->empty()){
				while(m_itCurrFindResultsList->second->empty()){ 
					m_itCurrFindResultsList++;

					if(m_itCurrFindResultsList==m_FindResultsByRelevanceCollection.end()){
						m_itCurrFindResultsList=m_NullCollection.end();
						m_itCurrFindResult=m_NullCollectionList.end();
						return false;
					}
				}
			}

			m_itCurrFindResult=m_itCurrFindResultsList->second->begin();
		}

		return true;
	}
}

CFindResult* CFindResultsByRelevanceCollection::GetCurrentFindResult(void)
{
	return m_itCurrFindResult==m_NullCollectionList.end()?NULL:(*m_itCurrFindResult);
}

//--------------------------------------------------------------------//

CFindResultCollection::CFindResultCollection(void)
:m_CurrentAddingMode(eamUndefined), m_uiCurrentAddingSession(0)
{
}

CFindResultCollection::~CFindResultCollection(void)
{
	Clear();
}

void CFindResultCollection::Clear(void)
{
	m_FindResultsByRelevanceCollection.Clear();
	Clear(&m_FindResultsCollection);
	Clear(&m_TempFindResultsCollection);
}

void CFindResultCollection::Clear(TFindResultsCollection* pFindResultsCollection)
{
	if(ISNULL(pFindResultsCollection)) return;

	for(TFindResultsCollection::iterator itFindResult=pFindResultsCollection->begin(); 
		itFindResult!=pFindResultsCollection->end(); itFindResult++){
		if(itFindResult->second){
			delete itFindResult->second;
		}
	}

	pFindResultsCollection->clear();
}

void CFindResultCollection::StartFindResultAdding(EAddingMode AddingMode)
{
	//для первой сессии режим добавления всегда eamAlways
	if(!m_uiCurrentAddingSession) AddingMode=eamAlways;

	//устанавливаем режим работы
	SetAddingMode(AddingMode);
	
	if(m_CurrentAddingMode==eamIfExist){
		//если был режим добавления "только если результат уже есть"
		//чистим коллекцию по релевантности, в нее все будет добавлятся заново
		m_FindResultsByRelevanceCollection.Clear();
	}
	
	//чистим вспомогательную на всякий случай
	Clear(&m_TempFindResultsCollection);
}

void CFindResultCollection::StopFindResultAdding(void)
{
	if(m_CurrentAddingMode==eamIfExist){
		//если был режим добавления "только если результат уже есть"
		//имеем все результаты во вспомогательной коллекции
		
		//очищаем основную
		Clear(&m_FindResultsCollection);
		
		//свапируем со вспомогательной
		m_FindResultsCollection.swap(m_TempFindResultsCollection);
	}

	//View();

	m_uiCurrentAddingSession++;
}

CFindResult* CFindResultCollection::AddFindResult(unsigned int uiTextIndex, unsigned int uiRelevance)
{
	CFindResult* pFindResult=NULL;
	switch(m_CurrentAddingMode){
		case eamAlways:
			{
				//добавляем результат в общую коллекцию, если такой уже есть увеличиваем его релевантность
				TFindResultsCollection::iterator itFindResult=m_FindResultsCollection.find(uiTextIndex);
				if(itFindResult==m_FindResultsCollection.end()){
					itFindResult=m_FindResultsCollection.insert(
						TFindResultsCollection::value_type(uiTextIndex, new CFindResult(uiTextIndex, 0))).first;

					pFindResult=itFindResult->second;
					//устанавливаем ему релевантность					
					pFindResult->SetRelevance(uiRelevance);
					//добавляем в коллекции по релевантности
					m_FindResultsByRelevanceCollection.AddFindResult(pFindResult);

				}else{
					
					pFindResult=itFindResult->second;
					if(pFindResult->GetRelevance()<uiRelevance){
						//если результат уже есть, удаляем его из коллекции по релевантности
						m_FindResultsByRelevanceCollection.RemoveFindResult(itFindResult->second);
						//устанавливаем ему новую релевантность					
						pFindResult->SetRelevance(uiRelevance);
						//добавляем в коллекции по релевантности
						m_FindResultsByRelevanceCollection.AddFindResult(pFindResult);

					}
				}

			}
			break;
		case eamIfExist:
			{
				//добавляем результат во вспомогательную коллекцию, 
				//только если такой уже есть в основной и увеличиваем его релевантность
				TFindResultsCollection::iterator itFindResult=m_FindResultsCollection.find(uiTextIndex);
				if(itFindResult!=m_FindResultsCollection.end()){
					//получаем результат из основной коллекции, клдаем его во вспомогательную
					m_TempFindResultsCollection.insert(
						TFindResultsCollection::value_type(itFindResult->first, itFindResult->second));
					pFindResult=itFindResult->second;
					//из основной убираем
					m_FindResultsCollection.erase(itFindResult);

					//меняем релевантность
					if(pFindResult->GetRelevance()<uiRelevance) pFindResult->SetRelevance(uiRelevance);

					//добавляем в коллекции по релевантности
					m_FindResultsByRelevanceCollection.AddFindResult(pFindResult);
				}else{
					//если в основной нет, смотрим во вспомогательной
					itFindResult=m_TempFindResultsCollection.find(uiTextIndex);
					if(itFindResult!=m_TempFindResultsCollection.end()){
						//если есть во вспомогательной добавляем релевантность, возвращаем
						pFindResult=itFindResult->second;
						if(pFindResult->GetRelevance()<uiRelevance){	
							//если результат уже есть, удаляем его из коллекции по релевантности
							m_FindResultsByRelevanceCollection.RemoveFindResult(pFindResult);
							//меняем релевантность
							pFindResult->SetRelevance(uiRelevance);
							//добавляем в коллекции по релевантности
							m_FindResultsByRelevanceCollection.AddFindResult(pFindResult);
						}
					}else{
						//если нигде нет
						pFindResult=NULL;
					}
				}
			}
			break;
		default:
			TO_CONSOLE(L"AddingMode undefined. Call StartFindResultAdding with correct AddingMode");
			pFindResult=NULL;
			break;
	}

	return pFindResult;
}

CFindResult* CFindResultCollection::GetFindResult(unsigned int uiTextIndex)
{
	TFindResultsCollection::iterator itFindResult=m_FindResultsCollection.find(uiTextIndex);
	if(itFindResult!=m_FindResultsCollection.end()){
		return itFindResult->second;
	}
	return NULL;
}

void CFindResultCollection::ToString(wstring& wszValue)
{
	if(m_FindResultsCollection.empty()){
		wszValue.assign(L"-Empty-");
		return;
	}
	
	wstring wsFindResult;
	for(TFindResultsCollection::iterator itFindResult=m_FindResultsCollection.begin(); itFindResult!=m_FindResultsCollection.end(); itFindResult++){
		if(itFindResult->second){
			wszValue.append(L"FindResult:\n\t");
			itFindResult->second->ToString(wsFindResult);
			wszValue.append(wsFindResult);
		}
	}
}

void CFindResultCollection::View(void)
{
	ToConsole(L"//-------------------CFindResultCollection");
	wstring wsValue;
	ToString(wsValue);
	ToConsole(wsValue.c_str());
	ToConsole(L"//------------------Eof-CFindResultCollection");
}

//--------------------------------------------------------------------//

}
}
}
}
}
