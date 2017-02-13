#pragma once

//#include "SMQuestionAnalysisR.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{

//				class Russian::CSMQuestionAnalysisR;

class CQuestionBase
	//: public Russian::CSMQuestionAnalysisR
{
public:
	CQuestionBase(void);
	~CQuestionBase(void);
//	void Init(CSMManager * pManager);
protected:
	//CSMQuestionAnalysis * m_pQuestionAnalysis;

	//IUnit * m_pCurrentUnit;

	//virtual void Analyse()=0;

	///// первый юнит текущего предложения
	//IUnit * GetFirstUnit();
	///// последний юнит текущего предложения
	//IUnit * GetLastUnit();
	///// очередной юнит
	//IUnit * GetNextUnit();
	///// предыдущий юнит
	//IUnit * GetPrevUnit();
	/////
	//IUnit * NextUnit( IUnit *  & pUnit );

	//SMorphoDescription * GetUnitDescription(SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);



};


			}
		}
	}
}