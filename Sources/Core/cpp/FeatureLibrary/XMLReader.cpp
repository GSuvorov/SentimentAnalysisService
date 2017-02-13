#include "StdAfx.h"
#include ".\xmlreader.h"
#pragma warning (disable : 4267)


namespace SS
{
	namespace Core
	{
		namespace Features
		{
			namespace Base
			{
				CXMLReader::CXMLReader(void)
				{
				}

				CXMLReader::~CXMLReader(void)
				{
				}

				///получение строкового описани€ значени€ хар-ки
				void CXMLReader::GetIntegerXMLValue(CBaseType* pBaseType, unsigned int value, std::string& sXML) const
				{
					sXML= "";
					AppendTypeHeader(pBaseType, sXML);
					sXML+= "=\"";
					sXML+= pBaseType->GetTypeDescription();
					sXML+= "\">\n";
						sXML+= "	";
						AppendValueHeader(0, pBaseType, sXML);
						sXML+= "=\"";
						sXML+= pBaseType->GetValueDescription(0);
						sXML+= "\">";
						char bff[20];
						sXML+= ltoa(value, bff, 10);
						AppendValueEnd(0, pBaseType, sXML);
						sXML+= "\n";

					AppendTypeEnd(pBaseType, sXML);
					sXML+= "\n";
				}

				///установление значени€ хар-ки по строковому описанию
				void CXMLReader::SetIntegerXMLValue(CBaseType* pBaseType, unsigned int* p_value, const std::string& sXML)
				{
					////дл€ начала ищем раздел, в котором находитс€ инфа по данному типу
					std::string s = sXML;
					//GetTypeXMLRegion(pBaseType, sXML, s);

					//if(s.empty()){
					//	SS_THROW(L"не найдена данна€ характеристика в ’ћЋ строке: " AND pBaseType->GetTypeName() AND " в с троке " AND sXML.c_str())
					//}

						unsigned int ulValue = GetValueXMLValue(0, pBaseType, s);
						if(ulValue == 0xFFFFFFFF){
							SS_THROW(L"не найдено одно из значений характеристики в ’ћЋ строке: " AND pBaseType->GetTypeName() AND " в с троке " AND sXML.c_str())
						}
						*p_value = ulValue;
				}

				///получение строкового описани€ значени€ хар-ки
				void CXMLReader::GetMapXMLValue(CBaseType* pBaseType, unsigned int value, std::string& sXML) const
				{
					unsigned int nValues = pBaseType->GetValueCount();
					//unsigned int i = 0;
					unsigned int i = 1;
					sXML= "";
					AppendTypeHeader(pBaseType, sXML);
					sXML+= "=\"";
					sXML+= pBaseType->GetTypeDescription();
					sXML+= "\">\n";
					while(i < nValues){
						sXML+= "	";
						AppendValueHeader(i, pBaseType, sXML);
						sXML+= "=\"";
						sXML+= pBaseType->GetValueDescription(i);
						sXML+= "\">";
						sXML+= GetBit(value, i) ? "1" : "0";
						AppendValueEnd(i, pBaseType, sXML);
						sXML+= "\n";
						i++;
					}

					AppendTypeEnd(pBaseType, sXML);
					sXML+= "\n";
				};

				///установление значени€ хар-ки по строковому описанию
				void CXMLReader::SetMapXMLValue(CBaseType* pBaseType, unsigned int* p_value, const std::string& sXML)
				{
					//дл€ начала ищем раздел, в котором находитс€ инфа по данному типу
					std::string s;
					GetTypeXMLRegion(pBaseType, sXML, s);

					if(s.empty()){
						SS_THROW(L"не найдена данна€ характеристика в ’ћЋ строке: " AND pBaseType->GetTypeName() AND " в с троке " AND sXML.c_str())
					}

					unsigned int nValues = pBaseType->GetValueCount();
					unsigned int i = 1;
					//unsigned int i = 0;
					while(i < nValues){
						unsigned char ucBit = GetValueXMLValue(i, pBaseType, s);
						if(ucBit == 0xFF){
							SS_THROW(L"не найдено одно из значений характеристики в ’ћЋ строке: " AND pBaseType->GetTypeName() AND " в с троке " AND sXML.c_str())
						}
						SetBit(p_value, i, ucBit);
						i++;
					}

				};

				///получение строкового описани€ хар-ки
				void CXMLReader::GetMapString(CBaseType* pBaseType, unsigned int value, std::string& s) const
				{
					unsigned int nValues = pBaseType->GetValueCount();
					unsigned int i = 0;
					s = "";

					while(i < nValues){
						if(GetBit(value, i)){
							s+= pBaseType->GetValueName(i);
							s+= " ";
						}
						i++;
					}
				}

				///получение строкового описани€ значени€ хар-ки
				void CXMLReader::GetMapDescription(CBaseType* pBaseType, unsigned int value, std::string& s) const
				{
					unsigned int nValues = pBaseType->GetValueCount();
					unsigned int i = 0;
					s = "";

					while(i < nValues){
						if(GetBit(value, i)){
							s+= pBaseType->GetValueDescription(i);
							s+= " ";
						}
						i++;
					}
				}

				///вернет строку - заголовок типа
				void CXMLReader::AppendTypeHeader(CBaseType* pBaseType, std::string& s) const
				{
					s+= "<";
					s+= pBaseType->GetTypeName();
					s+= " group";
				}
				///вернет строку - закрывающую тип
				void CXMLReader::AppendTypeEnd(CBaseType* pBaseType, std::string& s) const
				{
					s+= "</";
					s+= pBaseType->GetTypeName();
					s+= ">";
				}
				///вернет строку - заголовок значени€ под номером n
				void CXMLReader::AppendValueHeader(unsigned int n, CBaseType* pBaseType, std::string& s) const
				{
					s+= "<";
					s+= pBaseType->GetValueName(n);
					s+= " type";
				}
				///вернет строку - закрывающую значение под номером n
				void CXMLReader::AppendValueEnd(unsigned int n, CBaseType* pBaseType, std::string& s) const
				{
					s+= "</";
					s+= pBaseType->GetValueName(n);
					s+= ">";
				}

				///вернет подстроку строди ’ћЋ, котора€ описывает значени€ данной характеристики
				void CXMLReader::GetTypeXMLRegion(CBaseType* pBaseType, const std::string& sXML, std::string& s)
				{
					s = "";
					std::string sType = "";
					AppendTypeHeader(pBaseType, sType);

					int pos = sXML.find(sType);

					if(pos == - 1) return;
					pos = sXML.find(">", pos);
					if(pos == - 1) return;

					sType.clear();
					AppendTypeEnd(pBaseType, sType);

					int posEnd = sXML.find(sType, pos);
					if(posEnd == - 1) return;

					s = sXML.substr(pos + 1, posEnd - pos - 1);
				};

				///вернет значение бита, полученное из ’ћЋ строки
				unsigned int CXMLReader::GetValueXMLValue(unsigned int n, CBaseType* pBaseType, std::string& sXML) const
				{
					std::string sType = "";
					AppendValueHeader(n, pBaseType, sType);
					int pos = sXML.find(sType);
					if(pos == - 1){
						return 0xFFFFFFFF;
					}
					pos = sXML.find(">", pos);
					if(pos == - 1){
						return 0xFFFFFFFF;
					}

					sType.clear();
					AppendValueEnd(n, pBaseType, sType);

					int posEnd = sXML.find(sType, pos);
					if(posEnd == - 1){
						return 0xFFFFFFFF;
					}

					std::string s;
					s = sXML.substr(pos + 1, posEnd - pos - 1);
					return (unsigned int)atol(s.c_str());
				}

				///вернет значение бита под номером n
				unsigned char CXMLReader::GetBit(unsigned int value, unsigned int n) const
				{
					return ((value >> n) & 0x00000001);
				};

				///установит значение бита под номером n
				void CXMLReader::SetBit(unsigned int* p_value, unsigned int n, unsigned char ucBit)
				{
					unsigned int i = 0x00000001;
					i = i << n;
					if(ucBit){
						(*p_value) |= i;
					}
					else{
						i = ~i;
						(*p_value) &= i;
					}
				};

			}
		}
	}
}
#pragma warning (default : 4267)
