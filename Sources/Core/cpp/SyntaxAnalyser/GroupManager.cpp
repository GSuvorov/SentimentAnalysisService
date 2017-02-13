#include "stdafx.h"
#include "GroupManager.h"
#include "ExpertGroup.h"

#include "ExpertFragmentationIndexation.h"
#include "ExpertFragmentationQuery.h"
#include "ExpertResumeQuery.h"
#include "ExpertDetectQuestionQuery.h"
#include "ExpertFragmentationRussian.h"
#include "GenericExpertUniformBinder.h"
#include "GenericExpertSubordinate.h"
#include "ExpertClause.h"
#include "ExpertCoordinate.h"
#include "UnitsExpert.h"
#include "BlockExpert.h"
#include "../ASSInterface/ISyntaxConstructions.h"

#include <algorithm>

namespace SS
{
namespace Syntax
{
namespace ExpertModel
{
	using namespace SS::Syntax::ExpertClause;
	using namespace SS::SyntaxConstructions::Types;
	using namespace std;

	///Конструктор
	CGroupManager::CGroupManager() : 
		  m_bGroupsLoaded(false),
		  m_pCurrentCollection(NULL),
		  m_pExpertResumeQuery(NULL),
		  m_pLoadManager(NULL),
		  m_pAnalyseParams(NULL),
		  m_ParsingType(ParsingType::ptUndefined),
		  m_eBase(ESyntaxBase::sbStandart)
	{
		//m_itCurrentGroup = m_NullCollection.end();
		m_mapCollections.insert( make_pair( ptQueryAnalyzeEnglish, &m_vQueryGroupsEng ) );
		m_mapCollections.insert( make_pair( ptFullParsingEnglish, &m_vIndexationGroupsEng ) );
		m_mapCollections.insert( make_pair( ptQueryAnalyzeRussian, &m_vQueryGroupsRus ) );
		m_mapCollections.insert( make_pair( ptFullParsingRussian, &m_vIndexationGroupsRus ) );
		m_mapCollections.insert( make_pair( ptMorphoResolveEnglish, &m_vIndexationAmbGroupsEng ) );
		m_mapCollections.insert( make_pair( ptMorphoResolveRussian, &m_vIndexationAmbGroupsRus ) );
	}

	///Деструктор
	CGroupManager::~CGroupManager()
	{
		Clear();
	}

	///Установить LoadManager
	void CGroupManager::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
	{
		m_pLoadManager = pLoadManager;
	}

	///Получение указателя на первую группу
	CExpertGroup* CGroupManager::GetFirstGroup()
	{
		SS_TRY
		{
			m_pCurrentCollection = m_mapCollections.find(m_ParsingType)->second;
			
			if ( m_pCurrentCollection->empty() ) 
			{ SS_THROW(L"Группы конструкций для данного типа разбора отсутствуют"); }
			
			m_itCurrentGroup = m_pCurrentCollection->begin();

			return (*m_itCurrentGroup);
		}
		SS_CATCH(L"")
	}
	
	///Получение указателя на следующую группу (вернет NULL если групп больше нет)
	CExpertGroup* CGroupManager::GetNextGroup()
	{
		SS_TRY
		{
			if ( m_itCurrentGroup == m_pCurrentCollection->end()  ) 
			{ SS_THROW(L"Не вызван метод GetFirstGroup()"); }
			
			++m_itCurrentGroup;

			if ( m_itCurrentGroup == m_pCurrentCollection->end() ) 
			{ 
				m_itCurrentGroup = m_pCurrentCollection->end() ; 
				return NULL;
			}
				
			return (*m_itCurrentGroup);
		}
		SS_CATCH(L"")
	}
	
	//Загрузка групп для конкретного языка
	void CGroupManager::LoadGroupsForLanguage(
						SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionsManager* pSyntaxConstructionsManager,
						TCollection & rAllGroups,
						TCollection & rAmbiguityGroups,
						TCollection & rQueryGroups,
						TCollection & rIndexationGroups )
	{
		SS_TRY
		{
			//Получение списка конструкций
			list<SS::SyntaxConstructions::Types::CConstructionEx> lstConstructions;
			SS::Interface::Core::SyntaxConstructions::ISyntaxConstructions* pSyntaxConstructions = NULL;
			pSyntaxConstructions = pSyntaxConstructionsManager->GetSyntaxConstructions();
			pSyntaxConstructions->Get( &(lstConstructions) );

			//Получение списка групп
			list<SS::SyntaxConstructions::Types::SGroupAndConstruction> lstGroups;
			SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionGroups* pSyntaxConstructionGroups = NULL;
			pSyntaxConstructionGroups = pSyntaxConstructionsManager->GetSyntaxConstructionGroups();
			pSyntaxConstructionGroups->Get( lstGroups );
			//Получение списков групп по типу разбора с их свойствами
			list<SGroupAndCharacteristics> lstIndexationGroups;
			list<SGroupAndCharacteristics> lstQueryGroups;
			pSyntaxConstructionGroups->GetGroupsForType(SS::Core::Types::eamIndexation, lstIndexationGroups);
			pSyntaxConstructionGroups->GetGroupsForType(SS::Core::Types::eamQuery, lstQueryGroups);

			//Хранилище уже созданных групп
			std::map<int, CExpertGroup*> mapExistGroups;
			std::map<int, CExpertGroup*>::iterator itExistGroup;
						
			//Перебор списка конструкций
			list<SS::SyntaxConstructions::Types::CConstructionEx>::iterator itConstr;
			list<SS::SyntaxConstructions::Types::SGroupAndConstruction>::iterator itGroup;
			list<SGroupAndCharacteristics>::iterator itGroupCh;
			int iIdGroup;
			
			for (itGroup = lstGroups.begin(); itGroup != lstGroups.end(); itGroup++)
			{
				for (itConstr = lstConstructions.begin(); itConstr != lstConstructions.end(); itConstr++)
				{
					if (itGroup->m_i32IDConstruction == itConstr->GetID())
					{ break; }		
				}
				if (itConstr == lstConstructions.end())
				{ SS_THROW(L"Неправильный список конструкций"); }

				//Получение идентификатора текущей группы
				iIdGroup = itGroup->m_i32IDGroup;
				//Создание эксперта
				CBaseExpert* pUnitsExpert;
				if ( (*(itConstr->begin()))->m_ObjectFeature.m_ScopeOfTheRule.Equal(Values::ScopeOfTheRule.scpSentenceBoundary) )
				{ pUnitsExpert = new CBlockExpert(*itConstr, m_pLoadManager, m_pAnalyseParams, iIdGroup, m_eBase); }
				else
				{ pUnitsExpert = new CUnitsExpert(*itConstr, m_pLoadManager, m_pAnalyseParams, iIdGroup, m_eBase); }

				//Создана ли уже такая группа
				itExistGroup = mapExistGroups.find(iIdGroup);
				if ( itExistGroup == mapExistGroups.end() ) 
				{
					CExpertGroup* pExpertGroup = new CExpertGroup(iIdGroup);
					if ( (*(itConstr->begin()))->m_ObjectFeature.m_ScopeOfTheRule.Equal(Values::ScopeOfTheRule.scpSentenceBoundary) )
					{ pExpertGroup->SetAsUnionBlocks(); }
					pExpertGroup->AddExpert(pUnitsExpert);
					rAllGroups.push_back(pExpertGroup);
					mapExistGroups.insert( std::map<int, CExpertGroup*>::value_type(iIdGroup, pExpertGroup) );
				}
				else
				{ itExistGroup->second->AddExpert(pUnitsExpert); }
			}
											
			//Группы для индексации
			for (itGroupCh = lstIndexationGroups.begin(); itGroupCh != lstIndexationGroups.end(); itGroupCh++)
			{
				itExistGroup = mapExistGroups.find(itGroupCh->iIdGroup);
				if ( itExistGroup != mapExistGroups.end() ) 
				{
					if ( itGroupCh->iResolveAmbiguity )
					{ 
						itExistGroup->second->SetAsAmbiguity(); 
						if ( find(rAmbiguityGroups.begin(), rAmbiguityGroups.end(), itExistGroup->second) == rAmbiguityGroups.end() ) 
						{ rAmbiguityGroups.push_back(itExistGroup->second); }
					}
					
					if ( find(rIndexationGroups.begin(), rIndexationGroups.end(), itExistGroup->second) == rIndexationGroups.end() ) 
					{ rIndexationGroups.push_back(itExistGroup->second); }
				}
				else
				{
					SS_THROW(L"(Группа не содержит конструкций) Несоответствие идентификаторов групп");
				}
			}

			//Группы для разбора запроса
			for ( itGroupCh = lstQueryGroups.begin(); itGroupCh != lstQueryGroups.end(); itGroupCh++ )
			{
				itExistGroup = mapExistGroups.find(itGroupCh->iIdGroup);
				if ( itExistGroup != mapExistGroups.end() ) 
				{
					if ( itGroupCh->iResolveAmbiguity ) 
					{ itExistGroup->second->SetAsAmbiguity(); }
					
					if ( find(rQueryGroups.begin(), rQueryGroups.end(), itExistGroup->second) == rQueryGroups.end() ) 
					{ rQueryGroups.push_back(itExistGroup->second); }
				}
				else
				{
					SS_THROW(L"(Группа не содержит конструкций) Несоответствие идентификаторов групп");
				}
			}
		}
		SS_CATCH(L"")
	}

	///Один полный цикл динамического разбора
	bool CGroupManager::DynamicAnalyse(bool bFragm, BoardElements::CBoard* pBoard)
	{
		SS_TRY
		{
			bool bMore = false;
			CExpertGroup* pExpertGroup = GetFirstGroup();
			
			for (;pExpertGroup && pExpertGroup->IsAmbiguity(); pExpertGroup = GetNextGroup())
			{
				for (CExpertGroup::TExpertPtrItr ig = pExpertGroup->begin(); ig != pExpertGroup->end(); ++ig)
				{
                    wstring str = (*ig)->GetName();

					//эксперт определения вопросительного слова при Запросе (Query)
					if (str == L"ExpertDetectQuestion") 
					{
						if ((*ig)->PassBoard((*pBoard), pBoard->GetFirstBlock()))
						    continue;
						else
						    return false;
					}

					//эксперт деления на блоки при Запросе (Query)
					if ((str == L"Fragmentation Expert Query") && (!bFragm))
					    continue;
							
					for (PBLOCK pBlock = pBoard->GetFirstBlock(); pBlock; pBlock = pBoard->GetNextBlock())
					    (*ig)->PassBoard(*pBoard, pBlock);
				}		
			}
			
			//Условие на случай если применялись только группы разреш омонимии
			if (!pExpertGroup)
			    return false;
						
			//Группа с которой начинается разбор после разбиения на гипотезы
			TCollectionItr itExpertGroupStart = m_itCurrentGroup;

			//Применение обычных экспертов
			int count = 0;
			for (BoardElements::PBLOCK pBlock = pBoard->GetFirstBlock(); pBlock; pBlock = pBoard->GetNextBlock())
			{
				while (pBlock->SwitchVersionDynamic(m_ParsingType))
				{
					for(m_itCurrentGroup = itExpertGroupStart, pExpertGroup = (*m_itCurrentGroup);
                        pExpertGroup && !pExpertGroup->IsUnionBlocks();
				        pExpertGroup = GetNextGroup())
					{
						for (CExpertGroup::TExpertPtrItr ig = pExpertGroup->begin(); ig != pExpertGroup->end(); ++ig)
						{
							wstring str = (*ig)->GetName();
							(*ig)->PassBoard(*pBoard, pBlock); 
						}
					}
					pBlock->SetBestVersionDynamic();
					++count;
				}
				//Обнулить состояние применимости
				for(m_itCurrentGroup = itExpertGroupStart, pExpertGroup = (*m_itCurrentGroup);
                    pExpertGroup && !pExpertGroup->IsUnionBlocks();
			        pExpertGroup = GetNextGroup())
				{
					for (CExpertGroup::TExpertPtrItr ig = pExpertGroup->begin(); ig != pExpertGroup->end(); ++ig)
					    (*ig)->ClearApplicableState();
				}
				//Установить окончательную гипотезу для блока
				pBlock->SetVersionFinal();
			}

			//Применение экспертов для работы с блоками
			for (; pExpertGroup; pExpertGroup = GetNextGroup())
			{
				for (CExpertGroup::TExpertPtrItr ig = pExpertGroup->begin(); ig != pExpertGroup->end(); ++ig)
				    (*ig)->PassBoard(*pBoard, NULL);
			}
						
			for (BoardElements::PBLOCK pBlock = pBoard->GetFirstBlock(); pBlock; pBlock = pBoard->GetNextBlock())
			{
				pBlock->SetVersionFinal();
				//Работа ResumeExpert - а
				if (m_ParsingType == ptQueryAnalyzeEnglish)
				    bMore = m_pExpertResumeQuery->PassBoard(*pBoard, pBlock);
			}
			return bMore;
		}
		SS_CATCH(L"");
	}

	void CGroupManager::ParseSentenceDynamic(BoardElements::CBoard* pBoard, ParsingType pType)
	{
		SS_TRY
		{
			SS::Core::CommonServices::CCriticalSection oCriticalSection(&m_oManagerLifeTimeForCriticalSection);
			
			bool bFragm = false;
			m_ParsingType = pType;
			bFragm = DynamicAnalyse(bFragm, pBoard);
			if (bFragm)
			{
				BoardElements::BoardElementsIterator itElem;
				for (itElem = pBoard->ElementsBegin(); itElem != pBoard->ElementsEnd(); itElem++)
				{ (*itElem)->Reset(); }
				DynamicAnalyse(bFragm, pBoard);
			}
			//Семантический эксперт
			//if (m_eBase == sbRussianNQ)
			//{ m_oExpertSemantic.PassBoard(*pBoard); }
			//Применение разбора к юнитам
			pBoard->AssignSyntaxInfoToBlackBoxMultilayer();
		}
		SS_CATCH(L"")
	}

	//Возвращает итератор на первую группу (методы нужен только для тестера)
	CGroupManager::TCollectionItr CGroupManager::GetBeginGroup()
	{
		SS_TRY
		{
			m_pCurrentCollection = m_mapCollections.find(m_ParsingType)->second;
			if ( m_pCurrentCollection->empty() ) 
			{ SS_THROW(L"Группы конструкций для данного типа разбора отсутствуют"); }
		}
		SS_CATCH(L"")
		
		return m_pCurrentCollection->begin();
	}

	//Возвращает итератор на группу за последней (методы нужен только для тестера)
	CGroupManager::TCollectionItr CGroupManager::GetEndGroup()
	{
		return m_pCurrentCollection->end();
	}

	///МЕТОДЫ ILoad
	///Загрузить конструкции
	bool CGroupManager::Load()
	{
		SS_TRY
		{
			using SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionsManager;
			SS::Core::CommonServices::CCriticalSection oCriticalSection(&m_oManagerLifeTimeForCriticalSection);

			if (m_bGroupsLoaded)
			{ return false; }

			//ЗАГРУЗКА ГРУПП И КОНСТРУКЦИЙ ИЗ БАЗЫ

			//Получение менеджера конструкций Английского языка
			ISyntaxConstructionsManager* pSyntaxConstructionsManagerEng = (ISyntaxConstructionsManager*)
			m_pLoadManager->GetInterface(L"./core/linguistics/SyntaxConstructions.dll", CLSID_ISyntaxConstructionManagerEng, IID_ISyntaxConstructionManager);
			//Создание групп экспертов для Английского языка
			LoadGroupsForLanguage(  pSyntaxConstructionsManagerEng,
									m_vAllGroupsEng,
									m_vIndexationAmbGroupsEng,
									m_vQueryGroupsEng,
									m_vIndexationGroupsEng  );

			//Получение менеджера конструкций Русского языка
			ISyntaxConstructionsManager* pSyntaxConstructionsManagerRus = NULL;
			if (m_eBase == sbStandart) 
			{
				pSyntaxConstructionsManagerRus = (ISyntaxConstructionsManager*)
				m_pLoadManager->GetInterface(L"./core/linguistics/SyntaxConstructions.dll", CLSID_ISyntaxConstructionManagerRus, IID_ISyntaxConstructionManager);
			}
			else if (m_eBase == sbRussianNQ)
			{
				pSyntaxConstructionsManagerRus = (ISyntaxConstructionsManager*)
				m_pLoadManager->GetInterface(L"./core/linguistics/SyntaxConstructions.dll", CLSID_ISyntaxConstructionManagerRus_NQ, IID_ISyntaxConstructionManager);
			}
			else
			{
				SS_THROW(L"Неизвестный тип синтакcической базы.");
			}
						
			//Создание групп экспертов для Русского языка
			LoadGroupsForLanguage(  pSyntaxConstructionsManagerRus,
									m_vAllGroupsRus,
									m_vIndexationAmbGroupsRus,
									m_vQueryGroupsRus,
									m_vIndexationGroupsRus  );

			//ЗАГРУЗКА ГРУПП И КОНСТРУКЦИЙ НАПИСАНЫХ ВРУЧНУЮ (Для Английского языка)

			/*1*/CExpertGroup* pGroupUtilityEng			= new CExpertGroup(1);			//Группа вспомогательных экспертов
			/*2*/CExpertGroup* pGroupUtilityQueryEng	= new CExpertGroup(1);			//Группа вспомогательных экспертов
			/*3*/CExpertGroup* pGroupUniformEng			= new CExpertGroup(99);			//Группа связи однородных членов
			/*4*/CExpertGroup* pGroupUniformFinalEng	= new CExpertGroup(100);		//Окончательная связь однородных членов

			pGroupUtilityEng->SetAsAmbiguity();
			pGroupUtilityQueryEng->SetAsAmbiguity();

			//Добавление для деструктора
			m_vAllGroupsEng.push_back(pGroupUtilityEng);
			m_vAllGroupsEng.push_back(pGroupUtilityQueryEng);
			m_vAllGroupsEng.push_back(pGroupUniformEng);
			m_vAllGroupsEng.push_back(pGroupUniformFinalEng);

			//Распределение по типу (Порядок заполнения не менять !!!)
			m_vIndexationAmbGroupsEng.insert(m_vIndexationAmbGroupsEng.begin(), pGroupUtilityEng);

			m_vIndexationGroupsEng.insert(m_vIndexationGroupsEng.begin(), pGroupUtilityEng);
			m_vIndexationGroupsEng.push_back(pGroupUniformEng);
			m_vIndexationGroupsEng.push_back(pGroupUniformFinalEng);

			m_vQueryGroupsEng.insert(m_vQueryGroupsEng.begin(), pGroupUtilityQueryEng);
			m_vQueryGroupsEng.push_back(pGroupUniformEng);
			m_vQueryGroupsEng.push_back(pGroupUniformFinalEng);

			/*1*/
			CExpertFragmentationIndexation* pExpertFragmIndexation = new CExpertFragmentationIndexation();
			pGroupUtilityEng->AddExpert(pExpertFragmIndexation);
			/*2*/
			CExpertDetectQuestionQuery* pExpertDetectQuestionQuery = new CExpertDetectQuestionQuery();
			pGroupUtilityQueryEng->AddExpert(pExpertDetectQuestionQuery);
			CExpertFragmentationQuery* pExpertFragmQuery = new CExpertFragmentationQuery();
			pGroupUtilityQueryEng->AddExpert(pExpertFragmQuery); 
			/*3*/
			//CGenericExpertUniformBinderEng* pExpertUniformEng = new CGenericExpertUniformBinderEng();
			//pGroupUniformEng->AddExpert(pExpertUniformEng);
			/*4*/
		/*	CGenericExpertUniformBinderFinalEng* pExpertUniformFinalEng = new CGenericExpertUniformBinderFinalEng();
			pGroupUniformFinalEng->AddExpert( pExpertUniformFinalEng );
			CGenericExpertSubordinate* pGenericExpertSubordinate = new CGenericExpertSubordinate();
			pGroupUniformFinalEng->AddExpert(pGenericExpertSubordinate);
			CExpertClauseRelative* pExpertClauseRelative = new CExpertClauseRelative();
			pGroupUniformFinalEng->AddExpert(pExpertClauseRelative);
			CExpertClauseAdjunct* pExpertClauseAdjunct = new CExpertClauseAdjunct();
			pGroupUniformFinalEng->AddExpert(pExpertClauseAdjunct);
			CExpertCoordinate* pExpertCoordinate = new CExpertCoordinate();
			pGroupUniformFinalEng->AddExpert(pExpertCoordinate);*/
			//Отдельный эксперт
			m_pExpertResumeQuery = new CExpertResumeQuery();

			//ЗАГРУЗКА ГРУПП И КОНСТРУКЦИЙ НАПИСАНЫХ ВРУЧНУЮ (Для Русского языка)

			//Группа для эксперта деления на блоки
			CExpertGroup* pGroupFragmentationRus = new CExpertGroup(1);
			pGroupFragmentationRus->SetAsAmbiguity();
			m_vAllGroupsRus.push_back(pGroupFragmentationRus);
			//Занесение в списки групп
			m_vQueryGroupsRus.insert(m_vQueryGroupsRus.begin(), pGroupFragmentationRus);
			m_vIndexationGroupsRus.insert(m_vIndexationGroupsRus.begin(), pGroupFragmentationRus);
			m_vIndexationAmbGroupsRus.insert(m_vIndexationAmbGroupsRus.begin(), pGroupFragmentationRus);
			//Создание эксперта
			CExpertFragmentationRusInd* pExpertFragmentationRusInd = new CExpertFragmentationRusInd();
			pGroupFragmentationRus->AddExpert(pExpertFragmentationRusInd);

			//Установка флага, что группы загружены
			m_bGroupsLoaded = true;
		}
		SS_CATCH(L"")
		return true;
	}

	///Перезагрузитьт конструкции
	bool CGroupManager::ReLoad()
	{
		if (!m_bGroupsLoaded)
		{ return false; }
		Clear();
		Load();
		return true;
	}
	
	///Выгрузить конструкции
	bool CGroupManager::Clear()
	{
		SS_TRY
		{
			for (TCollectionItr it = m_vAllGroupsEng.begin(); it != m_vAllGroupsEng.end(); ++it)
			{ delete (*it); }
			for (TCollectionItr it = m_vAllGroupsRus.begin(); it != m_vAllGroupsRus.end(); ++it)
			{ delete (*it); }
			
			delete m_pExpertResumeQuery;
			m_bGroupsLoaded = false;	

			//m_itCurrentGroup = m_NullCollection.end();
			m_pCurrentCollection = NULL;
			
			m_vAllGroupsEng.clear();
			m_vAllGroupsRus.clear();
			m_vQueryGroupsEng.clear();
			m_vIndexationGroupsEng.clear();
			m_vQueryGroupsRus.clear();
			m_vIndexationGroupsRus.clear();
			m_vIndexationAmbGroupsEng.clear();
			m_vIndexationAmbGroupsRus.clear();
		}
		SS_CATCH(L"")
		return true;
	}

	///Установить AnalyseParams
	void CGroupManager::SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams)
	{
		m_pAnalyseParams = pAnalyseParams;
	}


}
}
}