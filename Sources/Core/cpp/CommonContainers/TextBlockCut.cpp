//TextBlockCut.cpp
#include "StdAfx.h"
#include "textblockcut.h"
#include "IndexAndPosition.h"

namespace SS
{
namespace Core
{
namespace CommonContainers
{
	
//конструкторы
CTextBlockCut::CTextBlockCut(IBlackBoxTextStorage* pBlackBoxTextStorage, TLinguisticProcessorMode*	pLinguisticProcessorMode) 
	: 
	m_wszTextCut(NULL), m_wszPluginName(NULL), 
	m_wsIndexTypeName(L""), m_pBlackBoxTextStorage(pBlackBoxTextStorage),
	m_pLinguisticProcessorMode(pLinguisticProcessorMode),
	m_uiRelevance(0),
	m_pContainersFactory(NULL)
{
	SS_TRY
	{
		m_wszTextCut = new wchar_t[1];
		m_wszPluginName = new wchar_t[1];
		wcscpy(m_wszTextCut, L"");
		wcscpy(m_wszPluginName, L"");
		m_oBlockCutData.Level = 0;
		m_oBlockCutData.Number = 0;
		m_oBlockCutData.TextCutType = 0;
		m_listIter = m_list_pIndexAndPosition.end();
	}
	SS_CATCH(L"")
}

///присвоение "имени" типа индекса данной текстовой части
void CTextBlockCut::SetIndexTypeName(const wchar_t* wszName)
{
	SS_TRY
	{
		m_wsIndexTypeName = wszName;
	}
	SS_CATCH(L"")
}
///получение "имени" типа индекса данной текстовой части
const wchar_t* CTextBlockCut::GetIndexTypeName()
{
	SS_TRY
	{
		return m_wsIndexTypeName.c_str();
	}
	SS_CATCH(L"")
}

CTextBlockCut::~CTextBlockCut()
{
	Clear();
	if (m_wszTextCut)
	{
		delete [] m_wszTextCut;
	}
	if (m_wszPluginName) 
	{
		delete [] m_wszPluginName;
	}
}

//другие методы
void CTextBlockCut::Clear()										// очистка вырезки (удалит все индексы)
{
	SS_TRY
		for (m_listIter = m_list_pIndexAndPosition.begin(); m_listIter != m_list_pIndexAndPosition.end(); m_listIter++)
		{
			if ((*m_listIter)) 
			{
				delete (*m_listIter);
			}						
		}	

		m_list_pIndexAndPosition.clear();
		
	SS_CATCH(L"")
}

IIndexAndPosition* CTextBlockCut::AddIndexAndPosition()			// добавление индекса в коллекцию (добавит в себя и вернет указатель)
{
	SS_TRY
		IIndexAndPosition *pIndexAndPosition;
		pIndexAndPosition = m_pContainersFactory->CreateIndexAndPosition(m_pBlackBoxTextStorage, m_pLinguisticProcessorMode);
		m_list_pIndexAndPosition.push_back(pIndexAndPosition);
		return pIndexAndPosition;
	SS_CATCH(L"")
}

IIndexAndPosition* CTextBlockCut::GetFirstIndexAndPosition()	// получение первого индекса коллекции (0 - если нет)
{
	SS_TRY
		if (m_list_pIndexAndPosition.empty())
		{
			return NULL;
		}
		m_listIter = m_list_pIndexAndPosition.begin();
		return (*m_listIter);
	SS_CATCH(L"")
}

IIndexAndPosition* CTextBlockCut::GetNextIndexAndPosition()		// получение очередного индекса коллекции (не забывать вызвать 
{																// получение первого индекса) (0 - если закончились)
	SS_TRY
		if (m_listIter == m_list_pIndexAndPosition.end())
		{
			//SS_THROW(L" Не вызван метод GetFirstIndexAndPosition() ")
			SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L" Не вызван метод GetFirstIndexAndPosition() ");
			return NULL;
		}
		m_listIter++;
		if (m_listIter == m_list_pIndexAndPosition.end())
		{
			return NULL;
		}
		return (*m_listIter);
	SS_CATCH(L"")
}

// присвоение текстового куска в виде строки (используется плагинами GlobalSearch)
void CTextBlockCut::SetTextCut(const wchar_t* wszText)
{
	SS_TRY
		if (m_wszTextCut)
		{
			delete [] m_wszTextCut;
			m_wszTextCut = NULL;
		}
		if (wszText)
		{
			int len;
			len = (int) wcslen(wszText);
			m_wszTextCut = new wchar_t[len + 1];
			wcscpy(m_wszTextCut, wszText);
		}	
	SS_CATCH(L"")
}
// получение текстового куска в виде строки
const wchar_t* CTextBlockCut::GetTextCut()
{
	SS_TRY
		if (m_wszTextCut)
		{
			return m_wszTextCut;
		}
		return NULL;
	SS_CATCH(L"")
}

// установление "имени" плагина
void CTextBlockCut::SetPluginName(const wchar_t* wsName)
{
	SS_TRY
		if (m_wszPluginName) 
		{
			delete [] m_wszPluginName;
			m_wszPluginName = NULL;
		}
		if (wsName) 
		{
			int len;
			len = (int) wcslen(wsName);
			m_wszPluginName = new wchar_t[len + 1];
			wcscpy(m_wszPluginName, wsName);
		}
	SS_CATCH(L"")
}

// получение "имени" плагина
const wchar_t* CTextBlockCut::GetPluginName() const
{
	SS_TRY
		if (m_wszPluginName) 
		{
			return m_wszPluginName;
		}
		return NULL;
	SS_CATCH(L"")
}

// получение XML - строки содержимого контейнера
std::wstring* CTextBlockCut::GetXML()
{
	SS_TRY
		CIndexAndPosition* pIndexAndPosition = NULL;

		pIndexAndPosition = dynamic_cast <CIndexAndPosition*> (GetFirstIndexAndPosition());
		
		m_ws = L"<ITextBlockCut>";
		while (pIndexAndPosition)
		{
			m_ws += (*(pIndexAndPosition->GetXML()));
			pIndexAndPosition = dynamic_cast <CIndexAndPosition*> (GetNextIndexAndPosition());
		}
		m_ws += L"</ITextBlockCut>";
		return &m_ws;
	SS_CATCH(L"")
}

// возвращает тип текст блок
SS::Core::Types::IndexationSearch::ETextCutType CTextBlockCut::GetType()
{
	return (SS::Core::Types::IndexationSearch::ETextCutType) m_oBlockCutData.TextCutType;
}
// устанавливает тип текст блока
void CTextBlockCut::SetType(SS::Core::Types::IndexationSearch::ETextCutType eTextCutType)
{
	m_oBlockCutData.TextCutType = eTextCutType;
}
// возвращает уровень заголовка
unsigned int CTextBlockCut::GetLevel()
{
	return m_oBlockCutData.Level;
}
// устанавливает уровень заголовка
void CTextBlockCut::SetLevel(unsigned int uiLevel)
{
	m_oBlockCutData.Level = uiLevel;
}
// возвращает номер группы текст блок катов
unsigned int CTextBlockCut::GetNumber()
{
	return m_oBlockCutData.Number;
}
// устанавливает номер группы текст блок катов
void CTextBlockCut::SetNumber(unsigned int uiNumber)
{
	m_oBlockCutData.Number = uiNumber;
}	

}
}
}