#include "StdAfx.h"
#include ".\logic.h"
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

using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;

//--------------------------------------------------------------------//

void CLogic::Calculate(SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppChangedValue, 
					   SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppValue, 
					   CLogic::EOperator Operator)
{
	//логику проверяем на активность
	if(!IsActive()) return;

	switch(Operator){
	case CLogic::eoAND:
		MakeAnd(ppChangedValue, ppValue);
		break;
	case CLogic::eoOR:
		MakeOr(ppChangedValue, ppValue);
		break;
	default:
		TO_CONSOLE(L"Unsupport operation!!!");
		break;
	}
}

void CLogic::Init(CResultArgument* pResultArgument)
{
	if(!pResultArgument){
		m_bLogicActive=false;
	}else{
		m_bLogicActive=IsActivationLevel(pResultArgument->GetDepth());
	}
}

void CLogic::SetActivationLevels(unsigned char ucActivationLevelFirst, unsigned char ucActivationLevelLast)
{
	for(unsigned char i=ucActivationLevelFirst; i<=ucActivationLevelLast; i++){
		SetActivationLevel(i);
	}
}

void CLogic::SetActivationLevel(unsigned char ucActivationLevel)
{
	if(ucActivationLevel<32){
		unsigned int uiValue=1;
		uiValue=uiValue<<ucActivationLevel;

		if(!(m_uiActivationLevels&uiValue)){
			//количество уровней активности логики 
			m_ucActivationLevelsNumber++;
		}

		m_uiActivationLevels|=uiValue;
		
		if(ucActivationLevel>m_ucLastActivationLevel){
			//фиксируем номер последнего уровня активности логики 
			m_ucLastActivationLevel=ucActivationLevel;
		}

	}else{
		TO_CONSOLE(L"Too big activation level");
	}
}

bool CLogic::IsActivationLevel(unsigned char ucLevel)
{
	if(ucLevel<32){
		unsigned int uiValue=1;
		uiValue=uiValue<<ucLevel;
		return ((m_uiActivationLevels&uiValue)!=0)?true:false;
	}else{
		TO_CONSOLE(L"Too big level");
		return false;
	}
}

void CLogic::ToString(wstring& szValue, wstring szOffset)
{
	wstring szMyOffset(L"   ");
	szValue.append(szOffset);
	szValue.append(L"//-----CLogic:");
	
	szValue.append(L"\n"+szOffset+szMyOffset);

	wchar_t buff[255];
	switch(m_LogicID){
	case elBoolean:
		wsprintf(buff, L"LogicID: elBoolean");
		break;
	case elRelevance:
		wsprintf(buff, L"LogicID: elRelevance");
		break;
	case elFuzzy:
		wsprintf(buff, L"LogicID: elFuzzy");
		break;
	case elTextStatistic:
		wsprintf(buff, L"LogicID: elTextStatistic");
		break;
	case elSentenceAdjust:
		wsprintf(buff, L"LogicID: elSentenceAdjust");
		break;
	default:
		TO_CONSOLE(L"Undefined ID!!!");
		break;
	}
	szValue.append(buff);

	szValue.append(L"\n");
	szValue.append(szOffset);
	szValue.append(L"//-End-of-CLogic\n");
}

void CLogic::ClearValuesCollection(void)
{
	for(TCreatedValuesCollection::iterator itValue=m_CreatedValuesCollection.begin(); itValue!=m_CreatedValuesCollection.end(); itValue++){
		if(*itValue){
			delete (*itValue);
		}
	}
	m_CreatedValuesCollection.clear();
}

void CLogic::AddToValuesCollection(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue)
{
	if(pValue)
		m_CreatedValuesCollection.push_back(pValue);
}
//--------------------------------------------------------------------//

}
}
}
}
}
}