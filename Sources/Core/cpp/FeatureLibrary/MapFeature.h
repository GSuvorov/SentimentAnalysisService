#pragma once
#include "basefeature.h"
#include "XMLReader.h"



#pragma warning (disable : 4267)
namespace SS
{
	namespace Core
	{
		namespace Features
		{
using namespace SS::Core::Features::Base;
			namespace Base
			{
				/// характеристика, предназначенная для хранения "мап"-характеристик (аналог сета (энумератора, могущего принимать сразу несколько значений))
				template <
					/// размер в битах поля, предназначенного для хранения сингл-значения в словаре
					unsigned char nBitSize
					/// общее количество принимаемых характеристикой различных значений
					, unsigned char nValues
					/// тип класса, отвечающего за хранение информации о наборе (кол-во, имена значений, строковые описания)
					, class TType> class CMapFeature :
					public CBaseFeature
 				{
					static unsigned int m_MaxValue;
				public:
					CMapFeature()
					{
						Undefine();
					};

					CMapFeature(
						///номер значения сета(номер бита), котрый ставится в 1
						const unsigned char value)
					{
						SetSingleValue(value);
						//unsigned int m_MaxValue(1 << nValues);

						//if(value < m_MaxValue){
						//	m_value = value;
						//}
						//else{
						//	char bff1[20];
						//	char bff2[20];
						//	TType oType;
						//	
						//	SS_THROW((char*)oType.GetTypeName() AND " конструктор Value больше допустимого" AND "MaxMapValue = " AND ltoa(m_MaxValue, bff1, 10) AND " value = " AND ltoa(value, bff2, 10));
						//}
					};

					CMapFeature(
						///значение такого-же типа, которое будет присвоено этой характеристике
						const CMapFeature<nBitSize, nValues, TType>& value)
					{
						unsigned int  m_MaxValue((unsigned int)1 << nValues);
						if(value.m_value < m_MaxValue || m_MaxValue==0)
						{
							m_value = value.m_value;
						}
						else
						{
							wchar_t wcBuff1TypeName[100];
							wchar_t bff1[20];
							wchar_t bff2[20];
							TType oType;
							
							mbstowcs(wcBuff1TypeName, (char*)oType.GetTypeName() , strlen(oType.GetTypeName()));
			
							SS_THROW(wcBuff1TypeName AND \
									L" конструктор Value больше допустимого" AND \
									L"MaxMapValue = " AND \
									_ltow_s(m_MaxValue, bff1, 20, 10) AND \
									L" value = " AND \
									_ltow_s(value.m_value, bff2, 20, 10));
						}
					};

					virtual ~CMapFeature(void){};

					///сброс значения характеристики до Undefined
					void Undefine()
					{
						m_value = 0x00000001;
					};

					///вернет тру если хар-ка Undefined
					bool IsUndefined() const
					{
						return (m_value == 0x00000001);
					};

					void FillUp()
					{
						unsigned int m_MaxValue((unsigned int)1 << nValues);
						if(m_MaxValue)
						{
							m_value = m_MaxValue - 1;
						}
						else
						{
							m_value = 0xFFFFFFFF;
						}
					}

					///получение строкового описания хар-ки
					void GetValueDescription(std::string& s) const
					{
						TType oType;
						CXMLReader oReader;
						oReader.GetMapDescription(&oType, m_value, s);
					}

					///получение строкового описания хар-ки
					std::string GetValueDescription() const
					{
						std::string s;
						TType oType;
						CXMLReader oReader;
						oReader.GetMapDescription(&oType, m_value, s);

						return s;
					};


					///получение строкового описания значения хар-ки
					void GetXMLValue(std::string& sXML) const
					{
						TType oType;
						CXMLReader oReader;
						oReader.GetMapXMLValue(&oType, m_value, sXML);
					};

					///установление значения хар-ки по строковому описанию
					void SetXMLValue(const std::string& sXML){
						TType oType;
						CXMLReader oReader;
						m_value = 0;
						oReader.SetMapXMLValue(&oType, &m_value, sXML);
						if(m_value == 0)
							m_value = 1;
					};

					///получение строкового описания значения хар-ки
					void ToString(std::string& s) const
					{
						TType oType;
						CXMLReader oReader;
						oReader.GetMapString(&oType, m_value, s);
					};

					///получение строкового описания значения хар-ки
					std::string ToString() const
					{
						std::string s;
						TType oType;
						CXMLReader oReader;
						oReader.GetMapString(&oType, m_value, s);

						return s;
					};

					/////получение значения характеристики, закодированного в беззнаковом числе
					//unsigned int GetValue() const
					//{
					//	return 0;
					//};
					///установление значения хар-ки по закодированному в числе значению
					void SetValue(const unsigned int uiValue)
					{
						unsigned int m_MaxValue((unsigned int)1 << nValues);
						if(uiValue < m_MaxValue || m_MaxValue==0)
						{
							m_value = uiValue;
						}
						else
						{
							TType oType;

							wchar_t wcBuff1TypeName[100];
							wchar_t wcBuff1[20];
							wchar_t wcBuff2[20];					
							mbstowcs(wcBuff1TypeName, (char*)oType.GetTypeName() , strlen(oType.GetTypeName()));

							SS_THROW( wcBuff1TypeName \
									AND L" SetValue Value больше допустимого" \
									AND L"MaxMapValue = " \
									AND _itow_s(m_MaxValue, wcBuff1, 20, 10) \
									AND L" value = " \
									AND _itow_s(uiValue, wcBuff2, 20, 10) );
							//ДОБАВИТЬ

							SS_THROW(L"");
						}
					};

					///устанавливает значение бита номер ucValue в 1, всех остальных в 0
					/**
						данная функция предназначена для взаимодействия со словарем
						т.е. по значению из словаря устанавливает свое значение
						если номер бита больше максимально допустимого, вызовется эксцепция
					*/
					void SetSingleValue(unsigned char ucValue)
					{
						if(ucValue < nValues)
						{
							m_value = 0;
							SetBit(ucValue, 1);
						}
						else
						{
							m_value = 0;
							SetBit(0, 1);
							/*TType oType;

							wchar_t wcBuff1TypeName[100];
							wchar_t bff1[20];
							wchar_t bff2[20];					
							mbstowcs(wcBuff1TypeName, (char*)oType.GetTypeName() , strlen(oType.GetTypeName()));

							SS_THROW(wcBuff1TypeName AND \
									L" SetSingleValue Value больше допустимого" AND \
									L"nValues = " AND \
									_ltow(nValues, bff1, 10) AND \
									L" value = " AND \
									_ltow(ucValue, bff2, 10));*/
						}
					}

					///вернет значение (номер бита, который == 1), предназначенное для записи в словарь
					/**
						если в 1 стоит более одного бита, вызовется эксцепция
					*/
					unsigned char GetSingleValue() const
					{
						unsigned char i = 0;
						unsigned char ucRet = 0xFF;
						while(i < nValues)
						{
							unsigned char ucBit = GetBit(i);
							if(ucBit)
							{
								if(ucRet == 0xFF){
									ucRet = i;
								}
								else
								{
									TType oType;
									wchar_t wcBuff1TypeName[100];
									//wchar_t bff1[20];
									mbstowcs(wcBuff1TypeName, (char*)oType.GetTypeName() , strlen(oType.GetTypeName()));

									//ДОБАВИТЬ
									/*SS_THROW(wcBuff1TypeName AND \
										L" при попытке получения одного бита выявлено наличие нескольких бит, стоящих в 1, m_value = " AND \
										_ltow(m_value, bff1, 10));*/
								}
							}
							i++;
						}

						if(ucRet == 0xFF)
						{
							TType oType;

							wchar_t wcBuff1TypeName[100];
							wchar_t bff1[20];
							mbstowcs(wcBuff1TypeName, (char*)oType.GetTypeName() , strlen(oType.GetTypeName()));
		
							SS_THROW(wcBuff1TypeName AND \
									L" при попытке получения одного бита выявлено отсутствие битов, стоящих в 1, m_value = " AND \
									_ltow_s(m_value, bff1, 20, 10));
							return 0;
						}
						else
						{
							return ucRet;
						}
					}

					/// "оператор == " - сравнивает со значением другого класса такого-же типа
					bool Equal(const CMapFeature<nBitSize, nValues, TType>& Value) const
					{
						return (m_value == Value.m_value);
					}
					/// "оператор = " - присваивает значение другого класса такого-же типа
					void Assign(const CMapFeature<nBitSize, nValues, TType>& Value)
					{
						m_value = Value.m_value;
					};

					///опеределяет - содержит ли этот мап заданный . если результат равен 1 , то считаем, что не содержится, так как значение мапа 1 соответствует - неопределено.
					bool IsContain(const CMapFeature<nBitSize, nValues, TType>& Value) const
					{
						return ((m_value&(Value.m_value))>1);
					};

					/// затирает биты заданные value 
					void Remove(const CMapFeature<nBitSize, nValues, TType>& Value)
					{
						unsigned int temp=~Value.m_value;
						m_value &= temp;
					};

				std::auto_ptr<CBaseType> GetBaseType() const { return std::auto_ptr<CBaseType>(new TType()); }
	         
				///Количество не нулевых битов
				unsigned char GetValuesNumber() const
				{
				unsigned char RetVal = 0;

				for( unsigned int Value = m_value; Value; Value >>= 1 )
					RetVal += Value & 0x1; 

				return RetVal;
				}

				/// размер в битах поля, предназначенного для хранения сингл-значения в словаре
				static unsigned char GetBitSize() { return nBitSize; }

				/// общее количество принимаемых характеристикой различных значений
				static unsigned char GetValesCount() { return nValues; }

				protected:
					///вернет значение бита под номером n
					unsigned char GetBit(unsigned int n) const
					{
						return ((m_value >> n) & 0x00000001);
					};

					///установит значение бита под номером n в ucBit (0 или 1)
					void SetBit(unsigned int n, unsigned char ucBit)
					{
						unsigned int i = 0x00000001;
						i = i << n;
						if(ucBit)
						{
							m_value |= i;
						}
						else
						{
							i = ~i;
							m_value &= i;
						}
					};

				};


				//Класс предоставляет интерфейс перебора всех значений CMapFeature::GetSingleValue в
				//том случаее если этих значений более одного (CMapFeature::GetSingleValue в таком случае 
				//бросит исключение)
				class TMapFeatureSingleValueIterator
				{
					unsigned int m_Mask;
					unsigned char m_SingleValue;
		         
				public:
					TMapFeatureSingleValueIterator(): m_Mask(0), m_SingleValue(0) {}
					explicit TMapFeatureSingleValueIterator( const CBaseFeature &BaseFeature ) { Init(BaseFeature); }
		            
					//Инициализация перед началом перебора.
					//Изменения BaseFeature после функции Init не синхронизируются  
					void Init( const CBaseFeature &BaseFeature )
					{
					m_Mask = BaseFeature.GetValue(); 
					m_SingleValue = 0;

					Next();
					}

					//Перейти к следующему значению
					void Next()
					{
					//При первом вызове пропускаем значение Undefined
					do
					{
						m_Mask >>= 1; 
						++m_SingleValue;
					}
					while( m_Mask && !(m_Mask & 0x1) ); //Пока есть единицы и пока младший бит не единица
					}

					//Закончились ли значения
					bool IsEnd() const { return !m_Mask; }

					//Текущее значение
					unsigned char GetSingleValue() const { return m_SingleValue; }

					//Утановить значение BaseFeature, так чтобы 
					//CMapFeature::GetSingleValue() == TMapFeatureSingleValueIterator::GetSingleValue()
					void SetValueTo( CBaseFeature &BaseFeature ) const { BaseFeature.SetValue( 0x1 << GetSingleValue() ); } 
				};
			}
		}
	}
}
#pragma warning (default : 4267)
