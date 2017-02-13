#pragma once

#include "./smcausemanner.h"

namespace SS
{
namespace Semantic
{
namespace AnalysisExperts
{
namespace IndexationExperts
{
namespace English
{

/// проставляет семантику причины следующим конструкциям
/*
1. <сущ. с семантикой Place> + <(> + <after|for|from> + <сущ> = AdjunctManner
2. <слово "Name"(AllForms)> + <from|for|by> + <сущ> = AdjunctManner
*/

class CSMExNameCauseManner : public CSMCauseManner
{
	std::set<wstring> m_NameForms;
	std::set<wstring> m_PrepositionWords;
public:
	CSMExNameCauseManner(void);
	~CSMExNameCauseManner(void);
public:
	bool Init(IText * pIText);
	bool AnalizeSentence();

private:
	void Run1Rule();
	void Run2Rule();
private:
	bool IsNounPlace();
	bool IsNoun();
	bool IsBracket();
	bool IsPrepositions();
	bool IsNameWord();
	bool IsPlase();
	bool IsProperName();
private:
	bool FindNounPlace();
	bool FindBracket();
	bool FindPrepositions();
	bool FindNounOrProperNameAndApplyAdjunctManner();
	bool FindNameWord();
};

}
}
}
}
}