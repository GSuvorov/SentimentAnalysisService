#include "stdafx.h"
#include "IndexLoader.h"

using namespace SS::Dictionary;
using namespace SS::Dictionary::DATA_TYPE::NAMES;

#include "..\ASSInterface\ICoSeDi.h"
#include "..\ASSInterface\TCoSeDiDataTypes.h"
#include "..\[libs]\_Paths.h"

#define FILL_UNIT_INFO_BY_COMBINATORY(pUnit, pInfo, bMorpho, bNum) \
	bNum ? FillUnitInfoByNumberCombinatory(pUnit, pInfo, bMorpho) : FillUnitInfoByCombinatory(pUnit, pInfo, bMorpho)


namespace SS
{
	namespace LexicalAnalyzer
	{

		/////////////////////////////////////////////////////////////////////////////////////
		// Construction/Destruction
		/////////////////////////////////////////////////////////////////////////////////////

		CIndexLoader::CIndexLoader() :
	m_pMorphoAnalyzer(NULL),
		m_pSemantic(NULL),
		m_pSynonims(NULL),
		m_pStorage(NULL),
		m_pCombinatory(NULL),
		m_pNumCombinatory(NULL),
		m_bMorpho(false),
		m_bSemantic(false),
		m_bSynonims(false),
		m_bLite(false),
		m_bQuery(false),
		m_bUseQueryDeformer(false),
		m_pUnitStorage(NULL),
		m_pUnitChecker(NULL),
		m_pIWorkRegister(NULL),
		m_pSemanticSuffixesDict(NULL),
		m_pMorphoConverter(NULL)
	{
		m_vIDSources.reserve(MAX_WORDS_IN_SENTENCE);
	}

	CIndexLoader::~CIndexLoader()
	{
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// CIndexLoader interface

	void CIndexLoader::Init(ILoadManager* pLoadManager, IBlackBoxTextStorage* pStorage, 
		IAnalyseParams* pAnalyseParams, IR::TUnitStorage* pUnitStorage, 
		CUnitChecker* pUnitChecker)
	{
		SS_TRY;

		// init internal variables
		m_pUnitChecker = pUnitChecker;
		m_pUnitStorage = pUnitStorage;
		m_pStorage = pStorage;
		if(pAnalyseParams)
		{
			m_bMorpho = pAnalyseParams->GetAnalyseDepthParams()->IsMorpho();
			m_bSemantic = pAnalyseParams->GetAnalyseDepthParams()->IsSemantic();
			m_bSynonims = pAnalyseParams->GetAnalyseDepthParams()->IsSynonims();

			m_bQuery =	pAnalyseParams->GetAnalyseMod() == eamQuery ||
				pAnalyseParams->GetAnalyseMod() == eamRejectUnimportant;

			m_bLite = pAnalyseParams->m_bIsLiteMode;
		}

		// morphology analyzer
		if(!m_pMorphoAnalyzer)
		{
			IDictionary* pDict = (IDictionary*) 
				pLoadManager->GetInterface( DICTIONARIES_DLL_PATH, CLSID_IDictionary, IID_IDictionary);
			m_pMorphoAnalyzer = pDict->GetMorphologyAnalyser();
			m_pMorphoAnalyzer->SetAnalyseParams(pAnalyseParams);

		}

		if (!m_pMorphoConverter)
		{
			m_pMorphoConverter = (IAMConverterMorpho*)
				pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);
		}

		// CoSeDi
		ICoSeDiManager* pCoSeDiManager = (ICoSeDiManager*) pLoadManager->GetInterface(L"./core/linguistics/CoSeDi.dll", 
			CLSID_ICoSeDiManager, IID_ICoSeDiManager);
		if(!pCoSeDiManager)
			SS_THROW(L"Failed to load pCoSeDiManager");

		if(!pCoSeDiManager->GetLoader()->Load())
			SS_THROW(L"ICoSeDi::Load() failed");

		// semantic
		if(m_bSemantic)
		{
			m_pSemantic = pCoSeDiManager->GetSemanticAnalysis();
			if(!m_pSemantic)
				SS_THROW(L"Failed to load m_pSemantic");
		}

		// synonims
		if(m_bSynonims)
		{
			m_pSynonims = pCoSeDiManager->GetSynonimsSemanticAnalysis();
			if(!m_pSynonims)
				SS_THROW(L"Failed to load m_pSynonims");
		}

		// combinatory
		if(!m_pCombinatory)
		{
			m_pCombinatory = pCoSeDiManager->GetCombinatory();
			if(!m_pCombinatory)
				SS_THROW(L"Failed to load m_pCombinatory");
		}

		if (!m_pIWorkRegister)
		{
			m_pIWorkRegister = 
				(SS::Interface::Core::CommonServices::IWorkRegister*)
				pLoadManager->GetInterface(L"./core/LoadManager.dll", CLSID_WorkRegister, IID_WorkRegister);

		}

		if (!m_pSemanticSuffixesDict)
		{
			LoadSuffixesSemanticDict();
		}

		// number combinatory
		m_pNumCombinatory = (INumberCombinatory*) pLoadManager->GetInterface( DICTIONARIES_DLL_PATH, CLSID_INumberCombinatory, IID_INumberCombinatory);
		if(!m_pNumCombinatory)
			SS_THROW(L"Failed to create INumberCombinatory");

		m_pNumCombinatory->SetLoadManager(pLoadManager);

		SS_CATCH(L"");
	}

	void CIndexLoader::SetUseQueryDeformer(bool bSet)
	{
		m_bUseQueryDeformer = bSet;
	}

	void CIndexLoader::NextText()
	{
		SS_TRY;

		if(m_pMorphoAnalyzer)
			m_pMorphoAnalyzer->NextText();

		SS_CATCH(L"");
	}

	struct SMorphoComparer
	{
		bool operator()(const TWordInfo& _Left, const TWordInfo& _Right) const
		{
			return _Left.begin()->m_WordIndex.m_DictIndex.GetDictionaryIndex() > _Right.begin()->m_WordIndex.m_DictIndex.GetDictionaryIndex();
		}
	};

	bool CIndexLoader::FillMorphoInfo(IUnit* pUnit, LPCTSTR szWord, bool bOnlySearch)
	{
		if(!m_bMorpho)
			return false;

		SS_TRY;

		m_lMorpho.clear();

		USES_CONVERSION;
		TCHAR szText[MAX_CHARS_IN_WORD];
		_tcsncpy(szText, szWord, MAX_CHARS_IN_WORD - 1);
		szText[MAX_CHARS_IN_WORD - 1] = 0;
		m_pMorphoAnalyzer->GetWordInfo(T2CW(szText), &m_lMorpho, (bOnlySearch || m_bQuery) ? efzOnlySearch : efzFullAddNewWords/*efzOnlySearch*/);
		if(m_lMorpho.empty() || bOnlySearch)
			return !m_lMorpho.empty();

		stable_sort(m_lMorpho.begin(), m_lMorpho.end(), SMorphoComparer());

		IIndex* pIndex = NULL;
		for(std::list<TWordInfo>::iterator it = m_lMorpho.begin(); it != m_lMorpho.end(); ++it)
		{
			SWordInfo* pwi = &(it->front());

			IFeature* pFeature = m_pStorage->CreateFeature();
			pFeature->SetMorphoIndex(pwi->m_WordIndex.m_IDForm);
			pFeature->SetMorphoInfo(pwi->m_MorphoTypeInfo.m_MorphoInfo);
			pFeature->SetPartOfSpeech(pwi->m_SpecialMorpho);

			// if we have the same source id as in previous cycle, use previous pIndex
			long idSource = pwi->m_WordIndex.m_DictIndex.GetDictionaryIndex();
			if(!pIndex || pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex() != idSource)
			{
				if(!idSource)
				{
					SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"»з базы получен нулевой морфологический индекс" AND 
						pUnit->GetWord());
					continue;
				}

				pIndex = m_pStorage->CreateIndex();
				pIndex->GetDictionaryIndex()->AppendIndex(idSource);
				pUnit->AddIndex(pIndex);
			}

			pIndex->AddFeature(pFeature);
		}

		SS_CATCH(L"Failed on word " AND szWord);
		return true;
	}

	// прикручиваем всем неизвестным существительным с н
	void CIndexLoader::FillSemanticVERB(IUnit* pUnit)
	{
		for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			EIndexType eit = pIndex->GetDictionaryIndex()->GetFirst().GetIndexType();
			SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber dictNum = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber();
			if (eit == eitSemanticIndex)
			{
				return; // уже есть семантический 
			}
		}

		for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			EIndexType eit = pIndex->GetDictionaryIndex()->GetFirst().GetIndexType();
			SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber dictNum = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber();
			if (eit == eitMorphoIndex /*&& ((dictNum==SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus ||
									  dictNum==SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng))*/)
			{
				// надо определить сузествительное или нет
				// если да, то нашли
				for(IFeature * pFeature = pIndex->GetFirstFeature();pFeature; pFeature = pIndex->GetNextFeature())
				{
					SS::Core::Features::CMorphoFeature oMorphoFeature;
					int nPartOfSpeech = pFeature->GetPartOfSpeech();
					SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex = pIndex->GetDictionaryIndex()->GetFirst();
					m_pMorphoConverter->EnCode(oDictionaryIndex.GetDictionaryNumber(), 
						pFeature->GetMorphoInfo(),
						pFeature->GetPartOfSpeech(), &oMorphoFeature);	

					if (oMorphoFeature.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postNoun))
						//if (nPartOfSpeech==3) // существ.
					{
						SS::Core::Features::Types::TSemanticTypeType ts;			
						for(unsigned int i = 0; i < ts.GetValueCount(); ++i)
						{
							if (strcmp(ts.GetValueName(i), "smtVerb") == 0)
							{
								SS::Dictionary::Types::TDictionaryIndex  oSemanticsClass(i, SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);
								IIndex* pNewIndex = m_pStorage->CreateIndex();
								pNewIndex->GetDictionaryIndex()->AppendIndex(oSemanticsClass);
								pUnit->AddIndex(pNewIndex);
								return;
							}
						}						
					}
				}

			}
		}
	}

	void CIndexLoader::FillSemanticInfoBySuffix(IUnit* pUnit)
	{
		bool isFind = false;
		vector<string> types;
		for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			EIndexType eit = pIndex->GetDictionaryIndex()->GetFirst().GetIndexType();
			SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber dictNum = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber();
			if(pIndex->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase() &&
				(eit == eitMorphoIndex || eit == eitCombinatoryIndex) && 
				(dictNum==SS::Dictionary::DATA_TYPE::NAMES::ednUnknownRus ||
				dictNum==SS::Dictionary::DATA_TYPE::NAMES::ednRProperName ||
				dictNum==SS::Dictionary::DATA_TYPE::NAMES::ednUnknownEng  ||
				dictNum==SS::Dictionary::DATA_TYPE::NAMES::ednEProperName
				))
			{
				// неизвестное слово
				wstring word = pUnit->GetWord();
				word = word.substr(0, SS::Constants::MAX_CHARS_IN_WORD - 1);
				wstring strHi = word;
				_wcsupr_s((wchar_t*)strHi.c_str(), strHi.length() + 1);
				m_pSemanticSuffixesDict->FindWord(strHi, types);	
				if (types.size()>0)
				{
					// надо создать семантические индексы соответсвующие найденным индекс
					int i=0;
					isFind = true;				
				}
				else
				{
					std::list<SS::Dictionary::Types::TWordInfo> WIList;
					std::list<SS::Dictionary::Types::TWordInfo> lWordInfoDetect;
					m_pMorphoAnalyzer->GetWordInfo(word.c_str(), &WIList);
					for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = WIList.begin(); it != WIList.end(); ++it)
					{
						SS::Dictionary::Types::SWordInfo* pwi = &(it->front());
						m_pMorphoAnalyzer->GetWordFormsByIDSource(&(pwi->m_WordIndex.m_DictIndex), &lWordInfoDetect);
						for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it1 = lWordInfoDetect.begin(); it1 != lWordInfoDetect.end(); ++it1)
						{							
							wstring word = it1->front().m_wWord;
							word = word.substr(0, SS::Constants::MAX_CHARS_IN_WORD - 1);
							wstring strHi = word;
							_wcsupr_s((wchar_t*)strHi.c_str(), strHi.length() + 1);
							m_pSemanticSuffixesDict->FindWord(strHi, types);	
							if (!types.empty())
							{
								// надо создать семантические индексы соответсвующие найденным индекс
								int i=0;
								isFind = true;				
								break;
							}
						}			
						if (isFind) break;
					}
				}//if (types.size()>0)
			}		// if((eit == eitMorphoIndex	
			if (isFind)
			{
				// нашли
				for(unsigned int ti = 0; ti< types.size(); ti++)
				{
					SS::Core::Features::Types::TSemanticTypeType ts;
					SS::Dictionary::Types::TDictionaryIndex tInd;
					string s = types[ti];
					for(unsigned int i=0;i<ts.GetValueCount(); i++)
					{
						if (ts.GetValueName(i)==s)
						{
							SS::Dictionary::Types::TDictionaryIndex  oSemanticsClass(i, SS::Dictionary::DATA_TYPE::NAMES::ednSemantic);

							IIndex* pNewIndex = m_pStorage->CreateIndex();					
							pNewIndex->GetDictionaryIndex()->AppendIndex(oSemanticsClass);
							//pUnit->AddSemanticIndex(pIndex, pNewIndex);
							pUnit->AddIndex(pNewIndex);
						}
					}						
				}
				break;
			}
		}	//for					
	}

	void CIndexLoader::FillSemanticInfo(IUnit* pUnit)
	{
		if(!m_bSemantic)
			return;

		SS_TRY;

		//по всем морфоиндексам
		for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			TDictionaryIndex dictIndex = pIndex->GetDictionaryIndex()->GetFirst();
			EIndexType eit = dictIndex.GetIndexType();
			if(eit == eitMorphoIndex || eit == eitCombinatoryIndex)
			{
				m_stIndexes.clear();
				FillSemanticInfo(pIndex, &m_stIndexes);

				//по всем семантическим индексам, полученным дл€ данного морфоиндекса
				for(TIndexSet::iterator it = m_stIndexes.begin(); it != m_stIndexes.end(); ++it)
				{
					if(it->GetId() == 0)
					{
						SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"»з базы получен нулевой семантический индекс" AND 
							pUnit->GetWord());
						continue;
					}

					IIndex* pNewIndex = m_pStorage->CreateIndex();
					pNewIndex->GetDictionaryIndex()->AppendIndex(*it);
					//pUnit->AddSemanticIndex(pIndex, pNewIndex);
					
					pUnit->AddIndex(pNewIndex);
				}
			}
		}

		SS_CATCH(L"Failed on word " AND pUnit->GetWord());
	}

	void CIndexLoader::FillSynonims(IUnit* pUnit)
	{
		SS_TRY;

		m_stIndexes.clear();
		IIndex* pIndex = NULL;
		for(pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			EIndexType eit = pIndex->GetDictionaryIndex()->GetFirst().GetIndexType();
			if(eit == eitMorphoIndex || eit == eitCombinatoryIndex)
				FillSynonims(pIndex, &m_stIndexes);
		}

		if(m_bLite && m_bQuery)
		{
			TIndexSet stMorpho;
			for(pIndex = pUnit->GetFirstIndex(); pIndex; )
			{
				stMorpho.insert(pIndex->GetDictionaryIndex()->GetFirst());
				pIndex = pIndex->ReleaseIndex();
			}
			for(TIndexSet::iterator it = m_stIndexes.begin(); it != m_stIndexes.end(); ++it)
			{
				m_lSem.clear();
				m_pSynonims->SetClass(*it);
				m_pSynonims->GetWords(&m_lSem);
				stMorpho.insert(m_lSem.begin(), m_lSem.end());
			}
			for(TIndexSet::iterator it = stMorpho.begin(); it != stMorpho.end(); ++it)
			{
				IIndex* pNewIndex = m_pStorage->CreateIndex();
				pNewIndex->GetDictionaryIndex()->AppendIndex(*it);
				pUnit->AddIndex(pNewIndex);
			}
			return;
		}

		for(TIndexSet::iterator it = m_stIndexes.begin(); it != m_stIndexes.end(); ++it)
		{
			if(it->GetId() == 0)
			{
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"»з базы получен нулевой индекс синонимов" AND pUnit->GetWord());
				continue;
			}

			IIndex* pNewIndex = m_pStorage->CreateIndex();
			pNewIndex->GetDictionaryIndex()->AppendIndex(*it);
			pUnit->AddIndex(pNewIndex);
		}

		SS_CATCH(L"");
	}

	void CIndexLoader::FillWord(IUnit* pUnit)
	{
		SS_TRY;

		SWordInfo oInfo;
		SS::Dictionary::Types::SWordIndex oIndex;
		for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			for(IFeature* pFeature = pIndex->GetFirstFeature(); pFeature; pFeature = pIndex->GetNextFeature())
			{
				oIndex.m_IDForm = pFeature->GetMorphoIndex();
				oIndex.m_DictIndex = pIndex->GetDictionaryIndex()->GetFirst();

				if(!m_pMorphoAnalyzer->GetWordInfo(&oIndex, &oInfo))
					continue;

				pFeature->SetMorphoInfo(oInfo.m_MorphoTypeInfo.m_MorphoInfo);
				pFeature->SetPartOfSpeech(oInfo.m_SpecialMorpho);
				if(!*pUnit->GetWord())
					pUnit->SetWord(oInfo.m_wWord.c_str());
			}
		}

		SS_CATCH(L"");
	}

	void CIndexLoader::FillCombinatoryInfo(IR::CSentence* pSent)
	{
		SS_TRY;

		if(pSent->GetUnitCount() <= 1)
			return;

		// get cobinatory information list
		m_lCombiInfos.clear();
		FillCombiInfoList(pSent, m_lCombiInfos);

		// apply combinations to pSent
		ApplyCombinationList(&m_lCombiInfos, pSent, false);

		SS_CATCH(L"");
	}

	void CIndexLoader::FillNumberCombinatory(IR::CSentence* pSent)
	{
		SS_TRY;

		// fill id source vector
		bool bZero = false;

		int iPos = 0;
		for(IR::CUnit* pUnit = pSent->GetFirstUnit(); pUnit; pUnit = pSent->GetNextUnit(), iPos++)
		{
			// prevent zero sequences from parsing
			if(pUnit->GetTag() != 0)
			{
				// if previous unit had zero tag, but zero must not be first item of m_vIDSources
				if(bZero && !m_vIDSources.empty())
					m_vIDSources.push_back(SWordPos(0, iPos));
				m_vIDSources.push_back(SWordPos(pUnit->GetTag(), iPos));
			}

			bZero = (pUnit->GetTag() == 0);
		}

		if(!m_vIDSources.empty())
		{
			// get combinations info
			m_lCombiInfos.clear();
			m_pNumCombinatory->GetCombinationsInfo(&m_vIDSources, &m_lCombiInfos);
			m_vIDSources.erase(m_vIDSources.begin(), m_vIDSources.end());

			// apply combinations to pSent
			ApplyCombinationList(&m_lCombiInfos, pSent, true);
		}

		SS_CATCH(L"");
	}

	bool CIndexLoader::FillMorphoInfoToComplexNumber(IUnit* pParentUnit)
	{
		SS_TRY;

		unsigned __int64 uiResult = 0;

		int iPos = 0;
		for(IUnit* pUnit = pParentUnit->GetLeftChildUnit(); pUnit; pUnit = pUnit->GetRightUnit(), iPos++)
		{
			UINT uiVal = GetNumericValue(pUnit);
			if(uiVal != -1)
				uiResult = uiResult * 1000 + uiVal;
		}

		if(uiResult < MAX_NUMERIC_VALUE)
		{
			TCHAR sz[0x10] = { 0 };
			_stprintf_s(sz, 0x10, _T("%d"), (int) uiResult);
			FillMorphoInfo(pParentUnit, sz);
			return true;
		}

		SS_CATCH(L"");
		return false;
	}

	UINT CIndexLoader::GetNumericValue(IUnit* pUnit)
	{
		for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			DictionariesNumber edn = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber();
			if(edn == ednNumeric)
				return pIndex->GetDictionaryIndex()->GetFirst().GetId();
			else if(edn == ednFloatPointNumeric)
				return pIndex->GetDictionaryIndex()->GetFirst().GetId() & 0x001fffff;
		}

		return -1;
	}

	void CIndexLoader::CopySemanticsFromChildren(IUnit* pParentUnit)
	{
		SS_TRY;

		if(!m_bSemantic)
			return;

		m_stIndexes.clear();
		for(IUnit* pUnit = pParentUnit->GetLeftChildUnit(); pUnit; pUnit = pUnit->GetRightUnit())
		{
			for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
			{
				if(pIndex->GetDictionaryIndex()->GetFirst().GetIndexType() == eitSemanticIndex)
					m_stIndexes.insert(pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndex());
			}
		}

		for(TIndexSet::iterator it = m_stIndexes.begin(); it != m_stIndexes.end(); ++it)
		{
			IIndex* pNewIndex = m_pStorage->CreateIndex();
			pNewIndex->GetDictionaryIndex()->AppendIndex(*it);
			pParentUnit->AddIndex(pNewIndex);
		}

		SS_CATCH(L"");
	}

	void CIndexLoader::FillQueryWordSet(TIndexSet* pSet)
	{
		SS_TRY;

		static const LPCWSTR rszQueryWordsListName[] = { 
			L"list", L"name", L"what", L"who", L"why", 
			L"when", L"where", L"how", L"which", 
			L"что", L"кто", L"каков", L"какой", L"который", 
			L"чей", L"сколько", L"насколько", L"как", L"где", 
			L"когда", L"куда", L"откуда", L"почему", L"отчего", L"зачем", NULL };
			static const LPCWSTR rszQueryWords[] = { 
				L"what", L"who", L"why", L"when", L"where", L"how", L"which", 
				L"что", L"кто", L"каков", L"какой", L"который", 
				L"чей", L"сколько", L"насколько", L"как", L"где", 
				L"когда", L"куда", L"откуда", L"почему", L"отчего", L"зачем", NULL };
				const LPCWSTR* pszWords = m_bUseQueryDeformer ? rszQueryWordsListName : rszQueryWords;

				pSet->clear();
				for(int i = 0; pszWords[i]; i++)
				{
					UINT idSource = GetQueryWordSourceID(pszWords[i]);
					if(idSource == 0)
						SS_THROW(L"Ќевозможно определить основу вопросительного слова");
					pSet->insert(idSource);
				}

				SS_CATCH(L"");
	}

	struct SLocalUnit
	{
		SLocalUnit(IUnit* p) : pUnit(p)
		{
		}
		~SLocalUnit()
		{
			pUnit->ReleaseUnitHorizontal();
		}
		operator IUnit*()
		{
			return pUnit;
		}
		IUnit* operator->()
		{
			return pUnit;
		}
		IUnit* pUnit;
	};

	UINT CIndexLoader::GetQueryWordSourceID(LPCTSTR sz)
	{
		SS_TRY;

		SLocalUnit pUnit(m_pStorage->CreateUnit());
		if(!FillMorphoInfo(pUnit, sz))
			return 0;

		if(!m_bLite)
		{
			for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
			{
				if(m_pUnitChecker->GetPartOfSpeech(pIndex).IsContain(Values::PartOfSpeechTypes.postPronoun))
					return pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndexWithoutFlags();
			}

			for(IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
			{
				if(m_pUnitChecker->GetPartOfSpeech(pIndex).IsContain(Values::PartOfSpeechTypes.postAdverb))
					return pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndexWithoutFlags();
			}
		}

		IIndex* p = pUnit->GetFirstIndex();
		if(!p)
			return 0;
		return p->GetDictionaryIndex()->GetFirst().GetDictionaryIndexWithoutFlags();

		SS_CATCH(L"");
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// CIndexLoader functions

	void CIndexLoader::FillSemanticInfo(IIndex* pIndex, TIndexSet* pstIndexes, IUnit* pUnit)
	{
		SS_TRY;

		m_lSem.clear();
		m_pSemantic->GetClasses(pIndex->GetDictionaryIndex()->GetFirst(), &m_lSem);

		if(pUnit)
		{
			for(list<TDictionaryIndex>::iterator it = m_lSem.begin(); it != m_lSem.end(); ++it)
			{
				if(it->GetId() == 0)
				{
					SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"»з базы получен нулевой семантический индекс" AND 
						pUnit->GetWord());
					continue;
				}

				IIndex* pNewIndex = m_pStorage->CreateIndex();
				pNewIndex->GetDictionaryIndex()->AppendIndex(*it);
				pUnit->AddIndex(pNewIndex);
			}
		}
		else
		{
			for(list<TDictionaryIndex>::iterator it = m_lSem.begin(); it != m_lSem.end(); ++it)
			{
				TDictionaryIndex dctInd = *it;
				pstIndexes->insert(*it);
			}
		}

		SS_CATCH(L"");
	}

	void CIndexLoader::FillSynonims(IIndex* pIndex, TIndexSet* pstIndexes)
	{
		SS_TRY;

		m_lSem.clear();
		m_pSynonims->GetClasses(pIndex->GetDictionaryIndex()->GetFirst(), &m_lSem);
		for(list<TDictionaryIndex>::iterator it = m_lSem.begin(); it != m_lSem.end(); ++it)
			pstIndexes->insert(*it);

		SS_CATCH(L"");
	}

	void CIndexLoader::FillCombiInfoList(IR::CSentence* pSent, list<SCombinationNewInfo>& lCombiInfos)
	{
		SS_TRY;

		SSourceAndForm               oSourceAndForm;
		TSetOfSourcesAndForms      * pSetOfSourcesAndForms;

		m_stSorucesAndForms.clear();
		m_lSetsOfSourcesAndForms.clear();

		// fill list of ID sources
		for(IR::CUnit* pUnit = pSent->GetFirstUnit(); pUnit; pUnit = pSent->GetNextUnit())
		{
			m_lSetsOfSourcesAndForms.push_back(m_stSorucesAndForms);
			pSetOfSourcesAndForms = &(m_lSetsOfSourcesAndForms.back());

			for(IIndex* pIndex = pUnit->GetUnit()->GetFirstIndex(); pIndex; pIndex = pUnit->GetUnit()->GetNextIndex())
			{
				DictionariesNumber edn = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryNumber();
				switch(edn)
				{
				case ednSymbols:
				case ednEProperName:
				case ednENominalName:
				case ednRProperName:
				case ednRNominalName:
					for(IFeature* pFeature = pIndex->GetFirstFeature(); pFeature; pFeature = pIndex->GetNextFeature())
					{
						oSourceAndForm.IDSource = pIndex->GetDictionaryIndex()->GetFirst().GetDictionaryIndexWithoutFlags();
						oSourceAndForm.IDForm   = pFeature->GetMorphoIndex();
						pSetOfSourcesAndForms->insert(oSourceAndForm);
					}
					break;
				}
			}
		}

		// fill combinatory info
		m_pCombinatory->GetCombinationsInfo(m_lSetsOfSourcesAndForms.begin(), m_lSetsOfSourcesAndForms.end(), &lCombiInfos);

		SS_CATCH(L"");
	}

	struct SCombiComparer
	{
		bool BigType(int iCombiType) const
		{
			return iCombiType == ctNewLexicalUnit || iCombiType == ctComplexWord;
		}

		bool operator()(const SCombinationNewInfo& _Left, const SCombinationNewInfo& _Right) const
		{
			if(_Left.PosInClause == _Right.PosInClause)
				return BigType(_Right.CombiType) && !BigType(_Left.CombiType);
			return (_Left.PosInClause < _Right.PosInClause);
		}
	};

	void CIndexLoader::ApplyCombinationList(list<SCombinationNewInfo>* pCombiInfos, IR::CSentence* pSent, bool bNum)
	{
		SS_TRY;

		if(pCombiInfos->empty())
			return;

		// sort it by position of the first combination unit
		stable_sort(pCombiInfos->begin(), pCombiInfos->end(), SCombiComparer());

		list<SCombinationNewInfo>::iterator it = pCombiInfos->begin();
		UINT32 nPos = 0;

		for(IR::CUnit* pUnit = pSent->GetFirstUnit(); nPos < MAX_WORDS_IN_SENTENCE; pUnit = pSent->GetNextUnit(), nPos++)
		{
			if(nPos == it->PosInClause + it->NumOfMembers)
			{
				int iPrevPosInClause = it->PosInClause;
				if(++it == pCombiInfos->end())
					break;

				if(it->PosInClause == iPrevPosInClause)
				{
					nPos -= it->NumOfMembers;
					for(UINT i = 0; i < it->NumOfMembers; i++)
						pUnit = pSent->GetPrevUnit();
				}
			}

			if(nPos >= it->PosInClause && nPos < it->PosInClause + it->NumOfMembers)
			{
				if(it->CombiType == ctNewLexicalUnit || it->CombiType == ctComplexWord)
				{
					int count = it->NumOfMembers;
					// join if we have more than one unit in combination
					if(count > 1)
					{
						IR::CUnit* pNewUnit = m_pUnitStorage->Allocate();
						pNewUnit->SetUnit(m_pStorage->CreateUnit());
						pNewUnit->CopyWordTypes(pSent->GetNextUnitWithoutMove());
						pNewUnit->SetWordType(wtUndefined, wmLSequences);

						int iPrevPosInClause = it->PosInClause;
						do
						{
							FILL_UNIT_INFO_BY_COMBINATORY(pNewUnit, &(*it), true, bNum);
						}
						while(++it != pCombiInfos->end() && it->PosInClause == iPrevPosInClause);

						SS::Core::Features::CMorphoFeature oMorpho;
						m_pUnitChecker->GetMorpho(pNewUnit->GetUnit(), oMorpho);
						bool isCombi = true;
						if (oMorpho.m_OfficialType.IsContain(SS::Core::Features::Values::OfficialType.otPretext))
						{
							isCombi = false;
							IR::CUnit *pNextUnit = 0;
							for (int i = 0; i < count; ++i)
								pNextUnit = pSent->GetNextUnit();

							int shift = 0;
							if (pNextUnit)
							{
								SS::Core::Features::CMorphoFeature oNextMorpho;
								while (pNextUnit)
								{
									m_pUnitChecker->GetMorpho(pNextUnit->GetUnit(), oNextMorpho);
									if (oNextMorpho.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postAdverb))
									{
										pNextUnit = pSent->GetNextUnit();
										++shift;
									}
									else
										break;
								}
								
								if (pNextUnit && (oNextMorpho.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postNoun) ||
									oNextMorpho.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postAdjective)))
								{
									isCombi = oNextMorpho.m_CaseType.IsIntersectsWith(&oMorpho.m_CaseType);
								}
							}

							shift += count;
							for (int i = 0; i < shift; ++i)
								pSent->GetPrevUnit();
						}
						if (isCombi)
						{
							pSent->JoinUnits(pNewUnit, count);
							nPos += count - 1;
							m_pUnitChecker->FillSemanticTypes(pNewUnit);
						}
						else
							m_pUnitStorage->Release(pNewUnit, true);

						if(it == pCombiInfos->end())
							break;
					}
					else
						FILL_UNIT_INFO_BY_COMBINATORY(pUnit, &(*it), true, bNum);
				}
				else
					FILL_UNIT_INFO_BY_COMBINATORY(pUnit, &(*it), false, bNum);
			}
		}

		SS_CATCH(L"");
	}

	SS::Core::Features::Types::CombiType GetCombiType(int iCombiType)
	{
		switch(iCombiType)
		{
		case ctNewLexicalUnit:
			return SS::Core::Features::Values::CombiType.cmbtNewLexicalUnit;
		case ctVerbWithParticle:
			return SS::Core::Features::Values::CombiType.cmbtVerbWithParticle;
		case ctVerbOrAdjWithPrep:
			return SS::Core::Features::Values::CombiType.cmbtVerbOrAdjWithPrep;
		case ctVerbWithPrepPart:
			return SS::Core::Features::Values::CombiType.cmbtVerbWithPrepPart;
		case ctPhrasalVerbWithPrep:
			return SS::Core::Features::Values::CombiType.cmbtPhrasalVerbWithPrep;
		case ctNounWithPrep:
			return SS::Core::Features::Values::CombiType.cmbtNounWithPrep;
		case ctComplexWord:
			return SS::Core::Features::Values::CombiType.Undefined;
		default:
			return SS::Core::Features::Values::CombiType.Undefined;
		}
	}

	void CIndexLoader::FillUnitInfoByCombinatory(IR::CUnit* pUnit, SCombinationNewInfo* pInfo, bool bMorpho)
	{
		SS_TRY
		{
			// set internal representation word type
			pUnit->SetWordType(wtWord, wmDefault, false);

			// set combi type
			pUnit->GetUnit()->CreateSyntaxFeature();
			pUnit->GetUnit()->GetSyntaxFeature()->GetSyntaxFeatureApriori()->m_CombiType = 
				GetCombiType(pInfo->CombiType);

			if(bMorpho && m_bMorpho && pInfo->ID != 0)
			{
				// fill morpho info
				TMorphoInfo oMorphoInfo;
				TSpecialMorpho oSpecialMorpho;

				m_pMorphoAnalyzer->GetMorphoInfoByIDType((DictionariesNumber)(pInfo->ID >> 24), pInfo->IDType, 0, 
					&oMorphoInfo, &oSpecialMorpho);

				if(!pInfo->ID)
				{
					SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"»з базы получен нулевой морфологический индекс" AND 
						pUnit->GetUnit()->GetWord());
					return;
				}

				IIndex* pIndex = m_pStorage->CreateIndex();
				pIndex->GetDictionaryIndex()->AppendIndex(pInfo->ID);
				pUnit->GetUnit()->AddIndex(pIndex);

				if(oMorphoInfo || oSpecialMorpho)
				{
					IFeature* pFeature = m_pStorage->CreateFeature();
					pFeature->SetMorphoIndex(0);
					pFeature->SetMorphoInfo(oMorphoInfo);
					pFeature->SetPartOfSpeech(oSpecialMorpho);
					pIndex->AddFeature(pFeature);
				}

				// fill semantic info
				if(m_bSemantic)
					FillSemanticInfo(pIndex, NULL, *pUnit);
			}
		}
		SS_CATCH(L"");
	}

	void CIndexLoader::FillUnitInfoByNumberCombinatory(IR::CUnit* pUnit, SCombinationNewInfo* pInfo, bool bMorpho)
	{
		SS_TRY
		{
			pUnit->SetWordType(wtNumber, wmDefault, false);
			pUnit->SetWordType(wtNumber, wmNumberSemantic);
			pUnit->GetUnit()->SetHashKey(pInfo->HashKey);
			if(pInfo->HashKey <= 31)
				pUnit->SetWordType(wtNumberDay, wmDates);
			else if(pInfo->HashKey < MAX_YEAR_NUMBER && pInfo->HashKey > MIN_YEAR_NUMBER)
				pUnit->SetWordType(wtNumberYear, wmDates);
			else
				pUnit->SetWordType(wtNumber, wmDates);

			if(bMorpho && m_bMorpho && pInfo->ID != 0)
			{
				if(!pInfo->ID)
				{
					SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"»з базы получен нулевой морфологический индекс" AND
						pUnit->GetUnit()->GetWord());
					return;
				}

				// fill morpho info
				IIndex* pIndex = m_pStorage->CreateIndex();
				pIndex->GetDictionaryIndex()->AppendIndex(pInfo->ID);
				pUnit->GetUnit()->AddIndex(pIndex);

				if(pInfo->uiMorphoInfo || pInfo->uiPartOfSpeech)
				{
					IFeature* pFeature = m_pStorage->CreateFeature();
					pFeature->SetMorphoInfo(pInfo->uiMorphoInfo);
					pFeature->SetPartOfSpeech(pInfo->uiPartOfSpeech);

					pIndex->AddFeature(pFeature);
				}

				// fill semantic info
				if(m_bSemantic)
					FillSemanticInfo(pIndex, NULL, *pUnit);
			}
		}
		SS_CATCH(L"");
	}

	void CIndexLoader::LoadSuffixesSemanticDict()
	{
		wstring wDataBasePath = 
			m_pIWorkRegister->GetParameter(L"PathDataBase") ;
		wstring wDict = wDataBasePath + L"CoSeDiRussian\\Semantics\\SematicSuffixes.dct";	

		m_pSemanticSuffixesDict = new TreeDictionary::CTreeDictionary;
		m_pSemanticSuffixesDict->LoadDict(wDict.c_str());  	
	}

	/// ѕолучить индекс первого символа после после числа
	int CIndexLoader::GetCharBeforeNumber(wstring word)
	{
		for(int i = 0; i < word.length() ; ++i)
		{
			if (!iswdigit(word[i]))
				return i;
		}
		return word.length();
	}

	/// ѕолучить индекс первого символа после после римского числа
	int CIndexLoader::GetCharBeforeRomanNumber(wstring word)
	{
		for(int i = 0; i < word.length() ; ++i)
		{
			if (!IsRomanSymbol(word[i]))
				return i;
		}
		return word.length();
	}

	bool CIndexLoader::IsRomanNumber(wstring str)
	{
		std::list<SS::Dictionary::Types::TWordInfo> oLst;
			
		m_pMorphoAnalyzer->GetWordInfo(str.c_str(), &oLst, SS::Dictionary::Types::efzOnlySearch);

		for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = oLst.begin(); it != oLst.end(); ++it)
		{
			SS::Core::Features::CMorphoFeature oMorphoFeature;
			SS::Dictionary::Types::SWordInfo wi = (*it).front();
			if (wi.m_WordIndex.m_DictIndex.GetDictionaryNumber() == ednRomanNumeric)
			{
				return true;
			}
		}

		return false;
	}

	/// явл€етс€ ли строка пор€дковым римским числом (XVIIIый)
	bool CIndexLoader::IsRomanOrdinal(wstring word)
	{
		int charBeforeRomanNumber = GetCharBeforeRomanNumber(word);

		wstring startPart = word.substr(0, charBeforeRomanNumber);
		if (startPart.empty())
			return false;

		if(IsRomanNumber(startPart) && (charBeforeRomanNumber < word.length()))
		{
			if(word[charBeforeRomanNumber] == '-')
				++charBeforeRomanNumber;

			wstring endPart = word.substr(charBeforeRomanNumber, word.length() - 1);
			return IsOrdinalParticle(endPart.c_str());
		}

		return false;
	}

	/// явл€етс€ ли строка пор€дковым окончанием
	bool CIndexLoader::IsOrdinalParticle(LPCTSTR szLexem)
{
		if (_tcsicmp(szLexem, _T("ый")) == 0 ||
			_tcsicmp(szLexem, _T("й")) == 0 ||
			_tcsicmp(szLexem, _T("ого")) == 0 ||
			_tcsicmp(szLexem, _T("го")) == 0 ||
			_tcsicmp(szLexem, _T("о")) == 0 ||
			_tcsicmp(szLexem, _T("ому")) == 0 ||
			_tcsicmp(szLexem, _T("му")) == 0 ||
			_tcsicmp(szLexem, _T("ым")) == 0 ||
			_tcsicmp(szLexem, _T("м")) == 0 ||
			_tcsicmp(szLexem, _T("ом")) == 0 ||			
			_tcsicmp(szLexem, _T("а€")) == 0 ||
			_tcsicmp(szLexem, _T("€")) == 0 ||
			_tcsicmp(szLexem, _T("ую")) == 0 ||
			_tcsicmp(szLexem, _T("ю")) == 0 ||
			_tcsicmp(szLexem, _T("ой")) == 0 ||
			_tcsicmp(szLexem, _T("ое")) == 0 ||
			_tcsicmp(szLexem, _T("е")) == 0 ||
			_tcsicmp(szLexem, _T("ые")) == 0 ||
			_tcsicmp(szLexem, _T("ых")) == 0 ||
			_tcsicmp(szLexem, _T("х")) == 0 ||
			_tcsicmp(szLexem, _T("ыми")) == 0 ||
			_tcsicmp(szLexem, _T("ми")) == 0)
		{
			return true;
		}
		return false;
}

	bool CIndexLoader::IsRomanSymbol(wchar_t ch)
	{
		if( ch == 'I' || ch == 'V' || 
			ch == 'X' || ch == 'L' || 
			ch == 'C' || ch == 'D' || 
			ch == 'M')
			return true;
		return false;
	}

	}
}
