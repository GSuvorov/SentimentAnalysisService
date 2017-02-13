#include "stdafx.h"
#include "RootItem.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CRootItem::CRootItem() :
		m_pLeftChild(NULL),
		m_pRightChild(NULL)
{
}

CRootItem::~CRootItem()
{
	ReleaseChildren();
}

/////////////////////////////////////////////////////////////////////////////////////
// CRootItem interface

void CRootItem::AddChild(CLeafItem* pItem)
{
	if(pItem->m_pLeft || pItem->m_pParent || pItem->m_pRight)
		SS_THROW(L"Неправильное использование CItem::AddChild");

	pItem->m_pParent = this;
	pItem->m_pLeft = m_pRightChild;
	if(m_pRightChild)
	{
		m_pRightChild->m_pRight = pItem;
		m_pRightChild = pItem;
	}
	else
	{
		m_pLeftChild = pItem;
		m_pRightChild = pItem;
	}
}

void CRootItem::ReleaseChildren()
{
	for(CLeafItem* pItem = GetFirstChild(); pItem; )
		pItem = pItem->ReleaseItem();
	m_pLeftChild = m_pRightChild = NULL;
}

bool CRootItem::IsItemEmpty()
{
	return m_pLeftChild == NULL;
}

CLeafItem* CRootItem::ReleaseItem()
{
	delete this;
	return NULL;
}

CLeafItem* CRootItem::GetFirstChild()
{
	return m_pLeftChild;
}

CLeafItem* CRootItem::GetLastChild()
{
	return m_pRightChild;
}

}
}
}
