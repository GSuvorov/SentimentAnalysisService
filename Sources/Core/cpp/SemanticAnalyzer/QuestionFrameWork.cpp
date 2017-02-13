#include "StdAfx.h"
#include ".\questionframework.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{


CQuestionFrameWork::CQuestionFrameWork(void)
{
}

CQuestionFrameWork::~CQuestionFrameWork(void)
{
}

SS::Core::Features::CMorphoFeature * CQuestionFrameWork::GetUnitMorphoFeature(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(!pUnit) return NULL;
	CQuestionFrameWork::TUMF::iterator i;
	i=m_UnitMorphoFeature.find(pUnit);
	if(i!=m_UnitMorphoFeature.end())
		return &(i->second);
	return NULL;
}
void CQuestionFrameWork::AddUnitMorphoFeature(SS::Interface::Core::BlackBox::IUnit* pUnit,SS::Core::Features::CMorphoFeature & oMorphoFeature )
{
	if(!pUnit) return ;
	m_UnitMorphoFeature.insert(CQuestionFrameWork::TUMF::value_type(pUnit,oMorphoFeature));
}

SS::Core::Features::CSemanticFeature * CQuestionFrameWork::GetUnitSemanticFeature(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(!pUnit) return NULL;
	CQuestionFrameWork::TUSF::iterator i;
	i=m_UnitSemanticFeature.find(pUnit);
	if(i!=m_UnitSemanticFeature.end())
		return &(i->second);
	return NULL;
}
void CQuestionFrameWork::AddUnitSemanticFeature(SS::Interface::Core::BlackBox::IUnit* pUnit,SS::Core::Features::CSemanticFeature & oSemanticFeature )
{
	if(!pUnit) return ;
	m_UnitSemanticFeature.insert(CQuestionFrameWork::TUSF::value_type(pUnit,oSemanticFeature));
}

SMorphoDescription * CQuestionFrameWork::GetUnitDescription(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if(!pUnit) return NULL;
	CQuestionFrameWork::TUMD::iterator i;
	i=m_UnitDescription.find(pUnit);
	if(i!=m_UnitDescription.end())
		return &(i->second);
	return NULL;
}
void CQuestionFrameWork::AddUnitDescription(SS::Interface::Core::BlackBox::IUnit* pUnit,SMorphoDescription & oDescription )
{
	if(!pUnit) return ;
	m_UnitDescription.insert(CQuestionFrameWork::TUMD::value_type(pUnit,oDescription));
}


void CQuestionFrameWork::Clear()
{
	m_UnitMorphoFeature.clear();
	m_UnitSemanticFeature.clear();
	m_UnitDescription.clear();
}


			}
		}
	}
}