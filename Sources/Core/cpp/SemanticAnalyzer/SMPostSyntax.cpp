#include "StdAfx.h"
#include ".\smpostsyntax.h"

#include ".\smquestionparser.h"
#include ".\smquerygenerator.h"
#include ".\smspecifycategory.h"
#include ".\smwordsremover.h"
#include ".\smsyntaxcategoriespulldown.h"
#include ".\smverbformcorrector.h"
#include ".\smsemanticinserter.h"
#include ".\smindexremover.h"
#include ".\smtreetransformation.h"



namespace SS
{
	namespace Semantic
	{
		namespace Groups
		{

CSMPostSyntax::CSMPostSyntax(void):
		CSMExpertGroup(_T("разбор вопроса после синтаксиса"))
{
	CSMExpert * pExpert;

	pExpert=new PostAnalysisExperts::PostSyntaxAnalysis::CSMTreeTransformation;
	AddExpert(pExpert);

	pExpert=new PostAnalysisExperts::PostSyntaxAnalysis::CSMQuestionParser;
	AddExpert(pExpert);

	pExpert=new PostAnalysisExperts::PostSyntaxAnalysis::CSMSpecifyCategory;
	AddExpert(pExpert);

	pExpert=new PostAnalysisExperts::PostSyntaxAnalysis::CSMWordsRemover;
	AddExpert(pExpert);

	pExpert=new PostAnalysisExperts::PostSyntaxAnalysis::CSMIndexRemover;
	AddExpert(pExpert);

	pExpert=new PostAnalysisExperts::PostSyntaxAnalysis::CSMSemanticInserter;
	AddExpert(pExpert);

	pExpert=new PostAnalysisExperts::PostSyntaxAnalysis::CSMSyntaxCategoriesPullDown;
	AddExpert(pExpert);
}


CSMPostSyntax::~CSMPostSyntax(void)
{
}


		}
	}
}