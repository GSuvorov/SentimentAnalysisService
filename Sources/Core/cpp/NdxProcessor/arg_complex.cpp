#include "StdAfx.h"
#include ".\arg_complex.h"
#include ".\logic.h"
#include ".\arg_res.h"
#include ".\index_container_nodes_s.h"

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

using namespace SS::Core::NdxSE::NdxProcessor::Extracting::Logics;

//--------------------------------------------------------------------//

CComplexValue::CComplexValue(void)
{
}

CComplexValue::~CComplexValue(void)
{
	Reset();
}

void CComplexValue::Reset(void)
{
	TLogicValuesCollection::iterator itLogicValue=m_LogicValuesCollection.begin();
	while(itLogicValue!=m_LogicValuesCollection.end()){
		if(itLogicValue->second){
			//удаляем значение данной логики
			//delete itLogicValue->second;
			itLogicValue->second=NULL;
		}
		++itLogicValue;
	}
}

void CComplexValue::AddLogic(CLogic* pLogic)
{
	if(ISNULL(pLogic)) return;
	m_LogicValuesCollection.push_back(pair<CLogic*, CValue*>(pLogic, (CValue*)0));
}

void CComplexValue::FillComplexValue(CValue* pValue, CComplexArgument* pComplexArgument)
{
	if(ISNULL(pComplexArgument)) return;

	TLogicValuesCollection::iterator itLogicValue=m_LogicValuesCollection.begin();
	while(itLogicValue!=m_LogicValuesCollection.end()){
		if(itLogicValue->first){
			//char buf[100]="";
			//wsprintf(buf, "CreateLogicValue_%u", (unsigned int)itLogicValue->first->GetLogicID());
			//START_TICK(buf);
			//создаем значение данной логики
			itLogicValue->second=itLogicValue->first->CreateLogicValue(pValue, pComplexArgument);
			//STOP_TICK(buf);
		}
		++itLogicValue;
	}
}

void CComplexValue::Calculate(CComplexValue* pChangedValue, CComplexValue* pValue, CLogic::EOperator Operator)
{
	//проходим по логикам и вычисляем по оператору, логики сами что знают что к чему
	TLogicValuesCollection::iterator itChangedLogicValue=pChangedValue->m_LogicValuesCollection.begin();
	TLogicValuesCollection::iterator itLogicValue=pValue->m_LogicValuesCollection.begin();
	while(itChangedLogicValue!=pChangedValue->m_LogicValuesCollection.end() || itLogicValue!=pValue->m_LogicValuesCollection.end()){
		if(itChangedLogicValue->first){
			itChangedLogicValue->first->
				Calculate(&(itChangedLogicValue->second), &(itLogicValue->second), Operator);
		}
		++itLogicValue;
		++itChangedLogicValue;
	}
}

void CComplexValue::FillResultArgument(CResultArgument* pResultArgument)
{
	TLogicValuesCollection::iterator itLogicValue=m_LogicValuesCollection.begin();
	while(itLogicValue!=m_LogicValuesCollection.end()){
		if(itLogicValue->first){
			//логика отдает свою часть вычисленного результату
			itLogicValue->first->ToResultValue(itLogicValue->second, pResultArgument);
		}
		++itLogicValue;
	}
}

Containers::CValue* CComplexValue::GetValueByLogic(Logics::CLogic::ELogicID LogicID)
{
	TLogicValuesCollection::iterator itLogicValue=m_LogicValuesCollection.begin();
	while(itLogicValue!=m_LogicValuesCollection.end()){
		if(itLogicValue->first->GetLogicID()==LogicID){
			return itLogicValue->second;
		}
		++itLogicValue;
	}

	return NULL;
}

void CComplexValue::View(void)
{
	wstring szValue;
	wstring szOffset=L"";

	szValue.append(L"//--------------ComplexValue:\n");

	TLogicValuesCollection::iterator itLogicValue=m_LogicValuesCollection.begin();
	while(itLogicValue!=m_LogicValuesCollection.end()){
		if(itLogicValue->first){
			itLogicValue->first->ToString(szValue, szOffset);
			if(itLogicValue->second){
				itLogicValue->second->ToString(szValue);
			}else{
				szValue.append(L"No value");
			}
			szValue.append(L"\n");
		}
		++itLogicValue;
	}

	szValue.append(L"\n//----------End-of-ComplexValue\n");
	
	ToConsole(szValue.c_str());
}

//--------------------------------------------------------------------//

CComplexArgument::CComplexArgument(unsigned int uiArgumentID, float fKoeff, SS::Interface::Core::CommonContainers::SQueryIndexAttributes ArgumentAttributes)
:CArgument(uiArgumentID, fKoeff, ArgumentAttributes), m_pStartValue(NULL), m_uiBlockID(EMPTY_VALUE)
{
}

CComplexArgument::~CComplexArgument(void)
{
}

void CComplexArgument::SetComplexValue(CComplexValue* pValue)
{	
	//устанавливаем значение
	m_pCurrentValue=pValue;
}

void CComplexArgument::SetValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue)
{
	if(m_pCurrentValue)
		((CComplexValue*)m_pCurrentValue)->FillComplexValue(pValue, this);
}

void CComplexArgument::FillResultArgument(CResultArgument* pResultArgument)
{
	//if(ISNULL(pResultArgument)) return;

	if(m_pCurrentValue)
		((CComplexValue*)m_pCurrentValue)->FillResultArgument(pResultArgument);
}

void CComplexArgument::PopBackSourceValue(void)
{
	if(m_SourceValuesStack.empty()) return;
	
	Containers::CValue* pValue=m_SourceValuesStack.back();
	m_SourceValuesStack.pop_back();
	
	if(pValue){ 
		Containers::CCoordinateNode_4s* pNode=dynamic_cast<Containers::CCoordinateNode_4s*>(pValue);
		pNode->ResetNode();
	}
}

void CComplexArgument::PushBackSourceValue(Containers::CValue* pValue)
{
	m_pStartValue=NULL;
	m_SourceValuesStack.push_back(pValue);
}

void CComplexArgument::InitSourceValuesStack(Containers::CValue* pValue)
{
	m_SourceValuesStack.clear();
	m_pStartValue=pValue;
}

//--------------------------------------------------------------------//

}
}
}
}
}