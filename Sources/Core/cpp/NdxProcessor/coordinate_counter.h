#pragma once

#include "..\ASDInterface\INdxDataStorage.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Indexing
{

//--------------------------------------------------------------------//

///счетчик координат
class CCoordinateCounter
{
public:
	CCoordinateCounter(void);

	///сбрасывает счетчик
	void Reset(void);
	///инкрементирует координаты определнного типа
	void Increment(SS::Interface::Core::NdxSE::NdxDataStorage::SNdxLevelInfo::EIndexCoordinateType CoordinateType, unsigned int uiStep = 1);
	///возвращает координаты определнного типа
	unsigned int GetCoordinate(SS::Interface::Core::NdxSE::NdxDataStorage::SNdxLevelInfo::EIndexCoordinateType CoordinateType);
	///обновляет значение координаты определнного типа, возвращает разницу со старым значением
	int Update(SS::Interface::Core::NdxSE::NdxDataStorage::SNdxLevelInfo::EIndexCoordinateType CoordinateType, unsigned int uiValue);

protected:
	///сбрасывает координаты определнного типа
	void ResetCoordinate(SS::Interface::Core::NdxSE::NdxDataStorage::SNdxLevelInfo::EIndexCoordinateType CoordinateType);
	///сбрасывает подчиненные координаты для определнного типа
	void ResetSubCoordinates(SS::Interface::Core::NdxSE::NdxDataStorage::SNdxLevelInfo::EIndexCoordinateType CoordinateType);

	///количество типов координат с которыми работает счетчик
	static const unsigned int m_uiCoordinatesTypesNumber=7;
	///массив содержит координаты всех типов
	unsigned int m_arCoordinates[m_uiCoordinatesTypesNumber];
	
};

//--------------------------------------------------------------------//

}
}
}
}
}