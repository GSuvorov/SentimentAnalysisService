#pragma once

#include "RootItem.h"
#include "RootIterateItem.h"
#include "SyntaxFeature.h"

#include "..\ASSInterface\TDictionaryManager.h"
#include "..\ASCInterface\TCoommonTypes.h"
using namespace SS::Core::Types::IndexationSearch;

#include "..\ASSInterface\IBlackBoxUnit.h"
//#include "..\ASSInterface\Citation.h"
using namespace SS::Interface::Core::BlackBox;
using namespace SS::Core::BlackBox::Types;

#include <string>
#include <map>
#include <vector>
using namespace std;


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CUnit : 
		public CMiddleItem,
		public IUnit
{

public:
	CUnit();
	virtual ~CUnit();

public:
	virtual void AddUnitChild(IUnit *pUnit);
	virtual std::list<IUnit*> GetChildList();

	virtual void SetTransPosition(unsigned char uchTransPosition);
	virtual unsigned char GetTransPosition();
	virtual void SetPosition(unsigned int uiPosition);
	virtual unsigned int GetPosition();				
	virtual unsigned int GetFirstCharPosition();
	virtual void SetFirstCharPosition(unsigned int uiPosition);
	virtual EHighlightType GetHighlightType();
	virtual void SetHighlightType(EHighlightType hlt);
	virtual bool GetIsInAnswerSentence();
	virtual void SetIsInAnswerSentence(bool bIn);
	virtual const wchar_t* GetWord();
	virtual void SetWord(const wchar_t* wcWord);
	virtual void SetSyntaxFlag(bool bl);
	virtual bool GetSyntaxFlag();
	virtual ISyntaxFeature* GetSyntaxFeature();
	virtual ISyntaxFeature* CreateSyntaxFeature();
	virtual void AddIndex(IIndex* pIndex);
	virtual IIndex* GetFirstIndex();
	virtual IIndex* GetNextIndex();
	virtual void AddUnitVertical(IUnit* pUnit);
	virtual void AddUnitHorizontal(IUnit* pUnit);
	virtual void InsertUnitVertical(IUnit* pUnit);
	virtual void JoinUpInterval(IUnit* pLeft, IUnit* pRight);
	virtual IUnit* Pop();
	virtual IUnit* ReleaseUnitHorizontal();
	virtual IUnit* ReleaseUnitVertical();
	virtual void ClearUnit();
	virtual IUnit* GetParentUnit();
	virtual IUnit* GetLeftChildUnit();
	virtual IUnit* GetRightChildUnit();
	virtual IUnit* GetLeftUnit();
	virtual IUnit* GetRightUnit();
	virtual unsigned int GetChildCount();
	virtual bool HasChilds();
	virtual bool HasIndexType(EIndexType oIndexType, bool bIsAnalyseChild = false);
	virtual SUnitInfo* GetInfo();
	virtual void ClearInfo();
	// new func
	virtual int GetIsTargetObjectUnit();
	virtual int GetIsTargetObjectUnitFromParent();
	virtual void SetIsTargetObjectUnit(int bIsTargetObjectUnit);

	virtual void SetIsTargetObjectUnitFromChilds(int bIsTargetObjectUnit);
	virtual int GetIsTargetObjectUnitFromChilds();


	virtual unsigned int GetFirstCharPositionNew();
    virtual int GetLastCharPositionNew(const wchar_t *pText);

	virtual IUnit* GetReferenceUnit();
	virtual void SetReferenceUnit(IUnit* unit);

	virtual void SetCitation(TCitation *cit);
	virtual TCitation* GetCitation();
	virtual wstring GetWordFull();
	virtual bool IsVisual();

	virtual void SetTone(int nTone);
	virtual void SetToneStrong(int nTone);
	virtual int GetTone();

	virtual void SetRealTone(int nTone);
	virtual int GetRealTone();

	virtual void SetRealInversion(bool bInv);
	virtual bool GetRealInversion();

	virtual void SetInverted(bool bInv);
	virtual bool GetInverted();

	virtual void SetInversion(int nInversion);
	virtual int GetInversion();

	virtual void SetTermNumber(int nTermNumber);
	virtual int GetTermNumber();

	virtual void SetToneTermCount(int nTermNumber);
	virtual int GetToneTermCount();

	virtual void SetStopWordUnit(int val);
	virtual int GetStopWordUnit();

	virtual void SetNumber(int val);
	virtual int GetNumber();

	virtual void SetUnitToneDict(wchar_t* val);
	virtual wchar_t* GetUnitToneDict();

	virtual bool IsRusInitial();

	virtual bool IsInitial();

	virtual void SetFreeForTone(bool val);
	virtual bool GetFreeForTone();

	virtual void SetUnknownWord(bool val);
	virtual bool GetUnknownWord();

	virtual void SetPatternName(wstring val);
	virtual wstring GetPatternName();

	virtual void SetHashKey(__int64 val);
	virtual __int64 GetHashKey();


	virtual bool IsEvristicAbbr();

	virtual void SetEvristicLN(bool val);
	virtual bool GetEvristicLN();

	virtual void SetIsNominalOnly(bool val);
	virtual bool GetIsNominalOnly();

/*public:
	virtual void AddSemanticIndex(IIndex* pMorphoIndex, IIndex* pSemanticIndex);*/

private:
	void SetDefaultValues();
	bool HasIndexType(IUnit* pUnit, EIndexType oIndexType);

private:
	CRootIterateItem m_oIndexParent;
	CSyntaxFeature* m_pSyntaxFeature;
	SUnitInfo* m_pInfo;
	unsigned int m_uiPosition;
	unsigned int m_uiFirstCharPosition;
    int m_uiLastCharPosition;
	EHighlightType m_eHighlight;
	wstring m_strWord;
	bool m_bIsInAnswerSentence;
	bool m_bSyntaxFlag;
	unsigned char m_uchTransPosition;

	int m_nIsTargetObjectUnit; // если юнит относится к целевому объектe дайджеста, он помечается этим флагом
	// по умолчанию он  = false
	int m_nIsTargetObjectUnitFormChilds;
	TCitation *pCitata;
	IUnit* pReferenceUnit;
	int m_nInversion;
	int m_nTone; 
	int m_nTermNumber;	
	int m_nToneTermCount;
	int m_nStopUnit;
	int m_nRealTone;
    bool m_bRealToneCancel;
	bool m_bRealInversion;
	wstring m_unitToneDict;
	bool m_bInverted; // служит для проверки инвертирован ли глагол
	//int nPossibleCitSubjectUnit;
	bool m_bFreeForTone;
	bool m_bUnknownWord;
	wstring m_strPatternName;
	bool m_bEvristicLN;
	__int64 m_iHashKey;
	int m_nNumber;
	bool m_bNominalOnly;
	
	list<IUnit*> m_Childs;

/*private:
	typedef std::map<IIndex*, std::vector<IIndex*>> TMorphoSemanticMap;
	TMorphoSemanticMap m_MorphoSemanticMap;

public:
	virtual void OnPopItem(CLeafItem* pItem);*/

};

}
}
}
