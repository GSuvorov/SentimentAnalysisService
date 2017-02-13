#include "stdafx.h"
#include "ExpertFragmentationIndexation.h"
#include <algorithm>

namespace SS
{
namespace Syntax
{
namespace ExpertModel
{
	using namespace SS::Interface::Core;
	using namespace std;

	CExpertFragmentationIndexation::CExpertFragmentationIndexation()
	{
		InitDelim();
	}

	void CExpertFragmentationIndexation::InitDelim()
	{
		SS_TRY
		{
			//Союзы
			m_conjSingle.push_back(L"whether");
			m_conjSingle.push_back(L"if");
			m_conjSingle.push_back(L"because");
			m_conjSingle.push_back(L"unless");
			m_conjSingle.push_back(L"lest");
			m_conjSingle.push_back(L"and");
			m_conjSingle.push_back(L"or");
			//So that, in order that, as if
			vector<wstring> tmp;
			tmp.push_back(L"so");
			tmp.push_back(L"that");
			m_conjMultiple.push_back(tmp);
			tmp.erase(tmp.begin(), tmp.end());
			tmp.push_back(L"in");
			tmp.push_back(L"order");
			tmp.push_back(L"that");
			m_conjMultiple.push_back(tmp);
			tmp.erase(tmp.begin(), tmp.end());
			tmp.push_back(L"as");
			tmp.push_back(L"if");
			m_conjMultiple.push_back(tmp);
			tmp.erase(tmp.begin(), tmp.end());
			//who, whose, what, which
			m_pronounSingleNotBegin.push_back(L"who");
			m_pronounSingleNotBegin.push_back(L"whose");
			m_pronounSingleNotBegin.push_back(L"whom");
			m_pronounSingleNotBegin.push_back(L"what");
			m_pronounSingleNotBegin.push_back(L"which");
			m_pronounSingleNotBegin.push_back(L",");
			//when, where, how, why 
			m_adverbSingleNotBegin.push_back(L"when");
			m_adverbSingleNotBegin.push_back(L"where");
			m_adverbSingleNotBegin.push_back(L"how");
			m_adverbSingleNotBegin.push_back(L"why");
			m_adverbSingleNotBegin.push_back(L"while");
			//,
			m_Divisor = L",";
		}
		SS_CATCH(L"") 
	}

	bool CExpertFragmentationIndexation::IsConj(BoardElements::BoardElementsIterator elem) const
	{
		SS_TRY
		{
			bool b = 					 IsPronounSingleNotBegin(elem) ||
					 IsAdverbSingleNotBegin(elem);
					 					 

			bool b1 = IsConjMultiple(elem) || IsConjSingleBegin(elem) ||
					 IsConjSingle(elem);
			
			//Если предыдущий элемент запятая, то на блоки не бить
			/*if ( (*(elem-1))->GetWord() == m_Divisor )
				return false;
			else*/
			//if (b1 && (*(elem-1))->GetWord() == m_Divisor)
			//	return true;
			if (b || b1) return  true;
		}
		SS_CATCH(L"")
	}

	//Распознавание одиночных союзов
	bool CExpertFragmentationIndexation::IsConjSingle(BoardElements::BoardElementsIterator elem) const
	{
		SS_TRY
		{			
			if (find(m_conjSingle.begin(), m_conjSingle.end(), (*elem)->GetWord()) != m_conjSingle.end())
			{
				(*elem)->SetBeatsBlocks();
				return true;
			}
		}
		SS_CATCH(L"")
		return false;
	}

	//Распознавание одиночных союзов не в начале предложения
	bool CExpertFragmentationIndexation::IsConjSingleBegin(BoardElements::BoardElementsIterator elem) const
	{
		SS_TRY
		{
			if ( find(m_conjSingleBegin.begin(), m_conjSingleBegin.end(), (*elem)->GetWord()) != m_conjSingleBegin.end() )
			{
				(*elem)->SetBeatsBlocks();
				return true;
			}
		}
		SS_CATCH(L"")
		return false;
	}

	//Распознавание сложных союзов
	bool CExpertFragmentationIndexation::IsConjMultiple(BoardElements::BoardElementsIterator elem) const
	{
		SS_TRY
		{
			std::vector<wstring> multipleWords;
			multipleWords.push_back((*elem)->GetWord());
			++elem;
			multipleWords.push_back((*elem)->GetWord());
			if ( find(m_conjMultiple.begin(), m_conjMultiple.end(), multipleWords) != m_conjMultiple.end() )
			{
				(*elem)->SetBeatsBlocks();
				(*(--elem))->SetBeatsBlocks();
				return true;
			}
			if (++elem == m_last) return false;
			multipleWords.push_back((*elem)->GetWord());
			if( find( m_conjMultiple.begin(), m_conjMultiple.end(), multipleWords ) != m_conjMultiple.end() )
			{
				(*elem)->SetBeatsBlocks();
				(*(--elem))->SetBeatsBlocks();
				(*(--elem))->SetBeatsBlocks();
				return true;
			}
		}
		SS_CATCH(L"")
		return false;
	}

	//Распознавание одиночных местоимений не в начале предложения
	bool CExpertFragmentationIndexation::IsPronounSingleNotBegin(BoardElements::BoardElementsIterator elem) const
	{
		SS_TRY
		{		
			if (find(m_pronounSingleNotBegin.begin(), m_pronounSingleNotBegin.end(), (*elem)->GetWord()) != m_pronounSingleNotBegin.end())
			{
				(*elem)->SetBeatsBlocks();
				return true;
			}
		}
		SS_CATCH(L"")
		return false;
	}

	//Распознавание одиночных наречий не в начале предложения
	bool CExpertFragmentationIndexation::IsAdverbSingleNotBegin(BoardElements::BoardElementsIterator elem) const
	{
		SS_TRY
		{
			if (find(m_adverbSingleNotBegin.begin(), m_adverbSingleNotBegin.end(), (*elem)->GetWord()) != m_adverbSingleNotBegin.end())
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