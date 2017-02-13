#pragma once


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CRootItem;

class CLeafItem
{
	friend class CRootItem;
public:
	CLeafItem();
	virtual ~CLeafItem();

public:
	///добавляет элемент справа от себя
	void InsertItem(CLeafItem* pItem);
	///вырезает себя из родительского списка детей и возвращает элемент справа
	CLeafItem* PopItem();
	///удаляет всех своих детей и вырезает себя из списка детей своего родителя
	CLeafItem* ReleaseItem();
	///возвращает элемент справа
	CLeafItem* GetRight();
	///возвращает элемент слева
	CLeafItem* GetLeft();
	///возвращает родительский элемент
	CRootItem* GetParent();

private:
	CLeafItem* m_pLeft;
	CLeafItem* m_pRight;
	CRootItem* m_pParent;
};

}
}
}
