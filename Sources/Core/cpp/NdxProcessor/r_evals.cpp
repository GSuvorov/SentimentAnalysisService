#include "StdAfx.h"
#include ".\r_evals.h"
#include ".\prx_eval.h"
#include ".\prx_eval_hdr.h"
#include ".\prx_eval_tbl.h"
#include ".\prx_eval_ans.h"

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
namespace Logics
{

using namespace SS::Interface::Core::CommonContainers;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;
using namespace SS::Core::NdxSE::NdxProcessor::Extracting;

//--------------------------------------------------------------------//
CRelevanceEvaluators::CRelevanceEvaluators(CContainersFactory* pContainersFactory) : 
	CMasterCollection<CRelevanceEvaluator>(0),
	CFactoryClient<CContainersFactory>(pContainersFactory), m_bCheckSequence(false),
	m_uiIntervalLimitWeight(0), m_uiIntervalLimitNumber(0),
	m_pFindResultCollection(NULL), m_pIndexStorageHeaders(NULL),
	m_EvalProductionMode(epmTxtStatProximity), m_pIndexStorageTables(NULL)
{
}

CRelevanceEvaluators::CRelevanceEvaluators(CContainersFactory* pContainersFactory, 
										   unsigned int uiIntervalLimitWeight, unsigned int uiIntervalLimitNumber) :
	CMasterCollection<CRelevanceEvaluator>(0),
	CFactoryClient<CContainersFactory>(pContainersFactory), m_bCheckSequence(false),
	m_uiIntervalLimitWeight(uiIntervalLimitWeight), m_uiIntervalLimitNumber(uiIntervalLimitNumber),
	m_pFindResultCollection(NULL), m_pIndexStorageHeaders(NULL),
	m_EvalProductionMode(epmTblProximity), m_pIndexStorageTables(NULL)
{
}

CRelevanceEvaluators::CRelevanceEvaluators(CContainersFactory* pContainersFactory, bool bCheckSequence, 
										   unsigned int uiIntervalLimitWeight, unsigned int uiIntervalLimitNumber) :
	CMasterCollection<CRelevanceEvaluator>(0),
	CFactoryClient<CContainersFactory>(pContainersFactory), m_bCheckSequence(bCheckSequence),
	m_uiIntervalLimitWeight(uiIntervalLimitWeight), m_uiIntervalLimitNumber(uiIntervalLimitNumber),
	m_pFindResultCollection(NULL), m_pIndexStorageHeaders(NULL),
	m_EvalProductionMode(epmTxtProximity), m_pIndexStorageTables(NULL)
{
}

CRelevanceEvaluators::CRelevanceEvaluators(CContainersFactory* pContainersFactory, bool bCheckSequence, 
		unsigned int uiIntervalLimitWeight, unsigned int uiIntervalLimitNumber, 
		CFindResultCollection* pFindResultCollection, Storages::CIndexStorageHeaders* pIndexStorageHeaders) : 
	CMasterCollection<CRelevanceEvaluator>(0),
	CFactoryClient<CContainersFactory>(pContainersFactory), m_bCheckSequence(bCheckSequence),
	m_uiIntervalLimitWeight(uiIntervalLimitWeight), m_uiIntervalLimitNumber(uiIntervalLimitNumber),
	m_pFindResultCollection(pFindResultCollection), m_pIndexStorageHeaders(pIndexStorageHeaders),
	m_EvalProductionMode(epmHdrAndTxtProximity), m_pIndexStorageTables(NULL)
{
}

CRelevanceEvaluators::CRelevanceEvaluators(CContainersFactory* pContainersFactory, bool bCheckSequence, 
		unsigned int uiIntervalLimitWeight, unsigned int uiIntervalLimitNumber, 
		CFindResultCollection* pFindResultCollection, 
		Storages::CIndexStorageHeaders* pIndexStorageHeaders, 
		Storages::CIndexStorageTables* pIndexStorageTables) :
	CMasterCollection<CRelevanceEvaluator>(0),
	CFactoryClient<CContainersFactory>(pContainersFactory), m_bCheckSequence(bCheckSequence),
	m_uiIntervalLimitWeight(uiIntervalLimitWeight), m_uiIntervalLimitNumber(uiIntervalLimitNumber),
	m_pFindResultCollection(pFindResultCollection), 
	m_pIndexStorageHeaders(pIndexStorageHeaders), m_pIndexStorageTables(pIndexStorageTables),
	m_EvalProductionMode(epmHdrAndTblProximity)
{
}

CRelevanceEvaluators::~CRelevanceEvaluators(void)
{
	ClearRangedIntervals();
}

CRelevanceEvaluator* CRelevanceEvaluators::AddEvaluator(unsigned int uiEvaluatorID)
{
	//создаем вычислитель по идентификатору трансформации
	CRelevanceEvaluator* pEvaluator=NULL;
	switch(m_EvalProductionMode){
		case epmTxtProximity:
			///вычислители близости в тексте
			pEvaluator=new CProximityEvaluator_Text(uiEvaluatorID, m_bCheckSequence, GetFactory());
			break;
		case epmHdrAndTxtProximity:
			///вычислители близости в тексте с учетом заголовков
			pEvaluator=new CProximityEvaluator_HdrText(m_pFindResultCollection, m_pIndexStorageHeaders, uiEvaluatorID, m_bCheckSequence, GetFactory());
			break;
		case epmTblProximity:
			///вычислители близости в таблицах
			pEvaluator=new CProximityEvaluator_Table(uiEvaluatorID, m_bCheckSequence, GetFactory());
			break;
		case epmHdrAndTblProximity:
			///вычислители близости в таблицах с учетом заголовков
			pEvaluator=new CProximityEvaluator_HdrTable(m_pIndexStorageTables, m_pFindResultCollection, m_pIndexStorageHeaders, uiEvaluatorID, m_bCheckSequence, GetFactory());
			break;
		case epmTxtStatProximity:
			///вычислители близости в таблицах с учетом заголовков
			pEvaluator=new CProximityEvaluator_Answer(uiEvaluatorID, GetFactory());
			break;
		default:
			break;
	}
	
	//устанавливаем владельца вычислителю
	if(pEvaluator) pEvaluator->SetOwner(this);

	//кладем его в коллекцию
	return AddChildElement(pEvaluator->GetCollectionID(), pEvaluator);
}

void CRelevanceEvaluators::AddArgumentPosition(CComplexArgument* pComplexArgument, CCoordinateNode_4s* pCoordinateNode)
{
	if(ISNULL(pComplexArgument)) return;
	//AddArgumentPositionByID(pComplexArgument->GetAttributes().m_TransformationID, pComplexArgument, pCoordinateNode);

	for(unsigned int uiChildID=0; uiChildID<m_ChildCollection.size(); uiChildID++){
		if(m_ChildCollection[uiChildID]){
			AddArgumentPositionByID(uiChildID, pComplexArgument, pCoordinateNode);
		}
	}
}

void CRelevanceEvaluators::SetDocumentBasisIndex(unsigned int uiValue)
{
	TChildCollection::iterator itProximityEvaluator=m_ChildCollection.begin();
	do{
		if((*itProximityEvaluator)){	
			(*itProximityEvaluator)->SetDocumentBasisIndex(uiValue);
		}
	}while(++itProximityEvaluator!=m_ChildCollection.end());
}

float CRelevanceEvaluators::Evaluate(unsigned int uiEvalID)
{
	float fWeight=0.0;

	///очищаем коллекцию интервалов
	ClearRangedIntervals();
	
	///вычисляем интервалы кладем их в общую коллекцию
	TChildCollection::iterator itProximityEvaluator=m_ChildCollection.begin();
	do{
		if((*itProximityEvaluator)){	
			///вычисляем интервалы
			(*itProximityEvaluator)->Evaluate(uiEvalID);
			/////переносим вычисленные интервалы в общую коллекцию
			//AddToRangedIntervals((*itProximityEvaluator)->GetIntervalCollection());
			////очищаем их у вычислителя
			//(*itProximityEvaluator)->GetIntervalCollection()->clear();
		}
	}while(++itProximityEvaluator!=m_ChildCollection.end());

	///переносим вычисленные интервалы в общую коллекцию
	AddToRangedIntervals();
	
	if(!m_RangedIntervals.empty()){ 
		//устанавливаем лучший вес
		fWeight=(float)m_RangedIntervals.rbegin()->first;
	}

	//return m_EvalProductionMode==epmTxtStatProximity?fWeight:(fWeight*1000);
	return fWeight;
}

void CRelevanceEvaluators::ClearRangedIntervals(void)
{
	for(TRangedIntervalCollection::iterator itRI=m_RangedIntervals.begin(); itRI!=m_RangedIntervals.end(); itRI++){
		if(itRI->second){
			TIntervalCollection::iterator itInterval=itRI->second->begin();
			while(itInterval!=itRI->second->end()){
				if((*itInterval)) delete (*itInterval);
				++itInterval;
			}
			delete itRI->second;
		}
	}

	m_RangedIntervals.clear();
}

void CRelevanceEvaluators::AddToRangedIntervals(void)
{
	pair<TRangedIntervalCollection::iterator, bool> InsRes;
	for(TIntervalsByHeadTail::iterator itInterval=m_IntervalsByHeadTail.begin(); itInterval!=m_IntervalsByHeadTail.end(); itInterval++){
		//ищем интервалы с таким же весом
		TRangedIntervalCollection::iterator itRI=m_RangedIntervals.find(itInterval->second->m_uiWeight);
		if(itRI==m_RangedIntervals.end()){
			InsRes=m_RangedIntervals.insert(
				TRangedIntervalCollection::value_type(itInterval->second->m_uiWeight, new TIntervalCollection));
			itRI=InsRes.first;
		}

		//вставляем интервал в коллекцию
		itRI->second->push_back(itInterval->second);
	}
	m_IntervalsByHeadTail.clear();
}

TIntervalCollection* CRelevanceEvaluators::GetBestIntervals(void)
{
	if(m_RangedIntervals.empty()){ 
		return NULL;
	}else{ 
		return m_RangedIntervals.rbegin()->second;
	}
}

void CRelevanceEvaluators::FillIntervals(CResultArgumentWithIntervals* pResultArgumentWithIntervals)
{
	if(ISNULL(pResultArgumentWithIntervals)) return;
	
	TIntervalCollection* pResultOrtIntervals=new TIntervalCollection;
	pResultArgumentWithIntervals->GetResultIntervalCollection()->push_back(pResultOrtIntervals);

	if(m_RangedIntervals.empty()) return;

	unsigned int uiIntervalsNumber=0;
	TRangedIntervalCollection::iterator itIntervalCollection=m_RangedIntervals.end();
	do{
		itIntervalCollection--;
		if(itIntervalCollection->first>=m_uiIntervalLimitWeight && 
			(uiIntervalsNumber<m_uiIntervalLimitNumber || m_uiIntervalLimitNumber==0)){
			//если проходим ограничения по весовому и количественному порогу, кладем интервалы в общую коллекцию			
			uiIntervalsNumber+=(unsigned int)itIntervalCollection->second->size();
			
			for(unsigned int i=0; i<itIntervalCollection->second->size(); i++){
				pResultOrtIntervals->push_back(itIntervalCollection->second->at(i)->Clone());
			}
		}else{
			//если ограничения не пройдены выходим
			break;
		}
	}while(itIntervalCollection!=m_RangedIntervals.begin());
	
	//если коллекция интервалов все рано пустая, кладем просто лучшие
	if(pResultOrtIntervals->empty()){
		TIntervalCollection* pTempIntervalCollection=GetBestIntervals();
		if(pTempIntervalCollection){
			for(unsigned int i=0; i<pTempIntervalCollection->size(); i++){
				pResultOrtIntervals->push_back(pTempIntervalCollection->at(i)->Clone());
			}
		}
	}
}

bool CRelevanceEvaluators::AddInterval(Containers::CInterval* pInterval)
{
	if(ISNULL(pInterval)) return false;
	
	//ToConsole(L"Input");
	//pInterval->View();

	int Bound=GetFactory()->GetSearchSettings()->m_ResultsSettings.m_uiIntervalJoinRange;
	
	//определяем интервал не меньший левой границе
	CCompareInterval CompareInterval;
	CompareInterval.m_uiHeadPos=max((int)(pInterval->m_uiHeadPos-Bound), 0);
	CompareInterval.m_uiTailPos=max((int)(pInterval->m_uiTailPos-Bound), 0)/*pInterval->m_uiTailPos+Bound*/;
	//ToConsole(L"CompareInterval.m_uiHeadPos", CompareInterval.m_uiHeadPos);
	//ToConsole(L"CompareInterval.m_uiTailPos", CompareInterval.m_uiTailPos);
	//ToConsole(L"m_IntervalsByHeadTail", m_IntervalsByHeadTail.size());

	TIntervalsByHeadTail::iterator itInterval=m_IntervalsByHeadTail.lower_bound(CompareInterval);
	if(itInterval==m_IntervalsByHeadTail.end()){
		//если подходящий интервал не найден, добавляем воходной в коллекцию
		CompareInterval.m_uiHeadPos=pInterval->m_uiHeadPos;
		CompareInterval.m_uiTailPos=pInterval->m_uiTailPos;
		m_IntervalsByHeadTail.insert(TIntervalsByHeadTail::value_type(CompareInterval, pInterval));
		return true;
	}

	//ToConsole(L"xxx");

	//коллекция для инициализации итераторов
	TIntervalsByHeadTail NullCollection;
	//определяем наилучший интервал удовлетворяющий ограничениям по границе и длине
	TIntervalsByHeadTail::iterator itResultInterval=NullCollection.end();
	CInterval Interval, ResultInterval; 
	while(itInterval!=m_IntervalsByHeadTail.end() && 
		itInterval->first.m_uiHeadPos<=pInterval->m_uiHeadPos+Bound){

		//определяем будующий интервал
		Interval.m_uiHeadPos=min(itInterval->second->m_uiHeadPos, pInterval->m_uiHeadPos);
		Interval.m_uiTailPos=max(itInterval->second->m_uiTailPos, pInterval->m_uiTailPos);
		Interval.m_uiWeight=max(itInterval->second->m_uiWeight, pInterval->m_uiWeight);
		
		if(Interval.GetLength()<CRelevanceEvaluator::m_uiMaxIntervalLength){
			//если он проходит по длине
			if(itResultInterval==NullCollection.end()){
				//для первого интервала без проверки
				ResultInterval=Interval;
				itResultInterval=itInterval;
			}
			else
			{
				if (abs((double)itResultInterval->first.m_uiHeadPos-pInterval->m_uiHeadPos) >= 
					abs((double)itInterval->first.m_uiHeadPos-pInterval->m_uiHeadPos))
				{
					//если предыдущий подходящий интервал ближе к добавляемому
					//обновляем результат
					ResultInterval=Interval;
					itResultInterval=itInterval;
				}
			}
		}

		itInterval++;
	}
	
	//обрабатываем результат
	if(itResultInterval!=NullCollection.end()){
		//ToConsole(L"Input");
		//pInterval->View();
		//ToConsole(L"Was");
		//itResultInterval->second->View();
		//ToConsole(L"Updated");
		//ResultInterval.View();

		itResultInterval->second->m_uiHeadPos=ResultInterval.m_uiHeadPos;
		itResultInterval->second->m_uiTailPos=ResultInterval.m_uiTailPos;
		itResultInterval->second->m_uiWeight=ResultInterval.m_uiWeight;
		return false;
	}else{
		//если подходящий интервал не найден, добавляем входной в коллекцию
		CompareInterval.m_uiHeadPos=pInterval->m_uiHeadPos;
		CompareInterval.m_uiTailPos=pInterval->m_uiTailPos;
		m_IntervalsByHeadTail.insert(TIntervalsByHeadTail::value_type(CompareInterval, pInterval));
		return true;
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}
}
