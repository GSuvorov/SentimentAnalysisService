#include "stdafx.h"
#include "CDictionaryPool.h"

CTreeDictionary* CDictionaryPool::m_pDeletedOrgPrefixes = NULL;
CTreeDictionary* CDictionaryPool::m_pDeletedReductions = NULL;
CTreeDictionary* CDictionaryPool::m_pHomonymyNouns = NULL;
CTreeDictionary* CDictionaryPool::m_pHomonymyAdjectives = NULL;
CTreeDictionary* CDictionaryPool::m_pNounVerbsNoIE = NULL;
CTreeDictionary* CDictionaryPool::m_pNounVerbExclusionIE = NULL;
CTreeDictionary* CDictionaryPool::m_pPosts = NULL;

void CDictionaryPool::Load(wstring strDatabasePath, UnitPropertiesOperator* unitOperator)
{
}

void CDictionaryPool::LoadFromCollection(vector<vector<wstring>> &dictionaries, UnitPropertiesOperator* unitOperator)
{
    if(dictionaries.size() > eDictionary::reductions)
    {
	    m_pDeletedReductions = new CTreeDictionary();
	    m_pDeletedReductions->LoadDict(dictionaries[reductions]);
    }

	if(dictionaries.size() > eDictionary::deletedOrgPrefixes)
    {
	    m_pDeletedOrgPrefixes = new CTreeDictionary();
	    m_pDeletedOrgPrefixes->LoadDict(dictionaries[deletedOrgPrefixes]);
    }

	if(dictionaries.size() > eDictionary::homonymyNouns)
	{
		m_pHomonymyNouns = new CTreeDictionary();
	    m_pHomonymyNouns->LoadDict(dictionaries[homonymyNouns]);
	}

	if(dictionaries.size() > eDictionary::homonymyAdjectives)
	{
		m_pHomonymyAdjectives = new CTreeDictionary();
	    m_pHomonymyAdjectives->LoadDict(dictionaries[homonymyAdjectives]);
	}

	if(dictionaries.size() > eDictionary::nounVerbsNoIE)
	{
		m_pNounVerbsNoIE = new CTreeDictionary();
	    m_pNounVerbsNoIE->LoadDict(dictionaries[nounVerbsNoIE]);
	}

	if(dictionaries.size() > eDictionary::nounVerbExclusionIE)
	{
		m_pNounVerbExclusionIE = new CTreeDictionary();
	    m_pNounVerbExclusionIE->LoadDict(dictionaries[nounVerbExclusionIE]);
	}

	if(dictionaries.size() > eDictionary::posts)
	{
		m_pPosts = new CTreeDictionary();
	    m_pPosts->LoadDict(dictionaries[posts]);
	}
}

bool CDictionaryPool::IsFindOrgPrefixes(wstring strWordForCheck)
{
	vector<string> vectStrTypes;
	m_pDeletedOrgPrefixes->FindWord(strWordForCheck, vectStrTypes, true);
	return !vectStrTypes.empty();
}

/*bool CDictionaryPool::IsFindStopAdjectives(wstring strWordForCheck, const wstring &profile)
{
	vector<string> vectStrTypes;
	map<wstring, CTreeDictionary*>::iterator it = m_deletedAdjectivePrefixes.find(profile);
	if (it != m_deletedAdjectivePrefixes.end())
	{
		wstring word = _wcslwr((wchar_t*)strWordForCheck.c_str());
		(*it).second->FindWord(word, vectStrTypes, true);
	}
	return !vectStrTypes.empty();
}*/

bool CDictionaryPool::IsFindReduction(wstring strWordForCheck)
{
	vector<string> vectStrTypes;
	m_pDeletedReductions->FindWord(strWordForCheck, vectStrTypes, true);
	return !vectStrTypes.empty();
}

bool CDictionaryPool::IsOnlyNoun(wstring strWordForCheck)
{
	vector<string> vectStrTypes;
	m_pHomonymyNouns->FindWord(strWordForCheck, vectStrTypes, true);
	return !vectStrTypes.empty();
}

bool CDictionaryPool::IsOnlyAdjective(wstring strWordForCheck)
{
	vector<string> vectStrTypes;
	m_pHomonymyAdjectives->FindWord(strWordForCheck, vectStrTypes, true);
	return !vectStrTypes.empty();
}

bool CDictionaryPool::IsNounVerb(wstring normalWord)
{
	vector<string> vectStrTypes;
	if (normalWord.size()>3 && (normalWord.substr(normalWord.size() - 2) == L"ие"))
	{
		m_pNounVerbExclusionIE->FindWord(normalWord, vectStrTypes, true);
		return vectStrTypes.empty();
	}
	else
	{
		m_pNounVerbsNoIE->FindWord(normalWord, vectStrTypes, true);
		return !vectStrTypes.empty();
	}
}

bool CDictionaryPool::IsPost(const wstring &normalWord)
{
	vector<string> vectStrTypes;
	m_pPosts->FindWord(normalWord, vectStrTypes, true);
	return !vectStrTypes.empty();
}