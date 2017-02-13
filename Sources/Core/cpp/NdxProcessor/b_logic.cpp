#include "StdAfx.h"
#include ".\b_logic.h"
#include ".\console.h"
#include ".\arg_res.h"
#include ".\arg_complex.h"
#include ".\box_containers_s.h"

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

CBooleanLogic::CBooleanLogic(void)
:CLogic(CLogic::elBoolean), m_fResultMaxWeight(0.0)
{
}

CBooleanLogic::CBooleanLogic(ELogicID LogicID)
:CLogic(LogicID), m_fResultMaxWeight(0.0)
{
}

CBooleanLogic::~CBooleanLogic(void)
{
}

CValue* CBooleanLogic::CreateLogicValue(CValue* pValue, CComplexArgument* pComplexArgument)
{
	if(ISNULL(pComplexArgument)) return NULL;
	CValue* pNewValue=NULL;

	if(IsIdleCalculation()){
		if(pValue){
			//входное значение обязательно должно быть не валидным
			TO_CONSOLE(L"pValue is valid in IdleCalculation");
			return NULL;
		}
		
		//создаем простое значение
		pNewValue=new CValue();	
		//устанавливаем вес значения
		pNewValue->SetValueWeight(pComplexArgument->GetKoeff());

	}else{
		if(pValue==NULL) return NULL;
		//логику проверяем на активность
		if(!IsActive()) return NULL;
		
		pNewValue=new CBitMap_4s(((CCompositeValue*)pValue)->GetOrtsMaxNumber());

		if(pNewValue){
			//создаем копию со значения
			pNewValue->MakeOr(pValue);
			//устанавливаем вес значения
			pNewValue->SetValueWeight(pComplexArgument->GetKoeff());
		}
	}

	//добаваляем ссылку себе чтоб потом удалить
	AddToValuesCollection(pNewValue);

	return pNewValue;
}

void CBooleanLogic::ToResultValue(CValue* pValue, CResultArgument* pResultArgument)
{
	if(IsIdleCalculation()){
		if(ISNULL(pValue)) return;

		//если был холостой прогон, то в результирующем значении максимальный вес сохраняем его
		m_fResultMaxWeight=pValue->GetValueWeight();
		m_fResultMaxWeight=(m_fResultMaxWeight*GetActivationLevelsNumber());
		//char buf[50];
		//sprintf(buf, "ResultMaxWeight: %f", m_fResultMaxWeight);
		//ToConsole(buf);

	}else{

		//логику проверяем на активность
		if(!IsActive()) return;

		if(ISNULL(pResultArgument)) return;
		if(ISNULL(pResultArgument->GetMyself())) return;
		if(ISNULL(pResultArgument->GetMyself()->GetValue())) return;

		if(pValue){
			//получаем значение результата
			CCompositeValue* pCompositeValue=pResultArgument->GetMyself()->GetValue();

			//pValue->View();

			//подливаем значение к результату
			if(pCompositeValue->IsEmpty())
				pCompositeValue->MakeOr(pValue);
			else
				pCompositeValue->MakeAnd(pValue);

			//добавляем результату вес значения
			pResultArgument->GetMyself()->SetKoeff(
				pResultArgument->GetMyself()->GetKoeff()+pValue->GetValueWeight());

			if(pResultArgument->GetDepth()==GetLastActivationLevel()){
				///если последний активный уровень переводим вес в процентный от максимального
				pResultArgument->GetMyself()->SetKoeff(EvalResultWeight(pResultArgument->GetMyself()->GetKoeff()));
			}

			//char buf[50];
			//sprintf(buf, "ResultArgument: %f", pResultArgument->GetMyself()->GetKoeff());
			//ToConsole(buf);

			//pCompositeValue->View();
		}else{
			pResultArgument->GetMyself()->GetValue()->Reset();
		}
	}
	
	//удаляем созданные значения
	ClearValuesCollection();
}

void CBooleanLogic::MakeAnd(CValue** ppChangedValue, CValue** ppValue)
{
	//логику проверяем на активность
	if(!IsActive()) return;

	if(ISNULL(ppChangedValue)) return;
	if(ISNULL(ppValue)) return;

	//если оба не валидны ни че не делаем
	if(!(*ppChangedValue) && !(*ppValue)) return;

	//START_TICK(L"CBooleanLogic::MakeAnd");

	if((*ppChangedValue) && (*ppValue)){
		//если оба значения валидны выполняем операцию
		(*ppChangedValue)->MakeAnd((*ppValue));
	}else if(!(*ppValue)){
		//если только изменяемое значение валидно, сбрасываем свое значение
		(*ppChangedValue)->Reset();
	}else if(!(*ppChangedValue)){
		//если только изменяемое значение не валидно, 
		//отдаем неизменяемое значение и сбрасываем его
		(*ppChangedValue)=(*ppValue);
		(*ppChangedValue)->Reset();
		(*ppValue)=NULL;
	}
	//STOP_TICK(L"CBooleanLogic::MakeAnd");
}

void CBooleanLogic::MakeOr(CValue** ppChangedValue, CValue** ppValue)
{
	//логику проверяем на активность
	if(!IsActive()) return;

	if(ISNULL(ppChangedValue)) return;
	if(ISNULL(ppValue)) return;

	//если неизменяемое значение не валидно ни че не делаем 
	if(!(*ppValue)) return;

	//START_TICK(L"CBooleanLogic::MakeOr");
	if((*ppChangedValue) && (*ppValue)){
		
		//ToConsole("Before");
		//(*ppChangedValue)->View();
		//(*ppValue)->View();
		//ToConsole("eof-Before");

		//если оба значения валидны выполняем операцию
		(*ppChangedValue)->MakeOr((*ppValue));

		//ToConsole("After");
		//(*ppChangedValue)->View();
		//ToConsole("eof-After");

	}else if(!(*ppChangedValue)){
		//если только изменяемое значение не валидно, 
		//отдаем неизменяемое значение
		(*ppChangedValue)=(*ppValue);
		(*ppValue)=NULL;
	}

	//STOP_TICK(L"CBooleanLogic::MakeOr");
}

//--------------------------------------------------------------------//

CBooleanLogicWithSentencesAdjacent::CBooleanLogicWithSentencesAdjacent(int iSizeAdjacent/*, SSearchParameter::EAdjacentSearchIntervalType IntervalType*/)
:m_iSizeAdjacent(iSizeAdjacent)/*, 
m_IntervalType(IntervalType)*/
{
}

CBooleanLogicWithSentencesAdjacent::~CBooleanLogicWithSentencesAdjacent(void)
{
}

void CBooleanLogicWithSentencesAdjacent::MakeAnd(CValue** ppChangedValue, CValue** ppValue)
{
	//логику проверяем на активность
	if(!IsActive()) return;

	//если оба не валидны ни че не делаем
	if(!(*ppChangedValue) && !(*ppValue)) return;

	if((*ppChangedValue) && (*ppValue)){
		//если оба значения валидны выполняем операцию, сливания с учетом смежности
		(*ppChangedValue)->MakeSmoothAnd((*ppValue), (unsigned int)m_iSizeAdjacent);
	}else if(!(*ppValue)){
		//если только изменяемое значение валидно, сбрасываем свое значение
		(*ppChangedValue)->Reset();
	}else if(!(*ppChangedValue)){
		//если только изменяемое значение не валидно, 
		//отдаем неизменяемое значение и сбрасываем его
		(*ppChangedValue)=(*ppValue);
		(*ppChangedValue)->Reset();
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}
}