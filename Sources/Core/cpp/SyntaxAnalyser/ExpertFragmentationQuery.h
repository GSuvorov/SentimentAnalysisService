//ExpertFragmentationQuery.h
#pragma once

#include "BaseExpertFragmentation.h"
#include <vector>

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			
			class CExpertFragmentationQuery : public CBaseExpertFragmentation
			{
			private:
				///Части речи, бьющие предложение на блоки ('and' 'or' ',') 
				std::vector<std::wstring> m_ConjQuery;
				///Одиночная запятая
				std::wstring m_Divisor;
			
			private:
				///Проверяет, является ли данный элемент союзом.
				virtual bool IsConj(BoardElements::BoardElementsIterator elem) const;
				///Распознавание одиночных союзов.
				bool IsConjQuery(BoardElements::BoardElementsIterator elem) const;
			
			public:
				//конструктор и деструктор
				CExpertFragmentationQuery();
				virtual ~CExpertFragmentationQuery(){};
				//другие методы
				///Получение названия эксперта
				std::wstring GetName() const
				{
					return L"Fragmentation Expert Query";
				}
			};

		}
	}
}