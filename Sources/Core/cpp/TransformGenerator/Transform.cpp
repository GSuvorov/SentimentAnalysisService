#include "StdAfx.h"
#include ".\Transform.h"

#include "..\FeatureLibrary\define.h"
using namespace SS::Core::Features;

#include "..\ASSInterface\iterators.h"
using namespace SS::Iterators;


namespace SS
{
namespace TransformGenerator
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CTransform::CTransform(ISemanticAnalysis* pSemanticAnalysis, IBlackBoxTextStorage* pTextStorage,
					   IBlackBoxQueryStorage* pQueryStorage, ICombinatory* pCombinatory) :
		m_iMax(1),
		m_iCurrClass(0),
		m_eLanguage(ealUndefined),
		m_fWeight(1.),
		m_pSemanticAnalysis(pSemanticAnalysis),
		m_pTextStorage(pTextStorage),
		m_pQueryStorage(pQueryStorage),
		m_pCombinatory(pCombinatory)
{
}

CTransform::~CTransform()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CTransform interface

bool CTransform::Compare(CTransform* pTran)
{
	SS_TRY;

	TItemList::iterator it = m_lSeq.FirstValid(),
						itComp = pTran->m_lSeq.FirstValid();
	for(; it != m_lSeq.end() && itComp != pTran->m_lSeq.end(); m_lSeq.NextValid(it), pTran->m_lSeq.NextValid(itComp))
	{
		if((*it)->sc != (*itComp)->sc)
			return false;
	}

	return true;

	SS_CATCH(L"");
	return false;
}

void CTransform::AddSC(int sc)
{
	SS_TRY
	{
		SGroupItem* pItem = new SGroupItem;
		pItem->sc = sc;
		pItem->pUnit = NULL;
		m_lSeq.push_back(pItem);
	}
	SS_CATCH(L"")
}

void CTransform::AddGroup(IUnit* pUnit)
{
	SS_TRY;

	ESyntacticCategory sc = GetSyntaxCategory(pUnit);

	bool bInserted = false;
	for(TItemList::iterator it = m_lSeq.begin(); it != m_lSeq.end() && (*it)->sc; it++)
	{
		// Если находим категорию группы в шаблоне, добавляем ее в эту позицию (если она не занята)
		if((*it)->sc == sc && !(*it)->pUnit)
		{
			(*it)->pUnit = pUnit;
			bInserted = true;
			break;
		}
	}

	// Если категория группы отсутствует в шаблоне трансформации, добавляем ее в конец массива
	if(!bInserted)
	{
		SGroupItem* pItem = new SGroupItem;
		pItem->sc = 0;
		pItem->pUnit = pUnit;
		m_lSeq.push_back(pItem);
	}

	SS_CATCH(L"");
}

ITransformation* CTransform::GetFirstTransform(EAnalyseLanguage eLanguage, const SUnitParams& oParams)
{
	SS_TRY;

	m_iMax = 1;
	m_iCurrClass = 0;
	m_eLanguage = eLanguage;
	m_oParams = oParams;
	FillSemanticClasses();
	return GenerateTransformation();

	SS_CATCH(L"");
}

ITransformation* CTransform::GetNextTransform()
{
	SS_TRY;

	if(++m_iCurrClass == m_iMax)
		return NULL;
	return GenerateTransformation();

	SS_CATCH(L"");
}

void CTransform::SetWeight(float fWeight)
{
	m_fWeight = fWeight;
}

/////////////////////////////////////////////////////////////////////////////////////
// CTransform functions

ITransformation* CTransform::GenerateTransformation()
{
	SS_TRY;

	ITransformation* pTrans = m_pQueryStorage->CreateTransformation();
	pTrans->SetWeight(m_fWeight);

	bool bBegun = false;
	int iNum = 0;
	for(TItemList::iterator it = m_lSeq.begin(); it != m_lSeq.end(); it++)
	{
		if(!(*it)->pUnit)
			continue;

		IGroup* pGroup = GetGroup(*it);
		pTrans->AddGroup(pGroup);
		switch(m_eLanguage)
		{
			case ealEnglish:
				if((*it)->sc)
				{
					// Всем группам, категория которых присутствует в шаблоне, даем одинаковый SequenceID
					pGroup->SetSequenceID(iNum);
					bBegun = true;
				}
				else
				{
					if(bBegun)
						pGroup->SetSequenceID(++iNum);
					else
					{
						pGroup->SetSequenceID(0);
						bBegun = true;
					}
				}
				break;

			case ealUndefined:
			case ealRussian:
				pGroup->SetSequenceID(iNum++);
				break;
		}
	}

	return pTrans;

	SS_CATCH(L"");
}

void CTransform::FillSemanticClasses()
{
	SS_TRY;

	for(TItemList::iterator it = m_lSeq.begin(); it != m_lSeq.end(); ++it)
	{
		if(!(*it)->pUnit)
			continue;

		for(CSentenceIterator its = (*it)->pUnit; *its != (*it)->pUnit->GetRightUnit(); ++its)
		{
			if(!(*its)->HasChilds())
				continue;

			FillSemanticClasses(*it, *its);
		}
	}

	SS_CATCH(L"");
}

void CTransform::FillSemanticClasses(SGroupItem* pItem, IUnit* pUnit)
{
	SS_TRY;

	typedef multimap<int, TDictionaryIndex> TFormMap;

	TFormMap mClasses;
	for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{
		TDictionaryIndex oInd = pIndex->GetDictionaryIndex()->GetFirst();
		if(oInd.GetIndexType() != eitCombinatoryIndex)
			continue;

		list<TDictionaryIndex> lstClasses;
		m_pSemanticAnalysis->GetClasses(oInd, &lstClasses);

		for(list<TDictionaryIndex>::iterator itl = lstClasses.begin(); itl != lstClasses.end(); ++itl)
		{
			list<TDictionaryIndex> lstWords;
			m_pSemanticAnalysis->SetClass(*itl);
			m_pSemanticAnalysis->GetWords(&lstWords);
			for(list<TDictionaryIndex>::iterator itf = lstWords.begin(); itf != lstWords.end(); ++itf)
			{
				SCombinationNewInfo oCombiInfo;
				if(!m_pCombinatory->GetCombinationInfo(itf->GetDictionaryIndex(), &oCombiInfo))
					continue;

				mClasses.insert(TFormMap::value_type(oCombiInfo.NumOfMembers, *itf));
			}
		}
	}

	if(!mClasses.empty())
	{
		// create combi unit
		SCombiUnit* pCombi = new SCombiUnit;
		pCombi->pUnit = pUnit;
		pCombi->bMerge = false;
		pItem->lCombiUnits.push_back(pCombi);

		// fill combi unit's classes
		int iPrev = 0;
		for(TFormMap::iterator it = mClasses.begin(); it != mClasses.end(); ++it)
		{
			if(iPrev == (*it).first)
				pCombi->bMerge = true;
			pCombi->vClasses.push_back((*it).second);
			iPrev = (*it).first;
		}

		if(!pCombi->bMerge)
			m_iMax = max(m_iMax, mClasses.size());
	}

	SS_CATCH(L"");
}

IGroup* CTransform::GetGroup(SGroupItem* pItem)
{
	SS_TRY;

	//if(!pItem->lCombiUnits.empty())
	//{
	//	// rebuild group if there are some combinatory synonyms
	//	for(TCombiUnits::iterator it = pItem->lCombiUnits.begin(); it != pItem->lCombiUnits.end(); ++it)
	//	{
	//		if((*it)->bMerge)
	//		{
	//			for(int iNum = 0; iNum < (*it)->vClasses.size(); iNum++)
	//				AddClassToCombination(*it, iNum, iNum == 0);
	//		}
	//		else
	//		{
	//			int iNum = min(m_iCurrClass, (*it)->vClasses.size() - 1);
	//			AddClassToCombination(*it, iNum, true);
	//		}
	//	}
	//}

	CGroup oGroup(m_pQueryStorage);
	oGroup.Create(pItem->pUnit, m_oParams);
	return oGroup;

	SS_CATCH(L"");
	return NULL;
}

void CTransform::AddClassToCombination(SCombiUnit* pCombi, int iNum, bool bRemoveOldUnits)
{
	SS_TRY;

	TDictionaryIndex oInd = pCombi->vClasses[iNum];

	SCombinationNewInfo oCombiInfo;
	if(!m_pCombinatory->GetCombinationInfo(oInd.GetDictionaryIndex(), &oCombiInfo))
		return;

	if(bRemoveOldUnits)
	{
		for(IUnit* pChild = pCombi->pUnit->GetLeftChildUnit(); pChild; )
			pChild = pChild->ReleaseUnitHorizontal();
		for(int i = 0; i < oCombiInfo.NumOfMembers; i++)
		{
			IUnit* pChild = m_pTextStorage->CreateUnit();
			FillUnit(pChild, &oCombiInfo.WordIndexes[i]);
			pCombi->pUnit->AddUnitVertical(pChild);
		}
	}
	else
	{
		IUnit* pChild = pCombi->pUnit->GetLeftChildUnit();
		for(int i = 0; i < oCombiInfo.NumOfMembers; i++)
		{
			FillUnit(pChild, &oCombiInfo.WordIndexes[i]);
			pChild = pChild->GetRightUnit();
		}
	}

	SS_CATCH(L"");
}

void CTransform::FillUnit(IUnit* pUnit, CoSeDi::DataTypes::SWordIndex* pInfo)
{
	SS_TRY;

	for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{
		if(pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex() == pInfo->IDSource)
			return;
	}

	IIndex* pIndex = m_pTextStorage->CreateIndex();
	if(m_oParams.m_bIDForm && pInfo->IDForm != -1)
	{
		IFeature* pFeature = m_pTextStorage->CreateFeature();
		pFeature->SetMorphoIndex(pInfo->IDForm);
		pIndex->AddFeature(pFeature);
	}

	TDictionaryIndex oInd(pInfo->IDSource);
	pIndex->GetDictionaryIndex()->AppendIndex(oInd);
	pUnit->AddIndex(pIndex);

	SS_CATCH(L"");
}

}
}
