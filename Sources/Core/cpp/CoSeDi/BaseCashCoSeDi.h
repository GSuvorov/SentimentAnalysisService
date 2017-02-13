#pragma once

#include ".\CashCoSeDi.h"

namespace SS
{
	namespace CoSeDi
	{

		const int ciMaxIDSource = 200000;

		/// класс реализует базовую функциональность кэшей, общую для DBMS- и SQL-кэшей
		class CBaseCashCoSeDi
		{
		public:
			/// конструктор
			CBaseCashCoSeDi(void){};
			/// деструктор
			virtual ~CBaseCashCoSeDi(void){}
			
			/// проверяет содержится ли основа слова в словаре или нет
			/** 
				\param uiIDSource - индекс основы слова
				\param ewr - показывает в какой таблице должно содержаться слово
				\return bool - результат проверки
			*/
			bool IsExistIDSource(unsigned int uiIDSource, SS::CoSeDi::DataTypes::EWorkRegion ewr)
			{
			
				SS_TRY
				{
 					SS::CoSeDi::DataTypes::S_bf_ui_WorkRegions oS_bf_ui_WorkRegions;	
					oS_bf_ui_WorkRegions = m_ucExistence[uiIDSource];

					switch(ewr)
					{
					case SS::CoSeDi::DataTypes::wrSemantic:
						return oS_bf_ui_WorkRegions.IsSemantic();
						break;
					case SS::CoSeDi::DataTypes::wrCombinatory:
						return oS_bf_ui_WorkRegions.IsCombinatory();
						break;
					case SS::CoSeDi::DataTypes::wrSyntax:
						return oS_bf_ui_WorkRegions.IsSyntax();
						break;
					default:
						return false;
						break;
					}					
				}		
				SS_CATCH(L"");

				return true;


			}

			/// добавляет основу в кэш
			/** 
				\param uiIDSource - индекс основы слова
				\param ewr - название таблицы, в которую добавляется слово
				\return void
			*/
			void AddIDSource(unsigned int uiIDSource, SS::CoSeDi::DataTypes::EWorkRegion ewr)
			{
				SS_TRY;
				///////////////////////////////////////////////////////////////////////
				/*	сохранение параметров в памяти для быстрого доступа	*/

				SS::CoSeDi::DataTypes::S_bf_ui_WorkRegions oS_bf_ui_WorkRegions;	
				oS_bf_ui_WorkRegions = m_ucExistence[uiIDSource];				
				switch(ewr)
				{
				case SS::CoSeDi::DataTypes::wrSemantic:
					oS_bf_ui_WorkRegions.SetSemantic();
					break;
				case SS::CoSeDi::DataTypes::wrCombinatory:
					oS_bf_ui_WorkRegions.SetCombinatory();
					break;
				case SS::CoSeDi::DataTypes::wrSyntax:
					oS_bf_ui_WorkRegions.SetSyntax();
					break;
				}				
				m_ucExistence[uiIDSource] = *((unsigned short*)&oS_bf_ui_WorkRegions);

		
				SS_CATCH(L"");
				
			}
		protected:
			/*! \var unsigned short m_ucExistence[ciMaxIDSource];
			*  \brief     массив существования основы в словаре
			*/
			unsigned short m_ucExistence[ciMaxIDSource];							
		};
	};
};