#pragma once

#include "..\ASSInterface\IBlackBox.h"
using namespace SS::Interface::Core::BlackBox;


namespace SS
{
namespace LexicalAnalyzer
{

class CIndexLoader;

enum EWordType
{
	wtUndefined = 0,
	wtWord = 0x11,
	// Число
	wtNumber,
	// Месяц
	wtMonth,
	wtLWord,
	// Last Name (Фамилия)
	wtLName,
	// First Name (Имя)
	wtFName,
	// Middle Name (Отчество)
	wtMName,
	wtInitial,
	wtGenitive,
	wtNumberDay,
	wtNumberYear,
	wtComma,
	wtApostroph,
	wtSmallWord,
	wtWordCentury,
	/*Слово - год, полугодие*/
	wtWordYear,
	wtDefis,
	wtWordDay,
	wtDMY,
	wtGenConj,
	wtOrg,
	wtSpecificOrg,
	wtQuote,
	wtRQuote,
	wtCity,
	wtState,
	wtCountry,
	wtQuotation,
	wtNameParticle,
	wtAmpersand,
	wtTire,
	// 3 цифры
	wtThreeDigits,
	wtMeasureDay,
	wtMeasureMonth,
	wtName,
	wtNonSyntax,
	wtColon,
	wtSlash,
	wtPoint,
	wtEngWord,
	wtLetterS,
	wtURLPart,
	wtAt,
	wtRomanNumber,
	wtSTerminated,
	wtOrdinalParticle,
	wtLetterT,
	wtVolume,
	wtQuarter,
	wtTo,
	wtLSmallWord,
	wtCityState,
	wtQListWord,
	wtQThe,
	wtQNames,
	wtQOf,
	wtQWhat,
	wtQAre,
	wtVerb,
	wtProperName,
	wtGeoSemantic,
	wtDatePreposition,
	wtProperNameUnknown,
	wtOrgAbbr,
	wtGeoConj,
	wtGeoSemanticPre,
	wtGenetiveAdj,
	wtGenetiveNoun,
	wtNounSeqRoot,
	wtDativeAdj,
	wtDativeNoun,	
	wtDativeAndGenAdj,
	wtDativeAndGenNoun,	
	wtLQuote,
	wtOrdinalNumber,
	wtYearQuarter,
	//2 цифры
	wtTwoDigits,
	//1 цифра
	wtOneDigit,
	//Время дня - утро, вечер
	wtWordDayTime,
	// Десятилетие, Декада (semtitIndef)
	wtWordDecade,
	// 4 цифры
	wtFourDigits,
	// 5 цифр
	wtFiveDigits,
	// 6 цифр
	wtSixDigits,
	// 7 цифр
	wtSevenDigits,
	// 8 цифр
	wtEightDigits,
	// 9 цифр
	wtNineDigits,
	// 10 цифр
	wtTenDigits,
	// 11 цифр
	wtElevenDigits,
	// Левая скобка
	wtLeftBracket,
	// Правая скобка
	wtRightBracket,
	// Слово телефон
	wtTelephoneWord, 
	// URL
	wtURL,
	// "+"
	wtPlusSign,
	// Слова - доб., доб - означающие добавочный номер
	wtTelephoneAdditionWord
};

enum EWalkerMode
{
	wmDefault,			//0
	wmInitial,			//1
	wmQuotations,		//2
	wmNames,			//3
	wmDates,			//4
	wmOrganizations,	//5
	wmPlaces,			//6
	wmReparsedItems,	//7
	wmNumberSemantic,	//8
	wmLSequences,		//9
	wmNonSyntax,		//10
	wmQuarterTo,		//11
	wmQuery,			//12
	wmMultiNames,		//13
	wmInSequences,		//14
	wmTelephones		//15
};

#define IR_MODES_COUNT (wmTelephones + 1)

namespace IR
{

/////////////////////////////////////////////////////////////////////////////////////
// CPointerStorage

template<class T>
class CPointerStorage
{
public:
	~CPointerStorage()
	{
		for(unsigned int i = 0; i < m_vReleased.size(); i++)
			delete m_vReleased[i];
		for(TPointerSet::iterator it = m_stAllocated.begin(); it != m_stAllocated.end(); ++it)
			delete *it;
	}

public:
	void Reserve(int iCount)
	{
		m_vReleased.reserve(iCount);
		for(int i = 0; i < iCount; i++)
			m_vReleased.push_back(new T);
	}

	T* Allocate()
	{
		T* p = 0;
		if (!m_vReleased.empty())
			p = m_vReleased.back();
		if(p)
			m_vReleased.pop_back();
		else
			p = new T;
		m_stAllocated.insert(p);
		return p;
	}

	void Release(T* p, bool bInternalReset)
	{
		TPointerSet::iterator it = m_stAllocated.find(p);
		assert(it != m_stAllocated.end());
		(*it)->Reset(bInternalReset);
		m_vReleased.push_back(*it);
		m_stAllocated.erase(it);
	}

public:
	typedef vector<T*> TPointerVect;
	typedef set<T*> TPointerSet;
	TPointerVect m_vReleased;
	TPointerSet m_stAllocated;
};

/////////////////////////////////////////////////////////////////////////////////////
// CUnit

class CUnit
{
public:
	CUnit(IUnit* pUnit = NULL);

public:
	void Reset(bool bReleaseIUnit);

	void SetWordType(EWordType eType, EWalkerMode eMode = wmDefault, bool bReplace = true);
	EWordType GetType(EWalkerMode eMode);
	EWordType GetTypeStrict(EWalkerMode eMode);
	void ClearMode(EWalkerMode eMode);

	void SetUnit(IUnit* pUnit);
	IUnit* GetUnit();
	operator IUnit*();

	void SetTag(UINT uiTag) { m_uiTag = uiTag; }
	UINT GetTag() { return m_uiTag; }

	/// remove indexes of eitRemove type to all unit children (but not to this unit itself)
	void RemoveChildrenIndexes(EIndexType eitRemove);
	/// remove indexes of eitRemove type
	void RemoveIndexes(EIndexType eitRemove);
	/// remove indexes of ednRemove type
	void RemoveIndexes(DictionariesNumber ednRemove);

	/// checks if the unit has indexes of ednCompare type
	bool HasIndexes(DictionariesNumber ednCompare);

	void CopyUnit(IUnit* pUnit, IBlackBoxTextStorage* pStorage);
	bool HaveCoincidentIndexes(IUnit* pUnit, EIndexType eit);
	void CopyWordTypes(CUnit* pFrom);

private:
	IUnit* m_pUnit;
	EWordType m_rModes[IR_MODES_COUNT];
	UINT m_uiTag;
};

typedef CPointerStorage<CUnit> TUnitStorage;

/////////////////////////////////////////////////////////////////////////////////////
// CSentence

class CSentence
{
private:
	typedef list<CUnit*> TUnitList;
public:
	CSentence();

public:
	CUnit* GetFirstUnit();
	CUnit* GetNextUnit();
	CUnit* GetNextUnitWithoutMove();
	CUnit* GetPrevUnit();
	CUnit* GetLastUnit();

	void RemoveUnit(CUnit* pUnit);
	void RemoveUnits(int nToLeft, int nCount);
	void RemoveUnits(EWordType eType);

	/// resets sentence without clearing it, so it can be reused later
	void Reset();
	/// adds new unit (or reuses an old one), and returns its pointer
	CUnit* AddUnit();
	/// returns unit count
	int GetUnitCount();

	/// delete this
	void ReleaseSentence();
	/// creates BlackBox::ISentence from internal representation IR::CSentence
	ISentence* GetSentence(IBlackBoxTextStorage* pStorage, bool bOnlySyntax = false);
	vector<ISentence*> GetSentences(IBlackBoxTextStorage* pStorage, int maxUnitCountInSentence, bool bOnlySyntax = false);
	/// detects the most used language in the sentence
	void SetLanguage();
	/// get language
	EAnalyseLanguage GetLanguage();

	/// adds nCount units from the current position to pUnit, and replaces them with pUnit
	void JoinUnits(CUnit* pUnit, int nCount);
	/// removes nCount units from current position, and replaces them with ppUnit array
	void ReplaceUnits(CUnit** ppUnit, int iUnitCount, int nCount);
	/// nToLeft is how far from current position is the rightest of joined units
	void JoinUnits(CUnit* pUnit, int nCount, int nToLeft);
	/// nToLeft is how far from current position is the rightest of replaced units
	void ReplaceUnits(CUnit** ppUnit, int iUnitCount, int nCount, int nToLeft);

	/// joins last nCount units
	void JoinLastUnits(CUnit* pUnit, int nCount);
	/// join all sentence's units
	void JoinSentence(CUnit* pUnit);

	/// returns unit that is nToLeft items to the left from current position
	CUnit* GetUnit(int nToLeft);
	/// returns the unit that has eType type in eMode mode, and is within [CurrPos - nToLeft - nCount, CurrPos - nToLeft] interval
	CUnit* GetUnit(int nCount, int nToLeft, EWalkerMode eMode, EWordType eType);

	/// moves nCount units from pSource's end to this sentence beginning, if nCount < 0, all sentence is moved
	void MoveUnits(CSentence* pSource, int nCount);

	/// checks if pUnit is the first unit of sentence
	bool IsFirstUnit(CUnit* pUnit);

	/// checks if a sequence of units has coincident indexes of eit type
	bool HaveCoincidentIndexes(int nCount, int nToLeft, EIndexType eit);
	/// returns number of units with at least one index with eit type
	int GetUnitCount(int nCount, int nToLeft, EIndexType eit);

	/// checks if all words in the specified range have eType type (if bAll), or none of them (if !bAll)
	bool CheckRangeWordType(int nCount, int nToLeft, EWalkerMode eMode, EWordType eType, bool bAll = true);

	TUnitStorage* GetUnitStorage();

private:
	ISentence* GetSentence(IBlackBoxTextStorage* pStorage, TUnitList::iterator &startIt, int maxUnitCount, bool bOnlySyntax);

	TUnitList m_lUnits;
	TUnitList::iterator m_it;
	TUnitList::iterator m_itTop;
	int m_iUnitCount;

	EAnalyseLanguage m_Language;

	TUnitStorage m_oUnitStorage;
};

}
}
}
