#include "StdAfx.h"

#include ".\r_eval.h"
#include ".\r_evals.h"
#include <math.h>

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

CSequences::CSequences(void)
{
}

CSequences::~CSequences(void)
{
	Clear();
}

void CSequences::AddPosition(unsigned char ucSequenceID, unsigned char ucPosInSequence)
{
	TSequence* pSequence=NULL;
	if(ucSequenceID==(m_SequenceCollection.size()-1)){
		pSequence=m_SequenceCollection.back();
	}else if(ucSequenceID==m_SequenceCollection.size()){
		pSequence=new TSequence;
		m_SequenceCollection.push_back(pSequence);
	}else{
		TO_CONSOLE(L"Sequence ID not correct");
		ToConsole(L"SequenceID", (unsigned int)ucSequenceID);
		return;
	}
	
	if(pSequence && ucPosInSequence==pSequence->size()){
		TIntervalPositions::iterator itIntervalPositions=m_NullCollection.end();
		pSequence->push_back(itIntervalPositions);	
	}else{
		TO_CONSOLE(L"PosInSequence ID not correct");
		ToConsole(L"PosInSequence", (unsigned int)ucPosInSequence);
	}
}

void CSequences::Clear(void)
{
	for(TSequenceCollection::iterator itSequence=m_SequenceCollection.begin(); itSequence!=m_SequenceCollection.end(); itSequence++){
		if((*itSequence)){
			delete (*itSequence);
		}
	}

	m_SequenceCollection.clear();
}

void CSequences::Init(void)
{
	TIntervalPositions::iterator itIntervalPositions=m_NullCollection.end();
	for(TSequenceCollection::iterator itSequence=m_SequenceCollection.begin(); itSequence!=m_SequenceCollection.end(); itSequence++){
		if((*itSequence)){
			TSequence::iterator itPosition=(*itSequence)->begin(); 
			do{
				(*itPosition)=itIntervalPositions;	
			}while(++itPosition!=(*itSequence)->end());
		}
	}
}

CSequences::TSequence* CSequences::GetSequence(SIntervalPos* pIntervalPos)
{
	if(ISNULL(pIntervalPos)) return NULL;
	if(ISNULL(pIntervalPos->m_pCommonPositions)) return NULL;
	
	unsigned int uiSequenceID=pIntervalPos->m_pCommonPositions->GetSequenceID();
	unsigned int uiPosID=pIntervalPos->m_pCommonPositions->GetPosInSequence();
	
	if(uiSequenceID>=m_SequenceCollection.size()){
		TO_CONSOLE(L"Too big sequence ID!!!");
		ToConsole(L"uiSequenceID", uiSequenceID);
		return NULL;
	}

	TSequence* pSequence=m_SequenceCollection[uiSequenceID];
	
	if(uiPosID>=pSequence->size()){
		TO_CONSOLE(L"Too big position ID in sequence!!!");
		ToConsole(L"uiPosID", uiPosID);
		return NULL;
	}

	return pSequence;

}

bool CSequences::CheckOnSequences(TIntervalPositions::iterator* pitIntervalPos)
{
	if(ISNULL(pitIntervalPos)) return false;

	//START_TICK(L"CSequences::CheckOnSequences");

	for(TSequenceCollection::iterator itSequence=m_SequenceCollection.begin(); itSequence!=m_SequenceCollection.end(); itSequence++){
		if((*itSequence) && (*itSequence)->size()>1){
			//переходим к первым валидным позициям
			TSequence::iterator itPositionL=(*itSequence)->begin(); 
			for(;itPositionL!=(*itSequence)->end(); ++itPositionL) if((*itPositionL)!=m_NullCollection.end()) break;
			if(itPositionL==(*itSequence)->end()) continue;

			TSequence::iterator itPositionR=itPositionL+1; 
			for(;itPositionR!=(*itSequence)->end(); ++itPositionR) if((*itPositionR)!=m_NullCollection.end()) break;
			if(itPositionR==(*itSequence)->end()) continue;

			do{
				if((int)((*itPositionR)->second.m_uiIndexPosition-(*itPositionL)->second.m_uiIndexPosition)<=0){
					//предоставляем итератор нарушающий последовательность
					(*pitIntervalPos)=(*itPositionR);
					//STOP_TICK(L"CSequences::CheckOnSequences");
					return false;
				}

				//правая становится левой
				itPositionL=itPositionR;
				//переходим к очередной правой
				++itPositionR;
				for(;itPositionR!=(*itSequence)->end(); ++itPositionR) if((*itPositionR)!=m_NullCollection.end()) break;

			}while(itPositionR!=(*itSequence)->end());
		}
	}
	//STOP_TICK(L"CSequences::CheckOnSequences");

	return true;
}

bool CSequences::IsSequencesBreaked(SIntervalPos* pIntervalPos)
{
	if(ISNULL(pIntervalPos)) return false;
	if(ISNULL(pIntervalPos->m_pCommonPositions)) return false;

	//получаем последовательность для позиции
	TSequence* pSequence=GetSequence(pIntervalPos);
	
	if(ISNULL(pSequence)) return false;

	//если последовательностьс одной позицией проверять не нужно
	if(pSequence->size()==1) return false;

	unsigned int uiPosID=pIntervalPos->m_pCommonPositions->GetPosInSequence();
	//проверяем не нарушает ли данная позиция интервала
	
	//получаем текущую позицию по идентификатору
	TSequence::iterator itPosition=pSequence->begin()+uiPosID;
	if(uiPosID!=0){
		//определяем левого соседа и сравниваем с ним
		TSequence::iterator itPositionL=--itPosition;
		for(;itPositionL!=pSequence->end(); ++itPositionL) if((*itPositionL)!=m_NullCollection.end()) break;

		if((*itPositionL)!=m_NullCollection.end()  && itPositionL!=pSequence->end() && (int)(pIntervalPos->m_uiIndexPosition-(*itPositionL)->second.m_uiIndexPosition)<=0){
			//нарушение последовательности с левым соседом
			return true;
		}
	}

	if(uiPosID<pSequence->size()){
		//определяем правого соседа и сравниваем с ним
		TSequence::iterator itPositionR=++itPosition;
		for(;itPositionR!=pSequence->end(); ++itPositionR) if((*itPositionR)!=m_NullCollection.end()) break;
		if((*itPositionR)!=m_NullCollection.end() && itPositionR!=pSequence->end() && (int)((*itPositionR)->second.m_uiIndexPosition-pIntervalPos->m_uiIndexPosition)<=0){
			//нарушение последовательности с правым соседом
			return true;
		}
	}

	return false;
}

void CSequences::UpdateSequences(SIntervalPos* pIntervalPos, TIntervalPositions::iterator itIntervalPos)
{
	if(ISNULL(pIntervalPos)) return;
	if(ISNULL(pIntervalPos->m_pCommonPositions)) return;

	//получаем последовательность для позиции
	TSequence* pSequence=GetSequence(pIntervalPos);
	if(ISNULL(pSequence)) return;
	
	//обновляем позицию в последовательности
	(*pSequence)[pIntervalPos->m_pCommonPositions->GetPosInSequence()]=itIntervalPos;
}

//--------------------------------------------------------------------//

CRelevanceEvaluator::CRelevanceEvaluator(unsigned int uiEvalID, bool bCheckSequence, CContainersFactory* pContainersFactory)
:CMasterCollection<CWordInSequence>(uiEvalID), m_bCheckSequence(bCheckSequence),
CFactoryClient<CContainersFactory>(pContainersFactory), m_uiEvaluatedChildsNumber(EMPTY_VALUE), 
m_uiGoodIntervalsNumber(0), m_uiDocumentBasisIndex(EMPTY_VALUE), m_pRelevanceEvaluatorsOwner(NULL)
{
}

CRelevanceEvaluator::~CRelevanceEvaluator(void)
{
}

unsigned int CRelevanceEvaluator::GetEvaluatedChildsNumber(void)
{
	if(m_uiEvaluatedChildsNumber==EMPTY_VALUE){
		m_uiEvaluatedChildsNumber=0;
		TChildCollection::iterator itSP=m_ChildCollection.begin();
		while(itSP!=m_ChildCollection.end()){	
			if((*itSP) && (*itSP)->IsEvaluated()){
				++m_uiEvaluatedChildsNumber;
			}
			++itSP;
		}
	}
	return m_uiEvaluatedChildsNumber;
}

CWordInSequence* CRelevanceEvaluator::AddPosition(unsigned char ucPositionID, unsigned char ucSequenceID, unsigned char ucPosInSequence, SS::Interface::Core::NdxSE::IWord* pWord)
{
	if(!pWord) return NULL;

	//добавляем эталонную позицию в предложение
	CWordInSequence* pWordInSequence=
		new CWordInSequence(ucPositionID, GetFactory(), pWord->GetAsString(), pWord->GetLinguisticWeight(), ucSequenceID, ucPosInSequence);

	//устанавливаем слову поисковую необязательность
	pWordInSequence->SetObligatory(pWord->GetObligatoryFlag()==0?false:true);

	//добавляем элемент в коллекцию
	pWordInSequence=AddChildElement(pWordInSequence->GetID(), pWordInSequence); 

	if(ISNULL(pWordInSequence)) return NULL;

	if(m_bCheckSequence){
		m_SequenceCollection.AddPosition(pWordInSequence->GetSequenceID(), pWordInSequence->GetPosInSequence());
	}

	return pWordInSequence;
}

void CRelevanceEvaluator::AddArgumentPosition(CComplexArgument* pComplexArgument, CCoordinateNode_4s* pCoordinateNode)
{
	if(ISNULL(pComplexArgument)) return;
	//AddArgumentPositionByID(pComplexArgument->GetAttributes().m_IndexPosition, pComplexArgument, pCoordinateNode);

	for(unsigned int uiChildID=0; uiChildID<m_ChildCollection.size(); uiChildID++){
		if(m_ChildCollection[uiChildID]){
			AddArgumentPositionByID(uiChildID, pComplexArgument, pCoordinateNode);
		}
	}

}

void CRelevanceEvaluator::AddInterval(unsigned int uiHeadPos, unsigned int uiTailPos)
{
	if(ISNULL(m_pRelevanceEvaluatorsOwner)) return;

	CInterval* pInterval=CreateInterval(uiHeadPos, uiTailPos);
	if(!pInterval) return;

	//определяем вес интервала
	unsigned int uiWeight=GetIntervalWeight(pInterval);
	//pInterval->View();
	
	//если вес не нулевой кладем его в результирующую коллекцию
	if(uiWeight!=0){
		pInterval->m_uiWeight+=uiWeight;
		//считаем количество хороших интервалов
		if(pInterval->m_uiWeight>m_uiGoodIntervalLimitWeight) ++m_uiGoodIntervalsNumber;

		//pInterval->View();
		//m_IntervalCollection.push_back(pInterval);
		if(!m_pRelevanceEvaluatorsOwner->AddInterval(pInterval))
			//если интервал не занесся в результаты его надо удалить
			delete pInterval;
	}else{
		//если интервал не занесся в результаты его надо удалить
		delete pInterval;
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}
}
