#pragma once

#include "LeafItem.h"


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CRootItem
{
	friend class CLeafItem;
public:
	CRootItem();
	virtual ~CRootItem();

public:
	///добавляет детский элемент
	void AddChild(CLeafItem* pItem);
	///удаляет всех детей
	void ReleaseChildren();
	///проверяет есть ли дети
	bool IsItemEmpty();
	///удаляет себя
	virtual CLeafItem* ReleaseItem();
	///возвращяет первого ребенка
	virtual CLeafItem* GetFirstChild();
	///возвращает последнего ребенка
	virtual CLeafItem* GetLastChild();
	///вызывается при удалении элемента из списка детей
	virtual void OnPopItem(CLeafItem* pItem) { }

private:
	CLeafItem* m_pLeftChild;
	CLeafItem* m_pRightChild;
};

class CMiddleItem : 
		public CRootItem,
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
