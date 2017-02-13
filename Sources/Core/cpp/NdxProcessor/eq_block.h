#pragma once
#include ".\eq.h"
#include ".\arg_calc.h"

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
 
///блочная формула, содержит две коллекции блоков аргментов
///аргументы в блоке вычисляются, а блоки между собой вычисляются по И
class CBlockEquation : public CFactoryClient<CEvalFactory>, public CConsoleClient
{
public:
	CBlockEquation(CEvalFactory* pEvalFactory);
	virtual ~CBlockEquation(void);
	
	//--------формирование формулы
	///добавляет в формулу очередной И-блок
	virtual void AddAndBlock(void);
	///добавляет в формулу очередной ИЛИ-блок
	virtual void AddOrBlock(void);
	///добавляет в текущий добавленный блок группу аргументов
	virtual void AddGroup(CArgumentsGroup* pArgumentsGroup);
	//----------------------------

	///вычисляет значения по аргументам
	virtual void Calculate(CResultArgument* pResult);
	///возвращает логику используемую в вычислениях
	Logics::CCompositeLogic* GetEquationLogic(void){return m_pEquationLogic;};

	///готовит формулу к работе
	void IdlePreCalculate(void)
	{
		if(ISNULL(m_pEquationLogic)) return;
		//готовим логику к холостому прогону
		m_pEquationLogic->SetIdleCalculation();
	};

	///прогоняет расчет без вычисления значений
	void IdleCalculate(void)
	{
		if(ISNULL(m_pEquationLogic)) return;
		//сбрасываем холостой прогон у логик
		m_pEquationLogic->UnsetIdleCalculation();
	};

	///предвычисляет инитит логику формулы для работы с данным контайнером результатов
	void PreCalculate(CResultArgument* pResult)
	{
		if(ISNULL(pResult)) return;
		//инитим логику
		m_pEquationLogic->Init(pResult);
	};


	///сериализация в строку
	void ToString(wstring& wszValue);

protected:
	typedef vector<CCalcBlock*> TCalcBlockCollection;
	
	///И-блок формулы
	CCalcBlock* m_pANDBlock;
	///коллекция ИЛИ-блоков формулы
	TCalcBlockCollection m_ORBlockCollection;
	///текущий блок используется при добвалении
	CCalcBlock* m_pCurrentBlock; 
	///логика используемая в вычислениях
	Logics::CCompositeLogic* m_pEquationLogic;
};

//--------------------------------------------------------------------//

///блочная формула, в которой все группы аргументов вычисляются по ИЛИ
///для этого каждая группы кладется в отдельный ИЛИ-блок
class CBlockOREquation : public CBlockEquation
{
public:
	CBlockOREquation(CEvalFactory* pEvalFactory):CBlockEquation(pEvalFactory){};
	virtual ~CBlockOREquation(void){};

	//--------формирование формулы
	///добавляет в формулу очередной И-блок
	void AddAndBlock(void){};
	///добавляет в формулу очередной ИЛИ-блок
	void AddOrBlock(void){};
	///добавляет в текущий добавленный блок группу аргументов
	void AddGroup(CArgumentsGroup* pArgumentsGroup);
	//----------------------------
};

//--------------------------------------------------------------------//

///блочная формула, в которой все группы аргументов вычисляются по И
///для этого все группы кладутся в один И-блок, а ИЛИ-блоков нет вообще
class CBlockANDEquation : public CBlockEquation
{
public:
	CBlockANDEquation(CEvalFactory* pEvalFactory):CBlockEquation(pEvalFactory){};
	virtual ~CBlockANDEquation(void){};

	//--------формирование формулы
	///добавляет в формулу очередной И-блок
	void AddAndBlock(void){};
	///добавляет в формулу очередной ИЛИ-блок
	void AddOrBlock(void){};
	///добавляет в текущий добавленный блок группу аргументов
	void AddGroup(CArgumentsGroup* pArgumentsGroup);
	//----------------------------
};

//--------------------------------------------------------------------//

}
}
}
}
}