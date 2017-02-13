#include "StdAfx.h"
#include ".\smquestionanalysisr.h"
#include "RQKogda.h"
#include "RQChto.h"
#include "RQKto.h"
#include "RQKakoi.h"
#include "RQSkolko.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{
				namespace Russian
				{



CSMQuestionAnalysisR::CSMQuestionAnalysisR(void)
{
	m_bInit = false;
	m_AnalyseLanguage=SS::Core::Types::ealRussian;
	m_sDescription=_T("CSMQuestionAnalysisR");
	//---m_sDescription=_T(__FUNCTION__);

	m_pRQKogda=NULL;
	m_pRQChto=NULL;
	m_pRQKakoi=NULL;
	m_pRQKto=NULL;
	m_pRQSkolko=NULL;

}

CSMQuestionAnalysisR::~CSMQuestionAnalysisR(void)
{
	if(m_pRQKakoi) delete m_pRQKakoi;
	if(m_pRQChto) delete m_pRQChto;
	if(m_pRQKto) delete m_pRQKto;
	if(m_pRQKogda) delete m_pRQKogda;
	if(m_pRQSkolko) delete m_pRQSkolko;
}
bool CSMQuestionAnalysisR::AnalyseQuestion(EQuestionType eQType)
{
	return false;
}

bool CSMQuestionAnalysisR::AnalizeSentence()
{
	SS_TRY
	{
		if(m_AnalyseLanguage!=m_pCurrentSentence->GetLanguage()) return true;

		EQuestionType eQType;
		m_Frame.Clear();
		ProcessSentence();

		m_DInfo.Init();

		eQType=DetectQuestionTypeRussian();

		if(eQType==eqtNon) 
		{
			//StrictQuery();
			if(GetFirstUnit())
			{
				do{
					AddSyntaxCategory(m_pCurrentUnit,Values::SyntaxCategories.scAjunct);
					MoveToAjunct(m_pCurrentUnit);
				}while(GetNextUnit());
			}
			//SimpleQuery();
			return true;
		}

		MarkUnits(eQType);

		FillGroups();

		CSMQuestionAnalysisR * pExpert=NULL;

		switch(eQType) {
		case eqtRKogda:
		case eqtRGde:
		case eqtRKuda:
		case eqtROtkuda:
		case eqtRPochemu:
		case eqtRKak:
		case eqtRKakManner:
		case eqtROther:
			pExpert=m_pRQKogda;
			break;
		case eqtRChto:
			pExpert=m_pRQChto;
			break;
		case eqtRKto:
			pExpert=m_pRQKto;
			break;
		case eqtRKakoi:
		case eqtRKakova:
			pExpert=m_pRQKakoi;
			break;
		case eqtRSkolko:
			pExpert=m_pRQSkolko;
			break;
		default:
			break;
		}
		if(pExpert)
		{
	//		pExpert->SetSMManager(m_pSMManager);
			pExpert->Init(m_pSMManager,m_DInfo);
			pExpert->SetCurrentSentence(m_pCurrentSentence);
			pExpert->SetAnalyseParams(GetAnalyseParams());
			pExpert->AnalyseQuestion(eQType);
		}
		return true;
	}
	SS_CATCH(L"")
}
bool CSMQuestionAnalysisR::Init(IText * pIText)
{
	if(!CSMQuestionAnalysis::Init(pIText)) return false;

	if(!m_bInit)
	{
		m_bInit = true;

		list< pair<wstring,EQuestionType> > l_StrType;
		list< pair<wstring,EQuestionType> >::iterator itL;
		list<SS::Dictionary::Types::TWordInfo> oMorpho;
		list<SS::Dictionary::Types::TWordInfo>::iterator itM;
		if(m_SourceTypeTable.empty())
		{
			l_StrType.push_back(pair<wstring,EQuestionType>(_T("что"),eqtRChto));
			l_StrType.push_back(pair<wstring,EQuestionType>(_T("кто"),eqtRKto));
			l_StrType.push_back(pair<wstring,EQuestionType>(_T("где"),eqtRGde));
			l_StrType.push_back(pair<wstring,EQuestionType>(_T("когда"),eqtRKogda));
			l_StrType.push_back(pair<wstring,EQuestionType>(_T("куда"),eqtRKuda));
			l_StrType.push_back(pair<wstring,EQuestionType>(_T("откуда"),eqtROtkuda));
			l_StrType.push_back(pair<wstring,EQuestionType>(_T("сколько"),eqtRSkolko));
			l_StrType.push_back(pair<wstring,EQuestionType>(_T("как"),eqtRKak));
			l_StrType.push_back(pair<wstring,EQuestionType>(_T("какой"),eqtRKakoi));
			l_StrType.push_back(pair<wstring,EQuestionType>(_T("какова"),eqtRKakova));

			l_StrType.push_back(pair<wstring,EQuestionType>(_T("почему"),eqtRPochemu));
			l_StrType.push_back(pair<wstring,EQuestionType>(_T("зачем"),eqtROther));
			l_StrType.push_back(pair<wstring,EQuestionType>(_T("отчего"),eqtROther));
			l_StrType.push_back(pair<wstring,EQuestionType>(_T("насколько"),eqtROther));

			for (itL=l_StrType.begin();itL!=l_StrType.end();itL++) 
			{
				oMorpho.clear();
				GetWordMorphoInfo(itL->first.c_str(),oMorpho);
				//m_pMorphoService->GetWordMorphoInfo(itL->first.c_str(),oMorpho,SS::Dictionary::DATA_TYPE::efzBaseDictionary);
				for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
				{
					m_MorphoFeature.Undefine();
					m_pIAMCMorpho->EnCode(
						itM->front().m_WordIndex.m_DictIndex.GetDictionaryNumber(),
						itM->front().m_MorphoTypeInfo.m_MorphoInfo,
						itM->front().m_SpecialMorpho,
						&m_MorphoFeature);
					if ( m_MorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postPronoun) || m_MorphoFeature.m_PartOfSpeechTypes.Equal(Values::PartOfSpeechTypes.postAdverb) )
					{
						m_SourceTypeTable.insert(
							map<UINT,EQuestionType>::value_type(
							itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*  ->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()*/,
							itL->second));
						break;
					}
				}

			}
		}
		list< pair<wstring,Types::PartOfSpeechTypes> > l_WRemove;
		list< pair<wstring,Types::PartOfSpeechTypes> >::iterator itWR;
		if(m_s_WordToRemove.empty())
		{
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("такой"),Values::PartOfSpeechTypes.postPronoun));
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("долго"),Values::PartOfSpeechTypes.postAdverb));
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("равен"),Values::PartOfSpeechTypes.postAdjective));
			for (itWR=l_WRemove.begin();itWR!=l_WRemove.end();itWR++) 
			{
				oMorpho.clear();
				GetWordMorphoInfo(itWR->first.c_str(),oMorpho);
				for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
				{
					m_s_WordToRemove.insert(
						itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()
						);
				}
			}

			l_WRemove.clear();
		}

		if(m_s_VerbToRemove.empty())
		{
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("cмочь"),Values::PartOfSpeechTypes.postVerb));
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("стать"),Values::PartOfSpeechTypes.postVerb));
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("быть"),Values::PartOfSpeechTypes.postVerb));
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("начать"),Values::PartOfSpeechTypes.postVerb));
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("начаться"),Values::PartOfSpeechTypes.postVerb));//patch
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("должен"),Values::PartOfSpeechTypes.postAdjective));
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("может"),Values::PartOfSpeechTypes.postVerb));
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("мочь"),Values::PartOfSpeechTypes.postVerb));

			for (itWR=l_WRemove.begin();itWR!=l_WRemove.end();itWR++) 
			{
				oMorpho.clear();
				GetWordMorphoInfo(itWR->first.c_str(),oMorpho);
				for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
				{
					m_s_VerbToRemove.insert(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex());
				}
			}
		}
		if (m_s_RusComplexQ.empty())
		{
			l_WRemove.clear();

			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("образом"),Values::PartOfSpeechTypes.postNoun));
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("способом"),Values::PartOfSpeechTypes.postNoun));
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("связи"),Values::PartOfSpeechTypes.postNoun));
			l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("причина"),Values::PartOfSpeechTypes.postNoun));
			//l_WRemove.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("должен"),Values::PartOfSpeechTypes.postAdjective));

			for (itWR=l_WRemove.begin();itWR!=l_WRemove.end();itWR++) 
			{
				oMorpho.clear();
				GetWordMorphoInfo(itWR->first.c_str(),oMorpho);
				for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
				{
					m_s_RusComplexQ.insert(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex());
				}
			}
		}
		if(!m_pRQKogda)
			m_pRQKogda=new CRQSimpleAdjunct;
		if(!m_pRQChto)
			m_pRQChto=new CRQChto;
		if(!m_pRQKto)
			m_pRQKto=new CRQKto;
		if(!m_pRQKakoi)
			m_pRQKakoi=new CRQKakoi;
		if(!m_pRQSkolko)
			m_pRQSkolko=new CRQSkolko;

	}

	return true;
}
bool CSMQuestionAnalysisR::Init(CSMManager * pManager, SDescriptionInfo & DInfo)
{
	SetSMManager(pManager);
	CSMQuestionAnalysis::Init(NULL);

	m_DInfo=DInfo;
	return true;
}


bool CSMQuestionAnalysisR::CheckWhoCategory(IUnit * pHead, IUnit * pUnits,bool & bNameDef)
{
	if(!pHead || !pUnits) return false;

	SS::Core::Features::CSemanticFeature oF;
	IUnit * pUnit;
	bool bChilds=false;
	if (pUnits->HasChilds())
	{
		bChilds=true;
		pUnit=pUnits->GetLeftChildUnit();
	}else pUnit=pUnits;

	if (pUnit)
	{
		do{
			oF.Undefine();
			if( CollectSemanticFeature(oF,pUnit) )				
			{
				if ( ( oF.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName)||
					oF.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName)||
					oF.m_SPersonType.IsContain(Values::TSPersonType.semprstMiddleName)) 
					)
				{
					AddSyntaxCategory(pHead,Values::SyntaxCategories.scNameDefinition);
						bNameDef=true;
					return true;
				}else if (oF.m_SemanticType.IsContain(Values::TSemanticType.smtPlace) || oF.m_SemanticType.IsContain(Values::TSemanticType.smtPerson))
				{
					AddSyntaxCategory(pHead,Values::SyntaxCategories.scWhoDefinition);
						bNameDef=false;
					return true;
				}
			}
		}while(bChilds && (pUnit=pUnit->GetRightUnit()));
	}

	SetSyntaxCategory(pHead,Values::SyntaxCategories.scAdditionalEntity);
	AddAllPersonSemantic(pHead);
	return false;
}


EQuestionType CSMQuestionAnalysisR::DetectQuestionTypeRussian()
{
	UINT uiDeep=0;
	EQuestionType eQuestionType=eqtNon;
	map<UINT,EQuestionType>::iterator itQ;
	set<UINT>::iterator itF;
	bool bGood=false;
	///find question word 
	m_DInfo.Init();
	SMorphoDescription * pDescription;
	IUnit * pMem;
	if(!GetFirstUnit()) return eqtNon;
	do{
		if(IsSyntax()) continue;
		pDescription=GetUnitDescription();
		if(!pDescription || !GetFirstIndex()) continue;
		do{

			//wstring str = m_pCurrentUnit->GetWord();

			if(!IsMorphoIndex()) continue;
			itQ=m_SourceTypeTable.find(m_pCurrentIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex() & 0x7FFFFFFF);
			if (itQ!=m_SourceTypeTable.end())
			{
				if(eQuestionType==eqtNon) eQuestionType=itQ->second;
				pDescription->_ePOS=eposQuestion;
				pMem=m_pCurrentUnit;
				if (eQuestionType==eqtRChto)
				{
					bool bRemoveSubj=false;
					for( GetFirstIndex(); m_pCurrentIndex ;  )
					{
						if(/*(*/m_pCurrentIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*GetFirst())>>24*/ == SS::Dictionary::DATA_TYPE::NAMES::ednRProperName)
						{
							m_pCurrentIndex=m_pCurrentIndex->ReleaseIndex();
							bRemoveSubj=true;
						}else GetNextIndex();
					}
					if(bRemoveSubj)
					{
						RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin);
						RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject);
						pDescription->_oMorphoFeature.Undefine();
						CollectMorphoFeature(pDescription->_oMorphoFeature);
					}

				}
				if(eQuestionType!=eqtRKto&&eQuestionType!=eqtRChto)
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin);
				bGood=true;
				break;
			}else if(m_s_RusComplexQ.find(m_pCurrentIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex() & 0x7FFFFFFF)!=m_s_RusComplexQ.end())
			{
				eQuestionType=eqtROther;
				break;
			}

		}while(GetNextIndex());
		
	}while( GetNextUnit() && !bGood && uiDeep++<6);

	if(!bGood) return eqtNon;

	IUnit * pU2=m_pCurrentUnit;
	uiDeep=0;
	bGood=false;
	while( !bGood && uiDeep++<2 && GetNextUnit() && (pDescription=GetUnitDescription()) )
	{
		for (GetFirstIndex(); m_pCurrentIndex ; GetNextIndex()) 
		{
			if(m_s_RusComplexQ.find(m_pCurrentIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex() & 0x7FFFFFFF)!=m_s_RusComplexQ.end())
			{
				pDescription->_ePOS=eposQuestion;
				GetUnitDescription(pMem)->_ePOS=eposNon;
				eQuestionType=eqtROther;
				pU2=m_pCurrentUnit;
				bGood=true;
				break;
			}

		}
	}
	m_pCurrentUnit=pU2;
	wstring sWord;
	if (m_pCurrentUnit)
	{
		sWord=m_pCurrentUnit->GetWord();
		m_StringService.LowerSz((TCHAR*)sWord.c_str());
		if (m_StringService.StrEqual(_T("и"),sWord.c_str()))
		{
			if (GetNextUnit())
			{
				sWord=m_pCurrentUnit->GetWord();
				m_StringService.LowerSz((TCHAR*)sWord.c_str());
				if (m_StringService.StrEqual(_T("когда"),sWord.c_str())||m_StringService.StrEqual(_T("где"),sWord.c_str()) )
				{
					GetPrevUnit();
					HideCurrentUnit();
				}
			}
		}
	}


	// = такой,долго ? 
	set<UINT>::iterator itWR;
	bool bWasRemove=false;
	if(m_pCurrentUnit)
	{
		for (GetFirstIndex(); m_pCurrentIndex ; GetNextIndex()) 
		{
			itWR=m_s_WordToRemove.find(m_pCurrentIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex() & 0x7FFFFFFF);
			if (itWR!=m_s_WordToRemove.end())
			{
				HideCurrentUnit();
				bWasRemove=true;
				break;
			}
		}

	}
		
	if(!bWasRemove && eQuestionType==eqtRKak) eQuestionType=eqtRKakManner;

	if(bWasRemove && eQuestionType==eqtRChto)
	{
		pDescription=GetUnitDescription(m_pCurrentUnit->GetLeftUnit());
			if(pDescription->_oMorphoFeature.m_CaseType.IsContain(Values::CaseType.ctDative))
			eQuestionType=eqtRKakova;
	}

	return eQuestionType;

}

void CSMQuestionAnalysisR::MarkUnits(EQuestionType eQuestionType)
{
	SMorphoDescription * pMem=NULL, * pUnitD;
	SMorphoDescription * pDescription;
	set<UINT>::iterator itWR;

	for(; m_pCurrentUnit; GetNextUnit() )
	{
		//itD->_Unit->GetWord(sz_Word);
		//wstring str = m_pCurrentUnit->GetWord();
		pDescription=GetUnitDescription();
		if(  
			pDescription->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb) && 
			(!pDescription->_oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple) ||
			pDescription->_oMorphoFeature.m_ShortType.IsContain(Values::ShortType.stShort)) && 
			!pDescription->_oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfDeepr) 
			)
		{
			if ( pMem ) pMem->_ePOS=eposNon;
			pMem=pDescription;

			if (pDescription->_oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfPersonal)||
				m_s_VerbToRemove.find(m_pCurrentUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetDictionaryIndex() & 0x7FFFFFFF)!=m_s_VerbToRemove.end())
			{
				//itWR=m_s_VerbToRemove.find(m_pCurrentUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst());
				//if (itWR==m_s_VerbToRemove.end()) itWR=m_s_VerbToRemove.find(m_pCurrentUnit->GetLIndex()->GetDictionaryIndex()->GetFirst());
				//if(itWR!=m_s_VerbToRemove.end())
				//{
					pUnitD=GetUnitDescription(m_pCurrentUnit->GetRightUnit());
					if(pUnitD &&
						pUnitD->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb) && 
						( pUnitD->_oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfInfinitive) ||
						pUnitD->_oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple) &&
						pUnitD->_oMorphoFeature.m_ShortType.IsContain(Values::ShortType.stShort))
						)
					{

						SS::Core::Features::CSemanticFeature oF;
						bool bAimSemantic = false;
						oF.Undefine();
						if( CollectSemanticFeature(oF))
						{
							bAimSemantic = oF.m_SemanticType.Equal(Values::TSemanticType.smtVerb) && 
								(oF.m_SVerbType.Equal(Values::TSVerbType.semvtCost) ||
								oF.m_SVerbType.Equal(Values::TSVerbType.semvtWeigh));

							
						}
						if(m_pCurrentUnit->GetRightUnit() && !bAimSemantic)
						{
							m_pCurrentUnit=m_pCurrentUnit->ReleaseUnitHorizontal();
						}else if (!bAimSemantic)
						{
							m_pCurrentUnit->ReleaseUnitHorizontal();
							GetLastUnit();
						}
						pDescription=GetUnitDescription();
					}
				//}
			}

			//wstring str = m_pCurrentUnit->GetWord();
			pDescription->_ePOS=eposVerbMain;
			m_DInfo._pVerb=m_pCurrentUnit;
		}
	}

	m_pCurrentUnit=NULL;

	if(m_DInfo._pVerb)
		RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,m_DInfo._pVerb);
	//ищем означенный предлоги
	if( //m_pMorphoService->GetPrepositionSyntax(tDescription.front()._Unit->GetWord(sz_Word),NULL) 
		IsPrepositionSyntax(GetFirstUnit())
		)
	{
		GetUnitDescription()->_ePOS=eposPrep;
		m_DInfo._pPrep=m_pCurrentUnit;
	}
}
void CSMQuestionAnalysisR::FillGroups()
{
	SMorphoDescription * pDescription;
	SS::Core::Features::CSemanticFeature oSemanticFeature;
	IUnit * pHead;
	list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;

	pDescription=GetUnitDescription(GetFirstUnit());
	
	TBoxUnits l_Units,l_Units2;

	do{
		pDescription=GetUnitDescription();
		l_Units.push_back(m_pCurrentUnit);
		//wstring str = m_pCurrentUnit->GetWord();
		if (pDescription->_ePOS==eposQuestion) 
		{
			break;
		}
		else GetNextUnit();
	}while(m_pCurrentUnit );

	if(m_pCurrentUnit==NULL) return ;

	pHead=JoinUpUnits(l_Units);
	pHead->CreateSyntaxFeature()->SetIsQuestion();

	m_DInfo._pQuestion=pHead;

	m_pCurrentUnit=pHead;

	GetNextUnit();
	//wstring str = m_pCurrentUnit->GetWord();//
	while(m_pCurrentUnit && IsSyntax()) 
	{
		//str = m_pCurrentUnit->GetWord();
		GetNextUnit();
	}

	//if(m_pCurrentUnit)
		//str = m_pCurrentUnit->GetWord();//

	l_Units.clear();
	while( m_pCurrentUnit &&  GetUnitDescription()->_ePOS!=eposVerbMain && !IsSyntax() )
	{
		//str = m_pCurrentUnit->GetWord();
		l_Units.push_back(m_pCurrentUnit);
		GetNextUnit();
	}

	while( m_pCurrentUnit && IsSyntax() ) GetNextUnit();

	if (m_DInfo._pVerb)
	{
		if (m_pCurrentUnit)
		{
			//wstring str = m_pCurrentUnit->GetWord();
			IUnit * pVerb=m_DInfo._pVerb;

			pHead=m_pIBlackBoxFactory->CreateUnit();
			if(pVerb != m_pCurrentUnit && GetUnitDescription()->_ePOS == eposVerbMain)
			{
				m_pCurrentUnit->InsertUnitVertical(pHead);;
				pVerb->Pop();
				pHead->GetRightChildUnit()->AddUnitHorizontal(pVerb);

				RemoveSemanticIndicesFromUnit(l_Sem);
				KeepSpecifedSemanticTypes(Values::TSemanticType.smtVerb,l_Sem);
				SetSemanticIndices(l_Sem);
			}else
				pVerb->InsertUnitVertical(pHead);

			SetSyntaxCategory(pHead,Values::SyntaxCategories.scPredicate);

			RemoveSemanticIndicesFromUnit(l_Sem,pVerb);
			KeepSpecifedSemanticTypes(Values::TSemanticType.smtVerb,l_Sem);
			SetSemanticIndices(l_Sem,pVerb);

			m_pCurrentUnit=pHead;
			m_DInfo._pPredicate=pHead;

			if (IsContainFeature(efcRepresentBecome,pVerb))
			{
				GetNextUnit();
				pDescription = GetUnitDescription();
				if(pDescription && pDescription->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postAdjective) && 
					pDescription->_oMorphoFeature.m_ShortType.IsContain(Values::ShortType.stShort))
				{
					IUnit  *pUnit=NULL;
					pUnit = m_pCurrentUnit;
					m_pCurrentUnit = pUnit->Pop();
					m_DInfo._pPredicate->GetRightChildUnit()->AddUnitHorizontal(pUnit);
				}
			}else
			GetNextUnit();

			while(m_pCurrentUnit && IsSyntax()) GetNextUnit();

			if (l_Units.empty()==false) 
			{
				while( m_pCurrentUnit && !IsSyntax() )
				{
					l_Units2.push_back(m_pCurrentUnit);
					GetNextUnit();
				}
			}else
			{
				while( m_pCurrentUnit && !IsSyntax() )
				{
					l_Units.push_back(m_pCurrentUnit);
					GetNextUnit();
				}
			}

			while(m_pCurrentUnit && IsSyntax()) GetNextUnit();
		}
	}

	while(m_pCurrentUnit && !IsSyntax())
	{
		m_pCurrentUnit->CreateSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Assign(Values::SyntaxCategories.scAjunct);
		GetNextUnit();
	}
	if (l_Units.empty()==false)
	{
		pHead=JoinUpUnits(l_Units);
		SetSyntaxCategory(pHead,Values::SyntaxCategories.scMainEntity);
		m_DInfo._pMainEntity=pHead;
	}
	if (l_Units2.empty()==false)
	{
		pHead=JoinUpUnits(l_Units2);
		SetSyntaxCategory(pHead,Values::SyntaxCategories.scAdditionalEntity);
		m_DInfo._pAdditionalEntity=pHead;
	}

}

bool CSMQuestionAnalysisR::IsMorphologyCoordinated(IUnit * pU1, IUnit * pU2 , bool bCase, bool bGender , bool bNumber , bool bScipInstrumentalCase)
{
	if(!pU1 || !pU2 ) return false;
	if( !bCase && !bGender && !bNumber) return true;
	SS::Core::Features::CMorphoFeature oMorphoFeature2;
	IIndex * pIndex1, * pIndex2;
	IFeature * pF1, * pF2;
	for (pIndex1=pU1->GetFirstIndex() ; pIndex1 ; pIndex1=pU1->GetNextIndex()) 
	{
		if(!IsMorphoIndex(pIndex1)) continue;
		for(pIndex2=pU2->GetFirstIndex(); pIndex2 ; pIndex2=pU2->GetNextIndex())
		{
			if(!IsMorphoIndex(pIndex2)) continue;

			for (pF1=pIndex1->GetFirstFeature(); pF1 ; pF1=pIndex1->GetNextFeature()) 
			{
				m_pIAMCMorpho->EnCode(pIndex1->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*GetFirst() >> 24*/ , pF1->GetMorphoInfo(),pF1->GetPartOfSpeech(),&m_MorphoFeature);
				for (pF2=pIndex2->GetFirstFeature(); pF2 ; pF2=pIndex2->GetNextFeature()) 
				{
					m_pIAMCMorpho->EnCode(pIndex2->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*GetFirst() >> 24*/ , pF2->GetMorphoInfo(),pF2->GetPartOfSpeech(),&oMorphoFeature2);

					if(m_MorphoFeature.m_GenderType.GetValue()<=1)m_MorphoFeature.m_GenderType.FillUp();
					if(oMorphoFeature2.m_GenderType.GetValue()<=1)oMorphoFeature2.m_GenderType.FillUp();
					if(m_MorphoFeature.m_NumberType.GetValue()<=1)m_MorphoFeature.m_NumberType.FillUp();
					if(oMorphoFeature2.m_NumberType.GetValue()<=1)oMorphoFeature2.m_NumberType.FillUp();
					if(m_MorphoFeature.m_CaseType.GetValue()<=1)m_MorphoFeature.m_CaseType.FillUp();
					if(oMorphoFeature2.m_CaseType.GetValue()<=1)oMorphoFeature2.m_CaseType.FillUp();

					m_MorphoFeature.m_GenderType.AssignAND(&oMorphoFeature2.m_GenderType);
					m_MorphoFeature.m_NumberType.AssignAND(&oMorphoFeature2.m_NumberType);
					m_MorphoFeature.m_CaseType.AssignAND(&oMorphoFeature2.m_CaseType);

					if( bGender&&m_MorphoFeature.m_GenderType.GetValue()<=1 ||
						bNumber && m_MorphoFeature.m_NumberType.GetValue()<=1 ||
						bCase && m_MorphoFeature.m_CaseType.GetValue()<=1 && 
						( !bScipInstrumentalCase || !oMorphoFeature2.m_CaseType.IsContain(Values::CaseType.ctInstrumental) )  
					)
					return false;
				}

			}

		}
	}
	return true;

}

bool CSMQuestionAnalysisR::CheckSubjectFin(IUnit * pHead)
{
	if(!pHead) return false;
	if(!pHead->HasChilds())
	{
		if(IsContainFeature(efRCaseNominative,pHead))
		{
			SetSyntaxCategory(pHead,Values::SyntaxCategories.scMainEntity);
			return true;
		}
	}else
	{
		for (IUnit * pUnit=pHead->GetLeftChildUnit(); pUnit ; pUnit=pUnit->GetRightUnit() ) 
		{
			if(IsContainFeature(efRCaseNominative,pUnit))
			{
				SetSyntaxCategory(pHead,Values::SyntaxCategories.scMainEntity);
				return true;
			}

		}
	}
	return false;
}
Types::SyntaxCategories CSMQuestionAnalysisR::LegacyCategoryToEntity(IUnit * pUnit, IUnit * pVerb)
{
	bool bSubject=false,bWhoDef=false;
	if(IsContainSyntaxCategoryIndex(Values::SyntaxCategories.scWhoDefinition,pUnit)||
		IsContainSyntaxCategoryIndex(Values::SyntaxCategories.scWhatDefinition,pUnit))
	{
		RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pUnit);
		RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pUnit);
		bWhoDef=true;
	}
	else if(IsContainSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin,pUnit))
		bSubject=true;
	if (!bSubject && !bWhoDef) return Values::SyntaxCategories.scAdditionalEntity;
	if(pVerb )
	{
		if (!IsMorphologyCoordinated(pVerb,pUnit,false,true,true))
		{
			return Values::SyntaxCategories.scAdditionalEntity;
		}
		RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject,pUnit);


	}
	if(bSubject)
		return Values::SyntaxCategories.scMainEntity;
	return Values::SyntaxCategories.scAdditionalEntity;

}

void CSMQuestionAnalysisR::MoveToAjunct(IUnit * pGroup)
{
	if(!pGroup) return;

	if (pGroup->HasChilds())
	{
		IUnit * pUnit=pGroup->GetLeftChildUnit();
		do{
			AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunct,pUnit);
		}while(pUnit=pUnit->GetRightUnit());
	}else
	{
		//AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunct,pGroup);
	}

	return;

}




				}
			}
		}
	}
}