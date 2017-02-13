//TextBlock.cpp

#include "StdAfx.h"
#include "textblock.h"
#include "TextBlockCut.h"

namespace SS
{
namespace Core
{
namespace CommonContainers
{

//Конструктор
CTextBlock::CTextBlock() 
	: m_uiRelevance(0),
	  m_pTextFeature(NULL),
	  m_pSearchResult(NULL),
	  m_pContainersFactory(NULL)
{
	SS_TRY
	{
		SS::Core::CommonContainers::ITextBlock::m_IndexProcessorMark = 
		(SS::Core::Types::IndexationSearch::EIndexProcessor)0 ;
		m_Iter = m_lpTextBlockCut.end();
	}					
	SS_CATCH(L"")
}

//Деструктор
CTextBlock::~CTextBlock()
{
	if (m_pTextFeature)
	{ delete m_pTextFeature; }
	Clear();
}

//Характеристики текста, которому принадлежит этот текстовый блок
ITextFeature* CTextBlock::GetTextFeature()			
{
	SS_TRY
	{
		if (m_pTextFeature)
		{ return m_pTextFeature; }
		m_pTextFeature = m_pContainersFactory->CreateTextFeature();
	}
	SS_CATCH(L"")
	return m_pTextFeature;
}

//Очистка блока (удалит все вырезки из текста)
void CTextBlock::Clear()
{
	SS_TRY
	{
		for (m_Iter = m_lpTextBlockCut.begin(); m_Iter != m_lpTextBlockCut.end(); m_Iter++)
		{
			if ((*m_Iter))
			{ delete (*m_Iter);	}
		}
		m_lpTextBlockCut.clear();
	}
	SS_CATCH(L"")
}

bool CompareRelevance(ITextBlockCut* ptbFirst, ITextBlockCut* ptbSecond)
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

void CTextBlock::UpdateRelevance()
{
	SS_TRY
	{
		//static int i = 0;
		//++i;
		//wchar_t ws[20];
		//SAVE_LOG(SS_MESSAGE AND _itow(i,ws,10) AND __WFUNCTION__);
		//m_lpTextBlockCut.sort(CompareRelevance);
	}
	SS_CATCH(L"")
}

//Получение первой вырезки коллекции (0 - если нет)
ITextBlockCut* CTextBlock::GetFirstBlockCut()
{
	SS_TRY
	{
		if (m_lpTextBlockCut.empty())
		{ return 0;	}
		m_Iter = m_lpTextBlockCut.begin();
		return (*m_Iter);
	}
	SS_CATCH(L"")
	return NULL;
}

//Получение очередной вырезки коллекции (0 - если закончились)
ITextBlockCut* CTextBlock::GetNextBlockCut()
{
	SS_TRY
	{
		if (m_Iter == m_lpTextBlockCut.end())
		{
			//SS_THROW(L" Не вызван метод GetFirstBlockCut() ")
			SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L" Не вызван метод GetFirstBlockCut() ");
			return 0;
		}
		m_Iter++;
		if (m_Iter == m_lpTextBlockCut.end())
		{ return 0; }
		return (*m_Iter);
	}
	SS_CATCH(L"")
	return NULL;
}

//Получение XML - строки содержимого контейнера
std::wstring* CTextBlock::GetXML()
{
	SS_TRY
	{
		int iBase = 10;
		wchar_t Buffer[12];
		CTextBlockCut* pTextBlockCut = NULL;
		
		pTextBlockCut = dynamic_cast <CTextBlockCut*> (GetFirstBlockCut()); 
							
		m_ws = L"<?xml version=\"1.0\" encoding=\"windows-1251\" ?>";
		m_ws += L"<ITextBlock>";
		m_ws += L"<Relevance>";
		_itow(m_uiRelevance, Buffer, iBase);
		m_ws += Buffer;
		m_ws += L"</Relevance>";
		while (pTextBlockCut)
		{
			m_ws += (*(pTextBlockCut->GetXML()));
			pTextBlockCut = dynamic_cast <CTextBlockCut*> (GetNextBlockCut());
		}
		m_ws += L"</ITextBlock>";
	}
	SS_CATCH(L"")
	return &m_ws;
}

///Добавляет в коллекцию объект СTextBlockCut
void CTextBlock::AddCut(ITextBlockCut* pTextBlockCut)
{
	m_lpTextBlockCut.push_back(pTextBlockCut);
}

bool BlockCutComparer(ITextBlockCut* pFirst, ITextBlockCut* pSecond)
{
	return pFirst->GetRelevance() > pSecond->GetRelevance();
}

void CTextBlock::Sort()
{
	m_lpTextBlockCut.sort(BlockCutComparer);
}

}
}
}