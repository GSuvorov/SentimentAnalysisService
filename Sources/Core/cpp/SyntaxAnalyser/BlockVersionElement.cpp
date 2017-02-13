//BlockVersionElement.cpp

#include "stdafx.h"
#include "BlockVersionElement.h"
#include "BoardElement.h"

namespace SS
{
namespace Syntax
{
namespace BoardElements
{
	//Конструктор
	CBlockVersionElement::CBlockVersionElement() 
		: 
		m_pMorpho(NULL),
		m_isHead(true),
		m_iIdGroup(-111) 
	{}

	//Инициализация существующего элемента
	void CBlockVersionElement::Init(CBoardElement* pElem, PMORPHO pMorpho)
	{
		m_morphoFeatureApriori.AssignMorphoFeature(&pMorpho->GetMorphoFeature());
		m_syntaxFeatureAposteriori.Assign(&pElem->GetSyntaxFeatureDynamic());
		m_syntaxFeatureApriori.Assign(&pElem->GetSyntaxFeatureAprioriDynamic());
		m_pMorpho = pMorpho;
		m_isHead = true;
		m_iIdGroup = pElem->GetGroupDynamic();
		m_oSemanticService = pElem->GetSemanticService();
	}
	
}
}
}