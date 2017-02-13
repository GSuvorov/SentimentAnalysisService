#pragma once
#include ".\factory_client.h"
#include ".\cnt_factory.h"
#include ".\lgc_usings.h"

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

class CCalcEquation;
class CBlockEquation;
class CSubster;
class CResultsContainer;
class CResultEquation;
class CResultArgument;
class CResultsChecker;
class CEvaluator;

//--------------------------------------------------------------------//

///фабрика для создания объектов для вычисления
class CEvalFactory : public CFactoryClient<SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory>
{
public:
	CEvalFactory(SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory* pContainersFactory);
	virtual ~CEvalFactory(void);

	///создает составную логику
	virtual Logics::CCompositeLogic* CreateEquationLogic(void);
	///создает вычислительную формулу
	virtual CBlockEquation* CreateCalcEquation(void);
	///создает формулу результатов
	virtual CResultEquation* CreateResultEquation(void);
	///создает проверщик результатов
	virtual CResultsChecker* CreateResultsChecker(unsigned int uiValidResultsDepth);
	///создает подстановщик
	virtual CSubster* CreateSubster(void);
	///создает контейнер результатов
	virtual CResultsContainer* CreateResultsContainer(void);
	///создает вычислитель
	virtual CEvaluator* CreateEvaluator(void);
	///создает аргумент формулы результатов
	virtual CResultArgument* CreateResultArgument(unsigned int uiArgumentID, float fArgumentWeight, unsigned int uiArgumentDepth);
};

//--------------------------------------------------------------------//

}
}
}
}
}