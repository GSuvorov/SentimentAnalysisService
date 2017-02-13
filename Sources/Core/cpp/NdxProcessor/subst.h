#pragma once
#include ".\arg.h"
#include ".\res.h"
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

///подстановщик - подставляет аргумента значения согласно, своего алгоритма
/**инитится формулой, создает себе коллекцию аргументов, которым потом подставляются значения 
без участия формулы*/
class CSubster : public CConsoleClient
{
public:
	CSubster(void);
	virtual ~CSubster(void);
	
	///подставляет значения аргументам
	void IdleSubst(void);
	///начинает процесс подстановки значений аргументам
	void StartSubsting(void);
	///подставляет значения аргументам
	bool SubstValues(void);
	///обрабатывает результат
	void EvaluateResult(CArgument* pResult);
	///возвращает коллекцию аргументов с соответствующим им наборами значений
	TArgValCollection* GetArgValCollection(void){return &m_ArgValCollection;};
	///сбрасывает содержимое
	void Reset(void);

	///отображает содержимое объекта класса
	void View(void);
	///отображает содержимое коллекции аргумент-значения
	void ViewArgValCollection(void);

protected:
	///стек вычисляемых результатов 
	CResultsStack m_ResultsStack;
	///коллекция аргументов с соответствующим им наборами значений
	TArgValCollection m_ArgValCollection;
};

//--------------------------------------------------------------------//

}
}
}
}
}