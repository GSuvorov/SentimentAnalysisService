#include "StdAfx.h"
#include ".\res_check.h"

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

bool CResultsChecker::CheckResult(CResultArgument* pArgument, CArgument** ppCheckedArgument)
{
	if(ISNULL(pArgument)) return false;
	if(ISNULL(ppCheckedArgument)) return false;

	//проверка значения аргумента 
	//на пустоту 
	//на последовательность
	//на превышение глубины

	if(pArgument->GetMyself()->GetValue()->IsEmpty() ||
		pArgument->GetDepth()>=m_uiValidResultsDepth){
		(*ppCheckedArgument)=NULL;
		return true;
	}else{
		(*ppCheckedArgument)=pArgument->GetMyself();
		return true;
	}
}

void CResultsChecker::View(void)
{
	ToConsole(L"//---CResultsChecker:");
	ToConsole(L"ValidResultsDepth", m_uiValidResultsDepth+1);
	ToConsole(L"//---End-of-CResultsChecker");
}

//--------------------------------------------------------------------//

}
}
}
}
}