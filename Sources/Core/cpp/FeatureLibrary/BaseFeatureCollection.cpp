#include "StdAfx.h"
#include ".\basefeaturecollection.h"

//#include "../containers/exception.h"

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			namespace Base
			{
				const int allocationBlockSize = 10;
				CBaseFeatureCollection::CBaseFeatureCollection(void)
					: m_FeatureCount(0)
					, m_ppFeatures(NULL)
					, m_pppFeatures(&m_ppFeatures)
					, m_FeatureCollectionType(fctUnknown)
					, m_unusedFreeSpace(0)
				{
					
				}

				CBaseFeatureCollection::~CBaseFeatureCollection(void)
				{
					if(*m_pppFeatures){
						delete [](*m_pppFeatures);
						*m_pppFeatures = NULL;
					}
				}

				void CBaseFeatureCollection::AddFeatureToCollection(CBaseFeature* pFeature)
				{
					m_FeatureCount++;
					if( m_unusedFreeSpace == 0 )
					{
						CBaseFeature** ppFeatures = new CBaseFeature*[m_FeatureCount + allocationBlockSize];
						if(m_ppFeatures){
							memcpy(ppFeatures, m_ppFeatures, (m_FeatureCount - 1) * 4);
						} else {
							memset(ppFeatures, 0, (m_FeatureCount - 1) * 4);
						}
						ppFeatures[m_FeatureCount - 1] = pFeature;
						if(m_ppFeatures){
							delete []m_ppFeatures;
						}
						m_ppFeatures = ppFeatures;
						m_unusedFreeSpace = allocationBlockSize;
					}
					else
					{
						--m_unusedFreeSpace;
						m_ppFeatures[m_FeatureCount - 1] = pFeature;
					}			
				}

				/// сбрасывет значение своих переменных
				void CBaseFeatureCollection::Undefine()
				{
					for(unsigned int i = 0; i < m_FeatureCount; i++)
					{							
						m_ppFeatures[i]->Undefine();
					}
				}

				///вернет тру если хар-ка Undefined
				bool CBaseFeatureCollection::IsUndefined() const
				{
					for(unsigned int i = 0; i < m_FeatureCount; i++)
					{							
						if(!m_ppFeatures[i]->IsUndefined()) return false;
					}
					return true;
				};

				/// возвращает вектор битов, для последующей записи на диск и возможного восстановления информации
				void CBaseFeatureCollection::GetValue(std::vector<unsigned char>& vecOfByte)
				{
	//				SS_TRY;

					int iCurPosition = (int)vecOfByte.size();
					int iNewSize = iCurPosition + (m_FeatureCount*4);
					vecOfByte.resize(iNewSize);
					
					unsigned int iValue = 0;
					
					for(unsigned int i = 0; i < m_FeatureCount; i++, iCurPosition += 4)
					{
						iValue = m_ppFeatures[i]->GetValue();
						memcpy( &vecOfByte[iCurPosition], &iValue, 4);
					}

	//				SS_CATCH(L"");
				}

				///Тестовый для изменения размера SQL базы
				void CBaseFeatureCollection::GetValueTest(std::vector<unsigned char> & vecOfByte)
				{
					int iCurPosition = (int)vecOfByte.size();
					int iNewSize = iCurPosition + (m_FeatureCount*4) + 16;
					vecOfByte.resize(iNewSize);

					unsigned int iValue = 0;
					for(unsigned int i = 0; i < m_FeatureCount; i++, iCurPosition += 4)
					{
						iValue = m_ppFeatures[i]->GetValue();
						memcpy(&vecOfByte[iCurPosition], &iValue, 4);
						//Тестовый код
						//Добавляем 4 знчения каждое по 4 байта - итого 16 байт
						std::string wsTN = m_ppFeatures[i]->GetBaseType()->GetTypeName();
						if (wsTN == "TSVAcquireType")
						{
							iValue = 0;
							for (int j = 0; j < 4; ++j)
							{ 
								iCurPosition += 4;
								memcpy(&vecOfByte[iCurPosition], &iValue, 4);
							}
						}
						//Тестовый код
					}
				}

				///записывает вектор битов начиная с переданной позиции, позволяющий восстановить информацию о классе, смещает текущую позицию на число бит, занимаемое классом
				void CBaseFeatureCollection::SetValue(std::vector<unsigned char>& vecOfByte, unsigned int& uiCurPosition)
				{	
	//				SS_TRY;

					unsigned int uiValue = 0;
					for(unsigned int i = 0; i < m_FeatureCount; i++, uiCurPosition += 4)
					{							
						memcpy( &uiValue, &vecOfByte[uiCurPosition], 4);				
						m_ppFeatures[i]->SetValue(uiValue);

					}

	//				SS_CATCH(L"");
				}	

				///получение строкового описания значения хар-ки
				void CBaseFeatureCollection::GetXMLValue(std::string& sXML) const
				{
					sXML = "<?xml version=\"1.0\" encoding=\"windows-1251\" ?> ";
					AppendFeatureHeader(sXML);
					AppendFeatureValues(sXML);
					AppendFeatureEnd(sXML);
				}
				///установление значения хар-ки по строковому описанию
				void CBaseFeatureCollection::SetXMLValue(const std::string& sXML)
				{
					std::string s;
					GetFeatureXMLRegion(sXML, s);

					if(s.empty())
					{
						int iLen = (int)strlen(GetFeatureName());
						wchar_t * wBuffTypeName = new  wchar_t [iLen + 1]; 
						mbstowcs(wBuffTypeName, GetFeatureName(), iLen + 1);
						iLen = (int)strlen(sXML.c_str());
						wchar_t * wBuffXML = new  wchar_t [iLen + 1]; 
						mbstowcs(wBuffXML, sXML.c_str(), iLen + 1);
						SS_THROW( L"не найдена данная характеристика в ХМЛ строке: " AND 
							wBuffTypeName AND 
							L" в с троке " AND 
							wBuffXML );
						//SS_THROW(L"не найдена данная характеристика в ХМЛ строке: " AND GetFeatureName() AND " в с троке " AND sXML.c_str())
					}

					unsigned int i = 0;
					while(i < m_FeatureCount){
						m_ppFeatures[i]->SetXMLValue(s);
						i++;
					}
				}

				///получение строкового описания значения хар-к
				void CBaseFeatureCollection::ToString(std::string& s) const
				{
					s = "";
					unsigned int i = 0;
					while(i < m_FeatureCount){
						std::string sTemp;
						m_ppFeatures[i]->ToString(sTemp);
						s+= sTemp;
						s+= "/";
						i++;
					}
				}

				///получение строкового описания значения хар-к
				std::string CBaseFeatureCollection::ToString() const
				{
					std::string s = "";
					unsigned int i = 0;
					while(i < m_FeatureCount){
						std::string sTemp;
						m_ppFeatures[i]->ToString(sTemp);
						s+= sTemp;
						s+= "/";
						i++;
					}

					return s;
				}

			std::string CBaseFeatureCollection::GetPrintableDataDescription() const
			{
				std::string Ret;

				for( unsigned int i = 0; i < GetFeatureCount(); ++i )
				{
				//APL_ASSERT( GetFeature(i) );
				Ret += GetFeature(i)->GetPrintableDataDescription();
				}

				return Ret;
			}

				/// "оператор = " - присваивает значение другого класса такого-же типа
				void CBaseFeatureCollection::Assign(const CBaseFeatureCollection* pValue)
				{
					if(m_FeatureCollectionType != pValue->m_FeatureCollectionType){
						wchar_t bff1[20];
						wchar_t bff2[20];
						SS_THROW(L"несовпадение типов характеристик при операции, текущий тип = " AND 
								_ltow(m_FeatureCollectionType, bff1, 10) AND 
								L"; пришедший тип = " AND 
								_ltow(pValue->m_FeatureCollectionType, bff2, 10));
					}

					unsigned int i = 0;
					while(i < m_FeatureCount){
						m_ppFeatures[i]->Assign(pValue->m_ppFeatures[i]);
						i++;
					}
				}

				/// "оператор == " - сравнивает со значением другого класса такого-же типа
				bool CBaseFeatureCollection::Equal(const CBaseFeatureCollection* pValue) const
				{
					if(m_FeatureCollectionType != pValue->m_FeatureCollectionType){
						wchar_t bff1[20];
						wchar_t bff2[20];
						SS_THROW(L"несовпадение типов характеристик при операции, текущий тип = " AND 
								_ltow(m_FeatureCollectionType, bff1, 10) AND 
								L"; пришедший тип = " AND 
								_ltow(pValue->m_FeatureCollectionType, bff2, 10));
					}

					unsigned int i = 0;
					while(i < m_FeatureCount){
						if(m_ppFeatures[i]->Equal(pValue->m_ppFeatures[i]) == false){
							return false;
						}

						i++;
					}
					return true;
				}
				/// "оператор &= " - присваивает значение другого класса такого-же типа
				void CBaseFeatureCollection::AssignAND(const CBaseFeatureCollection* pValue)
				{
					if(m_FeatureCollectionType != pValue->m_FeatureCollectionType){
						wchar_t bff1[20];
						wchar_t bff2[20];
						SS_THROW(L"несовпадение типов характеристик при операции, текущий тип = " AND 
								_ltow(m_FeatureCollectionType, bff1, 10) AND 
								L"; пришедший тип = " AND 
								_ltow(pValue->m_FeatureCollectionType, bff2, 10));
					}

					unsigned int i = 0;
					while(i < m_FeatureCount){
						m_ppFeatures[i]->AssignAND(pValue->m_ppFeatures[i]);
						i++;
					}
				}
				/// "оператор |= " - присваивает значение другого класса такого-же типа
				void CBaseFeatureCollection::AssignOR(const CBaseFeatureCollection* pValue)
				{
					if(m_FeatureCollectionType != pValue->m_FeatureCollectionType){
						wchar_t bff1[20];
						wchar_t bff2[20];
						SS_THROW(L"несовпадение типов характеристик при операции, текущий тип = " AND 
							_ltow(m_FeatureCollectionType, bff1, 10) AND 
							L"; пришедший тип = " AND 
							_ltow(pValue->m_FeatureCollectionType, bff2, 10));
					}

					unsigned int i = 0;
					while(i < m_FeatureCount){
						m_ppFeatures[i]->AssignOR(pValue->m_ppFeatures[i]);
						i++;
					}
				}

				bool CBaseFeatureCollection::IsIncludes(const CBaseFeatureCollection* pValue) const
				{
					if(m_FeatureCollectionType != pValue->m_FeatureCollectionType){
						wchar_t bff1[20];
						wchar_t bff2[20];
						SS_THROW(L"несовпадение типов характеристик при операции, текущий тип = " AND 
							_ltow(m_FeatureCollectionType, bff1, 10) AND 
							L"; пришедший тип = " AND 
							_ltow(pValue->m_FeatureCollectionType, bff2, 10));
					}

					unsigned int i = 0;
					while(i < m_FeatureCount){
						if(m_ppFeatures[i]->IsIncludes(pValue->m_ppFeatures[i]) == false) return false;
						i++;
					}

					return true;
				}

				bool CBaseFeatureCollection::IsIntersectsWith(const CBaseFeatureCollection* pValue) const
				{
					if(m_FeatureCollectionType != pValue->m_FeatureCollectionType){
						wchar_t bff1[20];
						wchar_t bff2[20];
						SS_THROW(L"несовпадение типов характеристик при операции, текущий тип = " AND 
							_ltow(m_FeatureCollectionType, bff1, 10) AND 
							L"; пришедший тип = " AND 
							_ltow(pValue->m_FeatureCollectionType, bff2, 10));
					}

					unsigned int i = 0;
					while(i < m_FeatureCount){
						if(m_ppFeatures[i]->IsIntersectsWith(pValue->m_ppFeatures[i]) == false) return false;
						i++;
					}

					return true;
				}

				bool CBaseFeatureCollection::IsIntersectsOrEmpty(const CBaseFeatureCollection* pValue) const
				{
					if(m_FeatureCollectionType != pValue->m_FeatureCollectionType){
						wchar_t bff1[20];
						wchar_t bff2[20];
						SS_THROW(L"несовпадение типов характеристик при операции, текущий тип = " AND 
							_ltow(m_FeatureCollectionType, bff1, 10) AND 
							L"; пришедший тип = " AND 
							_ltow(pValue->m_FeatureCollectionType, bff2, 10));
					}

					unsigned int i = 0;
					while(i < m_FeatureCount){
						if(m_ppFeatures[i]->IsIntersectsOrEmpty(pValue->m_ppFeatures[i]) == false) return false;
						i++;
					}

					return true;
				}

				///вернет кол-во характеристик, входящих в коллекцию
				unsigned int CBaseFeatureCollection::GetFeatureCount() const
				{
					return m_FeatureCount;
				}
				///вернет характеристику из коллекции по порядковому номеру
				CBaseFeature* CBaseFeatureCollection::GetFeature(unsigned int nFeature)
				{
					if(nFeature < m_FeatureCount){
						return m_ppFeatures[nFeature];
					}
					return NULL;
				}

			const CBaseFeature* CBaseFeatureCollection::GetFeature(unsigned int nFeature) const
			{
				if(nFeature < m_FeatureCount){
				return m_ppFeatures[nFeature];
				}
				return NULL;
			}


				///вернет подстроку строди ХМЛ, которая описывает значения данной характеристики
				void CBaseFeatureCollection::GetFeatureXMLRegion(const std::string& sXML, std::string& s) const
				{
#pragma warning (disable : 4267)
					s = "";
					std::string sType = "";
					AppendFeatureHeader(sType);

					int pos = sXML.find(sType);

					if(pos == - 1) return;
					pos = sXML.find(">", pos);
					if(pos == - 1) return;

					sType.clear();
					AppendFeatureEnd(sType);

					int posEnd = sXML.find(sType, pos);
					if(posEnd == - 1) return;

					s = sXML.substr(pos + 1, posEnd - pos - 1);
#pragma warning (default : 4267)
				};

				///вернет строку - заголовок характеристики
				void CBaseFeatureCollection::AppendFeatureHeader(std::string& s) const
				{
					s+= "<";
					s+= GetFeatureName();
					s+= ">";
				}
				///вернет строку - закрывающую характеристику
				void CBaseFeatureCollection::AppendFeatureEnd(std::string& s) const
				{
					s+= "</";
					s+= GetFeatureName();
					s+= ">";
				}
				///вернет строку - тело характеристики
				void CBaseFeatureCollection::AppendFeatureValues(std::string& s) const
				{
					unsigned int i = 0;
					while(i < m_FeatureCount){
						std::string sXML;
						m_ppFeatures[i]->GetXMLValue(sXML);
						s+= sXML;
						i++;
					}
				}
		
			}
		}
	}
}