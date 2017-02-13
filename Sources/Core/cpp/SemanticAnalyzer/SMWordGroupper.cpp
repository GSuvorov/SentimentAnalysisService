#include "StdAfx.h"
#include ".\smwordgroupper.h"

namespace SS
{
	namespace Semantic
	{
		namespace PreAnalysisExperts
		{


CSMWordGroupper::CSMWordGroupper(void)
{
	m_sDescription=_T(__FUNCTION__);
}

CSMWordGroupper::~CSMWordGroupper(void)
{
}
bool CSMWordGroupper::AnalizeSentence()
{
	SS_TRY
	{
		TBoxUnits l_units;

		do
		{
			if ( SpecialTypeEquals(ustUndefined) && /*!IsSyntax()*/!IsSyntax() && 
				m_LexemType.IsWord()
				//( m_LexemType.IsENG() || m_LexemType.IsEng() ||m_LexemType.IsEnG()|| m_LexemType.IsRus() )
				)
			{
				do 
				{
					l_units.push_back(m_pCurrentUnit);
				}while( GetNextUnit()&& m_LexemType.IsWord()&&
					//(m_LexemType.IsENG() || m_LexemType.IsEng() ||m_LexemType.IsEnG()|| m_LexemType.IsRus() ) &&
					SpecialTypeEquals(ustUndefined) && !IsSyntax()
					);
				if (l_units.size()>1)
				{
					//SetSpecialType(l_units,ustProperGroup);
					IUnit * pHead=JoinUpUnits(l_units);
					SetSpecialType(ustProperGroup);
					m_pCurrentUnit=pHead;
				}
				l_units.clear();
			}
		}while( GetNextUnit() );
		return true;
	}
	SS_CATCH(L"")

}


		}
	}
}