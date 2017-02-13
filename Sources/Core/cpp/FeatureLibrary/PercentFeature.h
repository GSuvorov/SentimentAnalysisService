#pragma once

#include "IntegerFeature.h"
namespace SS
{
	namespace Core
	{
		namespace Features
		{
			namespace Base
			{
				/// характеристика типа "процент", принимает значения от 0 до 100
				template <class TType> class CPercentFeature :
					public CIntegerFeature<TType>
				{
				public:
					CPercentFeature()
					{
						SetValue(0);
					};

					CPercentFeature(const unsigned int value)
					{
						SetValue(value);
					};
					///установление значения хар-ки по закодированному в числе значению
					void SetValue(const unsigned int uiValue){
						if(uiValue <= 100){
							m_value = uiValue;
						}
						else{
							SS_THROW(L"недопустимо значение силы большее 100");
						}
					}

					/// "оператор = " - присваивает значение другого класса такого-же типа
					void Assign(const CBaseFeature* pValue){
						SetValue(pValue->GetValue());
					};

					/// "оператор &= " - присваивает значение другого класса такого-же типа
					void AssignAND(const CBaseFeature* pValue){SS_THROW(L"недопустимая операция");};
					/// "оператор |= " - присваивает значение другого класса такого-же типа
					void AssignOR(const CBaseFeature* pValue){SS_THROW(L"недопустимая операция");};

					/// математическое включение характеристик (включает ли данная характеристика значения переданной характеристики)
					bool IsIncludes(const CBaseFeature* pValue) const {
						return Equal(pValue);
					};

					/// математическое пересечение характеристик (есть ли хоть одно совпадающее значение у двух характеристик)
					bool IsIntersectsWith(const CBaseFeature* pValue) const {
						return Equal(pValue);
					};
				};
			}
		}
	}
}
