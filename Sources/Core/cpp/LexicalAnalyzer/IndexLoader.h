#pragma once

#include "..\ASSInterface\IBlackBoxUnit.h"
#include "..\ASSInterface\IBlackBox.h"
using namespace SS::Interface::Core::BlackBox;

#include "..\ASCInterface\defines.h"
#include "..\ASCInterface\IInterface.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASSInterface\ICoSeDi.h"
using namespace SS::Interface::Core::CoSeDi;
using namespace SS::CoSeDi::DataTypes;

#include "..\ASSInterface\IMainAnalyse.h"
using namespace SS::Interface::Core::MainAnalyse;

#include "..\ASSInterface\IDictionaryManager.h"
using namespace SS::Interface::Core::Dictionary;

#include "..\ASSInterface\TDictionaryManager.h"
using namespace SS::Dictionary::Types;

#include "InternalRepresentation.h"
#include "UnitChecker.h"
#include "TreeDictionary.h"


namespace SS
{
namespace LexicalAnalyzer
{

class CIndexLoader
{
public:
	CIndexLoader();
	~CIndexLoader();

public:
	void Init(ILoadManager* pLoadManager, IBlackBoxTextStorage* pStorage, IAnalyseParams* pAnalyseParams, 
		IR::TUnitStorage* pUnitStorage, CUnitChecker* pUnitChecker);
	void SetUseQueryDeformer(bool bSet);
	void NextText();

	bool FillMorphoInfo(IUnit* pUnit, LPCTSTR szWord, bool bOnlySearch = false);
	void FillSemanticInfo(IUnit* pUnit);
	void FillSemanticInfoBySuffix(IUnit* pUnit);
	void FillSemanticVERB(IUnit* pUnit);
	void FillSynonims(IUnit* pUnit);

	void FillWord(IUnit* pUnit);

	void FillCombinatoryInfo(IR::CSentence* pSent);
	void FillNumberCombinatory(IR::CSentence* pSent);

	bool FillMorphoInfoToComplexNumber(IUnit* pParentUnit);
	UINT GetNumericValue(IUnit* pUnit);

	void CopySemanticsFromChildren(IUnit* pParentUnit);

	typedef set<TDictionaryIndex> TIndexSet;
	void FillQueryWordSet(TIndexSet* pSet);
	UINT GetQueryWordSourceID(LPCTSTR sz);

	bool IsRomanNumber(wstring str);
	/// Является ли строка порядковым римским числом (XVIIIый)
	bool IsRomanOrdinal(wstring word);
	
	/// Является ли строка порядковым окончанием
	bool IsOrdinalParticle(LPCTSTR szLexem);

	/// Получить индекс первого символа после после числа
	int GetCharBeforeNumber(wstring word);

private:
	void FillSemanticInfo(IIndex* pIndex, TIndexSet* pstIndexes, IUnit* pUnit = NULL);
	void FillSynonims(IIndex* pIndex, TIndexSet* pstIndexes);
	void FillCombiInfoList(IR::CSentence* pSent, list<SCombinationNewInfo>& lCombiInfos);

	void ApplyCombinationList(list<SCombinationNewInfo>* pCombiInfos, IR::CSentence* pSent, bool bNum);
	void FillUnitInfoByCombinatory(IR::CUnit* pUnit, SCombinationNewInfo* pInfo, bool bMorpho);
	void FillUnitInfoByNumberCombinatory(IR::CUnit* pUnit, SCombinationNewInfo* pInfo, bool bMorpho);
	
	void LoadSuffixesSemanticDict();
	
	bool IsRomanSymbol(wchar_t ch);
	/// Получить индекс первого символа после после римского числа
	int GetCharBeforeRomanNumber(wstring word);

private:
	ISemanticAnalysis* m_pSemantic;
	ISemanticAnalysis* m_pSynonims;
	ICombinatory* m_pCombinatory;
	INumberCombinatory* m_pNumCombinatory;
	SS::Interface::Core::CommonServices::IWorkRegister * m_pIWorkRegister;
    SS::Interface::Core::AMConverter::IAMConverterMorpho* m_pMorphoConverter;

	bool m_bMorpho;
	bool m_bSemantic;
	bool m_bSynonims;
	bool m_bLite;
	IBlackBoxTextStorage* m_pStorage;
	IR::TUnitStorage* m_pUnitStorage;

	TIDSourceVector m_vIDSources;
	list<TWordInfo> m_lMorpho;
	list<SCombinationNewInfo> m_lCombiInfos;
	list<TDictionaryIndex> m_lSem;
	TIndexSet m_stIndexes;

	TSetOfSourcesAndForms m_stSorucesAndForms;
	TListOfSetsOfSourcesAndForms m_lSetsOfSourcesAndForms;

	IMorphologyAnalyser* m_pMorphoAnalyzer;
	CUnitChecker* m_pUnitChecker;

	bool m_bQuery;
	bool m_bUseQueryDeformer;

	TreeDictionary::CTreeDictionary* m_pSemanticSuffixesDict;
};

}
}
