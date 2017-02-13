#include "StdAfx.h"
#include ".\TransformModule.h"
#include ".\Transform.h"
#include ".\Group.h"

#include "..\ASSInterface\Iterators.h"
using namespace SS::Iterators;

#include "..\ASSInterface\TDictionaryManager.h"
using namespace SS::Dictionary::Types;

//#define DONT_CONSIDER_ID_FORM
#define WHO_IS_TWO_TRANSFORMATIONS


namespace SS
{
namespace TransformGenerator
{

/////////////////////////////////////////////////////////////////////////////////////
// Static members

int CTransformModule::s_Trans_Eng1[] = 
{
	scMainEntity, scPredicate, scAdditionalEntity, 0,
	0
};

int CTransformModule::s_Trans_Eng2[] = 
{
	scMainEntity, scPredicate, scAdditionalEntity, 0,
	scAdditionalEntity, scPredicate, scMainEntity, 0,
	0
};

int CTransformModule::s_Trans_Rus[] = 
{
	scPredicate, scMainEntity, scAdditionalEntity, 0,
	0
};

int CTransformModule::s_Trans_Undefined[] = 
{
	scPredicate, scMainEntity, scAdditionalEntity, 0,
	0
};

set<unsigned int> CTransformModule::s_sSC;
set<unsigned int> CTransformModule::s_sWhoIsSC;

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CTransformModule::CTransformModule() : 
		m_pBlackBoxQueryStorage(NULL),
		m_pBlackBoxTextStorage(NULL),
		m_pSemanticAnalysis(NULL),
		m_bLite(false),
		m_pCombinatory(NULL)
{
}

/////////////////////////////////////////////////////////////////////////////////////
// Component functions

HRESULT CTransformModule::QueryInterface(REFIID refIID, void** pBase)
{
	if(refIID == IID_RelevanceEvaluation)
		*pBase = (SS::Interface::Core::Rev::IRelevanceEvaluation*)this;
	else if(refIID == IID_Base)
		*pBase = (SS::Interface::IBase*)this;
	else
		*pBase = NULL;

	return S_OK;
}

ULONG CTransformModule::Release()
{
	delete this;
	return 0;
}

void CTransformModule::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	SS_TRY;

	CBaseCoreClass::SetLoadManager(pLoadManager);

	InitAcceptibleCategoriesList();

	m_pBlackBoxQueryStorage = 
		(SS::Interface::Core::BlackBox::IBlackBoxQueryStorage*)
		pLoadManager->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxQueryStorage, IID_BlackBoxQueryStorage);
	m_pBlackBoxTextStorage = 
		(SS::Interface::Core::BlackBox::IBlackBoxTextStorage*)
		pLoadManager->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxTextStorage, IID_BlackBoxTextStorage);

	ICoSeDiManager* pMgr = (ICoSeDiManager*)
		pLoadManager->GetInterface(L"./core/linguistics/CoSeDi.dll", CLSID_ICoSeDiManager, IID_ICoSeDiManager);
	m_pSemanticAnalysis = pMgr->GetCombiSynonimsAnalysis();
	m_pCombinatory = pMgr->GetCombinatory();

	m_oWeightTable.SetLoadManager(pLoadManager);
	m_oTypeChecker.SetLoadManager(pLoadManager);

	SS_CATCH(L"");
}

void CTransformModule::SetAnalyseParams(IAnalyseParams* pParams)
{
	SS_TRY;

	CAnalyseParams::SetAnalyseParams(pParams);
	m_bLite = pParams->m_bIsLiteMode;

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// CTransformModule interface

HRESULT CTransformModule::EvaluateQuery(IText* pText, IQuery* pQuery)
{
	if(!pText) return S_FALSE;
	if(!pQuery) return S_FALSE;
	if(!GetAnalyseParams()) return S_FALSE;
	if(!m_pBlackBoxQueryStorage) return S_FALSE;

	SS_TRY
	{
		pQuery->Clear();

		if(!pText->GetFirstParagraph())
		{
			return E_FAIL;
		}

		if(!pText->GetFirstParagraph()->GetFirstSentence())
		{
			return E_FAIL;
		}

		if(!DoTransform(pText->GetFirstParagraph()->GetFirstSentence(), pQuery/*, GetAnalyseParams()*/))
		{
			return E_FAIL;
		}

		return S_OK;
	}
	SS_CATCH(L"")

	return E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////////////
// CTransformModule functions

bool CTransformModule::DoTransform(ISentence* pSentence, IQuery* pQuery)
{
	SS_TRY;

	if(!pSentence->GetFirstUnit())
		SAVE_LOG(SS_MESSAGE AND L"!pSentence->GetFirstUnit()" AND __WFUNCTION__);

	CTrasfromList lTransforms;
	EAnalyseLanguage Lang = pSentence->GetLanguage();
	bool bWhoIs = IsWhoIsQuestion(pSentence);

	// Выбираем нужный набор трансформаций
	int* pSeq = NULL;
	switch(Lang)
	{
		case ealUndefined:
			pSeq = s_Trans_Undefined;
			break;
		case ealRussian:
			pSeq = s_Trans_Rus;
			break;
		case ealEnglish:
			if(m_bLite)
				pSeq = s_Trans_Eng1;
			else
#ifdef WHO_IS_TWO_TRANSFORMATIONS
				pSeq = s_Trans_Eng2;
#else
				pSeq = bWhoIs ? s_Trans_Eng1 : s_Trans_Eng2;
#endif
			break;
		default:
			return false;
	}

	// Заполняем список трансформаций, затем каждую трансформацию заполняем одной последовательностью категорий
	lTransforms.Init(pSeq, m_pSemanticAnalysis, m_pBlackBoxTextStorage, 
		m_pBlackBoxQueryStorage, m_pCombinatory);

	// Проходим юниты верхнего уровня исходного дерева и заполняем трансформации группами
#ifdef DONT_CONSIDER_ID_FORM
	bool bIDForm = false;
#else
	bool bIDForm = bWhoIs;
#endif

	SUnitParams oParams(bIDForm, bWhoIs, IsNameDefinition(pSentence), &m_oTypeChecker);
	for(IUnit* pUnit = pSentence->GetFirstUnit(); pUnit; pUnit = pUnit->GetRightUnit())
	{
		if(!m_bLite && !IsUnitInSet(&s_sSC, pUnit))
			continue;
		for(CTransform* p = lTransforms.GetFirstTransform(); p; p = lTransforms.GetNextTransform())
			p->AddGroup(pUnit);
	}

	// Из каждого CTransform создаем трансформацию и добавляем ее в pQuery
	CTransform* pPrev = NULL;
	for(CTransform* p = lTransforms.GetFirstTransform(); p; p = lTransforms.GetNextTransform())
	{
		if(!pPrev || !p->Compare(pPrev))
		{
			for(ITransformation* pTrans = p->GetFirstTransform(Lang, oParams); pTrans; pTrans = p->GetNextTransform())
			{
				m_oWeightTable.SetWeights(pTrans);
				pQuery->AddTransformation(pTrans);
			}
		}

		pPrev = p;
	}

	return true;

	SS_CATCH(L"");
	return false;
}

void CTransformModule::InitAcceptibleCategoriesList()
{
	SS_TRY;

	if(s_sSC.empty())
	{
		// Синтаксические категории, которые могут присутствовать в запросе
		s_sSC.insert(Values::SyntaxCategories.scMainEntity.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAdditionalEntity.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scPredicate.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAjunct.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAjunctTimeFixedPoint.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAjunctTimeEnd.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAjunctTimeSource.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAjunctPlaceSource.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAjunctTimeDuration.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAjunctPlaceFixedPoint.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAjunctPlaceSource.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAjunctPlaceEnd.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAjunctMeasure.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAjunctCause.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAjunctManner.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scAjunctGoal.GetSingleValue());

		s_sSC.insert(Values::SyntaxCategories.scNameDefinition.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scWhatDefinition.GetSingleValue());
		s_sSC.insert(Values::SyntaxCategories.scWhoDefinition.GetSingleValue());

		s_sSC.insert(Values::SyntaxCategories.scPrepositionObject.GetSingleValue());
	}

	if(s_sWhoIsSC.empty())
	{
		s_sWhoIsSC.insert(Values::SyntaxCategories.scNameDefinition.GetSingleValue());
		s_sWhoIsSC.insert(Values::SyntaxCategories.scWhatDefinition.GetSingleValue());
		s_sWhoIsSC.insert(Values::SyntaxCategories.scWhoDefinition.GetSingleValue());
	}

	SS_CATCH(L"");
}

bool CTransformModule::IsWhoIsQuestion(ISentence* pSent)
{
	SS_TRY;

	for(CSentenceIterator it = pSent->GetFirstUnit(); *it; ++it)
	{
		if(IsUnitInSet(&s_sWhoIsSC, *it))
			return true;
	}

	SS_CATCH(L"");
	return false;
}

bool CTransformModule::IsNameDefinition(ISentence* pSent)
{
	SS_TRY;

	unsigned int uiNameDef = Values::SyntaxCategories.scNameDefinition.GetSingleValue();
	for(CSentenceIterator it = pSent->GetFirstUnit(); *it; ++it)
	{
		for(IIndex* pIndex = (*it)->GetFirstIndex(); pIndex; pIndex = (*it)->GetNextIndex())
		{
			if(pIndex->GetDictionaryIndex()->GetFirst().GetIndexType() != eitSyntaxIndex)
				continue;

			if(pIndex->GetDictionaryIndex()->GetFirst().GetId() == uiNameDef)
				return true;
		}
	}

	SS_CATCH(L"");
	return false;
}

bool CTransformModule::IsUnitInSet(set<unsigned int>* pst, IUnit* pUnit)
{
	SS_TRY;

	for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{
		if(pIndex->GetDictionaryIndex()->GetFirst().GetIndexType() != eitSyntaxIndex)
			continue;

		unsigned int uiIndex = pIndex->GetDictionaryIndex()->GetFirst().GetId();
		if(pst->find(uiIndex) != pst->end())
			return true;
	}

	SS_CATCH(L"");
	return false;
}

}
}
