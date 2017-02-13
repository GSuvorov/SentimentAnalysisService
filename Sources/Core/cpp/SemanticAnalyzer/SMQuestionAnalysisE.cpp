#include "StdAfx.h"
#include ".\smquestionanalysise.h"
namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{
				namespace English
				{


CSMQuestionAnalysisE::CSMQuestionAnalysisE(void)
{
	m_bInit = false;
	m_uiToBeSourceIndex=0;
//	m_MoAmbResolver.Init(this);
	m_AnalyseLanguage=SS::Core::Types::ealEnglish;
	m_sDescription=_T(__FUNCTION__);
}

CSMQuestionAnalysisE::~CSMQuestionAnalysisE(void)
{
}

bool CSMQuestionAnalysisE::AnalizeSentence()
{
	SS_TRY
	{
		StrictQuery();
		return true;

		EQuestionType eQType;
		ProcessSentence();
	//	m_MoAmbResolver.Analyse();
		eQType=DetectQuestionType();
		
		if(eQType==eqtNon) return true;

		MarkUnits(eQType);
		FillGroups();



		if(eQType!=eqtEWho )
			CorrectPassiveEnglish();

		return true;
	}
	SS_CATCH(L"")
}
bool CSMQuestionAnalysisE::Init(IText * pIText)
{
	if(!CSMQuestionAnalysis::Init(pIText)) return false;

	if(!m_bInit)
	{
		m_bInit = true;

		list<SS::Dictionary::Types::TWordInfo> oMorpho;
		list<SS::Dictionary::Types::TWordInfo>::iterator itM;
		if(m_uiToBeSourceIndex==0)
		{
			GetWordMorphoInfo(_T("be"),oMorpho);
			for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
			{
				m_MorphoFeature.Undefine();
				m_pIAMCMorpho->EnCode(
					itM->front().m_WordIndex.m_DictIndex.GetDictionaryNumber(),
					itM->front().m_MorphoTypeInfo.m_MorphoInfo,
					itM->front().m_SpecialMorpho,
					&m_MorphoFeature);
				if ( m_MorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb) && m_MorphoFeature.m_VerbType.Equal(Values::VerbType.vrbtMain) 
					)
				{
					m_uiToBeSourceIndex = itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex();
					break;
				}
			}
			if ( m_uiToBeSourceIndex==0 ) return false;
		}
		if (m_StringQTypeTableEng.empty())
		{
			m_StringQTypeTableEng.insert(map<wstring,EQuestionType>::value_type(wstring(_T("who")),eqtEWho));
			m_StringQTypeTableEng.insert(map<wstring,EQuestionType>::value_type(wstring(_T("when")),eqtEWhen));
			m_StringQTypeTableEng.insert(map<wstring,EQuestionType>::value_type(wstring(_T("where")),eqtEWhere));
			m_StringQTypeTableEng.insert(map<wstring,EQuestionType>::value_type(wstring(_T("what")),eqtEWhat));
			m_StringQTypeTableEng.insert(map<wstring,EQuestionType>::value_type(wstring(_T("which")),eqtEWhat));
			m_StringQTypeTableEng.insert(map<wstring,EQuestionType>::value_type(wstring(_T("how")),eqtEHow));
			m_StringQTypeTableEng.insert(map<wstring,EQuestionType>::value_type(wstring(_T("whom")),eqtEWho));
		}
	}

	return true;
}

EQuestionType CSMQuestionAnalysisE::DetectQuestionType()
{
	UINT uiDeep=0;
	EQuestionType eQuestionType=eqtNon;

	m_DInfo.Init();

	wstring sz_Word;

	SMorphoDescription * pDescription;

	do
	{
		if(!SpecialTypeEquals(ustUndefined) || IsSyntax())
			continue;
		//pDescription->_Unit->GetWord(sz_Word.c_str());
		sz_Word=m_pCurrentUnit->GetWord();
		m_StringService.LowerSz((TCHAR*)sz_Word.c_str());
		
		pDescription=GetUnitDescription();
		if ( m_StringService.StrEqual(sz_Word.c_str(),_T("when")) )
		{
			pDescription->_ePOS=eposQuestion;
			eQuestionType=eqtEWhen;
			break;
		}else if (  m_StringService.StrEqual(sz_Word.c_str(),_T("where") )  )
		{
			pDescription->_ePOS=eposQuestion;
			eQuestionType= eqtEWhere;
			break;
		}else if ( m_StringService.StrEqual(sz_Word.c_str(),_T("what")) || m_StringService.StrEqual(sz_Word.c_str(),_T("which"))|| m_StringService.StrEqual(sz_Word.c_str(),_T("whose")) )
		{
			pDescription->_ePOS=eposQuestion;
			eQuestionType= eqtEWhat;
			break;
		}else if ( m_StringService.StrEqual(sz_Word.c_str(),_T("how")) || m_StringService.StrEqual(sz_Word.c_str(),_T("why")) )
		{
			pDescription->_ePOS=eposQuestion;
			eQuestionType= eqtEHow;
			
			if (GetNextUnit()) 
			{
				sz_Word=m_pCurrentUnit->GetWord();
				m_StringService.LowerSz((TCHAR*)sz_Word.c_str());
				if (m_StringService.StrEqual(sz_Word.c_str(),_T("long") ))
				{
					pDescription=GetUnitDescription();
					pDescription->_ePOS=eposLong;
				}
			}
			break;
		}else if (  m_StringService.StrEqual(sz_Word.c_str(),_T("who"))||m_StringService.StrEqual(sz_Word.c_str(),_T("whom"))  )
		{
			pDescription->_ePOS=eposQuestion;
			eQuestionType= eqtEWho;
			break;
		}else if (  m_StringService.StrEqual(sz_Word.c_str(),_T("why"))  )
		{
			pDescription->_ePOS=eposQuestion;
			eQuestionType= eqtEWhy;
			break;
		}
	}while(GetNextUnit() && uiDeep++<3);

	if(eQuestionType== eqtNon ) return eQuestionType;

	RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin);
	if(eQuestionType== eqtEWhere || eQuestionType== eqtEWhen)
		RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scObject);

}

void CSMQuestionAnalysisE::MarkUnits(EQuestionType eQuestionType)
{
	//find Aux
	SMorphoDescription * pDescription;
	do
	{
		pDescription=GetUnitDescription();
		if( pDescription->_oMorphoFeature.m_VerbType.IsContain(Values::VerbType.vrbtAux)||
			pDescription->_oMorphoFeature.m_VerbType.IsContain(Values::VerbType.vrbtMod) )
		{
			//pDescription->_Unit->GetWord(sz_Word);
			pDescription->_ePOS=eposAux;
			SetSpecialType(ustAux);
			break;
		}
	}while(GetNextUnit());

	bool bToBe=false;

	IUnit * pAux;

	if ( m_pCurrentUnit )
	{	
		//pDescription=pDescription; pDescription++;
		pAux=m_pCurrentUnit;
		GetNextUnit();
		bToBe=IsBe(pAux);
		if ( bToBe ) 
		{ 
			m_DInfo._pToBe=pAux;
			//if (m_DInfo._pToBe->m_EntityInfo.m_pExtra )m_DInfo._pToBe->m_EntityInfo.m_pExtra->m_s_SemanticTypes.clear();
			pDescription=GetUnitDescription();
			if(pDescription->_oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple)&& 
				pDescription->_oMorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPast))
			{
				pDescription->_ePOS=eposVerbMain;
				m_DInfo._pVerb=m_pCurrentUnit;

				return;
			}
		}
		else m_DInfo._pAux=pAux;

	}else GetFirstUnit();

	IUnit * pMem=NULL;
	do{
		if( ( SpecialTypeEquals(ustUndefined) ||SpecialTypeEquals(ustWordCombination))&& pDescription->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb) )
		{
			if(m_DInfo._pToBe)
			{
				if(!pDescription->_oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple) ||
					!pDescription->_oMorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPast)
					)
					continue;
			}

			if ( pMem ) GetUnitDescription(pMem)->_ePOS=eposNon;
			pMem=m_pCurrentUnit;
			pDescription->_ePOS=eposVerbMain;
			m_DInfo._pVerb=m_pCurrentUnit;
			if (SpecialTypeEquals(ustWordCombination))
			{
				//if (pDescription->_Unit->m_SyntaxUnitType==usntDateSyntax)
					break;
			}
			if(eQuestionType==eqtEWho ) break;
		}
	}while(GetNextUnit());

	//if ( m_DInfo._pVerb && IsBe(m_DInfo._pVerb)) m_DInfo._pVerb->m_MorphoInfo.IsContainFeature(efRepresentBecome);
	//ищем означенный предлоги, в 3 местах - в начале вопроса , в конце , после глагола ( если есть ) 
	pMem=m_pCurrentUnit;
	if( IsPrepositionSyntax(GetFirstUnit()) )//m_pMorphoService->GetPrepositionSyntax(tDescription.front()._Unit->GetWord(sz_Word),NULL) )
	{
		GetUnitDescription()->_ePOS=eposPrep;
		m_DInfo._pPrep=m_pCurrentUnit;
	}else 	if( IsPrepositionSyntax(GetLastUnit()) )
	{
		GetUnitDescription()->_ePOS=eposPrep;
		m_DInfo._pPrep=m_pCurrentUnit;
	}else{
		if (pMem)
		{
			m_pCurrentUnit=pMem;

			if (GetNextUnit()) 
			{
				if(!IsSyntax() &&
					IsPrepositionSyntax() )
				{
					GetUnitDescription()->_ePOS=eposPrep;
					m_DInfo._pPrep=m_pCurrentUnit;
				}

			}
		}
	}

}

bool CSMQuestionAnalysisE::IsBe(IUnit * pUnit)
{
	if(!pUnit)pUnit=m_pCurrentUnit;
	if(!pUnit) return false;
	if(FindIndex(m_uiToBeSourceIndex)) return true;
	return false;
}


void CSMQuestionAnalysisE::FillGroups()
{
	if (!m_DInfo._pAux && !m_DInfo._pToBe && !m_DInfo._pVerb) return ;

	wstring sz_Word;
	if(!GetFirstUnit()) return;

	TBoxUnits l_Units;
	SMorphoDescription* pDescription;
	IUnit* pHead;
	IUnit* pVerb;

	do
	{
		pDescription=GetUnitDescription();
		if( pDescription->_ePOS!=eposAux && pDescription->_ePOS!=eposToBe && pDescription->_ePOS!=eposVerbMain  )
		{
			l_Units.push_back(m_pCurrentUnit);
		}
		else break;
	}
	while ( GetNextUnit() );
	//if(l_Question.size()>1 && l_Question.back()->m_MorphoInfo.IsContainFeature(efPretext))
	//{
	//	l_Question.pop_back();
	//}
	pVerb=m_pCurrentUnit;

	if(l_Units.size()>1 && IsContainFeature(efcPretext,l_Units.back()))
	{
		l_Units.pop_back();
	}
	//SetSyntaxInfo(l_Question.front(),l_Question,usntQuestionGroup);
	pHead=JoinUpUnits(l_Units);
	
	//SetSyntaxType(usntQuestionGroup);

	while(m_pCurrentUnit && !!IsSyntax())GetNextUnit();
	
	// first entity - 
	//	while(it!=tDescription.end() && it->_ePOS==eposDet) it++;
	pDescription=GetUnitDescription();

	while (GetNextUnit() && GetUnitDescription()->_ePOS!=eposVerbMain && !IsSyntax() ) 
	{
		l_Units.push_back(m_pCurrentUnit);
	}
	
	pVerb=m_pCurrentUnit;

	while(m_pCurrentUnit && !!IsSyntax())	GetNextUnit();
	// something in entity 
	if (l_Units.empty()==false)
	{
		if(l_Units.size()==1 && IsContainFeature(efcPretext,l_Units.front()))
		{
			l_Units.clear();
		}else {
			//SetSyntaxInfo(l_Entity1.back(),l_Entity1,usntEntity1);
			//l_Entity1.back()->m_SyntaxInfo.SetSyntaxCategory(Values::SyntaxCategories.scMainEntity);
			pHead=JoinUpUnits(l_Units);
//			SetSyntaxType(usntEntity1,pHead);
			SetSyntaxCategory(pHead,Values::SyntaxCategories.scMainEntity);
		}
	}
	if (pVerb && GetUnitDescription(pVerb)->_ePOS==eposVerbMain) 
	{// 1 verb in query - and it is non aux/be 
//		SetSyntaxType(usntVerb);
		SetSyntaxCategory(pVerb,Values::SyntaxCategories.scPredicate);
	}
	// some 
	while(m_pCurrentUnit && !!IsSyntax())	GetNextUnit();
	//filling entity2 - if it exist
	while ( GetNextUnit() &&
		!IsSyntax()
		) 
	{
		l_Units.push_back(m_pCurrentUnit);
	}

	if (l_Units.empty()==false)
	{
		pHead=JoinUpUnits(l_Units);
//		SetSyntaxType(usntEntity2,pHead);
		SetSyntaxCategory(pHead,Values::SyntaxCategories.scAdditionalEntity);
		

	}

}


bool CSMQuestionAnalysisE::FindMeasurePrep(IUnit * pUnits,SS::Core::Features::CSemanticFeature& oSemanticFeature,list<long>& l_SemValues)
{
	if(!pUnits || !pUnits->HasChilds()) return false;

	IUnit * pUnit;

	TBoxUnits l_Rem;
	TBoxUnits::iterator it;
	IUnit * pSem=NULL;
	if (pUnit=pUnits->GetLeftChildUnit())
	{
		do{
			l_Rem.push_back(pUnit);
			if( IsContainFeature(efcSemanticCommon,pUnit) && CollectSemanticFeature(oSemanticFeature,pUnit))
			{
				if(oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure))
				{
					pSem=pUnit;
					pUnit=pUnit->GetRightUnit();
					if(pUnit && IsContainFeature(efcPretext,pUnit)||
						!pUnit && pSem->GetRightUnit() && /*!SyntaxTypeEquals(usntUndefined,pSem->GetRightUnit())*/IsSyntax(pSem->GetRightUnit()) )
						break;
					pSem=NULL;
					if(pUnit)pUnit=pUnit->GetLeftUnit();
				}
				oSemanticFeature.Undefine();
			}
		}while((pUnit=pUnit->GetRightUnit()));
	}
	if(oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtMeasure) && pSem)
	{
		for(it=l_Rem.begin(); it!=l_Rem.end(); it++)
		{
			//(*it)->m_SyntaxUnitType=usntUndefined;
			(*it)->Pop();
			m_pCurrentSentence->AddUnit(*it);
		}

		SetSingleSyntax(pSem);
		return true;
	}
	return false;
}

bool CSMQuestionAnalysisE::CheckWhoCategory(IUnit * pHead, IUnit * pUnits,bool & bNameDef)
{
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
			if (!SpecialTypeEquals(ustCitation)) continue;
			if(SpecialTypeEquals(ustComplexProperName) && IsContainFeature(efEDNounNominative,pUnit))
			{
				if (IsContainFeature(efcSemanticName))
				{
					AddSyntaxCategory(pHead,Values::SyntaxCategories.scNameDefinition);
					bNameDef=true;					
				}
				else 
				{
					AddSyntaxCategory(pHead,Values::SyntaxCategories.scWhoDefinition);
					bNameDef=false;
				}				
				return true;
			}
			oF.Undefine();
			if( CollectSemanticFeature(oF,pUnit)&& 
				(IsContainFeature(efEDNounNominative) )
				)
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
			if(IsContainFeature(efEDNounNominative,pUnit))
			{
				AddSyntaxCategory(pHead,Values::SyntaxCategories.scWhoDefinition);				
				bNameDef=false;
				return true;
			}


		}while(bChilds && (pUnit=pUnit->GetRightUnit()));
	}

	SetSyntaxCategory(pHead,Values::SyntaxCategories.scAdditionalEntity);
	AddAllPersonSemantic(pHead);
	return false;
}

void CSMQuestionAnalysisE::CorrectPassiveEnglish()
{
	if(!m_DInfo._pToBe || !m_DInfo._pVerb) return;
	SS::Core::Features::CMorphoFeature oMorphoFeature;

	//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,m_DInfo._pVerb->m_MorphoInfo,oMorphoFeature);
	SMorphoDescription * pDescription=GetUnitDescription(m_DInfo._pVerb);
	if(pDescription->_oMorphoFeature.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postVerb)&&
		pDescription->_oMorphoFeature.m_VerbForm.IsContain(Values::VerbForm.vfParticiple)&&
		pDescription->_oMorphoFeature.m_TimeType.IsContain(Values::TimeType.tmtPast)
		)
	{
		if ( GetFirstUnit() )
		{	
			while(!SpecialTypeEquals(ustAux) && GetNextUnit())
			{
			};
			if(SpecialTypeEquals(ustAux))
			{
				while(/*!SyntaxTypeEquals(usntVerb) &&*/ GetNextUnit())
				{
					RemoveSyntaxCategoryIndex(Values::SyntaxCategories.scSubjectFin);
				}
			}
		}

	}
}


				}
			}
		}
	}
}