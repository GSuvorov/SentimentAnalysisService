#pragma once

#include ".\index_container_nodes_s.h"
#include ".\arg_complex.h"

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

///эталон с коллекцией дочерних эталонов
template <class TChild>
class CMasterCollection
{
public:
	typedef vector<TChild*> TChildCollection;

	CMasterCollection(unsigned int uiID)
		:m_cuiCollectionID(uiID), m_uiValidChildsNumber(0)
	{
	}

	virtual ~CMasterCollection(void){Reset();};

	///добавляет и  возвращает элемент дочерней коллекции по идентификатору
	TChild* AddChildElement(unsigned int uiChildID, TChild* pChild)
	{
		if(uiChildID<m_ChildCollection.size()){
			TChild* pTempChild=m_ChildCollection[uiChildID];

			if(!pTempChild){
				m_ChildCollection[uiChildID]=pChild;
				m_uiValidChildsNumber++;
			}else{
				TO_CONSOLE(L"Child already exist!!!");
				return pTempChild;
			}

			return pChild;
		}else{
			//дополняем коллекцию до нужного размера
			TChild* pTempChild=NULL;
			while(uiChildID>=m_ChildCollection.size()){
				m_ChildCollection.push_back(pTempChild);
			}
			
			//пытаемся снова добавить 
			return AddChildElement(uiChildID, pChild);
		}
	}

	///возвращает элемент дочерней коллекции по идентификатору
	TChild* GetChildElement(unsigned int uiChildID)
	{
		if(uiChildID<m_ChildCollection.size()){
			return m_ChildCollection[uiChildID];
		}else{
			TO_CONSOLE(L"Too big uiChildID");
			ToConsole(L"uiChildID", (unsigned int)uiChildID);
			return NULL;
		}
	}

	///очищает дочернюю коллекцию эталонов
	void Reset(void)
	{
		for(TChildCollection::iterator itChild=m_ChildCollection.begin(); itChild!=m_ChildCollection.end(); itChild++){
			if((*itChild)){
				delete (*itChild);
				(*itChild)=NULL;
			}
		}
	}

	///очищает дочернюю коллекцию эталонов
	virtual void Clear(void)
	{
		for(TChildCollection::iterator itChild=m_ChildCollection.begin(); itChild!=m_ChildCollection.end(); itChild++){
			if((*itChild)){
				(*itChild)->Clear();
			}
		}
	}
	
	///добавляет позицию к эталонным предложениям
	void AddArgumentPositionByID(unsigned int uiID, 
		SS::Core::NdxSE::NdxProcessor::Extracting::CComplexArgument* pComplexArgument, 
		CCoordinateNode_4s* pCoordinateNode)
	{
		if(ISNULL(pComplexArgument)) return;

		TChild* pChild=GetChildElement(uiID);

		if(pChild){
			pChild->AddArgumentPosition(pComplexArgument, pCoordinateNode);
		}else{
			TO_CONSOLE(L"There are no such pChild");
			ToConsole(L"uiID", uiID);
		}
	}

	///добавляет позицию к эталонным предложениям
	virtual void AddArgumentPosition(SS::Core::NdxSE::NdxProcessor::Extracting::CComplexArgument* pComplexArgument, 
		CCoordinateNode_4s* pCoordinateNode){TO_CONSOLE(L"Not ovveriden");};

	///возвращает идентификатор эталонного предложения
	unsigned int GetCollectionID(void){return m_cuiCollectionID;};
	///возвращает количество валидных элементов дочерней колекции
	unsigned int GetValidChildsNumber(void){return m_uiValidChildsNumber;};

protected:
	///идентификатор коллекции
	const unsigned int m_cuiCollectionID;
	///коллекция дочерних эталонов
	TChildCollection m_ChildCollection;
	///количество валидных элементов дочерней колекции
	unsigned int m_uiValidChildsNumber;
};

//--------------------------------------------------------------------//

}
}
}
}
}