#pragma once

#include ".\basis_containers.h"

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

///структура для накопления данных при вычислениях в модели нечётких множеств
struct CAccumulator : public SS::Core::NdxSE::NdxProcessor::Containers::CValue
{
public:
	CAccumulator(unsigned int uiNumber=0);
	CAccumulator& operator=(const float fVal);

	///инициализация с числом элементов uiNumber
	void Init(int uiNumber=0);
	///проверка на пустоту структур
	bool isEmpty();
	///сливание аккумуляторов
	bool MergeAccumulators(CAccumulator& oAcc);
	///вычисление для операции ОR
	bool EvalA4ORFromAccumulator(float fBeta);
	///вычисление для операции AND
	bool EvalA4ANDFromAccumulator(float fBeta);

	///сериализация в строку
	void ToString(wstring& szValue);
	///отображает содержимое объекта класса
	void View(void);

	///здесь накапливается максимальноые значения элементов
	std::vector<float> m_vfMax;
	///здесь накапливается минимальные значения элементов
	std::vector<float> m_vfMin;
	///здесь накапливается средния значения элементов
	std::vector<float> m_vfMean;
	///число элементов
	unsigned int m_uiNumberOfElements;
	///число накопленных значений элементов (значащих)
	unsigned int m_uinMean;
	///флаг пустоты 
	bool m_isEmpty;

};

//--------------------------------------------------------------------//

}
}
}
}
}