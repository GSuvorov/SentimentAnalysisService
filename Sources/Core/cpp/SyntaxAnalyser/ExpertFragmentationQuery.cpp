#include "StdAfx.h"
#include ".\expertfragmentationquery.h"

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{

			//Конструктор
			CExpertFragmentationQuery::CExpertFragmentationQuery()
			{
				SS_TRY
					m_ConjQuery.push_back(L"and");
					m_ConjQuery.push_back(L"or");
					m_ConjQuery.push_back(L",");
					m_Divisor = L",";
				SS_CATCH(L"")
			}

			//Собств метод. Переопределяю
			bool CExpertFragmentationQuery::IsConj(BoardElements::BoardElementsIterator elem) const
			{
				bool b = IsConjQuery(elem);
				//Если предыдущий элемент запятая, то на блоки не бить
				if ( (*(elem-1))->GetWord() == m_Divisor )
					return false;
				else
					return b;
			}
				
			///Распознавание одиночных союзов
			bool CExpertFragmentationQuery::IsConjQuery(BoardElements::BoardElementsIterator elem) const
			{
				SS_TRY
				{
					if ( find(m_ConjQuery.begin(), m_ConjQuery.end(), (*elem)->GetWord()) != m_ConjQuery.end() )
					{
						(*elem)->SetBeatsBlocks();
						return true;
					}
				}
				SS_CATCH(L"")
				return false;
			}

		}
	}
}