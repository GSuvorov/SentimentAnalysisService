//ExpertResumeQuery.cpp

#include "StdAfx.h"
#include ".\expertresumequery.h"
#include "../FeatureLibrary/define.h"

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{

			//Конструктор/деструктор
			CExpertResumeQuery::CExpertResumeQuery()
			{
				SS_TRY
					m_vConj.push_back(L"and");
					m_vConj.push_back(L"or");
					m_vConj.push_back(L",");
				SS_CATCH(L"")
			}

			CExpertResumeQuery::~CExpertResumeQuery()
			{
			}

			//Общедоступные методы
			bool CExpertResumeQuery::PassBoard(BoardElements::CBoard & rBoard, BoardElements::PBLOCK pCurrBlock)
			{
				SS_TRY
					
					const unsigned int MustCountBlock = 1;
					const unsigned int MustCountHead = 1;
					unsigned int uiCountHead = 0;
					bool bIsCong = false;
					
					//Если блок не один то разбивать не надо
					if (rBoard.CountBlock() != MustCountBlock)
					{ return false; }
					
					//Подсчёт головных эл-ов и Проверка на наличие необходимых союзов					
					BoardElements::BoardElementsIterator itFirstElem = pCurrBlock->GetFirst();
					BoardElements::BoardElementsIterator itLastElem = pCurrBlock->GetLast();
					BoardElements::BoardElementsIterator itElem;
					
					for (itElem = itFirstElem; itElem != itLastElem; itElem++)
					{
						//Подсчёт головных
						if ( ((*itElem)->IsHead()) 
							 && !((*itElem)->GetFirstMorpho()->GetMorphoFeature().m_OfficialType.Equal(SS::Core::Features::Values::OfficialType.otSymbol) ) )
						{
							uiCountHead++;
						}
						//Проверка на наличие союзов
						if ( find(m_vConj.begin(), m_vConj.end(), (*itElem)->GetWord()) != m_vConj.end() )
						{
							bIsCong = true;
						}
					}

					if ( (uiCountHead > MustCountHead) && (bIsCong) )
					{
						return true;
					}
										
				SS_CATCH(L"");

				return false;
			}
			
			///Получить имя конструкции
			std::wstring CExpertResumeQuery::GetName() const
			{
				return L"ResumeExpert";
			}
		
		}
	}
}