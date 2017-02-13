#include "stdafx.h"
#include <math.h>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include "BaseDigestAnalyzer.h"
#include "..\assinterface\TCitation.h"
#include "..\assinterface\tmainanalysetypes.h"
#include "..\ASCInterface\defines.h"
#include "..\[libs]\_Paths.h"
#include "DictElems.h"
#include "usefull_str.h"

using namespace SS::Interface::Core;
using namespace SS::Interface::Core::MainAnalyse;
using namespace SS::Interface::Core::Dictionary;
using namespace SS::Interface::Core::BlackBox;

namespace SS
{
	namespace Digest
	{
		/// количество предложений перед и после предложения со смещением, 
		/// в котором будут искаться кореферентные ссылки
		const int TONALITY_COREFERENCE_AREA = 2;

		bool SortByPosInSentFull(IUnit* u1, IUnit* u2)
		{
			return u1->GetNumber() < u2->GetNumber();
		}

		bool SortByPosInSentForEntity(CEntity* e1, CEntity* e2)
		{			
			return e1->begin < e2->begin || (e1->begin==e2->begin && e1->unitList.size()>e2->unitList.size());
		}

		string GetStringByWstring(wstring wstr)
		{
			int nRet = WideCharToMultiByte(1251, 0, wstr.c_str(),  -1,
				NULL, 0, "#", NULL);

			char* str = new char[nRet+2];

			WideCharToMultiByte(1251, 0, wstr.c_str(),  -1,
				str, nRet+2, "#", NULL);

			string res = str;
			return res;
		}

		CBaseDigestAnalyzer::CBaseDigestAnalyzer() 
		{	
			m_pSyntaxAnalyser = NULL;
			m_pMorphoAnalyzer = NULL;
            m_ToneDictLoaded = false;

			m_entityPretextList.insert(L"с");
			m_entityPretextList.insert(L"для");
			m_entityPretextList.insert(L"в");
			m_entityPretextList.insert(L"во");
			m_entityPretextList.insert(L"из");
			m_entityPretextList.insert(L"при");
			m_entityPretextList.insert(L"по");
			m_entityPretextList.insert(L"о");
			m_entityPretextList.insert(L"об");
			m_entityPretextList.insert(L"над");
			m_entityPretextList.insert(L"за");
			m_entityPretextList.insert(L"на");
			m_entityPretextList.insert(L"от");
		}

		CBaseDigestAnalyzer::~CBaseDigestAnalyzer()
		{
		}

		void CBaseDigestAnalyzer::SetSentPreDigest(SS::Interface::Core::BlackBox::ISentence* pSent)
		{
			if(!pSent->IsPreDigest())
			{
				pSent->SetPreDigest(1);
				TUnitList sentUnits;
				pSent->GetLinearUnitsFull(sentUnits);
				NumberingSentenceUnits(sentUnits);
			}
		}

		void  CBaseDigestAnalyzer::MarkSentByOffset(IText* text,list<int>& offsets, bool isNext, bool isPrev)
		{
			m_lSentences.clear(); // очищаем линейный список предложений
			IUnit* pNextUnit;
			IUnit* pCurUnit;

			bool isFirst = true;
			for (IParagraph *pPar = text->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
			{
				for (ISentence *pSen = pPar->GetFirstSentence(); pSen; pSen = pSen->GetNextSentence())
				{
					SentenceInfo info;
					info.pSentence = pSen;
					m_lSentences.push_back(info);
				}
			}

			TSentList::iterator itSent = m_lSentences.begin();
			ISentence* sprev = 0;
			while (itSent != m_lSentences.end())
			{
				ISentence* sen = (*itSent).pSentence;
				sen->SetPrevSentenceLinear(sprev);
				sprev = sen;
				++itSent;
			}

			if (offsets.size() == 0)
			// если не передано смещений, то работаем в режиме 
			// обработки всего текста
			{
				for (TSentList::iterator it = m_lSentences.begin(); it != m_lSentences.end(); ++it)
				{
					SetSentPreDigest((*it).pSentence);
				}
			}
			else
			{
				TSentList::iterator it = m_lSentences.begin();
				while (it != m_lSentences.end())
				{
					ISentence *pCurSent = (*it).pSentence;
					TSentList::iterator nextIt = it;
					++nextIt;
					pCurUnit = pCurSent->GetFirstUnit(); 
					int cur_offset = unitOperator->GetSentenceOffset(pCurSent);
					int next_offset = 0;
					if (nextIt != m_lSentences.end())
					{
						ISentence *pNextSent = (*nextIt).pSentence;
						pNextUnit = pNextSent->GetFirstUnit();
						next_offset = unitOperator->GetSentenceOffset(pNextSent);
					}

					bool isFind = false;
					for (std::list<int>::iterator it1 = offsets.begin(); it1 != offsets.end(); ++it1)
					{
						int offset = *it1;
						if (((nextIt==m_lSentences.end() && cur_offset<=offset) ||
							(cur_offset<=offset && next_offset>offset)))
						{
							isFind = true;
                            break;
						}
					}

					if (!isFind)
					{
						if(!(*it).pSentence->IsPreDigest())
						{
							pCurSent->SetEnabled(0);
						}
					}
					else
					// берем текущее и следующее за ним предложение
					{
						pCurSent->SetEnabled(1);
						SetSentPreDigest(pCurSent);
						int count = TONALITY_COREFERENCE_AREA;
						TSentList::iterator iter = it;
						ISentence *pSent = 0;
						if (isPrev)
						{
							while (count > 0)
							{
								if (iter == m_lSentences.begin())
								{
									break;
								}
								--iter;
								--count;
								pSent = (*iter).pSentence;
								pSent->SetEnabled(1);

								SetSentPreDigest(pSent);
								SetCitationEnableForSynt(pSent);
							}
						}
						count = TONALITY_COREFERENCE_AREA;
						iter = it;
						if (isNext)
						{
							while (count > 0)
							{
								++iter;
								if (iter == m_lSentences.end())
								{
									break;
								}
								pSent = (*iter).pSentence;
								--count;
								pSent->SetEnabled(1);

								SetSentPreDigest(pSent);
								SetCitationEnableForSynt(pSent);
							}
						}
					}
					++it;
				}
			}
		}

		void CBaseDigestAnalyzer::MarkSentByOffset1(IText* pText, list<int>& offsets, list<int>& themes)
		{
			m_lSentences.clear(); // очищаем линейный список предложений
			IUnit* pNextUnit;
			IUnit* pCurUnit;
			ISentence* pNextSent;
			ISentence* pPrevSent;
			bool isFirst = true;
			for (IParagraph* pPar = pText->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
			{						
				for (ISentence* pSen = pPar->GetFirstSentence(); pSen;	pSen = pSen->GetNextSentence())
				{			
					if (pSen)
					{
						SentenceInfo info;
						info.pSentence = pSen;
						m_lSentences.push_back(info);
					}
				}
			}

			TSentList::iterator itSent = m_lSentences.begin();
			ISentence* sprev = NULL;
			while (itSent != m_lSentences.end())
			{
				ISentence* sen = (*itSent).pSentence;
				sen->SetPrevSentenceLinear(sprev);
				sprev = sen;
				++itSent;
			}

			TSentList::iterator it = m_lSentences.begin();
			while (it != m_lSentences.end())
			{
				pNextSent = NULL; 
				pPrevSent = NULL;
				ISentence *pCurSent = (*it).pSentence;
				if (it != m_lSentences.begin())
				{
					--it;
					pPrevSent = (*it).pSentence;
					++it;
				}
				++it;
				pCurUnit = pCurSent->GetFirstUnit(); 
				int cur_offset = unitOperator->GetSentenceOffset(pCurSent);
				int next_offset = 0;
				if (it != m_lSentences.end())
				{					
					pNextSent = (*it).pSentence;
					pNextUnit = pNextSent->GetFirstUnit();
					next_offset = unitOperator->GetSentenceOffset(pNextSent);					
				}

				bool isFind = false;
				int index = 0;
				list<int>::iterator it2 = themes.begin();
				for (list<int>::iterator it1 = offsets.begin(); it1 != offsets.end(); ++it1, ++it2)
				{
					int offset = *it1;
					if (((it==m_lSentences.end() && cur_offset<=offset) ||
						(cur_offset<=offset && next_offset>offset)))
					{
						isFind = true;
						break;			
					}
					++index;
				}

				if (isFind)
				{
					wchar_t sThemeID[15];
					wsprintfW(sThemeID, L"%d", *it2);
					pCurSent->SetMarkers(pCurSent->GetMarkers() + L"," + sThemeID);
					IUnit* pUnit = pCurSent->GetFirstUnit();
					wstring s = pUnit->GetWord();
				}
			}
		}

		bool CBaseDigestAnalyzer::IsUnitInsideEntity(CEntity* pEntity, SS::Interface::Core::BlackBox::IUnit* pUnit)
		{
			for (TUnitList::iterator it = pEntity->unitList.begin(); it != pEntity->unitList.end(); ++it)
			{
				if ((*it) == pUnit)
				{
					return true;
				}
			}
			return false;
		}

		bool CBaseDigestAnalyzer::IncludeEntityInside(CEntity* entParent, CEntity* entInside)
		{
			/// сначала проверим - возможно вторая цепочка уже внутри первой
			if (IsUnitInsideEntity(entParent, *entInside->unitList.begin()))
				return false;

			/// еще нет
			for(TUnitList::iterator it = entInside->unitList.begin(); it != entInside->unitList.end(); ++it)	
				entParent->unitList.push_back(*it);

			entParent->unitList.sort(SortByPosInSentFull);
			entParent->end = (*(--entParent->unitList.end()))->GetPosition();
			return true;
		}

		bool CBaseDigestAnalyzer::IsEntityListEqualWithPattern(list<EEntityType>& pattern, TEntityList& entityList)
		{
			int count = 0;
			if (entityList.size() < 2) return false;
			TEntityList::iterator it1 = entityList.begin();
			for (list<EEntityType>::iterator entTypeIt = pattern.begin(); entTypeIt!=pattern.end(); entTypeIt++)
			{
				if ((*it1)->entType != (*entTypeIt)) return false;
				it1++;
				if (it1 == entityList.end()) break;
			}

			SS::Core::Features::CMorphoFeature oMorfoUnit1;	
			SS::Core::Features::CMorphoFeature oMorfoUnit2;	
			IUnit* firstUnit = (*(*entityList.begin())->unitList.begin());
			IUnit* lastUnit = (*(*(--entityList.end()))->unitList.begin());
			unitOperator->GetUnitMorfoFeatures(firstUnit, oMorfoUnit1);
			unitOperator->GetUnitMorfoFeatures(lastUnit, oMorfoUnit2);
			if (oMorfoUnit1.m_CaseType.IsIntersectsWithout(&(oMorfoUnit2.m_CaseType)) &&
				oMorfoUnit1.m_NumberType.IsIntersectsWithout(&(oMorfoUnit2.m_NumberType)) &&
				oMorfoUnit1.m_GenderType.IsIntersectsWithout(&(oMorfoUnit2.m_GenderType)))
			{
				return true;
			}
			else
			{
				return false;
			}

		}

		void  CBaseDigestAnalyzer::EvristicGeoWithoutPredlogToOrg(TEntityList& allEntity, TUnitList& sentList)
		{
            SS::Core::Features::CMorphoFeature oMorfoGenetive;
            oMorfoGenetive.m_CaseType = SS::Core::Features::Values::CaseType.ctGenitive;

            TEntityList::iterator prev = allEntity.end();
			bool isPrevGeo = false;
			for(TEntityList::iterator iter = allEntity.begin(); iter != allEntity.end(); iter++)		
			{
				CEntity *pCurEnt = *iter;
				if (!pCurEnt->isUsed)
                    continue;
				bool isOrg = false;
                if ((pCurEnt->entType == entPlaceName) || ((pCurEnt->entType == entUnknownProper) && (pCurEnt->isPseudoPlace)))
				{
					IUnit* pFirstUnit = *(pCurEnt->unitList.begin());
					TUnitList::iterator it = unitOperator->FindUnit(sentList, pFirstUnit);
                    wstring str = pFirstUnit->GetWord();
                    IUnit *pSecond = pFirstUnit->GetRightUnit();
                    if (unitOperator->IsGeoPretext(pFirstUnit) ||
                        (pSecond && (str == L"с") && unitOperator->IsMorpho(pSecond, oMorfoGenetive)))
                    {
                        isOrg = false;
                    }
                    else if (it != sentList.begin())
					{
						--it;
						IUnit* preUnit = *it;
						str = preUnit->GetWord();
                        if((prev != allEntity.end()) && (it != sentList.begin()) && 
                           ((str == L"и") || ((str == L","))))
                        {
                            --it;
                            CEntity *prevEnt = *prev;
                            if (prevEnt->IsUnitInEntity(*it) && pCurEnt->IsEntCaseCoincident(prevEnt))
                            /// однородная сущность
                            {
                                if ((prevEnt->entType == entOrgName) && isPrevGeo)
                                    isOrg = true;
                            }
                            else
                                isOrg = true;
                        }
                        else if (!(unitOperator->IsGeoPretext(preUnit) ||
                            ((str == L"с") && unitOperator->IsMorpho(pFirstUnit, oMorfoGenetive))))
                        {
							isOrg = true;
                        }
					}
                    else
                        isOrg = true;
					
                    if (isOrg)
					{
                        if (pCurEnt->entType == entPlaceName)
                        /// меняем тип на организацию и устанавливаем флаг "псевдо география"
                        {
						    pCurEnt->entType = entOrgName;
						    pCurEnt->isPseudoPlace = true;
                        }
                        else if(pCurEnt->entType == entUnknownProper)
							pCurEnt->isPseudoPlace = false;
						isPrevGeo = true;
					}
				}
                prev = iter;
				if (!isOrg)
					isPrevGeo = pCurEnt->isPseudoPlace;
			}
		}

		void CBaseDigestAnalyzer::EvristicDeleteOrgFromPerson(TEntityList& allEntity)
		{
			bool isPersonName = false;
			for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); ++it)
			{
				isPersonName = false;
				CEntity *curEnt = *it;
				if (!curEnt->isUsed || (curEnt->m_lang == SS::Core::Types::ealEnglish))
					continue;
				if (curEnt->entType==entPersonName || curEnt->entType==entPersonInfo)
				{
					for(TUnitList::iterator iter = curEnt->unitList.begin(); iter != curEnt->unitList.end(); ++iter)
					{
						IUnit *pUnit = *iter;
						IUnit *pParent = pUnit->GetParentUnit();
						wstring str = pParent ? pParent->GetWord() : L"";
						if (pParent && str.empty() && !unitOperator->IsPersonName(pParent))
							continue;
						else
						{
							str = pUnit->GetWord();
							if (!str.empty() || unitOperator->IsPersonName(pUnit))
							{
								isPersonName = true;
								continue;
							}
						}

						if (isPersonName && str.empty() &&
							unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtOrganization) &&
							(iter != curEnt->unitList.begin()))
						{
							curEnt->unitList.erase(iter, curEnt->unitList.end());
							if (curEnt->unitList.empty())
								curEnt->isUsed = false;
							else
								curEnt->end = (*(--curEnt->unitList.end()))->GetPosition();
							if (curEnt->end<curEnt->begin || curEnt->unitList.empty())
								curEnt->isUsed = false;
							else
							{
								curEnt->entType = entDefault;
								SetEntityFullType(curEnt, allEntity);
							}
							break;
						}
					}
				}
			}
		}


		// если сразу за название организации стоит персона в родительном падеже, она может быть
		// к ней прицеплена по ошибке. Надо разделить
		// пример: пересказ статьи «Стратегическое планирование и политика» профессоров Уильяма Кинга и Девида Клиланда
		void CBaseDigestAnalyzer::EvristicDeletePersonFromOrg(TEntityList& allEntity)
		{
			SS::Core::Features::CMorphoFeature oMorphoNoun;
			oMorphoNoun.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;

			for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); ++it)
			{
				CEntity *curEnt = *it;
				if (!curEnt->isUsed || (curEnt->unitList.size()<2))
					continue;

				wstring str = curEnt->rootUnit->GetWord();
				// проверяем входит ли цепочка в какие-либо родительские, если да, то, скорее всего, в ней надо разделять.
				bool b = IsEntTypeParent(allEntity, curEnt);
				if (!b && !str.empty() && (curEnt->entType != entPlaceName))
					continue;
				int nonStrEmpCount = 0;
				bool isDeletePlaceUnits = false;
				if (((curEnt->entType==entOrgName/* || curEnt->entType==entUnknownProper*/) &&
					(str.empty() || !curEnt->rootUnit->GetIsNominalOnly() || b)) || (curEnt->entType == entPlaceName))
				{
					int qCount = 0;
					for(TUnitList::iterator it2 = curEnt->unitList.begin(); it2 != curEnt->unitList.end(); ++it2)
					{
						IUnit *pUnit = *it2;
						wstring str = pUnit->GetWord();
						if (!str.empty())
							++nonStrEmpCount;
						if (unitOperator->IsQuote(str))
							++qCount;
						if (str==L"им." || str==L"имени")
							break;
						if (str.empty() && (curEnt->entType == entPlaceName) && 
							unitOperator->IsSemantic(pUnit,  SS::Core::Features::Values::TSemanticType.smtPerson) &&
							unitOperator->IsSemantic(pUnit,  SS::Core::Features::Values::TSemanticType.smtPlace))
						{
							break;
						}

						if (!str.empty() && curEnt->entType==entPlaceName && 
							unitOperator->IsSemantic(pUnit,  SS::Core::Features::Values::TSemanticType.smtPlace) &&
							unitOperator->IsMorpho(pUnit, oMorphoNoun) &&
							(it2 == curEnt->unitList.begin()) && unitOperator->IsProperDict(pUnit))
						{
							isDeletePlaceUnits = true;
							continue;
						}

						if (isDeletePlaceUnits || (it2!=curEnt->unitList.begin() && 
							((nonStrEmpCount>1 && curEnt->entType==entPlaceName) || (nonStrEmpCount>0 && curEnt->entType==entOrgName)) 
							&& qCount%2==0 && pUnit->GetFirstIndex()!=NULL && pUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetLanguage()==SS::Core::Types::ealRussian &&
							unitOperator->IsSemantic(pUnit,  SS::Core::Features::Values::TSemanticType.smtPerson) && !
							unitOperator->IsSemantic(pUnit,  SS::Core::Features::Values::TSemanticType.smtOrganization)))
						{
							curEnt->unitList.erase(it2, curEnt->unitList.end());
							if (curEnt->unitList.empty())
								curEnt->isUsed = false;
							else
							{
								IUnit *pLast = curEnt->unitList.back();
								wstring word = pLast->GetWord();
								curEnt->end = pLast->GetPosition();
								if ((curEnt->end < curEnt->begin) || ((curEnt->end == curEnt->begin) && word.empty()))
									curEnt->isUsed = false;
							}
							if (curEnt->isUsed)
							{
								curEnt->entType = entDefault;
								SetEntityFullType(curEnt, allEntity);
							}
							break;
						}
					}
				}
			}
		}

		void CBaseDigestAnalyzer::EvristicDeletePlaceFromOrg(TEntityList& allEntity)
		{
			for (TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); ++it)
			{
				CEntity *pEntity = *it;
				if (pEntity->isUsed && (pEntity->entType == entOrgName))
				{
					IUnit* pRoot = pEntity->rootUnit;
					SS::Core::Features::CSemanticFeature oSemantic;
					unitOperator->GetUnitSemanticFeatures(pRoot, oSemantic);
					bool isOrganization = oSemantic.m_SemanticType.IsContain(SS::Core::Features::Values::TSemanticType.smtOrganization) &&
						oSemantic.m_SOrganizationType.IsContain(SS::Core::Features::Values::TSOrganizationType.orgtCompany);
					TEntityList inside;
					GetInsideEntities(pEntity, allEntity, inside);
					for (TEntityList::iterator iter = inside.begin(); iter != inside.end(); ++iter)
					{
						CEntity *pChild = *iter;
						if (pChild->rootUnit && pChild->isUsed && (pChild->nType == eEntityBaseType::Place))
						{
							pRoot = pChild->rootUnit;
							SS::Core::Features::CMorphoFeature oMorpho;
							unitOperator->GetUnitMorfoFeaturesFull(pRoot, oMorpho);
							if (oMorpho.m_CaseType.IsContain(SS::Core::Features::Values::CaseType.ctNominative) &&
								!oMorpho.m_CaseType.IsContain(SS::Core::Features::Values::CaseType.ctGenitive))
							/// аппозитивная связь 
							{
								pChild->isUsed = false;
							}
						}
					}
				}
			}
		}
		
		void CBaseDigestAnalyzer::EvristicSkobkiDelete(TEntityList& allEntity)
		{
			TEntityList newEntity;	
			for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); it++)		
			{
				CEntity *curEnt = *it;
				if (!curEnt->isUsed || (curEnt->entType == entPhone))
					continue;
				int count=0;
				wstring strFirst = (*curEnt->unitList.begin())->GetWord();
				for(TUnitList::iterator it2 = curEnt->unitList.begin(); it2 != curEnt->unitList.end(); it2++)	
				{
					count++;
					wstring str = (*it2)->GetWord();
					if (str==L"("||str==L"[" && (count>2 || strFirst.size()>0))
					{
						it2++;

						if (it2!= curEnt->unitList.end()/* && IsAbbr(*it2)*/)
						{
							for(TUnitList::iterator it3 = it2; it3 != curEnt->unitList.end(); it3++)
							{
								eEntityBaseType type = GetEntityType(*it3);
                                if (type != eEntityBaseType::Default)
								{
									CEntity* ent = new CEntity(unitOperator, curEnt->m_lang);
									ent->rootUnit = *it3;
									ent->nType = type;								
									unitOperator->GetLinearUnitsWithSort(ent->rootUnit, ent->unitList);
									ent->begin = (*ent->unitList.begin())->GetPosition();
									ent->end = (*(--ent->unitList.end()))->GetPosition();
									SetEntityFullType(ent, allEntity);
									newEntity.push_back(ent);
									break;									
								}			
							}
						}					
						curEnt->unitList.erase(--it2, curEnt->unitList.end());
						curEnt->end = (*(--curEnt->unitList.end()))->GetPosition();
						break;
					}
				}
			}

			for(TEntityList::iterator it1 = newEntity.begin(); it1 != newEntity.end(); it1++)		
			{
				allEntity.push_back(*it1);
			}
		}

		IUnit* CBaseDigestAnalyzer::GetNextUnit(ISentence* pSent, int position)
		{
			TUnitList sentUnits;
			pSent->GetLinearUnitsNonEmpty(sentUnits);	
			for(TUnitList::iterator it = sentUnits.begin(); it!=sentUnits.end(); it++)
			{
				if ((*it)->GetPosition()==position+1) return (*it);
			}
			return NULL;
		}

		void CBaseDigestAnalyzer::EvristicDeleteShortWordForm(TEntityList& allEntity)
		{
			for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); ++it)
			{
				CEntity *pEntity = *it;
				if ((pEntity->nType == eEntityBaseType::Other) &&
					(pEntity->entType != entUnknownProper))
				{
					for(TUnitList::iterator iter = pEntity->unitList.begin();
						iter != pEntity->unitList.end();
                        ++iter)
					{
						IUnit *pUnit = (*iter);
						CEntity *pSubEntity = NULL;
						if(iter != pEntity->unitList.begin())
							pSubEntity = GetEntityByUnit(allEntity, pUnit);
						if(pSubEntity && (pSubEntity->nType != eEntityBaseType::Other))
							continue;
						else if (unitOperator->IsShortWordForm(pUnit))
							pEntity->isUsed = false;
					}
				}
			}
		}

		void CBaseDigestAnalyzer::EvristicDeleteBadDates(TEntityList& lstAllEntities)
		{
			for(TEntityList::iterator itEnt = lstAllEntities.begin(); itEnt != lstAllEntities.end(); itEnt++)		
			{
                if ((*itEnt)->entType == entOrgName)
				{
					TUnitList lstRemoveUnits;
					for(TUnitList::iterator it = (*itEnt)->unitList.begin();
                        it != (*itEnt)->unitList.end();
                        ++it)
					{
						wstring str = (*it)->GetWord();
						if (str.empty() && unitOperator->IsSemantic((*it), SS::Core::Features::Values::TSemanticType.smtTimeInterval))
						{										
							unitOperator->GetLinearUnitsWithSort((*it), lstRemoveUnits);
						}
					}

					for(TUnitList::iterator it = lstRemoveUnits.begin(); it != lstRemoveUnits.end(); ++it)
						(*itEnt)->unitList.remove(*it);

					if ((*itEnt)->unitList.size()==0) 
						(*itEnt)->isUsed = false;
					else
					{
						(*itEnt)->begin = (*((*itEnt)->unitList.begin()))->GetPosition();
						(*itEnt)->end = (*((*itEnt)->unitList.rbegin()))->GetPosition();		
					}
				}
			}
		}

        void CBaseDigestAnalyzer::EvristicConvertBadDates(TEntityList& allEntity)
        {
            for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); it++)
            {
				CEntity *pEnt = *it;
                if((pEnt->entType == entDate) && pEnt->m_DateInterval.IsDateClear())
                {
					IUnit *pRoot = pEnt->rootUnit;
					if (pEnt->unitList.empty())
						pEnt->isUsed = false;
					else if (IsPerson(pRoot))
						pEnt->nType = eEntityBaseType::Person;
					else if (IsOther(pRoot))
						pEnt->nType = eEntityBaseType::Other;
					else if (unitOperator->IsVerbUnit(pRoot))
					{
						pEnt->nType = eEntityBaseType::Verb;
						GetVerbUnitList(pRoot, pEnt->unitList);
						if (pEnt->unitList.empty())
							pEnt->isUsed = false;
						else
						{
							pEnt->begin = (*pEnt->unitList.begin())->GetPosition();
							pEnt->end = (*(--pEnt->unitList.end()))->GetPosition();
						}
					}
					else
						pEnt->isUsed = false;
					if (pEnt->isUsed)
					{
						pEnt->entType = entDefault;
						SetEntityFullType(pEnt, allEntity);
					}
                }
            }
        }

		void CBaseDigestAnalyzer::EvristicCutPronounUnits(CEntity *pEntity, TEntityList& allEntity, SS::Interface::Core::BlackBox::ISentence* pSen)
		{
			if (!pEntity || (pEntity->unitList.size() == 1) || (pEntity->entType == entPronoun) || (pEntity->nType == eEntityBaseType::Verb))
				return;
			int startSize = pEntity->unitList.size();
			bool isStart = true;
			TUnitList::iterator parent = pEntity->unitList.end();
			TUnitList::iterator iter = pEntity->unitList.begin();
			while (iter != pEntity->unitList.end())
			{
				IUnit *pUnit = *iter;
				if (IsPronoun(pUnit))
				{
					TUnitList units;
					units.push_back(pUnit);
					allEntity.push_back(CreateNewEntity(EEntityType::entPronoun, eEntityBaseType::Pronoun, pUnit, pSen, units));
					if (isStart)
					{
						iter = pEntity->unitList.erase(iter);
						continue;
					}
					else
					{
						if ((parent != pEntity->unitList.end()) && (pUnit->GetParentUnit() == (*parent)))
							iter = parent;
						pEntity->unitList.erase(iter, pEntity->unitList.end());
						break;
					}
				}
				else
				{
					isStart = false;
					if (!pUnit->GetChildList().empty())
						parent = iter;
				}
				++iter;
			}
			int newSize = pEntity->unitList.size();
			if (newSize == 0)
				pEntity->isUsed = false;
			else if (newSize && (newSize != startSize))
			{
				pEntity->begin = (*pEntity->unitList.begin())->GetPosition();
				pEntity->end = (*(--pEntity->unitList.end()))->GetPosition();
			}
		}

		void CBaseDigestAnalyzer::EvristicPersonNameAndRomanNumberUnion(TEntityList &allEntities, TUnitList &unitList)
		{
			for(TEntityList::iterator it = allEntities.begin(); it != allEntities.end(); ++it)
            {
				CEntity *pEnt = *it;
				if ((pEnt->entType != entPersonName) || !pEnt->isUsed)
					continue;
                IUnit *pLastUnit = pEnt->unitList.back();
				IUnit *pNextUnit = unitOperator->GetUnitByPosition(unitList, pLastUnit->GetPosition() + 1);
				if (pNextUnit && unitOperator->IsRomanNumber(pNextUnit) &&
					unitOperator->IsSemantic(pNextUnit, SS::Core::Features::Values::TSemanticType.smtPerson))
				{
					pEnt->unitList.push_back(pNextUnit);
					pEnt->end = pNextUnit->GetPosition();
				}
			}
		}

        void CBaseDigestAnalyzer::EvristicSelectCeterus(ISentence* pSen, TEntityList& allEntity, TUnitList& lstSentList)
        {
			SS::Core::Features::Types::PartOfSpeechTypes morphoSemanticUnitsPartOfSpeech;
			morphoSemanticUnitsPartOfSpeech.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postNoun);
			morphoSemanticUnitsPartOfSpeech.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postAdjective);

            if(lstSentList.empty())
                return;

            vector<bool> unitsMap(lstSentList.back()->GetNumber(), false);
            for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); ++it)
            {
                CEntity *pEnt = *it;
				if(!pEnt->isUsed || ((pEnt->nType == eEntityBaseType::Verb) && (pEnt->unitList.size() > 1)))
                    continue;
                UpdateFreeUnitsMap(pEnt, unitsMap);
            }

            TUnitList entUnitList;
            for(TUnitList::iterator it = lstSentList.begin(); it != lstSentList.end(); ++it)
            {
                IUnit *pUnit = *it;
                int pos = pUnit->GetNumber() - 1;
				if((unitsMap.size() > pos) && !unitsMap[pos] && !unitOperator->IsInCombi(pUnit))
                {
                    EEntityType type = EEntityType::entUnknown;
					eEntityBaseType baseType = eEntityBaseType::Default;

					SMIList lstMI;
                    SS::Core::Features::CMorphoFeature oMorfo;
                    unitOperator->GetUnitMorfoFeaturesFullByMorphoAnalyzer(pUnit, oMorfo, &lstMI);
					if (unitOperator->IsUpperInsideSentence(pUnit) && oMorfo.m_PartOfSpeechTypes.IsIntersectsWith(&morphoSemanticUnitsPartOfSpeech))
					{
						baseType = GetUnitType(pUnit);
						switch (baseType)
						{
							case eEntityBaseType::Person:
								type = entPersonName;
								break;
							case eEntityBaseType::Place:
								type = entPlaceName;
								break;
							case eEntityBaseType::Organization:
								type = entOrgName;
								break;
							case eEntityBaseType::Product:
								type = entProduct;
								break;
							case eEntityBaseType::Event:
								type = entEvent;
								break;
							default:
								baseType = eEntityBaseType::Default;
						}
					}

					if (baseType == eEntityBaseType::Default)
					{
						if (oMorfo.m_PartOfSpeechTypes.IsIncludes(&SS::Core::Features::Values::PartOfSpeechTypes.postAdverb))
							type = EEntityType::entAdverb;
						else if (oMorfo.m_OfficialType.IsIncludes(&SS::Core::Features::Values::OfficialType.otPredicative))
							type = EEntityType::entPredicative;
						else if (oMorfo.m_PartOfSpeechTypes.IsIncludes(&SS::Core::Features::Values::PartOfSpeechTypes.postAdjective))
						/// прилагательное
						{
							type = EEntityType::entAdjective;
							for (SMIList::iterator it = lstMI.begin(); it != lstMI.end(); ++it)
							{
								SMorphoInfo *pInfo = &(*it);
								if (pInfo->partOfSpeech.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postAdjective))
								{
									if (!pInfo->shortType.IsUndefined())
										type = EEntityType::entShortAdjective;
									else if (pInfo->degreeType.IsContain(SS::Core::Features::Values::DegreeType.dtComparative))
										type = EEntityType::entComparative;
									else if (pInfo->degreeType.IsContain(SS::Core::Features::Values::DegreeType.dtSuperlative))
										type = EEntityType::entSuperlative;
								}
							}
						}
						else if(oMorfo.m_OfficialType.IsIncludes(&SS::Core::Features::Values::OfficialType.otPretext))
							type = EEntityType::entPretext;
						else
						{
							if(oMorfo.m_PartOfSpeechTypes.IsIncludes(&SS::Core::Features::Values::PartOfSpeechTypes.postNumeric))
								type = EEntityType::entNumeric;
							else
							{
								wstring word = pUnit->GetWord();
								if (unitOperator->IsQuote(word))
									type = EEntityType::entQuote;
								else
									continue;
							}
						}

						if (type == EEntityType::entAdverb)
							baseType = eEntityBaseType::Adverb;
						else if ((type == EEntityType::entAdjective) || (type == EEntityType::entShortAdjective) ||
							(type == EEntityType::entComparative) || (type == EEntityType::entSuperlative))
						{
							baseType = eEntityBaseType::Adjective;
						}
						else
							baseType = eEntityBaseType::Ceterus;
					}

                    entUnitList.clear();
					if (type == EEntityType::entQuote)
						entUnitList.push_back(pUnit);
					else
						unitOperator->GetUnitsInCombiWithSort(pUnit, entUnitList);

                    CEntity *pNew = CreateNewEntity(type, baseType, pUnit, pSen, entUnitList);
					if (pNew->IsGoodEnt())
					{
						UpdateFreeUnitsMap(pNew, unitsMap);
						allEntity.push_back(pNew);
					}
                }
            }
        }

        void CBaseDigestAnalyzer::UpdateFreeUnitsMap(CEntity *pEntity, vector<bool> &map)
        {
            for(TUnitList::iterator it = pEntity->unitList.begin(); it != pEntity->unitList.end(); ++it)
            {
                int pos = (*it)->GetNumber() - 1;
                if(map.size() > pos)
                    map[pos] = true;
            }
        }

		void CBaseDigestAnalyzer::EvristicRegionDelim(TEntityList& allEntity)
		{
			TUnitList unitsForDel;
			TEntityList newEntity;	
			for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); ++it)		
			{
				CEntity *curEnt = *it;
				unitsForDel.clear();
				int countVoid = 0;
				if (curEnt->isUsed && (curEnt->entType == entPlaceName || (curEnt->entType==entOrgName && curEnt->isPseudoPlace)))
				{
					for(TUnitList::reverse_iterator iter = curEnt->unitList.rbegin(); iter != curEnt->unitList.rend(); ++iter)
					{
						IUnit *pUnit = *iter;
						wstring str = pUnit->GetWord();
						if (str.empty())
                            countVoid++;
						if (str.empty() && 
							unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPlace) &&
							!unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson) &&
							pUnit->HasChilds() && countVoid>1)
						{							
							CEntity* ent = new CEntity(unitOperator, curEnt->m_lang);							
							ent->rootUnit = pUnit;
                            ent->nType = eEntityBaseType::Place;
							IUnit* pChild = ent->rootUnit->GetLeftChildUnit();
							unitsForDel.push_back(pUnit);
							while(pChild)
							{
								str = pChild->GetWord();
								if (!str.empty())
								{
									ent->unitList.push_back(pChild);
									unitsForDel.push_back(pChild);
								}
								pChild = pChild->GetRightUnit();
							}
							if (ent->unitList.empty())
								ent->isUsed = false;
							else
							{
								unitOperator->SortUnits(ent->unitList);
								ent->begin = (*(ent->unitList.begin()))->GetPosition();
								ent->end = (*(--ent->unitList.end()))->GetPosition();
								SetEntityFullType(ent, allEntity);
								newEntity.push_back(ent);
							}
						}
					}
					if (!unitsForDel.empty())
					{
						for(TUnitList::reverse_iterator iter = unitsForDel.rbegin(); iter != unitsForDel.rend(); ++iter)
							curEnt->unitList.remove(*iter);
					
						curEnt->isUsed = false;
					}
					/*if (curEnt->unitList.empty())
                        curEnt->isUsed = false;
					else
						curEnt->end = (*(--curEnt->unitList.end()))->GetPosition();*/
				}
			}

			for(TEntityList::iterator it = newEntity.begin(); it != newEntity.end(); ++it)
				allEntity.push_back(*it);
		}


		void CBaseDigestAnalyzer::FindRootRegions(TEntityList& allEntity, ISentence* pSent)
		{
			// если сразу за регионом стоит второй регион (или несколько подряд), то все они
			// считаются родитеслькими регионами: деревня Петушки Одинцовского района Московской области
			// и наоборот - если перед регионом стоят через запятую несколько регионов, то они родительские
			// надо объединить в один регион
			CEntity *findEnt = NULL;
			CEntity *prevEnt = NULL;
			if (allEntity.size()==0) return;

			for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); it++)		
			{
				CEntity *curEnt = *it;			
				if (curEnt->entType == entPlaceName || (curEnt->entType==entOrgName && curEnt->isPseudoPlace))
				{
					if (findEnt!=NULL)
					{
						if (prevEnt->end+1 == curEnt->begin ||
							(prevEnt->begin<curEnt->begin && prevEnt->end>curEnt->end))
						{
							findEnt->lstRootEntities.push_back(curEnt);
							curEnt->isUsed = false;
						}
						else findEnt=NULL;
					}else findEnt = curEnt;
				}else findEnt = NULL;

				prevEnt = curEnt;
			}

			findEnt = NULL;
			prevEnt=NULL;
			TEntityList::iterator it = allEntity.begin();
			while(true)		
			{
				if (it==allEntity.end()) break;
				CEntity *curEnt = *it;
				if (curEnt->entType == entPlaceName || (curEnt->entType==entOrgName && curEnt->isPseudoPlace))
				{
					if (findEnt!=NULL && prevEnt!=NULL)
					{
						IUnit* middleUnit = GetNextUnit(pSent, prevEnt->end);
						wstring str = L"";
						if (middleUnit!=NULL) str = middleUnit->GetWord();
						if (prevEnt->begin+2 == curEnt->begin && str==L",") // надо проверить на запятую между сущностями
						{
							findEnt->lstRootEntities.push_back(curEnt);
							curEnt->isUsed = false;
						}
						else
							findEnt=NULL;
					}else 
					{
						if (curEnt->entType == entPlaceName) findEnt = curEnt; 
					}
				}else
					findEnt = NULL;

				prevEnt = curEnt;			
				it++;
			}
		}
		
		// министерство обороны РФ
		void CBaseDigestAnalyzer::EvristicOrgGeoAbbrUnion(TEntityList& allEntity)
		{
			SS::Core::Features::CMorphoFeature oMorfoPattern;
			oMorfoPattern.Undefine();
			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;	

			for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); it++)		
			{
				CEntity *curEnt = *it;
				if (!curEnt->isUsed)
					continue;
				if ((curEnt->entType == entOrgName || curEnt->entType == entOrgNoInfo))
				{
					IUnit* pUnit = *curEnt->unitList.begin();					
					int nPos = curEnt->unitList.back()->GetPosition();
					if (nPos >= 0)
					{
						CEntity *nextEnt = GetEntityByUnitPosition(allEntity, nPos + 1);
						if (nextEnt && (nextEnt->unitList.size() == 1) && nextEnt->rootUnit->IsEvristicAbbr())
						{
							IUnit *pAbbrUnit = nextEnt->unitList.front();
							
							TEntityList::iterator iter = it;
							int curEntityBegin = curEnt->begin;
							int cutEntityEnd = curEnt->end;
							int newEndPosition = pAbbrUnit->GetPosition();
							CEntity *pEnt = *iter;
							while ((pEnt->begin <= curEntityBegin) && (pEnt->end == cutEntityEnd))
							{
								pEnt->unitList.push_back(pAbbrUnit);
								pEnt->end = newEndPosition;
								if (iter == allEntity.begin())
									break;
								else
									--iter;
								pEnt = *iter;
							}
							curEnt->nType=nextEnt->nType;
						}
					}
				}
			}
		}

		//администрация президента РФ
		void CBaseDigestAnalyzer::EvristicOgrNoInfoPersonrUnion(TEntityList& allEntity)
		{
			SS::Core::Features::CMorphoFeature oMorfoPattern;
			oMorfoPattern.Undefine();
			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;	
			oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.ctGenitive;	

			for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); it++)		
			{
				CEntity *curEnt = *it;
				if (!curEnt->isUsed)
					continue;
				if ((curEnt->entType == entOrgNoInfo) && curEnt->unitList.size()==1)
				{
					IUnit* pUnit = *curEnt->unitList.begin();					
					int nPos = (*curEnt->unitList.rbegin())->GetPosition();
					if (nPos>=0)
					{
						CEntity *nextEnt = GetEntityByUnitPosition(allEntity,nPos+1);
						if (nextEnt!=NULL && nextEnt->entType==entPersonInfo)
						{
							bool b = unitOperator->IsMorpho(nextEnt->rootUnit, oMorfoPattern); // обязательный родителный падеж
							if (b)
							{
								for(TUnitList::iterator it1 = nextEnt->unitList.begin(); it1 != nextEnt->unitList.end(); it1++)		
								{
									curEnt->unitList.push_back(*it1);
								}			
								curEnt->nType=nextEnt->nType;
								curEnt->entType = entOrgName; 					
								curEnt->end = (*(--curEnt->unitList.end()))->GetPosition();
							}							
						}
					}
				}
			}
		}

        void CBaseDigestAnalyzer::UnitReclassificationOnEnglish(TUnitList& sentList, eEntityBaseType &type)
        {
            SS::Core::Features::CMorphoFeature oMorfoVerb;
			oMorfoVerb.Undefine();
			oMorfoVerb.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;

            SS::Core::Features::CMorphoFeature oMorfoNoun;
			oMorfoNoun.Undefine();
            oMorfoNoun.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;

            SS::Core::Features::CMorphoFeature oMorfoDeterminer;
			oMorfoDeterminer.Undefine();
            oMorfoDeterminer.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
            oMorfoDeterminer.m_OfficialType = SS::Core::Features::Values::OfficialType.otDeterminers;
			
            TUnitList::iterator unitIt = sentList.begin();
            wstring word = (*unitIt)->GetWord();
			++unitIt;
			if(!word.empty() && (unitIt != sentList.end()))
            {
                std::list<SS::Dictionary::Types::TWordInfo> oLst;
                unitOperator->GetWordInfo(word, &oLst, SS::Dictionary::Types::efzOnlySearch);
                for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = oLst.begin();
                    it != oLst.end();
                    ++it)
                {
                    SS::Dictionary::Types::SWordInfo wi = (it->front());
                    if(wi.m_NewWord)
                        continue;

                    SS::Core::Features::CMorphoFeature oMorphoWord;
                    m_pAMConverter->EnCode(
				        wi.m_WordIndex.m_DictIndex.GetDictionaryNumber(),
				        wi.m_MorphoTypeInfo.m_MorphoInfo,
				        wi.m_SpecialMorpho,
				        &oMorphoWord);


                    /// прилагательное
                    bool isAdjective = oMorphoWord.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postAdjective);
                    /// числительное
                    bool isNumeric = oMorphoWord.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postNumeric);
                    /// артикль
                    bool isDeterminer = oMorphoWord.m_OfficialType.Equal(SS::Core::Features::Values::OfficialType.otDeterminers);
                    /// причастие на ED
                    bool isParticipleED = oMorphoWord.m_VerbForm.Equal(SS::Core::Features::Values::VerbForm.vfParticiple) && 
									 //---oMorphoWord.m_VerbForm.Equal(SS::Core::Features::Values::VerbForm.vfParticiple) &&
										  oMorphoWord.m_TimeType.Equal(SS::Core::Features::Values::TimeType.tmtPast);
                    /// причастие на ING
                    bool isParticipleING = oMorphoWord.m_VerbForm.Equal(SS::Core::Features::Values::VerbForm.vfParticiple) && 
								      //---oMorphoWord.m_VerbForm.Equal(SS::Core::Features::Values::VerbForm.vfParticiple) &&
										   oMorphoWord.m_TimeType.Equal(SS::Core::Features::Values::TimeType.tmtPresent);
                    /// предлог
                    bool isPretext = oMorphoWord.m_OfficialType.Equal(SS::Core::Features::Values::OfficialType.otPretext);
                    /// инфинитив
                    bool isInfinitive = oMorphoWord.m_VerbForm.Equal(SS::Core::Features::Values::VerbForm.vfInfinitive);
                    /// наречие
                    bool isAdverb = oMorphoWord.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postAdverb);
                    /// глагол
                    bool isVerb = oMorphoWord.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postVerb);

                    if (isAdjective || isNumeric || isDeterminer || isParticipleED)
                    {
                        if(unitOperator->IsMorpho(*unitIt, oMorfoNoun))
                        {
                            type = eEntityBaseType::Default;
                            break;
                        }
                    }
                    else if (isPretext)
                    {
                        if (unitOperator->IsMorpho(*unitIt, oMorfoNoun) || 
                            unitOperator->IsMorpho(*unitIt, oMorfoDeterminer))
                        {
                            type = eEntityBaseType::Default;
                            break;
                        }
                    }
                    else if (isInfinitive ||
                        (isParticipleING && !unitOperator->IsMorpho(*unitIt, oMorfoNoun)) ||
                        (isVerb && ((wi.m_wWord == L"is") || (wi.m_wWord == L"has") || (wi.m_wWord == L"does"))))
                    {
                        type = eEntityBaseType::Verb;
                        break;
                    }
                    else if (isAdverb)
                    {
                        wstring str = (*unitIt)->GetWord();
                        if((str == L",") || unitOperator->IsMorpho(*unitIt, oMorfoVerb))
                        {
                            type = eEntityBaseType::Default;
                            break;
                        }
                    }
                }
            }
        }

		// Ключевский завод, Ивановский комбинат
		void CBaseDigestAnalyzer::EvristicPersonOrgOrPlaceUnion(TEntityList& allEntity)
		{
			SS::Core::Features::CMorphoFeature oMorfoPattern;
			oMorfoPattern.Undefine();
			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;	

			for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); it++)		
			{
				CEntity *curEnt = *it;
				if (!curEnt->isUsed)
					continue;
				if (curEnt->entType == entPersonName && curEnt->unitList.size()==1)
				{
					IUnit* pUnit = *curEnt->unitList.begin();
					if (!unitOperator->isEvristicAdj(pUnit))
                        continue;
					int nPos = (*curEnt->unitList.begin())->GetPosition();
					if (nPos>=0)
					{
						CEntity *nextEnt = GetEntityByUnitPosition(allEntity,nPos+1);
						if (nextEnt!=NULL && (nextEnt->nType==2))
						{
							for(TUnitList::iterator it1 = nextEnt->unitList.begin(); it1 != nextEnt->unitList.end(); it1++)		
							{
								curEnt->unitList.push_back(*it1);
							}			
							curEnt->nType=nextEnt->nType;
							if (nextEnt->nType)
                                curEnt->entType = entOrgName;
                            else
								curEnt->entType = entPlaceName;
							curEnt->end = (*(--curEnt->unitList.end()))->GetPosition();
							nextEnt->isUsed = false;
							curEnt->entType = entOrgName;
						}
					}
				}
			}
		}


		void CBaseDigestAnalyzer::EvristicPersonUnitRus(TEntityList& allEntity)
		{
			// 1. сортируем все сущности по порядку следования их в предлоежнии
			// 2. идем слева напрао и ищем цепочки по всем поисковым шаблонам.
			// 3. найденные объединяем. 
			list<EEntityType> patternMatrixUnion[4];
			patternMatrixUnion[0].push_back(entPersonInfo);
			patternMatrixUnion[0].push_back(entPersonName);

			patternMatrixUnion[1].push_back(entPersonNoInfo);
			patternMatrixUnion[1].push_back(entPersonName);

			patternMatrixUnion[2].push_back(entPersonInfo);
			patternMatrixUnion[2].push_back(entPersonName);
		
			TEntityList curEntListUnion;   
			TEntityList curEntListInside;
			CEntity *lastEnt = NULL;
			TEntityList lstParents;
			for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); it++)		
			{
				CEntity *curEnt = *it;
				if (!curEnt->isUsed)
					continue;
				lstParents.clear();
				GetEntityParentList(allEntity, curEnt, lstParents); // мы должны проверять на соединение только
				// цепочки верхнего уровня.
				if (!lstParents.empty() || IsEntTypeParent(allEntity, curEnt))
					continue;
				if (lastEnt && (lastEnt->end+1 == curEnt->begin))
				{				
					curEntListUnion.push_back(lastEnt);
					curEntListUnion.push_back(curEnt);
				}
				if (curEntListUnion.size() == 2)
				{
					for(int i = 0; i < 3; ++i)
					{
						if (IsEntityListEqualWithPattern(patternMatrixUnion[i], curEntListUnion))
						{
							// объединяем сущности
							TEntityList::iterator it1 = curEntListUnion.begin();
							CEntity* rootEnt = *it1;
							rootEnt->entType = entPersonName;
							++it1;
							while(it1 != curEntListUnion.end())
							{
								CEntity* nextEnt = *it1;
								for(TUnitList::iterator it2 = nextEnt->unitList.begin(); it2 != nextEnt->unitList.end(); ++it2)
								{
									rootEnt->unitList.push_back(*it2);
								}

								//rootEnt->unitList.merge(nextEnt->unitList);
								if (nextEnt->nType == rootEnt->nType)
									nextEnt->isUsed = false;
								++it1;
							}
							rootEnt->end = (*(--rootEnt->unitList.end()))->GetPosition();
							break;
						}
					}
					curEntListUnion.clear();
				}
				/*
				if (lastEnt && (lastEnt->begin<curEnt->begin && lastEnt->end>curEnt->end))
				// цепочка целиком внутри предыдыущей
				{
					IncludeEntityInside(lastEnt, curEnt);
				}*/
				lastEnt = curEnt;
			}
		}


		// президент Петров с мэром Ивановым 
		void CBaseDigestAnalyzer::EvristicCDelimiterRus(TEntityList& allEntity)
		{
			TEntityList newEntity;		
			for(TEntityList::iterator it1 = allEntity.begin(); it1 != allEntity.end(); it1++)		
			{
				CEntity* pEnt = *it1;
				if (!pEnt->isUsed)
					continue;
				if (pEnt->entType!=entPersonName)
					continue;
				for(TUnitList::iterator it = pEnt->unitList.begin(); it != pEnt->unitList.end(); it++)		
				{
					IUnit* pUnit = *it;
					wstring s = pUnit->GetWord();				
					if (s==L"с" || s==L"С" || s==L"к" || s==L"К")
					{
						IUnit* pChildUnit = pUnit->GetLeftChildUnit();
						if (pChildUnit!=NULL)
						{						
							if (unitOperator->IsSemantic(pChildUnit, SS::Core::Features::Values::TSemanticType.smtPerson))
							{
								CEntity* pEntNew = new CEntity(unitOperator, pEnt->m_lang);
                                pEntNew->nType = eEntityBaseType::Person;
								pEntNew->rootUnit = pChildUnit;
								for(TUnitList::iterator it2 = ++it; it2!=pEnt->unitList.end(); ++it2)
								{
									pEntNew->unitList.push_back(*it2);
								}
								//this->GetLinearUnitsWithSort(pEntNew->rootUnit, pEntNew->unitList);
								pEntNew->begin = (*pEntNew->unitList.begin())->GetPosition();
								pEntNew->end = (*(--pEntNew->unitList.end()))->GetPosition();
								SetEntityFullType(pEntNew, allEntity);	
								it--;
								pEnt->unitList.erase(it,  pEnt->unitList.end());
								if (pEnt->unitList.size()==0)
								{
									pEnt->isUsed=false;
								}else
								{
									pEnt->end = (*(--pEnt->unitList.end()))->GetPosition();
								}
								newEntity.push_back(pEntNew);
								break;
							}
						}
					}
				}
			}

			for(TEntityList::iterator it1 = newEntity.begin(); it1 != newEntity.end(); it1++)		
			{
				allEntity.push_back(*it1);
			}
		}
	
		bool CBaseDigestAnalyzer::IsGoodForAbbr(TUnitList& findedUnits)
		{
			SS::Core::Features::CMorphoFeature unitMorpho;
			for(TUnitList::iterator it1 = findedUnits.begin(); it1 != findedUnits.end(); it1++)
			{
				IUnit* findUnit = *it1;
				unitOperator->GetUnitMorfoFeatures(findUnit,unitMorpho);
				if (findUnit->IsInitial() || unitOperator->IsSemantic(findUnit,  SS::Core::Features::Values::TSemanticType.smtPerson) ||
					!(unitMorpho.m_PartOfSpeechTypes.IsIntersectsWith(&(SS::Core::Features::Values::PartOfSpeechTypes.postNoun))||
					unitMorpho.m_PartOfSpeechTypes.IsIntersectsWith(&(SS::Core::Features::Values::PartOfSpeechTypes.postAdjective))))
				{
					return false;
				}
			}
			return true;
		}

		void CBaseDigestAnalyzer::FindAbbrevInSent(TUnitList& sentUnits,ISentence* pSent,
			IUnit* pUnitAbbr, TUnitList& findedList)
		{
			int curInd = 0;
			wstring strAbbr = pUnitAbbr->GetWord();
			wstring strAbbrCopy = strAbbr;
			wstring strAbbrLower = unitOperator->GetLowerWord(strAbbrCopy);

			for(TUnitList::iterator it = sentUnits.begin(); it != sentUnits.end(); ++it)
			{
				IUnit *pUnit = *it;
				wstring word = pUnit->GetWord();
				if (word==strAbbr && it!=sentUnits.begin())
				{
					--it;
					wstring prevword = (*it)->GetWord();
					if (prevword == L"(")
					{
						// надо взять все слова до скобки до слова с большой буквы, с которой начинается аббревиатура
						// причем этих слов должно быть не больше чем букв в аббревиатуре
						bool isFind = false;
						for(unsigned int i = 0; i < strAbbr.size(); ++i)
						{
							if (it == sentUnits.begin())
								break;
							--it;
							findedList.push_back(*it);
							wstring curWord = (*it)->GetWord();
							if ( curWord[ 0 ] == strAbbr[ i ] )
						    //---if ( curWord[ 0 ] == strAbbr[ 0 ] )
							{
								isFind = true;
								break;
							}
						}
						if ( isFind )
						{
							break;
						}
						else
						{
							for(unsigned int i = 0; i < findedList.size(); ++i)
							{							
								++it;
								if (it == sentUnits.end())
									break;
							}
							findedList.clear();
						}
					}
					if (it == sentUnits.end())
						break;
					++it;
					// теперь проверим скобку за аббревиатурой
					if (it == sentUnits.end())
						break;
					++it;
					if (it != sentUnits.end())
					{
						wstring nextword = (*it)->GetWord();
						if ( nextword == L"(" && it != sentUnits.end() )
						{
							// все что внутри скобок - наименование аббревиатуры
							for(unsigned int i = 0; i < strAbbr.size(); ++i)
							{							
								++it;
								if (it == sentUnits.end())
									break;
								findedList.push_back(*it);
								wstring curWord = (*it)->GetWord();
								if (curWord == L")")
									break;
							}					
							if (!findedList.empty())
							{
								wstring firstWord = (*findedList.begin())->GetWord();
								if (firstWord[0] == strAbbr[0])
									break; // считаем что нашли
								else
								{
									for(unsigned int i = 0; i < findedList.size(); ++i)
									{
										if (it == sentUnits.begin())
											break;
										--it;
									}
									findedList.clear();
								}
							}
						}
					}
					if (it == sentUnits.begin())
						break;
					--it;
				}

				if (!word.empty() && (word[0]==strAbbr[0]))
				{
					findedList.push_back(pUnit);
					for(unsigned int i = 1; i < strAbbr.size(); ++i)
					{
						++it;
						if (it == sentUnits.end())
							break;
						word = (*it)->GetWord();
						if (!word.empty() && (word[0]==strAbbr[i] || word[0]==strAbbrLower[i]))
							findedList.push_back(*it);
						else
							break;
					}
					bool isFind = true;
					if (findedList.size()==strAbbr.size())
					{
						// все слова должны быть лишь сущ. либо прилагат. Не должно быть 
						// семантики персоны
						if (pSent->GetLanguage()==SS::Core::Types::ealRussian)
							isFind = IsGoodForAbbr(findedList);
					}
					else
						isFind = false;
					if (!isFind)
					{
						for(unsigned int i = 0; i < findedList.size() - 1; ++i)
						{
							if (it == sentUnits.begin())
								break;
							--it;
						}
						findedList.clear();
					}
					else
						break; // нашли - выходим
					if (it == sentUnits.end())
						break;
				}
			} //for(TUnitList::iterator it = sentUnits.begin();
		}


		void CBaseDigestAnalyzer::FindAbbrevKorefName(IText* pText, 
			IUnit* pUnitAbbr,TUnitList& findedUnits)
		{			
			for(IParagraph* pPar = pText->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
			{														
				for(ISentence* pSen = pPar->GetFirstSentence(); pSen; pSen = pSen->GetNextSentence())
				{
					TUnitList sentUnits;
					pSen->GetLinearUnitsNonEmpty(sentUnits);
					IUnit* pFirstUnit = *sentUnits.begin();
					wstring str1 = pFirstUnit->GetWord();
					if (str1==L"То")
					{
						int h=0;
					}
					FindAbbrevInSent(sentUnits, pSen, pUnitAbbr, findedUnits);
					if (findedUnits.size()>0)
					{
						return;
					}
				}
			}
		}

		void CBaseDigestAnalyzer::ProcessRegion(ISentence* pSen, TUnitList& linkedPlaces)
		{		
			TUnitList sentUnits;
			pSen->GetLinearUnitsFull(sentUnits);
			set<wstring> findedRegions;
			linkedPlaces.clear();

			for (TUnitList::iterator itU = sentUnits.begin(); itU != sentUnits.end(); ++itU)
			{
				IUnit *pUnit = *itU;
				bool isRegionRoot = unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPlace,
					SS::Core::Features::Values::TSPersonType.Undefined, SS::Core::Features::Values::TSPlaceType.pltCountry) &&
					pUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase();
                /// при комбинаторике str будет пустым
                wstring str = unitOperator->GetMainFormText1(pUnit, SS::Core::Features::Values::GenderType.Undefined).form;
				if (!isRegionRoot && str.empty())
				{
					IUnit *pChildUnit = pUnit->GetLeftChildUnit();
					while(pChildUnit)
					{
						if (unitOperator->IsSemantic(pChildUnit, SS::Core::Features::Values::TSemanticType.smtPlace,
							SS::Core::Features::Values::TSPersonType.Undefined, SS::Core::Features::Values::TSPlaceType.pltProvince) ||
							unitOperator->IsSemantic(pChildUnit, SS::Core::Features::Values::TSemanticType.smtPlace,
							SS::Core::Features::Values::TSPersonType.Undefined, SS::Core::Features::Values::TSPlaceType.pltState))
						{
							isRegionRoot = true;
						}

                        wstring str1 = unitOperator->GetMainFormText1(pChildUnit, SS::Core::Features::Values::GenderType.Undefined).form;
					    if (unitOperator->IsQuote(str1))
                            str += L'\'';
					    if (str1.size() > 0)
						    str += str1 + L" ";

						pChildUnit = pChildUnit->GetRightUnit();
					}
				}

				if (isRegionRoot && (findedRegions.find(str)==findedRegions.end()))
				{
					linkedPlaces.push_back(pUnit);
                    findedRegions.insert(str);
				}
			}
		}

		void CBaseDigestAnalyzer::EvristicFrom_ToDates(TEntityList& lstEntities, TUnitList& sentList)
		{
			CEntity* lastEnt = NULL;
			set<wstring> prefixesBegin; // предлоги-префиксы для начала интервала
			set<wstring> prefixesEnd; // предлоги-префиксы для конца интервала

			prefixesBegin.insert(L"с");
			prefixesBegin.insert(L"С");						
			prefixesBegin.insert(L"from");
			prefixesBegin.insert(L"From");	

			prefixesEnd.insert(L"по");		
			prefixesEnd.insert(L"to");

			for(TEntityList::iterator it1 = lstEntities.begin(); it1 != lstEntities.end(); it1++)		
			{
				CEntity* curEnt = (*it1);
				if (!curEnt->isUsed)
					continue;
				if (curEnt->entType == entDate)
				{
					TUnitList::iterator it = unitOperator->FindUnit(sentList, *curEnt->unitList.begin());
					while(it!=sentList.begin())
					{
						it--;	
						wstring str = (*it)->GetWord();
						if (str.size()!=0) break;					 
					}					
					if ((*it)->GetFirstCharPositionNew()>=curEnt->GetBeginOffset() && it!=sentList.begin())
						it--;						
					wstring str = (*it)->GetWord();
					if (prefixesBegin.find(str)!=prefixesBegin.end())
					{
						lastEnt = curEnt;
						continue;
					}
					if (lastEnt!=NULL && prefixesEnd.find(str)!=prefixesEnd.end())
					{
						if (lastEnt->end+2==curEnt->begin) 
						{
							lastEnt->extDateRootUnit = curEnt->rootUnit->GetLeftChildUnit();
						}
					}

					//}
				}
				lastEnt = NULL;
			}
		}


		// позволяет решить проблемы с цепочками типа половина Камчатской области
		// если внутри цепрочки типа дата есть регион то цепочка меняется не регион
		void CBaseDigestAnalyzer::EvristicChangeDateToPlace(TEntityList& lstEntities)
		{		
			SS::Core::Features::CMorphoFeature oMorfoPattern;
			oMorfoPattern.Undefine();
			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;		
			oMorfoPattern.m_PartOfSpeechTypes.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postNumeric);		

			for(TEntityList::iterator it1 = lstEntities.begin(); it1 != lstEntities.end(); it1++)		
			{
				CEntity* curEnt = (*it1);
				if (!curEnt->isUsed)
					continue;
				if (curEnt->m_lang == SS::Core::Types::ealEnglish) continue;
				if (curEnt->entType == entDate && curEnt->m_DateInterval.begin.nYear==0)
				{
					for(TUnitList::iterator it = curEnt->unitList.begin(); it!=curEnt->unitList.end(); it++)
					{
                        if((*it)->GetParentUnit())
                            continue;
                        if (unitOperator->IsSemantic(*it,  SS::Core::Features::Values::TSemanticType.smtPlace) &&
							!unitOperator->IsMorpho(*it,oMorfoPattern) && !unitOperator->IsHavePoint(*it))
						{
							curEnt->entType = entPlaceName;
							break;
						}
					}
				}
			}
		}

		// находит однородные сущности и объединяет, за исключением сущностей верхнего уровня
		// т.е. если за какой-лтбо сущностью НЕ верхнего уровня следуют (пропуская запятые и союзы) сущности
		// такого же типа и согласованные по падежам, то все юниты этой сущности, а также запятые и союзы надо добавить в 
		// списки юнитов для всех родительских сущностей первой сущности 
		void CBaseDigestAnalyzer::EvristicHomogeneous(TEntityList& lstAllEntities, TUnitList& lstSentList, bool bSemanticNeeded)
		{
			CEntity* pPrevEnt = NULL;
			TUnitList lstHomogenUnits;
			SS::Core::Features::CMorphoFeature oMorfoPatternGenitive;
			oMorfoPatternGenitive.Undefine();
			oMorfoPatternGenitive.m_CaseType = SS::Core::Features::Values::CaseType.ctGenitive;

			bool bHomogenDiv = false;
			bool bHomogenConjFind = false;
			bool bHomogeneousEntFind = false;
			for(TUnitList::iterator itUnit = lstSentList.begin(); itUnit != lstSentList.end(); ++itUnit)
			{
				IUnit* pUnit = *itUnit;
				IUnit* pParent = pUnit->GetParentUnit();
				wstring str;
				if (pParent)
				{
					str = pParent->GetWord();
					if (str.empty())
						continue;
				}
				str = pUnit->GetWord();
				if (str==L"и" || str==L"или" || str==L",")
				{
                    bool conjInEnt = false;
                    for(TUnitList::iterator it = lstHomogenUnits.begin(); !conjInEnt && (it != lstHomogenUnits.end()); ++it)
                    {
                        if(*it == pUnit)
                            conjInEnt = true;
                    }
                    if(conjInEnt)
                        continue;

					if (bHomogenDiv ||  bHomogenConjFind)
					{
						if (bHomogenConjFind && bHomogeneousEntFind && lstHomogenUnits.size()>0)
							CopyHomogeneousUnits(lstAllEntities, pPrevEnt, lstHomogenUnits, false);
						lstHomogenUnits.clear();
						pPrevEnt = NULL;
						bHomogenDiv = false;
						bHomogenConjFind = false;
						bHomogeneousEntFind = false;
					}
                    else
					{
						lstHomogenUnits.push_back(pUnit);
						bHomogenDiv = true;
					}
					if (str != L",")
                        bHomogenConjFind = true;
				}
				else
				{
					CEntity* pCurEnt = GetEntityByUnit(lstAllEntities, pUnit);
                    if (pCurEnt && pCurEnt->nType!=eEntityBaseType::Verb && pPrevEnt &&
                        bHomogenDiv && pPrevEnt->IsHomogeneousUnionPossible(pCurEnt) &&
						unitOperator->IsMorpho(pCurEnt->rootUnit, oMorfoPatternGenitive) &&
						pCurEnt->IsEntCaseCoincident(pPrevEnt) &&
						(!bSemanticNeeded || pCurEnt->IsEntSemanticCoincident(pPrevEnt)))
					{
						for(TUnitList::iterator itEntUnit = pCurEnt->unitList.begin();
                            itEntUnit != pCurEnt->unitList.end();
                            itEntUnit++)
						{
							lstHomogenUnits.push_back(*itEntUnit);
						}
						bHomogeneousEntFind = true;
					}
					else
					{
						if (bHomogenConjFind && bHomogeneousEntFind && lstHomogenUnits.size()>0)
							CopyHomogeneousUnits(lstAllEntities, pPrevEnt, lstHomogenUnits, false);
						lstHomogenUnits.clear();
						pPrevEnt = pCurEnt;
						bHomogenConjFind = false;
						bHomogeneousEntFind = false;
					}
					bHomogenDiv = false;
				}
			}
			if (bHomogenConjFind && bHomogeneousEntFind && lstHomogenUnits.size()>0)
				CopyHomogeneousUnits(lstAllEntities, pPrevEnt, lstHomogenUnits, false);
		}

		// вспомогательная функция,которая копирует юниты всех однородных сущнностей в списки для коренвых
		void CBaseDigestAnalyzer::CopyHomogeneousUnits(TEntityList& lstAllEntities, 
			CEntity* pRootEnt, TUnitList& lstAllHomogeneousUnits, bool isIncludeCurrent)
		{
			TEntityList lstParents;
			int nNounCount = unitOperator->GetNominalNounCount(lstAllHomogeneousUnits);

			GetEntityParentList(lstAllEntities, pRootEnt, lstParents);
			if (isIncludeCurrent)
			{
				lstParents.push_front(pRootEnt);
			}
			// между такими сущностями, которые согласованы по типу и падежу может стоить либо запятая либо 
			// союз и/или, тогда можно объединять со всеми родительскими, для которых количество
			// существительных в нариц = кол-ву существительных в нариц. для добавляемых онитов		
			for(TEntityList::iterator itParentEnt = lstParents.begin(); itParentEnt != lstParents.end(); itParentEnt++)
			{
				CEntity* pParent = *itParentEnt;			
				if (pParent->GetNominalNounCount()>nNounCount || isIncludeCurrent)
				{
					for(TUnitList::iterator itUnit = lstAllHomogeneousUnits.begin(); itUnit != lstAllHomogeneousUnits.end(); itUnit++)
					{
						IUnit *pUnit = *itUnit;
						if (!unitOperator->FindInUnits(pParent->unitList, pUnit))
							pParent->unitList.push_back(pUnit);
					}
					pParent->end = (*pParent->unitList.rbegin())->GetPosition();
				}
			}
		}

		// позволяет провести постсвязывание генетивов в которых была цепочка однородных прилагательных.
		// Например: Российская ассоциация прямого и венчурного инвестирования 
		// надо будет досвзяать цепочки Российская ассоциация и "прямого и венчурного инвестирования"
		void CBaseDigestAnalyzer::EvristicPostHomogeneousGenetive(TEntityList& lstAllEntities, TUnitList& lstSentList)
		{
			SS::Core::Features::CMorphoFeature oMorfoPatternGenitive;
			oMorfoPatternGenitive.Undefine();
			oMorfoPatternGenitive.m_CaseType = SS::Core::Features::Values::CaseType.ctGenitive;

			for(TEntityList::iterator itEnt = lstAllEntities.begin(); itEnt != lstAllEntities.end(); itEnt++)		
			{
                if (!(*itEnt)->isUsed)
                    continue;

				if (((*itEnt)->entType == entOrgNoInfo) || ((*itEnt)->entType == entUnknown) ||
                    ((*itEnt)->entType == entNounVerb) || (*itEnt)->IsOgrNameWithoutNameGroup())
				{
					TUnitList::iterator itLastUnit = unitOperator->FindUnit(lstSentList, (*itEnt)->GetLastUnit());
					++itLastUnit;
					if (itLastUnit != lstSentList.end())
					{
						CEntity* pNextEnt = GetEntityByUnit(lstAllEntities, *itLastUnit);
						if (pNextEnt)
						{
							if ((pNextEnt->entType==entUnknown || pNextEnt->entType==entNounVerb || pNextEnt->entType==entUnknownProper) &&
								pNextEnt->isHaveHomogenousAdj && pNextEnt->IsMorpho(oMorfoPatternGenitive))
							{
								CopyHomogeneousUnits(lstAllEntities, *itEnt, pNextEnt->unitList, true);
							}
						}
					}
				}
			}
		}

        void CBaseDigestAnalyzer::EvristicDeleteNounIfAdj(TEntityList& allEntity)
        {
            SS::Core::Features::CMorphoFeature oMorfoPatternAdj;
			oMorfoPatternAdj.Undefine();
            oMorfoPatternAdj.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;	
			oMorfoPatternAdj.m_PartOfSpeechTypes.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postNumeric);	

            for(TEntityList::iterator itEnt = allEntity.begin(); itEnt != allEntity.end(); ++itEnt)
            {
				CEntity *pEnt = *itEnt;
                if(!pEnt->isUsed)
                    continue;

                if (unitOperator->IsMorpho(pEnt->rootUnit, oMorfoPatternAdj)/* ||
                    unitOperator->isEvristicAdj((*itEnt)->rootUnit)*/)
                {
                    bool isNoun = true;
                    TEntityList::iterator it = itEnt;
                    while(isNoun && (it != allEntity.begin()))
                    {
                        --it;
						CEntity *pOther = *it;
                        if(pOther->isUsed && (pOther->begin <= pEnt->begin) && (pOther->end >= pEnt->end))
                        /// родительская цепочка
                        {
                            for(TUnitList::iterator itUnit = pOther->unitList.begin();
                                isNoun && (*itUnit != pOther->rootUnit) && (itUnit != pOther->unitList.end());
                                ++itUnit)
                            {
                                if((*itUnit) == pEnt->rootUnit)
                                    isNoun = false;
                            }
                        }
                        else
                            break;

                    }
                    if(!isNoun)
                        pEnt->isUsed = false;
                }
            }
        }

		void CBaseDigestAnalyzer::EvristicHomogeneousAdj(TEntityList& lstAllEntities, TUnitList& lstSentList)
		{
			SS::Core::Features::CMorphoFeature oMorfoAdj;
			oMorfoAdj.Undefine();

			SS::Core::Features::CMorphoFeature oMorfoAdjPrev;
			oMorfoAdjPrev.Undefine();

			SS::Core::Features::CMorphoFeature oMorfoPatternAdj;
			oMorfoPatternAdj.Undefine();
			oMorfoPatternAdj.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;	
			oMorfoPatternAdj.m_PartOfSpeechTypes.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postNumeric);	

			SS::Core::Features::CMorphoFeature oMorfoPatternParticiple; // причастие
			oMorfoPatternParticiple.Undefine();
			oMorfoPatternParticiple.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;	
			oMorfoPatternParticiple.m_VerbForm = SS::Core::Features::Values::VerbForm.vfParticiple;
			oMorfoPatternParticiple.m_ShortType = SS::Core::Features::Values::ShortType.Undefined;

			SS::Core::Features::CMorphoFeature oMorfoPatternShort; // краткое причастие или прилагательное
			oMorfoPatternShort.Undefine();		
			oMorfoPatternShort.m_ShortType = SS::Core::Features::Values::ShortType.stShort;	

			IUnit* pLastHomogeneousDivider = NULL;

			for(TEntityList::iterator itEnt = lstAllEntities.begin(); itEnt != lstAllEntities.end(); itEnt++)		
			{
				CEntity* curEnt = *itEnt;	
				if (!curEnt->m_strPrefix.empty())
				{
					pLastHomogeneousDivider = NULL;
					TUnitList::iterator itUnit = unitOperator->FindUnit(lstSentList, curEnt->GetFirstUnit());
					unitOperator->GetUnitMorfoFeaturesFull(*itUnit, oMorfoAdj);
					if (itUnit!=lstSentList.end() && itUnit!=lstSentList.begin())
					{						
						while(itUnit!=lstSentList.begin())
						{
							itUnit--;
							IUnit* pUnit = *itUnit;
							wstring str = pUnit->GetWord();
							if (str==L"и" || str==L"или" || str==L",")
							{
								if (pLastHomogeneousDivider!=NULL)
                                    break;
								pLastHomogeneousDivider = pUnit;							
							}
							else if ((unitOperator->IsMorpho(pUnit, oMorfoPatternAdj) || 
                                unitOperator->IsMorpho(pUnit, oMorfoPatternParticiple)) &&
								!unitOperator->IsMorpho(pUnit, oMorfoPatternShort) && 
                                pLastHomogeneousDivider)
							{
								unitOperator->GetUnitMorfoFeaturesFull(*itUnit, oMorfoAdjPrev);							
								if (oMorfoAdj.IsIntersectsByCaseGenderNumber(oMorfoAdjPrev))
								{
									curEnt->unitList.push_front(pLastHomogeneousDivider);
									//curEnt->unitList.push_front(*itUnit);
									curEnt->AddAdjectiveHomogeneousPrefix(*itUnit);
									curEnt->isHaveHomogenousAdj = true;
								}
                                else
                                    break; // накнулись на несогласованное слово - выходим
								pLastHomogeneousDivider = NULL;
							}
							else
                                break;	// не определение или нет союзных юнитов - выходим										
						} //while(itUnit!=lstSentList.begin())
						curEnt->begin = (*curEnt->unitList.begin())->GetPosition();
					}	// if (itUnit!=lstSentList.end() && itUnit!=lstSentList.begin())			
				} // if (!curEnt->m_strPrefix.empty())
			}
		}

		void CBaseDigestAnalyzer::EvristicPersonInfoInsidePersonName(TEntityList& lstEntities, TUnitList& sentList)
		{
			TEntityList newEntity;
			for(TEntityList::iterator it = lstEntities.begin(); it != lstEntities.end(); ++it)
			{
				CEntity* curEnt = (*it);
				if (!curEnt->isUsed || (curEnt->entType != entPersonName))
					continue;

				TUnitList newUnitList;
				for(TUnitList::iterator iter = curEnt->unitList.begin(); iter != curEnt->unitList.end(); ++iter)
				{
					if (curEnt->m_setNameGroupUnits.find(*iter) != curEnt->m_setNameGroupUnits.end())
					/// началась именная группа
					{
						if(newUnitList.size() == 0)
							break;
						wstring lastWord = (*(--newUnitList.end()))->GetWord();
						if(lastWord.empty())
							newUnitList.pop_back();
						if(newUnitList.size() == 0)
							break;
						CEntity* ent = new CEntity(unitOperator, curEnt->m_lang);
						ent->rootUnit = curEnt->rootUnit;
						ent->nType = curEnt->nType;
						for(TUnitList::iterator it1 = newUnitList.begin(); it1 != newUnitList.end(); ++it1)
							ent->unitList.push_back(*it1);
						ent->begin = (*ent->unitList.begin())->GetPosition();
						ent->end = (*(--ent->unitList.end()))->GetPosition();
						SetEntityFullType(ent, lstEntities);
						if (ent->entType == entPlaceNoInfo)
							ent->isUsed = false;
						newEntity.push_back(ent);
						break;
					}
					else
						newUnitList.push_back(*iter);
				}
			}
			for(TEntityList::iterator it = newEntity.begin(); it != newEntity.end(); ++it)
				lstEntities.push_back(*it);
		}

		bool CBaseDigestAnalyzer::IsBadOrgPrefixWord(IUnit *pUnit)
		{
			wstring str = unitOperator->GetMainFormText1(pUnit,SS::Core::Features::Values::GenderType.Undefined).form;
			wstring strLo = unitOperator->GetLowerWord(str);
			if (strLo == L"указ" || strLo == L"закон" || strLo == L"предписание" ||
				strLo == L"книга" || strLo ==L"журнал" || strLo == L"система" || strLo == L"программа" ||
				strLo == L"передача" || strLo == L"шоу" || strLo == L"телепередача" || strLo == L"марка" ||
				strLo == L"самолет" || strLo == L"ледокол" || strLo == L"лайнер" || strLo==L"аэробуc" ||
				strLo == L"автомобиль" || strLo == L"дело")
			{
					return true;
			}
			return false;
		}

		void CBaseDigestAnalyzer::EvristicDelBadOrg(TEntityList& lstEntities, TUnitList& sentList)
		{
			for(TEntityList::iterator it1 = lstEntities.begin(); it1 != lstEntities.end(); ++it1)
			{
				CEntity* curEnt = (*it1);
				if (!curEnt->isUsed)
                    continue;
				if (curEnt->entType == entPersonName)
				{
					IUnit *pUnit = curEnt->rootUnit;
					wstring str = pUnit->GetWord();
					SS::Core::Features::CSemanticFeature oSemantic;
					unitOperator->GetUnitSemanticFeatures(pUnit, oSemantic);
					if (oSemantic.m_SPersonType.Equal(SS::Core::Features::Values::TSPersonType.semprstLastName) && 
						!pUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase() &&
						!str.empty() && !pUnit->GetEvristicLN())
					{
						curEnt->isUsed = false;
					}
				}

				if (curEnt->entType == entOrgName)
				{
					bool isFirstQuote = false;
					for(TUnitList::iterator it = curEnt->unitList.begin(); it != curEnt->unitList.end(); ++it)
					{
						wstring str = (*it)->GetWord();
						if (str.size()==0)
                            continue;
						if (unitOperator->IsQuote(str) || unitOperator->IsEnglishWord(*it))
                            isFirstQuote = true;
						break;
					}
                    if (isFirstQuote)
					{
						TUnitList lstParents;
						IUnit* pParent = curEnt->rootUnit->GetParentUnit();
						while(pParent)
						{
							lstParents.push_back(pParent);
							pParent = pParent->GetParentUnit();
						}
						SS::Core::Features::CMorphoFeature oMorfoPattern;

						if (lstParents.size()==0) // значит не связалось и надо искать терм перед кавычками
						{
							TUnitList::iterator itFirst = unitOperator->FindUnit(sentList, *curEnt->unitList.begin());
							if (itFirst != sentList.begin())
							{
								--itFirst;
								pParent = *itFirst;
								lstParents.push_back(pParent);
							}
						}

						oMorfoPattern.Undefine();
						oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;

						for(TUnitList::iterator it = lstParents.begin(); it!=lstParents.end(); it++)
						{
							pParent = *it;
							if (pParent && unitOperator->IsMorpho(pParent, oMorfoPattern) &&
								IsBadOrgPrefixWord(pParent) &&
								!(unitOperator->IsSemantic(pParent, SS::Core::Features::Values::TSemanticType.smtOrganization) ||
								unitOperator->IsSemantic(pParent, SS::Core::Features::Values::TSemanticType.smtPerson))
								&& !curEnt->IsSingleOrgInQuote())
							{
								curEnt->entType = entUnknownProper;
								//curEnt->isUsed = fal;
								break;
							}
						}
					}
				}
			}
		}

		void CBaseDigestAnalyzer::EvristicDelBadProper(TEntityList& lstEntities)
		{
			for (TEntityList::iterator it1 = lstEntities.begin(); it1 != lstEntities.end(); it1++)		
			{
				CEntity* pEntity = (*it1);
				if (pEntity->isUsed && (pEntity->unitList.size() == 1) &&
					(pEntity->entType == entUnknownProper || pEntity->entType == entPersonName))
				{
					IUnit *pPreviousUnit = pEntity->rootUnit->GetLeftUnit();
					if (!pPreviousUnit || !unitOperator->IsFirstSentenceUnit(pPreviousUnit))
						continue;
					wstring word = pPreviousUnit->GetWord();
					SS::Core::Features::CMorphoFeature morpho;
					unitOperator->GetUnitMorfoFeaturesFullByMorphoAnalyzer(pEntity->rootUnit, morpho);
					if ((unitOperator->IsQuote(word) || (word == L"—") || (word == L"-")) &&
						morpho.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postAdverb) &&
						!morpho.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postNoun))
					{
						pEntity->isUsed = false;
					}
				}
			}
		}

		// нефть марки Brent
		void CBaseDigestAnalyzer::EvristicDelBadPerson(TEntityList& lstEntities)
		{
			for(TEntityList::iterator it = lstEntities.begin(); it != lstEntities.end(); ++it)		
			{
				CEntity* pEntity = (*it);
				if (!pEntity->isUsed)
					continue;
				if (pEntity->entType == entPersonInfo || pEntity->entType == entPersonName)
				{
					if (unitOperator->IsSemantic(pEntity->rootUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
						SS::Core::Features::Values::TSPersonType.semprstFirstName))
					{									
						int nPos = pEntity->rootUnit->GetPosition();
						if (nPos >= 0)
						{
							CEntity *nextEnt = GetEntityByUnitPosition(lstEntities, nPos + 1);
							if (nextEnt && (nextEnt->entType == entOrgName))
								pEntity->isUsed = false;
						}
					}
				}
			}
		}

		void CBaseDigestAnalyzer::EvristicHyphenDates(TEntityList& lstEntities, TUnitList& sentList)
		{
			set<CEntity*> processedEnt;
			for(TEntityList::iterator it1 = lstEntities.begin(); it1 != lstEntities.end(); ++it1)		
			{			
				CEntity* curEnt = (*it1);
				if (!curEnt->isUsed)
					continue;
				if (processedEnt.find(curEnt) != processedEnt.end())
					continue;
				if (curEnt->entType == entDate)
				{
					TUnitList::iterator it = unitOperator->FindUnit(sentList, curEnt->GetLastUnit());
					++it;
					if (it != sentList.end())
					{
						wstring str = (*it)->GetWord();
						wstring sHyph = L"";
						sHyph.push_back((wchar_t)8211);
						if (str==L"-" || str==sHyph)
						{
							++it;
							if (it == sentList.end())
								break;
							CEntity* nextEnt = GetEntityByUnit(lstEntities, *it);
							if (nextEnt && (nextEnt->entType == entDate))
							{
								processedEnt.insert(curEnt);
								processedEnt.insert(nextEnt);

								nextEnt->m_DateInterval.begin = curEnt->m_DateInterval.begin;
								curEnt->m_DateInterval.end = nextEnt->m_DateInterval.end;	
							}						
						}
					}
				}
			}
		}


		void CBaseDigestAnalyzer::EvristicFindNextDates(TEntityList& lstEntities, TUnitList& sentList)
		{
			CEntity* lastEnt = NULL;
			set<wstring> prefixesBegin; // предлоги-префиксы для начала интервала
			set<wstring> prefixesEnd; // предлоги-префиксы для конца интервала

			prefixesBegin.insert(L"с");
			prefixesBegin.insert(L"С");		
			prefixesBegin.insert(L"после");
			prefixesBegin.insert(L"После");
			prefixesBegin.insert(L"from");
			prefixesBegin.insert(L"From");	

			prefixesEnd.insert(L"до");		
			prefixesEnd.insert(L"перед");	
			prefixesEnd.insert(L"по");		
			prefixesEnd.insert(L"to");

			for(TEntityList::iterator it1 = lstEntities.begin(); it1 != lstEntities.end(); it1++)		
			{
				CEntity* curEnt = (*it1);
				if (!curEnt->isUsed) continue;
				if (curEnt->entType == entDate)
				{
					curEnt->ProcessValueForDate();
					TUnitList::iterator it = unitOperator->FindUnit(sentList, *curEnt->unitList.begin());
					while(it!=sentList.begin())
					{
						it--;	
						wstring str = (*it)->GetWord();
						if (str.size()!=0) break;					 
					}					
					if ((*it)->GetFirstCharPositionNew()>=curEnt->GetBeginOffset() && it!=sentList.begin())
						it--;					
					wstring str = (*it)->GetWord();
					if (prefixesBegin.find(str)!=prefixesBegin.end())
					{
						curEnt->m_DateInterval.end.ClearDate();
						lastEnt = curEnt;		
					}else
						if (prefixesEnd.find(str)!=prefixesEnd.end())
						{
							if (lastEnt!=NULL)
							{
								curEnt->m_DateInterval.begin = lastEnt->m_DateInterval.begin;
								lastEnt->m_DateInterval.end = curEnt->m_DateInterval.end;						
							}else curEnt->m_DateInterval.begin.ClearDate();
							lastEnt = NULL;
						}else lastEnt = NULL;					
				}else
				{
					lastEnt = NULL;			 	
				} 
			}
		}

		void CBaseDigestAnalyzer::ProcessAbbreviation(IText* pText, TEntityList& lstEntities)
		{
			// ищем аббревиатуры внутри цепочек. Если находим - пытаемся разрешить
			for(TEntityList::iterator it1 = lstEntities.begin(); it1 != lstEntities.end(); it1++)		
			{			
				CEntity* pEntity = *it1;
				for(TUnitList::iterator it2 = pEntity->unitList.begin(); it2 != pEntity->unitList.end(); it2++)
				{
					TUnitList findedUnits;
					IUnit* pUnit = *it2;
					wstring str = pUnit->GetWord();
					if (IsAbbr(pUnit) && str.size()>2)
					{					
						FindAbbrevKorefName(pText, pUnit,findedUnits);													
					}
					pEntity->unitResolutions.push_back(findedUnits);		
				}

			}				
		}

		void CBaseDigestAnalyzer::GetInsideEntities(CEntity* inEnt, TEntityList &lstAll, TEntityList &lstInside)
		{
			for(TEntityList::iterator it = lstAll.begin(); it != lstAll.end(); ++it)
			{
				CEntity *pEnt = *it;
				if (!pEnt->isUsed)
                    continue;
				if (pEnt != inEnt)
				{
					if (pEnt->begin>=inEnt->begin && pEnt->end<=inEnt->end) 
						lstInside.push_back(pEnt);
				}
			}
		}

		wstring CBaseDigestAnalyzer::GetXMLForEntityText(const wstring &srcText, CEntity *inEnt,
			list<CEntity*>& insideEnt, int basic_offset, int sentOffset)
		{
			// insideEnt - список сущностей, который для данной сущности лежат внутри нее.
			// функция рекурсивная.
			int curOffset = basic_offset;
			ATL::CAtlStringW resStr = L"";
			for(list<CEntity*>::iterator it = insideEnt.begin(); it != insideEnt.end(); ++it)
			{
				CEntity *pEntity = *it;
				list<CEntity*> insideEnt1;
				if (pEntity->isProcessed)
                    continue;
				pEntity->isProcessed = true;
				GetInsideEntities(pEntity, insideEnt, insideEnt1);
				ATL::CAtlStringW strEnt = L"";
				if (insideEnt1.size() != 0)
				{
					strEnt = GetXMLForEntityText(srcText, pEntity, insideEnt1, pEntity->GetBeginOffset(), sentOffset).c_str();
				}
				else
				{
					int offset = pEntity->GetBeginOffset();
					int offset1 = pEntity->GetEndOffsetNew(srcText);
					if (offset1 < offset)
					{
                        offset1 = offset;
					}
					wstring cdatastr = unitOperator->GetUnmarkingTextXML(srcText, offset, offset1).c_str();
					if (offset1 - offset == 0)
					{
                        cdatastr.clear();
					}
					strEnt = pEntity->GetXML(cdatastr, sentOffset).c_str();
				}
				int offset = pEntity->GetBeginOffset();
				if (curOffset>offset)
				{
                    curOffset = offset;
				}
				if (offset - curOffset > 0)
				{
					resStr += unitOperator->GetUnmarkingTextXML(srcText, curOffset, offset).c_str();
				}
				resStr += strEnt;
				curOffset = pEntity->GetEndOffsetNew(srcText);
			}
			int offset = inEnt->GetEndOffsetNew(srcText);
			if (curOffset > offset)
			{
                offset = curOffset;
			}
			if (offset - curOffset > 0)
			{
				resStr += unitOperator->GetUnmarkingTextXML(srcText, curOffset, offset).c_str();
			}
			resStr = inEnt->GetXML(resStr.GetBuffer(), sentOffset).c_str();
			return resStr;
		}

		/*void CBaseDigestAnalyzer::ProcessKorefs(IText* pText,
			TEntityList lstEntities)
		{
			for (BlackBox::IParagraph* paragraph = pText->GetFirstParagraph(); paragraph != NULL; paragraph = paragraph->GetNextParagraph())
			{
				for (BlackBox::ISentence* sentence = paragraph->GetFirstSentence(); sentence != NULL; sentence = sentence->GetNextSentence())
				{
					sentence->SetEnabled(0);
				}
			}

			list<ISentence*> sentList;

			for(IParagraph* pPar = pText->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
			{														
				for(ISentence* pSen = pPar->GetFirstSentence(); pSen; pSen = pSen->GetNextSentence())
				{
					for(TEntityList::iterator it1 = lstEntities.begin(); it1 != lstEntities.end(); it1++)		
					{			
						CEntity* pEntity = *it1;
						wstring strProfLo = _wcslwr((wchar_t*)pEntity->m_strProfession.c_str());
						if (pEntity->entType == entPersonNoInfo)
						{
							//  надо найти все такие профессии, фамилии, отчества  и имена в тексте.
							TUnitList sentUnits;
							pSen->GetLinearUnitsFull(sentUnits);
							int index = 0;
							sentList.push_back(pSen);
							for(TUnitList::iterator it = sentUnits.begin(); it!=sentUnits.end(); it++)
							{
								wstring str = unitOperator->GetMainFormText1(*it, SS::Core::Features::Values::GenderType.Undefined);
								wstring strLo = _wcslwr((wchar_t*)str.c_str());
								if (strLo==strProfLo)
								{
									pSen->SetEnabled(1);
									sentList.push_back(pSen);
								}
							}							
						}
					} // циул по сущностям
				} // цикл по предложения
			} // цикл по парагрфам

			m_pSyntaxAnalyser->AnalyzeNext( pText); // синанализ тех предложений, которые еще не анализировались, но
			// в которых есть возможное разрешение			
		}*/

		bool CBaseDigestAnalyzer::IsSameTypeParentEntity(CEntity* pEnt, TEntityList& allEntityInSent)
		{
			for(TEntityList::iterator it =  allEntityInSent.begin(); it!= allEntityInSent.end(); it++)
			{
                CEntity *pParent = *it;
				if (pEnt == pParent)
					break;
				else if (pParent->isUsed && (pParent->begin <= pEnt->begin) && (pParent->end >= pEnt->end))
                /// родительская сущность
				{
					bool parentIsOrg = (pParent->nType == eEntityBaseType::Organization) && (pParent->entType != entOrgNoInfo);
					bool entityIsOrg = (pEnt->nType == eEntityBaseType::Organization) && (pEnt->entType != entOrgNoInfo);
					bool parentIsPlace = (pParent->nType == eEntityBaseType::Place) && (pParent->entType != entPlaceNoInfo);

                    if (parentIsPlace && ((pEnt->nType == eEntityBaseType::Person) || entityIsOrg))
                        return true; /// персоны и орги внутри географии не берем
                    if (parentIsOrg && ((pEnt->nType == eEntityBaseType::Person) || entityIsOrg))
                    /// персоны внутри организаций не берем
                    /// ПОКА не берем организации внутри организаций
                    {
                        return true;
                    }
                    if ((pParent->nType == pEnt->nType) && ((pEnt->entType == entPlaceName)))
                    /// отсеиваем некоторые одинаковые типы
                    {
                        return true;
                    }
                    if ((pParent->entType == entPersonName) &&
                        ((pEnt->entType == entPersonName) || (pEnt->entType == entPersonNoInfo) ||
						(pEnt->entType == entDate)))
                    /// отсеиваем некоторые типы внутри PersonName
                    {
                        return true;
                    }
					if ((pEnt->nType == eEntityBaseType::Other) && (parentIsOrg || parentIsPlace))
						return true;

					/// даты внутри сущностей не выделяем
					if ((pEnt->nType == eEntityBaseType::Date) && pParent)
						return true;
				}				
			}
			return false;
		}

		void CBaseDigestAnalyzer::SelectEntitiesWithSyntax(
			TUnitList &units,
			TEntityList& allEntityInSent,
			const wstring &baseDate,
			SS::Core::Types::EAnalyseLanguage language)
		{
			SS::Core::Features::CMorphoFeature oMorpho;
			oMorpho.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
			oMorpho.m_PartOfSpeechTypes.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postPronoun);
			oMorpho.m_PartOfSpeechTypes.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postVerb);

			CEntity* pEnt = 0;
			for(TUnitList::iterator it = units.begin(); it != units.end(); ++it)
			{
				IUnit *pUnit = *it;
				if (!unitOperator->IsMorpho(pUnit, oMorpho))
					continue;

                eEntityBaseType type = eEntityBaseType::Default;
				if (pUnit->GetReferenceUnit())
					type = GetEntityType(pUnit->GetReferenceUnit());
				else
					type = GetEntityType(pUnit);

                if ((it == units.begin()) && (language == SS::Core::Types::EAnalyseLanguage::ealEnglish))
                {
                    if ((type == eEntityBaseType::NounVerb) || (type == eEntityBaseType::Other) ||
                        (type == eEntityBaseType::Person))
                    {
                        UnitReclassificationOnEnglish(units, type);
                    }
                }


                if (type != eEntityBaseType::Default)
				{
					pEnt = new CEntity(unitOperator, language);
					pEnt->SetDate(baseDate);
					pEnt->rootUnit = pUnit;
					pEnt->rootType = type;
                    if (type != eEntityBaseType::Verb)
						unitOperator->GetLinearUnitsWithSort(pEnt->rootUnit, pEnt->unitList);
					else
						GetVerbUnitList(pUnit, pEnt->unitList);

					if (pUnit->GetReferenceUnit())
						unitOperator->GetLinearUnitsWithSort(pUnit->GetReferenceUnit(), pEnt->unitListRef);
					if (type != eEntityBaseType::Verb)
						pEnt->DeleteUncorrectUnitsFromBorders();

					if (!pEnt->unitList.empty())
					{
						bool isFindRoot = false;
						bool isContainChainInQuote = false;
						int position = -1;
						for(TUnitList::iterator iter = pEnt->unitList.begin(); iter != pEnt->unitList.end(); ++iter)
						{
							IUnit *pUnit = *iter;
							int pos = pUnit->GetPosition();
							if ((position < 0) || (pos - position <= 1))
								position = pos;
							else if (isFindRoot)
							{
								pEnt->unitList.erase(iter, pEnt->unitList.end());
								break;
							}
							else
							{
								pEnt->unitList.erase(pEnt->unitList.begin(), iter);
								isContainChainInQuote = false;
								position = pos;
							}
							if (unitOperator->IsQuoteParentUnit(pUnit))
							{
								if (isContainChainInQuote)
								{
									if (isFindRoot)
									{
										pEnt->unitList.erase(iter, pEnt->unitList.end());
										break;
									}
									else
									{
										pEnt->unitList.erase(pEnt->unitList.begin(), iter);
										position = pos;
									}
								}
								else
									isContainChainInQuote = true;
							}
							if (pEnt->rootUnit == *iter)
								isFindRoot = true;
						}

						if (!pEnt->unitList.empty())
						{
							pEnt->begin = (*pEnt->unitList.begin())->GetPosition();
							pEnt->end = (*(--pEnt->unitList.end()))->GetPosition();
							pEnt->nType = GetEntityType(pEnt->rootUnit, pEnt->unitList, pEnt->rootType);
							allEntityInSent.push_back(pEnt);
							pEnt = 0;
						}
					}
				}

				if(pEnt)
                {
                    delete pEnt;
                    pEnt = 0;
                }
			}
			allEntityInSent.sort(SortByPosInSentForEntity); // сортируем сущности предложения слева направо
		}

		void CBaseDigestAnalyzer::GetEntityForSent(
			ISentence* pSen,
			TEntityList& allEntityInSent,
			TUnitList& linkedPlaces,
			const wstring &baseDate)
		{
			TUnitList sentUnits;
			pSen->GetLinearUnitsFull(sentUnits);
			SaveDefisWordMorpho(sentUnits);

			SelectEntitiesWithSyntax(sentUnits, allEntityInSent, baseDate, pSen->GetLanguage());
            
			for(TEntityList::iterator it = allEntityInSent.begin(); it != allEntityInSent.end(); ++it)
			{
				CEntity* ent = *it;
				SetEntityFullType(ent, allEntityInSent);
				EvristicCutPronounUnits(ent, allEntityInSent, pSen);
				if (!ent->unitList.empty())
					EntityNormalization(ent, allEntityInSent);
				ent->isUsed = ent->IsGoodEnt() && ent->IsHaveNecessaryNoun();
			}

			EvristicDeleteShortWordForm(allEntityInSent);
			EntityTreeCorrection(allEntityInSent);
			EvristicDelBadOrg(allEntityInSent, sentUnits);
			EvristicDelBadPerson(allEntityInSent);
			EvristicDelBadProper(allEntityInSent);
			EvristicDeletePersonFromOrg(allEntityInSent);
			EvristicSkobkiDelete(allEntityInSent);
			EvristicPersonOrgOrPlaceUnion(allEntityInSent);
			EvristicOrgGeoAbbrUnion(allEntityInSent);
			EvristicOgrNoInfoPersonrUnion(allEntityInSent);
			EvristicCDelimiterRus(allEntityInSent);
            EvristicDeleteNounIfAdj(allEntityInSent);
			EvristicHomogeneousAdj(allEntityInSent, sentUnits);
			EvristicHomogeneous(allEntityInSent, sentUnits, false);
			EvristicPostHomogeneousGenetive(allEntityInSent, sentUnits);
			EvristicPersonUnitRus(allEntityInSent);
			EvristicChangeDateToPlace(allEntityInSent);
			EvristicRegionDelim(allEntityInSent);
			EvristicFrom_ToDates(allEntityInSent, sentUnits);
			EvristicFindNextDates(allEntityInSent, sentUnits);
			EvristicHyphenDates(allEntityInSent, sentUnits);
			EvristicPersonInfoInsidePersonName(allEntityInSent, sentUnits);
			EvristicPersonNameAndRomanNumberUnion(allEntityInSent, sentUnits);
			EvristicDeleteBadDates(allEntityInSent);
            EvristicConvertBadDates(allEntityInSent);
			EvristicDeletePlaceFromOrg(allEntityInSent);
			EntityTreeCorrection(allEntityInSent);

            EntityNormalization(allEntityInSent);
			for(TEntityList::iterator it = allEntityInSent.begin(); it != allEntityInSent.end(); ++it)		
			{
				CEntity *pEntity = *it;
				pEntity->isUsed = pEntity->IsGoodEnt() && pEntity->IsHaveNecessaryNoun();
			}
			EvristicSelectCeterus(pSen, allEntityInSent, sentUnits);
            allEntityInSent.sort(SortByPosInSentForEntity);
			
			CEntity *pPrevious = 0;
			int start = 0;
			int end = 0;
			CEntity* pHead = 0;
			for(TEntityList::iterator it = allEntityInSent.begin(); it != allEntityInSent.end(); ++it)		
			{
				CEntity* pEnt = *it;
				if (pPrevious && pEnt->isUsed)
					pEnt->isUsed = (pPrevious->begin != pEnt->begin) || (pPrevious->end != pEnt->end);
				if (pEnt->isUsed)
				{
					if (pEnt->entType == entPlaceName)
						for(TUnitList::iterator itU = linkedPlaces.begin(); itU != linkedPlaces.end(); ++itU)
							pEnt->lstLinkUnits.push_back(*itU);
					pPrevious = pEnt;

					if((pEnt->begin >= start) && (pEnt->end <= end))
					{
						if (pHead && (pHead->entType == entPersonName) && (pEnt->entType == entPersonName))
							pEnt->isUsed = false;
					}
					else
					{
						pHead = pEnt;
						start = pEnt->begin;
						end = pEnt->end;
					}
				}
			}
		}

		void CBaseDigestAnalyzer::GetVerbUnitList(SS::Interface::Core::BlackBox::IUnit *pRoot, TUnitList &unitList)
		{
			unitList.clear();
			wstring word = unitOperator->GetLowerWord(pRoot->GetWord());
			if(word == L"хотя")
                return;

            bool isCombiVerb = unitOperator->IsCombiUnit(pRoot);
            if(!isCombiVerb && unitOperator->IsInCombi(pRoot))
                return;

			if (isCombiVerb)
				unitOperator->GetLinearUnitsWithSort(pRoot, unitList);
			else
				unitList.push_back(pRoot);
		}

		void CBaseDigestAnalyzer::EntityTreeCorrection(TEntityList &allEntity)
		{
			int lastStart = 0;
			int lastEnd = 0;
			for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); ++it)
			{
				CEntity *pEntity = *it;
				if(!pEntity->isUsed)
					continue;
				if((it != allEntity.begin()) && (pEntity->end <= lastEnd))
				/// сущность принадлежит текущей ветви сущностей
				{
					TEntityList::iterator iter = it;
					--iter;
					while((*iter)->begin >= lastStart)
					/// проход по ветви сущностей вверх
					{
						CEntity *pParentEntity = *iter;
						if (pParentEntity->isUsed)
						{
							if ((pEntity->begin >= pParentEntity->begin) && (pEntity->end <= pParentEntity->end))
							/// сущность полностью входит в другую
							{
								if(!IncludeEntityInside(pParentEntity, pEntity))
									break;
							}
						}
						if(iter == allEntity.begin())
							break;
						--iter;
					}
				}
				if(pEntity->end > lastEnd)
				/// новая ветка сущностей
				{
					lastStart = pEntity->begin;
					lastEnd = pEntity->end;
				}
			}
		}

        void CBaseDigestAnalyzer::EntityNormalization(TEntityList &allEntity)
        {
			for(TEntityList::reverse_iterator it = allEntity.rbegin(); it != allEntity.rend(); ++it)
			{
				CEntity *pEntity = *it;
				if (pEntity->isUsed)
					EntityNormalization(pEntity, allEntity);
            }
        }

        void CBaseDigestAnalyzer::EntityNormalization(CEntity *pEntity, TEntityList &allEntity)
        {
            pEntity->GetNormalString();

            if (!((pEntity->m_lang == SS::Core::Types::EAnalyseLanguage::ealEnglish) &&
                ((pEntity->entType == EEntityType::entUnknown) || 
                (pEntity->entType == EEntityType::entUnknownProper))))
            {
                return;
            }
            
            TEntityList inside;
            GetInsideEntities(pEntity, allEntity, inside);

            if(inside.size() == 0)
                return;
            
            CEntity* pLast = *inside.rbegin();

            if (pLast->nounUnit == pEntity->nounUnit)
            /// подсущность у правого края
            {            
                pEntity->m_strPrefix = pLast->m_strPrefix;
                if(pEntity->m_strPrefix.empty())
                {
                    for(TUnitList::iterator iter = pEntity->unitList.begin();
                        (iter != pEntity->unitList.end()) && (*iter != pEntity->rootUnit);
                        ++iter)
                    {
                        wstring word = unitOperator->GetMainFormText1(*iter, SS::Core::Features::Values::GenderType.Undefined).form;
                        pEntity->m_strPrefix += word + L" ";
                    }
					pEntity->m_strPrefix = unitOperator->SpaceNormalize(pEntity->m_strPrefix);
					pEntity->m_strPrefix = unitOperator->QuoteNormalize(pEntity->m_strPrefix);
                }
            }
            else if (pLast->rootUnit == pEntity->rootUnit)
            /// подсущность у левого края
            {
                pEntity->m_strPrefix = L"";
                bool nounFind = false;
                for(TUnitList::iterator iter = pEntity->unitList.begin();
                    (iter != pEntity->unitList.end()) && (*iter != pEntity->nounUnit);
                    ++iter)
                {
                    if((*iter) == pLast->nounUnit)
                        nounFind = true;
                    else if(nounFind)
                    {
                        wstring word = unitOperator->GetMainFormText1(*iter, SS::Core::Features::Values::GenderType.Undefined).form;
                        pEntity->m_strPrefix += word + L" ";
                    }
                }
            }
        }

		wstring CBaseDigestAnalyzer::GetFactXML(const wstring &srcText, IText* pText, bool bFullText, bool bFullAnalyze, TEntityList& allEntity)
		{
			SS_TRY;

			TUnitList sentUnits;
			ATL::CAtlStringW resXML = L"<TEXT>";
			int textSize = srcText.size();
			TEntityList allEntityInSent;

			int entityID = 0;
			TEntityList::iterator entityIter = allEntity.begin();

#ifdef WITH_LOG
			ofstream filestr("D:\\NLP\\Lingvistic2012\\log.txt", fstream::app);
			filestr << std::endl << "RDF" << std::endl;
#endif
			int sentenceIndex = 0;
			for(TSentList::iterator it = m_lSentences.begin(); it != m_lSentences.end(); ++it)
			{
#ifdef WITH_LOG
				filestr << "save sentence №" << sentenceIndex << std::endl;
				filestr.flush();
#endif
				ATL::CAtlStringW sentText = L"<SENT";
				
				ISentence *pSentence = (*it).pSentence;
				int cur_offset = 0;
				if (it != m_lSentences.begin())
					cur_offset = unitOperator->GetSentenceOffset(pSentence);
				int next_offset = textSize;
				TSentList::iterator itSentNext = it;
				++itSentNext;
				if (itSentNext != m_lSentences.end())
					next_offset = unitOperator->GetSentenceOffset((*itSentNext).pSentence);

				sentText += L" POS=\"";
				sentText.AppendFormat(L"%d", cur_offset);
				sentText += L"\" TYPE=\"";
				sentUnits.clear();
				pSentence->GetLinearUnitsFull(sentUnits);
				if (!pSentence->IsPreDigest())
                {
					if (bFullText)
					{
						sentText += L"none\">";
						if (next_offset - cur_offset > 0)
						{
							sentText += unitOperator->GetUnmarkingTextXML(srcText, cur_offset, next_offset).c_str();
							sentText += L"</SENT>";
						}
						resXML += sentText;
					}
					continue;
				}
				sentText += L"fact\"";
				if (pSentence->GetLanguage() == SS::Core::Types::ealEnglish)
					sentText += L" LANG=\"EN\">";
				else
					sentText += L" LANG=\"RU\">";
				IUnit* pUnit = pSentence->GetFirstUnit();

				int unitOffset = cur_offset;
				allEntityInSent.clear();
				
				for(; entityIter != allEntity.end(); ++entityIter)
				{
					CEntity *pEntity = *entityIter;

					if (pEntity->m_pSent == pSentence)
                        allEntityInSent.push_back(pEntity);
					else
						break;

					if (pEntity->isUsed) 
					{
						pEntity->ID = entityID;
						++entityID;
						for(TEntityList::iterator itSub = pEntity->m_lstSubEntites.begin();
                            itSub != pEntity->m_lstSubEntites.end();
                            ++itSub)
						{
							(*itSub)->ID = entityID;
							++entityID;
						}
					}
				}

				/*if (bFullAnalyze)
					ProcessAbbreviation(pText, allEntityInSent);*/
				//FindRootRegions(allEntityInSent, pSen); 

				int lastOffset = 0;
				for(TEntityList::iterator itEnt = allEntityInSent.begin(); itEnt != allEntityInSent.end(); ++itEnt)
				{
					CEntity *pEntity = *itEnt;
					if (pEntity->isUsed)
					{
						int offset = pEntity->GetBeginOffset();
						if (unitOffset > offset)
							continue;

						list<CEntity*> insideEnt;
						GetInsideEntities(*itEnt, allEntityInSent, insideEnt);
						if ((offset != 0) && (offset - unitOffset >= 0))
							sentText += unitOperator->GetUnmarkingTextXML(srcText, unitOffset, offset).c_str();
						sentText += GetXMLForEntityText(srcText, pEntity, insideEnt, pEntity->GetBeginOffset(), cur_offset).c_str();	
						unitOffset = pEntity->GetEndOffsetNew(srcText);
					}
				}
				if (next_offset - unitOffset > 0)
					sentText += unitOperator->GetUnmarkingTextXML(srcText, unitOffset, next_offset).c_str();
				sentText += L"</SENT>";
				resXML += sentText;
				++sentenceIndex;
			}

			for (TEntityList::iterator itEnt = allEntity.begin(); itEnt != allEntity.end(); ++itEnt)
            {
				resXML += (*itEnt)->GetSubthemesXML(srcText).c_str();
                delete (*itEnt);
            }

			resXML += L"</TEXT>";
#ifdef WITH_LOG
			filestr.close();
#endif
			return resXML.GetBuffer();
			SS_CATCH(L"");
		}

		void CBaseDigestAnalyzer::SyntaxAnalyzation(
			const wstring &srcText,
			SS::Interface::Core::BlackBox::IText* text,
			const wstring &baseDate,
			bool languageAnalaze/* = false*/)
		{
#ifdef WITH_LOG
			ofstream filestr("D:\\NLP\\Lingvistic2012\\log.txt", fstream::app);
			filestr << std::endl << "syntax analyzation start" << std::endl;
#endif
			int sentenceIndex = 0;
			for (TSentList::iterator it = m_lSentences.begin(); it != m_lSentences.end(); ++it)
			{
#ifdef WITH_LOG
				filestr << "syntax sentence №" << sentenceIndex;
				filestr.flush();
#endif
				SentenceInfo *pInfo = &(*it);
				ISentence *pSen = pInfo->pSentence;
				if (!pSen->isEnabled() || (pSen->GetProcessed() != eProcessed::None))
					continue; // предложение не нужно для синтанализа

				m_pSyntaxAnalyser->AnalyzeNext(pSen);
				++sentenceIndex;
#ifdef WITH_LOG
				filestr << " - finish" << std::endl;
				filestr.flush();
#endif
			}
#ifdef WITH_LOG
			filestr << "syntax analyzation finish" << std::endl;
			filestr.close();
#endif
		}

		wstring CBaseDigestAnalyzer::GetFactXML(
			const wstring &srcText,
			IText* pText,
            list<int> &offsets,
			const wstring &baseDate,
			bool isTonality,
            bool bFullText,
            bool bFullAnalyze)
		{
			SS_TRY;

			TEntityList allEntity;
			unitOperator->ClearMorphoInformation();
			GetEntitiesFromText(allEntity, srcText, pText, offsets, baseDate, isTonality, bFullAnalyze);
			return GetFactXML(srcText, pText, bFullText, bFullAnalyze, allEntity);

			SS_CATCH(L"");
		}

		wstring CBaseDigestAnalyzer::GetEntityHash(const wstring &srcText, IText* pText)
		{
			TEntityList allEntity;
			list<int> offsets;
			GetEntitiesFromText(allEntity, srcText, pText, offsets, L"", false, false);

			CEntity *hashEnt = 0;
			for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); ++it)
			{
				CEntity* ent = *it;
				if (ent->isUsed && (ent->entType==entPersonInfo || ent->entType==entPersonName ||
					ent->entType==entOrgName || ent->entType==entPlaceName)) 				
				{
					hashEnt  = ent;
					break;
				}
			}

			wstring resStr = L"";
			if (hashEnt)
			{
				if (hashEnt->entType == entPlaceName )  
				{
					hashEnt->entType = entOrgName;
					if (hashEnt->unitList.size() == 1)
						resStr += L"_";
				}
                wstring str = hashEnt->GetNormalString();
				resStr += hashEnt->GetHash();
				if (resStr.empty())
					resStr += hashEnt->GetDefaultNormalString();
				resStr += L";" + str;		  
			}
			else
				resStr = L"";
			return resStr;
		}

		void CBaseDigestAnalyzer::GetEntitiesFromText(
			TEntityList& allEntity,
			const wstring& srcText,
			IText* pText,
            list<int>& offsets,
			const wstring& baseDate,
			bool isTonality,
            bool bFullAnalyze)
		{
			// пометили предложения для дополнительной обработки синтаном и выделения сущностей
			MarkSentByOffset(pText, offsets, isTonality, isTonality);
			/// теперь синтанализ
			SyntaxAnalyzation(srcText, pText, baseDate, true);

			TUnitList linkedPlaces;
#ifdef WITH_LOG
			ofstream filestr("D:\\NLP\\Lingvistic2012\\log.txt", fstream::app);
			filestr << std::endl << "NER" << std::endl;
#endif
			int sentenceIndex = 0;
			for (TSentList::iterator it = m_lSentences.begin(); it != m_lSentences.end(); ++it)
			{
#ifdef WITH_LOG
				filestr << "NER sentence №" << sentenceIndex << std::endl;
				filestr.flush();
#endif
				SentenceInfo* pInfo = &(*it);
				ISentence* pSentence = pInfo->pSentence;
				if (!pSentence->isEnabled())
				{
					continue;
				}
				if (pSentence->GetProcessed() != eProcessed::Entities)
				/// сущности для предложения еще не получены
				{
					if (bFullAnalyze)
					{
						ProcessRegion(pSentence, linkedPlaces);
					}
					GetEntityForSent(pSentence, pInfo->sentenceEntities, linkedPlaces, baseDate);
				}

				for (TEntityList::iterator iter = pInfo->sentenceEntities.begin(); iter != pInfo->sentenceEntities.end(); ++iter)
				{
					CEntity *pEntity = *iter;
					wstring value = pEntity->GetValue();
					if (value.length() > m_MaxEntityLength)
					{
						continue;
					}
					pEntity->m_pSent = pSentence;
					if (pEntity->isUsed)
					{
						allEntity.push_back(pEntity);
					}
				}
				++sentenceIndex;
			}
#ifdef WITH_LOG
			filestr.close();
#endif
		}

		wstring CBaseDigestAnalyzer::GetEntityNormalString(CEntity *pEnt)
		{
			wstring res = L"";
			for (TUnitList::iterator it = pEnt->unitList.begin(); it != pEnt->unitList.end(); it++)	
			{
				IUnit* unit = *it;
				wstring word = unit->GetWord();
				int ind1 = word.find(L".",0);
				int ind2 = word.find(L".",ind1+1);
				if (ind1>=0 && ind2>=0) continue;
				res += word + L" ";
			}	
			return res;
		}

		void CBaseDigestAnalyzer::AnalyzeNextBeforeSynt(IText* text, list<int>& offsets)
		{		
			MarkSentByOffset(text, offsets);
			for (list<TCitation*>::iterator it2 = allCitata.begin(); it2 != allCitata.end(); ++it2)
			{
				delete (*it2);
			}

			for (list<CDigestCitation*>::iterator it2 = allDigestCitata.begin(); it2 != allDigestCitata.end(); ++it2)
			{
				delete (*it2);
			}
			allCitata.clear();
			allDigestCitata.clear();		
			DoCitation1(text);		
		}

		bool CBaseDigestAnalyzer::MarkTone(ISentence* pSent, wstring& dictName)
		{
			TUnitList findedUnits;
			CToneDictPool *tdp = this->m_ToneDictMap[dictName];	
			tdp->stopwordsUnits->FindObjectInSentence(pSent, findedUnits, false, NULL); 
			if (findedUnits.size()!=0) return false;

			findedUnits.clear();
			tdp->adjectivesUnits->FindObjectInSentence(pSent, findedUnits, false, NULL);
			tdp->nounsUnits->FindObjectInSentence(pSent, findedUnits, false, NULL);
			tdp->nounsgenerativeUnits->FindObjectInSentence(pSent, findedUnits, false, NULL);
			tdp->objectnounsUnits->FindObjectInSentence(pSent, findedUnits, false, NULL);
			tdp->verbsUnits->FindObjectInSentence(pSent, findedUnits, false, NULL);
			findedUnits.clear();
			tdp->inversUnits->FindObjectInSentence(pSent, findedUnits, false, NULL);
			for(TUnitList::iterator it =findedUnits.begin();  it != findedUnits.end(); ++it)
			{
				IUnit *unit = *it;
				unit->SetInversion(-1);
			}	
			findedUnits.clear();							
			ReleaseSimpleInversions1(pSent);
			return true;
		}
		
		wstring CBaseDigestAnalyzer::GetXMLForSent(ISentence* pSent, int id, int nCitType, bool isLast)
		{
			wstring tone_teg_begin = L"<TONE";
			wstring cit_teg_begin = L"<CIT";
			wstring tone_teg_end = L"</TONE> ";
			wstring cit_teg_end = L"</CIT>";
			wstring sent_begin = L"<SENT ID=";
			wstring sent_end = L"</SENT> ";

			wstring senttext = L"";

			wchar_t sID[10];
			wsprintfW(sID, L"%d", id);		

			wchar_t sCode[10];
			wsprintfW(sCode, L"%d",nCitType);		

			TUnitList sentUnits;
			TUnitList findedUnits;
			TUnitList findedUnits1;
			TUnitList findedUnits2;
			TUnitList findedUnits3;
			pSent->GetLinearUnitsNonEmpty(sentUnits);	

			if ((nCitType==1 && isLast) || (nCitType>1)) 
			{
				citataVerbs->FindObjectInSentence(pSent, findedUnits1, false, NULL);
				citataFirstTermFour1->FindObjectInSentence(pSent, findedUnits2, false, NULL);
				citataFirstTermFour2->FindObjectInSentence(pSent, findedUnits3, false, NULL);		
			}

			int index = 0;
			for(TUnitList::iterator it = sentUnits.begin(); it!=sentUnits.end(); it++)
			{
				index++;
				IUnit* pUnit = *it;
				if (senttext.size()>0) senttext += L" ";
				wchar_t sTone[10];				
				int tone = pUnit->GetTone();
				int inv = pUnit->GetInversion();
				wsprintfW(sTone, L"%d", tone);								
				wstring strTone = sTone;
				if (tone!=0 || inv==-1)
				{
					wchar_t sTone[10];									    
					wsprintfW(sTone, L"%d", tone);								
					wstring strTone = sTone;				
					if (inv==-1) strTone = L"0";
					senttext += tone_teg_begin  + L" WEIGHT=\"" + strTone + L"\">" + pUnit->GetWord() + tone_teg_end;				
				} 
				else
				{   wchar_t sPos[10];
				wsprintfW(sPos, L"%d", index);	

				wstring citata_tag = L"";
				if (unitOperator->FindInUnits(findedUnits1, pUnit))
					citata_tag = L"verb";
				else if (unitOperator->FindInUnits(findedUnits2, pUnit))
					citata_tag = L"anfr";
				else if (unitOperator->FindInUnits(findedUnits3, pUnit))
					citata_tag = L"next";				
				if (citata_tag.size()!=0)
					senttext += cit_teg_begin + L" TYPE=\"" + citata_tag + L"\" OFFSET=\"" + sPos + L"\" CODE=\"" + sCode + L"\">" +  pUnit->GetWord() + cit_teg_end;
				else
				{
					senttext += pUnit->GetWord();
				}
				}
			}
			return sent_begin + L"\"" + sID + L"\" THEME=\"" + pSent->GetMarkers()  +L"\">" + senttext + sent_end;				
		}

		wstring CBaseDigestAnalyzer::GetSentDigestXML(IText* pText, list<int>& offsets, list<int>& themes, wstring& dictName)
		{
			wstring digesttext = L"<DIGEST>";
			wstring teg_end = L"</DIGEST>";
			wstring objteg_begin =  L"<OBJ>";
			wstring objteg_end =  L"</OBJ> ";
			wstring tone_teg_begin = L"<TONE";
			wstring tone_teg_end = L"</TONE> ";
			wstring sent_begin = L"<SENT ";
			wstring sent_end = L"</SENT> ";
			wstring sent_tone_neg = L" NEG=\" ";
			wstring sent_tone_pos = L" POS=\" ";
			wstring teg_beg = L"<DIGEST>";
			int id = 1;

			MarkSentByOffset1(pText, offsets, themes);
			for (list<TCitation*>::iterator it2 = allCitata.begin(); it2!=allCitata.end(); ++it2)
			{
				delete (*it2);
			}

			for (list<CDigestCitation*>::iterator it2 = allDigestCitata.begin(); it2!=allDigestCitata.end(); ++it2)
			{
				delete (*it2);
			}
			allCitata.clear();
			allDigestCitata.clear();		
			DoCitation1(pText, true);		// нашли все цитаты в тексте
		
			wstring resText = L"";
			for (list<TCitation*>::iterator it = allCitata.begin(); it != allCitata.end(); ++it)
			{
				ISentence* pSentBegin = (ISentence*)((*it)->pBeginCitata);
				ISentence* pSentEnd = (ISentence*)((*it)->pEndCitata);
				ISentence* pCurSent = pSentBegin;
				bool isFind = false;
				for (TSentList::iterator itSent = m_lSentences.begin(); itSent != m_lSentences.end(); ++itSent)
				{
					ISentence *pSent = (*itSent).pSentence;
					if (pSent == pSentBegin)
					{
						isFind = true;
					}
					if (isFind)
					{
						MarkTone(pSent, dictName);
						TSentList::iterator itSent1 = itSent;						
						resText = resText + GetXMLForSent(pSent, id, (*it)->nType, pSent == pSentEnd);
					}
					if (pSent == pSentEnd)
					{
						isFind = false;
					}
				}
				id++;
			}
			return (teg_beg + resText + teg_end);
		}

		wstring CBaseDigestAnalyzer::GetDigestXML()
		{
			wstring digesttext = L"<DIGEST>";
			wstring objteg_begin =  L"<OBJ>";
			wstring objteg_end =  L"</OBJ> ";
			wstring tone_teg_begin = L"<TONE";
			wstring tone_teg_end = L"</TONE> ";
            wstring cdata_begin = L"<![CDATA[";
			wstring cdata_end = L"]]>";

			set<IUnit*> citUnits;
			// группа map позволяте хранить инфу по тональности для каждоо дайджеста
			map<wstring, int> avgToneNeg; // привязывает имя субъекта к значению тональности средней
			map<wstring, int> avgTonePos;

			map<wstring, int> maxToneNeg;
			map<wstring, int> maxTonePos;

			map<wstring, int> numSent;	
			map<wstring, int> numSentNeg;	
			map<wstring, int> numSentPos;	

			SS::Core::Features::CMorphoFeature oMorfoPattern;
	
			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postPronoun; 
			oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			oMorfoPattern.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
			oMorfoPattern.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
			oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
			oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern.m_CommonType =  SS::Core::Features::Values::CommonType.Undefined;

			for(list<CDigestCitation*>::iterator it = allDigestCitata.begin();
                it != allDigestCitata.end();
                ++it)
			{
				CDigestCitation* dcit = *it;
				// тперь проверим, есть ли целевой объект внутри цитаты. Если есть то добавляем в дайджест тег
				// если нет то нет
				bool isTarget = false;

				wstring senttext = cdata_begin;

				TUnitList::iterator itUnit = dcit->citUnits.begin();
				citUnits.clear();
				for(; itUnit != dcit->citUnits.end(); ++itUnit)
				{				
					IUnit *unit1 = *itUnit;
					citUnits.insert(unit1);
					// senttext += L" ";
					IUnit *refunit = unit1->GetReferenceUnit();
					//	wstring str = unit1->GetWord();				
					if (((unit1->GetIsTargetObjectUnitFromParent()==1 && unit1->IsVisual()) || (unit1->GetIsTargetObjectUnit()==1) ||
						(refunit!=NULL && (refunit->GetIsTargetObjectUnitFromParent()==1 || refunit->GetIsTargetObjectUnit()==1)))||
						(((unit1->GetIsTargetObjectUnitFromChilds()==1 || unit1->GetIsTargetObjectUnit()==1) /*&& this->IsTopLevelUnit(pUnit)*/)||
						(refunit!=NULL && (refunit->GetIsTargetObjectUnitFromChilds()==1 || refunit->GetIsTargetObjectUnit()==1))))
					{
						// нашли значит теперь формируем XML-тег					
						isTarget = true;
					}							
				}
	
				if (isTarget)
				{
					RoleToneFind(dcit, citUnits, dcit->citUnits);
					itUnit = dcit->citUnits.begin();
					for(; itUnit != dcit->citUnits.end(); ++itUnit)
					{		
						IUnit *unit1 = *itUnit;
						senttext += L" ";
						IUnit *refunit = unit1->GetReferenceUnit();
						wstring str = unit1->GetWord();				
						if (((unit1->GetIsTargetObjectUnitFromParent()==1 && unit1->IsVisual()) || (unit1->GetIsTargetObjectUnit()==1) ||
							(refunit!=NULL && (refunit->GetIsTargetObjectUnitFromParent()==1 || refunit->GetIsTargetObjectUnit()==1)))||
							(((unit1->GetIsTargetObjectUnitFromChilds()==1 || unit1->GetIsTargetObjectUnit()==1) /*&& this->IsTopLevelUnit(pUnit)*/)||
							(refunit!=NULL && (refunit->GetIsTargetObjectUnitFromChilds()==1 || refunit->GetIsTargetObjectUnit()==1))))
						{
							senttext += objteg_begin + str + objteg_end;				
						}
						else
						{
							wchar_t sTone[10];				
							int tone = unit1->GetRealTone();
							bool bInv = unit1->GetRealInversion();
							wsprintfW(sTone, L"%d", tone);								
							wstring strTone = sTone;
							if (tone!=0 || bInv)
							{
								wchar_t sTone[10];									    
								wsprintfW(sTone, L"%d", tone);								
								wstring strTone = sTone;
								wstring toneDictName = unit1->GetUnitToneDict();
								if (bInv)
                                    strTone = L"0";
								senttext += tone_teg_begin  + L" WEIGHT=\"" + strTone + L"\" DICT=\"" + toneDictName +  L"\">" + unit1->GetWord() + tone_teg_end;
							}
                            else
							{
								senttext += unit1->GetWord();
							}
						}					
					}

					wstring author;
					wstring sentteg_begin = L"<SENT subject=\"";	
					if (dcit->pSubjectUnit==NULL)
						author = L"Автор";
					else
					{
						IUnit* refunit = dcit->pSubjectUnit->GetReferenceUnit();
						if (refunit && (IsPerson(refunit) || isOrganization(refunit)))
							author = GetAllTermsToNominative(refunit);
						else
						{
							//GetMainFormText(dcit->pSubjectUnit, SS::Core::Features::Values::GenderType.gtMasculine );
							if (unitOperator->IsMorpho(dcit->pSubjectUnit, oMorfoPattern))
							// неразрешили анафорию
							{
								author = L"Автор";
							}
                            else
								author = GetAllTermsToNominative(dcit->pSubjectUnit);
						}
						if(author != L"Автор")
						{
							wstring lower = unitOperator->GetLowerWord(author);
							if(lower == L"автор")
								author = L"Автор";
						}
					}
					sentteg_begin += author;

					--itUnit;
					IUnit* finunit = *itUnit;
					wstring finword = finunit->GetWord();
					if (finword!=L"." && finword!=L"!" && finword!=L"?" && finword!=L";" &&
                        !dcit->pSubjectUnit)
					{
						senttext += L"...";
					}
                    senttext += cdata_end;
                    
					wchar_t sNegTone[10];
					wchar_t sPosTone[10];
					wsprintfW(sNegTone, L"%d", dcit->toneNeg);
					wsprintfW(sPosTone, L"%d", dcit->tonePos);
					wstring strNegTone = sNegTone;
					wstring strPosTone = sPosTone;
					sentteg_begin += L"\" tone=\"" + strNegTone + L"," + strPosTone;
					sentteg_begin += L"\">";
					wstring sentteg_end = L"</SENT>";
					senttext = sentteg_begin + senttext + sentteg_end;
					digesttext += senttext;

					map<wstring, int>::iterator itTone = avgToneNeg.find(author);
					if (itTone == avgToneNeg.end())
						avgToneNeg.insert(map<wstring, int>::value_type(author, dcit->toneNeg));
                    else 
						(*itTone).second += dcit->toneNeg;

					itTone = avgTonePos.find(author);
					if (itTone == avgTonePos.end())
						avgTonePos.insert(map<wstring, int>::value_type(author, dcit->tonePos));
					else 
						(*itTone).second += dcit->tonePos;

					itTone = maxTonePos.find(author);
					if (itTone == maxTonePos.end())
						maxTonePos.insert(map<wstring, int>::value_type(author, dcit->tonePos));
					else  if ((*itTone).second < dcit->tonePos)
                        (*itTone).second = dcit->tonePos;

					itTone = maxToneNeg.find(author);
					if (itTone == maxToneNeg.end())
						maxToneNeg.insert(map<wstring, int>::value_type(author, dcit->toneNeg));
					else if ((*itTone).second>dcit->toneNeg)
                        (*itTone).second = dcit->toneNeg;

                    itTone = numSent.find(author);
					if (itTone == numSent.end())
						numSent.insert(map<wstring, int>::value_type(author,1));
					else 
						(*itTone).second++;					

					if (dcit->toneNeg !=0 )
                    {
						itTone = numSentNeg.find(author);
						if (itTone == numSentNeg.end())
							numSentNeg.insert(map<wstring, int>::value_type(author,1));
						else 
							(*itTone).second++;					
					}

					if (dcit->tonePos!=0)
                    {
						itTone = numSentPos.find(author);
						if (itTone==numSentPos.end())
							numSentPos.insert(map<wstring, int>::value_type(author,1));
						else 
							(*itTone).second++;					
					}
				}
			}

			// теперь для каждого субъекта берем тональность дайджеста
			for(map<wstring, int>::iterator itA = numSent.begin(); itA!=numSent.end(); ++itA)
			{
				wstring author = (*itA).first;
				map<wstring, int>::iterator itTone = avgToneNeg.find(author);			
				float markNegAvg = (float)(*itTone).second/(float)((*itA).second);
				itTone = avgTonePos.find(author);
				float markPosAvg = (float)(*itTone).second/(float)((*itA).second);
				itTone = maxToneNeg.find(author);
				float markNegMax = (float)(*itTone).second;
				itTone = maxTonePos.find(author);
				float markPosMax = (float)(*itTone).second;
				itTone = numSentNeg.find(author);
				float numNeg = 0.0;
				if (itTone != numSentNeg.end())
					numNeg = (float)(*itTone).second;

				itTone = numSentPos.find(author);
				float numPos = 0.0;
				if (itTone != numSentPos.end())
					numPos = (float)(*itTone).second;
				numNeg = sqrt((numNeg)/ (2*(numNeg+numPos)+1));
				numPos = sqrt((numPos)/ (2*(numNeg+numPos)+1));
				//float negDigMark = ((1-numNeg)*markNegAvg + (numNeg)*markNegMax);			
				//float posDigMark = ((1-numPos)*markPosAvg + (numPos)*markPosMax);
				float negDigMark = ((0.4)*markNegAvg + (0.6)*markNegMax);	
				float posDigMark = ((0.4)*markPosAvg + (0.6)*markPosMax);	

				if ((abs(negDigMark)-(float)((int)abs(negDigMark))>0.5))
                    negDigMark -= 1;
				if ((abs(posDigMark)-(float)((int)abs(posDigMark))>0.5))
                    posDigMark += 1;

				// эвристика - если одна тональность существенно превалирует над другой, то дополнительно увеличиваем
				// доминирующую и уменьшаем прогрывающую на 1
				/*if ((-negDigMark-posDigMark)>1){
				if (negDigMark!=-10)
				negDigMark+=-1;
				if (posDigMark!=0) posDigMark+=-1;
				}
				if ((posDigMark-(-negDigMark))>1)
				{
				if (posDigMark!=10)
				posDigMark+=1;
				if (negDigMark!=0) negDigMark+=1;
				}*/

				wchar_t sNegTone[10];
				wchar_t sPosTone[10];
				wsprintfW(sNegTone, L"%d", (int)negDigMark);
				wsprintfW(sPosTone, L"%d", (int)posDigMark);
				wstring strNegTone = sNegTone;
				wstring strPosTone = sPosTone;

				digesttext+=L"<AUTHOR name=\"" + author + L"\">" 
					+ L"<POS>" + strPosTone + L"</POS>" 
					+ L"<NEG>" + strNegTone + L"</NEG>" 
					+ L"</AUTHOR>";
			}

			digesttext += L"</DIGEST>";
			return digesttext.c_str();		
		}
		
		void CBaseDigestAnalyzer::RoleToneFind(CDigestCitation* dcit, set<IUnit*>& citataUnits, TUnitList& listCitataUnits)
		{
			ISentence* pSent = dcit->pSent;
			TUnitList findedUnits;
			CToneDictPool *tdp = this->m_ToneDictMap[mainObject->GetType()];
			tdp->adjectivesUnits->FindObjectInSentence(pSent, findedUnits, false, NULL);
			tdp->nounsUnits->FindObjectInSentence(pSent, findedUnits, false, NULL);
			tdp->nounsgenerativeUnits->FindObjectInSentence(pSent, findedUnits, false, NULL);
			tdp->objectnounsUnits->FindObjectInSentence(pSent, findedUnits, false, NULL);
			tdp->verbsUnits->FindObjectInSentence(pSent, findedUnits, false, NULL);
			findedUnits.clear();
			tdp->inversUnits->FindObjectInSentence(pSent, findedUnits, false, NULL);
			for(TUnitList::iterator it =findedUnits.begin();  it != findedUnits.end(); ++it)
			{
				IUnit *unit = *it;
				unit->SetInversion(-1);
			}

			findedUnits.clear();
			tdp->stopwordsUnits->FindObjectInSentence(pSent, findedUnits, false, NULL);
			for(TUnitList::iterator it =findedUnits.begin();  it != findedUnits.end(); ++it)
			{
				IUnit *unit = *it;
				unit->SetStopWordUnit(1);
			}	

			ReleaseSimpleInversions1(pSent);
			FindToneLexicForTargetObject(dcit, citataUnits,listCitataUnits);
		}
		
		// определяет тольнаонсть по эвристике Иванов - хороший человек
		void CBaseDigestAnalyzer::EvrisiticTire(TUnitList& sentList, IUnit* targetUnit, int& posTone, int& negTone)
		{
			bool isTarget = false;
			bool isTire = false;
			SS::Core::Features::CMorphoFeature oMorfoAnfr;	
			posTone = 0;
			negTone = 0;
			for(TUnitList::iterator it = sentList.begin(); it!=sentList.end(); ++it)
			{			
				IUnit *pUnit = *it;
				if (isTire)
				{
					unitOperator->GetUnitMorfoFeatures(pUnit,oMorfoAnfr);
					if (oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postNoun) ||
						oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postAdverb) ||
						oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postAdjective) ||
						wcscmp(pUnit->GetWord(), L"\"") == 0)
				 {
					 if (pUnit->GetTone()>0)
						 posTone += pUnit->GetTone();
					 else negTone += pUnit->GetTone();
					}else
				 {
					 break;
				 }
				}

				if (isTarget)
				{
					wstring str = pUnit->GetWord();
					wchar_t* str1 =  (wchar_t*)str.c_str();
					if (wcscmp(pUnit->GetWord(), L"-") == 0)
					{
						isTire = true;
					}
				}

				if (pUnit==targetUnit) isTarget = true;	
			}
		}

        IUnit* CBaseDigestAnalyzer::GetPhraseVerbParentUnit(IUnit* pUnit)
        {
            IUnit *pParent = 0;
            IUnit *pChild = pUnit->GetLeftChildUnit();
            while(pChild)
            {
                pParent = GetPhraseVerbParentUnit(pChild);
                if(!pParent)
                    pChild = pChild->GetRightUnit();
                else
                    pChild = 0;
            }
            if(!pParent && unitOperator->IsVerbUnit(pUnit))
                pParent = pUnit;
            return pParent;
        }

		IUnit* CBaseDigestAnalyzer::GetMainParentUnit(IUnit* pUnit)
		{
			IUnit *pUnit1 = pUnit;
			while(pUnit1->GetParentUnit())
                pUnit1 = pUnit->GetParentUnit();
			return pUnit1;
		}

		bool CBaseDigestAnalyzer::IsTopLevelUnit(IUnit* pUnit)
		{
			SS::Core::Features::CMorphoFeature oMorfoAnfr;		
			IUnit* pParent = pUnit->GetParentUnit();

			if (pParent==NULL)
			{
				return true;
			}
			else
			{
				unitOperator->GetUnitMorfoFeatures(pParent,oMorfoAnfr);
				if ((oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postVerb)&&
					(oMorfoAnfr.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfInfinitive) ||
					oMorfoAnfr.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfPersonal))))
				{
					return true;
				}
			}
			return false;
		}

		// определяет привязан ли терм к глаголу.
		bool CBaseDigestAnalyzer::isNonVerbParent(IUnit* pUnit)
		{
			SS::Core::Features::CMorphoFeature oMorfoAnfr;		
			IUnit* pParent = pUnit->GetParentUnit();

			if (pParent == NULL)
			{
				unitOperator->GetUnitMorfoFeatures(pUnit,oMorfoAnfr);
				// глаголы тут не учитываем
				if ((oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postVerb)&&
					(oMorfoAnfr.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfInfinitive) ||
					oMorfoAnfr.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfPersonal)))
				   )
				{
					return false;
				}				
			}
			else
			{
				return false;
			}
			return true;
		}

		bool CBaseDigestAnalyzer::IsPersonWithInfo(IUnit* pUnit)
		{
			SS::Core::Features::CMorphoFeature oMorfoAnfr;
			unitOperator->GetUnitMorfoFeatures(pUnit,oMorfoAnfr);

			if (oMorfoAnfr.m_CommonType.IsIntersectsWith(&SS::Core::Features::Values::CommonType.cmtProper) &&
				unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson))
			{
				return true;
			}

			IUnit* pChildUnit = pUnit->GetLeftChildUnit();
			while (pChildUnit)
			{
				//SS::Core::Features::CMorphoFeature oMorfoAnfr1;
				unitOperator->GetUnitMorfoFeatures(pChildUnit, oMorfoAnfr);

				if (oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postNoun) &&
					unitOperator->IsSyntax(pChildUnit, SS::Core::Features::Values::SyntaxCategories.scAttribute))
				{
					return true;
				}
				pChildUnit = pChildUnit->GetRightUnit();
			}

			return false;
		}

		void CBaseDigestAnalyzer::GetAllUnitsWithoutVerb(TUnitList& sentList, TUnitList& unitList, IUnit* targetUnit)
		{
			unitList.clear();
			bool isTarget = false;
			for (TUnitList::iterator it = sentList.begin(); it!=sentList.end(); ++it)
			{			
				IUnit *pUnit = *it;
				if (isTarget)
				{
					SS::Core::Features::CMorphoFeature oMorfoAnfr;

					unitOperator->GetUnitMorfoFeatures(pUnit,oMorfoAnfr);

					wstring str = pUnit->GetWord();			
					if (str == L"где" || str == L"когда" || str == L"что" ||
						(unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson) && IsPersonWithInfo(pUnit)))
					{
						break;
					}

					if (isNonVerbParent(pUnit) && !(pUnit->GetIsTargetObjectUnitFromChilds()==1 || pUnit->GetIsTargetObjectUnit()==1))
					{
						unitList.push_back(pUnit);
					}
				}
				if (pUnit==targetUnit) isTarget = true;					
			}
		}

		IUnit* CBaseDigestAnalyzer::GetNearestVerb(IUnit* pUnit, TUnitList& unitList )
		{
			bool isTargetFind = false;
			for(TUnitList::iterator it = unitList.begin(); it!=unitList.end(); ++it)
			{
				IUnit *pCurUnit = *it;
				if (isTargetFind && unitOperator->IsSyntax(pCurUnit, SS::Core::Features::Values::SyntaxCategories.scMain))
				{
					return pCurUnit;
				}

				if (pUnit == pCurUnit) // нашли целевой. ищем ближайший юнит с синтаксической ролью scMain
				{
					isTargetFind = true;
				}
			}
			return 0;
		}

		// теперь для каждого найденного целевого объекта в предложении надо найти всю 
		// тональную лексику, которая ему соотв. 
		// для этого надо сначала определить находитсмя ли наш объект в состоянии подлежащего
		// либо перед ни стоит специальная лексика "при", "из-за", "о", "у", "c".
		// если так, то ищем всю тональную лекиску входящшую в тоже дерево, что и целевой объект
		// при этом не учитываем тональную лекску у которйо в качетве дочернего атрибута есть персона или организац
		// либо которая сама явлвяется дочерним атрибутом для персоны или организации, котроая не есть целевой объект.
		// при этом лексика найденная однажды второй раз не учитывается
		// кроме того, вся лекиска лежащаа не далее чем на три терма от целевого объекта также считается относящейся к нему
		void CBaseDigestAnalyzer::FindToneLexicForTargetObject(CDigestCitation* dcit, set<IUnit*>& citataUnits, TUnitList& listCitataUnits)
		{
			ISentence* pSent = dcit->pSent;
			TUnitList sentUnits;
			TUnitList relevantList;
			TUnitList noneUnits;
			TUnitList noneVerbUnits;
			GetLinearUnitsFull(pSent, sentUnits);
			set<int> allTermNumbers;		
			int allSentTonePos = 0;
			int allSentToneNeg = 0;
			SS::Core::Features::CMorphoFeature oMorfoPattern;

			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
			oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			oMorfoPattern.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
			oMorfoPattern.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
			oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
			oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern.m_CommonType = SS::Core::Features::Values::CommonType.Undefined;

			FindUnits(pSent, oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.scMain,
				SS::Core::Features::Values::TSemanticType.Undefined, 2, 
				2, NULL, noneUnits,
				relevantList);

			set<IUnit*> verbList;
			for(TUnitList::iterator it = relevantList.begin(); it!=relevantList.end(); ++it)
			{
				verbList.insert(*it);
			}

			bool isPrevStopWord = false;
			for(TUnitList::iterator it = sentUnits.begin(); it!=sentUnits.end(); ++it)
			{
				IUnit *pUnit = *it;	
				IUnit *pParent = pUnit->GetParentUnit();
				IUnit *refunit = pUnit->GetReferenceUnit();
				if (!isPrevStopWord)					
					if (((pUnit->GetIsTargetObjectUnitFromChilds()==1 || pUnit->GetIsTargetObjectUnit()==1) /*&& this->IsTopLevelUnit(pUnit)*/)||
						(refunit!=NULL && (refunit->GetIsTargetObjectUnitFromChilds()==1 || refunit->GetIsTargetObjectUnit()==1)/* && this->IsTopLevelUnit(pUnit)*/))

					{
						// взяли целевой pUnit. Теперь ищем по дерву всю тональную лексику. кроме глагола. глагол
						// исследуем в конце
						//
						int nTonePos = 0;
						int nToneNeg = 0;
						int nTonePos1 = 0;
						int nToneNeg1 = 0;
						int nVerbTone = 0;
						int nVerbInvers = 1;
						IUnit *pVerb = NULL;

						/*	EvrisiticTire(sentUnits, pUnit, nTonePos1, nToneNeg1 );
						allSentTonePos += nTonePos1;
						allSentToneNeg += nToneNeg1;	*/
						if (this->isNonVerbParent(pUnit))
						{
							pVerb = this->GetNearestVerb(pUnit, sentUnits);
						}
						//pUnit->
						GetAllUnitsWithoutVerb(sentUnits, noneVerbUnits,pUnit);
						/*ToneUnitAnalyze(pUnit, allTermNumbers,nToneNeg, nTonePos, 
						nVerbTone, nVerbInvers,
						verbList, citataUnits,
						!IsSyntax(pUnit, SS::Core::Features::Values::SyntaxCategories.scObject),
						pVerb,noneVerbUnits);*/
						this->ToneUnitAnalyzeVersion2(pUnit, allTermNumbers,nToneNeg, nTonePos, listCitataUnits);
						allSentTonePos += nTonePos;
						allSentToneNeg += nToneNeg;				
						nToneNeg = 0;
						nTonePos = 0;
						//if (allSentToneNeg>=0){
						//	if (!IsSyntax(pUnit, SS::Core::Features::Values::SyntaxCategories.scObject))
						//	{
						//	//ToneUnitAnalyzeForNearUnits(pSent, pUnit, allTermNumbers,nToneNeg, nTonePos, citataUnits);
						//	}
						//}
						allSentTonePos += nTonePos;
						allSentToneNeg += nToneNeg;
					}

					/*if (pUnit->GetStopWordUnit()==1)
					{
					isPrevStopWord = true;
					}else isPrevStopWord = false;*/

			}	


			if ((float)-allSentToneNeg*2.0>10.0) allSentToneNeg = -10;
			else allSentToneNeg = allSentToneNeg*2.0;

			if ((float)allSentTonePos*2.0>10.0) allSentTonePos = 10;
			else allSentTonePos = allSentTonePos*2.0;
			
			dcit->toneNeg = allSentToneNeg;
			dcit->tonePos = allSentTonePos;
		}

		void CBaseDigestAnalyzer::ToneUnitAnalyzeForNearUnits(
			ISentence* pSent, IUnit* pUnit, set<int>& allTermNumbers, int& nToneNeg, int& nTonePos, set<IUnit*>& citataUnits)
		{
			// ищем ближайшие к данному термы (не далее трех). если етсь тонлаьные и они еще не учтены - учитываем
			TUnitList sentUnits;
			pSent->GetLinearUnits(sentUnits);

			SS::Core::Features::CMorphoFeature oMorfoAnfr;		

			for (TUnitList::iterator it = sentUnits.begin(); it!=sentUnits.end(); ++it)
			{

				IUnit* curUnit = *it;
				//oMorfoAnfr.Init();
				unitOperator->GetUnitMorfoFeatures(curUnit,oMorfoAnfr);
				// глаголы тут не учитываем
				if (!(oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postVerb)&&
					(oMorfoAnfr.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfInfinitive) ||
					oMorfoAnfr.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfPersonal))))
				{
					if (citataUnits.find(curUnit)!=citataUnits.end())
					{ 
						int n = curUnit->GetPosition()-pUnit->GetPosition();
						if (n >= -3 && n <= 3)
						{
							if (allTermNumbers.find(curUnit->GetTermNumber())==allTermNumbers.end()) // еще не задействоали
							{
								if (curUnit->GetTone()<0) 
								{
									nToneNeg+= curUnit->GetTone();
									curUnit->SetRealTone(-1);
									allTermNumbers.insert(curUnit->GetTermNumber());
								}
								else if (curUnit->GetTone()>0)
								{
									nTonePos += curUnit->GetTone();
									curUnit->SetRealTone(1);
									allTermNumbers.insert(curUnit->GetTermNumber());
								}
							}
						}
					}
				}
			}
		}

		// надо все дерево разбить на ветки - ветка подлежащего, ветка нашего объекта, остальнвые ветки
		// глагол может инвертировать только толньаонсть отсальных веток
		void CBaseDigestAnalyzer::GetAllBranches(IUnit* pUnit, TUnitList& targetUnits, TUnitList& subjUnits, TUnitList& otherUnits)
		{
			IUnit *pTreeUnit = pUnit->GetLeftChildUnit();
			while (pTreeUnit != NULL)
			{				
				if (IsTargetInBranch(pTreeUnit))
				{
					targetUnits.push_back(pTreeUnit);
				}
				else if (IsSubjectInBranch(pTreeUnit))
				{
					subjUnits.push_back(pTreeUnit);
				}
				else
				{
						otherUnits.push_back(pTreeUnit);
				}
				pTreeUnit = pTreeUnit->GetRightUnit();
			}
		}

		bool CBaseDigestAnalyzer::IsTargetInBranch(IUnit* pUnit)
		{
			if (pUnit->GetIsTargetObjectUnit()) return true;
			//if (IsSyntax(pUnit, SS::Core::Features::Values::SyntaxCategories.scSubjectFin)) return true;
			IUnit *pTreeUnit = pUnit->GetLeftChildUnit();
			while (pTreeUnit!=NULL)
			{		
				bool b = IsTargetInBranch(pTreeUnit);
				if (b) return true;
				pTreeUnit = pTreeUnit->GetRightUnit();
			}
			return false;
		}	

		bool CBaseDigestAnalyzer::IsSubjectInBranch(IUnit* pUnit)
		{
			//if (pUnit->GetIsTargetObjectUnit()) return true;
			if (unitOperator->IsSyntax(pUnit, SS::Core::Features::Values::SyntaxCategories.scSubjectFin))
			{
				return true;
			}
			IUnit *pTreeUnit = pUnit->GetLeftChildUnit();
			while (pTreeUnit)
			{		
				if (IsSubjectInBranch(pTreeUnit))
				{
					return true;
				}
				pTreeUnit = pTreeUnit->GetRightUnit();
			}
			return false;
		}	

		bool CBaseDigestAnalyzer::IsInversNegativeAllSent(IUnit* pUnit)
		{
			// идем от целевого объекта до родительствкого. Если встречаем инверсный предлог
			// то тонльаонсть негатива превращаем в позитив.
			IUnit* pParent = pUnit;
			while (pParent->GetParentUnit() != NULL)
			{
				pParent = pParent->GetParentUnit();
				SS::Core::Features::CMorphoFeature oMorfoPattern;
				
				oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial; 
				oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
				oMorfoPattern.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
				oMorfoPattern.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
				oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
				oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
				oMorfoPattern.m_CommonType =  SS::Core::Features::Values::CommonType.Undefined;

				if (unitOperator->IsMorpho(pParent, oMorfoPattern) && pParent->GetInversion()==-1)
				{
					return true;
				}			
			}
			return false;
		}

		void SetRealToToUnitList(TUnitList& unitList, int realTone)
		{					
			for(TUnitList::iterator it = unitList.begin(); it!=unitList.end(); ++it)
			{
				IUnit* pUnit = *it;
				pUnit->SetRealTone(realTone);
			}
		}

		// если лексика входит в какой либо семнитческий юнит типа персона, организация или регион или
		// наоборот включает в себя таковой юнит, то ее нельзя в качестве тольнаной брать
		void  CBaseDigestAnalyzer::SetUnitFreedomForTone(TUnitList& listCitataUnits)
		{
			// идем по всем юнитам. Если юнит имеет один из типов, то помечаем все юниты до корня как
			// несвободные и все дочерние тоже как не свободные
			for (TUnitList::iterator it = listCitataUnits.begin(); it != listCitataUnits.end(); ++it)
			{
				IUnit *pUnit = *it;
				if ((IsPerson(pUnit) || isOrganization(pUnit) || IsPlace(pUnit)))
				{
					// сначала идем вверх
					IUnit* pParent = pUnit->GetParentUnit();
					while (pParent != NULL)
					{
						pParent->SetFreeForTone(false);
						pParent = pParent->GetParentUnit();			
					}
					// теперь идем вниз
					SetUnFreeUnitRecursive(pUnit);
				}
			}
		}

		void  CBaseDigestAnalyzer::SetUnFreeUnitRecursive(IUnit* pUnit)
		{			
			IUnit* pChildUnit = pUnit->GetLeftChildUnit();
			while (pChildUnit)
			{
				if (!pChildUnit->GetInverted())
				{
					pChildUnit->SetFreeForTone(false);
				}
				SetUnFreeUnitRecursive(pChildUnit);
				pChildUnit = pChildUnit->GetRightUnit();
			}		
		}

		bool CBaseDigestAnalyzer::IsPreNoun(IUnit* pObjUnit, TUnitList& listCitataUnits)
		{
			TUnitList::iterator it = unitOperator->FindUnit(listCitataUnits,pObjUnit);
			SS::Core::Features::CMorphoFeature oMorfoAnfr;
			if (it != listCitataUnits.end() && it != listCitataUnits.begin())
			{			
				while (it != listCitataUnits.begin())
				{
					it--;
					IUnit* pUnit = *it;
					unitOperator->GetUnitMorfoFeatures(pUnit,oMorfoAnfr);
					if (!pUnit->IsRusInitial() && (oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postNoun) &&
						oMorfoAnfr.m_AnimInAnimType.IsIntersectsWith(&SS::Core::Features::Values::AnimInAnimType.aiaInanimation)))				
					{
						return true;
					}				
				}
			}

			return false;
		}

		void  CBaseDigestAnalyzer::ExtLexicGenetiveForUnit(IUnit* pObjUnit, TUnitList& listCitataUnits, TUnitList& genetiveToneList)
		{
			TUnitList::iterator it = unitOperator->FindUnit(listCitataUnits,pObjUnit);
			SS::Core::Features::CMorphoFeature oMorfoAnfr;
			if (it != listCitataUnits.end() && it != listCitataUnits.begin())
			{			
				while (it != listCitataUnits.begin())
				{
					it--;
					IUnit* pUnit = *it;
					unitOperator->GetUnitMorfoFeatures(pUnit,oMorfoAnfr);
					if ((oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postNoun) &&
						oMorfoAnfr.m_AnimInAnimType.IsIntersectsWith(&SS::Core::Features::Values::AnimInAnimType.aiaInanimation)) ||
						oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postAdjective))
					{
						genetiveToneList.push_back(pUnit);
					}
					else 
					{
						break;
					}

				}
			}
		}

		void  CBaseDigestAnalyzer::LexicGenetiveForUnit(IUnit* pObjUnit, TUnitList& genetiveToneList)
		{
			genetiveToneList.push_back(pObjUnit);
			IUnit* pParent = pObjUnit->GetParentUnit();
			SS::Core::Features::CMorphoFeature oMorfoAnfr;
			//  еще определения надо проверить, стоящие перед объектом 
			IUnit* childUnit = pObjUnit->GetLeftChildUnit();
			while (childUnit != NULL)
			{
				unitOperator->GetUnitMorfoFeatures(childUnit,oMorfoAnfr);
				if (oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postAdjective))
				{
					genetiveToneList.push_back(childUnit);
				}
				childUnit = childUnit->GetRightUnit();
			}
			unitOperator->GetUnitMorfoFeatures(pObjUnit,oMorfoAnfr);
			if (!oMorfoAnfr.m_CaseType.IsIntersectsWith(&SS::Core::Features::Values::CaseType.ctGenitive))
			{
				return;
			}
			while (pParent != NULL)
			{
				unitOperator->GetUnitMorfoFeatures(pParent,oMorfoAnfr);
				if (!oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postNoun) &&
					!oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postAdjective))
				{
					break;			      			 
				}
				genetiveToneList.push_back(pParent);	
				pParent = pParent->GetParentUnit();
			}
			return;
		}

		// если идет тональный юнит перед которым предлог и до этого уже встретился тональнй юнит
		// с предлогоом то если тональности различаются - обнуляем отнальность второго.
		void CBaseDigestAnalyzer::SetTwoPrepositionToneDelete(TUnitList& listCitataUnits)
		{
			SS::Core::Features::CMorphoFeature oMorfoAnfr;
			int numOfPretext = 0;
			int lasttone = 0;
			for (TUnitList::iterator it = listCitataUnits.begin(); it != listCitataUnits.end(); ++it)
			{
				IUnit *pUnit = *it;		
				unitOperator->GetUnitMorfoFeatures(pUnit,oMorfoAnfr);
				if (oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postOfficial) &&
					oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::OfficialType.otPretext)) 
				{
					numOfPretext++;
				}
				else if (!oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postNoun) &&
					     !oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postAdjective))
				{
					numOfPretext=0;
				}
				else
				{
					if (numOfPretext>1)
					{
						if (lasttone*pUnit->GetTone()<0)   { pUnit->SetToneStrong(0); pUnit->SetRealTone(0); }
					}
					else
					{
						lasttone = pUnit->GetTone();
					}
				}
			}
		}

		void CBaseDigestAnalyzer::ExtToneLexicProcess(
			TUnitList& listCitataUnits,
			TUnitList::iterator curIterator, int direction,
			set<int>& allTermNumbers, int& nToneNeg, int& nTonePos,
			int& nToneNegVerb, int& nTonePosVerb,
			bool isVerb, bool isExtLexic,
			TUnitList& findedUnits,
			bool& isVerbInversion,
			TUnitList& posUnits, TUnitList& negUnits, TUnitList& verbUnits,
			TUnitList& subjFins)
		{	
			isVerbInversion = false;
			while (curIterator != listCitataUnits.begin() && curIterator != listCitataUnits.end())
			{
				IUnit* nextUnit = *curIterator;										
				if 	(unitOperator->FindInUnits(subjFins, nextUnit) ||
					(nextUnit->GetStopWordUnit()==1 && direction==1)|| unitOperator->FindInUnits(findedUnits,nextUnit) || (wcscmp(nextUnit->GetWord(), L"\"") == 0 && IsTopLevelUnit(nextUnit))) 
				{
					break; // если нашли конец блока то выходим
				}
				int nTone = nextUnit->GetTone();
				if (isVerb && IsVerb(nextUnit))
				{
					if (nextUnit->GetInversion()==-1) isVerbInversion = true;
					verbUnits.push_back(nextUnit);
					if (nTone!=0)
					{
						if (allTermNumbers.find(nextUnit->GetTermNumber())==allTermNumbers.end())
						{
							if (nTone>0)  
							{ 
								nTonePosVerb += nTone;							
							}
							else 
							{
								nToneNegVerb += nTone;

							}
							allTermNumbers.insert(nextUnit->GetTermNumber());
							if (nTone > 0)  
							{ 				
								nextUnit->SetRealTone(1);
								posUnits.push_back(nextUnit);
							}
							else  
							{							
								nextUnit->SetRealTone(-1);
								negUnits.push_back(nextUnit);
							}								
						}					
					}
				}
				else
					if (isExtLexic && nTone!=0 && nextUnit->GetFreeForTone())								 
					{
						if (allTermNumbers.find(nextUnit->GetTermNumber())==allTermNumbers.end())
						{
							if (nTone > 0)  
							{ 
								nTonePos += nTone;							
							}
							else 
							{
								nToneNeg += nTone;
							}
							allTermNumbers.insert(nextUnit->GetTermNumber());
							if (nTone > 0) 
							{ 				
								nextUnit->SetRealTone(1);
								posUnits.push_back(nextUnit);
							}
							else 
							{							
								nextUnit->SetRealTone(-1);
								negUnits.push_back(nextUnit);
							}								
						}																									
					}
					if (direction==-1) --curIterator;
					else curIterator++;			
			}
		}

		bool CBaseDigestAnalyzer::IsVerb(IUnit* pUnit)
		{
			SS::Core::Features::CMorphoFeature oMorfoAnfr;
			unitOperator->GetUnitMorfoFeatures(pUnit,oMorfoAnfr);
			if ((oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postVerb)&&
				(oMorfoAnfr.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfInfinitive) ||
				oMorfoAnfr.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfPersonal)))
			   )
			{
				return true;
			}			
			return false;
		}


		void CBaseDigestAnalyzer::ToneUnitAnalyzeVersion2(IUnit* pObjUnit,
			set<int>& allTermNumbers, int& nToneNeg, int& nTonePos,TUnitList& listCitataUnits)
		{
			bool isVerb = false;
			bool isExtLexic = false;
			bool isSubjectFind = false;
			IUnit* verb = NULL;
			TUnitList findedUnits;
			TUnitList anaphoraList;
			TUnitList specVerbList;

			pronounAnfrThird->FindObjectInUnits(listCitataUnits, anaphoraList, true, NULL);

			toneBlockDelimiters->FindObjectInUnits(listCitataUnits, findedUnits, true, NULL);			
			citataVerbs->FindObjectInUnits(listCitataUnits, specVerbList, true, NULL);

			SetUnitFreedomForTone(listCitataUnits);
			SetTwoPrepositionToneDelete(listCitataUnits);
			TUnitList allSubjFins;

			for (TUnitList::iterator it = listCitataUnits.begin(); it!=listCitataUnits.end(); ++it)
			{
				IUnit *pUnit = *it;
				if (this->IsSubject(pUnit)) // нашли подлежащее
				{
					allSubjFins.push_back(pUnit);
					isSubjectFind = true;
					if (pUnit == pObjUnit) 
					{
						isVerb = (specVerbList.size() == 0);
						isExtLexic = true;
						verb = pUnit->GetParentUnit();
						//break;
					}
					else
					{
						isVerb = (specVerbList.size() != 0);
						// если подлежащее отлично от целевого объекта и является персоной, организацией или регионом, 
						// то не считаем тональную лексику, которая появляется во второстепенных членах предложения.
						if (IsPerson(pUnit) || isOrganization(pUnit) || IsPlace(pUnit)) 
							isExtLexic = false;
						else 
							isExtLexic = true;
					}
				}
			}

			if (!isSubjectFind) // все еще не нашли подлежащее
			{
				// ищем. Берем номинатив
				SS::Core::Features::CMorphoFeature oMorfoAnfr;
				SS::Core::Features::CMorphoFeature oMorfoVerb;
				SS::Core::Features::CMorphoFeature oMorfoObj;
				IUnit* customSubjFin  = NULL;
				TUnitList customSubjFinList;

				TUnitList preUnitList;
				bool isVerbFound = false;

				for (TUnitList::iterator it = listCitataUnits.begin(); it!=listCitataUnits.end(); ++it)
				{
					IUnit *pUnit = *it;			  					
					if ((IsNominative(pUnit) && IsTopLevelUnit(pUnit) && !IsPreNoun(pUnit, listCitataUnits))||
						IsPronounMain(pUnit)||(IsNominativeAll(pUnit) && unitOperator->FindInUnits(anaphoraList, pUnit))) // подчиненные не берем
					{			  
						if (isVerbFound) // глагол уже был и перед ним не было подлжащего
						{
							unitOperator->GetUnitMorfoFeatures(pUnit,oMorfoObj);
							if (oMorfoObj.m_GenderType.IsIntersectsWithout(&(oMorfoVerb.m_GenderType)) &&
								oMorfoObj.m_NumberType.IsIntersectsWithout(&(oMorfoVerb.m_NumberType)))
							{					
								customSubjFin = pUnit;
								allSubjFins.push_back(customSubjFin);
							}
						}
						else
						{
							customSubjFinList.push_back(pUnit);
						}
						continue;
					}

					if (IsVerb(pUnit))
					{
						unitOperator->GetUnitMorfoFeatures(pUnit,oMorfoVerb);
						if (customSubjFinList.size()!=0)
						{
							for (TUnitList::iterator it1 = customSubjFinList.begin(); it1!=customSubjFinList.end(); ++it1)
							{
								unitOperator->GetUnitMorfoFeatures(*it1,oMorfoObj);
								if (oMorfoObj.m_GenderType.IsIntersectsWithout(&(oMorfoVerb.m_GenderType)) &&
									oMorfoObj.m_NumberType.IsIntersectsWithout(&(oMorfoVerb.m_NumberType)))
								{	
									allSubjFins.push_back(*it1);
									break;
								}
							}
							customSubjFinList.clear();

						}
						else 
						{ 
							isVerbFound = true; 
						}
					}

					// проверяем один из разделителе ,a ,и ,но ,что ,где ,то ,когда
					// если наткнулись, то ищем заново подлежащие и сказуемые
					if (unitOperator->FindInUnits(findedUnits,pUnit) || (wcscmp(pUnit->GetWord(), L"\"") == 0 && IsTopLevelUnit(pUnit)))
					{
						customSubjFin = NULL;
						isVerbFound = false;
					}
				}

				if (unitOperator->FindInUnits(allSubjFins, pObjUnit))
				{
					isVerb = (specVerbList.size() == 0);
					isExtLexic = true;
				}
				else
				{
					isVerb = (specVerbList.size() != 0);
					isExtLexic = true;
					for(TUnitList::iterator it = allSubjFins.begin(); it!=allSubjFins.end(); ++it)
					{
						IUnit* pUnit = *it;
						if (IsPerson(pUnit) || isOrganization(pUnit) || this->IsPlace(pUnit)||IsPronounMain(pUnit)) 
						{
							isExtLexic = false;
						}
					}
				}
			} // if (!isSubjectFind)


			// теперь мы уже знаем надо ли учитывать глагол и дополнительную лексику
			// идем от целевого юнита влево до разделителя предложений, либо до местоимения он/она, либо до персоны в номинативе
			//
			TUnitList genetiveLexic;
			LexicGenetiveForUnit(pObjUnit, genetiveLexic);
			ExtLexicGenetiveForUnit(pObjUnit, listCitataUnits, genetiveLexic);

			int nToneNeg1 = 0, nToneNeg2 = 0, nToneNeg3 = 0,  nTonePos1= 0, nTonePos2 = 0, nTonePos3=0 ;
			int nToneNegVerb = 0, nTonePosVerb = 0;
			bool isVerbInversion1 = false;
			bool isVerbInversion2 = false;
			TUnitList posUnits, negUnits, verbUnits;
			for (TUnitList::iterator it = listCitataUnits.begin(); it!=listCitataUnits.end(); ++it)
			{
				if ((*it)==pObjUnit)
				{
					TUnitList::iterator itLeft = it;							
					TUnitList::iterator itRight = it;	
					int count = 0;

					if (it != listCitataUnits.begin())
					{
						--itLeft;
						ExtToneLexicProcess(listCitataUnits,itLeft, -1, allTermNumbers, nToneNeg1, nTonePos1, nToneNegVerb, nTonePosVerb, isVerb, isExtLexic, findedUnits, isVerbInversion1,posUnits, negUnits, verbUnits, allSubjFins);
					}

					if (it != listCitataUnits.end())
					{
						++itRight;
						ExtToneLexicProcess(listCitataUnits,itRight, 1, allTermNumbers, nToneNeg2, nTonePos2, nToneNegVerb, nTonePosVerb, isVerb, isExtLexic, findedUnits, isVerbInversion2,posUnits, negUnits, verbUnits, allSubjFins);
					}
				} // if ((*it)==pObjUnit)
			} //for(TUnitList::iterator it = listCitataUnits.begin(); it!=listCitataUnits.end(); ++it)	

			for (TUnitList::iterator it = genetiveLexic.begin(); it!=genetiveLexic.end(); ++it)
			{
				IUnit* pUnit = *it;
				int nTone = pUnit->GetTone();
				if (nTone!=0)
				{
					if (allTermNumbers.find(pUnit->GetTermNumber())==allTermNumbers.end())
					{
						if (nTone > 0)  
						{ 
							nTonePos3 += nTone;
						}
						else 
						{
							nToneNeg3 += nTone;
						}
						allTermNumbers.insert(pUnit->GetTermNumber());
						if (nTone > 0) 
						{ 				
							pUnit->SetRealTone(1);														
						}
						else  
						{							
							pUnit->SetRealTone(-1);							
						}								
					}					
				}
			}

			if ((isVerbInversion1 || isVerbInversion2) && ((nTonePos1 + nTonePos2)!=0))
			{
				if (nTonePosVerb == 0)
				{
					SetRealToToUnitList(verbUnits, 0);
				}
				nToneNegVerb = 0;
			}			
			if (nToneNegVerb!=0 && !(isVerbInversion1 || isVerbInversion2))  
			{
				SetRealToToUnitList(posUnits, 0);
				nTonePos1 = 0; nTonePos2 = 0; nTonePosVerb =0;
			}

			if (nToneNeg1 + nToneNeg2 + nToneNeg3!=0)  
			{  
				if (nToneNegVerb==0) SetRealToToUnitList(verbUnits, 0);
				nTonePosVerb =0;
			}		
			nToneNeg = nToneNeg1 + nToneNeg2 + nToneNeg3 + nToneNegVerb;
			nTonePos = nTonePos1 + nTonePos2 + nTonePos3 + nTonePosVerb;
		}

		bool CBaseDigestAnalyzer::IsNominative(IUnit* pUnit)
		{
			SS::Core::Features::CMorphoFeature oMorfoPattern;
			
			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun; 
			oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			oMorfoPattern.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
			oMorfoPattern.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
			oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.ctNominative;
			oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern.m_CommonType =  SS::Core::Features::Values::CommonType.Undefined;

			return unitOperator->IsMorpho(pUnit, oMorfoPattern);
		}

		bool CBaseDigestAnalyzer::IsNominativeAll(IUnit* pUnit)
		{
			SS::Core::Features::CMorphoFeature oMorfoPattern;			
			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.Undefined; 
			oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			oMorfoPattern.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
			oMorfoPattern.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
			oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.ctNominative;
			oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern.m_CommonType =  SS::Core::Features::Values::CommonType.Undefined;

			return unitOperator->IsMorpho(pUnit, oMorfoPattern);
		}

		void CBaseDigestAnalyzer::ToneUnitAnalyze(IUnit* pUnit,
			set<int>& allTermNumbers, int& nToneNeg, int& nTonePos,
			int& nVerbTone, int& nVerbInvers,
			set<IUnit*>& verbList,
			set<IUnit*>& citataUnits,
			bool isObjectTree,
			IUnit* pVerb,
			TUnitList& nonVerbList
			)
		{
			IUnit* pParent = pUnit;
			if (pVerb != NULL)
			{
				pParent = pVerb;
			}
			else
			{
				while (pParent->GetParentUnit() != NULL)
				{
					pParent = pParent->GetParentUnit();
				}
			}

			TUnitList list1, list2, list3;
			GetAllBranches(pParent, list1, list2, list3);

			if (!isObjectTree)
			{
				// сначала ищем корень, а потом рекурсивно ищем тонлаьность. При этотм формируем массим номеров тональной лекски
				// потому что тональная лекиска мож6ет  быть представлена словосочетанием, но учтена один раз.			
				list2.clear(); list3.clear();
			}

			for(TUnitList::iterator it = nonVerbList.begin(); it!=nonVerbList.end(); ++it)
			{
				IUnit* pUnit1 = *it;
				list3.push_back(pUnit1);
			}

			// pParent - самый родительский в дереве, в которое 				

			int nTone = pParent->GetTone();

			SS::Core::Features::CMorphoFeature oMorfoAnfr;		
			unitOperator->GetUnitMorfoFeatures(pParent,oMorfoAnfr);

			if (citataUnits.find(pParent)!=citataUnits.end())
			{
				if (!(oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postVerb)&&
					(oMorfoAnfr.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfInfinitive) ||
					oMorfoAnfr.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfPersonal))))
				{
					if (nTone!=0 && allTermNumbers.find(pParent->GetTermNumber())==allTermNumbers.end()) // еще не находили такой терм
					{
						if (nTone>0) {
							nTonePos += nTone;
							pParent->SetRealTone(1);
						}
						else
						{
							nToneNeg += nTone;
							pParent->SetRealTone(-1);
						}
						allTermNumbers.insert(pParent->GetTermNumber());
					}
				}
				else
				{
					if (verbList.find(pParent) != verbList.end())
					{						
						nVerbTone = pParent->GetTone();
						nVerbInvers = pParent->GetInversion();
					}
				}
			}

			int n = 0;
			int nNeg = 0; 
			int nPos = 0;
			TUnitList negUnits;
			TUnitList posUnits;
			for(TUnitList::iterator it = list3.begin(); it!=list3.end(); ++it)
			{
				IUnit* pRoot = *it;
				n=0;
				RecursiveToneUnitAnalyze(pRoot, allTermNumbers, nNeg, nPos,
					nVerbTone, nVerbInvers, verbList, citataUnits, n, negUnits, posUnits);
				// если где-то в ветке нашелся негаитв то весь позитивы по этой ветке отменяется
				if (nNeg!=0) 
				{
					nPos=0;
					SetRealToToUnitList(posUnits, 0);
				}
			}		

			if (pParent != NULL && pParent->GetInverted())
			{
				// глагол был инвертирован
				if (nNeg!=0)
				{
					nPos=-nNeg;
					nNeg = 0;
					SetRealToToUnitList(negUnits, 1);
					SetRealToToUnitList(posUnits, 0);					
				}else
					if (nPos!=0)
					{
						nNeg=-nPos;
						nPos = 0;
						SetRealToToUnitList(negUnits, 0);
						SetRealToToUnitList(posUnits, -1);						
					}
			}
			
			if (unitOperator->IsSyntax(pUnit,  SS::Core::Features::Values::SyntaxCategories.scSubjectFin) ||
				unitOperator->IsSyntax(pUnit,  SS::Core::Features::Values::SyntaxCategories.scObjectOfPreposition))
			{						

				if (!pParent->GetRealInversion()) // если глагол никого не инвертировал
				{
					if (nVerbTone<0)
					{
						if (allTermNumbers.find(pParent->GetTermNumber())==allTermNumbers.end())
						{
							nNeg += nVerbTone;				 
						}
						SetRealToToUnitList(posUnits, 0);
						pParent->SetRealTone(-1);
						nPos = 0;
					}else
					{
						if (nNeg==0){
							nPos += nVerbTone;
							if (nVerbTone>0) pParent->SetRealTone(1);
						}
					}	

					if (nVerbTone!=0)
					{
						allTermNumbers.insert(pParent->GetTermNumber());
					}
				}
			}

			int nNeg1 = 0;
			int nPos1 = 0;
			n=0;
			TUnitList negUnits1;
			TUnitList posUnits1;
			for(TUnitList::iterator it = list1.begin(); it!=list1.end(); ++it)
			{
				IUnit* pRoot = *it;
				n=0;
				RecursiveToneUnitAnalyze(pRoot, allTermNumbers, nNeg1, nPos1,
					nVerbTone, nVerbInvers, verbList, citataUnits, n,negUnits, posUnits);
				if (nNeg1!=0) 
				{
					nPos1=0;
					SetRealToToUnitList(posUnits1, 0);
				}
			}		

			nTonePos += nPos + nPos1;
			nToneNeg += nNeg + nNeg1;

			nNeg1 = 0;
			nPos1 = 0;
			n=0;
			TUnitList negUnits2;
			TUnitList posUnits2;
			for(TUnitList::iterator it = list2.begin(); it!=list2.end(); ++it)
			{
				IUnit* pRoot = *it;
				RecursiveToneUnitAnalyze(pRoot, allTermNumbers, nNeg1, nPos1,
					nVerbTone, nVerbInvers, verbList, citataUnits, n,negUnits2, posUnits2);
				if (nNeg1!=0) 
				{
					nPos1=0;
					nTonePos = 0;
					SetRealToToUnitList(posUnits2, 0);
					SetRealToToUnitList(posUnits1, 0);
				}
			}		

			nTonePos += nPos1;
			nToneNeg +=  nNeg1;

			// если глагол был инвертирован с позитивного на негативный, то весь негатив надо 
			if ( IsInversNegativeAllSent(pUnit))
			{
				// все которые были RealTone негативные = 1 (стали позитивными). Все которые были позитивные стали = 0
				if (pParent->GetRealTone()==-1) pParent->SetRealTone(1);
				for(TUnitList::iterator it = negUnits.begin(); it!=negUnits.end(); ++it)
				{
					IUnit* pUnit = *it;
					if (pUnit->GetRealTone()==-1) pUnit->SetRealTone(1);
					if (pUnit->GetRealTone()==1) pUnit->SetRealTone(0);
				}
				for(TUnitList::iterator it = posUnits.begin(); it!=posUnits.end(); ++it)
				{
					IUnit* pUnit = *it;
					if (pUnit->GetRealTone()==-1) pUnit->SetRealTone(1);
					if (pUnit->GetRealTone()==1) pUnit->SetRealTone(0);
				}

				for(TUnitList::iterator it = negUnits1.begin(); it!=negUnits1.end(); ++it)
				{
					IUnit* pUnit = *it;
					if (pUnit->GetRealTone()==-1) pUnit->SetRealTone(1);
					if (pUnit->GetRealTone()==1) pUnit->SetRealTone(0);
				}
				for(TUnitList::iterator it = posUnits1.begin(); it!=posUnits1.end(); ++it)
				{
					IUnit* pUnit = *it;
					if (pUnit->GetRealTone()==-1) pUnit->SetRealTone(1);
					if (pUnit->GetRealTone()==1) pUnit->SetRealTone(0);
				}

				for(TUnitList::iterator it = negUnits2.begin(); it!=negUnits2.end(); ++it)
				{
					IUnit* pUnit = *it;
					if (pUnit->GetRealTone()==-1) pUnit->SetRealTone(1);
					if (pUnit->GetRealTone()==1) pUnit->SetRealTone(0);
				}
				for(TUnitList::iterator it = posUnits2.begin(); it!=posUnits2.end(); ++it)
				{
					IUnit* pUnit = *it;
					if (pUnit->GetRealTone()==-1) pUnit->SetRealTone(1);
					if (pUnit->GetRealTone()==1) pUnit->SetRealTone(0);
				}
				nTonePos += -nToneNeg;
				nToneNeg = 0;
			}
		}


		void CBaseDigestAnalyzer::RecursiveToneUnitAnalyze(IUnit* pUnit,
			set<int>& allTermNumbers, int& nToneNeg, int& nTonePos,
			int& nVerbTone, int& nVerbInvers,
			set<IUnit*>& verbList,set<IUnit*>& citataUnits,
			int& numOfPretext, TUnitList& negUnits,TUnitList& posUnits)
		{	
			IUnit *pTreeUnit = pUnit;
			int nTone = pTreeUnit->GetTone();
			SS::Core::Features::CMorphoFeature oMorfoAnfr;		
			unitOperator->GetUnitMorfoFeatures(pTreeUnit,oMorfoAnfr);
			if (citataUnits.find(pTreeUnit)!=citataUnits.end())
			{				
				if (!(oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postVerb)&&
					(oMorfoAnfr.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfInfinitive) ||
					oMorfoAnfr.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfPersonal))))
				{
					if (nTone!=0)
					{
						if (allTermNumbers.find(pTreeUnit->GetTermNumber())==allTermNumbers.end()) // еще не находили такой терм
						{
							if (nTone>0)  { 
								nTonePos += nTone;							
							}
							else  {
								nToneNeg += nTone;

							}

							allTermNumbers.insert(pTreeUnit->GetTermNumber());
						}
						if (nTone>0)  { 				
							pTreeUnit->SetRealTone(1);
							posUnits.push_back(pTreeUnit);
						}
						else  {							
							pTreeUnit->SetRealTone(-1);
							negUnits.push_back(pTreeUnit);
						}
					}
				}else
				{
					// глагол. ндао проверить чтобы он был в роли scMain. Тогда учитываем его
					if (nVerbTone==0 && nVerbInvers==1) // еще не находили
					{
						if (verbList.find(pTreeUnit)!=verbList.end())
						{
							// глагол сказуемое.
							nVerbTone = pTreeUnit->GetTone();
							nVerbInvers = pTreeUnit->GetInversion();
							if (allTermNumbers.find( pTreeUnit->GetTermNumber())==allTermNumbers.end())
							{
								allTermNumbers.insert(pTreeUnit->GetTermNumber());
							}
						}
					}
				}
			}
			if (oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postOfficial) &&
				oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::OfficialType.otPretext)) numOfPretext++;
			if (numOfPretext<2)			
			{
				IUnit *pTreeUnit = pUnit->GetLeftChildUnit();
				while(pTreeUnit!=NULL)
				{						
					RecursiveToneUnitAnalyze(pTreeUnit,allTermNumbers, 
						nToneNeg, nTonePos, nVerbTone, nVerbInvers, verbList, citataUnits, numOfPretext, negUnits, posUnits);	
					pTreeUnit = pTreeUnit->GetRightUnit();
				}
			}
		}

		void CBaseDigestAnalyzer::ReleaseSimpleInversions1(ISentence* pSent)
		{
			TUnitList findedUnits;
			TUnitList toneUnits;
			pSent->GetLinearUnits(findedUnits);
			TUnitList::iterator it1;
			int curTermNumber = 0;
			TUnitList findedInversUnits;
			TUnitList internalUnits;
			set<IUnit*> allInvers;
			SS::Core::Features::CMorphoFeature oMorfoPattern;
			int curNum = -1;
			for (TUnitList::iterator it = findedUnits.begin(); it!=findedUnits.end(); ++it)
			{
				if ((*it)->GetTone()!=0 && (*it)->GetTermNumber()!=curNum) 
				{
					toneUnits.push_back(*it);
					curNum = (*it)->GetTermNumber();
				}
			}

			// теперь для каждого тонального терма пытаемся инвертировать тональность
			// для этого двигаемся назад от терма пропуская неинверсные термы (глаголы и наречия).
			toneUnits.reverse();
			for(TUnitList::iterator it = toneUnits.begin(); it!=toneUnits.end(); ++it)
			{
				findedInversUnits.clear();
				internalUnits.clear();
				IUnit* toneUnit = *it;
				it1 = unitOperator->FindUnit(findedUnits, toneUnit);		
				if (it1!=findedUnits.end() && it1!=findedUnits.begin())
				{
					int numOfInvers = 0;
					int lastNum = -1;
					while(it1!=findedUnits.begin())
					{
						it1--;
						IUnit *pUnit = *it1;
						if (allInvers.find(pUnit)!=allInvers.end()) continue;
						if (pUnit->GetInversion()==-1)
						{						
							findedInversUnits.push_back(pUnit);						
							if (pUnit->GetTermNumber()!=lastNum)
							{
								numOfInvers++;	
							}
							lastNum = pUnit->GetTermNumber();
						}else
							if (numOfInvers==0)
							{
								SS::Core::Features::CMorphoFeature oMorfoUnit;		
								unitOperator->GetUnitMorfoFeatures(pUnit,oMorfoUnit);
								bool b =
									(oMorfoUnit.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postVerb) &&
									oMorfoUnit.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfPersonal)) ||
									((oMorfoUnit.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postAdverb)||
									oMorfoUnit.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postPronoun))
									&&
									pUnit->GetTone()==0);
								if (b)  { internalUnits.push_back(pUnit); continue;}
								else break;
							}
							else break;
					}
					if (numOfInvers%2==1)
					{
						int nTone = toneUnit->GetTone();
						for(TUnitList::iterator it2= findedUnits.begin(); it2!=findedUnits.end(); ++it2)
						{						
							if ((*it2)->GetTermNumber()== toneUnit->GetTermNumber())
							{
								if (!(*it2)->GetInverted())
								{
									(*it2)->SetTone(-nTone);
									(*it2)->SetInverted(true);
								}
							}
						}

						for(TUnitList::iterator it2 = findedInversUnits.begin();  it2 !=  findedInversUnits.end(); ++it2)
						{
							IUnit *invUnit = *it2;	
							invUnit->SetRealInversion(true); // реально сработала
							if (allInvers.find(invUnit)==allInvers.end()) 
							{
								allInvers.insert(invUnit);
							}
						}

						for(TUnitList::iterator it2 = internalUnits.begin();  it2 !=  internalUnits.end(); ++it2)
						{
							(*it2)->SetToneStrong(0);
						}
					}
				}
			}
		}

		void CBaseDigestAnalyzer::ReleaseSimpleInversions(ISentence* pSent)
		{
			// идем назад по предложению от текущего юнита.
			TUnitList findedUnits;
			pSent->GetLinearUnits(findedUnits);
			TUnitList::iterator it;
			int curTermNumber = 0;
			TUnitList findedInversUnits;
			SS::Core::Features::CMorphoFeature oMorfoPattern;
			
			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdverb; // наречие
			oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			oMorfoPattern.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
			oMorfoPattern.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
			oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
			oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern.m_CommonType =  SS::Core::Features::Values::CommonType.Undefined;

			for(TUnitList::iterator it = findedUnits.begin(); it!=findedUnits.end(); ++it)
			{
				IUnit *pUnit = *it;				
				if (pUnit->GetInversion()==-1)
				{
					findedInversUnits.push_back(pUnit);
				}else
				{
					SS::Core::Features::CMorphoFeature oMorfoVerb;		
					unitOperator->GetUnitMorfoFeatures(pUnit,oMorfoVerb);

					bool b = unitOperator->IsSyntax(pUnit, SS::Core::Features::Values::SyntaxCategories.scChain)  ||
						(unitOperator->IsMorpho(pUnit, oMorfoPattern) && (pUnit->GetTone()==0))||
						(oMorfoVerb.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postVerb) &&
						oMorfoVerb.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfPersonal) && pUnit->GetTone()==0);				
					if (b) continue;
					if (findedInversUnits.size()==0) continue;					
					int numOfInvers = 0;
					bool isVerb = true;				
					for(TUnitList::iterator it1 = findedInversUnits.begin();  it1 !=  findedInversUnits.end(); ++it1)
					{
						IUnit *unit = *it1;		
						// сначла проверим не глагол ли
						SS::Core::Features::CMorphoFeature oMorfoAnfr;		
						unitOperator->GetUnitMorfoFeatures(unit,oMorfoAnfr);											
						if(unit->GetTermNumber()!=curTermNumber)
						{
							isVerb = false;
							numOfInvers++;
							curTermNumber = unit->GetTermNumber();
						}						
						curTermNumber = unit->GetTermNumber();						
					}

					if (pUnit->GetTone()!=0 && numOfInvers>0)						
					{
						for(TUnitList::iterator it1 = findedInversUnits.begin();  it1 !=  findedInversUnits.end(); ++it1)
						{
							IUnit *invUnit = *it1;	
							invUnit->SetRealInversion(true); // реально сработала
						}
					}
					if (numOfInvers%2==1) 
					{
						if (pUnit->GetTone()!=0)						
						{													
							// у всех юнитов, с тем же GetTermNumber надо тоже поменть тональность
							for(TUnitList::iterator it1 = findedUnits.begin(); it1!=findedUnits.end(); ++it1)
							{
								if ((*it1)->GetTermNumber() == pUnit->GetTermNumber()) 
								{
									(*it1)->SetTone(-pUnit->GetTone());
									(*it1)->SetInverted(true);
								}
							}
						}						
					}
					findedInversUnits.clear();
				}
			}	
		}

		void CBaseDigestAnalyzer::ProcessCitationSubject()
		{
			TCitation* pPrevCit = NULL;
			for (TSentList::iterator it = m_lSentences.begin();  it != m_lSentences.end(); ++it)
			{
				ISentence *sent = (*it).pSentence;
				if (!sent->IsPreDigest())
				{
					continue;
				}
				pPrevCit = ProcessCitationSubjectForSent(sent, pPrevCit);
			}
		}

		IUnit* CBaseDigestAnalyzer::GetMaxParentBySemType(IUnit* unit)
		{
			IUnit* cur = unit;
			IUnit* res = unit;
			while (cur != NULL && unitOperator->IsSemantic(cur, SS::Core::Features::Values::TSemanticType.smtPerson))
			{
				res = cur;
				cur = cur->GetParentUnit();
			}
			return res;
		}

		void CBaseDigestAnalyzer::ProcessCitataSubjectUnitVerbExt(TCitation* pCitata, TUnitList& sentUnits, ISentence* pSent, IUnit* unit)
		{
			SS::Core::Features::CMorphoFeature oMorfoPattern1;		
			oMorfoPattern1.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postPronoun; 
			oMorfoPattern1.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			oMorfoPattern1.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
			oMorfoPattern1.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
			oMorfoPattern1.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
			oMorfoPattern1.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern1.m_CommonType =  SS::Core::Features::Values::CommonType.Undefined;

			SS::Core::Features::CMorphoFeature oMorfoPattern2;		
			oMorfoPattern1.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun; 
			oMorfoPattern1.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			oMorfoPattern1.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
			oMorfoPattern1.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
			oMorfoPattern1.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
			oMorfoPattern1.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern1.m_CommonType =  SS::Core::Features::Values::CommonType.Undefined;

			if (unit->GetCitation()!=NULL) return;
			
			for (TUnitList::iterator it1 =  sentUnits.begin(); it1!=sentUnits.end(); ++it1)
			{
				IUnit* sentUnit = *it1;
				if (sentUnit==unit)
				{
					TUnitList::iterator itLeft = it1;							
					int count = 0;

					if (it1 != sentUnits.begin())
						while (itLeft!=sentUnits.begin())
						{
							IUnit* nextUnit = *itLeft;
							if (nextUnit->GetCitation()!=NULL) break;
							if (this->IsPerson(nextUnit) || this->isOrganization(nextUnit)||this->IsPlace(nextUnit))

							{
								pCitata->pSubjectUnit = GetMaxParentBySemType(nextUnit);
								return;
							}
							--itLeft;
							count++;
							if (count>8) break;
						}
						count = 0;
						TUnitList::iterator itRight = it1;	
						if (it1!=sentUnits.end())
							while (itRight != sentUnits.end())
							{
								IUnit* nextUnit = *itRight;
								if (nextUnit->GetCitation()!=NULL) break;
								if (this->IsPerson(nextUnit) || this->isOrganization(nextUnit)||this->IsPlace(nextUnit))

								{
									pCitata->pSubjectUnit = GetMaxParentBySemType(nextUnit);
									return;
								}
								++itRight;
								count++;
								if (count > 8) break;
							}			
				}
			}
		}

		void CBaseDigestAnalyzer::ProcessCitataSubjectUnitVerb(TCitation* pCitata, TUnitList& sentUnits, ISentence* pSent)
		{
			TUnitList findedUnits;
			SS::Core::Features::CMorphoFeature oMorfoPattern1;		
			oMorfoPattern1.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postPronoun; 
			oMorfoPattern1.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			oMorfoPattern1.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
			oMorfoPattern1.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
			oMorfoPattern1.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
			oMorfoPattern1.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern1.m_CommonType =  SS::Core::Features::Values::CommonType.Undefined;

			SS::Core::Features::CMorphoFeature oMorfoPattern2;		
			oMorfoPattern1.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun; 
			oMorfoPattern1.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			oMorfoPattern1.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
			oMorfoPattern1.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
			oMorfoPattern1.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
			oMorfoPattern1.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern1.m_CommonType =  SS::Core::Features::Values::CommonType.Undefined;

			SS::Core::Features::CMorphoFeature oMorfoAnfr;
			oMorfoAnfr.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postPronoun;

			citataVerbs->FindObjectInSentence(pSent, findedUnits, true, NULL);
			for (TUnitList::iterator it = findedUnits.begin(); it != findedUnits.end(); ++it)
			{
				IUnit* unit = *it;
				IUnit* pChildUnit =  unit->GetLeftChildUnit();
				if (unit->GetCitation()!=NULL) continue;
				while (pChildUnit)
				{			
					if (unit->GetCitation()!=NULL || pChildUnit->GetCitation()!=NULL)
					{
						pChildUnit = pChildUnit->GetRightUnit();
						continue;
					}
					if ((this->IsPerson(pChildUnit) || this->isOrganization(pChildUnit)||this->IsPlace(pChildUnit)) &&
					    this->IsSubject(pChildUnit))
					{
						pCitata->pSubjectUnit = pChildUnit;
						return;
					}
					pChildUnit = pChildUnit->GetRightUnit();
				}			
				// не нашли, скорее всего недоработка синтанализа. берем термы сразу за глаголом и если он местоимение или
				// персона - это то что нужно (но не более трех термов)
				ProcessCitataSubjectUnitVerbExt(pCitata, sentUnits, pSent,unit);
			}
		}

		// находит субъект цитирования
		void CBaseDigestAnalyzer::ProcessCitataSubjectUnit(TCitation* pCitata)
		{
			TUnitList findedUnits;
			TUnitList sentUnits;
			ISentence* pSent = NULL;	

			if (pCitata->pSubjectUnit!=NULL) return; // уже найден
			// сначала пытаемся найти в начадлном предложении
			if (pCitata->pBeginCitata!=NULL)
			{
				pSent = (ISentence*)pCitata->pBeginCitata;
				// ищем в начальном предложении циататы
				citataFirstTermFour1->FindObjectInSentence(pSent, findedUnits, true, NULL);
				if (findedUnits.size()==3)
				{
					// нашли эвристику "по его словам/по его утверждению"
					// надо взять средний из найденный юнитов - местоимение - он и будет субъектом цитирования
					pCitata->pSubjectUnit = *(++findedUnits.begin());
					return;
				}

				findedUnits.clear();
				citataFirstTermFour2->FindObjectInSentence(pSent, findedUnits, true, NULL);
				GetLinearUnitsFull(pSent,sentUnits);						
				if (findedUnits.size()==2)
				{
					IUnit *lastUnit = *(++findedUnits.begin());
					// нашли эвристику "по словам/по утверждению". Надо посмореть юнит сразху за ней и если это
					// персона то это и будет субъект цитирования
					for(TUnitList::iterator it = sentUnits.begin(); it!=sentUnits.end(); ++it)
					{
						if (*it==lastUnit)
						{
							++it;
							if (it!=sentUnits.end())
							{
								if ((this->IsPerson(*it) || this->isOrganization(*it)||this->IsPlace(*it)) &&
									this->IsSubject(*it))
								{
									// нашли 
									pCitata->pSubjectUnit = *it;
									return;
								}
							}
						} //if (*it==lastUnit)
					} //for(TUnitList::iterator it...
				} // if (findedUnits.size()==2)

				// до сих пор не нашли.
				// тогда надо проверить на сказуемые в предложении. И если сказуемое совпадает с одним из сказуемых цитировани
				// то надо найти подлежащее и проверить его на тип персона. если нашил то эта персона и есть субъект
				// цитирования
				ProcessCitataSubjectUnitVerb(pCitata, sentUnits, pSent);


			} // if (pCitata->pBeginCitata!=NULL)

			if (pCitata->pEndCitata!=NULL)
			{
				pSent = (ISentence*)pCitata->pEndCitata;
				sentUnits.clear();
				GetLinearUnitsFull(pSent,sentUnits);
				ProcessCitataSubjectUnitVerb(pCitata, sentUnits, pSent);
			}
		}

		TCitation* CBaseDigestAnalyzer::ProcessCitationSubjectForSent(ISentence* pSent, TCitation* pPrevCit1)
		{
			// берем последовательно все юниты предложения
			// если юнит имеет режим цитирование то ищем его субъект цитирования.
			// если не имеет то субъект - Автор.
			// если изменяется режим цитирования на каком то юните - имеем новое цитирование
			TUnitList sentUnits;

			pSent->GetLinearUnitsFull(sentUnits);
			bool isChangeCit = true;
			TCitation* pPrevCit = pPrevCit1;		
			CDigestCitation *digestCit = NULL;

			for(TUnitList::iterator it = sentUnits.begin(); it != sentUnits.end(); ++it)
			{
				IUnit *pUnit = *it;
				TCitation* pCit = pUnit->GetCitation();				
				if (pCit!=pPrevCit)
				{
					isChangeCit = true;
				}

				if (isChangeCit)
				{
					digestCit = new CDigestCitation;
					digestCit->pSent = pSent;
					allDigestCitata.push_back(digestCit);
					digestCit->citUnits.push_back(pUnit);
					if (pCit==NULL) digestCit->pSubjectUnit = NULL;
					else if (pCit==pPrevCit1)
					{
						digestCit->pSubjectUnit = (IUnit*)pPrevCit1->pSubjectUnit;
					}else
					{
						ProcessCitataSubjectUnit(pCit);
						digestCit->pSubjectUnit = (IUnit*)pCit->pSubjectUnit;
					}
					isChangeCit = false;
					pPrevCit = pCit;

				}else 
					if (digestCit!=NULL)
					{
						//ProcessCitataSubjectUnit(pCit);
						digestCit->citUnits.push_back(pUnit);						
					}						
			}
			return pPrevCit;
		}

		void CBaseDigestAnalyzer::SetCitationEnableForSynt(ISentence* pSent)
		{
			TUnitList sentUnits;

			pSent->GetLinearUnitsFull(sentUnits);

			for(TUnitList::iterator it = sentUnits.begin(); it!=sentUnits.end(); ++it)
			{
				IUnit* pUnit = *it;
				TCitation* cit = pUnit->GetCitation();
				if (cit)
				{
					if (cit->pBeginCitata)
					{
						((ISentence*)cit->pBeginCitata)->SetEnabled(1);
					}

					if (cit->pEndCitata)
					{
						((ISentence*)cit->pEndCitata)->SetEnabled(1);
					}
				}
			}
		}

		// местоимения - который/которую/которого и т.д.
		void CBaseDigestAnalyzer::DoPronounThird(ISentence* sent, IUnit* pronoun)
		{
			TUnitList relevantList;
			TUnitList noneUnits;	

			SS::Core::Features::CMorphoFeature oMorfoAnfr;
			SS::Core::Features::CMorphoFeature oMorfoPattern;

			int pronounCitataStatus = pronoun->GetCitation()==NULL?0:1;
			unitOperator->GetUnitMorfoFeatures(pronoun,oMorfoAnfr);

			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
			oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			oMorfoPattern.m_GenderType = oMorfoAnfr.m_GenderType;
			oMorfoPattern.m_NumberType  = oMorfoAnfr.m_NumberType;
			oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
			oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern.m_CommonType = SS::Core::Features::Values::CommonType.Undefined;

			FindUnits(sent, oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.Undefined,
				SS::Core::Features::Values::TSemanticType.smtPerson, 2, 
				pronounCitataStatus, pronoun, noneUnits,
				relevantList);

			if (relevantList.size()!=0)
			{
				// надо взять последний в списке он и есть разрешение
				IUnit *lastUnit = *(--relevantList.end());
				pronoun->SetReferenceUnit(lastUnit);
				if (lastUnit->GetIsTargetObjectUnit()==1 || lastUnit->GetIsTargetObjectUnitFromChilds()==1|| 
					lastUnit->GetIsTargetObjectUnitFromParent()==1)
				{
					sent->SetDigest(1);
				}
				return; 
			}
		}

		// ее/его
		void CBaseDigestAnalyzer::DoPronounSecond(ISentence* sent, IUnit* pronoun )
		{
			TUnitList relevantList;
			TUnitList noneUnits;			
			TUnitList sentUnits;

			GetLinearUnitsFull(sent, sentUnits);

			// юниты стоящие ближе чем на три юнита перед пестоимение (за исключением случая когда перед ним стоит союзи И
			// не могут быть разрешением (вероятность мала).
			for(TUnitList::iterator it = sentUnits.begin(); it != sentUnits.end(); ++it)
			{
				if ((*it)==pronoun) {
					if (it!=sentUnits.begin())
					{
						--it;
						IUnit* unit = *it;
						if (wcscmp(unit->GetWord(), L"и") != 0)
						{
							int count =0;
							while(it!=sentUnits.begin())
							{				
								--it;
								noneUnits.push_back(*it);
								count++;
								if (count==3) break;
							}		
						}
					}					
					break;
				}
			}

			SS::Core::Features::CMorphoFeature oMorfoAnfr;
			SS::Core::Features::CMorphoFeature oMorfoPattern;

			int pronounCitataStatus = pronoun->GetCitation()==NULL?0:1;
			unitOperator->GetUnitMorfoFeatures(pronoun,oMorfoAnfr);

			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
			oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.aiaAnimation;
			oMorfoPattern.m_GenderType = oMorfoAnfr.m_GenderType;
			oMorfoPattern.m_NumberType  = oMorfoAnfr.m_NumberType;
			oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
			oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern.m_CommonType = SS::Core::Features::Values::CommonType.Undefined;

			FindUnits(sent, oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.Undefined,
				SS::Core::Features::Values::TSemanticType.smtPerson, 0, 
				pronounCitataStatus, pronoun, noneUnits,
				relevantList);
			if (relevantList.size()!=0)
			{
				// надо взять последний в списке он и есть разрешение
				IUnit *lastUnit = *(--relevantList.end());
				pronoun->SetReferenceUnit(lastUnit);
				if (lastUnit->GetIsTargetObjectUnit()==1 || lastUnit->GetIsTargetObjectUnitFromChilds()==1|| 
					lastUnit->GetIsTargetObjectUnitFromParent()==1)
				{
					sent->SetDigest(1);
				}
				return; 
			}

			FindUnits(sent->GetPrevSentenceLinear(), oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.Undefined,
				SS::Core::Features::Values::TSemanticType.smtPerson, 0, 
				pronounCitataStatus,pronoun,
				noneUnits,relevantList);
			if (relevantList.size()!=0)
			{
				// надо взять последний в списке он и есть разрешение
				IUnit *lastUnit = *(--relevantList.end());
				pronoun->SetReferenceUnit(lastUnit);
				if (lastUnit->GetIsTargetObjectUnit()==1 || lastUnit->GetIsTargetObjectUnitFromChilds()==1|| 
					lastUnit->GetIsTargetObjectUnitFromParent()==1)
				{
					sent->SetDigest(1);
				}
				return; 
			}
			oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			FindUnits(sent, oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.Undefined,
				SS::Core::Features::Values::TSemanticType.Undefined, 0, 
				pronounCitataStatus,pronoun,
				noneUnits,relevantList);
			if (relevantList.size()!=0)
			{
				// надо взять последний в списке он и есть разрешение
				IUnit *lastUnit = *(--relevantList.end());
				pronoun->SetReferenceUnit(lastUnit);
				if (lastUnit->GetIsTargetObjectUnit()==1 || lastUnit->GetIsTargetObjectUnitFromChilds()==1|| 
					lastUnit->GetIsTargetObjectUnitFromParent()==1)
				{
					sent->SetDigest(1);
				}
				return; 
			}

			FindUnits(sent, oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.Undefined,
				SS::Core::Features::Values::TSemanticType.Undefined, 2, 
				pronounCitataStatus,pronoun,
				noneUnits,relevantList);
			if (relevantList.size()!=0)
			{
				// надо взять последний в списке он и есть разрешение
				IUnit *lastUnit = *(--relevantList.end());
				pronoun->SetReferenceUnit(lastUnit);
				if (lastUnit->GetIsTargetObjectUnit()==1 || lastUnit->GetIsTargetObjectUnitFromChilds()==1|| 
					lastUnit->GetIsTargetObjectUnitFromParent()==1)
				{
					sent->SetDigest(1);
				}
				return; 
			}
			FindUnits(sent->GetPrevSentenceLinear(), oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.Undefined,
				SS::Core::Features::Values::TSemanticType.Undefined, 2, 
				pronounCitataStatus,pronoun,
				noneUnits,relevantList);
			if (relevantList.size()!=0)
			{
				// надо взять последний в списке он и есть разрешение
				IUnit *lastUnit = *(--relevantList.end());
				pronoun->SetReferenceUnit(lastUnit);
				if (lastUnit->GetIsTargetObjectUnit()==1 || lastUnit->GetIsTargetObjectUnitFromChilds()==1|| 
					lastUnit->GetIsTargetObjectUnitFromParent()==1)
				{
					sent->SetDigest(1);
				}
				return; 
			}				
		}

		bool SortByPosInSent(IUnit* u1, IUnit* u2)
		{
			return u1->GetFirstCharPosition() < u2->GetFirstCharPosition();
		}

		wstring CBaseDigestAnalyzer::GetAllTermsToNominative(IUnit* pUnit)
		{
			// надо найти списоr всех подъюнитов, вклчюая сам юнит, которые надо преобразовать
			// в именительный падеж. Таковыми являются все имена собственные + терм самого юнита			
			list<IUnit*> allUnits;
			SS::Core::Features::Types::GenderType gendType;
			SS::Core::Features::CMorphoFeature oMorfoPattern;

			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
			oMorfoPattern.m_CommonType = SS::Core::Features::Values::CommonType.cmtProper;
			oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			oMorfoPattern.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
			oMorfoPattern.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
			oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
			oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;		

			unitOperator->GetLinearUnits(pUnit, allUnits);
			allUnits.sort(SortByPosInSent);
			GetGenderForUnit(pUnit, gendType);
			bool isRoot = false;
			wstring res = L"";
			for(list<IUnit*>::iterator it = allUnits.begin(); it!=allUnits.end(); ++it)
			{			
				IUnit* curUnit = *it;
				wstring word = curUnit->GetWord();
				if (curUnit->GetFirstCharPosition()==-1 || word.size()==0) continue;
				if (!isRoot || unitOperator->IsMorpho(curUnit, oMorfoPattern))
				{
					// нормализуем терм.
					word = unitOperator->GetMainFormText(curUnit, gendType);
				}
				if (curUnit == pUnit) isRoot = true;
				res += word + L" ";
			}
            res = unitOperator->QuoteNormalize(res);
			res = unitOperator->SpaceNormalize(res);
			return res;		
		}

		void CBaseDigestAnalyzer::GetGenderForUnit(IUnit* pUnit, SS::Core::Features::Types::GenderType& resType)
		{					
			// имя, потом отчетсво, потом фамилию. если не нашли то мужской берем за основу.
			resType = SS::Core::Features::Values::GenderType.Undefined;
			std::list<SS::Dictionary::Types::TWordInfo> oLst;
			wstring word = pUnit->GetWord();
			if (word.size()==0)
			{
				IUnit* childUnit = pUnit->GetLeftChildUnit();
				while(childUnit!=NULL)
				{
					if (unitOperator->IsSemantic(childUnit, SS::Core::Features::Values::TSemanticType.Undefined,
						SS::Core::Features::Values::TSPersonType.semprstFirstName))
					{
						GetGenderForUnit(childUnit, resType);
						return;
					}
					childUnit = childUnit->GetRightUnit();
				}
				childUnit = pUnit->GetLeftChildUnit();
				while(childUnit!=NULL)
				{
					if (unitOperator->IsSemantic(childUnit, SS::Core::Features::Values::TSemanticType.Undefined,
						SS::Core::Features::Values::TSPersonType.semprstMiddleName))
					{
						GetGenderForUnit(childUnit, resType);
						return;
					}
					childUnit = childUnit->GetRightUnit();
				}
				childUnit = pUnit->GetLeftChildUnit();
				while(childUnit!=NULL)
				{
					if (unitOperator->IsSemantic(childUnit, SS::Core::Features::Values::TSemanticType.Undefined,
						SS::Core::Features::Values::TSPersonType.semprstLastName))
					{
						GetGenderForUnit(childUnit, resType);
						return;
					}
					childUnit = childUnit->GetRightUnit();
				}

				resType = SS::Core::Features::Values::GenderType.gtMasculine;
				return;

			}
			unitOperator->GetWordInfo(word, &oLst, SS::Dictionary::Types::efzOnlySearch);
			if(oLst.empty())
			{ resType = SS::Core::Features::Values::GenderType.gtMasculine; return; }

			for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = oLst.begin(); it != oLst.end(); ++it)
			{	
				SS::Core::Features::CMorphoFeature oMorphoFeature;			
				SS::Dictionary::Types::SWordInfo* pwi1 = &(it->front());

				m_pAMConverter->EnCode(
					pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
					pwi1->m_MorphoTypeInfo.m_MorphoInfo,
					pwi1->m_SpecialMorpho,
					&oMorphoFeature);

				if (oMorphoFeature.m_GenderType.IsIntersectsWith(&SS::Core::Features::Values::GenderType.gtMasculine))
				{
					resType = SS::Core::Features::Values::GenderType.gtMasculine;
					return;
				}else
				{
					resType = oMorphoFeature.m_GenderType;
				}
			}				
		}

		void CBaseDigestAnalyzer::DoPronounMain(ISentence* sent, IUnit* pronoun)
		{
			SS::Core::Features::CMorphoFeature oMorfoAnfr;
			SS::Core::Features::CMorphoFeature oMorfoPattern;

			TUnitList relevantList;
			TUnitList noneUnits;

			int pronounCitataStatus = pronoun->GetCitation()==NULL?0:1;
			unitOperator->GetUnitMorfoFeatures(pronoun,oMorfoAnfr);
			// теперь надо найти наиболее релевантное разрешения для местоимения

			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
			oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.aiaAnimation;
			oMorfoPattern.m_GenderType = oMorfoAnfr.m_GenderType;
			oMorfoPattern.m_NumberType  = oMorfoAnfr.m_NumberType;
			oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
			oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern.m_CommonType = SS::Core::Features::Values::CommonType.Undefined;

			FindUnits(sent, oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.scSubjectFin,
				SS::Core::Features::Values::TSemanticType.Undefined, 0, 
				pronounCitataStatus,pronoun,
				noneUnits,relevantList);					
			if (relevantList.size()!=0)
			{
				// надо взять последний в списке он и есть разрешение
				IUnit *lastUnit = *(--relevantList.end());
				pronoun->SetReferenceUnit(lastUnit);			
				if (lastUnit->GetIsTargetObjectUnit()==1 || lastUnit->GetIsTargetObjectUnitFromChilds()==1|| 
					lastUnit->GetIsTargetObjectUnitFromParent()==1)
				{
					sent->SetDigest(1);
				}
				return; 
			}

			FindUnits(sent,oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.Undefined,
				SS::Core::Features::Values::TSemanticType.smtPerson, 0, 
				pronounCitataStatus,pronoun,
				noneUnits,relevantList);
			if (relevantList.size()!=0)
			{
				// надо взять последний в списке он и есть разрешение
				IUnit *lastUnit = *(--relevantList.end());
				pronoun->SetReferenceUnit(lastUnit);
				if (lastUnit->GetIsTargetObjectUnit()==1 || lastUnit->GetIsTargetObjectUnitFromChilds()==1|| 
					lastUnit->GetIsTargetObjectUnitFromParent()==1)
				{
					sent->SetDigest(1);
				}
				return; 
			}

			FindUnits(sent->GetPrevSentenceLinear(),oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.scSubjectFin,
				SS::Core::Features::Values::TSemanticType.Undefined, 0, 
				pronounCitataStatus,pronoun,
				noneUnits,relevantList);
			if (relevantList.size()!=0)
			{
				// надо взять последний в списке он и есть разрешение
				IUnit *lastUnit = *(--relevantList.end());
				pronoun->SetReferenceUnit(lastUnit);
				if (lastUnit->GetIsTargetObjectUnit()==1 || lastUnit->GetIsTargetObjectUnitFromChilds()==1|| 
					lastUnit->GetIsTargetObjectUnitFromParent()==1)
				{
					sent->SetDigest(1);
				}
				return; 
			}


			FindUnits(sent, oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.Undefined,
				SS::Core::Features::Values::TSemanticType.Undefined, 0, 
				pronounCitataStatus,pronoun,
				noneUnits,relevantList);
			if (relevantList.size()!=0)
			{
				// надо взять последний в списке он и есть разрешение
				IUnit *lastUnit = *(--relevantList.end());
				pronoun->SetReferenceUnit(lastUnit);
				if (lastUnit->GetIsTargetObjectUnit()==1 || lastUnit->GetIsTargetObjectUnitFromChilds()==1|| 
					lastUnit->GetIsTargetObjectUnitFromParent()==1)
				{
					sent->SetDigest(1);
				}
				return; 
			}


			FindUnits(sent->GetPrevSentenceLinear(), oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.Undefined,
				SS::Core::Features::Values::TSemanticType.Undefined, 0,
				pronounCitataStatus,pronoun,
				noneUnits,relevantList);
			if (relevantList.size()!=0)
			{
				// надо взять последний в списке он и есть разрешение
				IUnit *lastUnit = *(--relevantList.end());
				pronoun->SetReferenceUnit(lastUnit);
				if (lastUnit->GetIsTargetObjectUnit()==1 || lastUnit->GetIsTargetObjectUnitFromChilds()==1|| 
					lastUnit->GetIsTargetObjectUnitFromParent()==1)
				{
					sent->SetDigest(1);
				}
				return; 
			}


			oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			FindUnits(sent, oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.Undefined,
				SS::Core::Features::Values::TSemanticType.Undefined, 2,
				pronounCitataStatus,pronoun,
				noneUnits,relevantList);
			if (relevantList.size()!=0)
			{
				// надо взять последний в списке он и есть разрешение
				IUnit *lastUnit = *(--relevantList.end());
				pronoun->SetReferenceUnit(lastUnit);
				if (lastUnit->GetIsTargetObjectUnit()==1 || lastUnit->GetIsTargetObjectUnitFromChilds()==1|| 
					lastUnit->GetIsTargetObjectUnitFromParent()==1)
				{
					sent->SetDigest(1);
				}
				return; 
			}


			FindUnits(sent->GetPrevSentenceLinear(), oMorfoPattern,
				SS::Core::Features::Values::SyntaxCategories.Undefined,
				SS::Core::Features::Values::TSemanticType.Undefined, 2, 
				pronounCitataStatus,pronoun,
				noneUnits,relevantList);
			if (relevantList.size()!=0)
			{
				// надо взять последний в списке он и есть разрешение
				IUnit *lastUnit = *(--relevantList.end());
				pronoun->SetReferenceUnit(lastUnit);
				if (lastUnit->GetIsTargetObjectUnit()==1 || lastUnit->GetIsTargetObjectUnitFromChilds()==1|| 
					lastUnit->GetIsTargetObjectUnitFromParent()==1)
				{
					sent->SetDigest(1);
				}
				return; 
			}		
		}

		TCitation* CBaseDigestAnalyzer::CreateCitation(ISentence* firstSent, ISentence *endSent, IUnit *subjUnit)
		{
			TCitation* cit = new TCitation();
			allCitata.push_back(cit);
			cit->pBeginCitata = firstSent;
			cit->pEndCitata = endSent;
			cit->pSubjectUnit = subjUnit;
			return cit;
		}

		// эвристика 1. Если предложение начинается с кавычки, то считаем количество оставшихся кавычек в этом предложении,
		// которые при этом не вязвялются последовательностью кавычка-запятая-тире. Если их нечетное число - то кавычка не начало
		// цитаты, а в противном случае - начало.
		int CBaseDigestAnalyzer::EvristicOneStart(ISentence* pSen, TUnitList& inQuoteList)
		{
			TUnitList sentenceUnits;
			TUnitList sentenceLinearUnits;
			TUnitList findedUnits;
			TUnitList findedUnits1;
			TCitation* curCit= NULL;

			pSen->GetLinearUnitsFull(sentenceUnits);	
			pSen->GetLinearUnits(sentenceLinearUnits);
			
			if (sentenceLinearUnits.size()<3) return 0;
			IUnit* unit_start = (*sentenceLinearUnits.begin());
			wstring word = unit_start->GetWord();					
			// эвристика 1
			if (word==L"\"")
			{				
				// ищем последовательность кавычка-запятая-тире. Если нашли - то цитата все от начала предложения до нее.
				// если нет, то считаем количество кавычек в предложении
				citataLastTermOne->FindObjectInSentence(pSen, findedUnits, true,NULL);
				if (findedUnits.size()!=0)
				{
					// все юниты между термами - цитата
					IUnit* unit_end = *(findedUnits.begin());			
					curCit = CreateCitation(pSen, pSen, NULL);
					curCit->nType = 1;
					for(TUnitList::iterator it1 = sentenceUnits.begin(); it1 != sentenceUnits.end(); ++it1)
					{
						IUnit* citUnit = *it1;
						if (citUnit->GetFirstCharPositionNew()>unit_start->GetFirstCharPositionNew() &&
							citUnit->GetFirstCharPositionNew()<unit_end->GetFirstCharPositionNew())
						{
							citUnit->SetCitation(curCit);
						}
					}				
					return 1; // нашли сразу в предложении окончание цитаты
				}else
				{
					citataQuoteTerm->FindObjectInSentence(pSen, findedUnits, true,NULL);
					if (findedUnits.size()%2==1)
					{													
						for(TUnitList::iterator it1 = sentenceUnits.begin(); it1 != sentenceUnits.end(); ++it1)
						{
							IUnit* citUnit = *it1;
							if (citUnit->GetFirstCharPositionNew()>unit_start->GetFirstCharPositionNew())
								inQuoteList.push_back(citUnit);
						}
						return 2; //нашли начало цитаты
					}
				} // else findedUnits.size()							
			} // if str = "
			return 0; // не нашли начала цитаты или вся цитата помеситлась в это предложение
		}

		// эвристика 2. Если в предложении встречается двоеточие-кавччка, то это начало цитаты.
		int CBaseDigestAnalyzer::EvristicTwoStart(ISentence* pSen, TUnitList& inQuoteList)
		{
			TUnitList sentenceUnits;
			TUnitList findedUnits;
			TUnitList findedUnits1;
			TCitation* curCit= NULL;

			pSen->GetLinearUnitsFull(sentenceUnits);	
			citataFirstTerm->FindObjectInSentence(pSen, findedUnits, true, NULL);
			if (findedUnits.size()!=0) // нашли
			{
				TUnitList::iterator it  = findedUnits.end();
				--it;
				IUnit *lastFindedUnit = *it;
				bool bCitBegin = false;

				citataLastTermTwo->FindObjectInSentence(pSen, findedUnits1, true, lastFindedUnit);
				if (findedUnits1.size()!=0)
				{
					// тут же нашли и конец цитаты.
					curCit = CreateCitation(pSen, pSen, NULL);	
					curCit->nType = 2;					
					for(TUnitList::iterator it1 = sentenceUnits.begin(); it1 != sentenceUnits.end(); ++it1)
					{
						IUnit *curUnit = *it1;
						if (curUnit->GetFirstCharPositionNew()>lastFindedUnit->GetFirstCharPositionNew() &&
							curUnit->GetFirstCharPositionNew()<(*findedUnits1.begin())->GetFirstCharPositionNew())
						{
							curUnit->SetCitation(curCit);
						}					
					}
					return 1;
				}else
				{							
					for(TUnitList::iterator it1 = sentenceUnits.begin(); it1 != sentenceUnits.end(); ++it1)
					{
						IUnit *curUnit = *it1;
						if (curUnit->GetFirstCharPositionNew()>lastFindedUnit->GetFirstCharPositionNew())
						{
							inQuoteList.push_back(curUnit);
						}							
					}
					return 2;
				}
			}
			return 0;
		}

		int CBaseDigestAnalyzer::EvristicThreeStart(ISentence* pSen, TUnitList& inQuoteList)
		{
			TUnitList sentenceUnits;
			TUnitList findedUnits;
			TUnitList findedUnits1;
			TCitation* curCit= NULL;
			IUnit *lastFindedUnit1 = NULL;

			TUnitList inQuoteListIn;
			SS::Core::Features::CMorphoFeature resMorphoFeature;

			pSen->GetLinearUnitsFull(sentenceUnits);	
			citataVerbs->FindObjectInSentence(pSen, findedUnits, true,NULL); // ищем спец глагол
			// дополнительно надо проверить, что глагол в активной форме и персональный
			for(TUnitList::iterator it1 = findedUnits.begin(); it1 != findedUnits.end(); ++it1)
			{
				unitOperator->GetUnitMorfoFeatures(*it1, resMorphoFeature);
				if (resMorphoFeature.m_VerbForm.IsIntersectsWith(&SS::Core::Features::Values::VerbForm.vfPersonal) &&
					resMorphoFeature.m_VoiceType.IsIntersectsWith(&SS::Core::Features::Values::VoiceType.vtActive))
				{
					findedUnits1.push_back(*it1);
				}
			}
			//citataLastTermThree->FindObjectInSentence(pSen, findedUnits1, true); // ищем спец глагол
			if (findedUnits1.size()!=0) // нашли
			{
				// теперь надо найти слово ЧТО не далее чем в 6 юнитах
				int nUnitCount = 0;
				TUnitList::iterator it  = findedUnits1.begin();			
				IUnit *lastFindedUnit = *it;
				int nPossibleCit = 0;
				for(TUnitList::iterator it1 = sentenceUnits.begin(); it1 != sentenceUnits.end(); ++it1)
				{
					IUnit *curUnit = *it1;
					if (curUnit==lastFindedUnit) // нашли юнит
					{
						nPossibleCit = 1;
						continue;
					}
					if (nPossibleCit==1)
					{
						wstring str = curUnit->GetWord();
						if (str==L"что")
						{
							nPossibleCit = 2;
							nUnitCount = 0;
						}else
						{
							nUnitCount++;
						}
						continue;
					}

					if (nPossibleCit==2)
					{
						wstring str = curUnit->GetWord();
						inQuoteListIn.push_back(curUnit);
						if (str==L"\"" && nUnitCount<5)
						{
							// нашли начало цитаты
							nPossibleCit = 3;
							lastFindedUnit1 = curUnit;						
						}
						else
						{
							nUnitCount++;
						}
						continue;
					}

					if (nPossibleCit==3)
					{
						inQuoteListIn.push_back(curUnit);
					}

					if (nPossibleCit==1)
						if (nUnitCount>6) break; // не нашли что					
				}

				if (nPossibleCit==3)
				{
					findedUnits1.clear();
					citataLastTermThree->FindObjectInSentence(pSen, findedUnits1, true, lastFindedUnit1);
					if (findedUnits1.size()!=0) // нащли конец в этом же предложении
					{
						curCit = CreateCitation(pSen, pSen, NULL);
						curCit->nType = 3;
						for(TUnitList::iterator it1 = inQuoteListIn.begin(); it1 != inQuoteListIn.end(); ++it1)
						{
							IUnit* curUnit = *it1;
							if (curUnit->GetFirstCharPositionNew()<
								(*findedUnits1.begin())->GetFirstCharPositionNew()) break;
							curUnit->SetCitation(curCit);
						}
						return 1;
					}
					else
					{
						for(TUnitList::iterator it1 = inQuoteListIn.begin(); it1 != inQuoteListIn.end(); ++it1)
						{
							IUnit* curUnit = *it1;
							inQuoteList.push_back(curUnit);
						}
						return 2; // цитата
					}
				}else
					if (nPossibleCit==2)
					{
						curCit = CreateCitation(pSen, pSen, NULL);
						curCit->nType = 3;
						for(TUnitList::iterator it1 = inQuoteListIn.begin(); it1 != inQuoteListIn.end(); ++it1)
						{
							IUnit* curUnit = *it1;
							//if (curUnit==(*findedUnits1.begin())) break;
							curUnit->SetCitation(curCit);
						}
						return 1;
					}
			}
			return 0;
		}

		// по его словам/по словам Иванова
		int CBaseDigestAnalyzer::EvristicFourStart(ISentence* pSen, TUnitList& inQuoteList)
		{
			TUnitList sentenceUnits;
			TUnitList findedUnits;
			TUnitList findedUnits1;
			TCitation* curCit= NULL;

			pSen->GetLinearUnitsFull(sentenceUnits);	
			citataFirstTermFour1->FindObjectInSentence(pSen, findedUnits, true, NULL);
			citataFirstTermFour2->FindObjectInSentence(pSen, findedUnits, true, NULL);
			if (findedUnits.size()!=0) // нашли
			{
				TUnitList::iterator it  = findedUnits.end();
				--it;
				IUnit *lastFindedUnit = *it;
				bool isCitStart = false;
				int nPossibleCit = 0;

				for(TUnitList::iterator it1 = sentenceUnits.begin(); it1 != sentenceUnits.end(); ++it1)
				{
					IUnit *curUnit = *it1;

					if (curUnit==lastFindedUnit)
					{
						nPossibleCit = 1;
						continue;
					}
					wstring str = curUnit->GetWord();
					if (nPossibleCit == 1 && str == L",") // начало цитаты
					{
						curCit = CreateCitation(pSen, pSen, NULL);
						curCit->nType = 4;
						nPossibleCit = 2;
						continue;
					}
					if (nPossibleCit == 2)
					{
						curUnit->SetCitation(curCit);
					}
				}	
				return 1;
			}
			return 0;
		}

		void CBaseDigestAnalyzer::DoCitation1(IText* text, bool isFull)
		{			
			// эвристика 1. Если предложение начинается с кавычки, то считаем количество оставшихся кавычек в этом предложении,
			// которые при этом не вязвялются последовательностью кавычка-запятая-тире. Если их нечетное число - то кавычка не начало
			// цитаты, а в противном случае - начало.
			// эвристика 2. Если в предложении встречается двоеточие-кавччка, то это начало цитаты.
			// эвристика 3. Если в предложении встречается спецглагол,за ним без знаков препинаний, кроме запятой перед ЧТО
			// есть слово ЧТО, после которого на расстоянии не более чем 3 юнита есть кавычка, то эта кавычка начало цитаты 
			// эвристика 4. Если в предложении встречается спецглагол,за ним без знаков препинаний, кроме запятой перед ЧТО
			// есть слово ЧТО, и нет кавычки дальше, то все что идет сразу после что и до конца предложения - цитата.
			//
			int EvristicNumber = 0;
			ISentence* pCitStartSen = NULL;
			for(IParagraph* pPar = text->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
			{	
				// цитаты через параграфы проходить не могут		
				int nCitStart = 0;
				TCitation* curCit= NULL;
				TUnitList inQuoteList;		
				pCitStartSen = NULL;
				EvristicNumber = 0;
				int numOfSentInCit = 0;

				for(ISentence* pSen = pPar->GetFirstSentence(); pSen; pSen = pSen->GetNextSentence())
				{	
					TUnitList sentenceUnits;
					TUnitList findedUnits;
					TUnitList findedUnits1;

					pSen->GetLinearUnitsFull(sentenceUnits);
					if (nCitStart==2) // цитата уже идет - надо найти ее конец
					{
						// надо найти последовательно кавычка-запята-тире в предложении
						// если нашли - цитата закончена. если не нашли, то все юниты предложения
						// заносим в список предварительного отобора. Если нашли, то всем юнитам
						// из списка отбора и юнитам текущего предложения вплоть до найденной поседоватлеьности надо
						// сопоставить цитату. но если наступит конец параграфа, а конец цитаты так и не нашли, то значит какая-то бага в вормате текса и юниты не делаем цитируемыми
						if (EvristicNumber==1)
							citataLastTermOne->FindObjectInSentence(pSen, findedUnits, true, NULL);
                        else if (EvristicNumber==2 || EvristicNumber==3 || EvristicNumber==4)
						    citataLastTermTwo->FindObjectInSentence(pSen, findedUnits, true, NULL);

						if (numOfSentInCit>10000)
						{
							numOfSentInCit = 0;
							inQuoteList.clear();
							EvristicNumber = 0;
							pCitStartSen = NULL;
							nCitStart = 0;
							continue;
						}

						if (findedUnits.size() != 0) // нашли конец цитаты
						{	
							curCit = CreateCitation(pCitStartSen, pSen, NULL);
							curCit->nType = EvristicNumber;
							for(TUnitList::iterator it = sentenceUnits.begin(); it != sentenceUnits.end(); ++it)
							{							
								IUnit* citUnit = *it;
								if (citUnit==(*findedUnits.begin()))
                                    break;
								citUnit->SetCitation(curCit);
							}
							for(TUnitList::iterator it = inQuoteList.begin(); it != inQuoteList.end(); ++it)
							{
								IUnit* citUnit = *it;
								citUnit->SetCitation(curCit);
							}
							EvristicNumber = 0;
							pCitStartSen = NULL;
							nCitStart = 0;
							inQuoteList.clear();
							numOfSentInCit = 0;		
						}
                        else
						{
							for(TUnitList::iterator it = sentenceUnits.begin(); it != sentenceUnits.end(); ++it)
								inQuoteList.push_back(*it);
							numOfSentInCit++;
						}					
					}
					else
					{								
						nCitStart = EvristicOneStart(pSen,inQuoteList);
						if (nCitStart==2)
						{
							EvristicNumber = 1;
							pCitStartSen = pSen;
							continue;
						}
                        else if (nCitStart==1)
                            continue;

						// эврсистика 2 (первая не сработала)					
						nCitStart = EvristicTwoStart(pSen,inQuoteList);
						if (nCitStart==2)
						{
							EvristicNumber = 2;
							pCitStartSen = pSen;
							continue;
						}
                        else if (nCitStart==1)
                            continue;

						nCitStart = EvristicThreeStart(pSen,inQuoteList);
						if (nCitStart==2)
						{
							EvristicNumber = 3;
							pCitStartSen = pSen;
							continue;
						}
                        else if (nCitStart==1)
                            continue;

						nCitStart = EvristicFourStart(pSen, inQuoteList);
						if (nCitStart==2)
						{
							EvristicNumber = 4;
							pCitStartSen = pSen;
							continue;
						}
                        else if (nCitStart==1)
                            continue;

						if (nCitStart==0 && isFull)
						{
							curCit = CreateCitation(pSen, pSen, NULL);
						}
					} // else isCitStart
				} // for  sent
			} // for par
			if (EvristicNumber!=0)
			{
				CreateCitation(pCitStartSen, text->GetLastParagraph()->GetLastSentence(), NULL);
			}
		}

		void CBaseDigestAnalyzer::DoCitation(IText* text)
		{
			// найдо натйи в тексте все цитаты. Т.е. нечто начинающееся кавычкой и заканчивающеея кавычкой
			// при этом либо перед первой кавычкой должен быть зна двоеточия, либо после последней - знаки запятой и тире.
			// Цитата може занимать несколько предложений. С каждый юнитом, если он внутри цитаты
			// связывается объект типа CCitation. При этом этото объкт включает в себя информацию о предложении
			// начале цитаты и предложении конце. Чтобы потом опредлить по этиим предложениям кто являлся субъектом уитирования		
			// больше 
			for(IParagraph* pPar = text->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
			{	
				// цитаты через параграфы проходить не могут
				bool isCitStart = false;
				TCitation* curCit= NULL;
				list<IUnit*> inQuoteList;
				for(ISentence* pSen = pPar->GetFirstSentence(); pSen;	pSen = pSen->GetNextSentence())
				{	
					//ISentence *sent = *it;
					// теперь ищем в предложение последовательность : " или просто кавычку если она начало предложения
					list<IUnit*> sentenceUnits;
					pSen->GetLinearUnitsFull(sentenceUnits);			
					bool isFirstQuote =false;
					int nQuouteCount = 0;
					for(list<IUnit*>::iterator it = sentenceUnits.begin(); it!=sentenceUnits.end(); ++it)
					{
						IUnit* unit = *it;
						wstring str = unit->GetWord();
						if (str==L":" || it == sentenceUnits.begin())
						{					
							if (it != sentenceUnits.begin()) ++it;
							else isFirstQuote = true;

							if (it!=sentenceUnits.end())
							{
								IUnit* nextUnit = *it;
								wstring strNext = nextUnit->GetWord();
								if (strNext == L"\"") // фиксируем начало цитаты
									// теперь надо найти следующие символы ", - или ". или "! или "?
									// в предложении. Если их четсное число или 0 - значит цитата не закрывается в этом предложении
									// четное число может быть в том случае если внутри цитаты встретится  наименование в кавычках
								{								
									isCitStart = true;
									curCit = new TCitation();
									curCit->pBeginCitata = pSen;
									curCit->pEndCitata = NULL;
									curCit->pSubjectUnit = NULL;
								}
							}
						}else
						{
							if (str==L"\"")
							{
								if (isCitStart)
								{
									++it;
									bool isEndFind = false;
									if (it!=sentenceUnits.end())
									{
										IUnit* nextUnit = *it;
										wstring strNext = nextUnit->GetWord();
										if (strNext==L"." || strNext==L";" || strNext==L"!" || strNext == L"?")
										{
											//нашли конец цитаты
											isCitStart = false;
											for(list<IUnit*>::iterator it=inQuoteList.begin(); it!=inQuoteList.end(); ++it)
											{
												IUnit* pUn = *it;
												pUn->SetCitation(curCit);
											}
										}else
											if (strNext==L",")
											{
												++it;
												if (it!=sentenceUnits.end())
												{
													nextUnit = *it;
													strNext.clear();
													strNext = nextUnit->GetWord();
													wchar_t ch = (wchar_t)8211; // тире может быть и этим символом
													wstring strCh;
													strCh.clear();
													strCh.push_back(ch);
													//wchar_t ch1 = strNext.c_str()[0];
													if (strNext==L"-" || strNext==strCh)
													{
														// нашли конец цитаты
														isCitStart = false;
														for(list<IUnit*>::iterator it=inQuoteList.begin(); it!=inQuoteList.end(); ++it)
														{
															IUnit* pUn = *it;
															pUn->SetCitation(curCit);
														}
													}
												}
											} // if str == ","
											else // просто кавыка
												nQuouteCount++; //считаем число кавычек

									}else isEndFind = true;
								} // if isCitStart

							} // if str == """
							else {		
								if (isCitStart) inQuoteList.push_back(unit);
							}
						} // else if str==":"					
					} // for unit
					if (isFirstQuote) // если эвристика - по первому символу кавычке, то
						// счтиаем сколько в оставшихся в предложении кавычек (без , -) если нечетное, то это не цитата, а наименование в начале прделоежни
					{
						if (nQuouteCount % 2==1)
						{
							isCitStart = false;
							inQuoteList.clear();
						}
						isFirstQuote = false;
						nQuouteCount = 0;
					}
				} // for sent
			} // for par
		}	

		void CBaseDigestAnalyzer::DoKoref(IText* text, IObjectDescr* objDescr)
		{			
			TUnitList relevantList;

			for (TSentList::iterator it = m_lSentences.begin();  it != m_lSentences.end(); ++it)
			{
				ISentence *sent = (*it).pSentence;
				if (!sent->IsPreDigest())
				{
					continue;
				}

				objDescr->FindObjectInSentence(sent, relevantList, false, NULL , true);
				for (TUnitList::iterator itUnit = relevantList.begin(); itUnit!=relevantList.end(); ++itUnit)
				{
					IUnit* pUnit = *itUnit;
					pUnit->SetIsTargetObjectUnit(1);
				}

				if (relevantList.size()!=0) 
				{
					sent->SetDigest(1);
				}
			}
		}

		bool CBaseDigestAnalyzer::IsPronounMain(IUnit* pUnit)
		{
			SS::Core::Features::CMorphoFeature oMorfoPattern;

			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postPronoun;
			oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			oMorfoPattern.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
			oMorfoPattern.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
			oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.ctNominative;
			oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.pncIndividual;
			oMorfoPattern.m_CommonType = SS::Core::Features::Values::CommonType.Undefined;

			return unitOperator->IsMorpho(pUnit, oMorfoPattern);
		}

		void CBaseDigestAnalyzer::DoAnaphora(IText* text, bool isFull)
		{
			SS::Core::Features::CMorphoFeature oMorfoPattern;
			TUnitList noneUnits;

			for(TSentList::iterator it = m_lSentences.begin();  it != m_lSentences.end(); ++it)
			{
				ISentence *sent = (*it).pSentence;
				if (!sent->IsPreDigest())
					continue;

				// теперь ищем анафорию в предложении и пытаемся разрешить ее на
				// термы из текущего предложения
				list<IUnit*> anaphoraList;
				list<IUnit*> relevantList;
				// сначала местоимения типа он, она, оно. Эти местоимения всегда подлежащие
				oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postPronoun;
				oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
				oMorfoPattern.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
				oMorfoPattern.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
				oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.ctNominative;
				oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.pncIndividual;
				oMorfoPattern.m_CommonType = SS::Core::Features::Values::CommonType.Undefined;

				FindUnits(sent, oMorfoPattern,
					SS::Core::Features::Values::SyntaxCategories.Undefined,
					SS::Core::Features::Values::TSemanticType.Undefined, 2, 2,NULL, noneUnits, anaphoraList);
				for(list<IUnit*>::iterator it = anaphoraList.begin(); it!=anaphoraList.end(); ++it)
				{
					IUnit* pUnit = *it;
					DoPronounMain(sent,pUnit);						
				}

				if (isFull)
				{
					anaphoraList.clear();

					oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.ctAccusative;
					oMorfoPattern.m_CaseType.AssignOR(&SS::Core::Features::Values::CaseType.ctGenitive);
					oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.pncIndividual;
					oMorfoPattern.m_CommonType = SS::Core::Features::Values::CommonType.Undefined;

					FindUnits(sent, oMorfoPattern,
						SS::Core::Features::Values::SyntaxCategories.Undefined,
						SS::Core::Features::Values::TSemanticType.Undefined, 2, 2,NULL, noneUnits, anaphoraList);

					for(list<IUnit*>::iterator it = anaphoraList.begin(); it!=anaphoraList.end(); ++it)
					{
						IUnit* pUnit = *it;
						DoPronounSecond(sent,pUnit);						
					}
					anaphoraList.clear();					
					pronounAnfrThird->FindObjectInSentence(sent, anaphoraList, true, NULL);
					for(list<IUnit*>::iterator it = anaphoraList.begin(); it!=anaphoraList.end(); ++it)
					{
						IUnit* pUnit = *it;
						DoPronounThird(sent,pUnit);						
					}
				}
			}
		}

		bool CBaseDigestAnalyzer::IsRelevantChildUnits(IUnit* parent)
		{
			// если родительский юнит - персона то среди дочерних не должно быть союзов
			SS::Core::Features::CMorphoFeature oMorfoPattern;
			
			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
			oMorfoPattern.m_AnimInAnimType = SS::Core::Features::Values::AnimInAnimType.Undefined;
			oMorfoPattern.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
			oMorfoPattern.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
			oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
			oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern.m_CommonType =  SS::Core::Features::Values::CommonType.Undefined;
			if (unitOperator->IsSemantic(parent, SS::Core::Features::Values::TSemanticType.smtPerson))
			{
				IUnit* pChildUnit = parent->GetLeftChildUnit();
				while (pChildUnit)
				{			  			
					if (unitOperator->IsMorpho(pChildUnit, oMorfoPattern)) return false;
					pChildUnit = pChildUnit->GetRightUnit();
				}
			}
			return true;
		}

		void CBaseDigestAnalyzer::SetParentTarget(ISentence* sent, IUnit* unit)
		{
			SS::Core::Features::CMorphoFeature oMorfoPattern;
			TUnitList findedUnitsStop;

			unitOperator->GetUnitMorfoFeatures(unit,oMorfoPattern);
			stopListForGen->FindObjectInSentence(sent, findedUnitsStop, false, NULL);

			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;			
			oMorfoPattern.m_GenderType = SS::Core::Features::Values::GenderType.Undefined;
			oMorfoPattern.m_NumberType  = SS::Core::Features::Values::NumberType.Undefined;
			oMorfoPattern.m_CaseType = SS::Core::Features::Values::CaseType.Undefined;
			oMorfoPattern.m_PronounClass = SS::Core::Features::Values::PronounClass.Undefined;
			oMorfoPattern.m_CommonType =  SS::Core::Features::Values::CommonType.Undefined;
			IUnit* parent = unit->GetParentUnit();
			while(parent)
			{
				wstring word = parent->GetWord();
				if (unitOperator->FindInUnits(findedUnitsStop, parent))
				{
					break;
				}
				if (((unitOperator->IsMorpho(parent, oMorfoPattern) && word.size()!=0) || (word.size()==0 && 
					unitOperator->IsSemantic(parent, SS::Core::Features::Values::TSemanticType.smtPerson))) && IsRelevantChildUnits(parent))
				{
					//parent->SetIsTargetObjectUnit(1);
					parent->SetIsTargetObjectUnitFromChilds(1);
				}
                else
                    break;
				parent = parent->GetParentUnit();
			}
		}

        void CBaseDigestAnalyzer::AdditionalObjectSearch(
            IObjectDescr* objDescr, ISentence* pSent, TUnitList& findedUnits, const list<int>& offsets)
        {
            if (offsets.empty())
			{
                return;
			}

            set<IUnit*> findUnits;
            TUnitList sentUnits;
		    pSent->GetLinearUnits(sentUnits);

            int sentOffset = unitOperator->GetSentenceOffset(pSent);
            list<int>::const_iterator offsetIt = offsets.begin();
            for (; offsetIt != offsets.end(); ++offsetIt)
			{
                if ((*offsetIt) >= sentOffset)
				{
                    break;
				}
			}

            for (TUnitList::iterator it = sentUnits.begin(); it != sentUnits.end(); ++it)
            {
                IUnit *pUnit = *it;
                int offset = unitOperator->GetUnitOffset(pUnit);
                if (offsetIt != offsets.end())
                {
                    if (offset == (*offsetIt))
                    {
                        TUnitList units;
                        unitOperator->GetLinearUnitsWithSort(pUnit, units);
                        for (TUnitList::iterator iter = units.begin(); iter != units.end(); ++iter)
						{
                            findUnits.insert(*iter);
						}
                    }

                    if (offset >= (*offsetIt))
					{
                        ++offsetIt;
					}
                }
            }

            for (set<IUnit*>::iterator it = findUnits.begin(); it != findUnits.end(); ++it)
			{
                findedUnits.push_back(*it);
			}
        }

		void CBaseDigestAnalyzer::AnalyzeNextAfterSynt( IText* text, IObjectDescr* objDescr, list<int>& offs)
		{	
            LoadToneDicts();
			mainObject = objDescr;
			TUnitList findedUnits;
			TUnitList findedUnitsStop;
			objDescr->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, false);
			CToneDictPool *tdp = m_ToneDictMap[mainObject->GetType()];
			for (TSentList::iterator it = m_lSentences.begin();  it != m_lSentences.end(); ++it)
			{
				set<IUnit*> setUnits;
				set<IUnit*> nonObjUnits;
				ISentence *sent = (*it).pSentence;
				if (!sent->IsPreDigest())
				{
                    continue;
				}
				objDescr->FindObjectInSentence(sent, findedUnits, false, NULL);
				stopListForGen->FindObjectInSentence(sent, findedUnitsStop, false, NULL);
				for(TUnitList::iterator itUnit = findedUnits.begin(); itUnit!=findedUnits.end(); ++itUnit)
				{
					setUnits.insert(*itUnit);
				}

				for (TUnitList::iterator itUnit = findedUnits.begin(); itUnit!=findedUnits.end(); ++itUnit)
				{
					IUnit* curUnit = *itUnit;
					int nPos = curUnit->GetPosition();
					for (TUnitList::iterator itUnitStop =findedUnitsStop.begin(); itUnitStop!=findedUnitsStop.end(); ++itUnitStop)
					{
						IUnit* curStopUnit = *itUnitStop;
						if (setUnits.find(curStopUnit)==setUnits.end() && curStopUnit->GetPosition() == nPos-1)
						{
							int nonpos = curStopUnit->GetPosition();
							for (TUnitList::iterator itUnitNon = findedUnits.begin(); itUnitNon!=findedUnits.end(); ++itUnitNon)
							{
								IUnit* nonUnit = *itUnitNon;
								if (nonUnit->GetPosition() == nonpos+1)
								{
									nonpos++;
									nonObjUnits.insert(nonUnit);
								}
							}
						}
					}
				}

                AdditionalObjectSearch(objDescr, sent, findedUnits, offs);
                bool isFindTarget = false;
				for (TUnitList::iterator itUnit = findedUnits.begin(); itUnit!=findedUnits.end(); ++itUnit)
				{				
					IUnit* pUnit = *itUnit;
					if (nonObjUnits.find(pUnit) == nonObjUnits.end())
					{
						pUnit->SetIsTargetObjectUnit(1);
						// надо дополнительно родительские объекты сделать целевыми
						SetParentTarget(sent, pUnit);
						isFindTarget = true;
					}
				}
				if (isFindTarget)
				{
                    sent->SetDigest(1); 
				}
				// по синониму - предложение нам не подходит
				findedUnits.clear();
			}
			DoKoref(text, objDescr);
			DoAnaphora(text);
			ProcessCitationSubject();
			int max_neg = 0;
			int max_pos = 0;
			int avg_neg = 0;
			int avg_pos = 0;			
			// теперь у каждого предложения есть оценка тональности
			// определяем два числа - среднее и максмум по предложениям.
			// и берем сумму этих двух чисел деленуцю на два
		}

		ULONG CBaseDigestAnalyzer::Release()
		{
			delete this;
			return 0;
		}

		HRESULT CBaseDigestAnalyzer::QueryInterface(REFIID pIID, void** pBase)
		{
			if (pIID == IID_DigestAnalyzer)
			{
				*pBase = static_cast<SS::Interface::Core::IDigestAnalyzer*> (this);
			}
			else if (pIID == IID_Base)
			{
				*pBase = static_cast<SS::Interface::IBase*>( this );
			}
			else
			{
				*pBase = NULL;
			}
			return S_OK;	
		}

		void CBaseDigestAnalyzer::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
		{
			SS_TRY
			{
				if (m_pMorphoAnalyzer==NULL)
				{
					SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);

					// analyse params		
					m_pBlackBoxTextStorage =
						(IBlackBoxTextStorage *)
						pLoadManager->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxTextStorage, IID_BlackBoxTextStorage);

					IAnalyseParams* pAnalyseParams = (IAnalyseParams*)
						pLoadManager->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams);
					pAnalyseParams->SetCurrentSystemMode(SS::Core::Types::ecsmGlobal);
					pAnalyseParams->SetAnalyseMod(SS::Core::Types::eamIndexation);
					pAnalyseParams->GetAnalyseDepthParams()->SetIsMorpho(true);
					pAnalyseParams->GetAnalyseDepthParams()->SetIsSemantic(true);
					pAnalyseParams->GetAnalyseDepthParams()->SetIsSyntax(true);
					pAnalyseParams->GetAnalyseDepthParams()->SetIsSynonims(false);

					// morphology
					IDictionary* pDict = (IDictionary*) 
						pLoadManager->GetInterface( DICTIONARIES_DLL_PATH, CLSID_IDictionary, IID_IDictionary);
					m_pMorphoAnalyzer = pDict->GetMorphologyAnalyser();
					m_pMorphoAnalyzer->SetAnalyseParams(pAnalyseParams);

					m_pAMConverter = 
						(SS::Interface::Core::AMConverter::IAMConverterMorpho*)
						pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);

					m_pAMConverterSyntaxAposteriori = 
						(SS::Interface::Core::AMConverter::IAMConverterSyntaxAposteriori*)
						pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterSyntax, IID_AMConverterSyntaxAposteriori);

					m_pAMConverterSemantic = 
						(SS::Interface::Core::AMConverter::IAMConverterSemantic*)
						pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterSemantic, IID_AMConverterSemantic);

					m_pSyntaxAnalyser = 
						(SS::Interface::Core::ISyntaxAnalyzer*)
						GetLoadManager()->GetInterface(L"./core/linguistics/SyntaxAnalyzer.dll", CLSID_SyntaxAnalyzer, IID_SyntaxAnalyzer);

					m_pIWorkRegister = 
						(SS::Interface::Core::CommonServices::IWorkRegister*)
						pLoadManager->GetInterface(L"./core/LoadManager.dll", CLSID_WorkRegister, IID_WorkRegister);

					if( m_pIWorkRegister == 0 )
						SS_THROW(L"Ошибка при получении WorkRegister");
							
					unitOperator = new UnitPropertiesOperator(m_pAMConverterSemantic, m_pAMConverter,m_pMorphoAnalyzer);
					Init();
				}


			}
			SS_CATCH(L"")
		}

		bool CBaseDigestAnalyzer::IsAbbr(IUnit* pUnit)
		{
			IIndex* pIndex;
			IDictionaryIndex* pDictionaryIndex;
			EIndexType oIndexType;
			wstring str = pUnit->GetWord();
			if (str.size()<2) return false;

			for(pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
			{
				pDictionaryIndex = pIndex->GetDictionaryIndex();

				if(pDictionaryIndex)
				{
					SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex = pDictionaryIndex->GetFirst();
					oIndexType = oDictionaryIndex.GetIndexType();
					if (oIndexType==eitSemanticIndex)
					{		
						int index = oDictionaryIndex.GetDictionaryIndex();
						SS::Core::Features::CSemanticFeature oSemanticFeature;							
						m_pAMConverterSemantic->EnCode(index, &oSemanticFeature);					
						if (oSemanticFeature.m_SemanticType.Equal(SS::Core::Features::Values::TSemanticType.smtService) &&
							oSemanticFeature.m_SServiceTypes.Equal(SS::Core::Features::Values::TSServiceTypes.sstAcronymCitation))
						{
							return true;
						}						
					}
				}
			}
			return false;
		}

		void  CBaseDigestAnalyzer::GetLinearUnitsFull(ISentence* pSen, TUnitList& allUnits)
		{
			IUnit* pUnit = pSen->GetFirstUnit();	
			IUnit* pParentUnit = pUnit->GetParentUnit();
			while(pUnit)
			{ 
				pParentUnit = pUnit->GetParentUnit();
				if (pParentUnit==NULL || !unitOperator->IsSemantic(pParentUnit, SS::Core::Features::Values::TSemanticType.smtPerson))
					allUnits.push_back(pUnit);
				wstring str = pUnit->GetWord();	
				int off = pUnit->GetFirstCharPositionNew();
				if (!unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson) ||
					(pUnit->GetIsTargetObjectUnit()==0 && pUnit->GetIsTargetObjectUnitFromChilds()==0))
					AddChildUnits(pUnit, allUnits);
				pUnit = pUnit->GetRightUnit();							
			}
			allUnits.sort(SortByPosInSentFull);
		}

		void CBaseDigestAnalyzer::AddChildUnits(IUnit* unit, TUnitList& findedUnits)
		{
			IUnit* pChildUnit = unit->GetLeftChildUnit();
			int nChildUnit = 0;
			while(pChildUnit)
			{			
				if (!unitOperator->IsSemantic(pChildUnit, SS::Core::Features::Values::TSemanticType.smtPerson) ||
					(pChildUnit->GetIsTargetObjectUnit()==0 && pChildUnit->GetIsTargetObjectUnitFromChilds()==0))
					AddChildUnits(pChildUnit, findedUnits);			
				findedUnits.push_back(pChildUnit);			
				wstring str =pChildUnit->GetWord();	
				int off = pChildUnit->GetFirstCharPositionNew();
				pChildUnit = pChildUnit->GetRightUnit();
			}	
			wstring str = unit->GetWord();
		}


		void CBaseDigestAnalyzer::FindUnits(
			ISentence* pSen,
			SS::Core::Features::CMorphoFeature oMorfoAnfr,			
			SS::Core::Features::Types::SyntaxCategories syntType,
			SS::Core::Features::Types::TSemanticType semType,			
			int ParentType, int CitataType, 
			IUnit* edgeUnit,
			TUnitList& noneUnits /* те юниты которые не рассматриваем как разрешения априори */, 
			TUnitList& allUnits)
		{
			if (pSen==NULL) return;
			TUnitList resUnits;

			GetLinearUnitsFull(pSen, resUnits);

			for(TUnitList::iterator it = resUnits.begin(); it!=resUnits.end(); ++it)
		 {
			 IUnit* pUnit = *it;
			 IUnit* parentUnit = pUnit->GetParentUnit();			 

			 if (pUnit==edgeUnit)
				 break; // нашли до крайнего - дальше не ищем	
			 bool isNoneUnit = false;
			 for(TUnitList::iterator itNonUnits =  noneUnits.begin(); itNonUnits != noneUnits.end(); ++itNonUnits)
			 {
				 if ((*itNonUnits)==pUnit) { isNoneUnit = true; break; }
			 }
			 if (isNoneUnit) continue;

			 wstring str = pUnit->GetWord();

			 if (unitOperator->IsSyntax(pUnit, syntType) &&
				 unitOperator->IsSemantic(pUnit, semType) &&
				 unitOperator->IsMorpho(pUnit,  oMorfoAnfr))
			 {
				 bool isParent = false;				
				 SS::Core::Features::CMorphoFeature oMorphoFeatureVerb;
				 SS::Core::Features::CMorphoFeature oMorphoFeatureOfficial;
				 oMorphoFeatureVerb.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
				 oMorphoFeatureOfficial.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;

				 if (ParentType==0)
				 {
					 if (parentUnit!=NULL)
					 {

						 if (unitOperator->IsMorpho(parentUnit, oMorphoFeatureVerb) ||
							 unitOperator->IsMorpho(parentUnit, oMorphoFeatureOfficial))
						 {
							 isParent = true;
						 }
					 }else isParent = true;
				 }else
					 if (ParentType==1)
					 {					
						 if (parentUnit!=NULL)
						 {
							 if (!unitOperator->IsMorpho(parentUnit, oMorphoFeatureVerb) &&
								 !unitOperator->IsMorpho(parentUnit, oMorphoFeatureOfficial)													 
								 )
							 {
								 isParent = true;
							 }
						 }else isParent = false;
					 }else
						 isParent = true;

					 if (isParent){
						 // тперь на цитату надо проверить
						 bool isCitata = false;
						 if (CitataType==2) // все равно где юнит - в цитате или вне ее
						 {
							 isCitata = true; 
						 }
						 else
							 if (CitataType==0 && pUnit->GetCitation()==NULL) // юнит должен быть вне цитаты
							 {
								 isCitata = true;
							 }
							 else
								 if (CitataType==1 && pUnit->GetCitation()!=NULL)
								 {
									 isCitata = true;
								 }

								 if (isCitata)
									 allUnits.push_back(pUnit);
					 }
			 }
		 }
		}

		void CBaseDigestAnalyzer::ReinitToneDicts(wstring& type_name)
		{
			CToneDictPool *tdp = new CToneDictPool();
			m_ToneDictMap[type_name] = tdp;
			tdp->inversUnits = m_pBlackBoxTextStorage->CreateObjectDescr();
			tdp->inversUnits->SetType(L"tone_inversion.txt");
			tdp->nounsgenerativeUnits = m_pBlackBoxTextStorage->CreateObjectDescr();
			tdp->nounsgenerativeUnits->SetType(L"tone_nounsgenerative.txt");
			tdp->nounsUnits = m_pBlackBoxTextStorage->CreateObjectDescr();
			tdp->nounsUnits->SetType(L"tone_nouns.txt");
			tdp->adjectivesUnits = m_pBlackBoxTextStorage->CreateObjectDescr();
			tdp->adjectivesUnits->SetType(L"tone_adjectives.txt");
			tdp->verbsUnits = m_pBlackBoxTextStorage->CreateObjectDescr();
			tdp->verbsUnits->SetType(L"tone_verbs.txt");
			tdp->objectnounsUnits = m_pBlackBoxTextStorage->CreateObjectDescr();
			tdp->objectnounsUnits->SetType(L"tone_objectnouns.txt");
			tdp->stopwordsUnits = m_pBlackBoxTextStorage->CreateObjectDescr();	
			tdp->stopwordsUnits->SetType(L"tone_stopwords.txt");
		}

		void CBaseDigestAnalyzer::AddWordToDict(wstring& type_name, int dict_code, string& word)
		{		 
			CToneDictPool *tdp = m_ToneDictMap[type_name];
			string text;
			set<char> mBreak; mBreak.insert( ' ' ); mBreak.insert( '\t' );
			vector<string> vsString;
			UseString::SplitStr( (char*)word.c_str(), mBreak, vsString );
			if (!vsString.size())
			{
				return;
			}

			string& sTone = vsString.back();
			vsString.pop_back();
			int nTone = atoi( sTone.c_str() );

			for (std::vector<string>::iterator iS = vsString.begin(); iS != vsString.end(); iS++)
			{			    
				if( !text.empty() ) text += " ";
				text += *iS;
			}			  	
			if (dict_code==0) // словарь инверсии
		        tdp->inversUnits->AddSynonim((wchar_t*)CA2W(text.c_str()), nTone);
			else if (dict_code==1) 
    			tdp->adjectivesUnits->AddSynonim((wchar_t*)CA2W(text.c_str()), nTone);
			else if (dict_code==2) 
				tdp->nounsUnits->AddSynonim((wchar_t*)CA2W(text.c_str()), nTone);
            else if (dict_code==3) 
				tdp->nounsgenerativeUnits->AddSynonim((wchar_t*)CA2W(text.c_str()), nTone);
			else if (dict_code==4) 
                tdp->objectnounsUnits->AddSynonim((wchar_t*)CA2W(text.c_str()), nTone);
			else if (dict_code==5) 
			    tdp->verbsUnits->AddSynonim((wchar_t*)CA2W(text.c_str()), nTone);
			else if (dict_code==6)
                tdp->stopwordsUnits->AddSynonim((wchar_t*)CA2W(text.c_str()), nTone);
		}

		void CBaseDigestAnalyzer::GenerateDictLing(wstring& type_name)
		{
			CToneDictPool *tdp =  m_ToneDictMap[type_name];
			tdp->inversUnits->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, false);
			tdp->adjectivesUnits->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, false);
			tdp->nounsgenerativeUnits->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, false);
			tdp->nounsUnits->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, false);
			tdp->objectnounsUnits->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, false);
			tdp->verbsUnits->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, false);
			tdp->stopwordsUnits->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, false);
		}

		void CBaseDigestAnalyzer::LoadToneDicts()
		{
            if (m_ToneDictLoaded)
			{
                return;
			}

			wstring base_path = L"ToneDicts\\";
			wstring search_pattern = m_pIWorkRegister->GetParameter(L"PathUserResources");
			search_pattern += L"ToneDicts\\*";

			/*int nRet = WideCharToMultiByte(1251, 0, search_pattern.c_str(),  -1,
				NULL, 0, "#", NULL);

			char* str = new char[nRet+2];

			WideCharToMultiByte(1251, 0, search_pattern.c_str(),  -1,
				str, nRet+2, "#", NULL);*/

			WIN32_FIND_DATAW FindFileData;
			HANDLE hFind = FindFirstFileW(search_pattern.c_str(), &FindFileData);

			if (hFind != INVALID_HANDLE_VALUE)
			{		
				BOOL res = FindNextFileW(hFind, &FindFileData);
				while (res != 0)
				{
					wstring objtype_folder = FindFileData.cFileName;
					if (objtype_folder != L"..")
					{
						// теперь по полученному пути надо загрузить все файлы
						// и сформировать пул словарей для данного типа объектов
						CToneDictPool *tdp = new CToneDictPool();
						wstring dict_file = base_path +  objtype_folder + L"\\tone_inversion.txt";
						LoadDict(dict_file, &(tdp->inversUnits), true);		
						dict_file = base_path +  objtype_folder  + L"\\tone_inversion.txt";
						LoadDict(dict_file, &(tdp->inversUnits), true);		
						dict_file = base_path +  objtype_folder  + L"\\tone_adjectives.txt";
						LoadDict(dict_file, &(tdp->adjectivesUnits), true);
						dict_file = base_path +  objtype_folder  + L"\\tone_nouns.txt";
						LoadDict(dict_file, &(tdp->nounsUnits), true);
						dict_file = base_path +  objtype_folder  + L"\\tone_objectnouns.txt";
						LoadDict(dict_file, &(tdp->objectnounsUnits), true);
						dict_file = base_path +  objtype_folder  + L"\\tone_verbs.txt";
						LoadDict(dict_file, &(tdp->verbsUnits), true);
						dict_file = base_path +  objtype_folder  + L"\\tone_nounsgenerative.txt";
						LoadDict(dict_file, &(tdp->nounsgenerativeUnits), true);								
						dict_file = base_path +  objtype_folder  + L"\\tone_stop_words.txt";
						LoadDict(dict_file, &(tdp->stopwordsUnits), false);
                        TToneDictMap::iterator it = m_ToneDictMap.find(objtype_folder);
                        if ((it != m_ToneDictMap.end()) && ((*it).second == 0))
                            (*it).second = tdp;
                        else
                            m_ToneDictMap.insert(TToneDictMap::value_type(objtype_folder, tdp));
                        /*if(it != m_ToneDictMap.end())
                            (*it).second = tdp;*/

						// тперь надо считаь конкретные данные из фалов
					}
					res = FindNextFileW(hFind, &FindFileData);
				}
			}
            m_ToneDictLoaded = true;
		}

		void CBaseDigestAnalyzer::LoadDict(wstring dict_file, IObjectDescr** objDescr, bool isTone)
		{
			wstring wDataBasePath = m_pIWorkRegister->GetParameter(L"PathUserResources") ;
			wstring wDict = wDataBasePath + dict_file;
			char buf[300];			
			UINT32 iAccessMode, iShareMode;
			iAccessMode = GENERIC_READ;
			iShareMode = FILE_SHARE_READ;

			int nRet = WideCharToMultiByte(1251, 0, wDict.c_str(),  -1, NULL, 0, "#", NULL);
			char* str = new char[nRet + 2];

			WideCharToMultiByte(1251, 0, wDict.c_str(),  -1, str, nRet+2, "#", NULL);	

			ifstream ifs(str);		
			int status = 0;	
			int count = 0;
			(*objDescr) = m_pBlackBoxTextStorage->CreateObjectDescr();		
			(*objDescr)->SetType((wchar_t*)dict_file.c_str());			
			while (!ifs.eof())
			{			
				ifs.getline(buf,sizeof(buf));
				if (strlen(buf) < 2) break;
				if (buf[0] == '/') continue;	

				int nTone = 0;
				if (isTone)
				{
					string text;
					set<char> mBreak; mBreak.insert( ' ' ); mBreak.insert( '\t' );
					vector<string> vsString;
					UseString::SplitStr( buf, mBreak, vsString );
					if( !vsString.size() )
						continue;

					string sTone = vsString.back();
					vsString.pop_back();
					nTone = atoi( sTone.c_str() );

					for (std::vector<string>::iterator iS = vsString.begin(); iS != vsString.end(); iS++)
					{			    
						if( !text.empty() ) text += " ";
						text += *iS;
					}			  
					(*objDescr)->AddSynonim((wchar_t*)CA2W(text.c_str()), nTone);			
				}
				else
				{
					(*objDescr)->AddSynonim((wchar_t*)CA2W(buf));			
				}
			}
			(*objDescr)->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, false);
			delete[] str;
		}

		void CBaseDigestAnalyzer::Init()
		{	
            wstring basePath = m_pIWorkRegister->GetParameter(L"PathUserResources");
			CDictionaryPool::Load(basePath, unitOperator);
			//pDictionaryPool = new CDictionaryPool(m_pIWorkRegister->GetParameter(L"PathDataBase"), unitOperator);
			//LoadSimpleDictionary(L"DigestDicts\\deleted_org_prefixes.dct",&m_pDeletedOrgPrefixes); - 

			//после отпуска доделаю. Необходимо в поиске по древовидному словарю сделать проверку не тока на суффикс но и на все слово.

			//LoadToneDicts();
			LoadDict(L"DigestDicts\\citata_verbs.dct", &citataVerbs, false);
			LoadDict(L"DigestDicts\\stop_genetive_list.dct", &stopListForGen, false);
			//LoadDict(L"DigestDicts\\org_prefixes.dct", &stopListForGen, false);

			citataSecondTerm = 
				m_pBlackBoxTextStorage->CreateObjectDescr();
			citataFirstTerm = 
				m_pBlackBoxTextStorage->CreateObjectDescr();
			citataLastTermOne = 
				m_pBlackBoxTextStorage->CreateObjectDescr();
			citataQuoteTerm = 
				m_pBlackBoxTextStorage->CreateObjectDescr();
			citataLastTermTwo = 
				m_pBlackBoxTextStorage->CreateObjectDescr();
			citataLastTermThree = 
				m_pBlackBoxTextStorage->CreateObjectDescr();
			citataFirstTermFour1 = 
				m_pBlackBoxTextStorage->CreateObjectDescr();
			citataFirstTermFour2 = 
				m_pBlackBoxTextStorage->CreateObjectDescr();

			citataFirstTerm->AddSynonim(L": \"");
			citataFirstTerm->AddSynonim(L": «");
			citataFirstTerm->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, true);

			citataLastTermOne->AddSynonim(L"\" , -");
			citataLastTermOne->AddSynonim(L"» , -");
			citataLastTermOne->AddSynonim(L", -");
			wstring s = L"\" , ";		
			s.push_back((wchar_t)8211);
			citataLastTermOne->AddSynonim(s);
			s = L", ";		
			s.push_back((wchar_t)8211);
			citataLastTermOne->AddSynonim(s);
			s = L"» , ";		
			s.push_back((wchar_t)8211);
			citataLastTermOne->AddSynonim(s);		
			citataLastTermOne->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, true);

			citataLastTermTwo->AddSynonim(L"\" .");
			citataLastTermTwo->AddSynonim(L"\" !");
			citataLastTermTwo->AddSynonim(L"\" ?");	
			citataLastTermTwo->AddSynonim(L"» .");
			citataLastTermTwo->AddSynonim(L"» !");
			citataLastTermTwo->AddSynonim(L"» ?");		
			citataLastTermTwo->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, true);

			citataLastTermThree->AddSynonim(L"\" .");
			citataLastTermThree->AddSynonim(L"\" !");
			citataLastTermThree->AddSynonim(L"\" ?");		
			citataLastTermThree->AddSynonim(L"\" ,");		
			citataLastTermThree->AddSynonim(L"\" ;");
			citataLastTermThree->AddSynonim(L"» .");
			citataLastTermThree->AddSynonim(L"» !");
			citataLastTermThree->AddSynonim(L"» ?");		
			citataLastTermThree->AddSynonim(L"» ,");		
			citataLastTermThree->AddSynonim(L"» ;");		
			citataLastTermThree->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, true);

			citataQuoteTerm->AddSynonim(L"\"");	    
			citataQuoteTerm->AddSynonim(L"«");
			citataQuoteTerm->AddSynonim(L"»");
			citataQuoteTerm->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, true);

			citataFirstTermFour1->AddSynonim(L"по его словам");
			citataFirstTermFour1->AddSynonim(L"по его мнению");
			citataFirstTermFour1->AddSynonim(L"по его предположению");		
			citataFirstTermFour1->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, true);
			citataFirstTermFour2->AddSynonim(L"по словам");
			citataFirstTermFour2->AddSynonim(L"по мнению");
			citataFirstTermFour2->AddSynonim(L"по предположению");		
			citataFirstTermFour2->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, true);

			pronounAnfrThird =  m_pBlackBoxTextStorage->CreateObjectDescr();
			pronounAnfrThird->AddSynonim(L"который");
			pronounAnfrThird->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, true);

			toneBlockDelimiters = m_pBlackBoxTextStorage->CreateObjectDescr();
			toneBlockDelimiters->AddSynonim(L", а");
			toneBlockDelimiters->AddSynonim(L", и");
			toneBlockDelimiters->AddSynonim(L", но");
			toneBlockDelimiters->AddSynonim(L", что");
			toneBlockDelimiters->AddSynonim(L", где");
			toneBlockDelimiters->AddSynonim(L", когда");
			toneBlockDelimiters->AddSynonim(L", откуда");
			toneBlockDelimiters->AddSynonim(L", когда");
			toneBlockDelimiters->AddSynonim(L", пока");
			toneBlockDelimiters->AddSynonim(L", покамест");
			toneBlockDelimiters->AddSynonim(L", сколько");
			toneBlockDelimiters->AddSynonim(L", то");
			toneBlockDelimiters->AddSynonim(L", поскольку");
			toneBlockDelimiters->AddSynonim(L", насколько");
			toneBlockDelimiters->AddSynonim(L", который");
			toneBlockDelimiters->AddSynonim(L":");
			toneBlockDelimiters->AddSynonim(L", -");
			s = L", ";		
			s.push_back((wchar_t)8211);
			toneBlockDelimiters->AddSynonim(s);

			toneBlockDelimiters->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, true);

			specialVerbs = m_pBlackBoxTextStorage->CreateObjectDescr();
			specialVerbs->AddSynonim(L"критиковать");
			specialVerbs->AddSynonim(L"ругать");
			specialVerbs->AddSynonim(L"обличать");
			specialVerbs->AddSynonim(L"изобличать");
			specialVerbs->AddSynonim(L"подвергнуть");
			specialVerbs->AddSynonim(L"отметить");
			specialVerbs->MorphoTranslate(this->m_pMorphoAnalyzer, m_pAMConverter, true);
		}

		void CBaseDigestAnalyzer::GetEntitiesXMLRecursive(IUnit* pUnit, list<IUnit*>& entityLst)
		{
			IUnit* pChildUnit = pUnit->GetLeftChildUnit();

			while (pChildUnit)
			{
				if (isEntity(pChildUnit))
				{
					entityLst.push_back(pChildUnit);
				}
				//else
				//{
				GetEntitiesXMLRecursive(pChildUnit, entityLst);
				//}

				pChildUnit = pChildUnit->GetRightUnit();
			}
		}

		IUnit* CBaseDigestAnalyzer::GetRootVerb(IUnit* pUnit)
		{
			IUnit* lastVerb = NULL;
			IUnit* curUnit = pUnit->GetParentUnit();
			while (curUnit)
			{
				if (unitOperator->IsSyntax(curUnit, SS::Core::Features::Values::SyntaxCategories.scMain))
				{
					return curUnit;
				}
				curUnit = curUnit->GetParentUnit();			 
			}
			return NULL;
		}

		// создает сущность по типу и списку юнитов
        CEntity* CBaseDigestAnalyzer::CreateNewEntity(
			EEntityType entType, eEntityBaseType nType, IUnit* pRootUnit, ISentence* pSen, TUnitList& lstUnits)		 
		{
			unitOperator->SortUnits(lstUnits);
			CEntity* pEnt = new CEntity(unitOperator, pSen->GetLanguage());
			pEnt->SetDate(L"");
			pEnt->rootUnit = pRootUnit;
			pEnt->nType = nType;
			pEnt->entType = entType;
			unitOperator->AddUnitsToList(pEnt->unitList, lstUnits);
			pEnt->begin = (*pEnt->unitList.begin())->GetPosition();
			pEnt->end = (*pEnt->unitList.rbegin())->GetPosition();
			pEnt->m_pSent = pSen;
            pEnt->GetNormalString();
			return pEnt;
		}
		
		void CBaseDigestAnalyzer::GenerateSubEntWithoutLeftAdjectives(CEntity* pEntity, TEntityList& inside, CEntity* pParentEntity)
		{
			bool isRootFind = false;
			TUnitList lstUnitsForSubentity;
			// генерируем подсущность с отсечением прилагательных
			if (pEntity->m_strPrefix.empty())
			{
                return;
			}

            TUnitList adjectives;
            
            IUnit *pFirst = 0;
            IUnit *pLast = 0;
            IUnit *pRoot = pEntity->rootUnit;
            if ((pEntity->m_lang == SS::Core::Types::EAnalyseLanguage::ealEnglish) && (inside.size() > 0))
            {
                CEntity* pInside = *inside.rbegin();
                if ((pInside->nounUnit == pEntity->nounUnit) && !pInside->m_strPrefix.empty())
                /// подсущность у правого края
                {
                    pFirst = pInside->unitList.front();
                    pRoot = pInside->rootUnit;
                }
                else if(pInside->rootUnit == pEntity->rootUnit)
                /// подсущность у левого края
                {
                    pLast = pInside->nounUnit;
                    pRoot = pEntity->nounUnit;
                }
            }
            
            for (TUnitList::iterator it = pEntity->unitList.begin();
                 it != pEntity->unitList.end();
                 ++it)
            {
				int size = lstUnitsForSubentity.size();
                IUnit *pUnit = *it;
                if (pFirst)
                {
                    if (pUnit != pFirst)
					{
                        lstUnitsForSubentity.push_back(pUnit);
					}
                    else
					{
                        pFirst = 0;
					}
                }
                else if (pLast)
                {
                    lstUnitsForSubentity.push_back(pUnit);
                    if (pUnit == pLast)
					{
                        pLast = 0;
					}
                }
                
                if (!(pFirst || pLast))
                {
                    if (pUnit == pRoot)
					{
				        isRootFind = true;
					}

			        if (isRootFind)
					{
				        lstUnitsForSubentity.push_back(pUnit);
					}
                }
				if (size == lstUnitsForSubentity.size())
				{
					adjectives.push_back(pUnit);
				}
            }
			IUnit *pLastUnit = NULL;
			for (TUnitList::reverse_iterator it = adjectives.rbegin(); it != adjectives.rend(); ++it)
			{
				if (!pLastUnit || !IsPartOfSpeech(pLastUnit, SS::Core::Features::Values::PartOfSpeechTypes.postOfficial))
				{
					CEntity *pNewEntity = CreateNewEntity(entUnknown, eEntityBaseType::Other, pEntity->rootUnit, pEntity->m_pSent, lstUnitsForSubentity);
					EntityNormalization(pNewEntity, inside);
					pParentEntity->AddSubEntity(pNewEntity);
				}
				pLastUnit = *it;
				unitOperator->GetLinearUnitsWithSort(pLastUnit, lstUnitsForSubentity);
			}
		}

		void CBaseDigestAnalyzer::GenerateSubentitiesWithoutRightUnits(CEntity* pEntity, TEntityList& lstAllEntityInSent)
		{
            TEntityList lstInsideEntities;
			GetInsideEntities(pEntity, lstAllEntityInSent, lstInsideEntities);
			GenerateSubEntWithoutLeftAdjectives(pEntity, lstInsideEntities, pEntity);
			
            // генерим подсущности с правым отсечением
			// для этого получаем список подцпепочек, у которых правый юнит совпадает с 
            // правым юнитом исходной цепочки, и генерим подсущности, отсекая от исходной все юниты подсущности.
			// к полученной сущности дополнительно применяем функцию генерации подсущности без прилагательного
			for (TEntityList::iterator itEntity = lstInsideEntities.begin(); itEntity != lstInsideEntities.end(); ++itEntity)
			{
				CEntity *pChildEntity = *itEntity;
				if (pChildEntity->GetLastUnit() == pEntity->GetLastUnit())
				{
					TUnitList lstUnitsForSubentity;
					TUnitList insideList;
					for (TUnitList::iterator itUnit = pEntity->unitList.begin(); itUnit != pEntity->unitList.end(); ++itUnit)
					{
						IUnit *pUnit = *itUnit;
						if (!pChildEntity->IsUnitInEntity(pUnit))
						{
							if (IsHavePartOfSpeech(pUnit, SS::Core::Features::Values::PartOfSpeechTypes.postNoun))
							{
								unitOperator->AddUnitsToList(lstUnitsForSubentity, insideList);
								lstUnitsForSubentity.push_back(pUnit);
								insideList.clear();
							}
							else
							{
								insideList.push_back(pUnit);
							}
						}
					}
					if (!lstUnitsForSubentity.empty())
					{
                        wstring str = (*lstUnitsForSubentity.rbegin())->GetWord();
                        if ((str == L"и") || (str == L"или") || (str == L","))
						{
                            lstUnitsForSubentity.pop_back();
						}

                        CEntity *pNewEntity = CreateNewEntity(entUnknown, eEntityBaseType::Other, pEntity->rootUnit, pEntity->m_pSent, lstUnitsForSubentity);
						pNewEntity->DeleteUncorrectUnitsFromBorders();
                        EntityNormalization(pNewEntity, pEntity->m_lstSubEntites);
						pEntity->AddSubEntity(pNewEntity);
                        TEntityList inside;
			            GetInsideEntities(pNewEntity, pEntity->m_lstSubEntites, inside);
						GenerateSubEntWithoutLeftAdjectives(pNewEntity, inside, pEntity);
					}
				}				
			}
		}

        void CBaseDigestAnalyzer::NumberingSentenceUnits(TUnitList& sentUnits)
        {
            int index = 0;
			for (TUnitList::iterator it = sentUnits.begin(); it != sentUnits.end(); ++it)
			{
				++index;
				(*it)->SetNumber(index);
			}
        }

		void CBaseDigestAnalyzer::SaveDefisWordMorpho(const TUnitList& units)
		{
			for (TUnitList::const_iterator it = units.begin(); it != units.end(); ++it)
			{
				IUnit *pUnit = *it;
				wstring word = pUnit->GetWord();
				bool isEnglishWord = unitOperator->IsEnglishWord(pUnit);
				if (word.empty())
					continue;
				vector<wstring> words = unitOperator->GetChangeablePartsByDefis(pUnit);
				if (words.empty())
					continue;

				SStorageUnitInfo info;
				if (words.size() == 1)
				{
					unitOperator->GetUnitMorfoFeaturesFullByMorphoAnalyzer(words[0], info._morpho);
					if (unitOperator->IsEnglishWord(words[0]) &&
						info._morpho.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postNoun))
					{
						info._morpho.Undefine();
						info._morpho.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;
					}
				}
				else
				/// два слова
				{
					if (isEnglishWord)
						unitOperator->GetUnitMorfoFeaturesFullByMorphoAnalyzer(words[1], info._morpho);
					else
					{
						SMIList result;
						bool isAnimation = false;
						unitOperator->GetIntersectMorpho(words[0], words[1], result, 0, 0, &isAnimation);

						info._morpho.Undefine();
						info._morpho.m_PartOfSpeechTypes.Assign(SS::Core::Features::Values::PartOfSpeechTypes.postNoun);

						SS::Core::Features::Types::AnimInAnimType animType;
						SMIList::iterator it = result.begin();
						while (it != result.end())
						/// определяем одушевленность и отсекаем лишнее
						{
							bool isClear = false;
							SMorphoInfo *pInfo = &(*it);
							if (pInfo->animationType.Equal(SS::Core::Features::Values::AnimInAnimType.aiaAnimation))
							/// одушевленная форма
							{
								if (!animType.Equal(SS::Core::Features::Values::AnimInAnimType.aiaAnimation))
								/// первая одушевленная форма
								{
									isClear = true;
									animType = pInfo->animationType;
								}
							}
							else if (animType.Equal(SS::Core::Features::Values::AnimInAnimType.aiaAnimation))
							{
								it = result.erase(it);
								continue;
							}
							else if (pInfo->animationType.Equal(SS::Core::Features::Values::AnimInAnimType.aiaInanimation))
							/// неодушевленная форма
							{
								if (animType.IsUndefined())
								{
									isClear = true;
									animType = pInfo->animationType;
								}
							}
							if (isClear && (it != result.begin()))
							{
								--it;
								it = result.erase(result.begin(), it);
							}
							++it;
						}
						if (isAnimation)
							info._morpho.m_AnimInAnimType.Assign(SS::Core::Features::Values::AnimInAnimType.aiaAnimation);
						else
							info._morpho.m_AnimInAnimType.Assign(animType);
						list<SS::Core::Features::Types::CaseType> caseList;
						for (it = result.begin(); it != result.end(); ++it)
						/// задаем род и число
						{
							SMorphoInfo *pInfo = &(*it);
							/// приоритет мужскому роду
							if (!info._morpho.m_GenderType.Equal(SS::Core::Features::Values::GenderType.gtMasculine))
							{
								bool isUpdate = false;
								if (pInfo->genType.Equal(SS::Core::Features::Values::GenderType.gtMasculine))
								{
									isUpdate = true;
									info._morpho.m_GenderType.Assign(pInfo->genType);
								}
								else if (!info._morpho.m_GenderType.Equal(SS::Core::Features::Values::GenderType.gtFemale))
								{
									if (!pInfo->genType.IsUndefined() && !info._morpho.m_GenderType.Equal(pInfo->genType))
									{
										isUpdate = true;
										info._morpho.m_GenderType.Assign(pInfo->genType);
									}
								}
								if (isUpdate)
								{
									info._morpho.m_CaseType.Undefine();
									info._morpho.m_NumberType.Undefine();
									caseList.clear();
								}
							}
							if (info._morpho.m_GenderType.Equal(pInfo->genType))
							/// установка прочих морфологических характеристик по части речи и роду
							/// условие одушевленности уже учтено, т.к. лишнее уже удалили из коллекции
							{
								if (!info._morpho.m_CaseType.IsIncludes(&pInfo->caseType))
									caseList.push_back(pInfo->caseType);
								{
									if (info._morpho.m_CaseType.IsUndefined())
										info._morpho.m_CaseType.Assign(pInfo->caseType);
									else
										info._morpho.m_CaseType.AssignOR(&pInfo->caseType);
								}
							
								if (!info._morpho.m_NumberType.Equal(SS::Core::Features::Values::NumberType.ntSingle))
								/// приоритет единственному числу
								{
									if (pInfo->numberType.Equal(SS::Core::Features::Values::NumberType.ntSingle))
										info._morpho.m_NumberType.Assign(pInfo->numberType);
									else if (!pInfo->numberType.IsUndefined() && !info._morpho.m_NumberType.Equal(pInfo->numberType))
										info._morpho.m_NumberType.Assign(pInfo->numberType);
								}
							}
						}
						if (caseList.size() > 1)
						/// больше одного падежа - сформируем список морфосвойств
						{
							for(list<SS::Core::Features::Types::CaseType>::iterator it = caseList.begin();
								it != caseList.end();
								++it)
							{
								SMorphoInfo sInfo = unitOperator->CreateSMorphoInfo(info._morpho);
								sInfo.caseType.Assign(*it);
								info._listMI.push_back(sInfo);
							}
						}
					}
				}
				unitOperator->SetMorphoInformation(pUnit, info);
			}
		}

        wstring CBaseDigestAnalyzer::GetEntitiesXML(const wstring& srcText, IText* pText)
        {
            SS_TRY;

			TEntityList allEntity;
            GetSimpleEntities(pText, false, allEntity);

			for (TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); ++it)
			{
				CEntity *pEnt = *it;
				if (pEnt->entType == entDate)
				{
					pEnt->isUsed = false;
				}
			}

            // теперь надо сформировать XML
			return GetFactXML(srcText, pText, true, false, allEntity);
			SS_CATCH(L"");
        }

		void CBaseDigestAnalyzer::SetNounForUndefined(TUnitList& units)
		{
			for (TUnitList::iterator it = units.begin(); it != units.end(); ++it)
			{
				IUnit *pUnit = *it;
				SS::Core::Features::CMorphoFeature morpho;
				unitOperator->GetUnitMorfoFeaturesFull(pUnit, morpho);
				if (morpho.m_PartOfSpeechTypes.IsUndefined())
				{
					morpho.m_PartOfSpeechTypes.Assign(SS::Core::Features::Values::PartOfSpeechTypes.postNoun);
					SStorageUnitInfo info;
					info._morpho = morpho;
					unitOperator->SetMorphoInformation(pUnit, info);
				}
			}
		}

		void CBaseDigestAnalyzer::GetSimpleEntities(IText* pText, bool isSelectOnlyCeterus, TEntityList& allEntity)
		{
            TEntityList allEntityInSent;

            SS::Core::Features::Types::TSPersonType persType;
            persType = SS::Core::Features::Values::TSPersonType.semprstFirstName;
            persType.AssignOR(&SS::Core::Features::Values::TSPersonType.semprstLastName);
            persType.AssignOR(&SS::Core::Features::Values::TSPersonType.semprstMiddleName);
            persType.AssignOR(&SS::Core::Features::Values::TSPersonType.semprstName);

			m_lSentences.clear();
			unitOperator->ClearMorphoInformation();

            for (IParagraph* pPar = pText->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
			{						
				for (ISentence* pSen = pPar->GetFirstSentence(); pSen; pSen = pSen->GetNextSentence())
				{
					SentenceInfo info;
					info.pSentence = pSen;
                    m_lSentences.push_back(info);
                    SS::Core::Types::EAnalyseLanguage language = pSen->GetLanguage();
                    pSen->SetPreDigest(1);
                    allEntityInSent.clear();

                    TUnitList sentUnits;
			        pSen->GetLinearUnitsFull(sentUnits);
                    NumberingSentenceUnits(sentUnits);
					SelectSimpleDates(pSen, allEntityInSent, sentUnits);
					SaveDefisWordMorpho(sentUnits);
					SetNounForUndefined(sentUnits);
					if (!isSelectOnlyCeterus)
					{
						switch (language)
						{
							case SS::Core::Types::EAnalyseLanguage::ealEnglish:
								GetEntitiesXMLFromEngSentence(pSen, allEntityInSent, sentUnits);
								break;
							case SS::Core::Types::EAnalyseLanguage::ealRussian:
								GetEntitiesXMLFromRusSentence(pSen, allEntityInSent, sentUnits);
								break;
						}
						EvristicDeleteShortWordForm(allEntityInSent);
					}

                    EvristicSelectCeterus(pSen, allEntityInSent, sentUnits);
                    allEntityInSent.sort(SortByPosInSentForEntity);
                    EntityNormalization(allEntityInSent);

                    for (TEntityList::iterator it = allEntityInSent.begin(); it != allEntityInSent.end(); ++it)
                    {
                        CEntity *pEnt = *it;
                        if (pEnt->IsGoodEnt() && pEnt->IsHaveNecessaryNoun() && !pEnt->IsUnknownHaveReduction())
                        {
							wstring word = pEnt->rootUnit->GetWord();
                            if (unitOperator->IsSemantic(pEnt->rootUnit, SS::Core::Features::Values::TSemanticType.smtPerson, persType) &&
								!unitOperator->IsNominalDict(word) && iswupper(word[0]))
                            {
	                            pEnt->isUsed = false;
                            }
                            
                            if (pEnt->isUsed &&
								!((pEnt->nType == eEntityBaseType::Verb) || (pEnt->nType == eEntityBaseType::Ceterus)))
                            {
                                GenerateSubentitiesWithoutRightUnits(pEnt, allEntityInSent);
                            }
                        }
                        else
						{
                            pEnt->isUsed = false;
						}
                    }
                    /*EvristicEntityUnionByPretext(allEntityInSent);
                    allEntityInSent.sort(SortByPosInSentForEntity);*/

                    for (TEntityList::iterator it = allEntityInSent.begin(); it != allEntityInSent.end(); ++it)
					{
						CEntity *pEntity = *it;
						wstring value = pEntity->GetValue();
						if ((value.length() <= m_MaxEntityLength) && pEntity->isUsed)
						{
                            allEntity.push_back(pEntity);
						}
					}
                }
            }
		}

		void CBaseDigestAnalyzer::SelectSimpleDates(SS::Interface::Core::BlackBox::ISentence* pSentence, TEntityList& allEntityInSent, TUnitList& sentUnits)
		{
			for (TUnitList::iterator it = sentUnits.begin(); it != sentUnits.end(); ++it)
			{
				IUnit *pUnit = *it;
				if (!pUnit->GetParentUnit() && IsSimpleDate(pUnit))
				{
					TUnitList entUnitList;
					unitOperator->GetLinearUnitsWithSort(pUnit, entUnitList);
					CEntity *pNew = CreateNewEntity(entDate, eEntityBaseType::Date, pUnit, pSentence, entUnitList);
                    allEntityInSent.push_back(pNew);
				}
			}
		}

		bool CBaseDigestAnalyzer::IsSimpleDate(SS::Interface::Core::BlackBox::IUnit* pUnit)
		{
			wstring patternType = pUnit->GetPatternName();
			if ((patternType == L"DI") || (patternType == L"DM") || (patternType == L"DM1") ||
				(patternType == L"DMY") || (patternType == L"DMY1") || (patternType == L"DMY2") || (patternType == L"DMY3") ||
				(patternType == L"DMY4") || (patternType == L"DMY5") || (patternType == L"DMY6") ||
				(patternType == L"DMYW") || (patternType == L"DMYW1") || (patternType == L"DMYW2") ||
				(patternType == L"DMW") || (patternType == L"DMW1") || (patternType == L"DMW2") ||
				(patternType == L"DYI") || (patternType == L"YM") || (patternType == L"YMD") ||
				(patternType == L"Year") || (patternType == L"Year1") || (patternType == L"Year2") ||
				(patternType == L"Century") || (patternType == L"NumericYear"))
			{
				return true;
			}
			return false;
		}

        void CBaseDigestAnalyzer::EvristicEntityUnionByPretext(TEntityList& allEntity)
        {
            /*for(TEntityList::iterator it = allEntityInSent.begin(); it != allEntityInSent.end(); ++it)
            {
                CEntity *pEnt = *it;
                if (pEnt->isUsed && (pEnt->entType == EEntityType::entPretext))
                {
                    wstring str = pEnt->GetValue();
                }
            }*/
        }
        
        void CBaseDigestAnalyzer::GetEntitiesXMLFromEngSentence(
            SS::Interface::Core::BlackBox::ISentence* pSentence, TEntityList& allEntityInSent, TUnitList& sentUnits)
        {
            SS::Core::Features::CMorphoFeature oMorfoVerbInfinitive;
			oMorfoVerbInfinitive.Undefine();
			oMorfoVerbInfinitive.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
            oMorfoVerbInfinitive.m_VerbForm = SS::Core::Features::Values::VerbForm.vfInfinitive;

            SS::Core::Features::CMorphoFeature oMorfoVerbParticipleED;
			oMorfoVerbParticipleED.Undefine();
			oMorfoVerbParticipleED.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
            oMorfoVerbParticipleED.m_TimeType = SS::Core::Features::Values::TimeType.tmtPast;
            oMorfoVerbParticipleED.m_VerbForm = SS::Core::Features::Values::VerbForm.vfParticiple;

            SS::Core::Features::CMorphoFeature oMorfoVerbParticipleING;
			oMorfoVerbParticipleING.Undefine();
			oMorfoVerbParticipleING.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
            oMorfoVerbParticipleING.m_TimeType = SS::Core::Features::Values::TimeType.tmtPresent;
            oMorfoVerbParticipleING.m_VerbForm = SS::Core::Features::Values::VerbForm.vfParticiple;

            SS::Core::Features::CMorphoFeature oMorfoHelpVerb;
			oMorfoHelpVerb.Undefine();
			oMorfoHelpVerb.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
            oMorfoHelpVerb.m_VerbType = SS::Core::Features::Values::VerbType.vrbtAux;
            oMorfoHelpVerb.m_VerbType.AssignOR(&SS::Core::Features::Values::VerbType.vrbtMod);

            SS::Core::Features::CMorphoFeature oMorfoNoTransitionalParticiple;
			oMorfoNoTransitionalParticiple.Undefine();
			oMorfoNoTransitionalParticiple.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
            oMorfoNoTransitionalParticiple.m_VerbForm = SS::Core::Features::Values::VerbForm.vfParticiple;
            oMorfoNoTransitionalParticiple.m_TransitionalType = SS::Core::Features::Values::TransitionalType.tltNoTransitional;

            SS::Core::Features::CMorphoFeature oMorfoAllowOfficial;
			oMorfoAllowOfficial.Undefine();
			oMorfoAllowOfficial.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
			oMorfoAllowOfficial.m_OfficialType = SS::Core::Features::Values::OfficialType.otParticle;
			oMorfoAllowOfficial.m_OfficialType.AssignOR(&SS::Core::Features::Values::OfficialType.otConjunction);
            oMorfoAllowOfficial.m_OfficialType.AssignOR(&SS::Core::Features::Values::OfficialType.otInfMarker);

            SS::Core::Features::CMorphoFeature oMorfoAdverb;
			oMorfoAdverb.Undefine();
            oMorfoAdverb.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdverb;

            SS::Core::Features::CMorphoFeature oMorfoNoun;
			oMorfoNoun.Undefine();
			oMorfoNoun.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;	

            SS::Core::Features::CMorphoFeature oMorfoAdjective;
			oMorfoAdjective.Undefine();
			oMorfoAdjective.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;

			SS::Core::Features::CMorphoFeature oMorfoNumeric;
			oMorfoNumeric.Undefine();
			oMorfoNumeric.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNumeric;

            SS::Core::Features::CMorphoFeature oMorfoShort;
			oMorfoShort.Undefine();
			oMorfoShort.m_ShortType = SS::Core::Features::Values::ShortType.stShort;
			
            SS::Core::Features::CMorphoFeature oMorfoParticiple;
			oMorfoParticiple.Undefine();
			oMorfoParticiple.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;	
			oMorfoParticiple.m_VerbForm = SS::Core::Features::Values::VerbForm.vfParticiple;

            SS::Core::Features::CMorphoFeature oMorfoConjuction;
			oMorfoConjuction.Undefine();
            oMorfoConjuction.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;	
            oMorfoConjuction.m_OfficialType = SS::Core::Features::Values::OfficialType.otConjunction;
            
            SS::Core::Features::CMorphoFeature oMorfoSingleNoun;
            oMorfoSingleNoun.Undefine();
            oMorfoSingleNoun.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
            oMorfoSingleNoun.m_NumberType = SS::Core::Features::Values::NumberType.ntSingle;

			/// притяжательное местоимение
			SS::Core::Features::CMorphoFeature oMorfoPosessivePronoun;
			oMorfoPosessivePronoun.Undefine();
			oMorfoPosessivePronoun.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postPronoun;
			oMorfoPosessivePronoun.m_PronounClass = SS::Core::Features::Values::PronounClass.pncPossessive;

            SS::Core::Features::CMorphoFeature oMorfoGenetive;
            oMorfoGenetive.Undefine();
            oMorfoGenetive.m_CaseType.AssignOR(&SS::Core::Features::Values::CaseType.ctGenitive);

			SS::Core::Features::CMorphoFeature oMorfoDeterminer;
			oMorfoDeterminer.Undefine();
            oMorfoDeterminer.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
            oMorfoDeterminer.m_OfficialType = SS::Core::Features::Values::OfficialType.otDeterminers;

			SS::Core::Features::CMorphoFeature oMorfoPretext;
			oMorfoPretext.Undefine();
            oMorfoPretext.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
			oMorfoPretext.m_OfficialType = SS::Core::Features::Values::OfficialType.otPretext;

            TUnitList entUnitList;
            TUnitList insideList;
            TUnitList lstOneWordVerbs;

            set<unsigned int> stCombiVerbsUnitNumbers; // хранит позиции уже включенных в цепочки юнитов комби-глаголов
            set<unsigned int> stCombiNounsUnitNumbers; // хранит позиции уже включенных в цепочки юнитов существительных
            set<unsigned int> stNounUnits; // хранит позиции существительных и местоимений
			set<unsigned int> stAdjectiveUnits; // хранит позиции прилагательных
			set<unsigned int> stParticipleUnits; // хранит позиции причастий
			set<unsigned int> stSavedEntitiesUnits; // хранит позиции юнитов, уже включенных в сущности

			/// сохраним позиции юнитов в уже выделенных сущностях
			for (TEntityList::iterator it = allEntityInSent.begin(); it != allEntityInSent.end(); ++it)
			{
				CEntity *pEntity = *it;
				for (TUnitList::iterator unitIt = pEntity->unitList.begin(); unitIt != pEntity->unitList.end(); ++unitIt)
				{
					IUnit *pUnit = *unitIt;
					unsigned int position = pUnit->GetPosition();
					stSavedEntitiesUnits.insert(position);
				}
			}

            /// выделим 100%-е глаголы
			bool isContain_TO = false;
			wstring helpVerb = L"";
			for(TUnitList::iterator it = sentUnits.begin(); it != sentUnits.end(); ++it)
			{
				bool createVerb = false;
				IUnit *pUnit = *it;
                if(pUnit->GetParentUnit())
                    continue;

				wstring str = unitOperator->GetLowerWord(pUnit->GetWord());
				if (str == L"to")
				{
					isContain_TO = true;
					continue;
				}
				else if (unitOperator->IsVerbUnit(pUnit))
				{
					IUnit *pParent = pUnit;
					if (unitOperator->IsCombiUnit(pUnit))
						pUnit = GetPhraseVerbParentUnit(pUnit);
					unsigned int position = pUnit->GetPosition();
					if (stSavedEntitiesUnits.find(position) != stSavedEntitiesUnits.end())
						continue;
					bool needInsert = false;
					if (helpVerb != L"")
					{
						/// can/may/must/will/shall + инфинитив без to
						needInsert = ((helpVerb == L"can") || (helpVerb == L"may") || (helpVerb == L"must") ||
							(helpVerb == L"will") || (helpVerb == L"shall")) && !isContain_TO &&
							unitOperator->IsMorpho(pUnit, oMorfoVerbInfinitive);
						/// be в любой форме + причастие на –ing или -ed (2-3 формы)
						needInsert = needInsert || ((helpVerb == L"be") && 
							(unitOperator->IsMorpho(pUnit, oMorfoVerbParticipleED) ||
							unitOperator->IsMorpho(pUnit, oMorfoVerbParticipleING)));
						/// have в любой форме + be в форме been (III форма глагола) + причастие на –ing или  ed (2-3 формы)
						needInsert = needInsert || ((helpVerb == L"have") && (str == L"been"));
					}
						
					if ((needInsert || helpVerb.empty()) && unitOperator->IsMorpho(pUnit, oMorfoHelpVerb))
					{
						wstring normalization = unitOperator->GetVerbNormText(pUnit);
						insideList.push_back(pUnit);
						helpVerb = normalization;
						continue;
					}
					else if(needInsert)
                    {
                        for(TUnitList::iterator iter = insideList.begin(); iter != insideList.end(); ++iter)
						{
							stCombiVerbsUnitNumbers.insert((*iter)->GetPosition());
							entUnitList.push_back(*iter);
						}
                        insideList.clear();
						stCombiVerbsUnitNumbers.insert(position);
                        unitOperator->GetLinearUnitsWithSort(pParent, entUnitList);
						createVerb = true;
                    }
					else
					{
						IUnit *pPrev = pUnit->GetLeftUnit();
						IUnit *pNext = pUnit->GetRightUnit();
						wstring word = pUnit->GetWord();
						if ((pPrev && IsPartOfSpeech(pPrev, SS::Core::Features::Values::PartOfSpeechTypes.postAdverb)) ||
							(pNext && unitOperator->IsMorpho(pNext, oMorfoPosessivePronoun)))
						{
							stCombiVerbsUnitNumbers.insert(position);
							unitOperator->GetLinearUnitsWithSort(pUnit, entUnitList);
							createVerb = true;
						}
						else if(pPrev && (!word.empty() && (word.back() == L's')))
						/// форма на 's'
						{
							wstring prevWord = pPrev->GetWord();
							if ((unitOperator->IsMorpho(pPrev, oMorfoConjuction) && (prevWord != L"or") && (prevWord != L"and")) ||
								(prevWord == L"that") || (prevWord == L"which"))
							{
								stCombiVerbsUnitNumbers.insert(position);
								unitOperator->GetLinearUnitsWithSort(pUnit, entUnitList);
								createVerb = true;
							}
						}
					}
				}
				else if (!entUnitList.empty() && (unitOperator->IsMorpho(pUnit, oMorfoAllowOfficial) ||
					unitOperator->IsMorpho(pUnit, oMorfoAdverb)))
				{
                    insideList.push_back(pUnit);
				}
				else
				{
					insideList.clear();
					entUnitList.clear();
				}

				if (createVerb && !entUnitList.empty())
				{
					CEntity *pEnt = CreateNewEntity(entVerb, eEntityBaseType::Verb, pUnit, pSentence, entUnitList);
					allEntityInSent.push_back(pEnt);
					entUnitList.clear();
					TUnitList::reverse_iterator iter = pEnt->unitList.rbegin();
					while (true)
					{
						IUnit *pSmallVerb = *iter;
						if (pSmallVerb == pEnt->unitList.front())
							break;
						entUnitList.insert(entUnitList.begin(), pSmallVerb);
						if (!pSmallVerb->GetParentUnit() && unitOperator->IsVerbUnit(pSmallVerb))
						{
							allEntityInSent.push_back(CreateNewEntity(entVerb, eEntityBaseType::Verb, pSmallVerb,
								pSentence, entUnitList));
						}
						++iter;
					}
					entUnitList.clear();
				}

				isContain_TO = false;
				helpVerb = L"";
			}

			isContain_TO = false;
			/// флаг, что перед сущностью стоит предлог "from"
			bool isBeforeEntity_FROM = false;
			/// флаг, требующий очистки информации по окружению сущности
			bool clearInfo = false;
			/// флаг для пропуска выделения сущности по следующему существительному
			bool isMissNextNoun = false;
			/// выделение сущностей
            for(TUnitList::iterator it = sentUnits.begin(); it != sentUnits.end(); ++it)
			{
				IUnit *pUnit = *it;
				if (pUnit->GetParentUnit() ||
					unitOperator->IsHavePoint(pUnit) ||
					IsPartOfSpeech(pUnit, SS::Core::Features::Values::PartOfSpeechTypes.postOfficial))
				{
                    continue;
				}

				unsigned int position = pUnit->GetPosition();
                if ((stCombiVerbsUnitNumbers.find(position) != stCombiVerbsUnitNumbers.end()) ||
					(stSavedEntitiesUnits.find(position) != stSavedEntitiesUnits.end()))
				{
                    continue; // уже посчитали
				}

                entUnitList.clear();
				insideList.clear();
				wstring word = unitOperator->GetLowerWord(pUnit->GetWord());
				if ((word == L"to") || (word == L"one"))
				{
					clearInfo = true;
					continue;
				}
				if (IsPronoun(pUnit))
				{						
					entUnitList.push_back(pUnit);
                    stNounUnits.insert(position);
                    allEntityInSent.push_back(CreateNewEntity(entPronoun, eEntityBaseType::Pronoun, pUnit, pSentence, entUnitList));
					continue;
				}

				bool isAdjective = IsPartOfSpeech(pUnit, SS::Core::Features::Values::PartOfSpeechTypes.postAdjective) ||
					(stAdjectiveUnits.find(position) != stAdjectiveUnits.end());
				bool isNoun = IsPartOfSpeech(pUnit, SS::Core::Features::Values::PartOfSpeechTypes.postNoun) ||
					(stNounUnits.find(position) != stNounUnits.end());
				bool isParticiple = (stParticipleUnits.find(position) != stParticipleUnits.end()) ||
					(IsPartOfSpeech(pUnit, SS::Core::Features::Values::PartOfSpeechTypes.postVerb) && 
					unitOperator->IsMorpho(pUnit, oMorfoParticiple));
				bool isPossibleNumeric = unitOperator->IsMorpho(pUnit, oMorfoNumeric);
				if (!isParticiple && unitOperator->IsMorpho(pUnit, oMorfoVerbParticipleED))
				/// форма на 'ed' - причастие
				{
					isParticiple = true;
					stParticipleUnits.insert(position);
				}

				if (!isPossibleNumeric && !(isAdjective || isNoun || isParticiple))
				/// есть неопределенность
				{
					IUnit *pPrev = pUnit->GetLeftUnit();
					IUnit *pNext = pUnit->GetRightUnit();
					wstring prevWord = pPrev ? pPrev->GetWord() : L"";
					wstring nextWord = pNext ? pNext->GetWord() : L"";
					/// форма на 's'
					bool isSForm = (!word.empty() && (word.back() == L's'));
					unsigned int prevPosition = pPrev ? pPrev->GetPosition() : -1;
					if (unitOperator->IsMorpho(pUnit, oMorfoVerbParticipleING))
					/// 'ing'-овая форма
					{
						/// первое слово в предложении
						isNoun = !pPrev;
						bool isPrevAdverb = false;
						if (!isNoun && IsPartOfSpeech(pPrev, SS::Core::Features::Values::PartOfSpeechTypes.postAdverb))
						{
							pPrev = pPrev->GetLeftUnit();
							prevWord = pPrev ? pPrev->GetWord() : L"";
							/// наречие стоит в начале предложения
							isNoun = !pPrev;
							isPrevAdverb = true;
						}
						/// после запятой
						isNoun = isNoun || (prevWord == L",");
						/// впереди предлог
						isNoun = isNoun || unitOperator->IsMorpho(pPrev, oMorfoPretext);

						if (!isNoun)
						{
							/// после наречия
							isParticiple = isPrevAdverb;
							/// после союзов кроме "or" и "and"
							isParticiple = isParticiple || ((unitOperator->IsMorpho(pPrev, oMorfoConjuction) &&
								(prevWord != L"or") && (prevWord != L"and")));
							/// перед "which" или "that"
							isParticiple = isParticiple || (prevWord == L"that") || (prevWord == L"which");
							isNoun = !isParticiple;
						}
					}
					else if (unitOperator->IsMorpho(pUnit, oMorfoSingleNoun) || isSForm)
					/// исходная форма или форма на 's'
					{
						/// первое слово в предложении
						isNoun = !pPrev;
						/// впереди артикль
						isNoun = isNoun || unitOperator->IsMorpho(pPrev, oMorfoDeterminer);
						/// впереди предлог
						isNoun = isNoun || unitOperator->IsMorpho(pPrev, oMorfoPretext);
						/// впереди местоименное прилагательное
						isNoun = isNoun || IsAdjectivePronoun(pPrev);
						/// впереди прилагательное
						isNoun = isNoun || (stAdjectiveUnits.find(prevPosition) != stAdjectiveUnits.end());
						/// после "which" или "that"
						isNoun = isNoun || ((nextWord == L"that") || (nextWord == L"which"));
						if (isNoun && !isSForm && unitOperator->IsMorpho(pUnit, oMorfoAdjective))
							isNoun = false; /// еще предстоит разрешить омонимию существительное/прилагательное
					}
				}

				if (!isBeforeEntity_FROM && isContain_TO && unitOperator->IsMorpho(pUnit, oMorfoVerbInfinitive))
				/// найден глагол
				{
					entUnitList.push_back(pUnit);
					allEntityInSent.push_back(CreateNewEntity(entVerb, eEntityBaseType::Verb, pUnit, pSentence, entUnitList));
					stCombiVerbsUnitNumbers.insert(position);
				}
				else if (isAdjective)
					stAdjectiveUnits.insert(position);
				else if (isParticiple)
					stParticipleUnits.insert(position);
				else if (!isPossibleNumeric && (isNoun || (unitOperator->IsMorpho(pUnit, oMorfoNoun))))
				{
					if (isMissNextNoun)
					{
						isMissNextNoun = false;
						continue;
					}

					TUnitList::iterator iter = it;
					IUnit *pLastHomogeneousDivider = NULL;
					IUnit *pOtherUnit = NULL;
					unsigned int otherPos = -1;
					unitOperator->GetLinearUnitsWithSort(pUnit, entUnitList);
					IUnit *pNounIsParticiple = unitOperator->IsMorpho(pUnit, oMorfoVerbParticipleING) ? pUnit : NULL;
					IUnit *pLastParticiple = pNounIsParticiple;
					bool isEntityStart = true;
					while(true) // влево
					{
						if (iter == sentUnits.begin())
                            break;
						--iter;
						pOtherUnit = *iter;
                        if(pOtherUnit->GetParentUnit())
							continue;
						otherPos = pOtherUnit->GetPosition();
						isParticiple = (stParticipleUnits.find(otherPos) != stParticipleUnits.end());
						isAdjective = (stAdjectiveUnits.find(otherPos) != stAdjectiveUnits.end());
						wstring str = unitOperator->GetLowerWord(pOtherUnit->GetWord());
						if (str == L"having")
							break;
						else if (isAdjective ||
							(isParticiple && (unitOperator->IsMorpho(pOtherUnit, oMorfoVerbParticipleED) ||
							(unitOperator->IsMorpho(pOtherUnit, oMorfoVerbParticipleING) &&
							unitOperator->IsMorpho(pOtherUnit, oMorfoNoTransitionalParticiple)))))
						{
							if (isParticiple && pLastParticiple && unitOperator->IsMorpho(pOtherUnit, oMorfoVerbParticipleING))
								break;
							
							if (pLastHomogeneousDivider) 
							{
                                unitOperator->GetLinearUnitsWithSort(pLastHomogeneousDivider, entUnitList);
                                stCombiNounsUnitNumbers.insert(pLastHomogeneousDivider->GetPosition());
								pLastHomogeneousDivider = NULL;
							}
							unitOperator->GetLinearUnitsWithSort(pOtherUnit, entUnitList);
                            stCombiNounsUnitNumbers.insert(otherPos);
							isEntityStart = false;
						}
						else if ((str == L"and") || (str == L"or") || (str == L","))
						{
							if ((entUnitList.size() == 1) || pLastHomogeneousDivider)
                                break;
							else
                                pLastHomogeneousDivider = pOtherUnit;							
						}
						else
						{
							if (str == L"from")
								isBeforeEntity_FROM = true;
							break;
						}

						if (isParticiple)
							pLastParticiple = pOtherUnit;
						else
							isParticiple = NULL;
					}
					iter = it;
					IUnit *pCurrent = pUnit;
					pLastParticiple = pNounIsParticiple;
					while(true) // вправо
					{
						++iter;
						if (iter == sentUnits.end())
                            break;
                        pOtherUnit = *iter;
                        if(pOtherUnit->GetParentUnit())
							continue;
						otherPos = pOtherUnit->GetPosition();
						wstring str = unitOperator->GetLowerWord(pOtherUnit->GetWord());

						isParticiple = unitOperator->IsMorpho(pOtherUnit, oMorfoVerbParticipleED);
						isAdjective = (stAdjectiveUnits.find(otherPos) != stAdjectiveUnits.end()) ||
							IsPartOfSpeech(pUnit, SS::Core::Features::Values::PartOfSpeechTypes.postAdjective) ||
							CDictionaryPool::IsOnlyAdjective(str);

						if (IsPronoun(pOtherUnit) || (str == L"having") || (str == L"e.g.") ||
							(stCombiVerbsUnitNumbers.find(otherPos) != stCombiVerbsUnitNumbers.end()))
						{
                            break;
						}
						else if (isAdjective || isParticiple)
							insideList.push_back(pOtherUnit);
                        else if (unitOperator->IsMorpho(pOtherUnit, oMorfoNoun) &&
							unitOperator->IsMorpho(pCurrent, oMorfoSingleNoun))
						{
							pCurrent = pOtherUnit;
                            stNounUnits.insert(otherPos);
						}
						else if (unitOperator->IsMorpho(pOtherUnit, oMorfoParticiple))
							insideList.push_back(pOtherUnit);
						else
						{
							if (str == L"to")
								isContain_TO = true;
                            break;
						}

						isParticiple = isParticiple || unitOperator->IsMorpho(pOtherUnit, oMorfoVerbParticipleING);
						if (isParticiple && pLastParticiple)
							break;
						else if (unitOperator->IsMorpho(pOtherUnit, oMorfoVerbParticipleING))
							pLastParticiple = pOtherUnit;
						else
							pLastParticiple = NULL;

						if (pCurrent == pOtherUnit)
						{
							for(TUnitList::iterator itInside = insideList.begin();
                                itInside != insideList.end();
                                ++itInside)
							{
                                unitOperator->GetLinearUnitsWithSort(*itInside, entUnitList);
								stCombiNounsUnitNumbers.insert((*itInside)->GetPosition());
							}
							insideList.clear();
							unitOperator->GetLinearUnitsWithSort(pOtherUnit, entUnitList);
						}
					}

					if (!entUnitList.empty())
					/// создаем новую сущность
					{
						isNoun = isNoun || CDictionaryPool::IsOnlyNoun(word);
						if (isEntityStart && !isNoun && (entUnitList.size() > 1) &&
							unitOperator->IsMorpho(pUnit, oMorfoAdjective))
						/// сделаем прилагательным первое слово цепочки
						{
							IUnit *pNext = pUnit->GetRightUnit();
							wstring str = unitOperator->GetLowerWord(pNext->GetWord());
							wstring end = L"tion";
							if (!unitOperator->IsMorpho(pNext, oMorfoVerbParticipleING) && 
								!((str.size() > end.size()) && (str.substr(str.size() - end.size(), end.size()) == end) &&
								(str != L"composition")))
							/// следующее существительное - не отглагольное
							{
								stAdjectiveUnits.insert(position);
								stCombiNounsUnitNumbers.insert(position);
								isMissNextNoun = true;
								pUnit = pNext;
							}
						}
						
						if (!isMissNextNoun)
							stNounUnits.insert(position);

						allEntityInSent.push_back(CreateNewEntity(entUnknown, eEntityBaseType::Other, pUnit, pSentence, entUnitList));
					}
				}

				if (clearInfo)
				{
					isContain_TO = false;
					isBeforeEntity_FROM = false;
					clearInfo = false;
				}
            }

            for (TUnitList::iterator it = sentUnits.begin(); it != sentUnits.end(); ++it)
			{	
				IUnit *pUnit = *it;
				unsigned int position = pUnit->GetPosition();
				if (pUnit->GetParentUnit() ||
					!unitOperator->IsVerbUnit(pUnit) ||
					(stCombiNounsUnitNumbers.find(position) != stCombiNounsUnitNumbers.end()) ||
                    (stNounUnits.find(position) != stNounUnits.end()) ||
					(stCombiVerbsUnitNumbers.find(position) != stCombiVerbsUnitNumbers.end()))
                /// не глагол или уже посчитали в других сущностях
                {
                    continue;
                }
				entUnitList.clear();
                unitOperator->GetLinearUnitsWithSort(pUnit, entUnitList);
                allEntityInSent.push_back(CreateNewEntity(entVerb, eEntityBaseType::Verb, pUnit, pSentence, entUnitList));					
			}
        }

        void CBaseDigestAnalyzer::GetEntitiesXMLFromRusSentence(
            SS::Interface::Core::BlackBox::ISentence* pSentence, TEntityList& allEntityInSent, TUnitList& sentUnits)
        {
			SS::Core::Features::CMorphoFeature oMorfoPattern1;
			oMorfoPattern1.Undefine();

			SS::Core::Features::CMorphoFeature oMorfoPattern2;
			oMorfoPattern2.Undefine();

			SS::Core::Features::CMorphoFeature oMorfoPatternNoun;
			oMorfoPatternNoun.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;	

			SS::Core::Features::CMorphoFeature oMorfoPatternGenetive;
			oMorfoPatternGenetive.m_CaseType = SS::Core::Features::Values::CaseType.ctGenitive;

			SS::Core::Features::CMorphoFeature oMorfoPatternDative;
			oMorfoPatternDative.m_CaseType = SS::Core::Features::Values::CaseType.ctDative;

			SS::Core::Features::CMorphoFeature oMorfoPatternAdj;
			oMorfoPatternAdj.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;
			//oMorfoPatternAdj.m_PartOfSpeechTypes.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postNumeric);	

			SS::Core::Features::CMorphoFeature oMorfoPatternParticiple; // причастие
			oMorfoPatternParticiple.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;	
			oMorfoPatternParticiple.m_VerbForm = SS::Core::Features::Values::VerbForm.vfParticiple;
			oMorfoPatternParticiple.m_ShortType = SS::Core::Features::Values::ShortType.Undefined;

            SS::Core::Features::CMorphoFeature oMorfoPatternAdverb;
            oMorfoPatternAdverb.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdverb;

			SS::Core::Features::CMorphoFeature oMorfoPatternShort; // краткое причастие или прилагательное
			oMorfoPatternShort.m_ShortType = SS::Core::Features::Values::ShortType.stShort;

			SS::Core::Features::CMorphoFeature oMorfoConjOrParticle; // союз частица
			oMorfoConjOrParticle.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;	
			oMorfoConjOrParticle.m_OfficialType = SS::Core::Features::Values::OfficialType.otParticle;
			oMorfoConjOrParticle.m_OfficialType.AssignOR(&SS::Core::Features::Values::OfficialType.otConjunction);

			SS::Core::Features::CMorphoFeature oMorfoVerbChain; 
			oMorfoVerbChain.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
			oMorfoVerbChain.m_VerbForm = SS::Core::Features::Values::VerbForm.vfInfinitive;

            SS::Core::Features::CMorphoFeature oMorfoVerbImperative; 
			oMorfoVerbImperative.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
            oMorfoVerbImperative.m_VerbForm = SS::Core::Features::Values::VerbForm.vfImperative;

            SS::Core::Features::CMorphoFeature oMorfoPatternNumeric;
            oMorfoPatternNumeric.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNumeric;
            oMorfoPatternNumeric.m_NumericType = SS::Core::Features::Values::NumericType.ntOrdering;

			SS::Core::Features::CMorphoFeature oMorfoPatternGenitiveNoun;
			oMorfoPatternGenitiveNoun.m_CaseType = SS::Core::Features::Values::CaseType.ctGenitive;
			oMorfoPatternGenitiveNoun.m_CaseType.AssignOR(&SS::Core::Features::Values::CaseType.ctDative);
			oMorfoPatternGenitiveNoun.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;	

			SS::Core::Features::CMorphoFeature oMorfoPatternGenitiveAdjective;
			oMorfoPatternGenitiveAdjective.m_CaseType = SS::Core::Features::Values::CaseType.ctGenitive;
			oMorfoPatternGenitiveAdjective.m_CaseType.AssignOR(&SS::Core::Features::Values::CaseType.ctDative);
			oMorfoPatternGenitiveAdjective.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;	

			SS::Core::Features::CMorphoFeature oMorfoPatternGenitiveParticiple;
			oMorfoPatternGenitiveParticiple.m_CaseType = SS::Core::Features::Values::CaseType.ctGenitive;
			oMorfoPatternGenitiveParticiple.m_CaseType.AssignOR(&SS::Core::Features::Values::CaseType.ctDative);
			oMorfoPatternGenitiveParticiple.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;	
			oMorfoPatternGenitiveParticiple.m_VerbForm = SS::Core::Features::Values::VerbForm.vfParticiple;

            SS::Core::Features::CMorphoFeature oMorfoPatternPretext;
            oMorfoPatternPretext.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
            oMorfoPatternPretext.m_OfficialType = SS::Core::Features::Values::OfficialType.otPretext;

            SS::Core::Features::Types::TSPersonType persType;
            persType = SS::Core::Features::Values::TSPersonType.semprstFirstName;
            persType.AssignOR(&SS::Core::Features::Values::TSPersonType.semprstLastName);
            persType.AssignOR(&SS::Core::Features::Values::TSPersonType.semprstMiddleName);
            persType.AssignOR(&SS::Core::Features::Values::TSPersonType.semprstName);

            IUnit* pParticleFar = NULL;

			// теперь надо извлечь все сущности из предложени
			// идет до очердного существительного.
			// далее возращаемся назад в поисках согласованных прилагательных/числительных, потом вперед
			// в поисках либо также согласованных прилагательных либо существительных в генетиве
			TUnitList entUnitList;
			EEntityType entType = entUnknown;
			set<int> stCombiVerbsUnitNumbers; // хранит позиции уже включенных в цепочки юнитов комби-глаголов
			set<int> stCombiNounsUnitNumbers; // хранит позиции уже включенных в цепочки юнитов существительных
            set<int> stNounUnits; // хранит позиции существительных и местоимений
			TUnitList insideList;
			TUnitList lstOneWordVerbs;

			// сначала выявляем цепочки глаголов
			for (TUnitList::iterator it = sentUnits.begin(); it != sentUnits.end(); it++)
			{
				IUnit *pUnit = *it;
                if((*it)->GetParentUnit())
                    continue;

				entUnitList.clear();
				insideList.clear();
				if (IsNeedMissUnit(*it))
                    continue;
                if (!unitOperator->IsEnglishWord(pUnit) && unitOperator->IsVerbUnit(pUnit) && !unitOperator->IsMorpho(pUnit, oMorfoVerbImperative))
				{
					TUnitList::iterator it1 = it;
					entUnitList.push_back(pUnit);
					while (true)
					{
						++it1;
						if (it1 == sentUnits.end())
                            break;
						if (unitOperator->IsMorpho(*it1, oMorfoConjOrParticle))
                            insideList.push_back(*it1);
						else if (unitOperator->IsMorpho(*it1, oMorfoVerbChain) ||
						    (unitOperator->IsMorpho(*it1, oMorfoPatternShort) &&
							unitOperator->IsPseudoAdjective(*it1)))
						{
							//insideList.push_back(*it1);
							for(TUnitList::iterator it2 = insideList.begin(); it2!=insideList.end(); ++it2)
							{
								stCombiVerbsUnitNumbers.insert((*it2)->GetPosition());
								entUnitList.push_back(*it2);
							}
                            insideList.clear();
							stCombiVerbsUnitNumbers.insert((*it1)->GetPosition());
							entUnitList.push_back(*it1);
						}
						else
							break; // иначе прервались
					}
					if (entUnitList.size() > 1)
					{
						stCombiVerbsUnitNumbers.insert((*it)->GetPosition());
                        allEntityInSent.push_back(CreateNewEntity(entVerb, eEntityBaseType::Verb, *it, pSentence, entUnitList));
					}
                    else if (entUnitList.size() == 1)
					{
						// если это причастие перед которым запятая или наречия и запятая - тоже его берем
						bool isParticiple = false;
						if (unitOperator->IsMorpho(*entUnitList.begin(), oMorfoPatternParticiple))
						{
							TUnitList::iterator itPrev = unitOperator->FindUnit(sentUnits, *entUnitList.begin());
							while (itPrev != sentUnits.begin())
							{
								--itPrev;
								if (unitOperator->IsComma(*itPrev))
                                {
									isParticiple = true;
                                    break;
                                }
                                else if(!unitOperator->IsMorpho(*itPrev, oMorfoPatternAdverb))
                                    break;
							}
						}
						if (isParticiple)
						{
							stCombiVerbsUnitNumbers.insert((*it)->GetPosition());
                            allEntityInSent.push_back(CreateNewEntity(entVerb, eEntityBaseType::Verb, *it, pSentence, entUnitList));
						}
						else
						{
							lstOneWordVerbs.push_back(*entUnitList.begin());
						}
					}
				}
			}

			SS::Core::Features::CMorphoFeature oMorfoPatternCurrent;
			oMorfoPatternCurrent.Undefine();
			SS::Core::Features::CMorphoFeature oMorfoPatternTemp;
			oMorfoPatternTemp.Undefine();
			IUnit *pCurNoun = NULL;
			IUnit* pLastHomogeneousDivider = NULL;
			TUnitList dividers;
			/// морфо-характеристики внутренних сущностей
			vector<SS::Core::Features::CMorphoFeature> insideMorpho;
			/// первые юниты внутренних сущностей
			vector<IUnit*> firstUnits;
			//// английские юниты
			TUnitList englishUnits;
			for (TUnitList::reverse_iterator it = sentUnits.rbegin(); it != sentUnits.rend(); ++it)
			{
				bool isEntitySeparator = false;
				bool isPseudoAdjective = false;
				IUnit *pUnit = *it;
                if(pUnit->GetParentUnit())
                    continue;

				unsigned int unitPosition = pUnit->GetPosition();
				wstring str = unitOperator->GetLowerWord(pUnit->GetWord());
				bool isEnglishWord = unitOperator->IsEnglishWord(pUnit);
				if (!isEnglishWord && !englishUnits.empty())
				{
					GetEntitiesXMLFromEngSentence(pSentence, allEntityInSent, englishUnits);
					englishUnits.clear();
				}
				if (stCombiVerbsUnitNumbers.find(unitPosition) != stCombiVerbsUnitNumbers.end()) // уже посчитали глаголом
					isEntitySeparator = true;
				else if (IsPronoun(pUnit))
				{
					if (!entUnitList.empty() && pCurNoun)
						allEntityInSent.push_back(CreateNewEntity(entUnknown, eEntityBaseType::Other, pCurNoun, pSentence, entUnitList));
					entUnitList.clear();
					entUnitList.push_back(pUnit);
                    stNounUnits.insert(unitPosition);
					allEntityInSent.push_back(CreateNewEntity(entPronoun, eEntityBaseType::Pronoun, pUnit, pSentence, entUnitList));
					pCurNoun = NULL;
				}
				else if (unitOperator->IsMorpho(pUnit, oMorfoPatternPretext) || (str == L"как") || unitOperator->IsRomanNumber(pUnit))
					isEntitySeparator = true;
				else if (isEnglishWord)
					englishUnits.push_back(pUnit);
				else if (unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson, persType) ||
					unitOperator->IsMorpho(pUnit, oMorfoPatternNoun))
				{
					SS::Core::Features::CSemanticFeature oSemantic;
					unitOperator->GetUnitSemanticFeatures(pUnit, oSemantic);
					bool isPersonSemantic = oSemantic.m_SemanticType.Equal(SS::Core::Features::Values::TSemanticType.smtPerson);
					bool isOnlyName = unitOperator->IsPersonOnlyNameDict(pUnit);
					if ((isOnlyName && !unitOperator->IsProperByUpper(pUnit)) || !isPersonSemantic)
					{
						unitOperator->GetUnitMorfoFeaturesFull(pUnit, oMorfoPatternTemp);
						if (unitOperator->IsPseudoAdjective(pUnit) && pCurNoun)
						{
							if (oMorfoPatternTemp.IsIntersectsByCaseGenderNumber(oMorfoPatternCurrent))
								isPseudoAdjective = true;
						}
						/// определили существительное

						if (!pCurNoun)
						{
							entUnitList.clear();
							insideMorpho.clear();
							firstUnits.clear();
						}
						else if (pCurNoun && !entUnitList.empty())
						{
							allEntityInSent.push_back(CreateNewEntity(entUnknown, eEntityBaseType::Other, pCurNoun, pSentence, entUnitList));
							if (pLastHomogeneousDivider || !(unitOperator->IsMorpho(pCurNoun, oMorfoPatternGenitiveNoun) ||
								unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson, persType)))
							{
								entUnitList.clear();
								insideMorpho.clear();
								firstUnits.clear();
							}
							else
							{
								insideMorpho.push_back(oMorfoPatternCurrent);
								firstUnits.push_back(entUnitList.front());
							}
						}
						pLastHomogeneousDivider = NULL;
						oMorfoPatternCurrent = oMorfoPatternTemp;
						pCurNoun = pUnit;
						unitOperator->GetLinearUnitsWithSort(pUnit, entUnitList);
						stNounUnits.insert(unitPosition);
						continue;
					}
				}

				if (isEntitySeparator || isEnglishWord)
				{}
				else if ((str==L"и" || str==L"или" || str==L",") && (entUnitList.size() > 1))
				{
					IUnit *pFirst = entUnitList.front();
					if(stNounUnits.find(pFirst->GetPosition()) == stNounUnits.end())
					{
						if (!pLastHomogeneousDivider)
							dividers.clear();
						pLastHomogeneousDivider = pUnit;
						dividers.push_back(pLastHomogeneousDivider);
						continue;
					}
				}
				else if (IsPartOfSpeech(*it, SS::Core::Features::Values::PartOfSpeechTypes.postOfficial))
				{}
				else if ((isPseudoAdjective || unitOperator->IsPseudoAdjective(pUnit)) &&
					!unitOperator->IsMorpho(pUnit, oMorfoPatternShort))
				{
					vector<SS::Core::Features::CMorphoFeature> inclusion;
                    inclusion.push_back(oMorfoPatternAdj);
                    inclusion.push_back(oMorfoPatternParticiple);
                    inclusion.push_back(oMorfoPatternNumeric);
                    vector<SS::Core::Features::CMorphoFeature> exclusion;
                    exclusion.push_back(oMorfoPatternShort);
					unitOperator->GetUnitMorfoFeaturesFull(pUnit, oMorfoPatternTemp, NULL, &inclusion, &exclusion);
					if (oMorfoPatternCurrent.IsIntersectsByCaseGenderNumber(oMorfoPatternTemp))
					{
                        /// пересечем морфо-характеристики для разрешения неопределенности
                        oMorfoPatternCurrent.m_GenderType.AssignAND(&oMorfoPatternTemp.m_GenderType);
                        oMorfoPatternCurrent.m_CaseType.AssignAND(&oMorfoPatternTemp.m_CaseType);
                        oMorfoPatternCurrent.m_NumberType.AssignAND(&oMorfoPatternTemp.m_NumberType);

						if (pLastHomogeneousDivider)
						{
							for(TUnitList::iterator iter = dividers.begin(); iter != dividers.end(); ++iter)
								unitOperator->GetLinearUnitsWithSort(*iter, entUnitList);
							pLastHomogeneousDivider = NULL;
						}
                        unitOperator->GetLinearUnitsWithSort(pUnit, entUnitList);
						stCombiNounsUnitNumbers.insert(unitPosition);
						continue;
					}
                    else if (unitOperator->IsMorpho(pUnit, oMorfoPatternParticiple))
					{
						// если причастие но не согласованное, то оно может быть согласовано
						// со следующим за текущим существительным (проводящим электричество пластинам)
						// в этом случае по генитиву соединять существительные нельзя!
						for (unsigned int i = 0; i < insideMorpho.size(); ++i)
						{
							if (insideMorpho[i].IsIntersectsByCaseGenderNumber(oMorfoPatternTemp))
							{
								int size = 0;
								for (TUnitList::iterator iter = entUnitList.begin(); iter != entUnitList.end(); ++iter)
								{
									if (firstUnits[i] == *iter)
									{
										entUnitList.resize(size);
										break;
									}
									++size;
								}
								break;
							}
						}
					}
				}

				if (pCurNoun)
				{
					if (!entUnitList.empty())
						allEntityInSent.push_back(CreateNewEntity(entUnknown, eEntityBaseType::Other, pCurNoun, pSentence, entUnitList));
					pCurNoun = 0;
				}
			}
			if (!englishUnits.empty())
				GetEntitiesXMLFromEngSentence(pSentence, allEntityInSent, englishUnits);
			if (!entUnitList.empty() && pCurNoun)
				allEntityInSent.push_back(CreateNewEntity(entUnknown, eEntityBaseType::Other, pCurNoun, pSentence, entUnitList));

			for (TUnitList::iterator it = lstOneWordVerbs.begin(); it != lstOneWordVerbs.end(); ++it)
			/// выделим неразмеченные глагольные юниты
			{
				unsigned int pos = (*it)->GetPosition();
				if ((stCombiNounsUnitNumbers.find(pos) != stCombiNounsUnitNumbers.end()) || (stNounUnits.find(pos) != stNounUnits.end()))
                /// уже посчитали в цепочка существительных
                {
                    continue;
                }
				entUnitList.clear();
				entUnitList.push_back(*it);												
                allEntityInSent.push_back(CreateNewEntity(entVerb, eEntityBaseType::Verb, *it, pSentence, entUnitList));					
			}							

			EvristicHomogeneous(allEntityInSent, sentUnits, true);
			//EvristicHomogeneousAdj(allEntityInSent, sentUnits);
        }
		
		bool CBaseDigestAnalyzer::IsNeedMissUnit(SS::Interface::Core::BlackBox::IUnit* pUnit)
		{
			bool isOfficial = IsPartOfSpeech(pUnit, SS::Core::Features::Values::PartOfSpeechTypes.postOfficial);
			return isOfficial || unitOperator->IsEnglishWord(pUnit);
		}

        bool CBaseDigestAnalyzer::IsPartOfSpeech(
			SS::Interface::Core::BlackBox::IUnit* pUnit, SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech)
        {
            SS::Core::Features::CMorphoFeature oMorphoFeature;
            unitOperator->GetUnitMorfoFeaturesFull(pUnit, oMorphoFeature);
            return oMorphoFeature.m_PartOfSpeechTypes.Equal(partOfSpeech);
        }

		bool CBaseDigestAnalyzer::IsHavePartOfSpeech(
			SS::Interface::Core::BlackBox::IUnit* pUnit, SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech)
		{
			SS::Core::Features::CMorphoFeature oMorphoFeature;
            unitOperator->GetUnitMorfoFeaturesFull(pUnit, oMorphoFeature);
			return oMorphoFeature.m_PartOfSpeechTypes.IsContain(partOfSpeech);
		}

		bool CBaseDigestAnalyzer::isEntity(IUnit* pUnit)
		{
			SS::Core::Features::CMorphoFeature oMorfoAnfr;			
			
			bool is_entity = false;			
			bool isInitial = false;
			wstring s= pUnit->GetWord();
			int i1 = s.find(L".");
			if (i1>=0 && pUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase()==1)
			{
				isInitial = true;
			}
			unitOperator->GetUnitMorfoFeatures(pUnit,oMorfoAnfr);
			if (((pUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetDictionaryNumber()==14 /* неизвестное слово */ &&
				(pUnit->GetFirstIndex()->GetDictionaryIndex()->GetFirst().GetFirstLetterUpperCase()==1 /* с большой буквы */)) ||
				oMorfoAnfr.m_PartOfSpeechTypes.IsIntersectsWith(&SS::Core::Features::Values::PartOfSpeechTypes.postNoun) ||
				unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson) ||
				unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtOrganization) ||
				unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPlace)||
				unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtService) ||
				unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtTimeInterval) ||
				unitOperator->IsSyntax(pUnit, SS::Core::Features::Values::SyntaxCategories.scAjunctPlaceSource) ||
				unitOperator->IsSyntax(pUnit, SS::Core::Features::Values::SyntaxCategories.scMainEntity) ||
				unitOperator->IsSyntax(pUnit, SS::Core::Features::Values::SyntaxCategories.scAjunctTimeSource) ||
				unitOperator->IsSyntax(pUnit,  SS::Core::Features::Values::SyntaxCategories.scAdditionalEntity))
				&& (!isInitial))
			{
				is_entity = true;
			}
			//}
			return is_entity;
		}

		bool CBaseDigestAnalyzer::IsPersonRecursive(IUnit* pUnit)
		{
			IUnit* pChildUnit = pUnit->GetLeftChildUnit();

			while (pChildUnit)
			{
				if (unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson))
				{						
					return true;
				}
				else
				{
					if (IsPersonRecursive(pUnit))
					{
						return true;
					}
				}
				pChildUnit = pChildUnit->GetRightUnit();
			}
			return false;
		}

		bool CBaseDigestAnalyzer::IsDate(IUnit* pUnit)
		{
			// надо искать тип Person среди всех дочерних узлов
			// если нашли - то персона
			// если pUnit с большой буквы и неизвестьное слово, то персона
			// если сам PUnit - одушевленный то персона
			/* if (IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPlace) &&
			!IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson) && 
			!IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtOrganization))			 
		 {
		 return true;
		 }
		 return false;	*/		 
			SS::Core::Features::CMorphoFeature oMorfoPattern;
			oMorfoPattern.Undefine();
			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;

			wstring word = pUnit->GetWord();
			bool isTime = unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtTimeInterval);
			if (unitOperator->IsSyntax(pUnit, SS::Core::Features::Values::SyntaxCategories.scAjunctTimeSource) ||
				(isTime && !unitOperator->IsMorpho(pUnit,oMorfoPattern)) || (word.empty() && isTime))
			{
				return true;
			}
			return false;
		}

		bool CBaseDigestAnalyzer::IsAddress(IUnit* pUnit)
		{			
			if (unitOperator->IsSyntax(pUnit, SS::Core::Features::Values::SyntaxCategories.scAjunctPlaceFixedPoint))
			{
				return true;
			}
			return false;
		}

		bool CBaseDigestAnalyzer::IsSubject(IUnit* pUnit)
		{			
			if (unitOperator->IsSyntax(pUnit, SS::Core::Features::Values::SyntaxCategories.scSubjectFin))
			{
				return true;
			}
			return false;
		}

		CEntity* CBaseDigestAnalyzer::GetEntityByUnitPosition(TEntityList& lstEntities, int nPosition)
		{
			for (TEntityList::iterator it = lstEntities.begin(); it != lstEntities.end(); ++it)
			{
				CEntity *pEnt = *it;
				if (pEnt->unitList.empty())
				{
					continue;
				}
				int nPos = (*pEnt->unitList.begin())->GetPosition();
				if (nPos == nPosition)
				{
					return pEnt;
				}
			}
			return NULL;
		}

		CEntity* CBaseDigestAnalyzer::GetEntityByUnit(TEntityList& lstEntities, IUnit* pUnit)
		{
			for (TEntityList::iterator it1 = lstEntities.begin(); it1 != lstEntities.end(); it1++)
			{
				if ((*it1)->unitList.size()==0 || !(*it1)->isUsed) continue;
				if ((*(*it1)->unitList.begin())==pUnit || (*it1)->rootUnit==pUnit)
				{
					return (*it1);
				}
			}
			return NULL;
		}

		bool isStandartAbbr(wstring& str)
		{
			if (str==L"ЗАО" || str==L"ОАО" || str==L"ПБОЮЛ" || str==L"ООО" ||
				str==L"ИЧП" || str==L"КБ" || str==L"АКБ" ||  str==L"НПО" ||  str==L"АО" ||
				str==L"EUR" || str==L"RUS" || str==L"СЭД"
			   ) 
			{
				return true;
			}
			return false;
		}

		void CBaseDigestAnalyzer::SetEntityFullType(CEntity* pEntity, TEntityList& lstEntities)
		{
			if (pEntity->entType != entDefault)
			{
				return;
			}

			SS::Core::Features::CMorphoFeature oMorfoPattern;
			oMorfoPattern.Undefine();
			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
			TUnitList unitList;		

			if (pEntity->nType == eEntityBaseType::Phone)
				pEntity->entType = entPhone;
			else if (pEntity->nType == eEntityBaseType::NounVerb)
				pEntity->entType = entNounVerb;
			else if (pEntity->nType == eEntityBaseType::Verb)
				pEntity->entType = entVerb;
			else if (pEntity->nType == eEntityBaseType::Pronoun)
				pEntity->entType = entPronoun;
			else if (pEntity->nType == eEntityBaseType::Event)
				pEntity->entType = entEvent;
			else if (pEntity->nType == eEntityBaseType::Product)
				pEntity->entType = entProduct;
			else if (pEntity->nType == eEntityBaseType::Other)
			{
                bool isPlace = true;
				for (TUnitList::iterator it1 = pEntity->unitList.begin(); it1 != pEntity->unitList.end(); it1++)
				{
					isPlace = isPlace && IsPlace(*it1);
					if (unitOperator->IsUpperInsideSentence(*it1))
                    {
                        if (isPlace)
						{
                            pEntity->isPseudoPlace = true;
						}
						pEntity->entType = entUnknownProper;
						return;
                    }
				}
				pEntity->entType = entUnknown;
			}
			else
			{
				if (pEntity->rootUnit->GetReferenceUnit())
				{
					for (TUnitList::iterator it1 = pEntity->unitListRef.begin();
						 it1 != pEntity->unitListRef.end();
						 ++it1)
					{
						unitList.push_back(*it1);
					}
				}
				else
				{
					for (TUnitList::iterator it1 = pEntity->unitList.begin();
						 it1 != pEntity->unitList.end();
						 ++it1)
					{
						unitList.push_back(*it1);
					}
				}

				if (pEntity->nType == eEntityBaseType::Person)
				{
					// Если натыкаемся на фамилию/имя/отчество/инициал или
					// Rxz/Exz c большой буквы, то считаем это полной цепочкой
					EEntityType resType = entPersonNoInfo;
					int numOfQuote = 0;
					IUnit* lastUnit = *unitList.rbegin();
					if (unitOperator->IsSemantic(lastUnit, SS::Core::Features::Values::TSemanticType.smtOrganization) &&
						lastUnit->GetIsNominalOnly() && !lastUnit->GetEvristicLN())
					{
						for (TUnitList::iterator it1 = unitList.begin(); it1 != unitList.end(); it1++)
						{
							IUnit* pUnit = *it1;	
							if (unitOperator->IsProper(pUnit))
							{
								pEntity->entType = entPersonInfo; 
								return;
							}
						}				
					}

					if (pEntity->m_lang == SS::Core::Types::ealEnglish &&
						unitOperator->IsPersonApostrof(pEntity->rootUnit))
					{
						pEntity->entType = entPersonName;
						return;
					}

					for (TUnitList::reverse_iterator it1 = unitList.rbegin(); it1 != unitList.rend(); ++it1)
					{
						IUnit* pUnit = *it1;	
						wstring str = pUnit->GetWord();
						if (unitOperator->IsQuote(str))
						{
							++numOfQuote;
						}

						// артикли и предлоги надо пропускать. Они не могут определять ничего
						if (unitOperator->IsMorpho(pUnit, oMorfoPattern))
						{
							continue;
						}
						if (unitOperator->IsParentGeoCombi(pUnit))
						{
							continue;
						}

						if (numOfQuote % 2 == 0)
						{
							using namespace SS::Core::Features::Values;
							bool isLastName = unitOperator->IsSemantic(pUnit, TSemanticType.smtPerson, TSPersonType.semprstLastName);
							bool isFirstName = unitOperator->IsSemantic(pUnit, TSemanticType.smtPerson, TSPersonType.semprstFirstName);
							bool isMiddleName = unitOperator->IsSemantic(pUnit, TSemanticType.smtPerson, TSPersonType.semprstMiddleName);
							bool isOrg = unitOperator->IsSemantic(pUnit, TSemanticType.smtOrganization);
							bool isUpper = unitOperator->IsProperByUpper(pUnit);
							bool isOneUnit = (unitList.size()== 1);
							bool isUnknown = unitOperator->IsProperUnknown(pUnit);
							bool isEvristicLN = pUnit->GetEvristicLN();
							bool isInitial = pUnit->IsInitial();
							bool isKnownName = isLastName || isFirstName || isMiddleName;
							if ((isOneUnit && isUpper && (isKnownName || isUnknown || isEvristicLN)) ||
								(!isOneUnit && ((isKnownName && isUpper && !str.empty()) || isInitial || isEvristicLN || (isUnknown && !isOrg))))
							{
								resType = entPersonName;
								break; // именованная персона
							}
						}

						if (pUnit->GetIsNominalOnly() && !str.empty() &&
							!unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson))
						{
							break;
						}
					}

					if (resType == entPersonNoInfo)
					{
						numOfQuote = 0;
						if ((unitList.size() == 1) && unitOperator->IsProperByUpper(*unitList.begin()) &&
							unitOperator->IsSemantic(*unitList.begin(), SS::Core::Features::Values::TSemanticType.smtPerson,
							SS::Core::Features::Values::TSPersonType.semprstLastName))
						{
							resType = entPersonName;
						}
						else
						{
							for (TUnitList::iterator it1 = unitList.begin(); it1 != unitList.end(); it1++)
							{
								IUnit* pUnit = *it1;	
								wstring str = pUnit->GetWord();
								if (unitOperator->IsQuote(str))
								{
									numOfQuote++;
								}

								//// артикли и предлоги надо пропускать. Они не могут определять ничего
								if (unitOperator->IsMorpho(pUnit,oMorfoPattern))
								{
									continue;							
								}

								if (pUnit!=*(unitList.begin()))
								{
									CEntity* pEnt = GetEntityByUnit(lstEntities, pUnit);
									if (pEnt && pEnt!=pEntity && pEnt->unitList.size()<pEntity->unitList.size())
									{
										SetEntityFullType(pEnt, lstEntities);
										EEntityType et = pEnt->entType;
										if (et==entOrgName || et==entPlaceName || et==entOrgAbbr || et==entOrgInfo)
										// если внутри персоны есть география с именем или организация с именем, то эта персона с частичной инфой
										{
											resType = entPersonInfo; 
										}
									}
								}
							}

							if (pEntity->m_lang == SS::Core::Types::ealEnglish && unitList.size()>1)
							{
								for(TUnitList::iterator it1 = unitList.begin(); it1 != unitList.end(); ++it1)
								{
									if (unitOperator->IsProper(*it1))
									{
										resType = entPersonInfo;
										break;
									}
								}
							}
						}
					}
					pEntity->entType = resType;
				}
				else if (pEntity->nType == eEntityBaseType::Organization)
				{
					bool isAllAbbr = true;
					for (TUnitList::iterator it1 = unitList.begin(); it1 != unitList.end(); ++it1)
					{
						IUnit* pUnit = *it1;
						wstring str = pUnit->GetWord();
						if (!(unitOperator->IsAbbr(pUnit) || pUnit->IsEvristicAbbr()))
						{
							isAllAbbr = false;
						}
						if (unitOperator->IsMorpho(pUnit, oMorfoPattern))
						{
							continue;
						}
						SS::Core::Features::CSemanticFeature oSemantic;
						unitOperator->GetUnitSemanticFeatures(pUnit, oSemantic);
						bool isOrg = oSemantic.m_SemanticType.Equal(SS::Core::Features::Values::TSemanticType.smtOrganization) &&
							oSemantic.m_SOrganizationType.Equal(SS::Core::Features::Values::TSOrganizationType.orgtCompany);
						if (unitOperator->IsUpperInsideSentence(pUnit) && isOrg)
						{
							pEntity->entType = entOrgName;
							return;
						}
					}
					if (isAllAbbr)
					{
						pEntity->entType = entOrgAbbr;
					}
					else
					{
						pEntity->entType = entOrgNoInfo;
					}
				}
				else if (pEntity->nType == eEntityBaseType::Place)
				{
					EEntityType resType = entPlaceNoInfo;
					bool isUpper = false;
					for (TUnitList::iterator it1 = unitList.begin(); it1 != unitList.end(); it1++)
					{
						IUnit* pUnit = *it1;
						if (unitOperator->IsMorpho(pUnit, oMorfoPattern))
						{
							continue;
						}
						wstring str = pUnit->GetWord();
						if (unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPlace) &&
							(str.empty() || unitOperator->IsUpperInsideSentence(pUnit)))
						{
							resType = entPlaceName;
							break;
						}
					}
					pEntity->entType = resType;
				}
				else if (pEntity->nType == eEntityBaseType::Date)
				{
					pEntity->entType = entDate;
				}
			}
		}

		bool CBaseDigestAnalyzer::IsSameTypeParent(IUnit* pUnit)
		{
			eEntityBaseType type = GetEntityType(pUnit);
			IUnit* pParent = pUnit->GetParentUnit();
			while(pParent)
			{
				eEntityBaseType type1 = GetEntityType(pParent);
				//if (type==5 && type1!=0) return true;
				//if (type1==7 && type==7) return true;

                /// персоны и орги внутри географии не берем
                if (type1==eEntityBaseType::Place && 
                    (type==eEntityBaseType::Person || type==eEntityBaseType::Organization))
                {
                    return true;
                }
                /// персоны внутри организаций не берем
                if (type1==eEntityBaseType::Organization && type==eEntityBaseType::Person)
                    return true;
                /// организации внутри организация ПОКА не берем
                if (type1==eEntityBaseType::Organization && type==eEntityBaseType::Organization)
                    return true;
                if (type1==type && 
                    (type==eEntityBaseType::Place || type==eEntityBaseType::Date || type==eEntityBaseType::Person))
                {
                    return true;
                }
				pParent = pParent->GetParentUnit();
			}
			return false;
		}

		// функция получает на вход списко сущностей (lstAllEntity), целевую сущность (pTargeEnt),
		// и возвращает список родительских сущностей для данной в параметре lstParent
		void  CBaseDigestAnalyzer::GetEntityParentList(TEntityList& lstAllEntity, CEntity* pTargetEnt, TEntityList& lstParent)
		{	    
			for(TEntityList::iterator it = lstAllEntity.begin(); it != lstAllEntity.end(); it++)		
			{
				CEntity *pCurEnt = *it;
				if (!pCurEnt->isUsed || pCurEnt == pTargetEnt) continue;
				if (pCurEnt->GetBeginOffset() <= pTargetEnt->GetBeginOffset() &&
					pCurEnt->GetEndOffset() >= pTargetEnt->GetEndOffset())
				{
					lstParent.push_back(pCurEnt);
				}
			}		
		}

		bool CBaseDigestAnalyzer::IsEntTypeParent(TEntityList& allEntity, CEntity* targetEnt)
		{
			for(TEntityList::iterator it = allEntity.begin(); it != allEntity.end(); it++)		
			{
				CEntity *curEnt = *it;
				if (!curEnt->isUsed || curEnt==targetEnt)
					continue;
				if (curEnt->GetBeginOffset()<=targetEnt->GetBeginOffset() &&
					curEnt->GetEndOffset()>=targetEnt->GetEndOffset() &&  (curEnt->entType==entPersonName ||
					curEnt->entType==entPersonInfo || curEnt->entType==entOrgName))
				{
					return true;
				}
			}
			return false;
		}

		eEntityBaseType CBaseDigestAnalyzer::GetEntityType(IUnit* pRoot, TUnitList& units, eEntityBaseType rootType)
		{
			eEntityBaseType result = rootType;
			for (TUnitList::iterator it = units.begin(); it != units.end(); ++it)
			{
				IUnit *pUnit = *it;
				if (pUnit == pRoot)
					break;
				if (unitOperator->IsUpperInsideSentence(pUnit))
					result = GetUnitType(pUnit);
			}
			return result;
		}

		eEntityBaseType CBaseDigestAnalyzer::GetUnitType(SS::Interface::Core::BlackBox::IUnit* pUnit)
		{
			eEntityBaseType result = eEntityBaseType::Default;
			SS::Core::Features::CSemanticFeature oSemantic;
			unitOperator->GetUnitSemanticFeatures(pUnit, oSemantic);
			if (oSemantic.m_SemanticType.Equal(SS::Core::Features::Values::TSemanticType.smtPerson))
				result = eEntityBaseType::Person;
			else if (oSemantic.m_SemanticType.Equal(SS::Core::Features::Values::TSemanticType.smtOrganization))
				result = eEntityBaseType::Organization;
			else if (oSemantic.m_SemanticType.Equal(SS::Core::Features::Values::TSemanticType.smtPlace))
				result = eEntityBaseType::Place;
			else if (oSemantic.m_SemanticType.Equal(SS::Core::Features::Values::TSemanticType.smtEvent))
				result = eEntityBaseType::Event;
			else if (oSemantic.m_SemanticType.Equal(SS::Core::Features::Values::TSemanticType.smtProduct))
				result = eEntityBaseType::Product;
			return result;
		}

		eEntityBaseType CBaseDigestAnalyzer::GetEntityType(IUnit* pUnit)
		{
			if (IsStop(pUnit))
				return eEntityBaseType::Default;
			if (IsPhone(pUnit))
				return eEntityBaseType::Phone;
			if (IsDate(pUnit))
                return eEntityBaseType::Date;
			if (IsProduct(pUnit))
				return eEntityBaseType::Product;
			if (IsEvent(pUnit))
				return eEntityBaseType::Event;
			if (IsPlace(pUnit))
                return eEntityBaseType::Place;
			if (isOrganization(pUnit))
                return eEntityBaseType::Organization;
			if (IsPerson(pUnit))
                return eEntityBaseType::Person;
			if (IsNounVerb(pUnit))
                return eEntityBaseType::NounVerb;
			if (IsOther(pUnit))
                return eEntityBaseType::Other;
			if (unitOperator->IsVerbUnit(pUnit))
                return eEntityBaseType::Verb;
			if (IsPronoun(pUnit))
                return eEntityBaseType::Pronoun;
            return eEntityBaseType::Default;
		}

		bool CBaseDigestAnalyzer::IsStop(IUnit* pUnit)
		{
			wstring& str = unitOperator->GetLowerWord(pUnit->GetWord());
			if (str == L"как")
				return true;
			return false;
		}

		bool CBaseDigestAnalyzer::IsPhone(SS::Interface::Core::BlackBox::IUnit* pUnit)
		{
			SS::Core::Features::CSemanticFeature semantic;
			unitOperator->GetUnitSemanticFeatures(pUnit, semantic);
			if (semantic.m_SemanticType.Equal(SS::Core::Features::Values::TSemanticType.smtService) &&
				semantic.m_SServiceTypes.Equal(SS::Core::Features::Values::TSServiceTypes.sstTelephoneNumber))
			{
				return true;
			}
			return false;
		}

		bool CBaseDigestAnalyzer::IsOther(IUnit* pUnit)
		{
			SS::Core::Features::Types::PartOfSpeechTypes notNoun = SS::Core::Features::Values::PartOfSpeechTypes.postPronoun;
			notNoun.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postOfficial);
			notNoun.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postAdverb);

			wstring str = unitOperator->GetLowerWord(pUnit->GetWord());
			if (str == L"кому")
			{
				if (pUnit->GetNumber() == 1)
					return false; /// в начале предложения
				IUnit *pPrevUnit = pUnit->GetLeftUnit();
				if (pPrevUnit)
				{
					str = pPrevUnit->GetWord();
					if(str == L",")
						return false;
				}
			}
			else if ((str == L"тому") || (str == L"том"))
			{
				IUnit *pNextUnit = pUnit->GetRightUnit();
				if (pNextUnit)
				{
					str = pNextUnit->GetWord();
					pNextUnit = pNextUnit->GetRightUnit();
					if((str == L",") && pNextUnit)
					{
						str = unitOperator->GetLowerWord(pNextUnit->GetWord());
						if ((str == L"кто") || (str == L"что"))
							return false;
					}
				}
			}

			SS::Core::Features::CMorphoFeature oMorfoFeature;
			unitOperator->GetUnitMorfoFeaturesFullByMorphoAnalyzer(pUnit, oMorfoFeature);
			if (!oMorfoFeature.m_PartOfSpeechTypes.IsIntersectsWithout(&notNoun))
			{
				unitOperator->GetUnitMorfoFeaturesFull(pUnit, oMorfoFeature);
				if (!unitOperator->IsInCombi(pUnit) && !str.empty() &&
					oMorfoFeature.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postNoun))
				{
					return true;
				}
			}
			return false;		 
		}	

		bool CBaseDigestAnalyzer::IsNounVerb(IUnit* pUnit)
		{
			if (IsOther(pUnit))
			{
				wstring strNorm = unitOperator->GetLowerWord(unitOperator->GetMainFormText1(pUnit, SS::Core::Features::Values::GenderType.Undefined).form);
				return CDictionaryPool::IsNounVerb(strNorm);
			}
			return false;		 
		}

        bool CBaseDigestAnalyzer::IsAdjectivePronoun(IUnit* pUnit)
        {
            wstring word = pUnit->GetWord();
            if ((word == L"any") || (word == L"few") || (word == L"his") || (word == L"its") ||
                (word == L"my") || (word == L"our") || (word == L"this") || (word == L"your") ||
                (word == L"their"))
            {
                return true;
            }
            return false;
        }

		bool CBaseDigestAnalyzer::IsPronoun(IUnit* pUnit)
		{
			IUnit *pParent = pUnit->GetParentUnit();
			if (!unitOperator->IsUnitChild(pParent, pUnit))
			{	
				SS::Core::Features::CMorphoFeature oMorfoFeature;
				unitOperator->GetUnitMorfoFeaturesFullByMorphoAnalyzer(pUnit, oMorfoFeature);
			
				wstring word = unitOperator->GetLowerWord(pUnit->GetWord());
				if ((word == L"тем") || (word == L"кому") ||
					oMorfoFeature.m_PartOfSpeechTypes.IsIncludes(&SS::Core::Features::Values::PartOfSpeechTypes.postPronoun))
				{
					return true;
				}
			}
			return false;
		}

		bool CBaseDigestAnalyzer::IsPlace(IUnit* pUnit)
		{
			// надо проверить на место и на предлог. Должен быть предлог перед географией, тогда это место
			// а иначе это юридическое лицо
			SS::Core::Features::CMorphoFeature oMorfoPattern;
			oMorfoPattern.Undefine();
			oMorfoPattern.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
			oMorfoPattern.m_PartOfSpeechTypes.AssignOR(&SS::Core::Features::Values::PartOfSpeechTypes.postAdverb);		 

			SS::Core::Features::CMorphoFeature oMorfoPattern1;
			oMorfoPattern1.Undefine();
			oMorfoPattern1.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postAdjective;

			if (unitOperator->IsInQuote(pUnit))
                return false;

			wstring str = pUnit->GetWord();
			SS::Core::Features::CSemanticFeature oSemantic;
			unitOperator->GetUnitSemanticFeatures(pUnit, oSemantic);
			bool isPlace = oSemantic.m_SemanticType.IsIncludes(&SS::Core::Features::Values::TSemanticType.smtPlace);
			if (isPlace && !str.empty() && !oSemantic.m_SPlaceType.IsUndefined() &&
				!oSemantic.m_SPlaceType.IsContain(SS::Core::Features::Values::TSPlaceType.pltProvince) &&
				!unitOperator->IsProperByUpper(pUnit))
			{
				isPlace = false;
			}
			if (isPlace && unitOperator->IsProperByUpper(pUnit))
				return true;
			bool bound1 = !oSemantic.m_SemanticType.IsIncludes(&SS::Core::Features::Values::TSemanticType.smtPerson) &&
				!unitOperator->IsMorpho(pUnit, oMorfoPattern);
			bool bound2 = !str.empty() && unitOperator->IsMorpho(pUnit, oMorfoPattern) && !pUnit->HasChilds();
			IUnit* pParent = pUnit->GetParentUnit();
			wstring strParent = L"test";
			if (pParent && !str.empty())
                strParent = pParent->GetWord();

			if (!bound2 && (unitOperator->IsSyntax(pUnit, SS::Core::Features::Values::SyntaxCategories.scAjunctPlaceSource) ||
				(isPlace && bound1)) && !pUnit->IsInitial() && 
				(!strParent.empty() || !unitOperator->IsUndefinedSyntax(pUnit)))
            {
                return true;
			}
            else
                return false;
		}

		bool CBaseDigestAnalyzer::IsEvent(SS::Interface::Core::BlackBox::IUnit* pUnit)
		{
			return unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtEvent);
		}

		bool CBaseDigestAnalyzer::IsProduct(SS::Interface::Core::BlackBox::IUnit* pUnit)
		{
			return unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtProduct);
		}

		bool CBaseDigestAnalyzer::IsPerson(IUnit* pUnit)
		{
			wstring str = pUnit->GetWord();
			/*if (unitOperator->IsInQuote(pUnit))
                return false;*/
			wstring strLower = unitOperator->GetLowerWord(str);
			if (strLower == L"якобы")
				return false;
			SS::Core::Features::CMorphoFeature oMorpho;
			SMIList morpho;
			unitOperator->GetUnitMorfoFeaturesFull(pUnit, oMorpho, &morpho);
			if (!oMorpho.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postNoun))
				return false;

			bool isLocative = false;
			for (SMIList::iterator it = morpho.begin(); !isLocative && (it != morpho.end()); ++it)
				if ((*it).caseType.Equal(SS::Core::Features::Values::CaseType.ctLocative))
					isLocative = true;

			bool isPersonSemantic = unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson);
			if (isPersonSemantic && isLocative)
			{
				isPersonSemantic = false;
				IUnit* pPrevious = pUnit->GetLeftUnit();
				IUnit* pParent = pUnit->GetParentUnit();
				if (pParent && unitOperator->IsUnitChild(pParent, pUnit))
					pPrevious = pParent->GetLeftUnit();
				if (pPrevious)
				{
					SS::Core::Features::CMorphoFeature oMorpho;
					unitOperator->GetUnitMorfoFeaturesFull(pPrevious, oMorpho);
					if(oMorpho.m_OfficialType.Equal(SS::Core::Features::Values::OfficialType.otPretext))
						isPersonSemantic = true;
				}
			}
			if (unitOperator->IsSyntax(pUnit, SS::Core::Features::Values::SyntaxCategories.scMainEntity) ||
				isPersonSemantic)
		    {
				unitOperator->GetUnitMorfoFeaturesFullByMorphoAnalyzer(pUnit, oMorpho);
				if (oMorpho.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postAdverb))
				{
					SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech = oMorpho.m_PartOfSpeechTypes;
					if (partOfSpeech.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postAdverb))
						return false;
					else
					{
						partOfSpeech.Remove(SS::Core::Features::Values::PartOfSpeechTypes.postAdverb);
						if (partOfSpeech.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postOfficial))
							return false;
					}
				}

				bool isOnlyName = unitOperator->IsPersonOnlyNameDict(pUnit);
			    if (!str.empty() && isOnlyName && !unitOperator->IsProperByUpper(pUnit))
                    return false;
			    
				if (!str.empty() && pUnit->GetIsNominalOnly() && !isPersonSemantic)
					return false;

			    IUnit* pChild = pUnit->GetLeftChildUnit();
			    if (isOnlyName && unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson,
				    SS::Core::Features::Values::TSPersonType.semprstLastName) &&
					!str.empty() && pChild && !unitOperator->IsProperUnknown(pChild) &&
				    !unitOperator->IsSemantic(pChild, SS::Core::Features::Values::TSemanticType.smtPerson))
                {
                    return false;
                }
			    return true;
            }
            else
                return false;
		}

		bool CBaseDigestAnalyzer::IsProperDict(IUnit* pUnit)
		{
			IIndex* pIndex;
			IDictionaryIndex* pDictionaryIndex;

			for (pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
			{
				pDictionaryIndex = pIndex->GetDictionaryIndex();

				if (pDictionaryIndex)
				{
					if (pDictionaryIndex->GetFirst().GetDictionaryNumber()==SS::Dictionary::DATA_TYPE::NAMES::ednEProperName ||
						pDictionaryIndex->GetFirst().GetDictionaryNumber()==SS::Dictionary::DATA_TYPE::NAMES::ednRProperName)
					{
						return true;
					}
				}
			}

			return false;
		}

		bool CBaseDigestAnalyzer::isOrganization(IUnit* pUnit)
		{
			wstring str = pUnit->GetWord();
			TUnitList ulist;
			unitOperator->GetLinearUnits(pUnit, ulist);
			SS::Core::Features::CSemanticFeature oSemantic;
			unitOperator->GetUnitSemanticFeatures(pUnit, oSemantic);
			bool isOrg = oSemantic.m_SemanticType.Equal(SS::Core::Features::Values::TSemanticType.smtOrganization)/* &&
				oSemantic.m_SOrganizationType.Equal(SS::Core::Features::Values::TSOrganizationType.orgtCompany)*/;
			if (str.empty())
			{
				if (!isOrg && oSemantic.m_SemanticType.IsUndefined())
				{
					isOrg = true;
					for (TUnitList::iterator it = ulist.begin(); it != ulist.end(); ++it)
					{
						wstring word = (*it)->GetWord();
						if (!word.empty() && (!(*it)->IsEvristicAbbr() || 
							unitOperator->IsSemantic(*it, SS::Core::Features::Values::TSemanticType.smtPlace)))
						{
							isOrg = false;
							break;
						}
					}
				}
			}
			else if (!pUnit->HasChilds() && pUnit->IsEvristicAbbr() &&
				     !unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPlace) &&
				     !unitOperator->IsSemantic(pUnit, SS::Core::Features::Values::TSemanticType.smtPerson))
			{
				return true;
			}
			else if (IsPronoun(pUnit))
			{
				return false;
			}
			return isOrg;
		}

		void CBaseDigestAnalyzer::LoadSimpleDictionary(wstring& dict_file, CTreeDictionary** pTreeDict)
		{
			wstring strDataBasePath = m_pIWorkRegister->GetParameter(L"PathDataBase");
			wstring strDict = strDataBasePath + dict_file;	

			*pTreeDict = new CTreeDictionary(unitOperator);
			(*pTreeDict)->LoadDict(strDict.c_str());  	
		}

		void CBaseDigestAnalyzer::GenerateSubentitiesByHomogeneous(CEntity* pEntity, TEntityList& lstAllEntityInSent)
		{
			TEntityList lstParentEntities;
			GetEntityParentList(lstAllEntityInSent, pEntity, lstParentEntities);
			for (TUnitList::iterator itUnit = pEntity->unitList.begin(); itUnit != pEntity->unitList.end(); itUnit++)
			{
				//if
			}
		}

        void CBaseDigestAnalyzer::LoadServiceDictionaries(vector<vector<wstring>>& dictionaries)
        {
            CDictionaryPool::LoadFromCollection(dictionaries, unitOperator);
        }

		void CBaseDigestAnalyzer::FilterMorphoInfo(SMIList* pLstMI, SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech)
		{
			if (pLstMI)
			{
				for (SMIList::iterator it = pLstMI->begin(); it != pLstMI->end();)
				{
					if ((*it).partOfSpeech.IsIntersectsWith(&partOfSpeech))
						++it;
					else
						it = pLstMI->erase(it);
				}
			}
		}

		EntitiesCollection CBaseDigestAnalyzer::SelectEntities(
			const wstring& srcText, SS::Interface::Core::BlackBox::IText* pText, const wstring& baseDate)
		{
			InitializeEntitiesSelection();

			TEntityList allEntity;
			list<int> offsets;
			GetEntitiesFromText(allEntity, srcText, pText, offsets, baseDate, false, false);

			return ConvertEntities(allEntity, srcText);
		}

		EntitiesCollection CBaseDigestAnalyzer::SelectSimpleEntities(
			const wstring& srcText, SS::Interface::Core::BlackBox::IText* pText, bool isSelectOnlyCeterus)
		{
			InitializeEntitiesSelection();
			
			TEntityList allEntity;
			GetSimpleEntities(pText, isSelectOnlyCeterus, allEntity);
			return ConvertEntities(allEntity, srcText);
		}

		void CBaseDigestAnalyzer::InitializeEntitiesSelection()
		{
			unitOperator->ClearMorphoInformation();
			// пометили предложения для дополнительной обработки синтаном и выделения сущностей
			//MarkSentByOffset(pText, offsets, isTonality, isTonality);
		}

		EntitiesCollection CBaseDigestAnalyzer::ConvertEntities(TEntityList& entities, const wstring& srcText)
		{
			using SS::Core::Types::CTextEntity;

			EntitiesCollection result;
			for (TEntityList::iterator it = entities.begin(); it != entities.end(); ++it)
			{
				result.push_back(ConvertEntity(*it, srcText));
			}
			return (result);
		}

		SS::Core::Types::CTextEntity CBaseDigestAnalyzer::ConvertEntity(CEntity* pEntity, const wstring& srcText)
		{
			SS::Core::Types::CTextEntity textEntity;
			textEntity.set_Value(pEntity->GetValue());
			textEntity.set_EntityType(pEntity->GetTypeStr());
			textEntity.set_StartPosition(pEntity->GetBeginOffset());
			textEntity.set_EndPosition(pEntity->GetEndOffsetNew(srcText));
			textEntity.set_MainNoun(pEntity->m_strProfession);
			textEntity.set_MainNounPrefix(pEntity->m_strPrefix);

			textEntity.set_FirstName(pEntity->m_strFirstName);
			textEntity.set_MiddleName(pEntity->m_strMiddleName);
			textEntity.set_LastName(pEntity->m_strLastName);
			textEntity.set_Name(pEntity->m_Name);
			textEntity.set_Post(pEntity->m_strPost);
			textEntity.set_IsPseudoPlace(pEntity->isPseudoPlace);
			textEntity.set_LinkedPlaces(pEntity->GetLinkedPlaces());

			textEntity.add_MorphoInformation(pEntity->GetMorphoProp());
			textEntity.add_Adjectives(pEntity->GetAdjectives());
			for (TEntityList::iterator it = pEntity->m_lstSubEntites.begin(); it != pEntity->m_lstSubEntites.end(); ++it)
				textEntity.add_SubEntity(ConvertEntity(*it, srcText));

			if (pEntity->m_lang == SS::Core::Types::ealEnglish)
				textEntity.set_Language(L"EN");
			else
				textEntity.set_Language(L"RU");
			return textEntity;
		}
	}	
}