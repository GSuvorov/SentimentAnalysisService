#include "stdafx.h"
#include "RootIterateItem.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CRootIterateItem::CRootIterateItem(/*CRootItem* pParent*/) :
		m_pCurrItem(NULL)//, m_pParent(pParent)
{
}

CRootIterateItem::~CRootIterateItem()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CRootIterateItem interface

void CRootIterateItem::OnPopItem(CLeafItem* pItem)
{
	if(pItem == m_pCurrItem)
		m_pCurrItem = pItem->GetRight();

	/*if(m_pParent)
	{
		m_pParent->OnPopItem(pItem);
	}*/
}

CLeafItem* CRootIterateItem::GetFirstChild()
{
	m_pCurrItem = CRootItem::GetFirstChild();
	return m_pCurrItem;
}

CLeafItem* CRootIterateItem::GetLastChild()
{
	m_pCurrItem = CRootItem::GetLastChild();
	return m_pCurrItem;
}

CLeafItem* CRootIterateItem::GetNextChild()
{
	m_pCurrItem = m_pCurrItem->GetRight();
	return m_pCurrItem;
}

CLeafItem* CRootIterateItem::GetPrevChild()
{
	m_pCurrItem = m_pCurrItem->GetLeft();
	return m_pCurrItem;
}

}
}
}
