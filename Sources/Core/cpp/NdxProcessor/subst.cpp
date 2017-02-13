#include "StdAfx.h"
#include ".\subst.h"
#include ".\console.h"

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

//--------------------------------------------------------------------//

CSubster::CSubster(void)
{
}

CSubster::~CSubster(void)
{
	Reset();

	//удаляем коллекцию аргументов с соответствующим им наборами значений
	TArgValCollection::iterator itArgValues=m_ArgValCollection.begin();
	while(itArgValues!=m_ArgValCollection.end()){
		
		if((*itArgValues)){
			delete (*itArgValues);
		}
		++itArgValues;
	}
}

void CSubster::Reset(void)
{
	//очищаем стек
	m_ResultsStack.Clear();
}

void CSubster::IdleSubst(void)
{
	//в цикле проходим по контейнерам, выставляем значения в коллекциях результата
	TArgValCollection::iterator itArgValues=m_ArgValCollection.begin();
	while(itArgValues!=m_ArgValCollection.end()){
		if(ISNULL((*itArgValues))) break;
	
		//текущий контейнер выставляет свежие значения, всех коллекции результата		
		(*itArgValues)->GetArgument()->SetValue(NULL);

		++itArgValues;
	}
}

void CSubster::StartSubsting(void)
{
	//в цикле проходим по контейнерам, выставляем значения в коллекциях результата
	SS::Core::NdxSE::NdxProcessor::Containers::TValuePtrsBuffer ValuePtrsBuffer(1);
	TArgValCollection::iterator itArgValues=m_ArgValCollection.begin();
	while(itArgValues!=m_ArgValCollection.end()){
		
		if(ISNULL((*itArgValues))) break;
		
		//текущий контейнер выставляет свежие значения, всех коллекции результата		
		(*itArgValues)->SetFreshValues(&ValuePtrsBuffer);
		//(*itArgValues)->GetArgument()->SetValue(ValuePtrsBuffer.front());
		(*itArgValues)->GetArgument()->InitSourceValuesStack(ValuePtrsBuffer.front());
		(*itArgValues)->SetLastUsedValue(ValuePtrsBuffer.front());
		ValuePtrsBuffer[0]=NULL;	
		
		++itArgValues;
	}
}

bool CSubster::SubstValues()
{

	//если стек пустой и подставлять нечего выходим
	if(m_ResultsStack.IsEmpty()) return false;
	
	if(ISNULL(m_ResultsStack.GetBackResult())) return false;
	
	//убираем из стека отработанные результаты
	while(!m_ResultsStack.IsEmpty() && m_ResultsStack.GetBackResult()->GetCurrentEvalID()==EMPTY_VALUE){
		m_ResultsStack.PopBackResult();
		//вместе с результом корректируем стеки аргументов
		for(TArgValCollection::iterator itArgVal=m_ArgValCollection.begin(); itArgVal!=m_ArgValCollection.end(); itArgVal++){
			(*itArgVal)->GetArgument()->PopBackSourceValue();
		}
	}

	//m_ResultsStack.View();

	//если стек опустел все... подставлять нечего выходим
	if(m_ResultsStack.IsEmpty()) return false;
	
	////вызываем для текущей коллекции аргументов со значениями подстановка
	//m_ResultsStack.GetBackResult()->GetCurrentArgValueCollection()->TriggerArguments();

	return true;
}

void CSubster::EvaluateResult(CArgument* pResult)
{
	//получаем крайние значения аргументов и корректируем их по входному результату
	if(!m_ResultsStack.IsEmpty()){
		if(ISNULL(m_ResultsStack.GetBackResult())) return;
		if(m_ResultsStack.GetBackResult()->GetCurrentEvalID()==EMPTY_VALUE) return;
		
		m_ResultsStack.GetBackResult()->MoveNextEvalID();
	}

	if(pResult){
		//если входной результат валидный
		//создаем новый результат стека и добавляем результат в стек	
		m_ResultsStack.PushBackResult(new CResult(pResult));
	}else{
		//если входной результат не валидный, нужно выкинуть последний значения из стеков аргументов
		for(TArgValCollection::iterator itArgVal=m_ArgValCollection.begin(); itArgVal!=m_ArgValCollection.end(); itArgVal++){
			(*itArgVal)->GetArgument()->PopBackSourceValue();
		}
	}
}

void CSubster::View(void)
{
	ToConsole(L"//-----------------Subster:");

	ViewArgValCollection();
	m_ResultsStack.View();

	ToConsole(L"//-------------End-of-Subster");
}

void CSubster::ViewArgValCollection(void)
{
	wstring szValue(L"");
	wstring szOffset(L"");
	szValue.reserve(1000);
	szValue.reserve(100);

	szValue.append(L"//-ArgValCollection:");
	
	for(TArgValCollection::iterator itArgVal=m_ArgValCollection.begin(); itArgVal!=m_ArgValCollection.end(); itArgVal++){
		szValue.append(L"\n\t");
		(*itArgVal)->GetArgument()->ToString(szValue);
		szValue.append(L"\n\t");
		(*itArgVal)->GetValuesContainer()->ToString(szValue);
	}
	szValue.append(L"//-End-of-ArgValCollection:");
	ToConsole(szValue.c_str());
}

//--------------------------------------------------------------------//

}
}
}
}
}