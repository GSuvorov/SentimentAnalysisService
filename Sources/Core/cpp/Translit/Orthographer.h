#pragma once

#include "..\ASCInterface\IInterface.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASCInterface\services.h"
using namespace SS::Core::CommonServices;

#include "..\ASSInterface\IMainAnalyse.h"
using namespace SS::Interface::Core::MainAnalyse;

#include "..\ASSInterface\IDictionaryManager.h"
using namespace SS::Interface::Core::Dictionary;

#include "..\ASSInterface\TDictionaryManager.h"
using namespace SS::Dictionary::Types;

#include "..\ASSInterface\TMainAnalyseTypes.h"
using namespace SS::Core::Types;

#include <string>
using namespace std;

#include "TransitionTable.h"
#include "WordBreaker.h"


class COrthographer : public CBaseCoreClass
{
public:
	COrthographer();
	virtual ~COrthographer();

public:
	virtual void SetLoadManager(ILoadManager* pLoadManager);
	void CheckOrthography(LPCWSTR szIncorrect, wstring* pCorrect);

private:
	void AddWord(LPWSTR szWord, wstring* pResult);

private:
	IMorphologyAnalyser* m_pMorphoAnalyzer;
	CHypotesisStorage* m_pHypotesis;
	CTransitionTable* m_pTable;
	CWordBreaker* m_pBreaker;
};
