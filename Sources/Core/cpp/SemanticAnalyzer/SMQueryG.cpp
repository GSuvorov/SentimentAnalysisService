#include "StdAfx.h"
#include ".\smqueryg.h"
#include ".\smquerygenerator.h"
#include ".\smverbformcorrector.h"

namespace SS
{
	namespace Semantic
	{
		namespace Groups
		{



CSMQueryG::CSMQueryG(void):
		CSMExpertGroup(_T("генерация вопроса поисковику"))
{
	CSMExpert * pExpert;

	pExpert=new PostAnalysisExperts::QueryGeneration::CSMVerbFormCorrector;
	AddExpert(pExpert);

	pExpert=new PostAnalysisExperts::QueryGeneration::CSMQueryGenerator;
	AddExpert(pExpert);

}

CSMQueryG::~CSMQueryG(void)
{
}


		}
	}
}