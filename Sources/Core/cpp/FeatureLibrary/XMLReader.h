#pragma once

#include "BaseType.h"
#include <string>

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			namespace Base
			{
				///КЛАСС, ПРЕДНАЗНАЧЕННЫЙ ДЛЯ ОБСЛУЖИВАНИЯ ЧТЕНИЯ/ЗАПИСИ ХМЛ ТАБЛИЦ ЗНАЧЕНИЙ
				class CXMLReader
				{
				public:
					CXMLReader(void);
					~CXMLReader(void);

					///получение строкового описания значения хар-ки
					void GetMapXMLValue(CBaseType* pBaseType, unsigned int value, std::string& sXML) const;
					///установление значения хар-ки по строковому описанию
					void SetMapXMLValue(CBaseType* pBaseType, unsigned int* p_value, const std::string& sXML);

					///получение строкового описания хар-ки
					void GetMapString(CBaseType* pBaseType, unsigned int value, std::string& s) const;

					///получение строкового описания значения характеристики
					void GetMapDescription(CBaseType* pBaseType, unsigned int value, std::string& s) const;

					///получение строкового описания значения хар-ки
					void GetIntegerXMLValue(CBaseType* pBaseType, unsigned int value, std::string& sXML) const;
					///установление значения хар-ки по строковому описанию
					void SetIntegerXMLValue(CBaseType* pBaseType, unsigned int* p_value, const std::string& sXML);
				protected:

					///вернет строку - заголовок типа
					void AppendTypeHeader(CBaseType* pBaseType, std::string& s) const;
					///вернет строку - закрывающую тип
					void AppendTypeEnd(CBaseType* pBaseType, std::string& s) const;

					///вернет строку - заголовок значения под номером n
					void AppendValueHeader(unsigned int n, CBaseType* pBaseType, std::string& s) const;
					///вернет строку - закрывающую значение под номером n
					void AppendValueEnd(unsigned int n, CBaseType* pBaseType, std::string& s) const;
					///вернет подстроку строди ХМЛ, которая описывает значения данной характеристики
					void GetTypeXMLRegion(CBaseType* pBaseType, const std::string& sXML, std::string& s);
					///вернет значение бита, полученное из ХМЛ строки
					unsigned int GetValueXMLValue(unsigned int n, CBaseType* pBaseType, std::string& sXML) const;

					///вернет значение бита под номером n
					unsigned char GetBit(unsigned int value, unsigned int n) const;

					///установит значение бита под номером n
					void SetBit(unsigned int* p_value, unsigned int n, unsigned char ucBit);
				};
			}
		}
	}
}