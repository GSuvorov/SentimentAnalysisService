#include "StdAfx.h"
#include ".\smdependentclause.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace IndexationExperts
			{


CSMDependentClause::CSMDependentClause(void)
{
	m_bInit = false;
	m_sDescription=_T(__FUNCTION__);
}

CSMDependentClause::~CSMDependentClause(void)
{
}

bool CSMDependentClause::AnalizeSentence()
{
	SS_TRY
	{
		UINT uiWordCount=0;
		wstring str;

		do{
			if(!TestNotQuestion()) break;

			uiWordCount++;

			if ((uiWordCount==1))
			{
				str=m_pCurrentUnit->GetWord();
				m_StringService.LowerSz((TCHAR*)str.c_str());

				if (str == wstring(_T("for")))
					AddCauseCategory();
				else if (m_pCurrentUnit->HasChilds())
				{
					IUnit * pUnit=m_pCurrentUnit->GetLeftChildUnit();
					do{
						str=pUnit->GetWord();
						m_StringService.LowerSz((TCHAR*)str.c_str());
						if (str == wstring(_T("for")))
						{							
							AddCauseCategory();
							break;
						}
					}while(pUnit=pUnit->GetRightUnit());

				}
			}

			if( !IsSyntax() )
			{
				{
					if(FindSingleConj(m_SinglePlace))
					{
						AddPlaceCategory();
						break;
					}
					else if (FindSingleConj(m_SingleTime))
					{
						AddTimeCategory();
						break;
					}
					else if (FindSingleConj(m_SingleManner))
					{
						AddMannerCategory();
						break;
					}
					else if (FindSingleConj(m_SingleCause))
					{
						if(uiWordCount>=2 && !m_LexemType.WordEqualsTo(_T("for")))
							AddCauseCategory();
						break;
					}
				}
				{
					/*if(FindMultiConj(m_MultiTime))
					{
						AddTimeCategory();
						break;
					}else*/ if(FindMultiConj(m_MultiManner))
					{
						AddMannerCategory();
						break;
					}
				}
				{
					if(FindMultiConj(m_MultiCause))
					{
						AddCauseCategory();
						break;
					}
				}
			}

		}while(GetNextUnit());
		return true;
	}
	SS_CATCH(L"")
	
}

bool CSMDependentClause::Init(IText * pIText)
{
	if(!CSMSentenceExpert::Init(pIText)) return false;

	if(!m_bInit)
	{
		m_bInit = true;

		list< pair<wstring,Types::PartOfSpeechTypes> > l_WordPOS;
		list< pair<wstring,Types::PartOfSpeechTypes> >::iterator itW;

		//list<SS::Dictionary::DATA_TYPE::Morpho_Info> oMorpho;
		//list<SS::Dictionary::DATA_TYPE::Morpho_Info>::iterator itM;
		list<SS::Dictionary::Types::TWordInfo> oMorpho;
		list<SS::Dictionary::Types::TWordInfo>::iterator itM;



		SS::Core::Features::CMorphoFeature oMorphoFearture;
		SS::Core::Features::CSemanticFeature oSemanticFeature;
		SS::Dictionary::Types::TDictionaryIndex sVal;

		oSemanticFeature.m_SemanticType=Values::TSemanticType.smtPlace;
		m_pIAMCSemantic->Code(&oSemanticFeature,&sVal);
		m_SemanticPlace.push_back(sVal);

		oSemanticFeature.m_SemanticType=Values::TSemanticType.smtTimeInterval;
		m_pIAMCSemantic->Code(&oSemanticFeature,&sVal);
		m_SemanticTime.push_back(sVal);

		if(m_SingleTime.empty())
		{
			//l_WordPOS.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("when"),Values::PartOfSpeechTypes.postAdverb ));
			l_WordPOS.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("когда"),Values::PartOfSpeechTypes.postAdverb ));
			//l_WordPOS.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("while"),Values::PartOfSpeechTypes.postOfficial ));
			//l_WordPOS.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("until"),Values::PartOfSpeechTypes.postOfficial ));
			//l_WordPOS.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("since"),Values::PartOfSpeechTypes.postOfficial ));
			for (itW=l_WordPOS.begin();itW!=l_WordPOS.end();itW++) 
			{
				oMorpho.clear();
				//GetWordMorphoInfo(itW->first.c_str(),oMorpho);
				GetWordMorphoInfo(itW->first.c_str(),oMorpho);
				for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
				{
					oMorphoFearture.Undefine();
					m_pIAMCMorpho->EnCode(
						itM->front().m_WordIndex.m_DictIndex.GetDictionaryNumber()/*itM->m_source_data.m_SourceFeatures.m_lIDSource>>24*/,
						itM->front().m_MorphoTypeInfo.m_MorphoInfo/*itM->m_source_data.m_SourceFeatures.m_shMorphoInfo*/,
						itM->front().m_SpecialMorpho/* itM->m_lPartOfSpeech*/,
						&oMorphoFearture);
					if(oMorphoFearture.m_PartOfSpeechTypes.IsContain(itW->second))
					{
						m_SingleTime.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex() /*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
						break;
					}
				}
			}
			l_WordPOS.clear();
		}
		if(m_SinglePlace.empty())
		{
			//l_WordPOS.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("where"),Values::PartOfSpeechTypes.postAdverb ));
			l_WordPOS.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("где"),Values::PartOfSpeechTypes.postAdverb ));
			for (itW=l_WordPOS.begin();itW!=l_WordPOS.end();itW++) 
			{
				oMorpho.clear();
				GetWordMorphoInfo(itW->first.c_str(),oMorpho);
				for (itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
				{
					oMorphoFearture.Undefine();
					m_pIAMCMorpho->EnCode(
						itM->front().m_WordIndex.m_DictIndex.GetDictionaryNumber()/*itM->m_source_data.m_SourceFeatures.m_lIDSource>>24*/,
						itM->front().m_MorphoTypeInfo.m_MorphoInfo/*itM->m_source_data.m_SourceFeatures.m_shMorphoInfo*/,
						itM->front().m_SpecialMorpho/* itM->m_lPartOfSpeech*/,
						&oMorphoFearture);
					if(oMorphoFearture.m_PartOfSpeechTypes.IsContain(itW->second))
					{
						m_SinglePlace.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
						break;
					}
				}
			}
			l_WordPOS.clear();
		}
		/*if(m_MultiTime.empty())
		{
			CSMDependentClause::TSourceList temp;
			UINT uiAsSource=0;

			oMorpho.clear();
			GetWordMorphoInfo(_T("as"),oMorpho);
			for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
			{
				oMorphoFearture.Undefine();
				m_pIAMCMorpho->EnCode(
						itM->front().m_WordIndex.m_DictIndex.GetDictionaryNumber(),
						itM->front().m_MorphoTypeInfo.m_MorphoInfo,
						itM->front().m_SpecialMorpho,
						&oMorphoFearture);
				if(oMorphoFearture.m_OfficialType.IsContain(Values::OfficialType.otPretext))
				{
					uiAsSource=itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex();
					break;
				}
			}
			if(!uiAsSource)return false;

			temp.push_back(uiAsSource);
			oMorpho.clear();
			GetWordMorphoInfo(_T("long"),oMorpho);
			for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
			{
				oMorphoFearture.Undefine();
				m_pIAMCMorpho->EnCode(
						itM->front().m_WordIndex.m_DictIndex.GetDictionaryNumber(),
						itM->front().m_MorphoTypeInfo.m_MorphoInfo,
						itM->front().m_SpecialMorpho,
						&oMorphoFearture);
				if(oMorphoFearture.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postAdverb))
				{
					temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex());
					break;
				}
			}
			temp.push_back(uiAsSource);
			m_MultiTime.push_back(temp);

			temp.clear();
			temp.push_back(uiAsSource);
			oMorpho.clear();
			GetWordMorphoInfo(_T("soon"),oMorpho);
			for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
			{
				oMorphoFearture.Undefine();
				m_pIAMCMorpho->EnCode(
						itM->front().m_WordIndex.m_DictIndex.GetDictionaryNumber(),
						itM->front().m_MorphoTypeInfo.m_MorphoInfo,
						itM->front().m_SpecialMorpho,
						&oMorphoFearture);
				if(oMorphoFearture.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postAdverb))
				{
					temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex());
					break;
				}
			}
			temp.push_back(uiAsSource);
			m_MultiTime.push_back(temp);
		}*/
		if(m_MultiManner.empty())
		{
			CSMDependentClause::TSourceList temp;
			UINT uiAsSource=0;

			oMorpho.clear();
			GetWordMorphoInfo(_T("as"),oMorpho);
			for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
			{
				oMorphoFearture.Undefine();
				m_pIAMCMorpho->EnCode(
						itM->front().m_WordIndex.m_DictIndex.GetDictionaryNumber()/*itM->m_source_data.m_SourceFeatures.m_lIDSource>>24*/,
						itM->front().m_MorphoTypeInfo.m_MorphoInfo/*itM->m_source_data.m_SourceFeatures.m_shMorphoInfo*/,
						itM->front().m_SpecialMorpho/* itM->m_lPartOfSpeech*/,
						&oMorphoFearture);
				if(oMorphoFearture.m_OfficialType.IsContain(Values::OfficialType.otPretext))
				{
					uiAsSource=itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/;
					break;
				}
			}
			if(!uiAsSource)return false;

			temp.push_back(uiAsSource);
			oMorpho.clear();
			GetWordMorphoInfo(_T("if"),oMorpho);
			for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
			{
				oMorphoFearture.Undefine();
				m_pIAMCMorpho->EnCode(
						itM->front().m_WordIndex.m_DictIndex.GetDictionaryNumber()/*itM->m_source_data.m_SourceFeatures.m_lIDSource>>24*/,
						itM->front().m_MorphoTypeInfo.m_MorphoInfo/*itM->m_source_data.m_SourceFeatures.m_shMorphoInfo*/,
						itM->front().m_SpecialMorpho/* itM->m_lPartOfSpeech*/,
						&oMorphoFearture);
				if(oMorphoFearture.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postOfficial))
				{
					temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
					break;
				}
			}

			m_MultiManner.push_back(temp);

			temp.clear();
			temp.push_back(uiAsSource);
			oMorpho.clear();
			GetWordMorphoInfo(_T("though"),oMorpho);
			for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
			{
				oMorphoFearture.Undefine();
				m_pIAMCMorpho->EnCode(
						itM->front().m_WordIndex.m_DictIndex.GetDictionaryNumber()/*itM->m_source_data.m_SourceFeatures.m_lIDSource>>24*/,
						itM->front().m_MorphoTypeInfo.m_MorphoInfo/*itM->m_source_data.m_SourceFeatures.m_shMorphoInfo*/,
						itM->front().m_SpecialMorpho/* itM->m_lPartOfSpeech*/,
						&oMorphoFearture);
				if(oMorphoFearture.m_PartOfSpeechTypes.IsContain(Values::PartOfSpeechTypes.postOfficial))
				{
					temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
					break;
				}
			}
			m_MultiManner.push_back(temp);

			oMorpho.clear();
			temp.clear();
			if(GetWordMorphoInfo(_T("с"),oMorpho))
			{
				itM=oMorpho.begin();
				temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
				oMorpho.clear();
				if(GetWordMorphoInfo(_T("помощью"),oMorpho))
				{
					itM=oMorpho.begin();
					temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
				}
			}
			if (temp.size()==2)
			{
				m_MultiManner.push_back(temp);
			}

		}
		if (m_SingleManner.empty())
		{
			l_WordPOS.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("путем"),Values::PartOfSpeechTypes.postOfficial ));
			for (itW=l_WordPOS.begin();itW!=l_WordPOS.end();itW++) 
			{
				oMorpho.clear();
				GetWordMorphoInfo(itW->first.c_str(),oMorpho);
				for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
				{
					m_SingleManner.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
					break;
				}
			}
			l_WordPOS.clear();
		}
		if (m_SingleCause.empty())
		{
			l_WordPOS.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("поскольку"),Values::PartOfSpeechTypes.postOfficial ));
			l_WordPOS.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("ибо"),Values::PartOfSpeechTypes.postOfficial ));
			l_WordPOS.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("because"),Values::PartOfSpeechTypes.postOfficial ));
			l_WordPOS.push_back(pair<wstring,Types::PartOfSpeechTypes>(_T("for"),Values::PartOfSpeechTypes.postOfficial ));
			for (itW=l_WordPOS.begin();itW!=l_WordPOS.end();itW++) 
			{
				oMorpho.clear();
				GetWordMorphoInfo(itW->first.c_str(),oMorpho);
				for ( itM=oMorpho.begin(); itM!=oMorpho.end() ; itM++)
				{
					oMorphoFearture.Undefine();
					m_pIAMCMorpho->EnCode(
						itM->front().m_WordIndex.m_DictIndex.GetDictionaryNumber()/*itM->m_source_data.m_SourceFeatures.m_lIDSource>>24*/,
						itM->front().m_MorphoTypeInfo.m_MorphoInfo/*itM->m_source_data.m_SourceFeatures.m_shMorphoInfo*/,
						itM->front().m_SpecialMorpho/* itM->m_lPartOfSpeech*/,
						&oMorphoFearture);
					if(oMorphoFearture.m_PartOfSpeechTypes.IsContain(itW->second))
					{
						m_SingleCause.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
						break;
					}
				}
			}
			l_WordPOS.clear();
		}
		if(m_MultiCause.empty())
		{
			CSMDependentClause::TSourceList temp;
			// in order that
			oMorpho.clear();
			if(GetWordMorphoInfo(_T("in"),oMorpho))
			{
				itM=oMorpho.begin();
				temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
				oMorpho.clear();
				if(GetWordMorphoInfo(_T("order"),oMorpho))
				{
					itM=oMorpho.begin();
					temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
					oMorpho.clear();
					if(GetWordMorphoInfo(_T("that"),oMorpho))
					{
						itM=oMorpho.begin();
						temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
					}
				}
			}
			if(temp.size()==3)
			{
				m_MultiCause.push_back(temp);
			}
			// so that
			oMorpho.clear();
			temp.clear();
			if(GetWordMorphoInfo(_T("so"),oMorpho))
			{
				itM=oMorpho.begin();
				temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
				oMorpho.clear();
				if(GetWordMorphoInfo(_T("that"),oMorpho))
				{
					itM=oMorpho.begin();
					temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
				}
			}
			if (temp.size()==2)
			{
				m_MultiCause.push_back(temp);
			}

			// потому что
			oMorpho.clear();
			temp.clear();
			if(GetWordMorphoInfo(_T("потому"),oMorpho))
			{
				itM=oMorpho.begin();
				temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
				oMorpho.clear();
				if(GetWordMorphoInfo(_T("что"),oMorpho))
				{
					itM=oMorpho.begin();
					temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
				}
			}
			if (temp.size()==2)
			{
				m_MultiCause.push_back(temp);
			}
			// так как
			oMorpho.clear();
			temp.clear();
			if(GetWordMorphoInfo(_T("так"),oMorpho))
			{
				itM=oMorpho.begin();
				temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
				oMorpho.clear();
				if(GetWordMorphoInfo(_T("как"),oMorpho))
				{
					itM=oMorpho.begin();
					temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
				}
			}
			if (temp.size()==2)
			{
				m_MultiCause.push_back(temp);
			}
			// из-за
			temp.clear();
			oMorpho.clear();
			if(GetWordMorphoInfo(_T("из"),oMorpho))
			{
				itM=oMorpho.begin();
				temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
				oMorpho.clear();
				if(GetWordMorphoInfo(_T("-"),oMorpho))
				{
					itM=oMorpho.begin();
					temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
					oMorpho.clear();
					if(GetWordMorphoInfo(_T("за"),oMorpho))
					{
						itM=oMorpho.begin();
						temp.push_back(itM->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/*itM->m_source_data.m_SourceFeatures.m_lIDSource*/);
					}
				}
			}
			if(temp.size()==3)
			{
				m_MultiCause.push_back(temp);
			}

		}
	}
	return true;
}


bool CSMDependentClause::TestNotQuestion()
{
	Mem();
	if(m_pCurrentUnit=m_pCurrentSentence->GetLastUnit())
	{
		UINT uiDepth=0;
		do
		{
			if(m_LexemType.IsSymbol() && m_LexemType.SymbolEqualsTo(_T("?")))
			{
				GotoMem();
				return false;
			}
		}while( uiDepth++<2 && GetPrevUnit());
	}
	GotoMem();
	return true;
}
bool CSMDependentClause::FindSingleConj(CSMDependentClause::TSourceList & oSingle)
{
	CSMDependentClause::TSourceList::iterator i;

	if(!m_pCurrentUnit->GetFirstIndex()) return false;

	//TMorphoInfoCollection::iterator itM=m_Unit->m_MorphoInfo.m_MorphoInfoCollection.begin();
	//UINT uiSource=m_pCurrentUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst();
	for (i=oSingle.begin(); i!=oSingle.end(); i++) 
	{
		if(IsIndexExist(m_pCurrentUnit->GetFirstIndex(), *i))
			return true;
	}
	return false;
}
bool CSMDependentClause::IsIndexExist(IIndex * pIndex, UINT uiSourceIndex)
{
	if(pIndex==NULL) return false;

	int iSub=0;

	UINT m_uiTest;

	m_uiTest = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex() & 0x7FFFFFFF;

	if(uiSourceIndex==m_uiTest)
		return true;
	iSub=uiSourceIndex-m_uiTest/*pIndex->GetDictionaryIndex()->GetFirst()*/;
	if(iSub>=-5 && iSub<=1)
	{
		//itM++;
		if(m_pCurrentUnit)
		{
			if (m_pCurrentUnit->GetFirstIndex())
			{
				if(pIndex=m_pCurrentUnit->GetNextIndex())
				{
					do{
						m_uiTest = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex() & 0x7FFFFFFF;
						if(uiSourceIndex==m_uiTest/*pIndex->GetDictionaryIndex()->GetFirst()*/)
						{
							return true;
						}
					}while(pIndex=m_pCurrentUnit->GetNextIndex());
				}
			}

		}
		/*pIndex=m_pCurrentUnit->GetNextIndex();

		if(pIndex==NULL) return false;

		if(uiSourceIndex==pIndex->GetDictionaryIndex()->GetFirst())
		return true;*/
	}
	return false;
}

void CSMDependentClause::AddPlaceCategory()
{
	//m_Unit->m_MorphoInfo.m_SynonimsCollection.insert((SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24) + 
	//	Values::SyntaxCategories.scAjunctPlaceFixedPoint.GetSingleValue());
	//m_Unit->m_MorphoInfo.m_SynonimsCollection.insert(m_SemanticPlace.begin(), m_SemanticPlace.end());
	AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctPlaceFixedPoint);
	SetSemanticIndices(m_SemanticPlace);
	//	m_Unit->m_MorphoInfo.InsertIndexSemantic(m_Unit->m_MorphoInfo.m_MorphoInfoCollection.front().m_ulSourceIndex,m_SemanticPlace);
}
void CSMDependentClause::AddTimeCategory()
{
	//m_Unit->m_MorphoInfo.m_SynonimsCollection.insert((SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24) + 
	//	Values::SyntaxCategories.scAjunctTimeFixedPoint.GetSingleValue());
	//m_Unit->m_MorphoInfo.m_SynonimsCollection.insert(m_SemanticTime.begin(), m_SemanticTime.end());
	AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctTimeFixedPoint);
	SetSemanticIndices(m_SemanticTime);
}
void CSMDependentClause::AddMannerCategory()
{
	//m_Unit->m_MorphoInfo.m_SynonimsCollection.insert((SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24) + 
	//	Values::SyntaxCategories.scAjunctManner.GetSingleValue());
	AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctManner);
}
void CSMDependentClause::AddCauseCategory()
{
	//m_Unit->m_MorphoInfo.m_SynonimsCollection.insert((SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24) + 
	//	Values::SyntaxCategories.scAjunctCause.GetSingleValue());
	AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctCause);
}
bool CSMDependentClause::FindMultiConj(list<TSourceList> & oMulti)
{
	CSMDependentClause::TSourceList::iterator i;
	if(!m_pCurrentUnit->GetFirstIndex()) return false;
	Mem();
	IIndex * pIndex;
	list<CSMDependentClause::TSourceList>::iterator iT=oMulti.begin();

	if (m_pCurrentUnit->HasChilds())
	{
		for(; iT!=oMulti.end(); iT++)
		{
			i=iT->begin();
			//itM=m_Unit->m_MorphoInfo.m_MorphoInfoCollection.begin();
			IUnit * pUnit=m_pCurrentUnit->GetLeftChildUnit();
			wstring str = pUnit->GetWord();
			if (pIndex=pUnit->GetFirstIndex())
			{
				Mem1();
				m_pCurrentUnit = pUnit;
				if(IsIndexExist(pIndex,*i))
				{
					i++;
					while(i!=iT->end())
					{
						if(!(pUnit=pUnit->GetRightUnit()))
						{
							break;
						}
						//itM=m_Unit->m_MorphoInfo.m_MorphoInfoCollection.begin();
						pIndex=pUnit->GetFirstIndex();
						Mem2();
						m_pCurrentUnit = pUnit;
						if(!IsIndexExist(pIndex,*i))
						{
							GotoMem2();
							break;
						}
						GotoMem2();
						i++;
					}
					if(i==iT->end()) return true;
				}
				GotoMem1();

			}
		}
		return false;
	}

	for(; iT!=oMulti.end(); iT++)
	{
		i=iT->begin();
		//itM=m_Unit->m_MorphoInfo.m_MorphoInfoCollection.begin();
		pIndex=m_pCurrentUnit->GetFirstIndex();
		if(IsIndexExist(pIndex,*i))
		{
			i++;
			while(i!=iT->end())
			{
				if(!GetNextUnit())
				{
					GotoMem();
					break;
				}
				//itM=m_Unit->m_MorphoInfo.m_MorphoInfoCollection.begin();
				pIndex=m_pCurrentUnit->GetFirstIndex();
				if(!IsIndexExist(pIndex,*i))
				{
					GotoMem();
					break;
				}
				i++;
			}
			if(i==iT->end()) return true;
		}
	}

	return false;	
}

			}
		}
	}
}