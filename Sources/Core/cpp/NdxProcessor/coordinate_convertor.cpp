#include "StdAfx.h"
#include ".\coordinate_convertor.h"
#include ".\console.h"

//--------------------------------------------------------------------//

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{

void CCoordinateConvertor::InsertToCoordinate(unsigned int* puiCoordinate, unsigned int uiValue, unsigned char ucStartBit, unsigned char ucBitPerLevel)
{
	if(ISNULL(puiCoordinate)) return;

	//определяем маску уровня
	unsigned int uiLevelMask=0xffffffff>>(32-ucBitPerLevel);
	
	//сдвигаем маску в нужное место
	uiLevelMask=uiLevelMask<<((ucStartBit+1)-ucBitPerLevel);
	uiLevelMask=~uiLevelMask;

	//сдвигаем значение в нужное место
	uiValue=uiValue<<((ucStartBit+1)-ucBitPerLevel);

	//сливаем по И с маской
	(*puiCoordinate)&=uiLevelMask; 
	//сливаем по ИЛИ со значением
	(*puiCoordinate)|=uiValue; 
}

unsigned int CCoordinateConvertor::SplitCoordinate(unsigned int uiCoordinate, 
														unsigned char ucStartBit, 
														unsigned char ucBitPerLevel) 
{
	//определяем маску уровня
	unsigned int uiLevelMask=0xffffffff>>(32-ucBitPerLevel);
	
	//сдвигаем маску в нужное место
	uiLevelMask=uiLevelMask<<((ucStartBit+1)-ucBitPerLevel);
	//сливаем по И с координатой
	uiCoordinate&=uiLevelMask; 
	//сдвигаем обратно
	uiCoordinate=uiCoordinate>>((ucStartBit+1)-ucBitPerLevel);

	//возвращаем вытащенное значение 
	return uiCoordinate;
}

//--------------------------------------------------------------------//

}
}
}
}


