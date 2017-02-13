//ExpertCoordinate.cpp
#include "StdAfx.h"
#include ".\expertcoordinate.h"

namespace SS
{
namespace Syntax
{
namespace ExpertClause
{
	using namespace SS::Core::Features;
	
	//Проход по доске 
	bool CExpertCoordinate::PassBoard(CBoard & rBoard, PBLOCK pBlock)
	{
		SS_TRY
		{
			//Получение первого блока
			PBLOCK pPrevBlock = rBoard.GetPrevBlock();
			if ( (!pPrevBlock) || pPrevBlock->Empty() )
			{ return true; }

			//Предполагается наличие союза
			BoardElementsIterator itConjunction = pBlock->GetFirst();
			//Флаг наличия необходимого союза
			bool IsConj = false;
			//Поиск среди BeatsBlock елем-ов второго блока необходимого элемента
			for (itConjunction; (itConjunction != pBlock->GetLast()) && ((*itConjunction)->BeatsBlocks()); ++itConjunction)
			{
				//Поиск во втором блоке необходимого союза
				if ( (*itConjunction)->GetFirstMorpho()->GetMorphoFeature().m_ConjunctionType.GetValue() &
					 Values::TConjunctionType.cjtCoordinating.GetValue() )
				{
					IsConj = true;
					break;
				}
			}
			if (!IsConj)
				return true;

			//В первом блоке ищем Main с подчиненным SubjectFin
			BoardElementsIterator itFirstMember;
			BoardElementsIterator itParentMember;
			BoardElementsIterator it;
			bool IsMain = false;
			for (it = pPrevBlock->GetFirst(); it != pPrevBlock->GetLast(); ++it)
			{
				if ( (*it)->GetSyntaxFeature().m_SyntaxCategories.GetValue() &
					 Values::SyntaxCategories.scSubjectFin.GetValue() )
				{
					itFirstMember = it;
					itParentMember = rBoard.ElementsBegin() + (*it)->GetParentIndex();
					while ( (itParentMember != itFirstMember) && pPrevBlock->Contain(itParentMember) ) 
					{
						if ( (*itParentMember)->GetSyntaxFeature().m_SyntaxCategories.GetValue() &
							Values::SyntaxCategories.scMain.GetValue() )
						{ 
							IsMain = true;
							break; 
						}
						itFirstMember = itParentMember;
						itParentMember = rBoard.ElementsBegin() + (*itFirstMember)->GetParentIndex();
					}
					if (IsMain)
					{ 
						itFirstMember = itParentMember;
						break; 
					}
				}
			}
			if (it == pPrevBlock->GetLast())
			{ return true; }

			//Во втором блоке ищем Main с подчиненным SubjectFin
			BoardElementsIterator itSecondMember;
			it = itConjunction;
			++it;
			IsMain = false;
			for (it; it != pBlock->GetLast(); ++it)
			{
				if ( (*it)->GetSyntaxFeature().m_SyntaxCategories.GetValue() &
					Values::SyntaxCategories.scSubjectFin.GetValue() )
				{
					itSecondMember = it;
					itParentMember = rBoard.ElementsBegin() + (*it)->GetParentIndex();
					while ( (itParentMember != itSecondMember) && pBlock->Contain(itParentMember) ) 
					{
						if ( (*itParentMember)->GetSyntaxFeature().m_SyntaxCategories.GetValue() &
							Values::SyntaxCategories.scMain.GetValue() )
						{ 
							IsMain = true;
							break; 
						}
						itSecondMember = itParentMember;
						itParentMember = rBoard.ElementsBegin() + (*itSecondMember)->GetParentIndex();
					}
					if (IsMain)
					{ 
						itSecondMember = itParentMember;
						break; 
					}
				}
			}
			if (it == pBlock->GetLast()) 
			{ return true; }
			
			//Сохранение текущих значений
			unsigned int iSecondParent = (*itSecondMember)->GetVerElem()->m_syntaxFeatureAposteriori.m_IdParent.GetValue();
			unsigned int iSecondPower = (*itSecondMember)->GetVerElem()->m_syntaxFeatureAposteriori.m_DependencyPower.GetValue();
			bool bSecondHead = (*itSecondMember)->GetVerElem()->m_isHead;
			unsigned int iConjParent = (*itConjunction)->GetVerElem()->m_syntaxFeatureAposteriori.m_IdParent.GetValue();
			unsigned int iConjPower = (*itConjunction)->GetVerElem()->m_syntaxFeatureAposteriori.m_DependencyPower.GetValue();
			//Второй элемент цепляем под первый
			(*itSecondMember)->GetVerElem()->m_syntaxFeatureAposteriori.m_IdParent = (unsigned)distance(rBoard.ElementsBegin(), itFirstMember);
			(*itSecondMember)->GetVerElem()->m_syntaxFeatureAposteriori.m_DependencyPower = 100;
			(*itSecondMember)->GetVerElem()->m_isHead = false;
			(*itConjunction)->GetVerElem()->m_syntaxFeatureAposteriori.m_IdParent = (unsigned)distance(rBoard.ElementsBegin(), itSecondMember);
			(*itConjunction)->GetVerElem()->m_syntaxFeatureAposteriori.m_DependencyPower = 100;
			//Объединяем два блока
			pBlock->AddBlock(pPrevBlock);
			//Восстановление значений
			(*itSecondMember)->GetVerElem()->m_syntaxFeatureAposteriori.m_IdParent.SetValue(iSecondParent);
			(*itSecondMember)->GetVerElem()->m_syntaxFeatureAposteriori.m_DependencyPower.SetValue(iSecondPower);
			(*itSecondMember)->GetVerElem()->m_isHead = bSecondHead;
			(*itConjunction)->GetVerElem()->m_syntaxFeatureAposteriori.m_IdParent.SetValue(iConjParent);
			(*itConjunction)->GetVerElem()->m_syntaxFeatureAposteriori.m_DependencyPower.SetValue(iConjPower);
		}
		SS_CATCH(L"")
		return true;
	}
	
	//Возвращает текстовое название эксперта
	std::wstring CExpertCoordinate::GetName() const
	{
		return L"ExpertCoordinate";
	}

}
}
}