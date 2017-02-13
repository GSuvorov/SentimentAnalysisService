#include "StdAfx.h"
#include ".\smcomplexname.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{


CSMComplexName::CSMComplexName(void) : m_lSemanticCitation(0)
{
	m_sDescription=_T(__FUNCTION__);
}

CSMComplexName::~CSMComplexName(void)
{
}

bool CSMComplexName::AnalizeSentence()
{
	//SS_TRY
	//{
		//if(!GetFirstUnit()) return true;

		//CSMComplexName::TLSmanticDescription oDesc;
		//CSMComplexName::SSmanticDescription oSem;

		//TBoxUnits l_units;
		//int iNotProperNames;
		//bool bEnglish;

		//do{
	//		if (  (m_LexemType.IsEng()|| m_LexemType.IsRus() || m_LexemType.IsEnG())/* && m_pCurrentUnit->m_SpecialUnitType==ustUndefined*/ )
	//		{
	//			if (m_LexemType.IsEng(3))
	//			{							
	//				if (IsPrepositionSyntax()) continue;
	//			}
	//			iNotProperNames=0;
	//			Mem();
	//			if ( m_LexemType.IsRus() ) bEnglish=false;
	//			else bEnglish=true;
	//			do 
	//			{
	//				if ( bEnglish==true &&( m_LexemType.Iseng(7) || m_LexemType.IsSymbol()&&(m_LexemType.IsSTire() || m_LexemType.IsSDefis() )) ||
	//					bEnglish==false && (m_LexemType.Isrus(2) || m_LexemType.IsSComma() )|| SpecialTypeEquals(ustInitials)//m_pCurrentUnit->m_SpecialUnitType==ustInitials
	//						)
	//				{ 
	//					iNotProperNames++;
	//					if ( iNotProperNames >1 || !IsValidWord()) break;
	//					oSem._bProper=false;
	//				}else
	//				{
	//					oSem._bProper=true;
	//				}
	//				oDesc.push_back(oSem);
	//				l_units.push_back(m_pCurrentUnit);
	//			}while( GetNextUnit()&& SpecialTypeEquals(ustUndefined) /*m_pCurrentUnit->m_SpecialUnitType==ustUndefined */&&
	//					(
	//						bEnglish==true && 
	//						(  m_LexemType.IsEng() || m_LexemType.Iseng(7) || m_LexemType.IsEnG() || m_LexemType.IsSymbol()&& (m_LexemType.IsSTire() || m_LexemType.IsSDefis()) 
	//								)  ||
	//								bEnglish==false && (  m_LexemType.IsRus() || m_LexemType.Isrus(2) ) ||
	//						/*m_pCurrentUnit->m_SpecialUnitType==ustInitials*/ SpecialTypeEquals(ustInitials) || 
	//						m_LexemType.IsSComma()
	//					)
	//				);
	//			ProcessUnits(l_units,oDesc);
	//			if (l_units.size()>1)
	////							if (l_units.empty()==false)
	//			{
	//				//GotoUnit(l_units.front());
	//				//Mem();
	//				DetectProper(oDesc,l_units);
	//				//if (l_units.empty()==false)GotoUnit(l_units.back());
	//
	//			}else GotoMem();
	//			l_units.clear();
	//			oDesc.clear();
	//		}
		//}while(GetNextUnit());
		return true;
	//}
	//SS_CATCH(L"")
}

//bool CSMComplexName::Init(IText * pIText)
//{
//}

void CSMComplexName::ProcessUnits(TBoxUnits & l_units, CSMComplexName::TLSmanticDescription& oDesc)
{
	//if ( l_units.empty()==false && ( m_LexemType.Iseng(0,l_units.back()) || m_LexemType.Isrus(0,l_units.back()) || m_LexemType.IsSymbol(l_units.back())))
	//{//удаляем последний элемент если он не имя собственное
	//	l_units.pop_back();
	//	oDesc.pop_back();
	//}
	TBoxUnits::iterator it;
	CSMComplexName::TLSmanticDescription::iterator itSD;
	//SS::Core::Features::CMorphoFeature oMorphoF;
	bool bComma=false;
//	TCHAR sz_Word[MAX_CHARS_IN_WORD + 1];

	for (it=l_units.begin(), itSD=oDesc.begin(); it!=l_units.end() ; it++ , itSD++)
	{
		if(m_LexemType.IsSymbol(*it) && m_LexemType.SymbolEqualsTo(_T("),_T("),*it) )
		{
			bComma=true;
			continue;
		}
		if (itSD->_bProper==false) continue;

		itSD->_oSemanticFeature.m_SPlaceType.SetValue(0);
		if ( CollectSemanticFeature(itSD->_oSemanticFeature,*it) )
		{
			m_MorphoFeature.Undefine();
			//m_pMorphoService->CollectMorphoFeatureFull(m_pIAMC,(*it)->m_MorphoInfo,oMorphoF);
			CollectMorphoFeature(m_MorphoFeature,*it);
			if (m_MorphoFeature.m_PronounClass.IsContain(Values::PronounClass.pncInterrogativeRelative)||
				m_MorphoFeature.m_AdverbClass.IsContain(Values::TAdverbClass.adcInterrogative_Relative))
				itSD->_bProper=false;
			else itSD->_bProper=true;
		}else itSD->_bProper=false;
	}
	if (l_units.empty()==false)
	{
		if (oDesc.back()._oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtOrganization ))
		{
			for (itSD=oDesc.begin(); itSD!=oDesc.end(); itSD++)
			{
				itSD->_oSemanticFeature.m_SemanticType=oDesc.back()._oSemanticFeature.m_SemanticType;
				itSD->_oSemanticFeature.m_SOrganizationType=oDesc.back()._oSemanticFeature.m_SOrganizationType;
				itSD->_bProper=true;
			}
		}
	}
	while (l_units.empty()==false)
	{
		if ( oDesc.front()._bProper==false ) 
		{
			oDesc.pop_front();
			l_units.pop_front();
		}else if ( oDesc.back()._bProper==false ) 
		{
			oDesc.pop_back();
			l_units.pop_back();
		}else break;
	}
	if(bComma)
	{
		itSD=oDesc.begin();
		if(itSD->_oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName))
		{
			while(itSD->_bProper && itSD!=oDesc.end())
			{
				itSD++;
			}
			if(itSD!=oDesc.end())
			{
				itSD++;
				if(itSD!=oDesc.end() && itSD->_oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName))
				{
					itSD++;
					if(itSD!=oDesc.end() && itSD->_oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstMiddleName))
					{
						bComma=false;
					}

				}

			}
		}
		if (bComma)
		{
			while(oDesc.size()>1)
			{
				oDesc.pop_back();
			}
		}
	}
}

void CSMComplexName::DetectProper(CSMComplexName::TLSmanticDescription& oDesc, TBoxUnits& l_units)
{
	SS::Core::Features::CSemanticFeature tsemFeature,oS;
	CSMComplexName::TLSmanticDescription::iterator it;
	SS::Core::Features::Types::TSPersonType oPersonType;
	UINT uiCount=0;
	list<long> l_Sem;
	list<long> l_SemGeo;
	IUnit * pHead=NULL;
	bool bLoc=false, bGood=false, bPerson=false;

	uiCount=MergeSemantic(oDesc,tsemFeature,oPersonType);

	bLoc=ConvertFeatureToType(tsemFeature,oPersonType,uiCount,l_SemGeo,l_Sem,bPerson);
	pHead=CheckSemantic(l_units,l_SemGeo,l_Sem);

	bLoc=tsemFeature.m_SemanticType.IsContain(Values::TSemanticType.smtOrganization) ;

//	TBoxUnits l_Add;
//	list<Types::SyntaxCategories> l_Cat;

	//if( bLoc /*&& FindPreposition(pHead,l_Add,l_Cat,false)*/ )
	//{
	//	//SetSyntaxInfo(pHead,l_Add,usntComplexProperNameSyntax);
	//	//SetSyntaxInfo(pHead,l_units,usntComplexProperNameSyntax);
	//	//pHead->m_SyntaxInfo.SetSyntaxCategory(l_Cat);
	//	//pHead->m_SyntaxInfo.SetSemantic(l_Sem);
	//}else
//		pHead->m_EntityInfo.SetSemantic(l_Sem);


		//SetSemanticIndices(l_Sem,pHead);
}

bool CSMComplexName::IsValidWord()
{
	//m_pMorphoService->CollectMorphoFeature(m_pIAMC,m_pCurrentUnit->m_MorphoInfo,oMorphoFeature);
//	m_MorphoFeature.Undefine();
//	CollectMorphoFeature(m_MorphoFeature);
	bool bG=false;
	if (!IsContainFeature(efcVerb) ) 
	{
		bG=true;
	}
	if (!bG) return false;
//	TCHAR sz_w[100]; 
	if ( IsPrepositionSyntax()) return false;

	return true;
}

bool CSMComplexName::ConvertFeatureToType(SS::Core::Features::CSemanticFeature & oSemanticFeature,SS::Core::Features::Types::TSPersonType&  oPersonType,  UINT uiNonProperCount , list<long> & l_SemGeo, list<long> & l_Sem, bool & bPerson)
{
	bool bLoc=false;
	if (oSemanticFeature.m_SemanticType.GetValue()>1) 
	{
		SS::Core::Features::CSemanticFeature oS;
		SS::Dictionary::Types::TDictionaryIndex lVal;
		if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtPerson) && uiNonProperCount<2) 
		{
			oS.Undefine();
			oS.m_SemanticType.Assign(Values::TSemanticType.smtPerson);
			if (oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstProfession)) 
			{
				oS.m_SPersonType.Assign(Values::TSPersonType.semprstProfession);
				m_pIAMCSemantic->Code(&oS,&lVal);
				l_Sem.push_back(lVal.GetDictionaryIndex());
			}
			if (oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality) && uiNonProperCount==0) 
			{
				oS.m_SPersonType.Assign(Values::TSPersonType.semprstNationality);
				m_pIAMCSemantic->Code(&oS,&lVal);
				l_Sem.push_back(lVal.GetDictionaryIndex());
			}
			if ( oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstName) && oPersonType.IsContain(Values::TSPersonType.semprstFirstName)&& oPersonType.IsContain(Values::TSPersonType.semprstLastName))
			{
				oS.m_SPersonType.Assign(Values::TSPersonType.semprstName);
				m_pIAMCSemantic->Code(&oS,&lVal);
				l_Sem.push_back(lVal.GetDictionaryIndex());
				bPerson=true;
			}
			//if ( oPersonType.IsContain(Values::TSPersonType.semprstFirstName)&& oPersonType.IsContain(Values::TSPersonType.semprstLastName))
			//{
			//	oS.m_SPersonType.Assign(Values::TSPersonType.semprstName);
			//	m_pIAMCSemantic->Code(&oS,&lVal);
			//	l_Sem.push_back(lVal.GetDictionaryIndex());
			//	bPerson=true;
			//}
		}
		if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtPlace)) 
		{
			oS.Undefine();
			oS.m_SemanticType.Assign(Values::TSemanticType.smtPlace);

			if ( oSemanticFeature.m_SPlaceType.IsIntersectsWith(&Values::TSPlaceType.Undefined) ) 
			{
				oS.m_SPlaceType.Undefine();
				m_pIAMCSemantic->Code(&oS,&lVal);
				l_SemGeo.push_back(lVal.GetDictionaryIndex());
				bLoc=true;
			}
			if (oSemanticFeature.m_SPlaceType.IsContain(Values::TSPlaceType.pltCity)) 
			{
				oS.m_SPlaceType.Assign(Values::TSPlaceType.pltCity);
				m_pIAMCSemantic->Code(&oS,&lVal);
				l_SemGeo.push_back(lVal.GetDictionaryIndex());
				bLoc=true;
			}
			if (oSemanticFeature.m_SPlaceType.IsContain(Values::TSPlaceType.pltContinent)) 
			{
				oS.m_SPlaceType.Assign(Values::TSPlaceType.pltContinent);
				m_pIAMCSemantic->Code(&oS,&lVal);
				l_SemGeo.push_back(lVal.GetDictionaryIndex());
				bLoc=true;
			}
			if (oSemanticFeature.m_SPlaceType.IsContain(Values::TSPlaceType.pltCountry)) 
			{
				oS.m_SPlaceType.Assign(Values::TSPlaceType.pltCountry);
				m_pIAMCSemantic->Code(&oS,&lVal);
				l_SemGeo.push_back(lVal.GetDictionaryIndex());
				bLoc=true;
			}
			if (oSemanticFeature.m_SPlaceType.IsContain(Values::TSPlaceType.pltProvince)) 
			{
				oS.m_SPlaceType.Assign(Values::TSPlaceType.pltProvince);
				m_pIAMCSemantic->Code(&oS,&lVal);
				l_SemGeo.push_back(lVal.GetDictionaryIndex());
				bLoc=true;
			}
			if (oSemanticFeature.m_SPlaceType.IsContain(Values::TSPlaceType.pltState)) 
			{
				oS.m_SPlaceType.Assign(Values::TSPlaceType.pltState);
				m_pIAMCSemantic->Code(&oS,&lVal);
				l_SemGeo.push_back(lVal.GetDictionaryIndex());
				bLoc=true;
			}
		}
		if (oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtOrganization)) 
		{
			oS.Undefine();
			oS.m_SemanticType.Assign(Values::TSemanticType.smtOrganization);
			if (oSemanticFeature.m_SOrganizationType.IsContain(Values::TSOrganizationType.orgtCompany)) {
				oS.m_SOrganizationType.Assign(Values::TSOrganizationType.orgtCompany);
				m_pIAMCSemantic->Code(&oS,&lVal);
				l_Sem.push_back(lVal.GetDictionaryIndex());
				bLoc=true;
			}
			if (oSemanticFeature.m_SOrganizationType.IsContain(Values::TSOrganizationType.orgtBank)) {
				oS.m_SOrganizationType.Assign(Values::TSOrganizationType.orgtBank);
				m_pIAMCSemantic->Code(&oS,&lVal);
				l_Sem.push_back(lVal.GetDictionaryIndex());
				bLoc=true;
			}
			if (oSemanticFeature.m_SOrganizationType.IsContain(Values::TSOrganizationType.orgtFund)) {
				oS.m_SOrganizationType.Assign(Values::TSOrganizationType.orgtFund);
				m_pIAMCSemantic->Code(&oS,&lVal);
				l_Sem.push_back(lVal.GetDictionaryIndex());
				bLoc=true;
			}
			if (oSemanticFeature.m_SOrganizationType.IsContain(Values::TSOrganizationType.orgtInternationalOrganization)) {
				oS.m_SOrganizationType.Assign(Values::TSOrganizationType.orgtInternationalOrganization);
				m_pIAMCSemantic->Code(&oS,&lVal);
				l_Sem.push_back(lVal.GetDictionaryIndex());
				bLoc=true;
			}
		}
	}
	return bLoc;
}

UINT CSMComplexName::MergeSemantic(CSMComplexName::TLSmanticDescription& oDesc,SS::Core::Features::CSemanticFeature & tsemFeature, SS::Core::Features::Types::TSPersonType & oPersonType)
{
	UINT uiCount=0;
	CSMComplexName::TLSmanticDescription::iterator it;

	tsemFeature.m_SemanticType.FillUp();
	tsemFeature.m_SOrganizationType.FillUp();
	tsemFeature.m_SPersonType.FillUp();
	tsemFeature.m_SPlaceType.FillUp();
	oPersonType.SetValue(0);

	for (it=oDesc.begin(); it!=oDesc.end(); it++)
	{
		if (it->_bProper) {
			tsemFeature.m_SemanticType.AssignAND(& it->_oSemanticFeature.m_SemanticType);
			if (it->_oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtOrganization) )
				tsemFeature.m_SOrganizationType.AssignAND(& it->_oSemanticFeature.m_SOrganizationType);
			if (it->_oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtPerson) )
			{
				if (
					it->_oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstFirstName)||
					it->_oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstLastName)||
					it->_oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstMiddleName)
					)
					it->_oSemanticFeature.m_SPersonType.AssignOR(& Values::TSPersonType.semprstName);
				tsemFeature.m_SPersonType.AssignAND(& it->_oSemanticFeature.m_SPersonType);
			}
			oPersonType.AssignOR(& it->_oSemanticFeature.m_SPersonType);
			if (it->_oSemanticFeature.m_SemanticType.IsContain(Values::TSemanticType.smtPlace) )
				tsemFeature.m_SPlaceType.AssignAND(& it->_oSemanticFeature.m_SPlaceType);
		}else uiCount++;
	}
	return uiCount;
}

IUnit * CSMComplexName::CheckSemantic(TBoxUnits & l_units , list<long> & l_SemGeo, list<long> & l_Sem )
{
	if ( l_SemGeo.empty()==false && l_units.size()==1 
		|| l_Sem.empty()==false 
		) 
	{ 
		IUnit * pHead;
		pHead=l_units.front();
		if (l_units.size()>1){ //SetExtraInfo(l_units.front(),l_units,ustComplexProperName);
			pHead=JoinUpUnits(l_units);
			SetSpecialType(ustComplexProperName,pHead);
		}

		if (l_units.size()>1) 
		{
			TBoxUnits::iterator itB;
			wstring str;
			for (itB=l_units.begin(); itB!=l_units.end(); itB++)
			{
				str+=(*itB)->GetWord();
			}
			pHead->SetWord(str.c_str());
		}
		return pHead;
	}
	return NULL;
}



		}
	}
}