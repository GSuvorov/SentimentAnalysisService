//BlockExpert.cpp

#include "StdAfx.h"
#include ".\blockexpert.h"
#include "../ASSInterface/IDictionaryManager.h"

namespace SS
{
namespace Syntax
{
namespace ExpertModel
{
	///Провести анализ детект-элемента
	bool CBlockExpert::ExecuteDetect(IObjectAnalyser* pObjectAnalyser)
	{
		//Позиция блока в предложении
		unsigned int iBlockPosition = pObjectAnalyser->GetConstructionObject()->m_ObjectFeature.m_BlockPosition.GetValue();
		
		if (iBlockPosition == 0)
		{
			PBLOCK pBlock;
			//Доанализ детект-блока
			if (m_pBlockDetect) 
			{
				//while (m_pBlockDetect->SwitchUnionVersion())
				//{
					if (pObjectAnalyser->Run()) 
					{ 
						m_UnionBlocks.push_back(m_pBlockDetect);
						return true; 
					}
				//}
				m_pBoard->SetCurrentBlock(m_pBlockDetect);
				pBlock = m_pBoard->GetNextBlock();
			}
			else
			{ pBlock = m_pBoard->GetFirstBlock(); }
		
			//Поиск в остальных блоках
			for (pBlock; pBlock; pBlock = m_pBoard->GetNextBlock())
			{
				m_itFirstR = m_itBlockBegin = pBlock->GetFirst();
				m_itLast = m_itBlockEnd = pBlock->GetLast();
				//Анализ				
				//while (pBlock->SwitchUnionVersion())
				//{
					if (pObjectAnalyser->Run())
					{
						//Детект-блок
						m_pBlockDetect = pBlock;
						m_pBlockRight = pBlock;
						m_pBlockLeft = pBlock;
						m_UnionBlocks.push_back(m_pBlockDetect);
						return true;
					}
				//}
			}
			//Если не сработал
			return false;
		}
		else
		{
			//Доанализ детект-блока
			if (m_pBlockDetect)
			{
				//while (m_pBlockDetect->SwitchUnionVersion())
				//{
					if (pObjectAnalyser->Run()) 
					{ 
						m_UnionBlocks.push_back(m_pBlockDetect);
						return true; 
					}
				//}
				return false;
			}
			
			//Если блоков меньше
			if (m_pBoard->CountBlock() < iBlockPosition)
			{ return false; }
			
			unsigned int iBlockNumber = 0;
			for (PBLOCK pBlock = m_pBoard->GetFirstBlock(); pBlock; pBlock = m_pBoard->GetNextBlock())
			{
				++iBlockNumber;
				if (iBlockNumber == iBlockPosition)
				{
					m_itFirstR = m_itBlockBegin = pBlock->GetFirst();
					m_itLast = m_itBlockEnd = pBlock->GetLast();
					//Анализ				
					//while (pBlock->SwitchUnionVersion())
					//{
						if (pObjectAnalyser->Run())
						{
							//Детект-блок
							m_pBlockDetect = pBlock;
							m_pBlockRight = pBlock;
							m_pBlockLeft = pBlock;
							m_UnionBlocks.push_back(m_pBlockDetect);
							return true;
						}
					//}
					return false;
				}
			}
		}
		return false;
	}
	
	///Провести анализ остальных элементов
	bool CBlockExpert::ExecuteOther(IObjectAnalyser* pObjectAnalyser)
	{
		//Количество блоков для просмотра
		unsigned int iBlockSearchScope = pObjectAnalyser->GetConstructionObject()->m_ObjectFeature.m_BlockSearchScope.GetValue();
		//Текущее количество блоков
		unsigned int iBlockCount = 0;
		
		if ( pObjectAnalyser->GetConstructionObject()->m_ObjectFeature.m_PositionToDetectedObject.Equal(Values::PositionToDetectedObject.ptdoRight) ||
			 pObjectAnalyser->GetConstructionObject()->m_ObjectFeature.m_PositionToDetectedObject.IsUndefined() )
		{ 
			//Доанализ блока
			//while (m_pBlockRight->SwitchUnionVersion())
			//{
				if (pObjectAnalyser->Run())
				{ return true; }
			//}
			
			//УСТАНОВИТЬ НУЖНЫЙ БЛОК
			m_pBoard->SetCurrentBlock(m_pBlockRight);
			PBLOCK pBlock = m_pBoard->GetNextBlock();
			for (pBlock; pBlock; pBlock = m_pBoard->GetNextBlock())
			{
				if (iBlockCount == iBlockSearchScope)
				{ return false;	}
				//Установка диапазона
				m_itFirstR = m_itBlockBegin = pBlock->GetFirst();
				m_itLast = m_itBlockEnd = pBlock->GetLast();
				//while (pBlock->SwitchUnionVersion())
				//{
					if (pObjectAnalyser->Run())
					{ 
						m_pBlockRight = pBlock;
						m_UnionBlocks.push_back(m_pBlockRight);
						return true;
					}
				//}
				++iBlockCount;
			}
		}
		else if ( pObjectAnalyser->GetConstructionObject()->m_ObjectFeature.m_PositionToDetectedObject.Equal(Values::PositionToDetectedObject.ptdoLeft) )
		{
			//Доанализ блока
			//while (m_pBlockLeft->SwitchUnionVersion())
			//{
				if (pObjectAnalyser->Run())
				{ return true; }
			//}
			
			//УСТАНОВИТЬ НУЖНЫЙ БЛОК
			m_pBoard->SetCurrentBlock(m_pBlockLeft);
			PBLOCK pBlock = m_pBoard->GetPrevBlock();
			for (pBlock; pBlock; pBlock = m_pBoard->GetPrevBlock())
			{
				if (iBlockCount == iBlockSearchScope)
				{ return false;	}
				//Установка диапазона
				m_itFirst = m_itBlockBegin = pBlock->GetFirst();
				m_itLastL = m_itBlockEnd = pBlock->GetLast();
				//while (pBlock->SwitchUnionVersion())
				//{
					if (pObjectAnalyser->Run())
					{
						m_pBlockLeft = pBlock;
						m_UnionBlocks.insert(m_UnionBlocks.begin(), m_pBlockLeft);
						return true;
					}
				//}
				++iBlockCount;
			}
		}
		return false;
	}
	
	//Провести анализ предложения
	void CBlockExpert::ExecuteAnalysis()
	{
		CBoardElement* pElem;
		BoardElementsIterator itCurrent;
		BoardElementsIterator itDetect;
		m_pBlockDetect = NULL;
		
		IModifiedConstruction::iterator itDetObj(m_pModifiedConstruction->begin());
		IModifiedConstruction::iterator itObj = itDetObj; ++itObj;
		
		while (ExecuteDetect(*itDetObj))
		{
			pElem = (*itDetObj)->GetWorkedElement();
			if (pElem)
			{
				//Пересчет диапазона
				itDetect = m_itBeginBoard + pElem->GetPosition();
				m_itLastL = itDetect;
				m_itFirstR = itDetect + 1;
				m_itBlockBegin = m_itFirst = m_pBlockDetect->GetFirst();
				m_itBlockEnd = m_itLast = m_pBlockDetect->GetLast();
			}
			else
			{ SS_THROW(L"Неверно инициализирован Детект-элемент модифицированной конструкции."); }

			//Проверка остальных
			itObj = itDetObj; ++itObj;
			for (itObj; itObj != m_pModifiedConstruction->end(); ++itObj)
			{
				//Установка IdParent последнего сработавшего
				if (pElem)
				{ (*itObj)->SetIdParent(pElem->GetPosition()); }
				else
				{ (*itObj)->SetIdParent(-1); }

				//Анализ
				if ( !ExecuteOther(*itObj) )
				{ break; }
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
							m_itFirst = m_pBlockLeft->GetFirst();
							m_itBlockBegin = m_pBlockLeft->GetFirst();
							m_itBlockEnd = m_pBlockLeft->GetLast();
						}
						else if ( itCurrent >= m_itFirstR)
						{ 
							m_itFirstR = itCurrent + 1; 
							m_itLast = m_pBlockRight->GetLast();
							m_itBlockBegin = m_pBlockRight->GetFirst();
							m_itBlockEnd = m_pBlockRight->GetLast();
						}
						else
						{ SS_THROW(L"Ошибка диапазона."); }
					}
				}
			}
			//Применение конструкции
			if ( itObj == m_pModifiedConstruction->end() ) 
			{ 
				ApplyConstruction();
				UniteBlocks();
				m_pBlockDetect = m_pBlockRight;
				m_itFirstR = m_pBlockDetect->GetFirst();
			}
			else
			{ 
				m_itFirstR = itDetect + 1;
				//Очистить конртейнеры с АТ морфологией у BoardElement-ов
				m_pModifiedConstruction->ClearContainersAT();
			}
			
			//Пересчет диапазона для нового поиска детект-элемента
			m_itLast = m_pBlockDetect->GetLast();
			m_itBlockBegin = m_pBlockDetect->GetFirst();
			m_itBlockEnd = m_pBlockDetect->GetLast();	
			
			//Очистка списка блоков
			m_UnionBlocks.clear();
		}
	}
	
	//Проход по доске
	bool CBlockExpert::PassBoard(CBoard & oBoard, PBLOCK pBlock)
	{
		SS_TRY
		{
			//Если блок один
			if (oBoard.CountBlock() == 1) 
			{ return false; }
			
			//Границы предложения
			m_itBlockBegin = oBoard.ElementsBegin();
			m_itBlockEnd = oBoard.ElementsEnd();
			
			//Проверка применимости конструкции
			if ( !IsApplicable() )
			{ return false;	}
	
			//Запомнить предложение
			m_pBoard = &oBoard;
			//Первый элемент всего предложения
			m_itBeginBoard = oBoard.ElementsBegin();
			//Очистка списка блоков
			m_UnionBlocks.clear();
			
			//Выполнить анализ
			ExecuteAnalysis();
		}
		SS_CATCH(L"")
		return false;
	}

	///Обьединить блоки
	void CBlockExpert::UniteBlocks()
	{
		SS_TRY
		{
			if (m_UnionBlocks.size() <= 1) 
			{ SS_THROW(L"Неверное срабатывание эксперта."); }
			std::list<PBLOCK>::iterator it_prev(m_UnionBlocks.begin());
			std::list<PBLOCK>::iterator it(it_prev);
			for (++it; it != m_UnionBlocks.end(); ++it)
			{
				(*it)->AddBlock(*it_prev);
				m_pBoard->SetCurrentBlock(*it_prev);
				m_pBoard->DeleteCurrentBlock();
				it_prev = it;
			}
		}
		SS_CATCH(L"")
	}

}
}
}