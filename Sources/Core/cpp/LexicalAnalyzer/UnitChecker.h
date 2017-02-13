#pragma once

#include "..\ASCInterface\defines.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASSInterface\IAMConverter.h"
using namespace SS::Interface::Core::AMConverter;

#include "..\FeatureLibrary\SemanticFeature.h"
using namespace SS::Core::Features;

#include "..\FeatureLibrary\define.h"
using namespace SS::Core::Features::Types;

#include "..\ASSInterface\IBlackBoxUnit.h"
#include "..\ASSInterface\IBlackBox.h"
using namespace SS::Interface::Core::BlackBox;

#include "..\ASSInterface\ICommon.h"
using namespace SS::Interface::Core::MainAnalyse;
using namespace SS::Interface::Core::Common;

#include "InternalRepresentation.h"
#include "FeatureChanger.h"


namespace SS
{
namespace LexicalAnalyzer
{

struct SRusLNSuffixInfo
{
	wstring suffix;
	wstring process;
	SS::Core::Features::Types::GenderType genType;
	SS::Core::Features::Types::CaseType caseType;
};

struct SMorphoInfo
{
	SS::Core::Features::Types::GenderType genType;
	SS::Core::Features::Types::CaseType caseType;

	bool operator==(const SMorphoInfo& inMI) const
	{	
		return 	(genType.Equal(inMI.genType)||
			genType.IsUndefined() || inMI.genType.IsUndefined())			
			&& (caseType.Equal(inMI.caseType) ||
			caseType.IsUndefined() || inMI.caseType.IsUndefined());			
	}

};

typedef std::list<SMorphoInfo> SMIList;


class CUnitChecker : public CAnalyseParams
{
public:
	CUnitChecker();
	~CUnitChecker();

// CAnalyseParams
public:
	virtual void SetAnalyseParams(IAnalyseParams* pAnalyseParams);

public:
	void Init(ILoadManager* pLoadManager, IBlackBoxTextStorage* pStorage);

	void AddDateIndex(IUnit* pUnit, const TSTimeIntervalType& Type);
	void AddPersonIndex(IUnit* pUnit, const TSPersonType& Type);
	void AddMeasureIndex(IUnit* pUnit, const TSMeasureType& Type);
	void AddPartOfSpeechIndex(IUnit* pUnit, const PartOfSpeechTypes& Type, const CaseType& Case, 
		const NumberType& Number, EAnalyseLanguage eal = ealEnglish);
	void AddNumericPartOfSpeechIndex(IUnit* pUnit, const NumericType& NumericType, wstring numEnd = L"");
	void AddPartOfSpeechIndex(IUnit* pUnit, IUnit* pSourceUnit);
	void AddOrganizationIndex(IUnit* pUnit, const TSOrganizationType& Type);
	void AddServiceTypeIndex(IUnit* pUnit, const TSServiceTypes& Type);
	void AddPlaceIndex(IUnit* pUnit, const TSPlaceType& Type);
	void AddBeSemanticIndex(IUnit* pUnit, const TSVBeType& Type);
	void AddSemanticIndex(IUnit* pUnit, const SS::Core::Features::Types::TSemanticType& Type);

	void AddAdjectiveIndexEvrisctic(IUnit* pUnit);
	bool IsAdjectiveEvrisctic(IUnit* pUnit);

	void AddSyntaxCategoryIndex(IUnit* pUnit, const SyntaxCategories& sc);

	void FillMorphoTypes(IR::CUnit* pUnit);
	void FillSemanticTypes(IR::CUnit* pUnit);

	void ChangeFeature(IUnit* pUnit, EChangeFeature ecf);
	void RemoveExtremeChildren(IUnit* pParentUnit);

	void ClearChildrenSemantics(IUnit* pUnit, const SS::Core::Features::Types::TSemanticType& TypeToLeave);
	void ClearSemantics(IUnit* pUnit, const SS::Core::Features::Types::TSemanticType& TypeToLeave);
	bool CopyIndexes(IUnit* pFrom, IUnit* pTo, const SS::Core::Features::Types::TSemanticType& Type);

	IUnit* ChildHasCase(IUnit* pParentUnit, const CaseType& Type);
	bool UnitHasCase(IUnit* pUnit, const CaseType& Type);
	void GetChildCases(IUnit* pParentUnit, list<CaseType>& Types);
	void GetCases(IUnit* pParentUnit, list<CaseType>& Types);
	
	void CopyFeaturesFromChildren(IUnit* pParentUnit,
		const SS::Core::Features::Types::PartOfSpeechTypes& PartOfSpeech = SS::Core::Features::Values::PartOfSpeechTypes.Undefined);

	SS::Core::Features::Types::PartOfSpeechTypes GetPartOfSpeech(IIndex* pIndex);
	bool HasSyntaxCategory(IUnit* pUnit, const SyntaxCategories& sc);

	bool HasSemantic(IUnit* pUnit, const SS::Core::Features::Types::TSemanticType& Type);
	bool HasSemantic1(IUnit* pUnit, const SS::Core::Features::Types::TSemanticType& Type,
		const SS::Core::Features::Types::TSPersonType& persType =  SS::Core::Features::Values::TSPersonType.Undefined);
	bool HasProfession(IUnit* pUnit);
	void GetMorpho(IUnit *pUnit, SS::Core::Features::CMorphoFeature& resMorphoFeature);

	bool HasPartOfSpeechType(IUnit* pUnit, const PartOfSpeechTypes& Type);

	bool IsOrdinal(IUnit* pUnit);
	IUnit* SetKeywordSyntax(IUnit* pUnit);

	void CopyFeaturesFromChildren1(IUnit* pParentUnit);

	void AddPartOfSpeechIndexProper(IUnit* pUnit, IUnit* pSourceUnit);

	bool isXZ(IUnit* pUnit);

	void AddPartOfSpeechIndex(IUnit* pUnit, const PartOfSpeechTypes& Type, const CaseType& Case, 
										const NumberType& Number,const GenderType& Gender, EAnalyseLanguage eal);

	

	void AddRusLastNameIndexEvrisctic(IUnit* pUnit);
	void AddPartOfSpeechIndexWithAnim(IUnit* pUnit, const PartOfSpeechTypes& Type, const CaseType& Case, 
										const NumberType& Number,const GenderType& Gender, EAnalyseLanguage eal);

	bool HasCase(IIndex* pIndex, const CaseType& Type);

	bool IsRusLNForms(SS::Interface::Core::BlackBox::IUnit *pUnit);

	/// проверка юнита на принадлежность словарю имен собственных
	bool IsProper(SS::Interface::Core::BlackBox::IUnit *pUnit);

private:

	void GetRusLNForms(SS::Interface::Core::BlackBox::IUnit *pUnit, SMIList& forms);

	void SetSemanticInfo(SS::Dictionary::Types::TDictionaryIndex oIndex, IR::CUnit* pUnit, EAnalyseLanguage language, bool isOfficial = false);
	/// returns true if pIndex is of NOUN type
	void SetMorphoInfo(IIndex* pIndex, IR::CUnit* pUnit);

	void AddSemanticIndex(IUnit* pUnit);
	void AddMorphoIndex(IUnit* pUnit,  EAnalyseLanguage eal = ealEnglish);



	DictionariesNumber GetChildrenDictionary(IUnit* pParent, DictionariesNumber ednDefault);



private:
	CSemanticFeature* m_pSemanticFeature;
	CMorphoFeature* m_pMorphoFeature;

	IAMConverterSemantic* m_pSemanticConverter;
	IAMConverterMorpho* m_pMorphoConverter;

	IBlackBoxTextStorage* m_pStorage;

	bool m_bMorpho;
	bool m_bSemantic;
	bool m_bSyntax;

	bool m_bIndexation;
};

}
}
