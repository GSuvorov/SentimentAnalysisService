#pragma once
#include ".\stl.h"
#include ".\console.h"

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

///узел дерева без поддержки хранения детей
template <class T>
class CUnChildNode
{
public:
	CUnChildNode(void)
		:m_pMyself(NULL), m_pParent(NULL), m_uiDepth(0), m_bLeaf(true){};

	CUnChildNode(T pMySelf)
		:m_pMyself(pMySelf), m_pParent(NULL), m_uiDepth(0), m_bLeaf(true){};

	virtual ~CUnChildNode(void)
	{
		if(m_pMyself){
			delete m_pMyself;
			m_pMyself=NULL;
		}
	};

	///устанавливает глубину узла
	void SetDepth(unsigned int uiValue){m_uiDepth=uiValue;};
	///возвращает глубину узла
	unsigned int GetDepth(void){return m_uiDepth;};
	///устанавливает родительский узел
	virtual void SetParent(CUnChildNode* pValue){m_pParent=pValue;};
	///возвращает родительский узел
	virtual CUnChildNode<T>* GetParent(void){return m_pParent;};
	///возвращает свою начинку
	T GetMyself(void){return m_pMyself;};
	///устанавливает свою начинку
	void SetMyself(T pValue){m_pMyself=pValue;};

	///проверяет узел на листовой
	bool IsLeaf(void){return m_bLeaf;};
	///проверяет узел на корневой
	bool IsRoot(void){return !m_uiDepth?true:false;};

protected:
	///сам
	T m_pMyself;
	///родительский узел
	CUnChildNode<T>* m_pParent;
	///текущая глубина узла
	unsigned int m_uiDepth;
	///флаг листового узла
	bool m_bLeaf;
};

//--------------------------------------------------------------------//

///функциональность узла дерева с коллекцией детей предназначенной для поиска по индексу
template <class T>
class CTreeNode : public CUnChildNode<T>
{
public:
	typedef map<unsigned int, CTreeNode<T>* > TChilds;
	
	CTreeNode(void)
	{
		m_itCurrChild = m_NullCollection.end();
	}
	CTreeNode(T pMySelf)
		:CUnChildNode<T>(pMySelf)
	{
		m_itCurrChild = m_NullCollection.end();
	}
	
	virtual ~CTreeNode(void)
	{
		DeleteChilds();
	};

	///возвращает коллекцию детей узла
	TChilds* GetChilds(void){return &m_Childs;};
	///возвращает количестов детей узла
	unsigned int GetChildsNumber(void){return m_Childs.size();};

	///возвращает ребенка узла по индексу
	virtual CTreeNode<T>* GetChild(unsigned int uiChildID)
	{
		TChilds::iterator itChild=m_Childs.find(uiChildID);
		if(itChild!=m_Childs.end()){
			return itChild->second;
		}else{
			return NULL;
		}
	};

	///добавляет ребенка данному узлу
	virtual CTreeNode<T>* AddChild(unsigned int uiChildID, CTreeNode<T>* pChildNode)
	{
		if(ISNULL(pChildNode)) return NULL;

		TChilds::iterator itChild=m_Childs.find(uiChildID);
		if(itChild==m_Childs.end()){
			pChildNode->SetParent(this);
			pChildNode->SetDepth(GetDepth()+1);
			m_Childs.insert(TChilds::value_type(uiChildID, pChildNode));
			m_bLeaf=false;
			return pChildNode;
		}else{
			return itChild->second;
		}
	};

	///удаляет детей у узла
	void DeleteChilds(void)
	{
		TChilds::iterator itChild=m_Childs.begin();
		while(itChild!=m_Childs.end()){
			if(itChild->second){
				delete itChild->second;
			}
			itChild++;
		}
		m_Childs.clear();
		m_bLeaf=true;
	};

	///удаляет ребенка у узла
	void DeleteChild(unsigned int uiChildID)
	{
		TChilds::iterator itChild=m_Childs.find(uiChildID);
		if(itChild!=m_Childs.end()){
			if(itChild->second) delete itChild->second;

			m_Childs.erase(itChild);

			if(m_Childs.empty()) m_bLeaf=true;
		}
	};
	
	///делает текущим первого ребенка
	bool MoveFirstChild(void)
	{
		if(m_Childs.empty()){
			m_itCurrChild=m_NullCollection.end();
			return false;
		}else{
			m_itCurrChild=m_Childs.begin();
			return true;
		}
	};

	///делает текущим следующего ребенка
	bool MoveNextChild(void)
	{
		if(m_itCurrChild == m_NullCollection.end()) return false;

		if(++m_itCurrChild!=m_Childs.end()){
			return true;
		}else{
			m_itCurrChild=m_NullCollection.end();
			return false;
		}

	};

	///возвращает текущего ребенка
	CTreeNode<T>* GetCurrentChild(void)
	{
		if(m_itCurrChild==m_NullCollection.end()) return NULL;
		return m_itCurrChild->second;
	};	

	///удаляет текущего ребенка
	void DeleteCurrentChild(void)
	{
		if(m_itCurrChild == m_NullCollection.end()) return;
		
		m_itCurrChild=m_Childs.erase(m_itCurrChild);
		
		if(m_itCurrChild==m_Childs.end()) m_itCurrChild==m_NullCollection.end();

		if(m_Childs.empty()) m_bLeaf=true;
	};	

protected:
	///коллекция детей
	TChilds m_Childs;
	///текущий ребенок 
	typename CTreeNode<T>::TChilds::iterator m_itCurrChild;
	///коллекция для инициализации итераторов
	TChilds m_NullCollection;
};

//--------------------------------------------------------------------//

///узел бинарного дерева
template <class T>
class CBinTreeNode : public CUnChildNode<T>
{
public:
	CBinTreeNode(void)
		:m_pLeftChild(NULL), m_pRightChild(NULL)
	{};
	CBinTreeNode(T pMySelf)
		:CUnChildNode<T>(pMySelf), m_pLeftChild(NULL), m_pRightChild(NULL)
	{};

	virtual ~CBinTreeNode(void)
	{
		DeleteChilds();
	};

	///добавляет ребенка данному узлу
	CBinTreeNode<T>* AddChild(CBinTreeNode<T>* pChildNode, bool bLeft)
	{
		if(ISNULL(pChildNode)) return NULL;
		if(bLeft){
			return AddChild(&m_pLeftChild, pChildNode);
		}else{
			return AddChild(&m_pRightChild, pChildNode);
		}
	};

	///добавляет ребенка данному узлу
	CBinTreeNode<T>* AddChild(CBinTreeNode<T>* pChildNode)
	{
		if(ISNULL(pChildNode)) return NULL;
		if(!m_pLeftChild){
			//если можно добавляем левым ребенком
			return AddChild(&m_pLeftChild, pChildNode);
		}
		if(!m_pRightChild){
			//если можно добавляем правым ребенком
			return AddChild(&m_pRightChild, pChildNode);
		}

		return NULL;
	};

	///проверяет что дети узла уже забиты
	bool ChildsOccupied(void){return (m_pLeftChild && m_pRightChild)?true:false;};
	///убирает у узла детей без удаления
	void RemoveChilds(void){m_pLeftChild=NULL; m_pRightChild=NULL;};
	///убирает у узла детей без удаления
	void DeleteChilds(void)
	{
		if(m_pLeftChild){
			delete m_pLeftChild;
			m_pLeftChild=NULL; 
		}
		
		if(m_pRightChild){
			delete m_pRightChild;
			m_pRightChild=NULL;
		}
	};
	///убирает у узла левого ребенка без удаления
	void RemoveLeftChild(void){m_pLeftChild=NULL;};
	///убирает у узла правого ребенка без удаления
	void RemoveRightChild(void){m_pRightChild=NULL;};
	///возвращает левого ребенка
	virtual CBinTreeNode<T>* GetLeftChild(void){return m_pLeftChild;};
	///возвращает правого ребенка
	virtual CBinTreeNode<T>* GetRightChild(void){return m_pRightChild;};
	///возвращает родительский узел
	CBinTreeNode<T>* GetParent(void){return (CBinTreeNode<T>*)CUnChildNode<T>::GetParent();};

private:
	///добавляет ребенка данному узлу
	CBinTreeNode<T>* AddChild(CBinTreeNode<T>** m_ppChildContainer, CBinTreeNode<T>* pChildNode)
	{
		if(!(*m_ppChildContainer)){
			m_bLeaf=false;
			(*m_ppChildContainer)=pChildNode;
			(*m_ppChildContainer)->SetDepth(m_uiDepth+1);
			(*m_ppChildContainer)->SetParent(this);
		}else{
			TO_CONSOLE(L"Child already exist!");
		}

		return (*m_ppChildContainer);
	};

protected:
	///левый ребенок
	CBinTreeNode<T>* m_pLeftChild;
	///правый ребенок
	CBinTreeNode<T>* m_pRightChild;
};

//--------------------------------------------------------------------//

///функциональность узла обычного дерева с векторной коллекцией детей
template <class T>
class CVecTreeNode : public CUnChildNode<T>
{
public:
	typedef vector< CVecTreeNode<T>* > TChilds;
	
	CVecTreeNode(void)
	{
		m_itCurrChild = m_NullCollection.end();
	}
	CVecTreeNode(T pMySelf)
		:CUnChildNode<T>(pMySelf)
	{
		m_itCurrChild = m_NullCollection.end();
	}
	
	virtual ~CVecTreeNode(void)
	{
		DeleteChilds();
	};

	///возвращает глубину узла
	TChilds* GetChilds(void){return &m_Childs;};

	///добавляет ребенка данному узлу
	virtual CVecTreeNode<T>* AddChild(CVecTreeNode<T>* pChildNode)
	{
		if(ISNULL(pChildNode)) return NULL;

		pChildNode->SetParent(this);
		pChildNode->SetDepth(GetDepth()+1);
		m_Childs.push_back(pChildNode);
		m_bLeaf=false;
		return pChildNode;
	};

	///удаляет детей у узла
	void DeleteChilds(void)
	{
		TChilds::iterator itChild=m_Childs.begin();
		while(itChild!=m_Childs.end()){
			if(*itChild){
				delete *itChild;
			}
			itChild++;
		}
		m_bLeaf=true;
	};

	///делает текущим первого ребенка
	bool MoveFirstChild(void)
	{
		if(m_Childs.empty()){
			m_itCurrChild=m_NullCollection.end();
			return false;
		}else{
			m_itCurrChild=m_Childs.begin();
			return true;
		}
	};

	///делает текущим следующего ребенка
	bool MoveNextChild(void)
	{
		if(m_itCurrChild == m_NullCollection.end()) return false;

		if(++m_itCurrChild!=m_Childs.end()){
			return true;
		}else{
			m_itCurrChild=m_NullCollection.end();
			return false;
		}

	};

	///возвращает текущего ребенка
	CVecTreeNode<T>* GetCurrentChild(void)
	{
		if(m_itCurrChild == m_NullCollection.end()) return NULL;
		return *m_itCurrChild;
	};	

protected:
	///коллекция детей
	TChilds m_Childs;
	///текущий ребенок 
	typename CVecTreeNode<T>::TChilds::iterator m_itCurrChild;
	///коллекция для инициализации итераторов
	TChilds m_NullCollection;
};

//--------------------------------------------------------------------//

}
}
}
}
}