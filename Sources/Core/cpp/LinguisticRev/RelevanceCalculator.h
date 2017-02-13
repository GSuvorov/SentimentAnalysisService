#pragma once

#include "SequenceComparer.h"
#include "AnswerWordFinder.h"
#include "RelevanceAlgorithm.h"


namespace SS
{
namespace LinguisticRev
{

class CRelevanceCalculator : 
		public CBaseCoreClass,
		public CAnalyseParams
{
public:
	CRelevanceCalculator();
	virtual ~CRelevanceCalculator();

public:
	virtual void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
	virtual void SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams);

	void Init(bool bSynonyms, IQuery* pQuery);
	/// searchs pText for an answer
	void UpdateRelevance(IText* pText);

private:
	/// searchs pSent for an answer
	STransRev UpdateRelevance(ISentence* pSent, IParagraph* pPara);
	/// checks if sentence contains answer and highlights transformation
	bool SentenceSucceeded(int iFuzzyLevel, ITransformation* pTransform);
	/// marks query units with title numbers if they are coincident by morphology
	void SetTitleToQuery(IText* pTitleText);
	/// returns the last obligatory transformation
	ITransformation* GetLastObligTransform();
	/// returns present words count divided by total transformation words count
	int CountPresentWordsPercentage(IParagraph* pPara, ITransformation* pTrans);

private:
	typedef map<STransRev, ITransformation*, greater_equal<STransRev> > TRevMap;
	TRevMap m_mRev;

	IQuery* m_pQuery;
	bool m_bSynonyms;

	CSequenceComparer m_oComparer;
	CAnswerWordFinder m_oFinder;
	CRelevanceAlgorithm m_oRevAlg;

	typedef map<ITransformation*, int> TPercentageMap;
	TPercentageMap m_mPercentage;
};

}
}
