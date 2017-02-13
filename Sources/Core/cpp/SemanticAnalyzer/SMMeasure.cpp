#include "StdAfx.h"
#include ".\smmeasure.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{

const wchar_t* cwTire = L"-";


CSMMeasure::CSMMeasure(void)
{
	m_bInit = false;
	m_sDescription=_T(__FUNCTION__);
	m_s_Time=new set<long>;
	m_bManyMeasureUnits = false;
}

CSMMeasure::~CSMMeasure(void)
{
	delete m_s_Time;
}

bool CSMMeasure::Init(IText * pIText)
{
	SS_TRY
	{
		if ( !CSMSentenceExpert::Init(pIText)) return false;

		if(!m_bInit)
		{
			m_bInit = true;

			SS::Core::Features::CSemanticFeature oSemanticFeature;
			SS::Dictionary::Types::TDictionaryIndex lVal=0;

			oSemanticFeature.m_SemanticType=Values::TSemanticType.smtTimeInterval;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			m_s_Time->insert(lVal.GetDictionaryIndex());

			oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitDateYear;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			m_s_Time->insert(lVal.GetDictionaryIndex());
			oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitDateDay;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			m_s_Time->insert(lVal.GetDictionaryIndex());
			oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitDateMonth;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			m_s_Time->insert(lVal.GetDictionaryIndex());
			oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitDayTime;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			m_s_Time->insert(lVal.GetDictionaryIndex());
			oSemanticFeature.m_STimeIntervalType=Values::TSTimeIntervalType.semtitCentury;
			m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
			m_s_Time->insert(lVal.GetDictionaryIndex());
		}

	}
	SS_CATCH(L"")

	return true;
}

bool CSMMeasure::IsTire()
{
	SS_TRY
	{
		return ((*m_pCurrentUnit->GetWord()) == (*cwTire));
	}
	SS_CATCH(L"")
}


bool CSMMeasure::AnalizeSentence()
{
	SS_TRY
	{
		TBoxUnits l_Units;
		bool bTire = false;
		SS::Core::Features::CSemanticFeature oSemanticFeature,oSemanticFeatureC;
		do{
			//wstring str = m_pCurrentUnit->GetWord();
			if (IsSyntax() || IsContainFeature(efcPretext) || IsTire() || m_LexemType.IsDigits()) continue;

			if (IsContainFeature(efcSemanticMeasure) && m_pCurrentUnit->GetInfo()->_TimeType.GetValue()<1)
			{
				Mem();
				if(!GetNextUnit()) GotoMem();
				else if((bTire=IsTire()) || (!IsSyntax() && IsContainFeature(efcSemanticMeasure) && 
					m_pCurrentUnit->GetInfo()->_TimeType.GetValue()<1 && !m_LexemType.IsDigits() && 
					!(IsSemNumber() && m_pCurrentUnit->HasChilds())))
				{
					if(!bTire && !m_bManyMeasureUnits)
					{
						GotoMem();
						if(!m_LexemType.IsDigits() && !(IsSemNumber() && m_pCurrentUnit->HasChilds()))
						{
							Mem2(); 
							m_bManyMeasureUnits = true;
						}
					}else
						GotoMem();
					continue;
				}else
					GotoMem();

				MeasureProcessEx();
			}else if (m_pCurrentUnit->GetInfo()->_TimeType.GetValue()>1 && IsContainFeature(efcSemanticMeasure))
			{
				MeasureTimeProcess();
			}else if (m_LexemType.IsSymbol() && m_LexemType.SymbolEqualsTo(_T("$")) )
			{
				l_Units.push_back(m_pCurrentUnit);
				if (AddDigitsBack(l_Units))
				{
					ProcessMeasure(l_Units,Values::TSMeasureType.msrtPrice);
				}else if(GetPrevUnit())
				{
					if(AddDigitsFront(l_Units))
						ProcessMeasure(l_Units,Values::TSMeasureType.msrtPrice);
					else GetNextUnit();
				}
				l_Units.clear();
				oSemanticFeatureC.Undefine();
			}else if (m_pCurrentUnit->GetInfo()->_TimeType.GetValue()>0)
			{
				TimeProcess();
			}
		}while(GetNextUnit());		
	}
	SS_CATCH(L"")

	return true;
}

void CSMMeasure::MeasureProcess()
{
	SS_TRY
	{
		TBoxUnits l_Units;
		SS::Core::Features::CSemanticFeature oSemanticFeature,oSemanticFeatureC;

		CollectSemanticFeature(oSemanticFeatureC);
		Mem();

		if(m_bManyMeasureUnits)
		{
			GotoMem2();
			m_bManyMeasureUnits = false;
		}

		//wstring str = m_pCurrentUnit->GetWord();

		if(!GetPrevUnit()) 
		{
			GotoMem();
		}
		else if (!m_LexemType.IsDigits() && !IsSemNumber())
		{
			Mem1();
			if(IsTire() && FindFirstTire())
				GotoMem2();
			else
				GotoMem1();

			CollectSemanticFeature(oSemanticFeature);
			if (oSemanticFeature.m_SPersonType.IsContain(Values::TSPersonType.semprstNationality))
			{
				l_Units.push_front(m_pCurrentUnit);
			}else if (oSemanticFeature.m_SemanticType.GetValue()<1)
			{
				if (m_LexemType.WordEqualsTo(_T("cubic"))|| m_LexemType.WordEqualsTo(_T("board")) )
				{
					oSemanticFeature.m_SMeasureType=Values::TSMeasureType.msrtVolume;
				}else if (m_LexemType.WordEqualsTo(_T("square"))||m_LexemType.WordEqualsTo(_T("sq."))||m_LexemType.WordEqualsTo(_T("квадратных"))||m_LexemType.WordEqualsTo(_T("кв.")))
				{
					oSemanticFeature.m_SMeasureType=Values::TSMeasureType.msrtSurface;
				}else if ( oSemanticFeatureC.m_SMeasureType.IsContain(Values::TSMeasureType.msrtWeight) && m_LexemType.WordEqualsTo(_T("register")))
				{
					oSemanticFeature.m_SMeasureType=Values::TSMeasureType.msrtVolume;
				}else if (m_LexemType.WordEqualsTo(_T("speed")))
				{
					oSemanticFeature.m_SMeasureType=Values::TSMeasureType.msrtSpeed;
				}else
				{
					l_Units.clear();
					GotoMem();
					oSemanticFeatureC.Undefine();
					oSemanticFeature.Undefine();
					return;
				}
				l_Units.push_front(m_pCurrentUnit);
			}
			if((IsContainFeature(efcRepresentBecome) && !m_LexemType.IsSymbol()) || !GetPrevUnit()) 
			{
				GotoMem();
			}else if (AddDigitsFront(l_Units))
			{
				if (oSemanticFeature.m_SMeasureType.IsContain(Values::TSMeasureType.msrtVolume))
					ProcessMeasure(l_Units,Values::TSMeasureType.msrtVolume);
				else if (oSemanticFeature.m_SMeasureType.IsContain(Values::TSMeasureType.msrtSurface))
					ProcessMeasure(l_Units,Values::TSMeasureType.msrtSurface);
				else if (oSemanticFeature.m_SMeasureType.IsContain(Values::TSMeasureType.msrtWeight))
					ProcessMeasure(l_Units,Values::TSMeasureType.msrtWeight);
				else if (oSemanticFeature.m_SMeasureType.IsContain(Values::TSMeasureType.msrtSpeed))
					ProcessMeasure(l_Units,Values::TSMeasureType.msrtSpeed);
				else 
				{
					list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;
					GotoMem();
					RemoveSemanticIndicesFromUnit(l_Sem);
					IUnit * pHead=JoinUpUnits(l_Units);
					if (pHead)
					{
						m_pCurrentUnit=pHead;
						AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctMeasure);
						SetSemanticIndicesWithOutGeneric(l_Sem);
						SetSyntax();
					}
				}
			}else 
				GotoMem();

		}else if(AddDigitsFront(l_Units))
		{
			bool bGood = false;
			if( oSemanticFeatureC.m_SMeasureType.IsContain(Values::TSMeasureType.msrtLength) && GetNextUnit() && m_LexemType.WordEqualsTo(_T("кв.")))
			{
				bGood = true;
				ProcessMeasure(l_Units,Values::TSMeasureType.msrtSurface);
			}else if(oSemanticFeatureC.m_SMeasureType.IsContain(Values::TSMeasureType.msrtLength))
			{
				UINT uiDepth=0;
				Mem2();
				while(m_pCurrentUnit&& uiDepth<4)
				{
					GetPrevUnit();
					if(m_LexemType.WordEqualsTo(_T("speed")))
					{
						bGood = true;
						ProcessMeasure(l_Units,Values::TSMeasureType.msrtSpeed);
					}
					uiDepth++;
				}
				GotoMem2();
				if(GetNextUnit() && m_LexemType.WordEqualsTo(_T("per")))
				{
					bGood = true;
					l_Units.clear();
					l_Units.push_back(m_pCurrentUnit);
					if (GetNextUnit() && m_pCurrentUnit->GetInfo()->_TimeType.GetValue()>0)
					{
						l_Units.push_back(m_pCurrentUnit);
						ProcessMeasure(l_Units,Values::TSMeasureType.msrtSpeed);
					}
					l_Units.clear();
				}
				GotoMem2();
			}

			if(!bGood)
			{
				list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;
				GotoMem();
				RemoveSemanticIndicesFromUnit(l_Sem);
				SetSemanticIndicesWithOutGeneric(l_Sem);

				IUnit * pHead=JoinUpUnits(l_Units);
				if (pHead)
				{	
					m_pCurrentUnit=pHead;
					AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctMeasure);
					SetSemanticIndicesWithOutGeneric(l_Sem);
					SetSyntax();
				}
			}

		}/*else GotoMem();*/
		l_Units.clear();
		oSemanticFeatureC.Undefine();
		oSemanticFeature.Undefine();
		GotoMem();
		if (AddDigitsBack(l_Units))
		{
			list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;
			GotoMem();
			RemoveSemanticIndicesFromUnit(l_Sem);
			IUnit * pHead=JoinUpUnits(l_Units);
			if (pHead)
			{
				m_pCurrentUnit=pHead;
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctMeasure);
				SetSemanticIndicesWithOutGeneric(l_Sem);
				SetSyntax();
			}
		}/*else GotoMem();*/
		GotoMem();
	}
	SS_CATCH(L"")
}

bool CSMMeasure::FindFirstTire()
{
	if(!m_pCurrentUnit) return false;

	if (GetPrevUnit() && !IsSyntax() && IsContainFeature(efcSemanticMeasure) && 
		m_pCurrentUnit->GetInfo()->_TimeType.GetValue()<1)
	{
		if (GetPrevUnit() && IsTire())
		{
			Mem2();
			return true;
		}
	}
	return false;
}

void CSMMeasure::MeasureProcessEx()
{
	if(!IsSetMeasure())
	{
		MeasureProcess();
	}
}

void CSMMeasure::MeasureTimeProcess()
{
	bool bAimWords = false;

	Mem();
	if (GetNextUnit())
	{
		bAimWords = CheckAimWords();
	}
	GotoMem();
	if (bAimWords)
	{
		TimeProcess();
		if(!IsSetMeasure())
		{
			MeasureProcess();
		}
	}else
	{
		if(!IsSetMeasure())
		{
			MeasureProcess();
		}
		TimeProcess();
	}
}

bool CSMMeasure::CheckAimWords()
{
	if (m_LexemType.WordEqualsTo(_T("after")) || m_LexemType.WordEqualsTo(_T("earlier")) || 
		m_LexemType.WordEqualsTo(_T("before")) || m_LexemType.WordEqualsTo(_T("ago")) || 
		m_LexemType.WordEqualsTo(_T("later")) ||  m_LexemType.WordEqualsTo(_T("назад")))
	{
		return true;
	}
	return false;
}

bool CSMMeasure::IsSetMeasure()
{
	IUnit * pUnit;

	if(m_pCurrentUnit->HasChilds() && !IsSemNumber())
	{
		pUnit = m_pCurrentUnit->GetLeftChildUnit();
		do{
			if (m_LexemType.IsDigits(0,pUnit) || IsSemNumber(pUnit))
			{
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctMeasure);
				return true;
			}
		}while(pUnit = pUnit->GetRightUnit());
	}
	return false;

}

void CSMMeasure::TimeProcess()
{
	SS_TRY
	{
		TBoxUnits l_Units;
		Mem();
		bool bMarked=false;
		list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;
		list<UINT> l_Categories;
		bool bOld = false;

		while(GetNextUnit())
		{
			if(m_LexemType.WordEqualsTo(_T("old")))
			{
				bOld=true;
				break;
			}
		}
		if(!bOld) GotoMem();
		else GetPrevUnit();
		if (GetNextUnit())
		{
			if ((CheckAimWords() || m_LexemType.IsSTire())&& !bOld)
			{

				l_Units.push_back(m_pCurrentUnit);
				if (GetPrevUnit())
					if (IsSemTime()) l_Units.push_front(m_pCurrentUnit);
				if (GetPrevUnit())
				{
					if (IsSemTime()) 
					{
						RemoveSemanticIndicesFromUnit(l_Sem);
						RemoveSyntaxCategoryIndices(l_Categories);
						l_Units.push_front(m_pCurrentUnit);
					}
					else if (IsDeterminer(m_pCurrentUnit))
						l_Units.push_front(m_pCurrentUnit);
					else if (!AddDigitsFront(l_Units))
						l_Units.clear();
				}
				GotoMem();
				RemoveSemanticIndicesFromUnit(l_Sem);
				RemoveSyntaxCategoryIndices(l_Categories);
				SecondProcessTimeFixedPoint(l_Units);
				bMarked=true;
				l_Units.clear();

			}else if (m_LexemType.WordEqualsTo(_T("old")))
			{
				if (GetPrevUnit())
				{
					if(IsTire()) GetPrevUnit();
					if (IsFindNumberSemTime(l_Units))
					{
						l_Units.push_front(m_pCurrentUnit);
						ProcessMeasure(l_Units,Values::TSMeasureType.msrtAge);
					}else if(GetPrevUnit())
					{
						if(IsTire()) GetPrevUnit();
						if (IsSemTime())
						{
							l_Units.push_front(m_pCurrentUnit);
							ProcessMeasure(l_Units,Values::TSMeasureType.msrtAge);
						}else GotoMem();
					}else GotoMem();
				}else GotoMem();

				bMarked=true;
				l_Units.clear();
			}else if(m_LexemType.WordEqualsTo(_T("per")))
			{
				GotoMem();
				if(GetPrevUnit() && AddDigitsFront(l_Units))
				{
					ProcessMeasure(l_Units,Values::TSMeasureType.msrtSpeed);
				}
				GotoMem();
				bMarked=true;
				l_Units.clear();
			}
		}
		if (!bMarked)
		{
			GotoMem();
			if(!SubTimeProcess())
			{
				MoveToAjunctTime();
			}
		}
	}
	SS_CATCH(L"")
}

bool CSMMeasure::IsFindNumberSemTime(TBoxUnits & l_Units)
{
	SS_TRY
	{
		if(!m_pCurrentUnit) return false;

		Mem2();
		if(GetPrevUnit() && AddDigitsFront(l_Units))
			return true;
		else if(m_pCurrentUnit && m_pCurrentUnit->HasChilds() && IsSemNumber(m_pCurrentUnit->GetLeftChildUnit()))
			return true;
		GotoMem2();

		return false;
	}
	SS_CATCH(L"")
}

void CSMMeasure::MoveToAjunctTime()
{
	IUnit * pUnit;

	if(m_pCurrentUnit->HasChilds() && !IsSemNumber())
	{
		pUnit = m_pCurrentUnit->GetLeftChildUnit();
		do{
			if (m_LexemType.IsDigits(0,pUnit) || IsSemNumber(pUnit))
			{
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctTimeDuration);
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctTimeFixedPoint);
				AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctTimeEnd);
				break;
			}
		}while(pUnit = pUnit->GetRightUnit());
	}
}

bool CSMMeasure::SubTimeProcess()
{
	TBoxUnits l_Units;
	IUnit * pHead;
	list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;

	l_Units.push_back(m_pCurrentUnit);
	if(!GetPrevUnit()) 
	{
		GotoMem();
		return false;
	}
	else if (AddDigitsFront(l_Units))
	{
		RemoveSemanticIndicesFromUnit(l_Sem,l_Units.back());
		KeepSpecifedSemanticTypes(Values::TSemanticType.smtTimeInterval,l_Sem);
		pHead=JoinUpUnits(l_Units);
		if (pHead)
		{
			m_pCurrentUnit=pHead;
			AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctTimeDuration);
			AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctTimeFixedPoint);
			SetSemanticIndicesWithOutGeneric(l_Sem);
			SetSyntax();
		}
		return true;
	}
	else if(m_LexemType.WordEqualsTo(_T("next")) || m_LexemType.WordEqualsTo(_T("last")))
	{
		l_Units.push_front(m_pCurrentUnit);
		GotoMem();
		ProcessTimeFixedPoint(l_Units,m_pCurrentUnit);
		return true;
	}else if(AddOfFront(l_Units))
	{
		GotoMem();
		ProcessTimeFixedPoint(l_Units,m_pCurrentUnit);
		return true;
	}else if(AddOfBack(l_Units))
	{
		pHead=JoinUpUnits(l_Units);
		if (pHead)
		{
			m_pCurrentUnit=pHead;
			RemoveSemanticIndicesFromUnit(l_Sem,l_Units.back());
			KeepSpecifedSemanticTypes(Values::TSemanticType.smtTimeInterval,l_Sem);
			SetSemanticIndicesWithOutGeneric(l_Sem);
			pHead->GetInfo()->_TimeType.AssignOR(&Values::TSemanticType.smtTimeInterval);
		}
		return true;
	}else
		MoveToAjunctTime();
		
	GotoMem();
	
	return false;
}

bool CSMMeasure::AddOfFront(TBoxUnits & l_Units)
{
	Mem2();
	IUnit * pMem = NULL;
	UINT uiDepth=0;
	do{
		if(m_LexemType.WordEqualsTo(_T("of")))
		{
			pMem = m_pCurrentUnit;
			if (GetPrevUnit())
			{
				if(m_LexemType.WordEqualsTo(_T("end")) || m_LexemType.WordEqualsTo(_T("beginning")) ||
					m_LexemType.WordEqualsTo(_T("middle")))
				{
					l_Units.push_front(pMem);
					l_Units.push_front(m_pCurrentUnit);
					GotoMem2();
					return true;
				}
			}
		}
		if(m_LexemType.SymbolEqualsTo(_T("."))||m_LexemType.IsSComma())
			break;
		uiDepth++;
	}while(GetPrevUnit() && uiDepth<5);

	GotoMem2();
	return false;
}

bool CSMMeasure::AddOfBack(TBoxUnits & l_Units)
{
	Mem2();
	SS::Core::Features::CSemanticFeature oSemanticFeature;
	bool bVal=false;
	
	IUnit * pMem = NULL;
	//do{
		GotoMem();
		if(GetNextUnit() && m_LexemType.WordEqualsTo(_T("of")))
		{
			pMem = m_pCurrentUnit;
			if (GetNextUnit())
			{
				CollectSemanticFeature(oSemanticFeature);
				if(oSemanticFeature.m_STimeIntervalType.IsContain(Values::TSTimeIntervalType.semtitDateYear))
				{
					l_Units.push_back(pMem);
					l_Units.push_back(m_pCurrentUnit);
					bVal = true;
				}
			}
		}
	oSemanticFeature.Undefine();
	GotoMem2();
	return bVal;
}

bool CSMMeasure::AddDigitsFront(TBoxUnits & l_Units)
{
	Mem1();
	bool bVal=false;
	if (m_LexemType.IsDigits() || IsSemNumber())
	{
		list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;
		do{
			if(m_LexemType.IsSComma() ||m_LexemType.IsSDot() || IsTire())
			{
				if(!GetPrevUnit() ) break;
				if (m_LexemType.IsDigits()  || IsSemNumber())
				{
					l_Units.push_front(m_pCurrentUnit);
					if(IsSemTime())
						RemoveSemanticIndicesFromUnit(l_Sem);
				}
			}
			if (m_LexemType.IsDigits()  || IsSemNumber())
			{
				l_Units.push_front(m_pCurrentUnit);	
				if(IsSemTime())
					RemoveSemanticIndicesFromUnit(l_Sem);
			}else 
				break;
			bVal=true;
		}while(GetPrevUnit());
	}
	GotoMem1();
	return bVal;
		
}
bool CSMMeasure::AddDigitsBack(TBoxUnits & l_Units)
{
	Mem1();
	bool bVal=false;
	if (GetNextUnit() && (m_LexemType.IsDigits() || IsSemNumber()))
	{
		list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;
		do{
			if( m_LexemType.IsSComma() ||m_LexemType.IsSDot() )
			{
				if(!GetNextUnit() ) break;
				if (m_LexemType.IsDigits() || IsSemNumber()/*oSemanticFeature.m_SMeasureType.IsContain(Values::TSMeasureType.msrtNumber)*/)
				{
					l_Units.push_back(m_pCurrentUnit);
					if(IsSemTime())
						RemoveSemanticIndicesFromUnit(l_Sem);
				}
			}
			if (m_LexemType.IsDigits() || IsSemNumber()/*oSemanticFeature.m_SMeasureType.IsContain(Values::TSMeasureType.msrtNumber)*/)
			{
				l_Units.push_back(m_pCurrentUnit);
				if(IsSemTime())
					RemoveSemanticIndicesFromUnit(l_Sem);
			}
			else break;
			bVal=true;
		}while(GetNextUnit());
	}
	GotoMem1();
	return bVal;
}

void CSMMeasure::ProcessMeasure(TBoxUnits & l_Units, Types::TSMeasureType oMeasure)
{
	if (l_Units.empty()) return;

	SS::Core::Features::CSemanticFeature oSemanticFeature;
	SS::Dictionary::Types::TDictionaryIndex lVal;
	
	oSemanticFeature.m_SMeasureType=oMeasure;
	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtMeasure;

	IUnit * pHead=JoinUpUnits(l_Units);
	if (pHead)
	{
		m_pCurrentUnit=pHead;
		AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctMeasure);
		m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
		lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
		AddSemanticIndex(lVal);
		SetSyntax();
	}
}

void CSMMeasure::ProcessTimeFixedPoint(TBoxUnits & l_Units, IUnit * pUnit)
{
	if (l_Units.empty()) return;

	IUnit * pHead=JoinUpUnits(l_Units);
	if (pHead)
	{
		m_pCurrentUnit=pHead;
		AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctTimeFixedPoint);
		list<SS::Dictionary::Types::TDictionaryIndex> l_Sem;
		RemoveSemanticIndicesFromUnit(l_Sem,pUnit);
		KeepSpecifedSemanticTypes(Values::TSemanticType.smtTimeInterval,l_Sem);
		SetSemanticIndicesWithOutGeneric(l_Sem);
		SetSyntax();
	}
}

void CSMMeasure::SecondProcessTimeFixedPoint(TBoxUnits & l_Units)
{
	if (l_Units.empty()) return;

	SS::Core::Features::CSemanticFeature oSemanticFeature;
	SS::Dictionary::Types::TDictionaryIndex lVal;

	oSemanticFeature.m_SemanticType=Values::TSemanticType.smtTimeInterval;

	IUnit * pHead=JoinUpUnits(l_Units);
	if (pHead)
	{
		m_pCurrentUnit=pHead;
		AddSyntaxCategoryIndex(Values::SyntaxCategories.scAjunctTimeFixedPoint);
		m_pIAMCSemantic->Code(&oSemanticFeature,&lVal);
		lVal.SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
		AddSemanticIndex(lVal);
		SetSyntax();
	}
}

bool CSMMeasure::IsSemNumber(IUnit * pUnit)
{
	if(!pUnit) pUnit=m_pCurrentUnit;

	SS::Core::Features::CSemanticFeature oSemanticFeature;

	CollectSemanticFeature(oSemanticFeature,pUnit);
	if (oSemanticFeature.m_SMeasureType.IsContain(Values::TSMeasureType.msrtNumber))
	{
		return true;
	}
	return false;

}

bool CSMMeasure::IsSemTime()
{
	if (!m_pCurrentUnit ) return false;

	IIndex * pIndex;
	UINT uiDNumber, uiSource;
	set<long>::iterator itF;
	bool bTime=false;

	if ((pIndex=m_pCurrentUnit->GetFirstIndex()))
	{
		do{
			uiDNumber = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber();
			uiSource = pIndex->GetDictionaryIndex()->GetFirst().GetId();
			if (uiDNumber==SS::Dictionary::DATA_TYPE::NAMES::ednSemantic)
			{
				if (!bTime)
				{
					itF=m_s_Time->find(uiSource);
					bTime=(itF!=m_s_Time->end());
				}else break;
			}
		}while((pIndex=m_pCurrentUnit->GetNextIndex()));
	}
	return (bTime);



}


		}
	}
}