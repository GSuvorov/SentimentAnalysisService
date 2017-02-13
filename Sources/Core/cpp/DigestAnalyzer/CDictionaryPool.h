#pragma once

#include "stdafx.h"
#include "../LexicalAnalyzer/TreeDictionary.h"
#include "HelpFunc.h"

using namespace SS::LexicalAnalyzer::TreeDictionary;

enum eDictionary
{
	/// сокращения
    reductions = 0,
	/// удаляемые префиксы организаций
	deletedOrgPrefixes = 1,
	/// всегда существительные
	homonymyNouns = 2,
	/// всегда прилагательные
	homonymyAdjectives = 3,
	/// отглагольные существительные, заканчивающиеся не на "ие"
	nounVerbsNoIE = 4,
	/// существительные, заканчивающиеся на "ие", но не отглагольные
	nounVerbExclusionIE = 5,
	/// должности
	posts,
};

class CDictionaryPool
{
private:
	static CTreeDictionary* m_pDeletedOrgPrefixes;
	static CTreeDictionary* m_pDeletedReductions;
	static CTreeDictionary* m_pHomonymyNouns;
	static CTreeDictionary* m_pHomonymyAdjectives;
	static CTreeDictionary* m_pNounVerbsNoIE;
	static CTreeDictionary* m_pNounVerbExclusionIE;
	static CTreeDictionary* m_pPosts;
public:
	static void Load(wstring strDatabasePath, UnitPropertiesOperator* unitOperator);

	/// загрузка словарей из коллекции
    static void LoadFromCollection(vector<vector<wstring>> &dictionaries, UnitPropertiesOperator* unitOperator);
	
	static bool IsFindOrgPrefixes(wstring strWordForCheck);

	static bool IsFindReduction(wstring strWordForCheck);

	static bool IsOnlyNoun(wstring strWordForCheck);

	static bool IsOnlyAdjective(wstring strWordForCheck);

	static bool IsNounVerb(wstring normalWord);

	static bool IsPost(const wstring &normalWord);
};


