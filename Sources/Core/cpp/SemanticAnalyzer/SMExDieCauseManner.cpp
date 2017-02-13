#include "StdAfx.h"
#include ".\smexdiecausemanner.h"
#include ".\smmanager.h"

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

CSMExDieCauseManner::CSMExDieCauseManner(void)
{
	m_bInit = false;
	m_AnalyseLanguage=SS::Core::Types::ealEnglish;
	m_sDescription=_T(__FUNCTION__);
}

CSMExDieCauseManner::~CSMExDieCauseManner(void)
{
}

bool CSMExDieCauseManner::findDie()
{
	SS_TRY
	{
		if(!m_pCurrentUnit)
			return false;
		do
		{
			if(IsDieWord(m_pCurrentUnit->GetWord()))
			{
				AddIndexes(m_DieCauseManner);
				return true;
			}
		}while(GetNextUnitWithChild());

		return false;
	}
	SS_CATCH(L"")
}
bool CSMExDieCauseManner::findPreposition(int &iNumWordsAfterDie)
{
	SS_TRY
	{
		while(GetNextUnitWithChild() && (++iNumWordsAfterDie<4))
		{
			if(IsPrepositionWord(m_pCurrentUnit->GetWord()))
			{
				return true;
			}
		}

		return false;
	}
	SS_CATCH(L"")
}

bool CSMExDieCauseManner::IsContainTimeAndPlase()
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
				if(
					(m_SemanticFeature.m_SemanticType.IsIntersectsWith(&Values::TSemanticType.smtPlace)) || 
					(m_SemanticFeature.m_SemanticType.IsIntersectsWith(&Values::TSemanticType.smtTimeInterval))
					)
					return true;
			}
		}
		
		return false;
	}
	SS_CATCH(L"")
}

bool CSMExDieCauseManner::findNoun(int &iNumWordsAfterDie)
{
	SS_TRY
	{
		int iPostion = 0;
		while(GetNextUnitWithChild() && (++iPostion<5) && (++iNumWordsAfterDie))
		{
			if(IsContainFeature(efcNoun))
			{
				if(IsContainTimeAndPlase())
					return false;

				AddMannerCategory();
				return true;
			}
			else if(IsNumeric() || IsContainTimeAndPlase())
			{
				return false;
			}

		}
		return false;
	}
	SS_CATCH(L"")
}

bool CSMExDieCauseManner::IsNumeric()
{
	SS_TRY
	{
		for(IIndex * pIndex = m_pCurrentUnit->GetFirstIndex(); pIndex ; pIndex = m_pCurrentUnit->GetNextIndex())
		{
			SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex;
			pIndex->GetDictionaryIndex()->GetFirst(&oDictionaryIndex);

			if(oDictionaryIndex.GetIndexType() == SS::Interface::Core::BlackBox::eitSemanticIndex)
			{
				for(IFeature * pFeature = pIndex->GetFirstFeature(); pFeature; pFeature = pIndex->GetNextFeature() )
				{
					m_pIAMCMorpho->EnCode(oDictionaryIndex.GetDictionaryNumber() , pFeature->GetMorphoInfo(), pFeature->GetPartOfSpeech() ,&m_MorphoFeature);
					
					if( m_MorphoFeature.m_PartOfSpeechTypes.IsIntersectsWith(&Values::PartOfSpeechTypes.postNumeric ))
						return true;
				}
			}
		}		
		return false;

	}
	SS_CATCH(L"")
}

void CSMExDieCauseManner::FindAfterDie(int &iNumWordsAfterDie)
{
	SS_TRY
	{
		// ищем предлог
		if(findPreposition(iNumWordsAfterDie))
		{
			// ищем ищем существительное
			if(!findNoun(iNumWordsAfterDie))
			{
				/// если существительное не найдено, пробуем сначала
				FindAfterDie(iNumWordsAfterDie);
			}

		}
	}
	SS_CATCH(L"")
}

bool CSMExDieCauseManner::AnalizeSentence()
{
	SS_TRY
	{
		UINT uiOutDepth=0;
		wstring sz_Word;

		// first expert
		while(findDie())
		{
			int iNumWordsAfterDie = 0;
			FindAfterDie(iNumWordsAfterDie);
		}

		// second expert
		FindDieMannerWords();

		return true;
	}
	SS_CATCH(L"")
}

bool CSMExDieCauseManner::FindDieMannerWords()
{
	if(!GetFirstUnit())
		return true;
	std::map<wstring, wstring>::iterator iter;
	do
	{

		wstring sz_Word(m_pCurrentUnit->GetWord());
		m_StringService.LowerSz((TCHAR*)sz_Word.c_str());

		iter = m_DieMannerWords.find(sz_Word);
		if(iter != m_DieMannerWords.end())
		{
			if(iter->second != L"")
			{
				if(!GetNextUnitWithChild())
					return false;

				sz_Word = m_pCurrentUnit->GetWord();
				m_StringService.LowerSz((TCHAR*)sz_Word.c_str());

				if(sz_Word != iter->second)
				{
					continue;
				}
			}	

			AddMannerCategory();
		}
		

	}
	while(GetNextUnitWithChild());
	return true;
}

bool CSMExDieCauseManner::Init(IText * pIText)
{
	SS_TRY
	{
		if(!CSMSentenceExpert::Init(pIText)) return false;

		if(!m_bInit)
		{
			m_bInit = true;
			if (m_DieCauseManner.empty())
			{
				m_pSMManager->GetDictionaryInterface()->GetWordInfo(_T("die"), &m_DieCauseManner, /*in*/SS::Dictionary::Types::efzBaseDictionary);
			}

			{
				m_DieWords.clear();
				m_DieWords.insert(wstring(_T("death")));
				m_DieWords.insert(wstring(_T("deaths")));

				m_DieWords.insert(wstring(_T("die")));
				m_DieWords.insert(wstring(_T("dies")));
				m_DieWords.insert(wstring(_T("died")));
				m_DieWords.insert(wstring(_T("dying")));

				m_DieWords.insert(wstring(_T("crucify")));
				m_DieWords.insert(wstring(_T("crucifies")));
				m_DieWords.insert(wstring(_T("crucified")));
				m_DieWords.insert(wstring(_T("crucifying")));

				m_DieWords.insert(wstring(_T("suicide")));
				m_DieWords.insert(wstring(_T("suicides")));
				m_DieWords.insert(wstring(_T("suicided")));
				m_DieWords.insert(wstring(_T("suiciding")));

				m_DieWords.insert(wstring(_T("kill")));
				m_DieWords.insert(wstring(_T("kills")));
				m_DieWords.insert(wstring(_T("killed")));
				m_DieWords.insert(wstring(_T("killing")));

				m_DieWords.insert(wstring(_T("assassinate")));
				m_DieWords.insert(wstring(_T("assassinates")));
				m_DieWords.insert(wstring(_T("assassinated")));
				m_DieWords.insert(wstring(_T("assassinating")));

				m_DieWords.insert(wstring(_T("murder")));
				m_DieWords.insert(wstring(_T("murders")));
				m_DieWords.insert(wstring(_T("murdered")));
				m_DieWords.insert(wstring(_T("murdering")));

				m_DieWords.insert(wstring(_T("execute")));
				m_DieWords.insert(wstring(_T("executes")));
				m_DieWords.insert(wstring(_T("executed")));
				m_DieWords.insert(wstring(_T("executing")));

				m_DieWords.insert(wstring(_T("choke")));
				m_DieWords.insert(wstring(_T("chokes")));
				m_DieWords.insert(wstring(_T("choked")));
				m_DieWords.insert(wstring(_T("choking")));
			}
	//////////////////////////////////////////////////
			{
				m_PrepositionWords.insert(wstring(_T("by")));
				m_PrepositionWords.insert(wstring(_T("of")));
				m_PrepositionWords.insert(wstring(_T("from")));
				m_PrepositionWords.insert(wstring(_T("in")));
			}
	/////////////////////////////////////////////////

			{
				m_DieMannerWords.insert(map<wstring,wstring>::value_type(wstring(_T("throat")), wstring(_T("infection"))) );
				m_DieMannerWords.insert(map<wstring,wstring>::value_type(wstring(_T("inflammatory")), wstring(_T("quinsy"))));
				m_DieMannerWords.insert(map<wstring,wstring>::value_type(wstring(_T("heart")), wstring(_T("attack"))));
				m_DieMannerWords.insert(map<wstring,wstring>::value_type(wstring(_T("throat")), wstring(_T("infection"))));
				m_DieMannerWords.insert(map<wstring,wstring>::value_type(wstring(_T("pneumonia")), wstring(_T(""))));
				m_DieMannerWords.insert(map<wstring,wstring>::value_type(wstring(_T("cerebral")), wstring(_T("hemorrhage"))));
				m_DieMannerWords.insert(map<wstring,wstring>::value_type(wstring(_T("typhus")), wstring(_T(""))));
				m_DieMannerWords.insert(map<wstring,wstring>::value_type(wstring(_T("appendix")), wstring(_T(""))));
				m_DieMannerWords.insert(map<wstring,wstring>::value_type(wstring(_T("aneurysm")), wstring(_T(""))));
			}

	//throat infection
	//inflammatory quinsy 
	//heart attack
	//pneumonia 
	//cerebral hemorrhage
	//typhus
	//appendix
	//aneurysm
		}

		return true;
	}
	SS_CATCH(L"")
}

bool CSMExDieCauseManner::IsPrepositionWord(const TCHAR * sWord)
{
	SS_TRY
	{
		wstring sz_Word(sWord);
		m_StringService.LowerSz((TCHAR*)sz_Word.c_str());
		set<wstring>::iterator i=m_PrepositionWords.find(sz_Word.c_str());
		return (i!=m_PrepositionWords.end());
	}
	SS_CATCH(L"")

}

bool CSMExDieCauseManner::IsDieWord(const TCHAR * sWord)
{
	SS_TRY
	{
		wstring sz_Word(sWord);
		m_StringService.LowerSz((TCHAR*)sz_Word.c_str());
		set<wstring>::iterator i=m_DieWords.find(sz_Word.c_str());
		return (i!=m_DieWords.end());
	}
	SS_CATCH(L"")
}

void CSMExDieCauseManner::AddIndexes(list<SS::Dictionary::Types::TWordInfo> & oDieCauseManner)
{
	IIndex* pIndex = NULL;
	std::list<SS::Dictionary::Types::TWordInfo>::iterator i;

	for (i=m_DieCauseManner.begin(); i!=m_DieCauseManner.end(); i++) 
	{
		SS::Dictionary::Types::SWordInfo* pwi = &(i->front());
		
		IFeature* pFeature = m_pIBlackBoxFactory->CreateFeature();
		pFeature->SetMorphoIndex(pwi->m_WordIndex.m_IDForm);
		pFeature->SetMorphoInfo(pwi->m_MorphoTypeInfo.m_MorphoInfo);
		pFeature->SetPartOfSpeech(pwi->m_SpecialMorpho);

		long idSource = pwi->m_WordIndex.m_DictIndex.GetDictionaryIndex();
		
		pIndex = m_pIBlackBoxFactory->CreateIndex();
		pIndex->GetDictionaryIndex()->AppendIndex(idSource);
		m_pCurrentUnit->AddIndex(pIndex);

		pIndex->AddFeature(pFeature);
	}
}

}
}
}
}
}