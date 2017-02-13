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


class CDirectConvertor : public CBaseCoreClass
{
public:
	CDirectConvertor();
	virtual ~CDirectConvertor();

public:
	virtual void SetLoadManager(ILoadManager* pLoadManager);
	void Lat2Rus(LPCWSTR szText, wstring* pResult);

public:
	void AddWord(LPWSTR szLatWord, wstring* pResult);

private:
	IMorphologyAnalyser* m_pMorphoAnalyzer;
	CHypotesisStorage* m_pHypotesis;
	CTransitionTable* m_pTable;
	CWordBreaker* m_pBreaker;
};
