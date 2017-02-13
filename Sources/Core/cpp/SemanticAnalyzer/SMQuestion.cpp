#include "StdAfx.h"
#include ".\smquestion.h"

//#include ".\SMAdverbialModifierE.h"
#include ".\SMFeatureCompute.h"
#include ".\SMCitation.h"
#include ".\SMComplexName.h"
#include ".\SMPlaceSpecification.h"
#include ".\SMMeasure.h"
#include ".\SMComplementE.h"
#include ".\SMGenetive.h"
#include ".\SMNounDetectionE.h"
#include ".\SMPassiveE.h"
//#include ".\SMSubjectObjectE.h"
#include ".\SMQuestionSubjectObjectE.h"
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
#include "SMQuestionAnalysisE.h"
#include "SMQuestionAnalysisR.h"
#include ".\smwordsremoverr.h"
#include ".\smsyntaxcategoriespulldown.h"
#include ".\SMExSimpleQueryParserR.h"

#include ".\smindexremoverr.h"

namespace SS
{
	namespace Semantic
	{
		namespace Groups
		{


CSMQuestion::CSMQuestion(void)
:CSMExpertGroup(_T("анализ вопроса для русского"))
{
	CSMExpert * pExpert;
	
	pExpert=new AnalysisExperts::CSMFeatureCompute;
	AddExpert(pExpert);

	pExpert=new AnalysisExperts::CSMMeasure;
	AddExpert(pExpert);

	/*pExpert=new AnalysisExperts::English::CSMAdverbialModifierE;
	AddExpert(pExpert);*/
	
	pExpert=new AnalysisExperts::English::CSMGenetive;	
	AddExpert(pExpert);

	pExpert=new AnalysisExperts::English::CSMNounDetectionE;
	AddExpert(pExpert);

	pExpert=new AnalysisExperts::English::CSMPassiveE;
	AddExpert(pExpert);

	/*pExpert=new AnalysisExperts::English::CSMSubjectObjectE;
	AddExpert(pExpert);*/

	pExpert=new AnalysisExperts::Question::English::CSMQuestionSubjectObjectE;
	AddExpert(pExpert);

	pExpert=new AnalysisExperts::Russian::CSMExSimpleQueryParserR;
	AddExpert(pExpert);

	pExpert=new AnalysisExperts::Russian::CSMAdverbialModifierR;
	AddExpert(pExpert);

	pExpert=new AnalysisExperts::Russian::CSMInderectObjectR;
	AddExpert(pExpert);

	pExpert=new AnalysisExperts::Russian::CSMPassiveR;
	AddExpert(pExpert);

	pExpert=new AnalysisExperts::Russian::CSMSubjectR;
	AddExpert(pExpert);

	pExpert=new AnalysisExperts::Question::Russian::CSMQuestionAnalysisR;
	AddExpert(pExpert);

	pExpert=new AnalysisExperts::Question::Russian::CSMWordRemoverR;
	AddExpert(pExpert);

	pExpert=new AnalysisExperts::Question::Russian::CSMIndexRemoverR;
	AddExpert(pExpert);

	pExpert=new PostAnalysisExperts::PostSyntaxAnalysis::CSMSyntaxCategoriesPullDown;
	AddExpert(pExpert);
}

CSMQuestion::~CSMQuestion(void)
{
}


		}
	}
}