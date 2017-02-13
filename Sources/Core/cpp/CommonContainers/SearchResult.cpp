// SearchResult.cpp
#include "StdAfx.h"
#include "searchresult.h"
#include "TextBlockEx.h"
#include <algorithm>

namespace SS
{
namespace Core
{
namespace CommonContainers
{

//КОНСТРУКТОРЫ
CSearchResult::CSearchResult() : 
		m_bIsSortedDataActuality(false), 
		m_bSorted(false),
		m_uiRequestID(-1),
		m_bGlobal(false),
		m_pContainersFactory(NULL)
{
	SS_TRY		
	{
		m_lIter = m_lpTextBlock.end();
		m_lIterSort = m_lpTextBlockSort.end();
	}
	SS_CATCH(L"")
}

CSearchResult::~CSearchResult()	
{
	Clear();
}

//ФУНКЦИЯ - БИНАРНЫЙ ПРЕДИКАТ НЕ ЧЛЕН КЛАССА
bool CompareRelevance(ITextBlock* ptbFirst, ITextBlock* ptbSecond)
{
	SS_TRY
	{
		if (ptbFirst->GetRelevance() >= ptbSecond->GetRelevance())
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	SS_CATCH(L"")
}

//СОБСТВЕННЫЕ МЕТОДЫ
void CSearchResult::Sort()
{
	SS_TRY
	{
		SL_OUT_DEBUG_STR (__WFUNCTION__);

		m_lpTextBlockSort.assign(m_lpTextBlock.begin(), m_lpTextBlock.end());
		m_lpTextBlockSort.sort(CompareRelevance);
		m_lIterSort = m_lpTextBlockSort.end();
		
		if (m_bGlobal) return;
		
		for(m_lIter = m_lpTextBlock.begin(); m_lIter != m_lpTextBlock.end(); ++m_lIter)
			dynamic_cast<CTextBlock*>(*m_lIter)->Sort();
	}
	SS_CATCH(L"")
}

// МЕТОДЫ IBase
ULONG CSearchResult::Release()
{
	delete this;
	return 0;
}

HRESULT CSearchResult::QueryInterface(REFIID pIID, void** pBase)	//приведение к интерфейсу с поданным REFIID.
																	//pBase будет NULL, если данный интерфейс не поддерживается
{
	if (IID_SearchResult == pIID)
	{
		*pBase = (ISearchResult*)this;
	}
	else if(IID_SearchResultEx == pIID)
	{
		*pBase = (ISearchResultEx*)( this );
	}
	else if( pIID == IID_Base )
	{
		*pBase = static_cast< SS::Interface::IBase* >( this );
	}
	else 
		*pBase = NULL;
	return S_OK;
}

//МЕТОДЫ ISearchResult
ITextBlock* CSearchResult::GetTextBlock(unsigned int nPosition)
{
	SS_TRY
	{
		SL_OUT_DEBUG_STR (__WFUNCTION__);

#ifdef _SS_UNITTESTS
		wchar_t wcToMsg[100];
		_itow(nPosition, wcToMsg, 10);
		wcscat(wcToMsg, L" ");
		wcscat(wcToMsg, L"= Position");
		wcscat(wcToMsg, L" ");
		wcscat(wcToMsg, __WFUNCTION__);
		SL_OUT_DEBUG_STR (wcToMsg);
#endif 

		if (nPosition >= m_lpTextBlock.size())
		{
			return NULL;
		}
		
		std::list<ITextBlock*>::iterator It;
		unsigned int uiPos;
		
		if (m_bSorted)
		{

SL_OUT_DEBUG_STR (L"m_bSorted == true");

			if(!m_bIsSortedDataActuality)
			{
SL_OUT_DEBUG_STR (L"m_bIsSortedDataActuality == true");
				Sort();
				m_bIsSortedDataActuality = true;
			}

			if (m_lpTextBlockSort.empty())
			{
				return NULL;
			}
			for (It = m_lpTextBlockSort.begin(), uiPos = 0; It != m_lpTextBlockSort.end(); It++, uiPos++)
			{
				if (uiPos == nPosition)
				{
					m_lIterSort = It;

					return (*It);
				}
			}
		}
		else
		{
			if (m_lpTextBlock.empty()) 
			{
				return NULL;
			}
			for (It = m_lpTextBlock.begin(), uiPos = 0; It != m_lpTextBlock.end(); It++, uiPos++)
			{
				if (uiPos == nPosition)
				{
					m_lIter = It;

					return (*It);
				}
			}
		}

		return NULL;
	}
	SS_CATCH(L"")
}

unsigned int CSearchResult::Count()				// вернет количество результатов поиска
{
	SS_TRY
	{
		return m_lpTextBlock.size();
	}
	SS_CATCH(L"")
}

void CSearchResult::SetSortedMode(bool bOn)		// установит режим обхода результатов
{
	SS_TRY
	{
		SL_OUT_DEBUG_STR (__WFUNCTION__);
#ifdef _SS_UNITTESTS
		if(bOn)
			SL_OUT_DEBUG_STR (L"On == true CSearchResult::SetSortedMode");
		else
			SL_OUT_DEBUG_STR (L"On == false CSearchResult::SetSortedMode");
#endif 

		m_bSorted = bOn;	
	}
	SS_CATCH(L"");
}

void CSearchResult::Clear()
{
	SS_TRY
	{

		SL_OUT_DEBUG_STR (__WFUNCTION__);

		m_bIsSortedDataActuality = false;

		for (m_lIter = m_lpTextBlock.begin(); m_lIter != m_lpTextBlock.end(); m_lIter++)
		{
			if ((*m_lIter)) 
			{
				delete (*m_lIter);
			}						
		}
		m_lpTextBlock.clear();
		m_lpTextBlockSort.clear();
		m_lIter = m_lpTextBlock.end();
		m_lIterSort = m_lpTextBlockSort.end();
		m_uiRequestID = -1;
		m_bGlobal = false;
	}
	SS_CATCH(L"")
}

void CSearchResult::AddTextBlock(ITextBlock * pTextBlock)
{
	SS_TRY
	{

//		SL_OUT_DEBUG_STR (__WFUNCTION__);
		m_bIsSortedDataActuality = false;

		if(find(m_lpTextBlock.begin(), m_lpTextBlock.end(), pTextBlock) == m_lpTextBlock.end())
		{
			m_lpTextBlock.push_back(pTextBlock);
		}
	}
	SS_CATCH(L"")
}


ITextBlock *CSearchResult::AddTextBlock()
{
	SS_TRY
	{
//		SL_OUT_DEBUG_STR (__WFUNCTION__);
		m_bIsSortedDataActuality = false;
		CTextBlockEx* pTextBlockEx;
		pTextBlockEx = dynamic_cast<CTextBlockEx*>(m_pContainersFactory->CreateTextBlock(GetLoadManager()));
		pTextBlockEx->SetSearchResult(this);
		m_lpTextBlock.push_back(pTextBlockEx);
		return pTextBlockEx;
	}
	SS_CATCH(L"")
}

ITextBlock *CSearchResult::GetFirstTextBlock()
{
	SS_TRY
	{
//		SL_OUT_DEBUG_STR (__WFUNCTION__);

		if (m_bSorted)
		{
			if(!m_bIsSortedDataActuality)
			{
				Sort();
				m_bIsSortedDataActuality = true;
			}

			if (m_lpTextBlockSort.empty())
			{
				return NULL;
			}
			m_lIterSort = m_lpTextBlockSort.begin();

			return (*m_lIterSort);
		}
		else
		{
			if (m_lpTextBlock.empty())
			{
				return NULL;
			}
			m_lIter = m_lpTextBlock.begin();
			return (*m_lIter);
		}
	}
	SS_CATCH(L"")
}

ITextBlock *CSearchResult::GetNextTextBlock()
{
	SS_TRY
	{
//		SL_OUT_DEBUG_STR (__WFUNCTION__);

		if (m_bSorted)
		{
			if(!m_bIsSortedDataActuality)
			{
				Sort();
				m_bIsSortedDataActuality = true;
			}

			if (m_lIterSort == m_lpTextBlockSort.end())
			{
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L" Не вызван метод GetFirstIndexAndPosition() ");
				return NULL;
			}
			m_lIterSort++;
			if (m_lIterSort == m_lpTextBlockSort.end())
			{
				return NULL; 
			}
#ifdef _SS_UNITTESTS
		wchar_t wcToMsg[100];
		_itow((*m_lIterSort)->GetRelevance(), wcToMsg, 10);
		wcscat(wcToMsg, L" m_lIterSort");
		SL_OUT_DEBUG_STR (wcToMsg);
#endif 

			return (*m_lIterSort);
		}
		else
		{
			if (m_lIter == m_lpTextBlock.end())
			{
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L" Не вызван метод GetFirstIndexAndPosition() ");
				return NULL;
			}
			m_lIter++;
			if (m_lIter == m_lpTextBlock.end())
			{
				return NULL; 
			}
#ifdef _SS_UNITTESTS
		wchar_t wcToMsg[100];
		_itow((*m_lIter)->GetRelevance(), wcToMsg, 10);
		wcscat(wcToMsg, L" m_lIter");
		SL_OUT_DEBUG_STR (wcToMsg);
#endif 

			return (*m_lIter);
		}
	}
	SS_CATCH(L"")
}

bool CSearchResult::GetXML(std::wstring* pXML)
{
	SetSortedMode(true);
	return m_oXML.GetXML(this, pXML);
}

bool CSearchResult::GetXML(std::wstring* pXML, int iFirst, int iCount)
{
	SetSortedMode(true);
	return m_oXML.GetXML(this, pXML, iFirst, iCount);
}

void CSearchResult::SetRequestID(unsigned int id)
{
	m_uiRequestID = id;
}

unsigned int CSearchResult::GetRequestID()
{
	return m_uiRequestID;
}

#include <msxml2.h>
#pragma comment(lib, "MsXml2.lib")

//Установить результат из xml (каждый вызов создает новый TextBlock)
void CSearchResult::SetGsXML(const wchar_t* wzXml)
{
	SS_TRY
	{
		m_bGlobal = true;

		xml2::IXMLDOMDocument2Ptr m_pXmlDoc;
		HRESULT hr = m_pXmlDoc.CreateInstance(CLSID_DOMDocument2);
		if (FAILED(hr)) return;
		m_pXmlDoc->loadXML( (BSTR) (void*) wzXml );
		//Result(один)
		xml2::IXMLDOMNodeListPtr pListResult = m_pXmlDoc->selectNodes(L"Result");
		xml2::IXMLDOMNodePtr pNodeResult = pListResult->nextNode();
		if (pNodeResult == NULL) return;
		//Для релевантности
		unsigned int uiRev = 0;
		ITextBlockCut* pTextBlockCut = NULL;
		ITextBlock* pTextBlock = NULL;
		//Для имени плагина
		_bstr_t bsName;
		//Plugin
		xml2::IXMLDOMNodeListPtr pListPlugins = pNodeResult->selectNodes(L"Plugin");
		xml2::IXMLDOMNodePtr pNodePlugin = pListPlugins->nextNode();
		while (pNodePlugin != NULL)
		{
			//Выбор атрибута Name
			xml2::IXMLDOMNamedNodeMapPtr pAttrMapPlugin = pNodePlugin->Getattributes();
			xml2::IXMLDOMNodePtr		 pAttrNodeName = pAttrMapPlugin->getNamedItem(L"Name");
			if (pAttrNodeName != NULL)
			{
				pAttrNodeName->put_dataType(L"string");
				bsName = pAttrNodeName->nodeTypedValue;
			}
			else
			{ bsName = L"NoName"; }
			//Page/Answer/text
			xml2::IXMLDOMNodeListPtr pListPage = pNodePlugin->selectNodes(L"Page");
			xml2::IXMLDOMNodePtr pNodePage = pListPage->nextNode();
			while (pNodePage != NULL)
			{
				xml2::IXMLDOMNodeListPtr pListAnswer = pNodePage->selectNodes(L"Answer");
				xml2::IXMLDOMNodePtr pNodeAnswer = pListAnswer->nextNode();
				while (pNodeAnswer != NULL)
				{
					//Выбор атрибута Rev
					xml2::IXMLDOMNamedNodeMapPtr pAttrMap = pNodeAnswer->Getattributes();
					xml2::IXMLDOMNodePtr		 pNodeAttr = pAttrMap->getNamedItem(L"Rev");
					if (pNodeAttr != NULL) 
					{
						pNodeAttr->put_dataType(L"int");
						uiRev = pNodeAttr->nodeTypedValue;
					}
					else
					{ uiRev = 0; }
					//TextBlock
					pTextBlock = AddTextBlock();
					pTextBlock->SetRelevance(uiRev);
					//head
					xml2::IXMLDOMNodeListPtr pListHead = pNodeAnswer->selectNodes(L"link");
					xml2::IXMLDOMNodePtr pNodeHead = pListHead->nextNode();
					while (pNodeHead != NULL)
					{
						pTextBlockCut = pTextBlock->AddBlockCut();
						pTextBlockCut->SetTextCut(pNodeHead->Gettext());
						pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctHeader);
						//Имя плагина выставляем только у заголовка
						pTextBlockCut->SetPluginName(bsName);
						//Следующий
						pNodeHead = pListHead->nextNode();
					}
					//text
					xml2::IXMLDOMNodeListPtr pListText = pNodeAnswer->selectNodes(L"text");
					xml2::IXMLDOMNodePtr pNodeText = pListText->nextNode();
					while (pNodeText != NULL)
					{
						pTextBlockCut = pTextBlock->AddBlockCut();
						pTextBlockCut->SetTextCut(pNodeText->Gettext());
						pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctText);
						//Следующий
						pNodeText = pListText->nextNode();
					}
					//ref
					xml2::IXMLDOMNodeListPtr pListRef = pNodeAnswer->selectNodes(L"ref");
					xml2::IXMLDOMNodePtr pNodeRef = pListRef->nextNode();
					while (pNodeRef != NULL)
					{
						pTextBlockCut = pTextBlock->AddBlockCut();
						pTextBlockCut->SetTextCut(pNodeRef->Gettext());
						//Метка таблицы используется для обозначения адреса
						pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctHttp); 
						//Следующий
						pNodeRef = pListRef->nextNode();
					}
					//Следующий ответ
					pNodeAnswer = pListAnswer->nextNode();
				}
				pNodePage = pListPage->nextNode();
			}
			pNodePlugin = pListPlugins->nextNode();
		}
	}
	SS_CATCH(L"")
}

}
}
}








