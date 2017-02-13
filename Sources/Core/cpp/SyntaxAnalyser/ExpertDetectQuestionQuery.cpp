//ExpertDetectQuestionQuery.cpp

#include "StdAfx.h"
#include ".\expertdetectquestionquery.h"

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{

			//Конструктор
			CExpertDetectQuestionQuery::CExpertDetectQuestionQuery()
			{
				SS_TRY
					
					m_vQuestions.push_back(L"who");
					m_vQuestions.push_back(L"what");
					m_vQuestions.push_back(L"which");
					m_vQuestions.push_back(L"when");
					m_vQuestions.push_back(L"where");
					m_vQuestions.push_back(L"why");
					m_vQuestions.push_back(L"how");
					m_vQuestions.push_back(L"whom");
				
				SS_CATCH(L"")
			}

			//Деструктор
			CExpertDetectQuestionQuery::~CExpertDetectQuestionQuery()
			{
			}

			//Другие методы
			///Возвращает true, если в предложении первым или вторым словом является вопрос
			bool CExpertDetectQuestionQuery::PassBoard(BoardElements::CBoard & rBoard, BoardElements::PBLOCK pBlock)
			{
				SS_TRY
					
					bool bIsQuestion = false;
					const unsigned int NeedCount = 2;
					unsigned int uiCount = 0;
					
					BoardElements::BoardElementsIterator itFirstElem = pBlock->GetFirst();
					BoardElements::BoardElementsIterator itLastElem = pBlock->GetLast();
					BoardElements::BoardElementsIterator itElem;
										
					for (itElem = itFirstElem; (itElem != itLastElem) && (uiCount < NeedCount); itElem++)
					{
						uiCount++;
						if ( find(m_vQuestions.begin(), m_vQuestions.end(), (*itElem)->GetWord()) != m_vQuestions.end() ) 
						{ bIsQuestion = true; }
					}

					return bIsQuestion;
					
				SS_CATCH(L"")
			}

			///Получить имя конструкции
			std::wstring CExpertDetectQuestionQuery::GetName() const
			{
				SS_TRY
					return L"ExpertDetectQuestion";
				SS_CATCH(L"")
			}

		}
	}
}