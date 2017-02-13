#pragma once
#include <string>
#include <memory>
#include "BaseType.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			namespace Base
			{
				/// базовый класс для всех наборов характеристик. содержит также эталонную(математическую) реализацию базовых операций над значениями характеристик
				class CBaseFeature
				{
				public:
					CBaseFeature(void);
					virtual ~CBaseFeature(void);

					/// сбрасывет значение своих переменных
					virtual	void Undefine();
					///вернет тру если хар-ка Undefined
					virtual bool IsUndefined() const;

					///получение строкового описания значения хар-ки
					virtual void GetXMLValue(std::string& sXML) const = 0;
					///установление значения хар-ки по строковому описанию
					virtual void SetXMLValue(const std::string& sXML) = 0;

					///получение строкового описания значения хар-ки
					virtual void ToString(std::string& s) const = 0;

					///получение значения характеристики, закодированного в беззнаковом числе
					virtual unsigned int GetValue() const;
					///установление значения хар-ки по закодированному в числе значению
					virtual void SetValue(const unsigned int uiValue);

					/// "оператор = " - присваивает значение другого класса такого-же типа
					virtual void Assign(const CBaseFeature* pValue);

					/// "оператор == " - сравнивает со значением другого класса такого-же типа
					virtual bool Equal(const CBaseFeature* pValue) const;
					/// "оператор &= " - присваивает значение другого класса такого-же типа
					virtual void AssignAND(const CBaseFeature* pValue, bool withoutUndef = false);
					/// "оператор |= " - присваивает значение другого класса такого-же типа
					virtual void AssignOR(const CBaseFeature* pValue, bool firstValue = false);

					/// математическое включение характеристик (включает ли данная характеристика значения переданной характеристики)
					virtual bool IsIncludes(const CBaseFeature* pValue) const;

					/// математическое пересечение характеристик (есть ли хоть одно совпадающее значение у двух характеристик)
					virtual bool IsIntersectsWith(const CBaseFeature* pValue) const;

					virtual bool IsIntersectsWithout(const CBaseFeature* pValue) const;

					/// пересечение характеристик
					virtual bool IsIntersectsOrEmpty(const CBaseFeature* pValue) const;

					/// Информация о типе (создаётся новый экземпляр CBaseType)
					virtual std::auto_ptr<CBaseType> GetBaseType() const = 0; 

					//Описание хранимых данных. Печатная версия (с минимумом лишней информации)
					std::string GetPrintableDataDescription() const;
					
				protected:
					unsigned int m_value;
				};
			}
		}
	}
}