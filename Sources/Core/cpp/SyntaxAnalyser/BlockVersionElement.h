//BlockVersionElement.h
#pragma once
#include "BoardCommonTypes.h"
#include "../ASSInterface/Construction.h"
#include "SemanticService.h"

namespace SS
{
namespace Syntax
{
namespace BoardElements
{
	///Элемент гипотезы разбора.
	class CBlockVersionElement
	{
	public:
		///Конструктор
		CBlockVersionElement();
		///Конструктор копирования
		CBlockVersionElement(const CBlockVersionElement & el) {	Assign(&el); };
		///Деструктор
		~CBlockVersionElement(){};
		///Инициализация существующего элемента
		void Init(CBoardElement* pElem, PMORPHO pMorpho);
		///Присвоить значение переданного элемента гипотезы
		void Assign(const CBlockVersionElement* pElem)
		{
			m_morphoFeatureApriori.Assign(&pElem->m_morphoFeatureApriori);
			m_syntaxFeatureAposteriori.Assign(&pElem->m_syntaxFeatureAposteriori);
			m_syntaxFeatureApriori.Assign(&pElem->m_syntaxFeatureApriori);
			m_pMorpho = pElem->m_pMorpho;
			m_isHead = pElem->m_isHead;
			m_iIdGroup = pElem->m_iIdGroup;
			m_oSemanticService = pElem->m_oSemanticService;
		}
	
	public:
		///Указывает на морфологическую характеристику элемента, породившую данный элемент.
		PMORPHO m_pMorpho;
		///Морфологическая информация (для детектирования)
		SS::Core::Features::CMorphoFeatureApriori m_morphoFeatureApriori;
		///Апостериорная синтаксическая информация.
		SS::Core::Features::CSyntaxFeatureAposteriori m_syntaxFeatureAposteriori;
		///Априорная синтаксическая информация
		SS::Core::Features::CSyntaxFeatureApriori m_syntaxFeatureApriori;
		///Указывает, стоит ли элемент в дереве на верхнем уровне.
		bool m_isHead;
		///Эксперт которой последним менял синтаксическую категорию элемента.
		int	m_iIdGroup;
		///Семантические характеристики
		CSemanticService m_oSemanticService;
	};
}
}
}