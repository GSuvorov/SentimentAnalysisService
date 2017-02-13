#pragma once
#include "smcausemanner.h"

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

/// выделение субъектов и объектов в тексте
class CSMExDieCauseManner :	public CSMCauseManner
{
	std::map<wstring, wstring> m_DieMannerWords;
	std::set<wstring> m_DieWords;
	std::set<wstring> m_PrepositionWords;
	std::list<SS::Dictionary::Types::TWordInfo> m_DieCauseManner;
public:
	CSMExDieCauseManner(void);
	virtual ~CSMExDieCauseManner(void);
public:
	bool AnalizeSentence();
	bool Init(IText * pIText);
private:
	bool findDie();
	bool findPreposition(int &iNumWordsAfterDie);
	bool findNoun(int &iNumWordsAfterDie);
	void FindAfterDie(int &iNumWordsAfterDie);
	bool FindDieMannerWords();

	bool IsNumeric();
	bool IsContainTimeAndPlase();
	bool IsDieWord(const TCHAR * sWord);
	bool IsPrepositionWord(const TCHAR * sWord);

	void AddIndexes(list<SS::Dictionary::Types::TWordInfo> & oDieCauseManner);
};


}
}
}
}
}