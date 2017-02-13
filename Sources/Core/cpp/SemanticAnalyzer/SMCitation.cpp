#include "StdAfx.h"
#include ".\smcitation.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{


CSMCitation::CSMCitation(void)
{
	m_uiSemanticCitation=0;
	m_sDescription=_T(__FUNCTION__);
}

CSMCitation::~CSMCitation(void)
{
}

bool CSMCitation::AnalizeSentence()
{
	SS_TRY
	{
		if(!GetFirstUnit()) return true;
		TBoxUnits oUnits;
		UINT uiSymbols=0,uiCount=0;
		do{
			if (IsContainFeature(efcSemanticCitation))
			{
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scObject);
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin);
			}
			//if ( m_LexemType.IsSQuotes() )
			//{//обработка цитат 
			//	oUnits.push_back(m_pCurrentUnit);
			//	uiCount=0;
			//	uiSymbols=0;
			//	bool bWasClose=false;
			//	Mem();
			//	while (!bWasClose && (uiCount<10|| IsStrictMode() )&& GetNextUnit() )
			//	{
			//		if (m_LexemType.IsSymbol()) 
			//		{
			//			if ( m_LexemType.IsSQuotes() )
			//			{
			//				bWasClose=true;
			//			}else uiSymbols++;
			//		}
			//		oUnits.push_back(m_pCurrentUnit);
			//		uiCount++;
			//	}
			//	if ( bWasClose && (uiSymbols<3 && ( uiCount-2)<7 ) || IsStrictMode() )
			//	{//была закрывающая кавычка и цитата не оч большая
			//		IUnit * pHead=JoinUpUnits(oUnits);
			//		AddSemanticIndex(m_uiSemanticCitation,pHead);
			//		SetSpecialType(ustCitation);
			//	}else GotoMem();
			//	oUnits.clear();
			//	}
		}while(GetNextUnit());
		return true;
	}
	SS_CATCH(L"")
}

bool CSMCitation::Init(IText * pIText)
{
	if(!CSMSentenceExpert::Init(pIText)) return false;
	if(m_uiSemanticCitation==0)
	{
		SS::Core::Features::CSemanticFeature oSemanticFeature;
		oSemanticFeature.m_SemanticType.Assign(Values::TSemanticType.smtService);
		oSemanticFeature.m_SServiceTypes.Assign(Values::TSServiceTypes.sstAcronymCitation);
		m_pIAMCSemantic->Code(&oSemanticFeature,(SS::Dictionary::Types::TDictionaryIndex*)&m_uiSemanticCitation);
	}
	return true;
}
//////////////////////////////////////
		}
	}
}