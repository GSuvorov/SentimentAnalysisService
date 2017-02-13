#pragma once

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

//--------------------------------------------------------------------//

///функциональность итема однонаправленного списка
template <class TItemType>
class CUnidirListItem
{
public:
	CUnidirListItem(void):m_pNextItem(NULL){};
	virtual ~CUnidirListItem(void){};

	///возвращает следующий узел в списке
	TItemType* GetNextItem(void){return m_pNextItem;};
	///устанавливает следующий узел в списке
	void SetNextItem(TItemType* pNextItem){m_pNextItem=pNextItem;};

protected:
	///следующий итем
	TItemType* m_pNextItem;
};

//--------------------------------------------------------------------//

///функциональность итема двунаправленного списка
template <class TItemType>
class CBidirListItem : public CUnidirListItem<TItemType>
{
public:
	CBidirListItem(void):m_pPrevItem(NULL){};
	virtual ~CBidirListItem(void){};

	///возвращает предыдущий узел в списке
	TItemType* GetPrevItem(void){return m_pPrevItem;};
	///устанавливает предыдущий узел в списке
	void SetPrevItem(TItemType* pPrevItem){m_pPrevItem=pPrevItem;};

protected:
	///предыдущий итем
	TItemType* m_pPrevItem;
};

//--------------------------------------------------------------------//

///хранилище итемов списка
template <class TListItem>
class CListContainer
{
public:
	CListContainer(void):m_pFirstItem(NULL), m_pLastItem(NULL){};
	virtual ~CListContainer(void){};

	///возвращает первый узел в списке
	TListItem* GetFirstItem(void){return m_pFirstItem;};
	///возвращает последний узел в списке
	TListItem* GetLastItem(void){return m_pLastItem;};
	///проверяет список на пустоту
	bool IsEmptyList(void){return (m_pFirstItem==NULL && m_pLastItem==NULL)?true:false;};

	///очищает список
	void DeleteItems(void)
	{
		TListItem* pItem=m_pFirstItem;
		TListItem* pTempItem=NULL;
		while(pItem){
			pTempItem=pItem->GetNextItem();
			delete pItem;
			pItem=pTempItem;
		}

		//зануляем указатели на концы списка
		m_pFirstItem=NULL;
		m_pLastItem=NULL;
	}	

protected:
	///первый итем
	TListItem* m_pFirstItem;
	///последний итем
	TListItem* m_pLastItem;

};

//--------------------------------------------------------------------//

}
}
}
}
}