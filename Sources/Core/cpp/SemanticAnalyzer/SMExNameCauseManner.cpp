#include "StdAfx.h"
#include ".\smexnamecausemanner.h"

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

const wchar_t* cwBracket = L"(";

CSMExNameCauseManner::CSMExNameCauseManner(void)
{
	m_AnalyseLanguage=SS::Core::Types::ealEnglish;
	m_sDescription=_T(__FUNCTION__);
}

CSMExNameCauseManner::~CSMExNameCauseManner(void)
{
}

bool CSMExNameCauseManner::Init(IText * pIText)
{
	SS_TRY
	{
		if(!CSMSentenceExpert::Init(pIText)) return false;

		if(!m_bInit)
		{
			m_bInit = true;

			// словоформы слова Name
			m_NameForms.clear();
			m_NameForms.insert(wstring(_T("name")));
			m_NameForms.insert(wstring(_T("names")));
			m_NameForms.insert(wstring(_T("named")));
			m_NameForms.insert(wstring(_T("naming")));

			// предологи
			m_PrepositionWords.clear();
			m_PrepositionWords.insert(wstring(_T("after")));
			m_PrepositionWords.insert(wstring(_T("for")));
			m_PrepositionWords.insert(wstring(_T("from")));
			m_PrepositionWords.insert(wstring(_T("by")));
		}
		return true;
	}
	SS_CATCH(L"")
}

bool CSMExNameCauseManner::AnalizeSentence()
{
	SS_TRY
	{
		Run1Rule();
		Run2Rule();

		return true;
	}
	SS_CATCH(L"")
}
void CSMExNameCauseManner::Run1Rule()
{
	SS_TRY
	{
		GetFirstUnit();

		while(FindNounPlace())
		{
			if(FindBracket() && FindPrepositions())
			{
				FindNounOrProperNameAndApplyAdjunctManner();
			}
		}
	}
	SS_CATCH(L"")
}
void CSMExNameCauseManner::Run2Rule()
{
	SS_TRY
	{
		GetFirstUnit();

		while(FindNameWord())
		{
			if(FindPrepositions())
			{
				FindNounOrProperNameAndApplyAdjunctManner();
			}
		}
	}
	SS_CATCH(L"")
}

bool CSMExNameCauseManner::IsNounPlace()
{
	SS_TRY
	{
		return (IsNoun()&&IsPlase());
	}
	SS_CATCH(L"")
}
bool CSMExNameCauseManner::IsNoun()
{
	SS_TRY
	{
		return IsContainFeature(efcNoun);
	}
	SS_CATCH(L"")
}

bool CSMExNameCauseManner::IsPlase()
{
	SS_TRY
	{
		for(IIndex * pIndex = m_pCurrentUnit->GetFirstIndex(); pIndex ; pIndex = m_pCurrentUnit->GetNextIndex())
		{
			SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex;
			pIndex->GetDictionaryIndex()->GetFirst(&oDictionaryIndex);

			if(oDictionaryIndex.GetIndexType() == SS::Interface::Core::BlackBox::eitSemanticIndex)
			{
				m_pIAMCSemantic->EnCode( oDictionaryIndex.GetId() , &m_SemanticFeature);				
				if(m_SemanticFeature.m_SemanticType.IsIntersectsWith(&Values::TSemanticType.smtPlace))
					return true;
			}
		}
	}
	SS_CATCH(L"")
}

bool CSMExNameCauseManner::IsBracket()
{
	SS_TRY
	{
		return ((*m_pCurrentUnit->GetWord()) == (*cwBracket));
	}
	SS_CATCH(L"")
}

bool CSMExNameCauseManner::IsPrepositions()
{
	SS_TRY
	{
		wstring sz_Word(m_pCurrentUnit->GetWord());
		m_StringService.LowerSz((TCHAR*)sz_Word.c_str());
		set<wstring>::iterator i=m_PrepositionWords.find(sz_Word.c_str());
		return (i!=m_PrepositionWords.end());
	}
	SS_CATCH(L"")
}
bool CSMExNameCauseManner::IsNameWord()
{
	SS_TRY
	{
		wstring sz_Word(m_pCurrentUnit->GetWord());
		m_StringService.LowerSz((TCHAR*)sz_Word.c_str());
		set<wstring>::iterator i=m_NameForms.find(sz_Word.c_str());
		return (i!=m_NameForms.end());
	}
	SS_CATCH(L"")
}

bool CSMExNameCauseManner::IsProperName()
{
	SS_TRY
	{
		for(IIndex * pIndex = m_pCurrentUnit->GetFirstIndex(); pIndex ; pIndex = m_pCurrentUnit->GetNextIndex())
		{
			SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex;
			pIndex->GetDictionaryIndex()->GetFirst(&oDictionaryIndex);

			if(oDictionaryIndex.GetDictionaryNumber() == SS::Dictionary::DATA_TYPE::NAMES::ednEProperName)
			{
				return true;
			}
		}
		return false;
	}
	SS_CATCH(L"")
}

bool CSMExNameCauseManner::FindNounPlace()
{
	SS_TRY
	{
		if(!m_pCurrentUnit)
			return false;
		do
		{
			if(IsNounPlace())
				return true;
		}while(GetNextUnitWithChild());

		return false;
	}
	SS_CATCH(L"")
}
bool CSMExNameCauseManner::FindBracket()
{
	SS_TRY
	{
		return (GetNextUnitWithChild() && IsBracket());
	}
	SS_CATCH(L"")
}
bool CSMExNameCauseManner::FindPrepositions()
{
	SS_TRY
	{
		return (GetNextUnitWithChild() && (IsPrepositions()));
	}
	SS_CATCH(L"")
}

bool CSMExNameCauseManner::FindNameWord()
{
	SS_TRY
	{
		if(!m_pCurrentUnit)
			return false;
		do
		{
			if(IsNameWord())
				return true;
		}while(GetNextUnitWithChild());

		return false;
	}
	SS_CATCH(L"")
}

bool CSMExNameCauseManner::FindNounOrProperNameAndApplyAdjunctManner()
{
	SS_TRY
	{
		int iCountWords = 0;
		while(GetNextUnitWithChild() && (++iCountWords) < 3)
		{
			if(IsNoun() || IsProperName())
			{
				AddMannerCategory();
				return true;
			}
		}
		return false;
	}
	SS_CATCH(L"")
}


}
}
}
}
}