#include "stdafx.h"
#include "UnitsExpert.h"

namespace SS
{
namespace Syntax
{
namespace ExpertModel
{
	//Провести анализ предложения
	void CUnitsExpert::ExecuteAnalysis(PBLOCK pBlock)
	{
		CBoardElement* pElem;
		BoardElementsIterator itCurrent;
		BoardElementsIterator itDetect;

		IModifiedConstruction::iterator itDetObj(m_pModifiedConstruction->begin());
		IModifiedConstruction::iterator itObj = itDetObj; ++itObj;

		while ( (*itDetObj)->Run() )
		{
			pElem = (*itDetObj)->GetWorkedElement();
			if (pElem)
			{
				//Пересчет диапазона
				itDetect = m_itBeginBoard + pElem->GetPosition();
				m_itLastL = itDetect;
				m_itFirstR = itDetect + 1;
			}
			else
			{ 
				SS_THROW(L"Неверно инициализирован Детект-элемент модифицированной конструкции."); 
			}

			//Проверка остальных
			itObj = itDetObj; ++itObj;
			for (itObj; itObj != m_pModifiedConstruction->end(); ++itObj)
			{
				//Установка IdParent последнего сработавшего
				if (pElem)
				{ 
					(*itObj)->SetIdParent(pElem->GetPosition()); 
				}
				else
				{ 
					(*itObj)->SetIdParent(-1); 
				}

				//Анализ
				if ( !(*itObj)->Run() )
				{ 
					break;
				}
				else
				{
					pElem = (*itObj)->GetWorkedElement();
					if (pElem)
					{
						//Пересчет диапазона
						itCurrent = m_itBeginBoard + pElem->GetPosition();
						if ( itCurrent < m_itLastL )
						{ 
							m_itLastL = itCurrent; 
						}
						else if ( itCurrent >= m_itFirstR)
						{
							m_itFirstR = itCurrent + 1; 
						}
						else
						{ 
							SS_THROW(L"Ошибка диапазона."); 
						}
					}
				}
			}
			//Применение конструкции
			if ( itObj == m_pModifiedConstruction->end() )
			{ 
				//Если конструкция не простая (а с анализом АТ)
				if ( m_pModifiedConstruction->IsAT() )
				{ 
					pBlock->IncCountATExpert();
					ApplyConstruction(pBlock->GetCountATExpert());
				}
				else
				{
					ApplyConstruction();
				}
			}
			else
			{
				//Очистить конртейнеры с АТ морфологией у BoardElement-ов
				m_pModifiedConstruction->ClearContainersAT();
			}

			//Пересчет диапазона для нового поиска
			m_itFirst = itDetect;
			m_itLastL = m_itLast;
			m_itFirstR = itDetect + 1;
		}
	}
	
	//Проход по доске
	bool CUnitsExpert::PassBoard(CBoard & oBoard, PBLOCK pBlock)
	{
		SS_TRY
		{
			//Первый элемент всего предложения
			m_itBeginBoard = oBoard.ElementsBegin();
			//Границы блока
			m_itBlockBegin = pBlock->GetFirst();
			m_itBlockEnd = pBlock->GetLast();
			//Границы диапазона
			m_itFirst = m_itFirstR = m_itBlockBegin;
			m_itLast = m_itLastL = m_itBlockEnd;

			//Проверка применимости конструкции
			if ( !m_bIsAmbiguity )
			{
				if (m_isCheckedApplication && !m_isApplicable)
				{ 
					return true;
				}
				else if (!m_isCheckedApplication)
				{
					m_isCheckedApplication = true;
					if ( !IsApplicable() )
					{
						m_isApplicable = false;
						return true;
					}
					else
					{ 
						m_isApplicable = true; 
					}
				}
			}
			//Выполнить анализ
			ExecuteAnalysis(pBlock);
		}
		SS_CATCH(L"")
		return false;
	}

}
}
}





