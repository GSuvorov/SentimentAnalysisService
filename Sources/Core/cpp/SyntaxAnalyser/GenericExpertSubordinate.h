//GenericExpertSubordinate.h
//Разработчик - Павленко Дмитрий

#pragma once

#include "BaseExpert.h"
#include <algorithm>


namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			///Вспомогательный эксперт с жестко прошитым правилом.
			/** Для связи предаточного предложения и записи 
			информации в гипотезу
			*/
			class CGenericExpertSubordinate : public CBaseExpert
			{
			private:
				//Слова и знаки препинания, разделяющие однородные члены предложения
				std::list<std::wstring> m_Conjunctions;
			
			public:
				//Конструктор
				CGenericExpertSubordinate();
				//Деструктор
				virtual ~CGenericExpertSubordinate(){};
				///Проход по доске
				bool PassBoard(BoardElements::CBoard & Board, BoardElements::PBLOCK pCurrBlock);
				///Получение названия эксперта
				std::wstring GetName() const
				{
					return L"Generic Expert Subordinate";
				}
			};


		}
	}
}