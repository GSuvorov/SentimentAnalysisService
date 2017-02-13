#pragma once

#include "basefeature.h"
#include "XMLReader.h"
namespace SS
{
	namespace Core
	{
		namespace Features
		{
			namespace Base
			{
				/// характеристика, способная хранить обычное unsigned int значение
				template <class TType> class CIntegerFeature :
					public CBaseFeature
				{
				public:
					CIntegerFeature(void)
					{
						m_value = 0;
					};

					CIntegerFeature(const unsigned int value)
					{
						m_value = value;
					};

					~CIntegerFeature(void)
					{
					};

					//оператор приведения к чару
					operator const unsigned int( ) const 
					{
						return m_value;
					}

					///получение строкового описания значения хар-ки
					void GetXMLValue(std::string& sXML) const
					{
						TType oType;
						CXMLReader oReader;
						oReader.GetIntegerXMLValue(&oType, m_value, sXML);
					};

					///установление значения хар-ки по строковому описанию
					void SetXMLValue(const std::string& sXML){
						TType oType;
						CXMLReader oReader;
						oReader.SetIntegerXMLValue(&oType, &m_value, sXML);
					};

					///получение строкового описания значения хар-ки
					void ToString(std::string& s) const
					{
						char bff[20];
						s = _ltoa_s(m_value, bff, 20, 10);
					};

				std::auto_ptr<CBaseType> GetBaseType() const { return std::auto_ptr<CBaseType>(new TType()); }
				};
			}
		}
	}
}