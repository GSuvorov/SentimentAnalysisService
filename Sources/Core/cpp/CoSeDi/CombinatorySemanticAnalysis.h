#pragma once

#include ".\ICoSeDi.h"

#include ".\combinatoryanalysis.h"
#include ".\semanticanalysis.h"
#include ".\semanticclassestree.h"

namespace SS
{
	namespace CoSeDi
	{
		namespace DBMS
		{
			class CCombinatorySemanticAnalysis : 
				public SS::CoSeDi::Intrefaces::ICombinatorySemanticAnalysis
			{
			public:
				CCombinatorySemanticAnalysis(
					SS::CoSeDi::DBMS::CCashCoSeDi*			pCashCoSeDi,
					SS::CoSeDi::DBMS::CCombinatoryAnalysis*	pCombinatoryAnalysis,
					SS::CoSeDi::DBMS::CSemanticAnalysis*		pSemanticAnalysis,
					SS::CoSeDi::DBMS::CSemanticClassesTree*	pSemanticClassesTree)
				{
					m_pCashCoSeDi			= pCashCoSeDi;
					m_pCombinatoryAnalysis	= pCombinatoryAnalysis;
					m_pSemanticAnalysis		= pSemanticAnalysis;
					m_pSemanticClassesTree	= pSemanticClassesTree;
				};
				~CCombinatorySemanticAnalysis(void){};
			public:
				bool	SetWorkRegion(SS::CoSeDi::DataTypes::EWorkRegion oeWorkRegion);
				bool	SetWords(T_list_IDSource * p_list_IDSource);
				void	GetCombinations(list<T_list_IDSource> * p_list_list_IDSource);
			private:
				SS::CoSeDi::DBMS::CCombinatoryAnalysis * m_pCombinatoryAnalysis;
				SS::CoSeDi::DBMS::CSemanticAnalysis*		m_pSemanticAnalysis;
				SS::CoSeDi::DBMS::CSemanticClassesTree*	m_pSemanticClassesTree;
				SS::CoSeDi::DBMS::CCashCoSeDi*			m_pCashCoSeDi;

			};
		};	
	};
};
