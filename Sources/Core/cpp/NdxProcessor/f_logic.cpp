#include "StdAfx.h"
#include ".\f_logic.h"
#include ".\arg_res.h"
#include ".\acc_containers.h"
#include ".\arg_complex.h"

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

using namespace SS::Core::NdxSE::NdxProcessor::Containers;

//--------------------------------------------------------------------//

CFuzzyLogic::CFuzzyLogic(void)
:CLogic(CLogic::elFuzzy), m_fBeta((float)0.7), m_fThreshold((float)0.7)
{
}

CFuzzyLogic::CFuzzyLogic(ELogicID LogicID)
:CLogic(LogicID), m_fBeta((float)0.7), m_fThreshold((float)0.7)
{
}

CFuzzyLogic::~CFuzzyLogic(void)
{
}

CValue* CFuzzyLogic::CreateLogicValue(CValue* pValue, CComplexArgument* pComplexArgument)
{
	//логику проверяем на активность
	if(!IsActive()) return NULL;

	if(ISNULL(pValue)) return NULL;

	CCompositeValue* pCompositeValue=(CCompositeValue*)pValue;
	if(ISNULL(pCompositeValue)) return NULL;

	CAccumulator* pAccumulator=new CAccumulator(pCompositeValue->GetOrtsMaxNumber());
	pAccumulator->m_uinMean++;
	
	//вес входного аргумента
	float fArgumentWeight=pComplexArgument->GetKoeff();

	if(pCompositeValue->MoveFirstOrt()) 
	{
		//текущий орт значения
		unsigned int uiCurrentOrt=0;
		do{ 
			uiCurrentOrt=pCompositeValue->GetCurrentOrt();
			
			//заполняем аккумулятор
			pAccumulator->m_vfMean[uiCurrentOrt]=
				pAccumulator->m_vfMean[uiCurrentOrt]+fArgumentWeight;
			if(pAccumulator->m_vfMax[uiCurrentOrt]<fArgumentWeight) 
				pAccumulator->m_vfMax[uiCurrentOrt]=fArgumentWeight;
		}while(pCompositeValue->MoveNextOrt());
	}

	return pAccumulator;
}

void CFuzzyLogic::ToResultValue(CValue* pValue, CResultArgument* pResultArgument)
{
	//логику проверяем на активность
	if(!IsActive()) return;

	if(ISNULL(pValue)) return;
	if(ISNULL(pResultArgument)) return;
	if(ISNULL(pResultArgument->GetMyself())) return;
	if(ISNULL(pResultArgument->GetMyself()->GetValue())) return;

	//получаем значение результата
	CCompositeValue* pCompositeValue=pResultArgument->GetMyself()->GetValue();
	//получаем акккумулятор с накопленными весами
	CAccumulator* pAccumulator=static_cast<CAccumulator*>(pValue);
	
	//проверяем на совпадение размеров
	if(pAccumulator->m_vfMean.size()!=pCompositeValue->GetOrtsMaxNumber()){
		TO_CONSOLE(L"Accumulator and CompositeValue size not equal");
		return;
	}

	//в цикле проходим по буферу аккумулятора
	for(unsigned int uiCurrentOrt=0; uiCurrentOrt<pAccumulator->m_vfMean.size(); uiCurrentOrt++){
		if(pAccumulator->m_vfMean[uiCurrentOrt]>m_fThreshold){
			//если вес больше порога выставляем орт	
			pCompositeValue->SetOrt(uiCurrentOrt);
		}
	}
}

void CFuzzyLogic::MakeAnd(CValue** ppChangedValue, CValue** ppValue)
{
	//логику проверяем на активность
	if(!IsActive()) return;

	if(ISNULL(ppChangedValue)) return;
	if(ISNULL(ppValue)) return;

	//если оба не валидны ни че не делаем
	if(!(*ppChangedValue) && !(*ppValue)) return;

	if((*ppChangedValue) && (*ppValue)){
		//если оба значения валидны выполняем операцию
		((CAccumulator*)(*ppValue))->EvalA4ANDFromAccumulator(m_fBeta);
		((CAccumulator*)(*ppChangedValue))->MergeAccumulators(*((CAccumulator*)(*ppValue)));
	}else if(!(*ppValue)){
		//если только изменяемое значение валидно, сбрасываем свое значение

	}else if(!(*ppChangedValue)){
		//если только изменяемое значение не валидно, 
		//отдаем неизменяемое значение и сбрасываем его
		(*ppChangedValue)=(*ppValue);
		(*ppValue)=NULL;
	}
}

void CFuzzyLogic::MakeOr(CValue** ppChangedValue, CValue** ppValue)
{
	//логику проверяем на активность
	if(!IsActive()) return;

	//если неизменяемое значение не валидно ни че не делаем 
	if(ISNULL(ppChangedValue)) return;
	if(ISNULL(ppValue)) return;

	//если оба не валидны ни че не делаем
	if(!(*ppValue)) return;

	if((*ppChangedValue) && (*ppValue)){
		//если оба значения валидны выполняем операцию
		((CAccumulator*)(*ppValue))->EvalA4ORFromAccumulator(m_fBeta);
		((CAccumulator*)(*ppChangedValue))->MergeAccumulators(*((CAccumulator*)(*ppValue)));
	}else if(!(*ppChangedValue)){
		//если только изменяемое значение не валидно, 
		//отдаем неизменяемое значение и сбрасываем его
		(*ppChangedValue)=(*ppValue);
		(*ppValue)=NULL;
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}
}