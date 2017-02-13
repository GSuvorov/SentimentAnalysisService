//IndexationResult.cpp
#include "StdAfx.h"
#include "indexationresult.h"

namespace SS
{
namespace Core
{
namespace CommonContainers
{
//конструкторы
CIndexationResult::CIndexationResult() : m_pContainersFactory(NULL)
{
	SS_TRY
		m_listIter = m_lpTextBlock.end();
	SS_CATCH(L"")
}

CIndexationResult::~CIndexationResult()
{
	Clear(); 
}

//другие методы
void CIndexationResult::Clear()
{
	SS_TRY
		for (m_listIter = m_lpTextBlock.begin(); m_listIter != m_lpTextBlock.end(); m_listIter++)
		{
			if ((*m_listIter)) 
			{
				delete (*m_listIter);
			}						
		}
		m_lpTextBlock.clear();
	SS_CATCH(L"")
}


ITextBlock *CIndexationResult::AddTextBlock()
{
	SS_TRY
	{
		ITextBlock* pTextBlock;
		pTextBlock = m_pContainersFactory->CreateTextBlock(GetLoadManager());
		m_lpTextBlock.push_back(pTextBlock);
		return pTextBlock;
	}
	SS_CATCH(L"")
	return NULL;
}

ITextBlock *CIndexationResult::GetFirstTextBlock()
{
	SS_TRY
		if (m_lpTextBlock.empty())
		{
			return 0;
		}
		m_listIter = m_lpTextBlock.begin();
		return (*m_listIter);
	SS_CATCH(L"")
}

ITextBlock *CIndexationResult::GetNextTextBlock()
{
	SS_TRY
		if (m_listIter == m_lpTextBlock.end())
		{
			SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Не вызван метод GetFirstTextBlock()");
			return 0;
		}
		m_listIter++;
		if (m_listIter == m_lpTextBlock.end())
		{
			return 0; 
		}
		return (*m_listIter);
	SS_CATCH(L"")
}

// методы IBase
ULONG CIndexationResult::Release()
{
	delete this;
	return 0;
}

HRESULT CIndexationResult::QueryInterface(REFIID pIID, void** pBase)	//приведение к интерфейсу с поданным REFIID. 
//pBase будет NULL, если данный интерфейс не поддерживается
{
	if (pIID==IID_IndexationResult)
	{
		*pBase = (IIndexationResult*)this;
	}
	else if( pIID == IID_Base )
	{
		*pBase=static_cast< SS::Interface::IBase* >( this );
	}
	else 
		*pBase = NULL;
	return S_OK;
}


}
}
}