#pragma once

#include "RootItem.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CRootIterateItem : public CRootItem
{
public:
	CRootIterateItem(/*CRootItem* pParent = 0*/);
	virtual ~CRootIterateItem();

public:
	///вызывается при удалении элемента из списка детей
	virtual void OnPopItem(CLeafItem* pItem);
	///возвращяет первого ребенка
	virtual CLeafItem* GetFirstChild();
	///возвращает последнего ребенка
	virtual CLeafItem* GetLastChild();
	///возвращает следующего ребенка
	CLeafItem* GetNextChild();
	///возвращает следующего ребенка
	CLeafItem* GetPrevChild();

private:
	CLeafItem* m_pCurrItem;
	//CRootItem* m_pParent;
};

class CMiddleIterateItem :
		public CRootIterateItem,
		public CLeafItem
{
protected:
	virtual CLeafItem* ReleaseItem()
	{
		CRootItem::ReleaseChildren();
		return CLeafItem::ReleaseItem();
	}
};

}
}
}
