#include "StdAfx.h"

#include ".\smexpert.h"
//#include "../ASSInterface/GUID.h"

#include ".\smmanager.h"


namespace SS
{
namespace Semantic
{

CSMExpert::CSMExpert(void) 
: m_pCurrentSentence(NULL)
, m_pCurrentUnit(NULL)
, m_pIText(NULL)
, m_pCurrentParagraph(NULL)
, m_pIAMCMorpho(NULL)
, m_pIAMCSemantic(NULL)
, m_bInit(false)
, m_AnalyseLanguage(SS::Core::Types::ealUndefined)
{
	SS_TRY
	{
		m_LexemType.Init(&m_pCurrentUnit);		
	}
	SS_CATCH(L"")
}

CSMExpert::~CSMExpert(void)
{
}

bool CSMExpert::Init(IText * pIText)
{
	SS_TRY
	{
		m_pIText=pIText;
		m_pCurrentParagraph=NULL;
		m_pCurrentSentence=NULL;
		m_pCurrentUnit=NULL;
		m_pCurrentIndex=NULL;

		if (m_pSMManager)
		{
			m_pIAMCMorpho=m_pSMManager->GetAMConverterMorpho();
			m_pIAMCSemantic=m_pSMManager->GetAMConverterSemantic();
			m_pOfficialVirtualSyntax=m_pSMManager->GetOfficialVirtualSyntax();
			m_pIBlackBoxFactory=m_pSMManager->GetBlackBoxTextStorage();
		}//else
			//SS_THROW(_T("SM Manager not set!"));
		
		if (!m_pIAMCMorpho || !m_pIAMCSemantic) return false;
		
		return true;
	}
	SS_CATCH(L"")

}
void CSMExpert::SetSMManager(CSMManager * pManager)
{
	SS_TRY
	{
		m_pSMManager=pManager;
	}
	SS_CATCH(L"")
	
}

bool CSMExpert::GetFirstParagraph()
{
	SS_TRY
	{
		if (!m_pIText) return false;
		m_pCurrentParagraph=m_pIText->GetFirstParagraph();
		return m_pCurrentParagraph!=NULL;
	}
	SS_CATCH(L"")
}
bool CSMExpert::GetNextParagraph()
{
	SS_TRY
	{
		if (!m_pCurrentParagraph) return false;
		m_pCurrentParagraph=m_pCurrentParagraph->GetNextParagraph();
		return m_pCurrentParagraph!=NULL;
	}
	SS_CATCH(L"")

}
bool CSMExpert::GetFirstSentence()
{
	SS_TRY
	{
		if (!m_pCurrentParagraph) return false;
		m_pCurrentSentence=m_pCurrentParagraph->GetFirstSentence();
		return m_pCurrentSentence!=NULL;
	}
	SS_CATCH(L"")

}
bool CSMExpert::GetNextSentence()
{
	SS_TRY
	{
		if (!m_pCurrentSentence) return false;
		m_pCurrentSentence=m_pCurrentSentence->GetNextSentence();
		return m_pCurrentSentence!=NULL;
	}
	SS_CATCH(L"")

}
IUnit * CSMExpert::GetFirstUnit()
{
	SS_TRY
	{
		if (!m_pCurrentSentence) return NULL;
		m_pCurrentUnit=m_pCurrentSentence->GetFirstUnit();
		return m_pCurrentUnit;
	}
	SS_CATCH(L"")
}
IUnit * CSMExpert::GetLastUnit()
{
	SS_TRY
	{
		if (!m_pCurrentSentence) return NULL;
		m_pCurrentUnit=m_pCurrentSentence->GetLastUnit();
		return m_pCurrentUnit;
	}
	SS_CATCH(L"")
}
IUnit * CSMExpert::GetNextUnit()
{
	SS_TRY
	{
		if (!m_pCurrentUnit) return NULL;
		m_pCurrentUnit=m_pCurrentUnit->GetRightUnit();
		return m_pCurrentUnit;
	}
	SS_CATCH(L"")
}

IUnit * CSMExpert::GetNextUnitWithChild()
{
	SS_TRY
	{
		if (!m_pCurrentUnit) return NULL;

		IUnit * m_pRememberUnit = m_pCurrentUnit;
		m_pCurrentUnit = m_pCurrentUnit->GetRightUnit();

		if((m_pCurrentUnit) && (m_pCurrentUnit->HasChilds()))
		{
			m_pCurrentUnit = m_pCurrentUnit->GetLeftChildUnit();
			if(!m_pCurrentUnit)
				GetNextUnitWithChild();
		}

		if(!m_pCurrentUnit )
		{
			m_pCurrentUnit = m_pRememberUnit->GetParentUnit();
			GetNextUnitWithChild();
		}
		
		return m_pCurrentUnit;
	}
	SS_CATCH(L"")
}

IUnit * CSMExpert::GetPrevUnit()
{
	SS_TRY
	{
		if (!m_pCurrentUnit) return NULL;
		m_pCurrentUnit=m_pCurrentUnit->GetLeftUnit();
		return m_pCurrentUnit;
	}
	SS_CATCH(L"")
}
IIndex * CSMExpert::GetFirstIndex(IUnit * pUnit)
{
	SS_TRY
	{
		if (pUnit==NULL) pUnit=m_pCurrentUnit;
		if (pUnit)
		{
			m_pCurrentIndex=pUnit->GetFirstIndex();
			return m_pCurrentIndex;
		}
		return NULL;
	}
	SS_CATCH(L"")
}
IIndex * CSMExpert::GetNextIndex(IUnit * pUnit)
{
	SS_TRY
	{
		if (pUnit==NULL) 
			pUnit=m_pCurrentUnit;
		if (pUnit)
		{
			m_pCurrentIndex=pUnit->GetNextIndex();
			return m_pCurrentIndex;
		}
		return NULL;
	}
	SS_CATCH(L"")
}

void CSMExpert::AddFeature(UINT uiFeature, IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) pUnit=m_pCurrentUnit;
		if(!pUnit) return;
		pUnit->GetInfo()->_uiFeature|=uiFeature;
	}
	SS_CATCH(L"")
}
void CSMExpert::AddFeature(CommonFeature eFeature,IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) pUnit=m_pCurrentUnit;
		if(!pUnit) return;
		pUnit->GetInfo()->_shCommonFeature|=eFeature;
	}
	SS_CATCH(L"")
}
void CSMExpert::AddFeature(ERusFeature eFeature,IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) pUnit=m_pCurrentUnit;
		if(!pUnit) return;
		pUnit->GetInfo()->_uiFeature|=eFeature;
	}
	SS_CATCH(L"")
}
void CSMExpert::AddFeature(EEngFeature eFeature,IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) pUnit=m_pCurrentUnit;
		if(!pUnit) return;
		pUnit->GetInfo()->_uiFeature|=eFeature;
	}
	SS_CATCH(L"")
}

bool CSMExpert::IsContainFeature(UINT uiFeature, IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) pUnit=m_pCurrentUnit;
		if(!pUnit) return false;
		return ((pUnit->GetInfo()->_uiFeature & uiFeature)>0);
	}
	SS_CATCH(L"")
}
bool CSMExpert::IsContainFeature(CommonFeature eFeature,IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) pUnit=m_pCurrentUnit;
		if(!pUnit) return false;
		return ((pUnit->GetInfo()->_shCommonFeature & eFeature)>0);
	}
	SS_CATCH(L"")
}
bool CSMExpert::IsContainFeature(ERusFeature eFeature,IUnit * pUnit)
{
	SS_TRY
	{
		return IsContainFeature((UINT)eFeature,pUnit);
	}
	SS_CATCH(L"")
	
}
bool CSMExpert::IsContainFeature(EEngFeature eFeature,IUnit * pUnit)
{
	SS_TRY
	{
		return IsContainFeature((UINT)eFeature,pUnit);
	}
	SS_CATCH(L"")	
}
void CSMExpert::RemoveFeature(CommonFeature eFeature,IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) pUnit=m_pCurrentUnit;
		if(!pUnit) return ;
		pUnit->GetInfo()->_shCommonFeature &= (~eFeature);
	}
	SS_CATCH(L"")
}

void CSMExpert::SetSpecialType(TSpecialUnitType eSpecialType, IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) pUnit=m_pCurrentUnit;
		if(!pUnit) return ;
		//pUnit->GetInfo()->_SpecialType=eSpecialType;			
	}
	SS_CATCH(L"")
}
bool CSMExpert::SpecialTypeEquals(TSpecialUnitType eSpecialType, IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) pUnit=m_pCurrentUnit;
		if(!pUnit) return false;
		return false;
	}
	SS_CATCH(L"")
}

void CSMExpert::SetSyntax(IUnit * pUnit, bool bFlag)
{
	SS_TRY
	{
		if(!pUnit) pUnit=m_pCurrentUnit;
		if(!pUnit) return ;
		m_pCurrentUnit->GetInfo()->_bIsSyntax=bFlag;
	}
	SS_CATCH(L"")
}
bool CSMExpert::IsSyntax(IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) pUnit=m_pCurrentUnit;
		if(!pUnit) return false;
		return m_pCurrentUnit->GetInfo()->_bIsSyntax;
	}
	SS_CATCH(L"")
}


bool CSMExpert::IsPrepositionSyntax(IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) pUnit=m_pCurrentUnit;
		if(!pUnit) return false;
		if(!pUnit->GetWord()) return false;
		wstring strW=pUnit->GetWord();
		m_StringService.LowerSz((TCHAR*)strW.c_str());
		//char buf[50];
		setlocale(LC_ALL, "");
		/*wcstombs(buf,strW.c_str(),50);
		string str(buf);*/
		return m_pOfficialVirtualSyntax->IsPrepositionSyntaxExist(&strW);
	}
	SS_CATCH(L"")
}

void CSMExpert::CollectMorphoFeature(SS::Core::Features::CMorphoFeature & oMorphoFeature, IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) pUnit=m_pCurrentUnit;
		if(!pUnit) return ;
		IIndex * pIndex=pUnit->GetFirstIndex();
		IFeature * pFeature;
		if(!pIndex) return;
		do{
			if(!(pFeature=pIndex->GetFirstFeature()) ||
				!pIndex->GetDictionaryIndex()) continue;
			do{
				m_MorphoFeatureCollect.Undefine();
				m_pIAMCMorpho->EnCode(/*(*/pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()/*GetFirst())>>24*/,pFeature->GetMorphoInfo(),pFeature->GetPartOfSpeech(),&m_MorphoFeatureCollect);
				oMorphoFeature.AssignOR(&m_MorphoFeatureCollect);
			}while((pFeature=pIndex->GetNextFeature()));
		}while(pIndex=pUnit->GetNextIndex());
	}
	SS_CATCH(L"")
}

bool CSMExpert::GetWordSourceIndices(const TCHAR * pWord,list<UINT> & l_Sources)
{
	SS_TRY
	{
		if(!pWord) return false;
		/*char buf[50];
		setlocale(LC_ALL, "");
		wcstombs(buf,pWord,50);
		string str(buf);*/
		list<SS::Dictionary::Types::TWordInfo> l_MorphoInfo;
		list<SS::Dictionary::Types::TWordInfo>::iterator i;
		if(
			m_pSMManager->GetDictionaryInterface()->GetWordInfo(pWord, &l_MorphoInfo, /*in*/SS::Dictionary::Types::efzBaseDictionary)
			/*GetByForm(&str,&l_MorphoInfo,SS::Dictionary::DATA_TYPE::efzBaseDictionary)*/)
		{
			for (i=l_MorphoInfo.begin();i!=l_MorphoInfo.end(); i++) 
			{
				l_Sources.push_back(
					i->front().m_WordIndex.m_DictIndex.GetDictionaryIndex()/* m_source_data.m_SourceFeatures.m_lIDSource*/);
			}
			return true;
		}
		return false;
	}
	SS_CATCH(L"")
}
bool CSMExpert::GetWordMorphoInfo(const TCHAR * pWord,list<SS::Dictionary::Types::TWordInfo>&  l_Info)
{
	SS_TRY
	{
		if(!pWord) return false;
		/*char buf[50];
		setlocale(LC_ALL, "");
		wcstombs(buf,pWord,50);
		string str(buf);*/
		//list<SS::Dictionary::Types::TWordInfo> l_MorphoInfo;
		l_Info.clear();
		//list<SS::Dictionary::Types::TWordInfo>::iterator i;
		if(
			m_pSMManager->GetDictionaryInterface()->GetWordInfo(pWord, &l_Info, /*in*/SS::Dictionary::Types::efzBaseDictionary)
			/*GetByForm(&str,&l_Info,SS::Dictionary::DATA_TYPE::efzBaseDictionary)*/
			)
		{
			//for (i=l_MorphoInfo.begin();i!=l_MorphoInfo.end(); i++) {
			//	l_Sources.push_back(i->front().m_WordIndex.m_DictIndex.GetDictionaryIndex());
			//}
			return true;
		}
		return false;
	}
	SS_CATCH(L"")
}


IUnit * CSMExpert::JoinUpUnits(TBoxUnits & oUnits)
{
	SS_TRY
	{
		if(oUnits.empty()) return NULL;
		IUnit * pUnit=m_pIBlackBoxFactory->CreateUnit();
		if(oUnits.size()==1 )
		{
			oUnits.front()->InsertUnitVertical(pUnit);
		}else pUnit->JoinUpInterval(oUnits.front(),oUnits.back());
		return pUnit;
	}
	SS_CATCH(L"")
}

void CSMExpert::SetSyntaxCategory(IUnit * pUnit,const Types::SyntaxCategories & oCategory)
{
	SS_TRY
	{
		pUnit->CreateSyntaxFeature()->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Assign(oCategory);
	}
	SS_CATCH(L"")

}
void CSMExpert::AddSyntaxCategory(IUnit * pUnit, const Types::SyntaxCategories & oCategory)
{
	SS_TRY
	{
		ISyntaxFeature * pFeature=pUnit->GetSyntaxFeature();
		if(!pFeature)
		{
			pFeature=pUnit->CreateSyntaxFeature();
			pFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.SetValue(0);
		}
		pFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.AssignOR(&oCategory);
	}
	SS_CATCH(L"")
}
void CSMExpert::DeleteSyntaxCategory(IUnit * pUnit, const Types::SyntaxCategories & oCategory)
{
	SS_TRY
	{
		ISyntaxFeature * pFeature=pUnit->GetSyntaxFeature();
		if(pFeature)
		{
			pFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.Remove(oCategory);
		}
	}
	SS_CATCH(L"")
}
bool CSMExpert::IsContainSyntaxCategory(IUnit * pUnit, const Types::SyntaxCategories & oCategory)
{
	SS_TRY
	{
		ISyntaxFeature * pFeature=pUnit->GetSyntaxFeature();
		if(pFeature)
		{
			return pFeature->GetSyntaxFeatureAposteriori()->m_SyntaxCategories.IsContain(oCategory);
		}
		return false;
	}
	SS_CATCH(L"")
}
void CSMExpert::AddSyntaxCategoryIndex(const Types::SyntaxCategories & oCategory, IUnit * pUnit)
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return;
		UINT uiIndex=oCategory.GetSingleValue()+ (SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24);
		AddIndex(uiIndex, pUnit);
	}
	SS_CATCH(L"")
}
void CSMExpert::AddSyntaxCategoryIndices(list<Types::SyntaxCategories> & l_oCategory, IUnit * pUnit)
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return;
		list<Types::SyntaxCategories>::iterator i;
		for (i=l_oCategory.begin(); i!=l_oCategory.end(); i++) 
			AddSyntaxCategoryIndex(*i,pUnit);
	}
	SS_CATCH(L"")
}
void CSMExpert::RemoveSyntaxCategoryIndex(const Types::SyntaxCategories & oCategory, IUnit * pUnit)
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return;
		UINT uiIndex=oCategory.GetSingleValue()+ (SS::Dictionary::DATA_TYPE::NAMES::ednSyntax<<24);
		IIndex * pIndex=FindIndex(uiIndex,pUnit);
		if(pIndex)
		{
			pIndex->ReleaseIndex();
		}
	}
	SS_CATCH(L"")
}
bool CSMExpert::IsContainSyntaxCategoryIndex(const Types::SyntaxCategories & oCategory, IUnit * pUnit)
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return false;

		UINT uiIndex=(SS::Dictionary::DATA_TYPE::NAMES::/*ednSemantic*/ednSyntax<<24) + oCategory.GetSingleValue();
		IIndex * pIndex=FindIndex(uiIndex,pUnit);
		return pIndex!=NULL;
	}
	SS_CATCH(L"")
}

IIndex * CSMExpert::FindIndex(UINT uiIndex, IUnit * pUnit)
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return NULL;
		IIndex * pIndex=pUnit->GetFirstIndex();
		if(!pIndex) return NULL;
		do{
			if(pIndex->GetDictionaryIndex() &&
				(pIndex->GetDictionaryIndex()->GetFirst()==uiIndex ) )
				return pIndex;
		}while((pIndex=pUnit->GetNextIndex()));
		return NULL;
	}
	SS_CATCH(L"")
}


void CSMExpert::AddIndex(SS::Dictionary::Types::TDictionaryIndex uiIndex, IUnit * pUnit )
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return;

		IIndex * pIndex=m_pIBlackBoxFactory->CreateIndex();
		pIndex->GetDictionaryIndex()->AppendIndex(uiIndex);
		pIndex->SetIndexationFlag(true);

		pUnit->AddIndex(pIndex);
	}
	SS_CATCH(L"")
}


bool CSMExpert::CollectSemanticFeature(SS::Core::Features::CSemanticFeature & oSemanticFeature, IUnit * pUnit)
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return false;

		IIndex * pIndex;
		//UINT uiIndex;
		bool bSemantic=false;
		SS::Core::Features::CSemanticFeature oSemanticFeature2;
		IDictionaryIndex* pDictionaryIndex;
		if (pIndex=pUnit->GetFirstIndex())
		{
			do{
				pDictionaryIndex = pIndex->GetDictionaryIndex();
				//uiIndex=pDictionaryIndex->GetFirst();
				if (pDictionaryIndex->GetFirst().GetDictionaryNumber()/*uiIndex>>24*/ == SS::Dictionary::DATA_TYPE::NAMES::ednSemantic)
				{
					//uiIndex&=0x00ffffff;
					//m_pIAMCSemantic->EnCode(uiIndex,&oSemanticFeature2);
					m_pIAMCSemantic->EnCode(pDictionaryIndex->GetFirst().GetId(),&oSemanticFeature2);
					oSemanticFeature.AssignOR(&oSemanticFeature2);
					bSemantic=true;
				}
			}while(pIndex=pUnit->GetNextIndex());
		}
		return bSemantic;
	}
	SS_CATCH(L"")
}
bool CSMExpert::KeepSpecifedSemanticTypes(const SS::Core::Features::Types::TSemanticType & oSemType, list<SS::Dictionary::Types::TDictionaryIndex> & l_Sem)
{
	SS_TRY
	{
		SS::Core::Features::CSemanticFeature oFeature;
		list<SS::Dictionary::Types::TDictionaryIndex>::iterator i;
		for (i=l_Sem.begin(); i!=l_Sem.end();) 
		{
			oFeature.Undefine();
			m_pIAMCSemantic->EnCode((*i).GetId(),&oFeature);
	//				if (oFeature.m_SemanticType.Equal(oSemType)) i++;
			if (oSemType.IsContain(oFeature.m_SemanticType)) i++;
			else i=l_Sem.erase(i);
		}
		return !l_Sem.empty() ;

	}
	SS_CATCH(L"")
}

void CSMExpert::RemoveSemanticIndicesFromUnit(list<SS::Dictionary::Types::TDictionaryIndex> & l_Sem, IUnit * pUnit)
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return;

		IIndex * pIndex;
		SS::Dictionary::Types::TDictionaryIndex uiIndex;
		if (pIndex=pUnit->GetFirstIndex())
		{
			do{
				uiIndex=pIndex->GetDictionaryIndex()->GetFirst();
				if (pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()== SS::Dictionary::DATA_TYPE::NAMES::ednSemantic)
				{
					//pIndex->PopUp();
					pIndex=pIndex->ReleaseIndex();
					//l_Sem.push_back(uiIndex&0x00ffffff);
					l_Sem.push_back(uiIndex);
				}else pIndex=pUnit->GetNextIndex();
			}while(pIndex);
		}
	}
	SS_CATCH(L"")
}

void CSMExpert::AddSemanticIndex(SS::Dictionary::Types::TDictionaryIndex lSem, IUnit * pUnit )
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return ;
		//UINT uiIndex=(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic<<24) + lSem;
		//AddIndex(uiIndex,pUnit);
		AddIndex(lSem,pUnit);
	}
	SS_CATCH(L"")
}

void CSMExpert::SetSemanticIndices(list<SS::Dictionary::Types::TDictionaryIndex> & l_Sem, IUnit * pUnit )
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return ;
		for (list<SS::Dictionary::Types::TDictionaryIndex>::iterator i=l_Sem.begin(); i!=l_Sem.end(); i++) 
		{
			AddSemanticIndex(*i, pUnit);
		}
	}
	SS_CATCH(L"")
}

void CSMExpert::SetSemanticIndicesWithOutGeneric(list<SS::Dictionary::Types::TDictionaryIndex> & l_Sem, IUnit * pUnit )
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return ;
		for (list<SS::Dictionary::Types::TDictionaryIndex>::iterator i=l_Sem.begin(); i!=l_Sem.end(); i++) 
		{
			AddSemanticIndex(i->GetDictionaryIndexWithoutFlags(), pUnit);
		}
	}
	SS_CATCH(L"")
}

bool CSMExpert::IsContainSemanticIndex(long  lSem,  IUnit * pUnit)
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return false;

		IIndex * pIndex;
		UINT uiIndex=(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic<<24) + lSem;
		if (pIndex=pUnit->GetFirstIndex())
		{
			do{
				//if(uiIndex == pIndex->GetDictionaryIndex()->GetFirst() )
				if(uiIndex == pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex())
					return true;
			}while(pIndex=pUnit->GetNextIndex());
		}
		return false;
	}
	SS_CATCH(L"")
}

void CSMExpert::AddSyntaxIndex(UINT uiIndex,IUnit * pUnit)
{
	SS_TRY
	{
		uiIndex+=(SS::Dictionary::DATA_TYPE::NAMES::ednSyntax)<<24;
		AddIndex(uiIndex,pUnit);
	}
	SS_CATCH(L"")
}
void CSMExpert::AddSyntaxCategoryIndices(list<UINT> & l_oCategory, IUnit * pUnit)
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return ;
		list<UINT>::iterator i;
		for (i=l_oCategory.begin(); i!=l_oCategory.end(); i++) {
			AddSyntaxIndex(*i,pUnit);
		}
	}
	SS_CATCH(L"")
}
void CSMExpert::RemoveSyntaxCategoryIndices(list<UINT> & l_oCategory, IUnit * pUnit)
{
	SS_TRY
	{
		if(pUnit==NULL) pUnit=m_pCurrentUnit;
		if(pUnit==NULL) return ;
		for (IIndex * i=pUnit->GetFirstIndex(); i ;) {
			if( (i->GetDictionaryIndex()->GetFirst().GetIndexType()/*GetFirst() >> 24*/)==SS::Interface::Core::BlackBox::eitSyntaxIndex)
			{
				l_oCategory.push_back(i->GetDictionaryIndex()->GetFirst().GetDictionaryIndex());
				i=i->ReleaseIndex();
			}else  i=pUnit->GetNextIndex();
		}
	}
	SS_CATCH(L"")
}


void CSMExpert::Mem()
{
	SS_TRY
	{
		m_oMem._pIText=m_pIText;
		m_oMem._pParagraph=m_pCurrentParagraph;
		m_oMem._pSentence=m_pCurrentSentence;
		m_oMem._pUnit=m_pCurrentUnit;
	}
	SS_CATCH(L"")
}
void CSMExpert::GotoMem()
{
	SS_TRY
	{
		m_pIText=m_oMem._pIText;
		m_pCurrentParagraph=m_oMem._pParagraph;
		m_pCurrentSentence=m_oMem._pSentence;
		m_pCurrentUnit=m_oMem._pUnit;
	}
	SS_CATCH(L"")
}
void CSMExpert::Mem1()
{
	SS_TRY
	{
		m_oMem1._pIText=m_pIText;
		m_oMem1._pParagraph=m_pCurrentParagraph;
		m_oMem1._pSentence=m_pCurrentSentence;
		m_oMem1._pUnit=m_pCurrentUnit;
	}
	SS_CATCH(L"")
}
void CSMExpert::GotoMem1()
{
	SS_TRY
	{
		m_pIText=m_oMem1._pIText;
		m_pCurrentParagraph=m_oMem1._pParagraph;
		m_pCurrentSentence=m_oMem1._pSentence;
		m_pCurrentUnit=m_oMem1._pUnit;
	}
	SS_CATCH(L"")
}
void CSMExpert::Mem2()
{
	SS_TRY
	{
		m_oMem2._pIText=m_pIText;
		m_oMem2._pParagraph=m_pCurrentParagraph;
		m_oMem2._pSentence=m_pCurrentSentence;
		m_oMem2._pUnit=m_pCurrentUnit;
	}
	SS_CATCH(L"")
}
void CSMExpert::GotoMem2()
{
	SS_TRY
	{
		m_pIText=m_oMem2._pIText;
		m_pCurrentParagraph=m_oMem2._pParagraph;
		m_pCurrentSentence=m_oMem2._pSentence;
		m_pCurrentUnit=m_oMem2._pUnit;
	}
	SS_CATCH(L"")
}
void CSMExpert::GotoUnit(IUnit * pUnit)
{
	SS_TRY
	{
		if(!pUnit) return;
		m_pCurrentUnit=pUnit;
	}
	SS_CATCH(L"")


}

/// убирает юнит и возвращает истину, если есть юниты спарава, ложь в противном случае ( текущим становится предыдущий)
bool CSMExpert::HideCurrentUnit()
{
	SS_TRY
	{
		if(m_pCurrentUnit->GetRightUnit())
		{
			m_pCurrentUnit=m_pCurrentUnit->ReleaseUnitHorizontal();
			return true;
		}else 			{
			m_pCurrentUnit->ReleaseUnitHorizontal();
			GetLastUnit();
			return false;
		}
		return false;
	}
	SS_CATCH(L"")


}

bool CSMExpert::IsSemanticIndex(IIndex* pIndex)
{
	SS_TRY
	{
		if(!pIndex || !pIndex->GetDictionaryIndex() ) return false;
		if ( (/*(*/pIndex->GetDictionaryIndex()->GetFirst().GetIndexType()/*GetFirst())>>24*/)==SS::Interface::Core::BlackBox::eitSemanticIndex/*SS::Dictionary::DATA_TYPE::NAMES::ednSemantic*/)
			return true;
		return false;
	}
	SS_CATCH(L"")


}
bool CSMExpert::IsMorphoIndex(IIndex* pIndex)
{
	SS_TRY
	{
		if(!pIndex) pIndex=m_pCurrentIndex;
		if(!pIndex || !pIndex->GetDictionaryIndex() ) return false;

		if( pIndex->GetDictionaryIndex()->GetFirst().GetIndexType() == SS::Interface::Core::BlackBox::eitMorphoIndex )
		{
			return true;
		}
		/*else
		switch((pIndex->GetDictionaryIndex()->GetFirst())>>24) {
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng :
		case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus :
		case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName :
		case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName :
		case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName :
		case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName :
		case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng :
		case SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus :
			return true;
			break;
		default:
			return false;
		}*/
		return false;
	}
	SS_CATCH(L"")

}

//////////////////////////////////////////////////////////////////////////

}
}
