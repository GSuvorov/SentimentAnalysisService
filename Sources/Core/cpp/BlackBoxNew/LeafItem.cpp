#include "stdafx.h"
#include "LeafItem.h"

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

CLeafItem::CLeafItem() :
		m_pLeft(NULL),
		m_pRight(NULL),
		m_pParent(NULL)
{
}

CLeafItem::~CLeafItem()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CLeafItem interface

void CLeafItem::InsertItem(CLeafItem* pItem)
{
	if(pItem->m_pLeft || pItem->m_pParent || pItem->m_pRight)
		SS_THROW(L"Неправильное использование CLeafItem::InsertItem");

	if(m_pRight)
		m_pRight->m_pLeft = pItem;
	else
		m_pParent->m_pRightChild = pItem;

	pItem->m_pParent = m_pParent;
	pItem->m_pLeft = this;
	pItem->m_pRight = m_pRight;
	m_pRight = pItem;
}

CLeafItem* CLeafItem::PopItem()
{
	if(!m_pParent)
		return NULL;

	// оповещаем родителя
	m_pParent->OnPopItem(this);

	// удаляем себя из родительского списка
	if(m_pLeft)
		m_pLeft->m_pRight = m_pRight;
	else
		m_pParent->m_pLeftChild = m_pRight;
	if(m_pRight)
		m_pRight->m_pLeft = m_pLeft;
	else
		m_pParent->m_pRightChild = m_pLeft;

	// обнуляем внутренние указатели
	CLeafItem* pRight = m_pRight;
	m_pLeft = m_pRight = NULL;
	m_pParent = NULL;
	return pRight;
}

CLeafItem* CLeafItem::ReleaseItem()
{
	// удаляем себя из родительского списка
	CLeafItem* pRight = PopItem();

	// удаляем себя
	delete this;
	return pRight;
}

CLeafItem* CLeafItem::GetRight()
{
	return m_pRight;
}

CLeafItem* CLeafItem::GetLeft()
{
	return m_pLeft;
}

CRootItem* CLeafItem::GetParent()
{
	return m_pParent;
}

}
}
}
