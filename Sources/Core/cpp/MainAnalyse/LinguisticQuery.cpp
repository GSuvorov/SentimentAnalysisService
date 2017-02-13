#include "StdAfx.h"
#include ".\LinguisticQuery.h"

#include "..\ASSInterface\ITextBlockEx.h"
#include "..\ASSInterface\IQueryResultEx.h"

#include "..\ASCInterface\defines.h"

#include "..\ASSInterface\Iterators.h"
using namespace SS::Iterators;

#include <algorithm>
#include <string>
#include <vector>
using namespace std;


namespace SS
{
namespace LinguisticProcessor
{

class CQueryIterator
{
public:
	CQueryIterator(IGroup* pGroup) : 
			m_pGroup(pGroup),
			m_pUnit(NULL)
	{
		if(m_pGroup)
			m_pUnit = m_pGroup->GetFirstQueryUnit();
	}

	IQueryUnit* operator*()
	{
		return m_pUnit;
	}

	CQueryIterator& operator++()
	{
		if(!m_pUnit)
			return *this;
		GotoNext();
		return *this;
	}

private:
	void GotoNext()
	{
		m_pUnit = m_pUnit->GetNextQueryUnit();
		while(!m_pUnit)
		{
			m_pGroup = m_pGroup ? m_pGroup->GetNextGroup() : NULL;
			if(!m_pGroup)
				return;
			m_pUnit = m_pGroup->GetFirstQueryUnit();
		}
	}

private:
	IQueryUnit* m_pUnit;
	IGroup* m_pGroup;
};


CLinguisticQuery::CLinguisticQuery(void): m_bRemember(false)
{
}

CLinguisticQuery::~CLinguisticQuery(void)
{
}

void CLinguisticQuery::ApplyQueryParams(
	/// параметры поиска
	SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
	/// параметры анализа запроса
	, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
	)
{
	SS_TRY
	{
	
		GetAnalyseParams()->SetAnalyseResultEx(SS::Core::Types::IndexationSearch::TSearchParams::saNormal);

		/* высталение параметров отвечающих за формирование индексов при анализе запроса*/
		if(m_oMode.m_bIsLiteMode)
		{
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsMorpho	(true/*m_oMode.GetAnalyseDepthParams()->IsMorpho()*/	);
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSemantic	(false/*m_oMode.GetAnalyseDepthParams()->IsSemantic()*/	);
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSyntax	(false/*m_oMode.GetAnalyseDepthParams()->IsSyntax()*/	);
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSynonims	(true/*m_oMode.GetAnalyseDepthParams()->IsSynonims()*/	);
		}else
		{
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsMorpho	(true/*m_oMode.GetAnalyseDepthParams()->IsMorpho()*/	);
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSemantic	(true/*m_oMode.GetAnalyseDepthParams()->IsSemantic()*/	);
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSyntax	(true/*m_oMode.GetAnalyseDepthParams()->IsSyntax()*/	);
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSynonims	(false/*m_oMode.GetAnalyseDepthParams()->IsSynonims()*/	);
		}

		GetAnalyseParams()->m_bIsLiteMode = m_oMode.m_bIsLiteMode;
		GetAnalyseParams()->SetAnalyseMod(SS::Core::Types::eamQuery);

		/* высталение параметров отвечающих за вид результатов анализа (строка или индексы) запроса*/
		if( SS::Core::Types::IndexationSearch::gqmIndex == pQueryParams->m_GenerateQueryMode)
		{
			GetAnalyseParams()->SetCurrentSystemMode(SS::Core::Types::ecsmLocal);
		}else
		{
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSynonims(false);
			GetAnalyseParams()->SetCurrentSystemMode(SS::Core::Types::ecsmGlobal);
		}		

		/* уточнение ранее выставленных параметров */
		if(SS::Core::Types::IndexationSearch::sdNormal == pQueryParams->m_SearchDepth)
		{
		}else if(SS::Core::Types::IndexationSearch::sdStrict == pQueryParams->m_SearchDepth)
		{
			SS_THROW(L"Неверный парамтр - pQueryParams->m_SearchDepth == SS::Core::Types::IndexationSearch::sdStrict");
		}else if(SS::Core::Types::IndexationSearch::sdSynonyms == pQueryParams->m_SearchDepth)
		{
			GetAnalyseParams()->GetAnalyseDepthParams()->SetIsSynonims(true);
		}else if(SS::Core::Types::IndexationSearch::sdRejectUnimportant == pQueryParams->m_SearchDepth)
		{
			if(GetAnalyseParams()->GetCurrentSystemMode() == SS::Core::Types::ecsmGlobal)
			{
				SAVE_LOG(SS_WARNING AND L"Указан некорректный режим! В глобальной версии не может быть режима RejectUnimportant!" AND __WFUNCTION__);
			}else
                GetAnalyseParams()->SetAnalyseMod(SS::Core::Types::eamRejectUnimportant);
		} 

		return;
	}
	SS_CATCH(L"")
}

void CLinguisticQuery::GenerateQuery(
	SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
	, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams	
	, SS::Core::Types::IndexationSearch::TAbsentWords* pAbsentWords)
{
	SS_TRY
	{

		m_pText->ClearText();

		ApplyQueryParams(pSearchParams, pQueryParams);

		m_pLexica->SetAnalyseParams(GetAnalyseParams());


		if(!m_oMode.m_bIsLiteMode)
		{
			if(m_pQueryDeformer)
				m_pQueryDeformer->SetAnalyseParams(GetAnalyseParams());
			m_pSyntaxAnalyzer->SetAnalyseParams(GetAnalyseParams());
			m_pSemanticAnalyze->SetAnalyseParams(GetAnalyseParams());
		}

		m_pRelevanceEvaluation->SetAnalyseParams(GetAnalyseParams());

		SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
		oNativeText.SetText(pQueryParams->GetQuery());

		m_pLexica->SetText(&oNativeText);

		if(m_pLexica->LoadNext())
		{
			m_pLexica->AnalyzeNext(m_pText, 0);

			if(IsITextEmpty())
				return;
			FillAbsentWords(pAbsentWords);

			if(!m_oMode.m_bIsLiteMode)
			{
				if(m_pQueryDeformer)
					m_pQueryDeformer->TransformQuery(m_pText);

				m_pSemanticAnalyze->AnalyseNext(m_pText);
				// TODO изменить
				if(
					(SS::Core::Types::ealEnglish == m_pText->GetFirstParagraph()->GetFirstSentence()->GetLanguage()) ||
					(SS::Core::Types::ealUndefined == m_pText->GetFirstParagraph()->GetFirstSentence()->GetLanguage())
					)
				{
					m_pSyntaxAnalyzer->AnalyzeNext(m_pText);
					m_pSemanticAnalyze->SyntaxQuestionPostProcess(m_pText);
				}
			}
			m_pLexica->FillSynonyms(m_pText);
		}		

		AddOneRootWords();

		m_pText->Normalize();
	}
	SS_CATCH((wchar_t*)pQueryParams->GetQuery())
}


void CLinguisticQuery::GenerateQuery(
	SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams
	, SS::Core::Types::IndexationSearch::TQueryParams* pQueryParams
	, SS::Interface::Core::CommonContainers::IQueryResult* pQueryResult
	, bool bIsRememberQuery								
	)
{
	SS_TRY
	{

		SL_OUT_DEBUG_STR(__WFUNCTION__);

		pQueryResult->Clear();
		m_pQuery->Clear();

		GenerateQuery(pSearchParams, pQueryParams, pQueryResult->GetAbsentWords());

	
		if(m_pRelevanceEvaluation->EvaluateQuery(m_pText, m_pQuery) == E_FAIL)
		{
			return;
		}			
		if(SS::Core::Types::IndexationSearch::gqmIndex == pQueryParams->m_GenerateQueryMode)
		{
			if(pQueryResult)
			{
				SS::Interface::Core::CommonContainers::IQueryResultEx* pQueryResultEx = 
					dynamic_cast<SS::Interface::Core::CommonContainers::IQueryResultEx*>(pQueryResult);

				pQueryResultEx->SetLinguisticProcessorMode(&m_oMode);

#ifdef _SS_UNITTESTS
				SAVE_LOG(SS_MESSAGE AND L"Копирование в из IQuery в результаты разбора запроса" AND __WFUNCTION__);
#endif
				pQueryResultEx->SetIQuery(m_pQuery);
			}		

		} else if(SS::Core::Types::IndexationSearch::gqmString == pQueryParams->m_GenerateQueryMode)
		{
			std::wstring wsQuery;
			m_pSemanticAnalyze->QueryToSearchString(m_pText, &wsQuery);

//SL_OUT_DEBUG_STR(wsQuery.c_str());

			pQueryResult->AddQueryIndex()->SetQueryWord(wsQuery.c_str());

			if(SS::Core::Types::ealRussian ==	m_pText->GetFirstParagraph()->GetFirstSentence()->GetLanguage())
			{
				pSearchParams->m_SearchLanguage = SS::Core::Types::IndexationSearch::TSearchParams::slRus;
			}else if(SS::Core::Types::ealEnglish ==	m_pText->GetFirstParagraph()->GetFirstSentence()->GetLanguage())
			{
				pSearchParams->m_SearchLanguage = SS::Core::Types::IndexationSearch::TSearchParams::slEng;
			}

			pSearchParams->m_SearchArea = GetAnalyseParams()->GetAnalyseResultEx();
		}

		SL_OUT_DEBUG_STR(__WFUNCTION__);

	}
	SS_CATCH((wchar_t*)pQueryParams->GetQuery())
}

void CLinguisticQuery::GetSynonims( SS::Dictionary::Types::TDictionaryIndex& oDictionaryIndex, std::vector<unsigned int>* pvIndices)
{
	SS_TRY
	{

		SS::Interface::Core::CoSeDi::ISemanticAnalysis * pSynonimsSemanticAnalysis = m_pCoSeDiManager->GetSynonimsSemanticAnalysis();

		std::list<SS::Dictionary::Types::TDictionaryIndex> olSynonimsClass;
		std::list<SS::Dictionary::Types::TDictionaryIndex> olSynonims;

		// get classes
		pSynonimsSemanticAnalysis->GetClasses(oDictionaryIndex, &olSynonimsClass);

		// get words indices
		for(std::list<SS::Dictionary::Types::TDictionaryIndex>::iterator iter = olSynonimsClass.begin();
			iter != olSynonimsClass.end(); iter++)
		{
			pSynonimsSemanticAnalysis->SetClass((*iter));
			pSynonimsSemanticAnalysis->GetWords(&olSynonims);
		}

		// transform results
		for(std::list<SS::Dictionary::Types::TDictionaryIndex>::iterator iter = olSynonims.begin();
			iter != olSynonims.end(); iter++)
		{
			pvIndices->push_back( iter->GetDictionaryIndexWithoutFlags() );			
		}

	}
	SS_CATCH(L"")
}

void CLinguisticQuery::GetQuerySourceIndices(std::vector<unsigned int>* pvIndices)
{
	SS_TRY;

	if(!m_pQuery)
		return;

	ITransformation* pTrans = m_pQuery->GetFirstTransformation();
	if(!pTrans)
		return;

	std::set<unsigned int> sIndexes;
	for(CQueryIterator it = pTrans->GetFirstGroup(); *it; ++it)
	{
		AddCollectionIndexes(&sIndexes, (*it)->GetQueryUnitIndexCollection(eitMorphoIndex));
		AddCollectionIndexes(&sIndexes, (*it)->GetQueryUnitIndexCollection(eitSynonymIndex));
	}

	pvIndices->clear();
	pvIndices->insert(pvIndices->end(), sIndexes.begin(), sIndexes.end());

	SS_CATCH(L"");
}

void CLinguisticQuery::GetQueryKeywords(LPCWSTR szQuery, std::vector<std::wstring>* pKeywords, 
										std::vector<std::vector<std::wstring> >* pMorphoForms)
{
	SS_TRY;

	SS::Core::Types::IndexationSearch::TSearchParams oSearchParams;
	SS::Core::Types::IndexationSearch::TQueryParams oQueryParams;
	oQueryParams.m_GenerateQueryMode = SS::Core::Types::IndexationSearch::gqmString;
	ApplyQueryParams(&oSearchParams, &oQueryParams);
	m_pLexica->SetAnalyseParams(GetAnalyseParams());
	m_pSemanticAnalyze->SetAnalyseParams(GetAnalyseParams());

	m_pText->ClearText();
	SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
	oNativeText.SetText(szQuery);

	m_pLexica->SetText(&oNativeText);
	if(m_pLexica->LoadNext())
	{
		m_pLexica->AnalyzeNext(m_pText, 0);
		if(!m_oMode.m_bIsLiteMode)
			m_pSemanticAnalyze->AnalyseNext(m_pText);
		m_pText->Normalize();
	}

	IParagraph* pPara = m_pText->GetFirstParagraph();
	if(!pPara)
		return;
	ISentence* pSent = pPara->GetFirstSentence();
	if(!pSent)
		return;

	std::list<SS::Dictionary::Types::TWordInfo> lWordInfo;
	for(IUnit* pUnit = pSent->GetFirstUnit(); pUnit; pUnit = pUnit->GetRightUnit())
	{
		ISyntaxFeature* pSyntaxFeature = pUnit->GetSyntaxFeature();
		if(pSyntaxFeature && pSyntaxFeature->IsQuestion())
			continue;

		for(CSentenceIterator it = pUnit; *it != pUnit->GetRightUnit(); ++it)
		{
			if(pKeywords)
				pKeywords->push_back((*it)->GetWord());
			if(pMorphoForms)
			{
				int iCount = 0;
				for(IIndex* pIndex = (*it)->GetFirstIndex(); pIndex; pIndex = (*it)->GetNextIndex())
				{
					vector<wstring> oVect;

					SS::Dictionary::Types::TDictionaryIndex oInd = pIndex->GetDictionaryIndex()->GetFirst();
					m_pDictionary->GetMorphologyAnalyser()->GetWordFormsByIDSource(&oInd, &lWordInfo);
					for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = lWordInfo.begin(); it != lWordInfo.end(); ++it)
					{
						SS::Dictionary::Types::SWordInfo oInfo = *it->begin();
						oVect.push_back(oInfo.m_wWord);
					}
					if(!oVect.empty())
						pMorphoForms->push_back(oVect);
				}
			}
		}
	}

	SS_CATCH((wchar_t*) szQuery);
}

void CLinguisticQuery::AddCollectionIndexes(std::set<unsigned int>* pSet, IQueryUnitIndexCollection* pColl)
{
	SS_TRY;

	if(!pColl)
		return;

	for(IQueryUnitIndex* pIndex = pColl->GetFirstQueryUnitIndex(); pIndex; pIndex = pIndex->GetNextQueryUnitIndex())
		pSet->insert(pIndex->GetIndex().GetDictionaryIndexWithoutFlags());

	SS_CATCH(L"");
}

bool CLinguisticQuery::IsITextEmpty()
{
	SS_TRY
	{
		if(!m_pText->GetFirstParagraph())
		{
			return true;
		}
		if(!m_pText->GetFirstParagraph()->GetFirstSentence())
		{
			return true;
		}
	}
	SS_CATCH(L"")

	return false;
}

void CLinguisticQuery::FillAbsentWords(SS::Core::Types::IndexationSearch::TAbsentWords* pAbsentWords/*, SS::Interface::Core::BlackBox::IUnit * pUnit*/)
{
	SS_TRY
	{
		for(SS::Interface::Core::BlackBox::ISentence * pSentence = m_pText->GetFirstParagraph()->GetFirstSentence();pSentence;
			pSentence = pSentence->GetNextSentence())
		{
			for(CSentenceIterator it = pSentence->GetFirstUnit(); *it; ++it)
			{
				if((*(*it)->GetWord()) == 0)
					continue;
				if(!(*it)->HasIndexType(SS::Interface::Core::BlackBox::eitMorphoIndex))
					pAbsentWords->push_back((*it)->GetWord());
			}
		}
	}
	SS_CATCH(L"")
}

void CLinguisticQuery::AddOneRootWords()
{
	SS_TRY
	{
		for (IParagraph * pParagraph = m_pText->GetFirstParagraph(); pParagraph; pParagraph = pParagraph->GetNextParagraph()) 
		{
			for (ISentence * pSentence = pParagraph->GetFirstSentence(); pSentence; pSentence = pSentence->GetNextSentence()) 
			{
				for(CSentenceIterator it = pSentence->GetFirstUnit(); *it; ++it)
				{
					if((*(*it)->GetWord()) == 0)
						continue;

					std::list<SS::Dictionary::Types::TDictionaryIndex> o_l_complexIDSource;
					for (IIndex* pIndex = (*it)->GetFirstIndex(); pIndex ;pIndex = (*it)->GetNextIndex()) 
					{
						std::list<SS::Dictionary::Types::TDictionaryIndex> o_l_IDSource;
						if (SS::Interface::Core::BlackBox::eitMorphoIndex == pIndex->GetDictionaryIndex()->GetFirst().GetIndexType()) 
						{	
							o_l_IDSource.clear();
							/*SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex;
							oDictionaryIndex.SetDictionaryIndex(pIndex->GetDictionaryIndex()->GetFirst());*/
							
							if(m_pDictionary->GetOneRoot()->SetDictionaryIndex(pIndex->GetDictionaryIndex()->GetFirst()))
							{
								m_pDictionary->GetOneRoot()->Get(&o_l_IDSource);							
							}
						}						
						for (std::list<SS::Dictionary::Types::TDictionaryIndex>::iterator iter = o_l_IDSource.begin();
							iter != o_l_IDSource.end(); iter++)
						{
							o_l_complexIDSource.push_back((*iter));
						}
					}
					for (std::list<SS::Dictionary::Types::TDictionaryIndex>::iterator iter = o_l_complexIDSource.begin();
						iter != o_l_complexIDSource.end(); iter++)
					{
						IIndex* pIndex = m_pBlackBoxTextStorage->CreateIndex();
						pIndex->GetDictionaryIndex()->AppendIndex(iter->GetDictionaryIndex());

						(*it)->AddIndex(pIndex);
					}					
					o_l_complexIDSource.clear();	
				}
			}
		}
	}
	SS_CATCH(L"")
}

}
}
