#include "StdAfx.h"
#include ".\expertfragmentationrussian.h"

namespace SS
{
namespace Syntax
{
namespace ExpertModel
{
	//Конструктор
	CExpertFragmentationRusInd::CExpertFragmentationRusInd()
	{
		//Запятая
		m_Divisor = L",";
		//Заполнение символами
		m_Symbols.push_back(L"(");
		m_Symbols.push_back(L")");
		//Заполнение наречиями
		m_Adverbs.push_back(L"где");
		m_Adverbs.push_back(L"куда");
		m_Adverbs.push_back(L"откуда");
		m_Adverbs.push_back(L"когда");
		m_Adverbs.push_back(L"пока");
		m_Adverbs.push_back(L"покамест");
		m_Adverbs.push_back(L"едва");
		m_Adverbs.push_back(L"насколько");
		m_Adverbs.push_back(L"сколько");
		//Скобки не анализировать
		CBaseExpertFragmentation::SetBracketAnalyse(false);
	}

	//Проверяет наличие элемента деления на блоки
	bool CExpertFragmentationRusInd::IsConj(BoardElements::BoardElementsIterator elem) const
	{
		return ( /*IsSymbol(elem) ||*/
				 IsMultiSimbol(elem) ||
				 IsAdverb(elem) );
	}

	//Проверка наличия символов
	bool CExpertFragmentationRusInd::IsSymbol(BoardElements::BoardElementsIterator elem) const
	{
		SS_TRY
		{
			if ( find(m_Symbols.begin(), m_Symbols.end(), (*elem)->GetWord()) != m_Symbols.end() )
			{
				(*elem)->SetBeatsBlocks();
				return true;
			}
		}
		SS_CATCH(L"")
		return false;
	}

	///Проверка наличия связок символов
	bool CExpertFragmentationRusInd::IsMultiSimbol(BoardElements::BoardElementsIterator elem) const
	{
		SS_TRY
		{
			//Первый элемент
			if ((*elem)->GetWord() != m_Divisor)
				return false;
			
			using namespace SS::Core::Features;
			bool bPretext = false;
		
			//Второй элемент
			++elem;
			SS::Syntax::BoardElements::PMORPHO pMorpho;
			for (pMorpho = (*elem)->GetFirstMorphoDynamic(); pMorpho; pMorpho = (*elem)->GetNextMorphoDynamic())
			{
				//Союз или местоимение
				if ( (pMorpho->GetMorphoFeature().m_OfficialType.Equal(Values::OfficialType.otConjunction)) ||
					 (pMorpho->GetMorphoFeature().m_PronounClass.Equal(Values::PronounClass.pncInterrogativeRelative)) )
				{
					(*elem)->SetBeatsBlocks();
					(*(--elem))->SetBeatsBlocks();
					return true;
				}
				//Предлог
				if (pMorpho->GetMorphoFeature().m_OfficialType.Equal(Values::OfficialType.otPretext)) 
				{ bPretext = true; }
			}
			//Если предлога не было
			if (!bPretext) return false;
			
			//Третий элемент
			if (++elem == m_last) return false;
			
			for (pMorpho = (*elem)->GetFirstMorphoDynamic(); pMorpho; pMorpho = (*elem)->GetNextMorphoDynamic())
			{
				//Местоимение
				if ( pMorpho->GetMorphoFeature().m_PronounClass.Equal(Values::PronounClass.pncInterrogativeRelative) )
				{
					(*elem)->SetBeatsBlocks();
					(*(--elem))->SetBeatsBlocks();
					(*(--elem))->SetBeatsBlocks();
					return true;
				}
			}
		}
		SS_CATCH(L"")
		return false;
	}

	///Проверка наличия наречия
	bool CExpertFragmentationRusInd::IsAdverb(BoardElements::BoardElementsIterator elem) const
	{
		SS_TRY
		{
			//Первый элемент
			if ((*elem)->GetWord() != m_Divisor)
				return false;
			//Второй элемент
			++elem;
			if ( find(m_Adverbs.begin(), m_Adverbs.end(), (*elem)->GetWord()) != m_Adverbs.end() ) 
			{
				(*elem)->SetBeatsBlocks();
				(*(--elem))->SetBeatsBlocks();
				return true;
			}
		}
		SS_CATCH(L"")
		return false;
	}
	
	//Возвращает текстовое название эксперта 
	std::wstring CExpertFragmentationRusInd::GetName() const
	{
		return L"ExpertFragmentationRusInd";	
	}

}
}
}