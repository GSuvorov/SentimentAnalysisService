#include "StdAfx.h"
#include ".\smindaxation.h"
#include ".\SMAdverbialModifierE.h"
#include ".\SMFeatureCompute.h"
#include ".\SMCitation.h"
#include ".\SMComplexName.h"
#include ".\SMPlaceSpecification.h"
#include ".\SMMeasure.h"
#include ".\SMComplementE.h"
#include ".\SMGenetive.h"
#include ".\SMNounDetectionE.h"
#include ".\SMPassiveE.h"
#include ".\SMSubjectObjectE.h"
#include ".\SMAdverbialModifierR.h"
#include ".\SMInderectObjectR.h"
#include ".\SMPassiveR.h"
#include ".\SMSubjectR.h"
#include ".\SMPassiveR.h"
#include ".\SMSubjectR.h"
#include ".\SMComplementR.h"
#include ".\SMDependentClause.h"
#include ".\SMCauseMannerE.h"
#include ".\SMCauseMannerR.h"
#include ".\SMExAnaphoraE.h"
#include ".\SMExAnaphoraR.h"

//#include ".\SMExClauseTimePlaceR.h"
//#include ".\SMExClauseTimePlaceE.h"
#include ".\SMExDieCauseManner.h"
#include ".\smexnamecausemanner.h"

namespace SS
{
namespace Semantic
{
namespace Groups
{


CSMIndaxation::CSMIndaxation(void)
:CSMExpertGroup(_T("индексация текста"))
{
	SS_TRY
	{
		CSMExpert * pExpert;

		pExpert=new AnalysisExperts::CSMFeatureCompute;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::CSMCitation;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::CSMComplexName;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::CSMPlaceSpecification;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::CSMMeasure;
		AddExpert(pExpert);

		//эксперт для проставления об-ва причины глаголу die
		pExpert=new AnalysisExperts::IndexationExperts::English::CSMExDieCauseManner;
		AddExpert(pExpert);
		//эксперт для проставления причины названия
		pExpert=new AnalysisExperts::IndexationExperts::English::CSMExNameCauseManner;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::English::CSMAdverbialModifierE;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::English::CSMGenetive;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::English::CSMNounDetectionE;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::English::CSMPassiveE;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::English::CSMSubjectObjectE;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::English::CSMComplementE;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::Russian::CSMAdverbialModifierR;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::Russian::CSMInderectObjectR;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::Russian::CSMPassiveR;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::Russian::CSMSubjectR;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::Russian::CSMComplementR;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::IndexationExperts::CSMDependentClause;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::IndexationExperts::English::CSMCauseMannerE;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::IndexationExperts::Russian::CSMCauseMannerR;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::IndexationExperts::English::CSMExAnaphoraE;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::IndexationExperts::Russian::CSMExAnaphoraR;
		AddExpert(pExpert);

		/*pExpert=new AnalysisExperts::IndexationExperts::English::CSMExClauseTimePlaceE;
		AddExpert(pExpert);

		pExpert=new AnalysisExperts::IndexationExperts::Russian::CSMExClauseTimePlaceR;
		AddExpert(pExpert);*/
	}
	SS_CATCH(L"")
}

CSMIndaxation::~CSMIndaxation(void)
{
}


}
}
}