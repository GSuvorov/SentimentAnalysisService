#pragma	once

#include "BoardCommonTypes.h"
#include "../FeatureLibrary/define.h"
#include "Container.h"

namespace SS
{
	namespace Syntax
	{
		namespace BoardElements
		{
			///Класс, позволяющий отсеивать неиспользуемые морфологические характеристики слова 
			class CMorphoFormsFilter
			{
			private:
				//Контейнер неиспользуемой морфологии
				std::list<PMORPHO> m_lstMorphoRemoved;

			private:
				///КЛАСС-ПРЕДИКАТ ДЛЯ СРАВНЕНИЯ МОРФОЛОГИЧЕСКИХ ФИЧ
				class CMorphoComparePredicate
				{
				public:
					///Конструктор
					CMorphoComparePredicate() : m_pMorphoFeature(NULL) {};
					///Сравнение морфологических фич без учета неиспользуемых категорий
					bool operator()(SS::Core::Features::CMorphoFeature* pMorphoFeature);
					///Установить фичу
					void SetMorphoFeature(SS::Core::Features::CMorphoFeature* pMorphoFeature)
					{ m_pMorphoFeature = pMorphoFeature; };
				
				private:
					///Сравниваемая MorphoFeature
					SS::Core::Features::CMorphoFeature* m_pMorphoFeature;
				};
				
				///КЛАСС-ПРЕДИКАТ ДЛЯ СРАВНЕНИЯ МОРФОЛОГИЧЕСКИХ ФИЧ (для русского языка)
				class CMorphoComparePredicateRus
				{
				public:
					///Конструктор
					CMorphoComparePredicateRus() : m_pMorphoFeature(NULL) {};
					///Сравнение морфологических фич без учета неиспользуемых категорий
					bool operator()(SS::Core::Features::CMorphoFeature* pMorphoFeature);
					///Установить фичу
					void SetMorphoFeature(SS::Core::Features::CMorphoFeature* pMorphoFeature)
					{ m_pMorphoFeature = pMorphoFeature; };

				private:
					///Сравниваемая MorphoFeature
					SS::Core::Features::CMorphoFeature* m_pMorphoFeature;
				};

				///ОБЪЕКТ-ПРЕДИКАТ
				CMorphoComparePredicate m_oPredicate;

				///ОБЪЕКТ-ПРЕДИКАТ Русский
				CMorphoComparePredicateRus m_oPredicateRus;
			
			public:
				///Временно изымает из морфологического контейнера ненужные морфологические фичи
				void FilterMorphoContainer(CContainer<MORPHO,SIZEMORPHO> & lstMorphoContainer);
				///Временно изымает из морфологического контейнера ненужные морфологические фичи (для русского языка)
				void FilterMorphoContainerRus(CContainer<MORPHO,SIZEMORPHO> & lstMorphoContainer);
				///Добавляет в контейнер морфологические фичи, которые не использовались (востановление)
				void RestoreMorphoContainer(CContainer<MORPHO,SIZEMORPHO> & lstMorphoContainer);
			};
			
		}
	}
}