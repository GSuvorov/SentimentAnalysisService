#pragma once

#include "..\ASSInterface\IBlackBox.h"
using namespace SS::Interface::Core::BlackBox;
using namespace SS::Interface::Core::BlackBox::Query;

#include "..\FeatureLibrary\define.h"
using namespace SS::Core::Features;


namespace SS
{
namespace TransformGenerator
{

/// Класс обеспечивающий функциональность по созданию и клонированию объектов типа IQueryUnitIndexCollection
class CQueryUnitIndexCollection
{
public:
	CQueryUnitIndexCollection(IBlackBoxQueryStorage* pBlackBoxQueryStorage, 
		IQueryUnitIndexCollection* pColl = NULL);
	~CQueryUnitIndexCollection();

public:
	/// Создает новую коллекцию индексов по типу и весу
	bool Create(EIndexType type, float flWeight);
	/// Создает новую коллекцию индексов и копирует в нее содержимое m_pColl
	IQueryUnitIndexCollection* Clone();
	operator IQueryUnitIndexCollection*();
	void Release();
	bool IsEmpty();

private:
	IQueryUnitIndexCollection* m_pColl;
	IBlackBoxQueryStorage* m_pBlackBoxQueryStorage;
};

}
}
