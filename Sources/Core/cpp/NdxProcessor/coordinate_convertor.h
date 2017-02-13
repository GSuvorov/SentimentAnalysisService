#pragma once

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{

//--------------------------------------------------------------------//

///переводит древовидные координаты в линейные
class CCoordinateConvertor
{
public:
	///вставляет значение в число в нужное место по битам
	static void InsertToCoordinate(unsigned int* puiCoordinate, unsigned int uiValue, unsigned char ucStartBit, unsigned char ucBitPerLevel);
	///достает значение из числа из нужного места по битам
	static unsigned int SplitCoordinate(unsigned int uiCoordinate, unsigned char ucStartBit, unsigned char ucBitPerLevel);
};

//--------------------------------------------------------------------//

}
}
}
}
