#pragma once

#include <list>
#include <map>

#include "stdafx.h"
#include "../ASSInterface/IDigestManager.h"
#include "../ASSInterface/IDictionaryManager.h"
#include "../ASSInterface/IAMConverter.h"
#include "../assinterface/iblackbox.h"
#include "../ASSInterface/ISyntaxAnalyzer.h"
#include "../../../[libraries]/GnuWin32/include/pcre.h"
#include "../ASSInterface/Constants.h"
#include "CToneDictPool.h"

#include <atlstr.h>
#include <atlcoll.h>

using namespace std;

typedef std::list<SS::Dictionary::Types::TWordInfo> TWIList;

struct SRusLNSuffixInfo
{
	wstring suffix;
	wstring process;
	SS::Core::Features::Types::GenderType genType;
	SS::Core::Features::Types::CaseType caseType;
};

struct SMorphoInfo
{
	/// род
	SS::Core::Features::Types::GenderType genType;
	/// падеж
	SS::Core::Features::Types::CaseType caseType;
	/// число
	SS::Core::Features::Types::NumberType numberType;
	/// одушевленность
    SS::Core::Features::Types::AnimInAnimType animationType;
	/// залог
	SS::Core::Features::Types::VoiceType voiceType;
	/// часть речи
    SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech;
	/// лицо
	SS::Core::Features::Types::PersonType personType;
	/// краткость
	SS::Core::Features::Types::ShortType shortType;
	/// степень сравнения
	SS::Core::Features::Types::DegreeType degreeType;
    
	void Clear()
	{
		genType.Undefine();
		caseType.Undefine();
		numberType.Undefine();
		animationType.Undefine();
		voiceType.Undefine();
		partOfSpeech.Undefine();
		personType.Undefine();
		shortType.Undefine();
		degreeType.Undefine();
	}

	bool operator==(const SMorphoInfo& inMI) const
	{
		bool genEqual = genType.Equal(inMI.genType) || genType.IsUndefined() || inMI.genType.IsUndefined();
		bool caseEqual = caseType.Equal(inMI.caseType) || caseType.IsUndefined() || inMI.caseType.IsUndefined();
		return genEqual && caseEqual;
	}

	bool IsIntersectsMorpho(const SMorphoInfo& inMI, bool genderTest = true, bool caseTest = true, bool numberTest = false) const
	{
		bool genderInter = !genderTest || genType.IsIntersectsWith(&inMI.genType) || genType.IsUndefined() || inMI.genType.IsUndefined();
		bool caseInter = !caseTest || caseType.IsIntersectsWith(&inMI.caseType) || caseType.IsUndefined() || inMI.caseType.IsUndefined();
		bool numberInter = !numberTest || numberType.IsIntersectsWith(&inMI.numberType) || numberType.IsUndefined() || inMI.numberType.IsUndefined();
		return genderInter && caseInter && numberInter;
	}
};

typedef std::list<SMorphoInfo> SMIList;
struct SStorageUnitInfo
{
	/// морфологическая информация
	SS::Core::Features::CMorphoFeature _morpho;
	/// список морфологических характеристик
	SMIList _listMI;
};
typedef map<SS::Interface::Core::BlackBox::IUnit*, SStorageUnitInfo> MorphoInformationList;

enum EQuoteType
{
	NotQuote = 0,
	Open,
	Close,
	OpenClose
};

struct WordForm
{
	wstring form;
	SS::Core::Features::Types::NumberType number;
	SS::Core::Features::Types::GenderType gender;

	WordForm()
	{
		form = L"";
		number = SS::Core::Features::Values::NumberType.Undefined;
		gender = SS::Core::Features::Values::GenderType.Undefined;
	}

	WordForm(wstring wordForm, SS::Core::Features::Types::NumberType formNumber)
	{
		form = wordForm;
		number = formNumber;
	}
};

struct MorphoTestInfo
{
	/// пройден тест на род
	bool IsGenderTestSuccess;
	/// пройден тест на падеж
	bool IsCaseTestSuccess;
	/// пройден тест на число
	bool IsNumberTestSuccess;
	/// пройден тест на регистр
	bool IsRegisterTestSuccess;
	/// пройден тест на время
	bool IsTimeTestSuccess;
	/// пройден тест на степень сравнения
	bool IsDegreeTestSuccess;
};

class UnitPropertiesOperator
{
private:
	SS::Interface::Core::AMConverter::IAMConverterSemantic*	m_pAMConverterSemantic;
	SS::Interface::Core::AMConverter::IAMConverterMorpho* m_pAMConverter;
	SS::Interface::Core::Dictionary::IMorphologyAnalyser* m_pMorphoAnalyzer;

	static MorphoInformationList m_morphoInfo;

public:
	UnitPropertiesOperator(
		SS::Interface::Core::AMConverter::IAMConverterSemantic*	pAMConverterSemantic,
		SS::Interface::Core::AMConverter::IAMConverterMorpho* pAMConverter,
		SS::Interface::Core::Dictionary::IMorphologyAnalyser* pMorphoAnalyzer)
	{
		m_pAMConverterSemantic = pAMConverterSemantic;
		m_pAMConverter = pAMConverter;
		m_pMorphoAnalyzer = pMorphoAnalyzer;
	}

	bool IsSemantic(SS::Interface::Core::BlackBox::IUnit* pUnit, SS::Core::Features::Types::TSemanticType semType,
		SS::Core::Features::Types::TSPersonType persType = SS::Core::Features::Values::TSPersonType.Undefined,
		SS::Core::Features::Types::TSPlaceType placeType = SS::Core::Features::Values::TSPlaceType.Undefined);

    /// прилагательное или причастие
	bool IsPseudoAdjective(SS::Interface::Core::BlackBox::IUnit* pUnit,
		SS::Core::Features::Types::CaseType caseType = SS::Core::Features::Values::CaseType.Undefined);

	bool IsMorpho(SS::Interface::Core::BlackBox::IUnit* pUnit, SS::Core::Features::CMorphoFeature oMorfoAnfr);

	void GetUnitMorfoFeatures(SS::Interface::Core::BlackBox::IUnit* pUnit,
		SS::Core::Features::CMorphoFeature& resMorphoFeature, SMIList *lstMI = NULL);

    /// возвращает все морфохарактеристики юнита по словарям
    void GetUnitMorfoFeaturesFullByMorphoAnalyzer(SS::Interface::Core::BlackBox::IUnit* pUnit,
		SS::Core::Features::CMorphoFeature &resMorphoFeature,
        SMIList *lstMI = NULL,
        vector<SS::Core::Features::CMorphoFeature> *pInclusion = NULL,
        vector<SS::Core::Features::CMorphoFeature> *pExclusion = NULL);
	void GetUnitMorfoFeaturesFullByMorphoAnalyzer(const wstring &word,
		SS::Core::Features::CMorphoFeature &resMorphoFeature,
        SMIList *lstMI = NULL,
        vector<SS::Core::Features::CMorphoFeature> *pInclusion = NULL,
        vector<SS::Core::Features::CMorphoFeature> *pExclusion = NULL);

	void GetUnitMorfoFeaturesFull(SS::Interface::Core::BlackBox::IUnit* pUnit,
		SS::Core::Features::CMorphoFeature &resMorphoFeature,
        SMIList *lstMI = NULL,
        vector<SS::Core::Features::CMorphoFeature> *pInclusion = NULL,
        vector<SS::Core::Features::CMorphoFeature> *pExclusion = NULL);
	void GetUnitMorfoFeaturesFull(const wstring &word,
		SS::Core::Features::CMorphoFeature &resMorphoFeature,
        SMIList *lstMI = NULL,
        vector<SS::Core::Features::CMorphoFeature> *pInclusion = NULL,
        vector<SS::Core::Features::CMorphoFeature> *pExclusion = NULL);

	void SaveMorpho(const unsigned char uchDictionaryNumber,
		const int iMorphoInfo,
		const int iPartOfSpeech,
		bool &isFirst,
		SS::Core::Features::CMorphoFeature &resMorphoFeature,
        SMIList *lstMI,
        vector<SS::Core::Features::CMorphoFeature> *pInclusion,
        vector<SS::Core::Features::CMorphoFeature> *pExclusion);

	void GetUnitSemanticFeatures(SS::Interface::Core::BlackBox::IUnit* pUnit,SS::Core::Features::CSemanticFeature& resSemanticFeature);

	wstring GetMainFormText(SS::Interface::Core::BlackBox::IUnit *pUnit,
		SS::Core::Features::Types::GenderType genderType);

	bool isHaveParentCombiNumeric(SS::Interface::Core::BlackBox::IUnit *pUnit);

	bool IsGoodDateSeq(SS::Interface::Core::BlackBox::IUnit *pUnit1,
		SS::Interface::Core::BlackBox::IUnit *pUnit2, TUnitList& sentUnits);

	bool FindInUnits(TUnitList& unitList, SS::Interface::Core::BlackBox::IUnit* pObjUnit);

	TUnitList::iterator FindUnit(TUnitList& list, SS::Interface::Core::BlackBox::IUnit* pUnit);

	void  GetLinearUnits(SS::Interface::Core::BlackBox::IUnit* root, list<SS::Interface::Core::BlackBox::IUnit*>& allUnits);

	bool IsProperUnknown(SS::Interface::Core::BlackBox::IUnit* pUnit); 

	bool IsNotHashOrgInsidePerson(SS::Interface::Core::BlackBox::IUnit* pUnit); 

    /**
    проверяет нахождение главной формы слова
    baseInfo - информация по проверяемому слову
    wordInfo - информация по главной форме проверяемого слова
    isVerb - проверяемому слово - глагол
    isParticiple - проверяемому слово - причастие
    partOfSpeechType - часть речи
    genderType - род для главной формы
    voiceType - залог для главной формы
    useCaseTest - проверять совпадение падежей
    useNumberTest - проверять на единственное число
    useRegisterTest - проверять регистр
    */
    bool IsSuitableMorphoInfo(SS::Dictionary::Types::TWordInfo baseInfo,
        SS::Dictionary::Types::TWordInfo wordInfo, bool isVerb, bool isParticiple,
        SS::Core::Features::Types::PartOfSpeechTypes partOfSpeechType,
        SS::Core::Features::Types::GenderType genderType, SS::Core::Features::Types::VoiceType voiceType,
		SS::Core::Features::Types::NumberType number,
        bool useCaseTest, bool useNumberTest, bool useRegisterTest, bool useTimeTest);

	MorphoTestInfo TestMorphoInfo(SS::Dictionary::Types::TWordInfo baseInfo, SS::Dictionary::Types::TWordInfo wordInfo, 
		bool isVerb, bool isParticiple, SS::Core::Features::Types::PartOfSpeechTypes partOfSpeechType,
        SS::Core::Features::Types::GenderType genderType, SS::Core::Features::Types::VoiceType voiceType,
		SS::Core::Features::Types::NumberType number);

	WordForm GetWordForm(SS::Dictionary::Types::TWordInfo wordInfo);

	WordForm GetWordMainForm(const wstring &word,
		SS::Core::Features::Types::GenderType genderType,
		SS::Core::Features::Types::NumberType number = SS::Core::Features::Values::NumberType.ntSingle,
        SS::Core::Features::Types::PartOfSpeechTypes partOfSpeechType = SS::Core::Features::Values::PartOfSpeechTypes.Undefined);
	WordForm GetMainFormText1(SS::Interface::Core::BlackBox::IUnit* pUnit,
		SS::Core::Features::Types::GenderType genderType,
		SS::Core::Features::Types::NumberType number = SS::Core::Features::Values::NumberType.ntSingle,
        SS::Core::Features::Types::PartOfSpeechTypes partOfSpeechType = SS::Core::Features::Values::PartOfSpeechTypes.Undefined);

	bool IsPersonName(SS::Interface::Core::BlackBox::IUnit* pUnit);	

	wstring GetCaseForSingle(SS::Interface::Core::BlackBox::IUnit *pUnit);

	void GetRusLNForms(SS::Interface::Core::BlackBox::IUnit *pUnit, SMIList& forms);

	wstring GetRusLNBase(SS::Interface::Core::BlackBox::IUnit *pUnit, SMorphoInfo form);

	/// проверка наличия в списке морфоформ определенных морфохарактеристик
	bool IsMorpho(const SMIList &forms, SMIList *pFilter,
		SS::Core::Features::Types::GenderType isGender = SS::Core::Features::Values::GenderType.Undefined,
		SS::Core::Features::Types::CaseType isCase = SS::Core::Features::Values::CaseType.Undefined,
		SS::Core::Features::Types::NumberType isNumber = SS::Core::Features::Values::NumberType.Undefined,
		SS::Core::Features::Types::AnimInAnimType isAnimation = SS::Core::Features::Values::AnimInAnimType.Undefined);
	bool IsMorpho(const SMIList &forms, SMIList *pFilter, SS::Core::Features::Types::CaseType isCase)
	{
		return IsMorpho(forms, pFilter, SS::Core::Features::Values::GenderType.Undefined, isCase);
	}
	bool IsMorpho(const SMIList &forms, SMIList *pFilter, SS::Core::Features::Types::NumberType isNumber)
	{
		return IsMorpho(forms, pFilter, SS::Core::Features::Values::GenderType.Undefined, SS::Core::Features::Values::CaseType.Undefined, isNumber);
	}
	bool IsMorpho(const SMIList &forms, SMIList *pFilter, SS::Core::Features::Types::AnimInAnimType isAnimation)
	{
		return IsMorpho(forms, pFilter, SS::Core::Features::Values::GenderType.Undefined, SS::Core::Features::Values::CaseType.Undefined,
			SS::Core::Features::Values::NumberType.Undefined, isAnimation);
	}

	void IntersectMorphoInfoLists(SMIList& formsIn1, SMIList& formsIn2, SMIList& formsOut,
		bool genderInter = true, bool caseInter = true, bool numberInter = false);

	bool IsProper(SS::Interface::Core::BlackBox::IUnit *pUnit);

	bool IsAbbr(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool IsParentGeo(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool IsSyntax(SS::Interface::Core::BlackBox::IUnit* pUnit, SS::Core::Features::Types::SyntaxCategories oSyntaxCat);

	/// проверка на неопределенность синтаксической роли
	bool IsUndefinedSyntax(SS::Interface::Core::BlackBox::IUnit* pUnit);

    /// эвристика на порядковые числительные
    /// по непонятной причине у всех числительных есть свойство порядка
    bool isEvristicNumericAdjective(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool isEvristicAdj(SS::Interface::Core::BlackBox::IUnit* pUnit);

	void  GetLinearUnitsWithSort(SS::Interface::Core::BlackBox::IUnit* root,
        list<SS::Interface::Core::BlackBox::IUnit*>& allUnits, bool isFirst = true);

    void GetUnitsInCombiWithSort(SS::Interface::Core::BlackBox::IUnit *pRoot,
        list<SS::Interface::Core::BlackBox::IUnit*> &allUnits);

	bool  IsVisualChild(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool  IsParentInitial(SS::Interface::Core::BlackBox::IUnit* pUnit);	

	bool IsParentGeoCombi(SS::Interface::Core::BlackBox::IUnit* pUnit);

	/// 1 - открывающая кавычка
	/// 0 - не кавычка
	/// -1 - закрывающая кавычка
	/// 2 - и открывающая и закрывающая (" ')
	EQuoteType IsQuote(const wstring &str);

	bool IsParentHavePoint(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool IsProperDict(SS::Interface::Core::BlackBox::IUnit *pUnit);

	bool IsHavePoint(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool IsProperByUpper(SS::Interface::Core::BlackBox::IUnit *pUnit);

	bool IsAnyUpper(const wstring &str);
	bool IsAllUpper(const wstring &str);

	/// Проверка, что юнит - первое слово в предложении
	bool IsFirstSentenceUnit(SS::Interface::Core::BlackBox::IUnit *pUnit);

	/// Проверка, что юнит начинается с большой буквы и это не первое слово в предложении
	bool IsUpperInsideSentence(SS::Interface::Core::BlackBox::IUnit *pUnit);

	bool IsInQuote(SS::Interface::Core::BlackBox::IUnit* pUnit);

	/// Проверка, что юнит - родительский для цепочки в кавычках
	bool IsQuoteParentUnit(SS::Interface::Core::BlackBox::IUnit* pUnit);

    /// является ли юнит геообъектным предлогом
    bool IsGeoPretext(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool IsInCombi(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool IsUnitChild(SS::Interface::Core::BlackBox::IUnit* pParent, SS::Interface::Core::BlackBox::IUnit* pUnit);

	wstring GetFullWord(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool IsEnglishWord(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool IsEnglishWord(const wstring &word);

	bool IsBaseMorhpo(SS::Interface::Core::BlackBox::IUnit* pUnit,
		SS::Core::Features::CMorphoFeature oMorfoAnfr);

	wstring GetVerbNormText(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool IsParentNonVerb(SS::Interface::Core::BlackBox::IUnit* pUnit);

	void SortUnits(TUnitList& allUnits);

	bool GetUnknownAdjNormForm(SS::Interface::Core::BlackBox::IUnit* pUnit);			

	/// персона только в словаре имен
	bool IsPersonOnlyNameDict(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool IsNominalEng(SS::Interface::Core::BlackBox::IUnit *pUnit);

	bool IsNominalDict(wstring str);

	bool IsPersonApostrof(SS::Interface::Core::BlackBox::IUnit* pUnit);

	int GetNominalNounCount(TUnitList& lstUnits);

	int GetSentenceOffset(SS::Interface::Core::BlackBox::ISentence* pSent);

	int GetUnitOffset(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool IsComma(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool IsCombiUnit(SS::Interface::Core::BlackBox::IUnit* pUnit);

	bool IsCombiUnitParent(SS::Interface::Core::BlackBox::IUnit* pUnit);

    int GetDefisCount(const wstring &word);

	void GetAllWordForms(wstring strWord, set<wstring>& lstForms);

	/// проверка юнита на глагол
	bool IsVerbUnit(SS::Interface::Core::BlackBox::IUnit* pUnit);

	/// попытка определения сокращений с дефисом (типа "ф-ла")
	bool UnitPropertiesOperator::IsShortWordForm(SS::Interface::Core::BlackBox::IUnit* pUnit);

    /// проверка на наличие только английских букв
    bool IsOnlyEnglishLetters(SS::Interface::Core::BlackBox::IUnit* pUnit);

    /// проверка на римские цифры в упрощенном варианте: I, V, X
    bool IsRomanNumber(SS::Interface::Core::BlackBox::IUnit* pUnit);
	bool IsRomanNumber(const wstring &str);
	/// проверка на арабское число
	bool IsArabicNumber(const wstring &str);
	/// конвертация римского числа в арабское в упрощенном варианте: : I, V, X
	int ConvertFromRomanNumber(const wstring &number);

	/// нормализация кавычек
    wstring QuoteNormalize(wstring strSrc);
	/// нормализация пробелов
	wstring SpaceNormalize(wstring strSrc);
	/// удаление притяжательных показателей 's и s'
	void RemovePossessiveSign(wstring &str);

	/// возвращает XML-элемент неразмеченного текста
	/// srcText - исходный текст
	/// start - начальное смещение
	/// end - конечное смещение
	wstring GetUnmarkingTextXML(const wstring &srcText, int start, int end);
	wstring Get_CDATA_TextXML(const wstring &srcText, int start, int end);

    /// создание элемента структуры SMorphoInfo из oMorphoFeature
    SMorphoInfo CreateSMorphoInfo(const SS::Core::Features::CMorphoFeature& oMorphoFeature);

	/// Очистка хранилища
	void ClearMorphoInformation() { m_morphoInfo.clear(); }
	/// Добавление информации в хранилище
	void SetMorphoInformation(SS::Interface::Core::BlackBox::IUnit *pUnit, SStorageUnitInfo info);
	/// Чтение сохраненной информации из хранилища
	bool GetMorphoInformation(SS::Interface::Core::BlackBox::IUnit *pUnit,
		SS::Core::Features::CMorphoFeature& resMorphoFeature, SMIList *lstMI);

	/// Возвращает слово в нижнем регистре
	wstring GetLowerWord(const wstring &str);

	void AddUnitsToList(TUnitList &to, const TUnitList &from);

	bool IsNumber(const wstring &str);

	/// возвращает изменяемые части слова через дефис на русском языке
	vector<wstring> GetChangeablePartsByDefis(SS::Interface::Core::BlackBox::IUnit *pUnit);

	bool GetIntersectMorpho(const wstring &leftWord, const wstring &rightWord, SMIList &result,
		wstring *pChangeWord = 0, wstring *pConstWord = 0, bool *pIsAnimation = 0);

	bool GetWordInfo(const wstring &word, std::list<SS::Dictionary::Types::TWordInfo> *oLst, SS::Dictionary::Types::EFindZone zone);

	vector<wstring> GetSetStrings(set<wstring> collection);

	SS::Interface::Core::BlackBox::IUnit* GetUnitByPosition(TUnitList unitList, int position);
};