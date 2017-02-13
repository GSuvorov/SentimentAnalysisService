#include "stdafx.h"
#include "MorphoFormsFilter.h"
#include "MorphoListElement.h"
#include <algorithm>

using namespace std;

namespace SS
{
namespace Syntax
{
namespace BoardElements
{
	///Сравнение морфологических фич
	bool CMorphoFormsFilter::CMorphoComparePredicate::operator()(SS::Core::Features::CMorphoFeature* pMorphoFeature)
	{
		SS::Core::Features::Types::GenderType		ownGT, parGT;
		SS::Core::Features::Types::AnimInAnimType	ownAIAT, parAIAT;
		SS::Core::Features::Types::CommonType		ownCT, parCT;
		SS::Core::Features::Types::CountType			ownCount, parCount;

		ownGT	 = m_pMorphoFeature->m_GenderType;
		parGT	 = pMorphoFeature->m_GenderType;
		ownAIAT  = m_pMorphoFeature->m_AnimInAnimType; 
		parAIAT  = pMorphoFeature->m_AnimInAnimType;
		ownCT	 = m_pMorphoFeature->m_CommonType;
		parCT	 = pMorphoFeature->m_CommonType;
		ownCount = m_pMorphoFeature->m_CountType;
		parCount = pMorphoFeature->m_CountType;

		m_pMorphoFeature->m_GenderType.Undefine();
		m_pMorphoFeature->m_AnimInAnimType.Undefine();
		m_pMorphoFeature->m_CommonType.Undefine();
		m_pMorphoFeature->m_CountType.Undefine();

		pMorphoFeature->m_GenderType.Undefine();
		pMorphoFeature->m_AnimInAnimType.Undefine();
		pMorphoFeature->m_CommonType.Undefine();
		pMorphoFeature->m_CountType.Undefine();

		bool retval = m_pMorphoFeature->Equal(pMorphoFeature);

		m_pMorphoFeature->m_GenderType		= ownGT;
		pMorphoFeature->m_GenderType		= parGT;
		m_pMorphoFeature->m_AnimInAnimType	= ownAIAT; 
		pMorphoFeature->m_AnimInAnimType	= parAIAT;
		m_pMorphoFeature->m_CommonType		= ownCT;
		pMorphoFeature->m_CommonType		= parCT;
		m_pMorphoFeature->m_CountType		= ownCount;
		pMorphoFeature->m_CountType			= parCount;

		return retval;
	}

	//Сравнение морфологических фич (для русского языка)
	bool CMorphoFormsFilter::CMorphoComparePredicateRus::operator()(SS::Core::Features::CMorphoFeature* pMorphoFeature)
	{
		SS::Core::Features::Types::AnimInAnimType ownAT, parAT;
		////Запоминаем
		//ownAT = m_pMorphoFeature->m_AnimInAnimType; 
		//parAT = pMorphoFeature->m_AnimInAnimType;
		////Обнуляем
		//m_pMorphoFeature->m_AnimInAnimType.Undefine();
		//pMorphoFeature->m_AnimInAnimType.Undefine();
		////Проверка
		//bool Result = m_pMorphoFeature->Equal(pMorphoFeature);
		////Восстанавливаем значения
		//m_pMorphoFeature->m_AnimInAnimType = ownAT; 
		//pMorphoFeature->m_AnimInAnimType   = parAT;
		//Результат
		return false;
	}
	
	///Временно изымает из морфологического контейнера ненужные морфологические фичи
	void CMorphoFormsFilter::FilterMorphoContainer(CContainer<MORPHO,SIZEMORPHO> & lstMorphoContainer) 
	{
		m_lstMorphoRemoved.clear();
		
		//Вспомогательное хранилище морфологии
		std::vector<SS::Core::Features::CMorphoFeature*> leftMorpho;
		//Перебор морфологии
		for (PMORPHO pMorpho = lstMorphoContainer.GetFirst(); pMorpho; pMorpho = lstMorphoContainer.GetNext())
		{
			//Инициализация предиката очередной морфофичей
			m_oPredicate.SetMorphoFeature(&pMorpho->GetMorphoFeature());
			//Если нет такой морфо то добавляем иначе удаляем
			if ( find_if(leftMorpho.begin(), leftMorpho.end(), m_oPredicate) == leftMorpho.end() )
			{ 
				leftMorpho.push_back(&(pMorpho->GetMorphoFeature()));	
			}
			else
			{
				m_lstMorphoRemoved.push_back(pMorpho);
				lstMorphoContainer.DeleteCurrent();
			}
		}
	}

	///Временно изымает из морфологического контейнера ненужные морфологические фичи (для русского языка)
	void CMorphoFormsFilter::FilterMorphoContainerRus(CContainer<MORPHO,SIZEMORPHO> & lstMorphoContainer)
	{
		m_lstMorphoRemoved.clear();

		//Вспомогательное хранилище морфологии
		std::vector<SS::Core::Features::CMorphoFeature*> leftMorpho;
		//Перебор морфологии
		for (PMORPHO pMorpho = lstMorphoContainer.GetFirst(); pMorpho; pMorpho = lstMorphoContainer.GetNext())
		{
			//Инициализация предиката очередной морфофичей
			m_oPredicateRus.SetMorphoFeature(&pMorpho->GetMorphoFeature());
			//Если нет такой морфо то добавляем иначе удаляем
			if ( find_if(leftMorpho.begin(), leftMorpho.end(), m_oPredicateRus) == leftMorpho.end() )
			{ 
				leftMorpho.push_back(&(pMorpho->GetMorphoFeature()));	
			}
			else
			{
				m_lstMorphoRemoved.push_back(pMorpho);
				lstMorphoContainer.DeleteCurrent();
			}
		}
	}
	
	//Добавляет в контейнер морфологические фичи, которые не использовались (востановление)
	void CMorphoFormsFilter::RestoreMorphoContainer(CContainer<MORPHO,SIZEMORPHO> & lstMorphoContainer)
	{
		std::list<PMORPHO>::iterator it;
		for (it = m_lstMorphoRemoved.begin(); it != m_lstMorphoRemoved.end(); it++)
		{ lstMorphoContainer.RestoreForAddress(*it); }
	}

	
}
}
}