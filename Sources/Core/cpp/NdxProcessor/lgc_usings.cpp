#include "StdAfx.h"
#include ".\lgc_usings.h"
#include ".\arg_res.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Extracting
{
namespace Logics
{

using namespace SS::Core::NdxSE::NdxProcessor::Extracting;

//--------------------------------------------------------------------//

CCompositeLogic::CCompositeLogic(void)
{
}

CCompositeLogic::~CCompositeLogic(void)
{
	//удаляем логику
	while(!m_LogicCollection.empty()){
		if(m_LogicCollection.back())
			delete m_LogicCollection.back();
		m_LogicCollection.pop_back();
	}
}

void CCompositeLogic::Init(CResultArgument* pResultArgument)
{
	if(ISNULL(pResultArgument)) return;
	
	//инитим логику
	TLogicCollection::iterator itLogic=m_LogicCollection.begin();
	while(itLogic!=m_LogicCollection.end()){
		if((*itLogic)){
			//если на входе нормальный контайнер результатов, готовим логики для вычислений
			(*itLogic)->Init(pResultArgument);
		}		
		itLogic++;
	}
}

void CCompositeLogic::SetIdleCalculation(void)
{
	TLogicCollection::iterator itLogic=m_LogicCollection.begin();
	while(itLogic!=m_LogicCollection.end()){
		if((*itLogic)){
			//готовим логики для холостого прогона
			(*itLogic)->SetIdleCalculation();
		}
		itLogic++;
	}
}

void CCompositeLogic::UnsetIdleCalculation(void)
{
	TLogicCollection::iterator itLogic=m_LogicCollection.begin();
	while(itLogic!=m_LogicCollection.end()){
		if((*itLogic)){
			//сбрасываем флаги холостого прогона
			(*itLogic)->UnsetIdleCalculation();
		}
		itLogic++;
	}
}

void CCompositeLogic::AddLogic(CLogic* pLogic)
{
	if(ISNULL(pLogic)) return;

	//добавляем логику
	m_LogicCollection.push_back(pLogic);
}

CComplexValue* CCompositeLogic::CreateComplexValue(void)
{
	//создаем сложное значение
	CComplexValue* pComplexValue=new CComplexValue();
	
	if(ISNULL(pComplexValue)) return NULL;

	TLogicCollection::iterator itLogic=m_LogicCollection.begin();
	while(itLogic!=m_LogicCollection.end()){
		if(ISNULL((*itLogic))) break;
		
		//добавляем значению логику
		pComplexValue->AddLogic(*itLogic);

		itLogic++;
	}
	
	AddToValuesCollection(pComplexValue);

	return pComplexValue;
}

//--------------------------------------------------------------------//

}
}
}
}
}
}