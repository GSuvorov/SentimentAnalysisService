#pragma once

#include ".\consoleclient.h"
#include ".\lgc_usings.h"
#include ".\ev_factory.h"

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

///формула - коллекция аргументов, связанных в дерево
template <class TArgument, class TResult>
class CEquation : public CFactoryClient<CEvalFactory>, public CConsoleClient
{
public:
	CEquation(CEvalFactory* pEvalFactory, TArgument* pRootArgument)
		:CFactoryClient<CEvalFactory>(pEvalFactory), m_pRootArgument(pRootArgument), m_pEquationLogic(NULL)
	{
	};

	virtual ~CEquation(void)
	{
		if(m_pRootArgument)
			delete m_pRootArgument;

		if(m_pEquationLogic)
			delete m_pEquationLogic;
	};
	
	///готовит формулу к работе
	void IdlePreCalculate(void)
	{
		if(ISNULL(m_pRootArgument)) return;
		if(ISNULL(m_pEquationLogic)) return;
		
		//готовим логику к холостому прогону
		m_pEquationLogic->SetIdleCalculation();
	};

	///прогоняет расчет без вычисления значений
	void IdleCalculate(void)
	{
		if(ISNULL(m_pRootArgument)) return;
		if(ISNULL(m_pEquationLogic)) return;
		
		//запускаем холостые вычисления
		m_pRootArgument->Calculate(NULL);

		//сбрасываем холостой прогон у логик
		m_pEquationLogic->UnsetIdleCalculation();
	};

	///предвычисляет инитит логику формулы для работы с данным контайнером результатов
	void PreCalculate(TResult* pResult)
	{
		if(ISNULL(pResult)) return;
		if(ISNULL(m_pRootArgument)) return;
		if(ISNULL(m_pEquationLogic)) return;
		
		//инитим логику
		m_pEquationLogic->Init(pResult);
	};

	///вычисляет значения по аргументам
	void Calculate(TResult* pResult)
	{
		if(ISNULL(pResult)) return;
		if(ISNULL(m_pRootArgument)) return;
		if(ISNULL(m_pEquationLogic)) return;
		
		//запсукаем вычисления
		m_pRootArgument->Calculate(pResult);
	};

	///возвращает корневой аргумент формулы
	TArgument* GetRootArgument(void){return m_pRootArgument;};
	///возвращает логику работы формулы
	Logics::CCompositeLogic* GetEquationLogic(void){return m_pEquationLogic;};

	///отображает содержимое объекта класса
	void View(void)
	{
		wstring szValue;
		wstring szOffset;
		
		//смотрим дерево
		ToConsole(L"//----------Equation:");
		szValue.reserve(1000);
		szOffset.reserve(100);
		if(m_pRootArgument)
			m_pRootArgument->ToString(szValue, szOffset);

		ToConsole(szValue.c_str());
		ToConsole(L"//------End-of-Equation");
	}

protected:
	///корневой аргумент формулы
	TArgument* m_pRootArgument;
	///логика используемая формулой в вычислениях
	Logics::CCompositeLogic* m_pEquationLogic;
};

//--------------------------------------------------------------------//

}
}
}
}
}