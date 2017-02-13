#include "StdAfx.h"
#include ".\search_factory.h"

#include ".\eval.h"
#include ".\res_check_cnv.h"
#include ".\arg_res_int.h"
#include ".\b_logic.h"
#include ".\f_logic.h"
#include ".\r_logic.h"
#include ".\hdr_logic.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Extracting
{

using namespace SS::Core::NdxSE::NdxProcessor::Storages;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;
using namespace SS::Interface::Core::CommonContainers;
using namespace SS::Core::Types::IndexationSearch;
using namespace SS::Core::NdxSE::NdxProcessor::Extracting::Logics;
using namespace SS::Interface::Core::NdxSE;

//--------------------------------------------------------------------//

CSearchFactory::CSearchFactory(
		const wchar_t* wszIndexStorageName,
		Storages::CStorageManager* pStorageManager, 
		SS::Core::NdxSE::NdxProcessor::Containers::CFindResultCollection* pSearchResult,
		CSearchResultFiller::TResultsTextsNumbers* pResultsTextsNumbers,
		IParametrizedQuery*	pPrmQueryResult,
		TSearchParams* pSearchParameter, 
		CNdxSearchSettings* pSearchSettings)
:CEvalFactory(pStorageManager->GetIndexStorage(wszIndexStorageName)->GetContainersFactory()), 
m_pConvertorToLinearResults(NULL), m_pESFillerFromQuery(NULL), m_pStorageManager(pStorageManager)
{
	//устанавливаем параметры работы
	GetFactory()->SetSearchSettings(pSearchSettings);

	m_SearchSettings.m_CommonSettings=(*pSearchParameter);
	m_SearchSettings.m_EngineSettings=(*pSearchSettings);

	//создаем конвертор результатов в линейные
	m_pConvertorToLinearResults=new 
		CSearchResultFiller(pSearchResult, GetFactory()->GetIndexStorage(), 
		pResultsTextsNumbers, 
		m_SearchSettings.m_EngineSettings.m_ResultsSettings.m_uiIntervalExpandRange,
		m_SearchSettings.m_EngineSettings.m_SearchSettings.m_StorageUsingMode);

	//создаем парсер запроса
	if(pSearchSettings->m_SearchSettings.m_EvalRelevanceMode==
			CNdxSearchSettings::SSearchSettings::erWordProximityWithStatistic)
		m_pESFillerFromQuery=new CESFillerFromQuery_Statistic(GetFactory(), pPrmQueryResult);
	else
		m_pESFillerFromQuery=new CESFillerFromQuery(GetFactory(), pPrmQueryResult);
}

CSearchFactory::~CSearchFactory(void)
{
	if(m_pConvertorToLinearResults) delete m_pConvertorToLinearResults;
	if(m_pESFillerFromQuery) delete m_pESFillerFromQuery;
}

CCompositeLogic* CSearchFactory::CreateEquationLogic(void)
{
	CCompositeLogic* pCompositeLogic=new CCompositeLogic();
	
	//устанавливаем основную логику булеву или fuzzy
	if(m_SearchSettings.m_EngineSettings.m_SearchSettings.m_SearchMode==CNdxSearchSettings::SSearchSettings::esmFuzzySearch){
		//добавляем Fuzzy логику
		CFuzzyLogic* pFuzzyLogic=new CFuzzyLogic();
		pFuzzyLogic->SetActivationLevels(0, GetFactory()->GetIndexStorage()->GetWordsControlLevelNumber());
		pCompositeLogic->AddLogic(pFuzzyLogic);
	}else if(m_SearchSettings.m_EngineSettings.m_SearchSettings.m_SearchMode==CNdxSearchSettings::SSearchSettings::esmBooleanSearch){
		//добавляем булеву логику с апдейтом позиций
		CBooleanLogic* pBooleanLogic=new CBooleanLogic();
		pBooleanLogic->SetActivationLevels(0, GetFactory()->GetIndexStorage()->GetWordsControlLevelNumber());
		pCompositeLogic->AddLogic(pBooleanLogic);
	}
	
	//создаем дополнительные логики
	if(m_SearchSettings.m_EngineSettings.m_SearchSettings.m_StorageUsingMode==CNdxSearchSettings::SSearchSettings::esumSingleStorage){
		//режим поиска в одном хранилище
		//устанавливаем логику расчета релевантности
		if(m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode!=CNdxSearchSettings::SSearchSettings::erUndefined){
			CRelevanceLogic* pRelevanceLogic=new CRelevanceLogic();
			pRelevanceLogic->SetActivationLevel(GetFactory()->GetIndexStorage()->GetWordsControlLevelNumber());
			pCompositeLogic->AddLogic(pRelevanceLogic);

			if(m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==CNdxSearchSettings::SSearchSettings::erTextsStatistic ||
				m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==CNdxSearchSettings::SSearchSettings::erAll){
				//устанавливаем учет статистики по текстам
				pRelevanceLogic->UseStatistic((CIndexStatisticStorage*)GetFactory()->GetIndexStorage(), 
					GetFactory()->GetIndexStorage()->GetTextsBinaryStorage()->GetTextsNumber());
			}

			if(m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==CNdxSearchSettings::SSearchSettings::erWordProximity ||
				m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==CNdxSearchSettings::SSearchSettings::erAll){

				if(!wcscmp(GetFactory()->GetIndexStorage()->GetIndexStorageName(), L"TABLES")){
					//устанавливаем учет близости расположения слов
					pRelevanceLogic->UseTableProximity(GetFactory(), 
						m_SearchSettings.m_EngineSettings.m_PatternSearchSettings.m_uiIndexProximityLimitWeight, 
						m_SearchSettings.m_EngineSettings.m_ResultsSettings.m_uiIntervalInTextMaxCount);
				}else{
					//устанавливаем учет близости расположения слов
					pRelevanceLogic->UseTextProximity(GetFactory(), 
						m_SearchSettings.m_EngineSettings.m_PatternSearchSettings.m_bCheckIndexSequence, 
						m_SearchSettings.m_EngineSettings.m_PatternSearchSettings.m_uiIndexProximityLimitWeight, 
						m_SearchSettings.m_EngineSettings.m_ResultsSettings.m_uiIntervalInTextMaxCount);
				}
				}else if(m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==CNdxSearchSettings::SSearchSettings::erWordProximityWithStatistic){
					if(!wcscmp(GetFactory()->GetIndexStorage()->GetIndexStorageName(), L"TEXT")){
						//устанавливаем учет близости расположения слов
						pRelevanceLogic->UseStatisticProximity(GetFactory());
					}
				}
			
			//устанавливаем логике запрос
			pRelevanceLogic->SetQuery(m_pESFillerFromQuery->GetFillingSource());
		}

	}else if(m_SearchSettings.m_EngineSettings.m_SearchSettings.m_StorageUsingMode==
			CNdxSearchSettings::SSearchSettings::esumHeadersAndTextStorage ||
			m_SearchSettings.m_EngineSettings.m_SearchSettings.m_StorageUsingMode==
					CNdxSearchSettings::SSearchSettings::esumHeadersAndTablesStorage){
		//режим поиска в текстах и заголовках хранилище
		if(!wcscmp(GetFactory()->GetIndexStorage()->GetIndexStorageName(), L"HEADERS")){
			//если имеем этап поикса в заголовках вне зависимости от параметров создаем спецальную логику
			CHeaderLogic* pHeaderLogic=new CHeaderLogic();
			pHeaderLogic->SetActivationLevel(GetFactory()->GetIndexStorage()->GetWordsControlLevelNumber());
			pCompositeLogic->AddLogic(pHeaderLogic);
			//устанавливаем логике запрос	
			pHeaderLogic->SetQuery(m_pESFillerFromQuery->GetFillingSource());

		}else if(!wcscmp(GetFactory()->GetIndexStorage()->GetIndexStorageName(), L"TEXT")){
			//для текстовго хранилища
			//создаем логику расчета близости, с учетом граничных позиций и заголовков
		
			CRelevanceLogic* pRelevanceLogic=new CRelevanceLogic();
			pRelevanceLogic->SetActivationLevel(GetFactory()->GetIndexStorage()->GetWordsControlLevelNumber());
			pCompositeLogic->AddLogic(pRelevanceLogic);

			if(m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==CNdxSearchSettings::SSearchSettings::erWordProximity ||
				m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==CNdxSearchSettings::SSearchSettings::erAll){
				//устанавливаем учет близости расположения слов
				pRelevanceLogic->UseTextAndHeadersProximity(GetFactory(), 
					m_pConvertorToLinearResults->GetCurrSearchResultCollection(),
					static_cast<Storages::CIndexStorageHeaders*>(m_pStorageManager->GetIndexStorage(L"HEADERS")),
					m_SearchSettings.m_EngineSettings.m_PatternSearchSettings.m_bCheckIndexSequence, 
					m_SearchSettings.m_EngineSettings.m_PatternSearchSettings.m_uiIndexProximityLimitWeight, 
					m_SearchSettings.m_EngineSettings.m_ResultsSettings.m_uiIntervalInTextMaxCount);
			}
			
			//устанавливаем логике запрос
			pRelevanceLogic->SetQuery(m_pESFillerFromQuery->GetFillingSource());
		}else if(!wcscmp(GetFactory()->GetIndexStorage()->GetIndexStorageName(), L"TABLES")){
			//для текстовго хранилища
			//создаем логику расчета близости, с учетом граничных позиций и заголовков
		
			CRelevanceLogic* pRelevanceLogic=new CRelevanceLogic();
			pRelevanceLogic->SetActivationLevel(GetFactory()->GetIndexStorage()->GetWordsControlLevelNumber());
			pCompositeLogic->AddLogic(pRelevanceLogic);

			if(m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==CNdxSearchSettings::SSearchSettings::erWordProximity ||
				m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==CNdxSearchSettings::SSearchSettings::erAll){
				//устанавливаем учет близости расположения слов
					pRelevanceLogic->UseTableAndHeadersProximity(GetFactory(), 
					m_pConvertorToLinearResults->GetCurrSearchResultCollection(),
					static_cast<Storages::CIndexStorageHeaders*>(m_pStorageManager->GetIndexStorage(L"HEADERS")),
					static_cast<Storages::CIndexStorageTables*>(m_pStorageManager->GetIndexStorage(L"TABLES")),
					m_SearchSettings.m_EngineSettings.m_PatternSearchSettings.m_bCheckIndexSequence, 
					m_SearchSettings.m_EngineSettings.m_PatternSearchSettings.m_uiIndexProximityLimitWeight, 
					m_SearchSettings.m_EngineSettings.m_ResultsSettings.m_uiIntervalInTextMaxCount);
			}
			
			//устанавливаем логике запрос
			pRelevanceLogic->SetQuery(m_pESFillerFromQuery->GetFillingSource());
		}
	}

	return pCompositeLogic;
}

CResultsContainer* CSearchFactory::CreateResultsContainer(void)
{
	if(ISNULL(GetFactory()->GetIndexStorage())) return NULL;
	
	unsigned int uiSearchLimitDepth=0;
	//устанавливаем глубину поиска - последний уровень
	uiSearchLimitDepth=GetFactory()->GetIndexStorage()->GetLevelStoragesNumber()-1;

	if(m_SearchSettings.m_CommonSettings.m_SearchInObtainedMode==TSearchParams::esiomText){
		//если поиск в найденном
		unsigned int uiCorrectionLimitDepth=EMPTY_VALUE;
		//глубина уровня текстов
		uiCorrectionLimitDepth=GetFactory()->GetIndexStorage()->GetWordsControlLevelNumber();

		//создаем контайнер с возможностью коррекции
		CResultsContainerWithCorrection* pResultsContainer=
			new CResultsContainerWithCorrection(
				this, 
				GetFactory()->GetIndexStorage(),
				uiSearchLimitDepth, 
				uiCorrectionLimitDepth);
		
		//заполняем формулу корректирующими значениями
		unsigned int CoordBuffer[5] = { 0 };
		CSearchResultFiller::TResultsTextsNumbers* pResultsTextsNumbers=m_pConvertorToLinearResults->GetResultsTextsNumbers();
		if(pResultsTextsNumbers && !pResultsTextsNumbers->empty()){
			for(CSearchResultFiller::TResultsTextsNumbers::iterator itTextNumber=pResultsTextsNumbers->begin(); itTextNumber!=pResultsTextsNumbers->end(); itTextNumber++){
				//ToConsole(L"TextNumber", (*itTextNumber));
				//кладем в буфер номер текста
				CoordBuffer[1]=(unsigned int)(*itTextNumber);
				//добавляем в контайнер номер текста
				pResultsContainer->AddCorrectionResult(CoordBuffer, 2);
			}

			//контайнер заполнен, очищаем собранные номера текста будем заполнять их заново
			pResultsTextsNumbers->clear();

			//pResultsContainer->View();
		}else{

			TO_CONSOLE(L"pResultsTextsNumbers not valid or empty! Perfom simple searching");		
			
			//удаляем созданный контайнер
			if(pResultsContainer) delete pResultsContainer;

			//если нет поиска в найденном создаем обычный контайнер 
			//с ограничением на количестов результатов
			return new CResultsContainer(
				this, 
				uiSearchLimitDepth);

		}
		
		return pResultsContainer;
	}else{
		
		//очищаем собранные номера текста будем заполнять их заново
		if(m_pConvertorToLinearResults->GetResultsTextsNumbers())
			m_pConvertorToLinearResults->GetResultsTextsNumbers()->clear();

		//если нет поиска в найденном создаем обычный контайнер 
		//с ограничением на количестов результатов
		return new CResultsContainer(
			this, 
			uiSearchLimitDepth);
	}

}

CResultsChecker* CSearchFactory::CreateResultsChecker(unsigned int uiValidResultsDepth)
{
	if(!wcscmp(GetFactory()->GetIndexStorage()->GetIndexStorageName(), L"HEADERS")){	
		
		//если поиск с учетом заголовков на стадии поиска в заголовках
		return new CResultsCheckerWithHIDConvertion(uiValidResultsDepth, 
			m_SearchSettings.m_CommonSettings.m_uiMaxSearchResultsCount,
			m_pConvertorToLinearResults);
	
	}else if(m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==
		CNdxSearchSettings::SSearchSettings::erWordProximity ||
		m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==
		CNdxSearchSettings::SSearchSettings::erAll ||
			m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==
			CNdxSearchSettings::SSearchSettings::erWordProximityWithStatistic){
		
		//при поиске с подсчетом близости позиций
		return new CResultsCheckerWithIntervalConvertion(uiValidResultsDepth, 
			m_SearchSettings.m_CommonSettings.m_uiMaxSearchResultsCount,
			m_pConvertorToLinearResults);
	}else{
		
		//при обычном поиске
		return new CResultsCheckerWithConvertion(uiValidResultsDepth, 
			m_SearchSettings.m_CommonSettings.m_uiMaxSearchResultsCount,
			m_pConvertorToLinearResults);
	}
}

CResultArgument* CSearchFactory::CreateResultArgument(unsigned int uiArgumentID, float fArgumentWeight, unsigned int uiArgumentDepth)
{

	if(!wcscmp(GetFactory()->GetIndexStorage()->GetIndexStorageName(), L"HEADERS")){	
		if((GetFactory()->GetIndexStorage()->GetLevelStoragesNumber()-1)==uiArgumentDepth){
			//если поиск с учетом заголовков на стадии поиска в заголовках
			//для последнего уровня мы создаем аргумент 
			//с хранением идентиифкаторов заголовков
			return new CResultArgument_HID(uiArgumentID, fArgumentWeight);
		}
	}else{
		if(m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==
			CNdxSearchSettings::SSearchSettings::erWordProximity ||
			m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==
			CNdxSearchSettings::SSearchSettings::erAll ||
			m_SearchSettings.m_EngineSettings.m_SearchSettings.m_EvalRelevanceMode==
			CNdxSearchSettings::SSearchSettings::erWordProximityWithStatistic){

			if((GetFactory()->GetIndexStorage()->GetLevelStoragesNumber()-1)==uiArgumentDepth){
				//если нужно вычислять близость, и мы создаем аргумент последнего уровня,
				//то используем аргумент с хранением вычисленных интервалов
				return new CResultArgumentWithIntervals(uiArgumentID, fArgumentWeight);
			}
		}
	}	

	//в остальных случаях создаем обычный аргумент
	return new CResultArgument(uiArgumentID, fArgumentWeight);
}

CEvaluator* CSearchFactory::CreateEvaluator(void)
{
	//создаем вычислитель
	CEvaluator* pEvaluator=new CEvaluator(this);
	//инитим вычислитель
	pEvaluator->Init(m_pESFillerFromQuery);

	return pEvaluator;
}

CBlockEquation* CSearchFactory::CreateCalcEquation(void)
{
	if(m_SearchSettings.m_EngineSettings.m_SearchSettings.m_StorageUsingMode==
		CNdxSearchSettings::SSearchSettings::esumHeadersAndTextStorage ||
		m_SearchSettings.m_EngineSettings.m_SearchSettings.m_StorageUsingMode==
		CNdxSearchSettings::SSearchSettings::esumHeadersAndTablesStorage){
		//для поиска в текстах и заголовках создаем другую вычислительную формулу 
		return new CBlockOREquation(this);
	}else{
		//для для остальных создаем обычную вычислительную формулу 
		return CEvalFactory::CreateCalcEquation();
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}