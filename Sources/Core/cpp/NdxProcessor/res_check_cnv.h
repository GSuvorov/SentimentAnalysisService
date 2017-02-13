#pragma once
#include ".\res_check.h"
#include ".\sr_filler.h"

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

///проверщик результатов с возможностью фонового конвертирования результатов 
//в линейные и с подсчетом и ограничением на количество результатов
class CResultsCheckerWithConvertion : public CResultsChecker
{
public:
	CResultsCheckerWithConvertion(unsigned int uiValidResultsDepth = 0, 
		unsigned int uiFinalResultsMaxNumber = 0,
		CSearchResultFiller* pSearchResultFiller = NULL);
	virtual ~CResultsCheckerWithConvertion(void);

	///инициализация проверщика
	void Init(void){m_uiFinalResultsCurrentNumber=0;};
	///проверяет результат на корректность
	bool CheckResult(CResultArgument* pArgument, CArgument** ppCheckedArgument);
	///отображает содержимое объекта класса
	void View(void);

protected:
	///конвертор результатов в линейные
	CSearchResultFiller* m_pSearchResultFiller;
	///текущее количество порожденных окончательных результатов
	unsigned int m_uiFinalResultsCurrentNumber;
	///максимальное число порожденных окончательных результатов
	const unsigned int m_uiFinalResultsMaxNumber;
};
//--------------------------------------------------------------------//

///проверщик результатов с возможностью фонового конвертирования результатов 
//в линейные с подсчетом и ограничением на количество результатов,
//используется при вычисленнии интервалов
class CResultsCheckerWithIntervalConvertion : public CResultsCheckerWithConvertion
{
public:
	CResultsCheckerWithIntervalConvertion(unsigned int uiValidResultsDepth = 0, 
		unsigned int uiFinalResultsMaxNumber = 0,
		CSearchResultFiller* pSearchResultFiller = NULL);

	///проверяет результат на корректность
	bool CheckResult(CResultArgument* pArgument, CArgument** ppCheckedArgument);
};

//--------------------------------------------------------------------//

///проверщик результатов с возможностью фонового конвертирования результатов 
//в линейные с подсчетом и ограничением на количество результатов,
//используется при вычисленнии интервалов
class CResultsCheckerWithHIDConvertion: public CResultsCheckerWithConvertion
{
public:
	CResultsCheckerWithHIDConvertion(unsigned int uiValidResultsDepth = 0, 
		unsigned int uiFinalResultsMaxNumber = 0,
		CSearchResultFiller* pSearchResultFiller = NULL);

	///проверяет результат на корректность
	bool CheckResult(CResultArgument* pArgument, CArgument** ppCheckedArgument);
};

//--------------------------------------------------------------------//

}
}
}
}
}