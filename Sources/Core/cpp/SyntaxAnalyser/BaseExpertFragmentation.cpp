//BaseExpertFragmentation.cpp

#include "StdAfx.h"
#include ".\baseexpertfragmentation.h"

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			//конструктор
			CBaseExpertFragmentation::CBaseExpertFragmentation() : m_bBracketAnalyse(true)
			{
				m_CommonDivisors.push_back(L";");
				m_CommonDivisors.push_back(L":");
				m_CommonDivisors.push_back(L"(");
				m_sLeftBracket = L"(";
				m_sRightBracket = L")";
				m_sDivisor = L";";
			}
			
			//проход по доске
			bool CBaseExpertFragmentation::PassBoard(BoardElements::CBoard & rBoard, BoardElements::PBLOCK pBlock)
			{
				SS_TRY
				{
					//Первый элемент текущего блока
					BoardElements::BoardElementsIterator itBegin = rBoard.ElementsBegin();
					//Элемент за последним
					m_last = rBoard.ElementsEnd();
					//Удалить все блоки					
					rBoard.DeleteBlocks();
					//Наличие левой скобки
					bool bLeftBracket = false;
					//Перебор элементов
					for (BoardElements::BoardElementsIterator it = itBegin; it != m_last; ++it)
					{
						CBoardElement *belem = *it;
						if ( !((*it)->IsHead()) )
						{ continue; }
						
						if (m_bBracketAnalyse) 
						{
							//ОПЕРАЦИИ СО СКОБКАМИ
							if (bLeftBracket) 
							{
								if ( (*it)->GetWord() == m_sRightBracket )
								{
									if (pBlock = rBoard.AddBlock()) 
									{ 
										++it;
										pBlock->Init(itBegin, it); 
										if (it == m_last) return true;		
										(*it)->SetBeatsBlocks();
										itBegin = it;
										bLeftBracket = false;
									}
								}
								continue;
							}
							if ((*it)->GetWord() == m_sLeftBracket) 
							{ bLeftBracket = true; }
							//КОНЕЦ ОПЕРАЦИЙ СО СКОБКАМИ

							if (it == itBegin) 
								continue;
							if (it == m_last-1) 
								break;
							if ( (find(m_CommonDivisors.begin(), m_CommonDivisors.end(), (*it)->GetWord()) != m_CommonDivisors.end()) || IsConj(it) ) 
							{
								if (pBlock = rBoard.AddBlock()) 
								{ 
									pBlock->Init(itBegin, it); 
									(*it)->SetBeatsBlocks();
									itBegin = it;
								}
							}
						}
						else
						{
							if (it == itBegin) 
								continue;
							if (it == m_last-1) 
								break;
							if ( ((*it)->GetWord() == m_sDivisor) || IsConj(it) ) 
							{
								if (pBlock = rBoard.AddBlock()) 
								{ 
									pBlock->Init(itBegin, it); 
									(*it)->SetBeatsBlocks();
									itBegin = it;
								}
							}
						}
					}
					if (pBlock = rBoard.AddBlock()) 
					{ pBlock->Init(itBegin, m_last); }
				
					return true;
				}
				SS_CATCH(L"")
			}

		}
	}
}