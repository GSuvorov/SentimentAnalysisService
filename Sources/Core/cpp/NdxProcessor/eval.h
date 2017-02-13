#pragma once

#include ".\eq_calc.h"
#include ".\eq_block.h"
#include ".\subst.h"
#include ".\res.h"
#include ".\ev_factory.h"
#include ".\es_filler.h"
#include ".\consoleclient.h"

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

///управляет процессом наполнения контейнера результатов
class CEvaluator : public CFactoryClient<CEvalFactory>, public CConsoleClient
{
public:
	CEvaluator(CEvalFactory* pEvalFactory);
	virtual ~CEvaluator(void);
	
	///инициализирует вычислитель
	void Init(CESFiller* pESFiller);
	///запускает процесс расчета
	bool Calculate(void);
	///продолжает процесс расчета
	bool CalculateNext(void);
	///забирает контейнер результатов у вычислителя
	CResultsContainer* ExtractResultsContainer(void);

	///отображает содержимое объекта класса
	void View(void);
	///отображает только контайнер результатов
	void ViewResultsContainer(void);
	
protected:
	///формула по которой ведуться вычисления
	//CCalcEquation* m_pEquation;
	CBlockEquation* m_pEquation;
	///подстановщик, подставляет аргументам формулы значения
	CSubster* m_pSubster;
	///контейнер результатов
	CResultsContainer* m_pResultsContainer;
	///текущий аргумент контейнера результатов
	CResultArgument* m_pCurrResultArgument;

};

//--------------------------------------------------------------------//

}
}
}
}
}