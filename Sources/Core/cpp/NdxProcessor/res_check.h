#pragma once
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

//--------------------------------------------------------------------//

///проверщик результатов 
class CResultsChecker : public CConsoleClient
{
public:
	CResultsChecker(unsigned int uiValidResultsDepth)
		:m_uiValidResultsDepth(uiValidResultsDepth){};
	virtual ~CResultsChecker(void){};
	
	///инициализация проверщика
	virtual void Init(void){};
	///проверяет результат на корректность
	virtual bool CheckResult(CResultArgument* pArgument, CArgument** ppCheckedArgument);
	///возвращает глубину валидных результатов
	unsigned int GetValidResultsDepth(void){return m_uiValidResultsDepth;}; 
	///отображает содержимое объекта класса
	void View(void);

protected:
	///глубина валидных результатов
	const unsigned int m_uiValidResultsDepth; 
};

//--------------------------------------------------------------------//

}
}
}
}
}