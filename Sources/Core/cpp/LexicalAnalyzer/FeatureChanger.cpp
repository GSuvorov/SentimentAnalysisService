#include "stdafx.h"
#include "FeatureChanger.h"

#include "..\ASSInterface\TDictionaryManager.h"
using namespace SS::Dictionary::Types;


namespace SS
{
namespace LexicalAnalyzer
{

/////////////////////////////////////////////////////////////////////////////////////
// Constructors
/////////////////////////////////////////////////////////////////////////////////////

CFeatureChanger::CFeatureChanger(IBlackBoxTextStorage* pStorage, CMorphoFeature* pMorphoFeature, IAMConverterMorpho* pMorphoConverter) :
		m_pStorage(pStorage),
		m_pMorphoFeature(pMorphoFeature),
		m_pMorphoConverter(pMorphoConverter)
{
}

/////////////////////////////////////////////////////////////////////////////////////
// CFeatureChanger interface

void CFeatureChanger::ChangeUnitFeature(IUnit* pUnit, EChangeFeature ecf)
{
	SS_TRY;

	for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; )
	{
		bool bMoveNextIndex = true;

		EIndexType eit = pIndex->GetDictionaryIndex()->GetFirst().GetIndexType();
		if(eit == eitMorphoIndex || eit == eitCombinatoryIndex)
		{
			DictionariesNumber edn = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber();
			for(IFeature* pFeature = pIndex->GetFirstFeature(); pFeature; pFeature = pIndex->GetNextFeature())
			{
				m_pMorphoFeature->Undefine();
				m_pMorphoConverter->EnCode(edn, pFeature->GetMorphoInfo(), pFeature->GetPartOfSpeech(), 
					m_pMorphoFeature);

				EAction e = ChangeFeature(pFeature, edn, ecf);
				if(e == actRemove)
				{
					pIndex = pIndex->ReleaseIndex();
					bMoveNextIndex = false;
					break;
				}
				else if(e == actAdd)
				{
					AddFeature(pIndex, edn);
					break;
				}
			}
		}

		if(bMoveNextIndex)
			pIndex = pUnit->GetNextIndex();
	}

	SS_CATCH(L"");
}

/////////////////////////////////////////////////////////////////////////////////////
// CFeatureChanger functions

void CFeatureChanger::ConvertFeature(IFeature* pFeature, DictionariesNumber edn)
{
	SS_TRY;

	int iMorphoInfo = 0, iPartOfSpeech = 0;
	m_pMorphoConverter->Code(edn, m_pMorphoFeature, &iMorphoInfo, &iPartOfSpeech);

	pFeature->SetMorphoIndex(0);
	pFeature->SetMorphoInfo(iMorphoInfo);
	pFeature->SetPartOfSpeech(iPartOfSpeech);

	SS_CATCH(L"");
}

CFeatureChanger::EAction CFeatureChanger::ChangeFeature(IFeature* pFeature, DictionariesNumber edn, EChangeFeature ecf)
{
	SS_TRY;

	if(ecf == cfSetGenitive)
	{
		if( !m_pMorphoFeature->m_PartOfSpeechTypes.IsIntersectsWith(&Values::PartOfSpeechTypes.postNoun) &&
			!m_pMorphoFeature->m_PartOfSpeechTypes.IsIntersectsWith(&Values::PartOfSpeechTypes.Undefined))
		{
			return actRemove;
		}

		m_pMorphoFeature->m_PartOfSpeechTypes = Values::PartOfSpeechTypes.postNoun;
		m_pMorphoFeature->m_CaseType = Values::CaseType.ctGenitive;
	}
	else if(ecf == cfSetNounCommonSingle)
	{
		if(!m_pMorphoFeature->m_PartOfSpeechTypes.IsIntersectsWith(&Values::PartOfSpeechTypes.Undefined))
			return actNothing;

		m_pMorphoFeature->m_PartOfSpeechTypes = Values::PartOfSpeechTypes.postNoun;
		m_pMorphoFeature->m_CaseType = Values::CaseType.ctCommon;
		m_pMorphoFeature->m_NumberType = Values::NumberType.ntSingle;
	}
	else if(ecf == cfSetNegative)
	{
		m_pMorphoFeature->m_NegativeType = Values::NegativeType.ntNegation;
	}
	else if(ecf == cfSetOrdering)
	{
		m_pMorphoFeature->m_NumericType = Values::NumericType.ntOrdering;
	}
	else if(ecf == cfAddOrdering)
	{
		if(!m_pMorphoFeature->m_PartOfSpeechTypes.IsIntersectsWith(&Values::NumericType.ntCount))
			return actNothing;

		m_pMorphoFeature->m_NumericType = Values::NumericType.ntOrdering;
		return actAdd;
	}

	ConvertFeature(pFeature, edn);

	SS_CATCH(L"");
	return actNothing;
}

void CFeatureChanger::AddFeature(IIndex* pIndex, DictionariesNumber edn)
{
	SS_TRY;

	int iMorphoInfo = 0, iPartOfSpeech = 0;
	m_pMorphoConverter->Code(edn, m_pMorphoFeature, &iMorphoInfo, &iPartOfSpeech);
	if(iMorphoInfo || iPartOfSpeech)
	{
		IFeature* pFeature = m_pStorage->CreateFeature();
		pFeature->SetMorphoIndex(0);
		pFeature->SetMorphoInfo(iMorphoInfo);
		pFeature->SetPartOfSpeech(iPartOfSpeech);
		pIndex->AddFeature(pFeature);
	}

	SS_CATCH(L"");
}

}
}
