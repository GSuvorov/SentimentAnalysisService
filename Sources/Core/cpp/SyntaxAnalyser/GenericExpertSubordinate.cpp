//GenericExpertSubordinate.cpp
//Разработчик - Павленко Дмитрий

#include "StdAfx.h"
#include ".\genericexpertsubordinate.h"

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			using namespace SS::Core::Features;
			
			//Конструктор
			CGenericExpertSubordinate::CGenericExpertSubordinate()
			{
				SS_TRY
				{
					m_Conjunctions.push_back(L"who");
					m_Conjunctions.push_back(L"which");
					m_Conjunctions.push_back(L"that");
				}
				SS_CATCH(L"")
			}

			///Проход по доске
			bool CGenericExpertSubordinate::PassBoard(BoardElements::CBoard & Board, BoardElements::PBLOCK pCurrBlock)
			{
				SS_TRY
				{
					BoardElements::BoardCollection NullCollection;
					BoardElements::BoardElementsIterator itCurrBlockMember = NullCollection.end();
					BoardElements::BoardElementsIterator itPrevBlockMember = NullCollection.end();
					BoardElements::BoardElementsIterator itNoun = NullCollection.end();
					BoardElements::BoardElementsIterator itQuest = NullCollection.end();
					BoardElements::BoardElementsIterator itVerbFirst = NullCollection.end();
					BoardElements::BoardElementsIterator itVerbSecond = NullCollection.end();
					
					int iNumHead = 0;				//текущий номер головного члена
					const int CountHeadVerb = 2;	//кол-во головных членов(глаголов) которые необходимо проанализировать
					const int CountHeadNoun = 1;	//кол-во головных членов(существительных) которые необходимо проанализировать

					BoardElements::PBLOCK pPrevBlock = Board.GetPrevBlock();
					if ( (!pPrevBlock) || pPrevBlock->Empty() ) 
					{ return true; }

					//Флаг наличия необходимого слова
					bool IsQuest = false;
					//Поиск среди BeatsBlock-ов необходимого элемента
					for (itQuest = pCurrBlock->GetFirst(); (itQuest != pCurrBlock->GetLast()) && ((*itQuest)->BeatsBlocks()); ++itQuest)
					{
						if ( find(m_Conjunctions.begin(), m_Conjunctions.end(), (*itQuest)->GetWord()) != m_Conjunctions.end() )
						{
							IsQuest = true;
							break;
						}
					}
					if (!IsQuest)
						return true;
									
					itCurrBlockMember = itQuest;
					++itCurrBlockMember;
					iNumHead = 0;
					
					//перебор предыдущего блока, чтобы была только одна голова и она была существительным
					for (itPrevBlockMember = pPrevBlock->GetFirst(); itPrevBlockMember != pPrevBlock->GetLast(); itPrevBlockMember++)
					{
						if ((*itPrevBlockMember)->IsHead())
						{
							iNumHead++;
							//если вторая голова то выход
							if (iNumHead > CountHeadNoun)
							{
								return true;
							}
							//если не сущ то выход
							if ( (*itPrevBlockMember)->GetFirstMorpho()->GetMorphoFeature().m_PartOfSpeechTypes.GetValue() !=
								 Values::PartOfSpeechTypes.postNoun.GetValue() )
							{
								return true;
							}
							itNoun = itPrevBlockMember;		//получаем существительное
						}
					}
					
					if (itNoun == NullCollection.end()) 
					{ return true; }

					iNumHead = 0;
					//перебор текущего блока
					while ( (itCurrBlockMember != pCurrBlock->GetLast()) && (iNumHead < CountHeadVerb) )
					{
						//если голова да ещё и глагол + совпадает по AgrimentType (проверка по "числу") с сущ из пред блока
						if ( (*itCurrBlockMember)->IsHead() && 
							 (*itCurrBlockMember)->GetFirstMorpho()->GetMorphoFeature().m_PartOfSpeechTypes.GetValue() ==
							 Values::PartOfSpeechTypes.postVerb.GetValue() &&
							 (*itCurrBlockMember)->GetFirstMorpho()->GetMorphoFeature().m_NumberType.GetValue() == 
							 (*itNoun)->GetFirstMorpho()->GetMorphoFeature().m_NumberType.GetValue() )
						{
							iNumHead++;
							if (iNumHead == 1)
							{
								itVerbFirst = itCurrBlockMember;
							}
							if (iNumHead == 2)
							{
								itVerbSecond = itCurrBlockMember;
								
								(*itNoun)->GetVerElem()->m_syntaxFeatureAposteriori.m_IdParent = (unsigned)distance(Board.ElementsBegin(), itVerbSecond);
								(*itNoun)->GetVerElem()->m_syntaxFeatureAposteriori.m_SyntaxCategories = Values::SyntaxCategories.scSubjectFin;
								(*itNoun)->GetVerElem()->m_syntaxFeatureAposteriori.m_DependencyPower = 100;
								(*itNoun)->GetVerElem()->m_syntaxFeatureAposteriori.m_SyntaxCategoryPower = 100;

								(*itVerbFirst)->GetVerElem()->m_syntaxFeatureAposteriori.m_IdParent = (unsigned)distance(Board.ElementsBegin(), itNoun);
								(*itVerbFirst)->GetVerElem()->m_syntaxFeatureAposteriori.m_SyntaxCategories = Values::SyntaxCategories.scAttribute;
								(*itVerbFirst)->GetVerElem()->m_syntaxFeatureAposteriori.m_DependencyPower = 100;
								(*itVerbFirst)->GetVerElem()->m_syntaxFeatureAposteriori.m_SyntaxCategoryPower = 100;

								(*itVerbSecond)->GetVerElem()->m_syntaxFeatureAposteriori.m_SyntaxCategories = Values::SyntaxCategories.scMain;
								(*itVerbSecond)->GetVerElem()->m_syntaxFeatureAposteriori.m_SyntaxCategoryPower = 100;

								(*itQuest)->GetVerElem()->m_syntaxFeatureAposteriori.m_IdParent = (unsigned)distance(Board.ElementsBegin(), itVerbFirst);
								(*itQuest)->GetVerElem()->m_syntaxFeatureAposteriori.m_SyntaxCategories = Values::SyntaxCategories.scSubjectFin;
								(*itQuest)->GetVerElem()->m_syntaxFeatureAposteriori.m_DependencyPower = 100;
								(*itQuest)->GetVerElem()->m_syntaxFeatureAposteriori.m_SyntaxCategoryPower = 100;

								pCurrBlock->AddBlock(pPrevBlock);
								continue;
							}
						}
						itCurrBlockMember++;
					}
				}
				SS_CATCH(L"");
				return true;
			}

		}
	}
}