#pragma once
#include "stdafx.h"

#include <list>
#include <map>
#include "../ASSInterface/IDigestManager.h"
#include "../ASSInterface/IDictionaryManager.h"
#include "../ASSInterface/IAMConverter.h"
#include "../assinterface/iblackbox.h"
#include "../ASSInterface/ISyntaxAnalyzer.h"
#include "../ASSInterface/ISelectSimpleEssence.h"
#include "../../../[libraries]/GnuWin32/include/pcre.h"
#include "CToneDictPool.h"

#include "DictElems.h"
#include "CTextCitation.h"
#include "Entity.h"
#include "HelpFunc.h"
#include "../LexicalAnalyzer/TreeDictionary.h"
#include "CDictionaryPool.h"

using namespace std;
using namespace SS::LexicalAnalyzer::TreeDictionary;

typedef map<wstring, CToneDictPool*> TToneDictMap;

namespace SS
{
	namespace Digest
	{
		struct SentenceInfo
		{
			SS::Interface::Core::BlackBox::ISentence *pSentence;
			TEntityList sentenceEntities;
		};

		typedef list<SentenceInfo> TSentList;
		using namespace SS::Interface::Core;

		///Класс, осуществляющий выделение сущностей из текста
		class CBaseDigestAnalyzer :	public SS::Interface::Core::IDigestAnalyzer
		{	
		private:
            /// флаг загрузки словарей тональности
            bool m_ToneDictLoaded;

			TSentList m_lSentences;
			SS::Interface::Core::Dictionary::IMorphologyAnalyser* m_pMorphoAnalyzer;
			SS::Interface::Core::AMConverter::IAMConverterMorpho* m_pAMConverter;
			SS::Interface::Core::AMConverter::IAMConverterSemantic*	m_pAMConverterSemantic;
			SS::Interface::Core::AMConverter::IAMConverterSyntaxAposteriori* m_pAMConverterSyntaxAposteriori;
			SS::Interface::Core::BlackBox::IBlackBoxTextStorage* m_pBlackBoxTextStorage;
			SS::Interface::Core::CommonServices::IWorkRegister * m_pIWorkRegister;
			SS::Interface::Core::ISyntaxAnalyzer* m_pSyntaxAnalyser;

			wstring wCitataDict;

			// блок шаблонов для цитирования
			SS::Interface::Core::BlackBox::IObjectDescr* citataVerbs;			
			SS::Interface::Core::BlackBox::IObjectDescr* citataSecondTerm;
			SS::Interface::Core::BlackBox::IObjectDescr* citataFirstTerm;
			SS::Interface::Core::BlackBox::IObjectDescr* citataLastTermOne;  // кавычка-запята-тире
			SS::Interface::Core::BlackBox::IObjectDescr* citataLastTermTwo;  // кавычка-точка или кавычка воскл. знак ио кавычка-вопросзнак
			SS::Interface::Core::BlackBox::IObjectDescr* citataLastTermThree;  // кавычка-точка или кавычка воскл. знак ио кавычка-вопросзнак
			SS::Interface::Core::BlackBox::IObjectDescr* citataQuoteTerm;
			SS::Interface::Core::BlackBox::IObjectDescr* citataFirstTermFour1; 
			SS::Interface::Core::BlackBox::IObjectDescr* citataFirstTermFour2; 
			SS::Interface::Core::BlackBox::IObjectDescr* stopListForGen;	
			SS::Interface::Core::BlackBox::IObjectDescr* pronounAnfrThird;
			SS::Interface::Core::BlackBox::IObjectDescr* toneBlockDelimiters;
			SS::Interface::Core::BlackBox::IObjectDescr* specialVerbs; 
			//------------------------------
			//IObjectDescr citata
			void Init();

			list<TCitation*> allCitata;
			list<CDigestCitation*> allDigestCitata;

			TToneDictMap m_ToneDictMap;

			SS::Interface::Core::BlackBox::IObjectDescr* mainObject;

			UnitPropertiesOperator* unitOperator;
			CTreeDictionary* m_pDeletedOrgPrefixes;
			CDictionaryPool* m_pDictionaryPool;

			/// список предлогов, которые могут входить в сущности
			set<wstring> m_entityPretextList;

		public:
			/// Конструктор
			CBaseDigestAnalyzer();
			virtual ~CBaseDigestAnalyzer();
			///Производит выделение дайджеста текста. Предложения, относящиеся к дайджесту, помечаются
			virtual void AnalyzeNextBeforeSynt(SS::Interface::Core::BlackBox::IText* text, list<int>& offsets);
			virtual void AnalyzeNextAfterSynt(SS::Interface::Core::BlackBox::IText* text,
				SS::Interface::Core::BlackBox::IObjectDescr* objectDescr, list<int>& offs);
			///освобождение
			ULONG Release();
			///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
			HRESULT QueryInterface(REFIID pIID, void** pBase);
			///переопределяю от IBase
			void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
			///переопределяю от CAnalyseParams
			//void SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams);
			virtual wstring GetDigestXML();

            /// выделение простых сущностей
			virtual wstring GetEntitiesXML(const wstring& srcText, SS::Interface::Core::BlackBox::IText* text);

			/// юнит требуется пропустить
			bool IsNeedMissUnit(SS::Interface::Core::BlackBox::IUnit* pUnit);

            /// проверка эквивалентности части речи
            bool IsPartOfSpeech(SS::Interface::Core::BlackBox::IUnit* pUnit, SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech);
			/// проверка наличия части речи
			bool IsHavePartOfSpeech(SS::Interface::Core::BlackBox::IUnit* pUnit, SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech);

            virtual void LoadServiceDictionaries(vector<vector<wstring>> &dictionaries);

			virtual void SyntaxAnalyzation(const wstring& srcText, SS::Interface::Core::BlackBox::IText* text,
				const wstring& baseDate, bool languageAnalaze = false);

			virtual wstring GetFactXML(
				const wstring& srcText,
				SS::Interface::Core::BlackBox::IText* text,
				list<int>& offsets, 
				const wstring& baseDate /*YYYY-mm-dd*/,
				bool isTonality = false,
				bool bFullText = false,
				bool bFullAnalyze = true);

			virtual wstring GetSentDigestXML(SS::Interface::Core::BlackBox::IText* pText,
                list<int>& offsets, list<int>& themes, wstring& dictName);

			virtual wstring GetEntityHash(const wstring& srcText, SS::Interface::Core::BlackBox::IText* text);

			virtual EntitiesCollection SelectEntities(const wstring& srcText, SS::Interface::Core::BlackBox::IText* pText, const wstring& baseDate);
			virtual EntitiesCollection SelectSimpleEntities(const wstring& srcText, SS::Interface::Core::BlackBox::IText* pText, bool isSelectOnlyCeterus);
		private:
			void GetSimpleEntities(SS::Interface::Core::BlackBox::IText* pText, bool isSelectOnlyCeterus, TEntityList& allEntity);
			void SelectSimpleDates(SS::Interface::Core::BlackBox::ISentence* pSentence, TEntityList& allEntityInSent, TUnitList& sentUnits);
			bool IsSimpleDate(SS::Interface::Core::BlackBox::IUnit* pUnit);
			void SetNounForUndefined(TUnitList& units);
			/// выделение сущностей из текста
			void GetEntitiesFromText(TEntityList& allEntity, const wstring& srcText, SS::Interface::Core::BlackBox::IText* pText,
				list<int>& offsets, const wstring& baseDate, bool isTonality, bool bFullAnalyze);
			/// выделение простых сущностей на русском языке
            void GetEntitiesXMLFromRusSentence(SS::Interface::Core::BlackBox::ISentence* pSentence, TEntityList& allEntityInSent, TUnitList& sentUnits);
            /// выделение простых сущностей на английском языке
            void GetEntitiesXMLFromEngSentence(SS::Interface::Core::BlackBox::ISentence* pSentence, TEntityList& allEntityInSent, TUnitList& sentUnits);

			/// сохранение морфлогических свойств слов, написанных через дефис
			void SaveDefisWordMorpho(const TUnitList& units);

			void MarkSentByOffset1(SS::Interface::Core::BlackBox::IText* pText, list<int>& offsets, list<int>& themes);

			void CBaseDigestAnalyzer::FindUnits(
				SS::Interface::Core::BlackBox::ISentence* pSen,					
				SS::Core::Features::CMorphoFeature oMorfoAnfr,
				SS::Core::Features::Types::SyntaxCategories oSyntaxFeatureBase,
				SS::Core::Features::Types::TSemanticType semType,			
				int ParentType, int CitataType, 
				SS::Interface::Core::BlackBox::IUnit* edgeUnit,
				TUnitList& noneUnits /* те юниты которые не рассматриваем как разрешения априори */, 
				TUnitList& allUnits);

			void DoAnaphora( SS::Interface::Core::BlackBox::IText* text, bool isFull = true);			 
			void DoPronounMain(SS::Interface::Core::BlackBox::ISentence *curSent, SS::Interface::Core::BlackBox::IUnit* pronoun);
			void DoPronounSecond(SS::Interface::Core::BlackBox::ISentence *sent, SS::Interface::Core::BlackBox::IUnit* pronoun);
			void DoPronounThird(SS::Interface::Core::BlackBox::ISentence *sent, SS::Interface::Core::BlackBox::IUnit* pronoun);

			void DoCitation(SS::Interface::Core::BlackBox::IText* text);
			void DoCitation1(SS::Interface::Core::BlackBox::IText* text, bool isFull=false);
			TCitation* CBaseDigestAnalyzer::CreateCitation(SS::Interface::Core::BlackBox::ISentence *firstSent,
				SS::Interface::Core::BlackBox::ISentence *endSent, SS::Interface::Core::BlackBox::IUnit *subjUnit);
			int EvristicOneStart(SS::Interface::Core::BlackBox::ISentence* pSen, TUnitList& inQuoteList);
			int EvristicTwoStart(SS::Interface::Core::BlackBox::ISentence* pSen, TUnitList& inQuoteList);
			int EvristicThreeStart(SS::Interface::Core::BlackBox::ISentence* pSen, TUnitList& inQuoteList);
			int EvristicFourStart(SS::Interface::Core::BlackBox::ISentence* pSen, TUnitList& inQuoteList);	
			void  SetCitationEnableForSynt(SS::Interface::Core::BlackBox::ISentence* pSent);
			void ProcessCitataSubjectUnit(TCitation* pCitata);
			TCitation* ProcessCitationSubjectForSent(SS::Interface::Core::BlackBox::ISentence* pSent, TCitation* pPrevCit1);
			void ProcessCitationSubject();
			void ProcessCitataSubjectUnitVerb(TCitation* pCitata, TUnitList& sentUnits, SS::Interface::Core::BlackBox::ISentence* pSent);
			void ProcessCitataSubjectUnitVerbExt(TCitation* pCitata, TUnitList& sentUnits,
				SS::Interface::Core::BlackBox::ISentence* pSent, SS::Interface::Core::BlackBox::IUnit* unit);

			void  GetLinearUnitsFull(SS::Interface::Core::BlackBox::ISentence* pSen, TUnitList& allUnits);
			void  AddChildUnits(SS::Interface::Core::BlackBox::IUnit* unit, TUnitList& findedUnits);

			void SetParentTarget(SS::Interface::Core::BlackBox::ISentence *sent,SS::Interface::Core::BlackBox::IUnit* unit);
			bool IsRelevantChildUnits(SS::Interface::Core::BlackBox::IUnit* parent);
			SS::Interface::Core::BlackBox::IUnit* GetMaxParentBySemType(SS::Interface::Core::BlackBox::IUnit* unit);
			
			void LoadDict(wstring dict_file, SS::Interface::Core::BlackBox::IObjectDescr** objDescr, bool isTone);
			void LoadToneDicts();
			void LoadSimpleDictionary(wstring& dict_file, CTreeDictionary** pTreeDict);

			void RoleToneFind(CDigestCitation* dcit, set<SS::Interface::Core::BlackBox::IUnit*>& citataUnits,TUnitList& listCitataUnits);

			void ReleaseSimpleInversions(SS::Interface::Core::BlackBox::ISentence* pSent);
			void ReleaseSimpleInversions1(SS::Interface::Core::BlackBox::ISentence* pSent);

			void FindToneLexicForTargetObject(CDigestCitation* dcit, set<SS::Interface::Core::BlackBox::IUnit*>& citataUnits, TUnitList& listCitataUnits);

			void RecursiveToneUnitAnalyze(SS::Interface::Core::BlackBox::IUnit* pUnit, set<int>& allTermNumbers, int& nToneNeg, int& nTonePos, 
				int& nVerbTone, int& nVerbInvers,
				set<SS::Interface::Core::BlackBox::IUnit*>& verbList,
				set<SS::Interface::Core::BlackBox::IUnit*>& citataUnits,
				int& numOfPretext, TUnitList& negUnits, TUnitList& posUnits);

			void ToneUnitAnalyze(SS::Interface::Core::BlackBox::IUnit* pUnit, set<int>& allTermNumbers, int& nToneNeg, int& nTonePos, 
				int& nVerbTone, int& nVerbInvers,
				set<SS::Interface::Core::BlackBox::IUnit*>& verbList, 
				set<SS::Interface::Core::BlackBox::IUnit*>& citataUnits,
				bool isObjectTree,
				SS::Interface::Core::BlackBox::IUnit* pVerb,	
				TUnitList& nonVerbList);

			void CBaseDigestAnalyzer::ToneUnitAnalyzeVersion2(SS::Interface::Core::BlackBox::IUnit* pUnit,
				set<int>& allTermNumbers, int& nToneNeg, int& nTonePos,TUnitList& listCitataUnits);

			void ToneUnitAnalyzeForNearUnits(
				SS::Interface::Core::BlackBox::ISentence* pSent, SS::Interface::Core::BlackBox::IUnit* pUnit,
				set<int>& allTermNumbers, int& nToneNeg, int& nTonePos, set<SS::Interface::Core::BlackBox::IUnit*>& citataUnits);
			//void ReleaseSimpleInversionOne(TUnitList& findedUnits, SS::Interface::Core::BlackBox::IUnit* pUnit);

			//void ProcessSentUnitsIndexes(ISentence* pSent);

			void  GetAllBranches(SS::Interface::Core::BlackBox::IUnit* pUnit,
				TUnitList& targetUnits, TUnitList& subjUnits, TUnitList& otherUnits);

			bool IsTargetInBranch(SS::Interface::Core::BlackBox::IUnit* pUnit);
			bool IsSubjectInBranch(SS::Interface::Core::BlackBox::IUnit* pUnit);

			void GetGenderForUnit(SS::Interface::Core::BlackBox::IUnit* pUnit, SS::Core::Features::Types::GenderType& resType);

			//void  GetLinearUnits(SS::Interface::Core::BlackBox::IUnit* root, list<SS::Interface::Core::BlackBox::IUnit*>& allUnits);

			wstring GetAllTermsToNominative(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsInversNegativeAllSent(SS::Interface::Core::BlackBox::IUnit* pUnit);

			void DoKoref(SS::Interface::Core::BlackBox::IText* text, SS::Interface::Core::BlackBox::IObjectDescr* objDescr);

			void ReinitToneDicts(wstring& type_name);
			void AddWordToDict(wstring& type_name, int dict_code, string& word);
			void GenerateDictLing(wstring& type_name);

			bool isNonVerbParent(SS::Interface::Core::BlackBox::IUnit* pUnit);

			SS::Interface::Core::BlackBox::IUnit* GetNearestVerb(SS::Interface::Core::BlackBox::IUnit* pUnit, TUnitList& unitList );

			void GetAllUnitsWithoutVerb(TUnitList& sentList, TUnitList& unitList, SS::Interface::Core::BlackBox::IUnit* targetUnit);

			SS::Interface::Core::BlackBox::IUnit* GetMainParentUnit(SS::Interface::Core::BlackBox::IUnit* pUnit);

			void EvrisiticTire(TUnitList& unitList, SS::Interface::Core::BlackBox::IUnit* pUnit, int& posTone, int& negTone);

			bool IsPersonWithInfo(SS::Interface::Core::BlackBox::IUnit* pUnit);

			//void EvristicDelimPersonAndOrg(TEntityList& allEntity);

			bool isEntity(SS::Interface::Core::BlackBox::IUnit* pUnit);

			void GetEntitiesXMLRecursive(SS::Interface::Core::BlackBox::IUnit* pUnit, list<SS::Interface::Core::BlackBox::IUnit*>& entityLst);

            /// получение главного глагола из фразового
            SS::Interface::Core::BlackBox::IUnit* GetPhraseVerbParentUnit(SS::Interface::Core::BlackBox::IUnit* pUnit);

            /// глагол
            bool IsVerbEnt(SS::Interface::Core::BlackBox::IUnit* pUnit);

            /// отглагольное существительное
            bool IsNounVerb(SS::Interface::Core::BlackBox::IUnit* pUnit);

            /// местоимение
            bool IsPronoun(SS::Interface::Core::BlackBox::IUnit* pUnit);

            /// местоименное прилагательное
            bool IsAdjectivePronoun(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool isOrganization(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsAbbr(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsPersonRecursive(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsPerson(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsPlace(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsEvent(SS::Interface::Core::BlackBox::IUnit* pUnit);
			bool IsProduct(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsOther(SS::Interface::Core::BlackBox::IUnit* pUnit);

			/// Защита от выделения заведомо неверных сущностей
			bool IsStop(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsPhone(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsDate(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsAddress(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsSubject(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsNominative(SS::Interface::Core::BlackBox::IUnit* pUnit);

			SS::Interface::Core::BlackBox::IUnit* GetRootVerb(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsTopLevelUnit(SS::Interface::Core::BlackBox::IUnit* pUnit);			  

			//bool FindInUnits(TUnitList& unitList, SS::Interface::Core::BlackBox::IUnit* pObjUnit);

			bool IsPronounMain(SS::Interface::Core::BlackBox::IUnit* pUnit);

			void LexicGenetiveForUnit(SS::Interface::Core::BlackBox::IUnit* pObjUnit, TUnitList& genetiveToneList);			  

			void SetUnFreeUnitRecursive(SS::Interface::Core::BlackBox::IUnit* pUnit);

			void SetUnitFreedomForTone(TUnitList& listCitataUnits);

			bool IsVerb(SS::Interface::Core::BlackBox::IUnit* pUnit);

			void ExtToneLexicProcess(
				TUnitList& listCitataUnits,
				TUnitList::iterator curIterator, int direction,
				set<int>& allTermNumbers, int& nToneNeg, int& nTonePos,
				int& nToneNegVerb, int& nTonePosVerb,
				bool isVerb, bool isExtLexic,TUnitList& findedUnits, bool& isVerbInversion,
				TUnitList& posUnits, TUnitList& negUnits, TUnitList& verbUnits, TUnitList& subjFins);

			void SetTwoPrepositionToneDelete(TUnitList& listCitataUnits);			

			bool IsNominativeAll(SS::Interface::Core::BlackBox::IUnit* pUnit);

			void  ExtLexicGenetiveForUnit(SS::Interface::Core::BlackBox::IUnit* pObjUnit, TUnitList& listCitataUnits, TUnitList& genetiveToneList);

			void SetSentPreDigest(SS::Interface::Core::BlackBox::ISentence* pSent);

			void  MarkSentByOffset(SS::Interface::Core::BlackBox::IText* text, 
				list<int>& offsets, bool isNext = true, bool isPrev = true);

			bool IsPreNoun(SS::Interface::Core::BlackBox::IUnit* pObjUnit, TUnitList& listCitataUnits);

			eEntityBaseType GetEntityType(SS::Interface::Core::BlackBox::IUnit* pUnit);
			eEntityBaseType GetEntityType(SS::Interface::Core::BlackBox::IUnit* pRoot, TUnitList& units, eEntityBaseType rootType);

			eEntityBaseType GetUnitType(SS::Interface::Core::BlackBox::IUnit* pUnit);

			bool IsSameTypeParent(SS::Interface::Core::BlackBox::IUnit* pUnit);

			void SetEntityFullType(CEntity* pEntity, TEntityList& lstEntities);			 

			bool IsProperDict(SS::Interface::Core::BlackBox::IUnit* pUnit);

			CEntity* GetEntityByUnit(TEntityList& lstEntities, SS::Interface::Core::BlackBox::IUnit* pUnit);

			CEntity* GetEntityByUnitPosition(TEntityList& lstEntities, int nPosition);

			void EvristicCDelimiterRus(TEntityList& allEntity);

            /// удаление сущностей, в которых главным существительным является слово, 
            /// которое уже определено как прилагательное
            void EvristicDeleteNounIfAdj(TEntityList& allEntity);

			void EvristicSkobkiDelete(TEntityList& allEntity);

			bool IsEntityListEqualWithPattern(list<EEntityType>& pattern, TEntityList& entityList);

			void EvristicPersonUnitRus(TEntityList& allEntity);

			void EvristicDeletePersonFromOrg(TEntityList& allEntity);

			/// Эвристическая фильтрация геоназваний из юр. лиц
			void EvristicDeletePlaceFromOrg(TEntityList& allEntity);

			/// если одна сущность по тексту внутри другой, но при этом реально ее юниты не включены в другую - надо включить
			/// entParent - родительская сущность
			/// entInside - сущность для включения
			/// retval - произошло ли включение юнитов
			bool IncludeEntityInside(CEntity* entParent, CEntity* entInside);

			/// Проверка, что внутри сущности есть указанный юнит
			bool IsUnitInsideEntity(CEntity* pEntity, SS::Interface::Core::BlackBox::IUnit* pUnit);

			wstring GetEntityNormalString(CEntity* pEnt);

			void ProcessAbbreviation(SS::Interface::Core::BlackBox::IText* pText, TEntityList& lstEntities);

			void ProcessRegion(SS::Interface::Core::BlackBox::IText* pText, TEntityList& lstEntities);

			void FindAbbrevKorefName(SS::Interface::Core::BlackBox::IText* pText, 
				SS::Interface::Core::BlackBox::IUnit* pUnitAbbr, TUnitList& findedList);

			void FindAbbrevInSent(TUnitList& sentList,  SS::Interface::Core::BlackBox::ISentence* pSent,
				SS::Interface::Core::BlackBox::IUnit* pUnitAbbr, TUnitList& findedList);

			bool IsGoodForAbbr(TUnitList& findedUnits);

            /// Переклассификация первого слова предложения
            void UnitReclassificationOnEnglish(TUnitList& sentList, eEntityBaseType& type);

			void EvristicPersonOrgOrPlaceUnion(TEntityList& allEntity);

			SS::Interface::Core::BlackBox::IUnit* GetNextUnit(SS::Interface::Core::BlackBox::ISentence* pSent, int position);

			void FindRootRegions(TEntityList& allEntity, SS::Interface::Core::BlackBox::ISentence* pSent);

			void ProcessRegion(SS::Interface::Core::BlackBox::ISentence* sent, TUnitList& linkedPlaces);

			void EvristicFindNextDates(TEntityList& lstEntities,TUnitList& sentList);
			void EvristicFrom_ToDates(TEntityList& lstEntities, TUnitList& sentList);

			void GetInsideEntities(CEntity* inEnt, TEntityList& lstAll, TEntityList &lstInside);

			wstring GetXMLForEntityText(const wstring& srcText, CEntity *inEnt,
				list<CEntity*>& insideEnt, int basic_offset, int sentOffset);

			void EvristicChangeDateToPlace(TEntityList& lstEntities);

			// если организация - строка в кавчках и у нее есть родительский элемент существительнеое БЕЗ
			// семантики организации, то это не организация
			void EvristicDelBadOrg(TEntityList& lstEntities,TUnitList& sentList);	
			void EvristicDelBadPerson(TEntityList& lstEntities);	
			void EvristicDelBadProper(TEntityList& lstEntities);	
			void EvristicHyphenDates(TEntityList& lstEntities, TUnitList& sentList);
			void EvristicOrgGeoAbbrUnion(TEntityList& allEntity);
			void EvristicOgrNoInfoPersonrUnion(TEntityList& allEntity);
			//void EvristicFilterPlace(TEntityList lstEntities);
			void  EvristicGeoWithoutPredlogToOrg(TEntityList& lstEntities, TUnitList& sentList);

			bool MarkTone(SS::Interface::Core::BlackBox::ISentence* pSent, wstring& dictName);

			wstring GetXMLForSent(SS::Interface::Core::BlackBox::ISentence* pSent, int id, int nCitType, bool isLast);

			bool IsEntTypeParent(TEntityList& allEntity, CEntity* curEnt);

			void EvristicPersonInfoInsidePersonName(TEntityList& allEntities, TUnitList& sentList);
			void EvristicPersonNameAndRomanNumberUnion(TEntityList &allEntities, TUnitList& unitList);

			bool IsBadOrgPrefixWord(SS::Interface::Core::BlackBox::IUnit* pUnit);

			void EvristicDeleteOrgFromPerson(TEntityList& allEntity);

			/*void ProcessKorefs(SS::Interface::Core::BlackBox::IText* pText, TEntityList lstEntities);*/

			void EvristicRegionDelim(TEntityList& allEntity);

			void GetEntityForSent(SS::Interface::Core::BlackBox::ISentence* sent, TEntityList& allEntityInSent,
				TUnitList& linkedPlaces, const wstring& baseDate);

			void SelectEntitiesWithSyntax(TUnitList &units, TEntityList& allEntityInSent, const wstring& baseDate, SS::Core::Types::EAnalyseLanguage language);

			bool IsSameTypeParentEntity(CEntity* pEnt, TEntityList& allEntityInSent);

			wstring GetFactXML(const wstring& srcText, SS::Interface::Core::BlackBox::IText* pText,
				bool bFullText, bool bFullAnalyze, TEntityList& allEntity);

			/// корректировка дерева сущностей
			void EntityTreeCorrection(TEntityList& allEntity);

            /// нормализация списка сущностей
            void EntityNormalization(TEntityList& allEntity);

            /// нормализация сущности
            void EntityNormalization(CEntity* pEntity, TEntityList& allEntity);

            void EvristicEntityUnionByPretext(TEntityList& allEntity);

			void EvristicHomogeneous(TEntityList& allEntities, TUnitList& sentList, bool bSemanticNeeded);

			void EvristicHomogeneousAdj(TEntityList& lstAllEntities, TUnitList& lstSentList);

			void  GetEntityParentList(TEntityList& lstAllEntity, CEntity* pTargetEnt, TEntityList& lstParent);

			CEntity* CreateNewEntity(EEntityType entType, eEntityBaseType nType, 
                SS::Interface::Core::BlackBox::IUnit* pRootUnit,
				SS::Interface::Core::BlackBox::ISentence* pSen, TUnitList& lstUnits);

			void CBaseDigestAnalyzer::CopyHomogeneousUnits(TEntityList& lstAllEntities, 
				CEntity* pRootEnt, TUnitList& lstAllHomogeneousUnits, bool isIncludeCurrent);

			void EvristicDeleteBadDates(TEntityList& lstAllEntities);

			/// удаление сущностей, содержащих сокращенные слова, записанные через дефис ("ф-ла")
			void EvristicDeleteShortWordForm(TEntityList& allEntity);

            /// преобразование ненормализованных дат в Unknown или UnknownProper
            void EvristicConvertBadDates(TEntityList& allEntity);

            /// выделение прочих типов сущностей
            void EvristicSelectCeterus(SS::Interface::Core::BlackBox::ISentence* pSen, TEntityList& allEntity, TUnitList& lstSentList);

			/// деление сущности по местоимениям
			void EvristicCutPronounUnits(CEntity* pEntity, TEntityList& allEntity, SS::Interface::Core::BlackBox::ISentence* pSen);

            /// обновление карты неразмеченных юнитов
            void UpdateFreeUnitsMap(CEntity* pEntity, vector<bool>& map);

			void EvristicPostHomogeneousGenetive(TEntityList& lstAllEntities, TUnitList& lstSentList);

			void GenerateSubentitiesWithoutRightUnits(CEntity* pEntity, TEntityList& lstAllEntityInSent);

			void GenerateSubentitiesByHomogeneous(CEntity* pEntity, TEntityList& lstAllEntityInSent);

            /// генерация подсущностей последовательным отсечением прилагательных слева
			void GenerateSubEntWithoutLeftAdjectives(CEntity* pEntity, TEntityList& inside, CEntity* pParentEntity);

            void NumberingSentenceUnits(TUnitList& sentUnits);

            /// дополнительный поиск объектов в предложении
            void AdditionalObjectSearch(
                SS::Interface::Core::BlackBox::IObjectDescr* objDescr,
                SS::Interface::Core::BlackBox::ISentence* pSent,
                TUnitList& findedUnits, const list<int>& offsets);

			/// Отсеиваются морфохарактеристики, часть речи которых не совпадает с указанной
			void FilterMorphoInfo(SMIList* pLstMI, SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech);

			void GetVerbUnitList(SS::Interface::Core::BlackBox::IUnit* pRoot, TUnitList& unitList);

			void InitializeEntitiesSelection();

			EntitiesCollection ConvertEntities(TEntityList& entities, const wstring& srcText);
				SS::Core::Types::CTextEntity ConvertEntity(CEntity* pSourceEntity, const wstring& srcText);
		};				
	}		
}
