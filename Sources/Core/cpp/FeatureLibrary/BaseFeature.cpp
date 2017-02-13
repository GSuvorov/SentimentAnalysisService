#include "StdAfx.h"
#include ".\basefeature.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			namespace Base
			{
				CBaseFeature::CBaseFeature(void) : m_value(0)
				{
				}

				CBaseFeature::~CBaseFeature(void)
				{
				}

				/// сбрасывет значение своих переменных
				void CBaseFeature::Undefine()
				{
					m_value = 0;
				}

				///вернет тру если хар-ка Undefined
				bool CBaseFeature::IsUndefined() const
				{
					return (m_value == 0);
				};


				///получение значения характеристики, закодированного в беззнаковом числе
				unsigned int CBaseFeature::GetValue() const
				{
					return m_value;
				}
				///установление значения хар-ки по закодированному в числе значению
				void CBaseFeature::SetValue(const unsigned int uiValue)
				{
					m_value = uiValue;
				}

				/// "оператор = " - присваивает значение другого класса такого-же типа
				void CBaseFeature::Assign(const CBaseFeature* pValue)
				{
					m_value = pValue->m_value;
				}

				/// "оператор == " - сравнивает со значением другого класса такого-же типа
				bool CBaseFeature::Equal(const CBaseFeature* pValue) const
				{
					return (m_value == pValue->m_value);
				}
				/// "оператор &= " - присваивает значение другого класса такого-же типа
				void CBaseFeature::AssignAND(const CBaseFeature* pValue, bool withoutUndef/* = false*/)
				{
					if (withoutUndef && IsUndefined())
						Assign(pValue);
					else
						m_value &= pValue->m_value;
				}
				/// "оператор |= " - присваивает значение другого класса такого-же типа
				void CBaseFeature::AssignOR(const CBaseFeature* pValue, bool firstValue/* = false*/)
				{
                    if(!firstValue)
					    m_value |= pValue->m_value;
                    else
                        Assign(pValue);
				}

				/// математическое включение характеристик (включает ли данная характеристика значения переданной характеристики)
				bool CBaseFeature::IsIncludes(const CBaseFeature* pValue) const
				{
					return ((m_value & pValue->m_value) == pValue->m_value);
				}

				/// математическое пересечение характеристик (есть ли хоть одно совпадающее значение у двух характеристик)
				bool CBaseFeature::IsIntersectsWith(const CBaseFeature* pValue) const
				{
					return ((m_value & pValue->m_value) != 0);
				}

				bool CBaseFeature::IsIntersectsWithout(const CBaseFeature* pValue) const
				{
					return (((m_value>>1) & (pValue->m_value>>1)) != 0);
				}

				/// пересечение характеристик
				bool CBaseFeature::IsIntersectsOrEmpty(const CBaseFeature* pValue) const
				{
					if( m_value == 1 )
						return true;
					return ((m_value & pValue->m_value) != 0);
				}

				std::string CBaseFeature::GetPrintableDataDescription() const
				{
					std::string Out;
					unsigned int Value = GetValue();
					std::auto_ptr<CBaseType> pType(GetBaseType());
					int n = 0;

					//APL_ASSERT( pType.get() );

					if( !pType->GetValueCount() )
					{
					//Это IntegerFeature
					if( Value != 0 )
					{
						std::string Tmp;
						ToString(Tmp);
		                                    
						Out += pType->GetValueName(Value);
						Out += ": ";
						Out += Tmp;
						++n;
					}
					}
					else
					{
					for( unsigned int i = 1; i < pType->GetValueCount(); ++i )
					{
						if( Value & (1 << i) )
						{
							if( n != 0 ) Out += ", ";
							Out += pType->GetValueName(i);
							++n;
						}
					}
					}


					if( n > 0 ) Out += ';';

					return Out;
				}
			}
		}
	}
}