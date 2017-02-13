#include "StdAfx.h"
#include ".\prx_union.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

using namespace SS::Core::NdxSE::NdxProcessor::Extracting;

//--------------------------------------------------------------------//

CWordUnion::CWordUnion(unsigned char ucPositionID, CContainersFactory* pContainersFactory, 
					   unsigned char ucSequenceID, unsigned char ucPosInSequence)
:CWordInSequence(ucPositionID, pContainersFactory, L"union", 0.0, ucSequenceID, ucPosInSequence),
m_uiEvalLimitWeightPart(pContainersFactory->GetSearchSettings()->m_PatternSearchSettings.m_uiUnionLimitWordWeightPart),
m_uiDirtyWordsCount(pContainersFactory->GetSearchSettings()->m_PatternSearchSettings.m_uiUnionSurplusWordsMaxCount),
m_uiDistanceWeightKoef(pContainersFactory->GetSearchSettings()->m_PatternSearchSettings.m_uiUnionDistanceWeightKoef),
m_uiWordsWeightKoef(pContainersFactory->GetSearchSettings()->m_PatternSearchSettings.m_uiUnionWordsWeightKoef),
m_uiUnionWeightKoef(pContainersFactory->GetSearchSettings()->m_PatternSearchSettings.m_uiUnionWeightKoef),
m_uiGrandWordWeightLimit(pContainersFactory->GetSearchSettings()->m_PatternSearchSettings.m_uiGrandWordWeightLimit)
{
}

CWordUnion::~CWordUnion(void)
{
	for(TWordCollection::iterator itWord=m_GrandWordCollection.begin(); itWord!=m_GrandWordCollection.end(); itWord++)
	{
		delete (*itWord);
	}

	for(TWordCollection::iterator itWord=m_SimpleWordCollection.begin(); itWord!=m_SimpleWordCollection.end(); itWord++)
	{
		delete (*itWord);
	}
}

void CWordUnion::AddWord(CWordInSequence* pWord)
{
	if(ISNULL(pWord)) return;

	//pWord->View();

	if(pWord->GetWeight()>=(float)m_uiGrandWordWeightLimit){
		m_GrandWordCollection.push_back(pWord);
	}else{
		m_SimpleWordCollection.push_back(pWord);
	}


	//добавляем к весу
	m_fWordWeight+=pWord->GetWeight();
}

void CWordUnion::AddArgumentPosition(CComplexArgument* pComplexArgument, CCoordinateNode_4s* pCoordinateNode)
{
	for(TWordCollection::iterator itWord=m_GrandWordCollection.begin(); itWord!=m_GrandWordCollection.end(); itWord++)
	{
		(*itWord)->AddArgumentPosition(pComplexArgument, pCoordinateNode);
	}

	for(TWordCollection::iterator itWord=m_SimpleWordCollection.begin(); itWord!=m_SimpleWordCollection.end(); itWord++)
	{
		(*itWord)->AddArgumentPosition(pComplexArgument, pCoordinateNode);
	}
}

void CWordUnion::Clear(void)
{
	for(TWordCollection::iterator itWord=m_GrandWordCollection.begin(); itWord!=m_GrandWordCollection.end(); itWord++)
	{
		(*itWord)->Clear();
	}
	for(TWordCollection::iterator itWord=m_SimpleWordCollection.begin(); itWord!=m_SimpleWordCollection.end(); itWord++)
	{
		(*itWord)->Clear();
	}
	m_CurrentPositions.clear();
}

void CWordUnion::ClearPositions(void)
{
	for(TWordCollection::iterator itWord=m_GrandWordCollection.begin(); itWord!=m_GrandWordCollection.end(); itWord++)
	{
		(*itWord)->ClearPositions();
	}

	for(TWordCollection::iterator itWord=m_SimpleWordCollection.begin(); itWord!=m_SimpleWordCollection.end(); itWord++)
	{
		(*itWord)->ClearPositions();
	}
	m_CurrentPositions.clear();
}

unsigned int CWordUnion::PrepareMakeCommonPositions(unsigned int uiEvalSentenceID)
{
	unsigned int uiRange=0;
	float fTemp=0.0;

	if(!m_GrandWordCollection.empty()){
		//готовим все слова, а ранг берем по наихудшему слову
		for(TWordCollection::iterator itWord=m_GrandWordCollection.begin(); itWord!=m_GrandWordCollection.end(); itWord++)
		{
			unsigned int uiTemp=(*itWord)->PrepareMakeCommonPositions(uiEvalSentenceID);
			if(uiTemp!=0){
				uiRange+=uiTemp;
				///подсчитываем вес участвующих слов
				fTemp+=(*itWord)->GetWeight();
			}
		}

		//если у обязательных слов нет позиций прерываемся
		if(fTemp==0.0) return 0;
	}else{
		ToConsole(L"m_GrandWordCollection is empty!!!");
	}

	//готовим все слова, а ранг берем по наихудшему слову
	for(TWordCollection::iterator itWord=m_SimpleWordCollection.begin(); itWord!=m_SimpleWordCollection.end(); itWord++)
	{
		unsigned int uiTemp=(*itWord)->PrepareMakeCommonPositions(uiEvalSentenceID);
		if(uiTemp!=0){
			uiRange+=uiTemp;
			///подсчитываем вес участвующих слов
			fTemp+=(*itWord)->GetWeight();
		}
	}

	//wchar_t buf[500];
	//swprintf(buf, L"PrepareMakeCommonPositions: FullWeight: %f, CurrWeight: %f, Range: %u, Part: %f", GetWeight(), fTemp, uiRange, fTemp*100/GetWeight());
	//ToConsole(buf);

	if((unsigned int)(fTemp*100/GetWeight())<m_uiEvalLimitWeightPart){
		//если участвующие слова не набрали порога по весу
		return 0;
	}

	uiRange*=GetWordsCount();
	return uiRange;
}

void CWordUnion::MakeCommonPositions(unsigned int uiEvalSentenceID, unsigned int uiLeftBound, unsigned int uiRightBound)
{
	float fTemp=0.0;

	if(!m_GrandWordCollection.empty()){
		for(TWordCollection::iterator itWord=m_GrandWordCollection.begin(); itWord!=m_GrandWordCollection.end(); itWord++)
		{
			//формируем позиции слова
			(*itWord)->MakePositions(uiEvalSentenceID, &m_CurrentPositions, uiLeftBound, uiRightBound);
			if(!(*itWord)->GetCurrentPositions()->empty()){
				///подсчитываем вес участвующих слов
				fTemp+=(*itWord)->GetWeight();
			}
		}


		//если у обязательных слов нет позиций прерываемся
		if(fTemp==0.0 || m_CurrentPositions.empty()){
			ClearPositions();
			return;
		}

		//определяем новые границы для необязательных слов
		unsigned int uiMaxDirtyWordsCount=m_uiDirtyWordsCount+GetWordsCount();

		unsigned int uiNewLeftBound=(*m_CurrentPositions.begin());
		uiNewLeftBound=(uiNewLeftBound>uiMaxDirtyWordsCount?uiNewLeftBound-uiMaxDirtyWordsCount:0);
		unsigned int uiNewRightBound=(*(--m_CurrentPositions.end()))+uiMaxDirtyWordsCount;
		uiLeftBound=max(uiNewLeftBound, uiLeftBound);
		uiRightBound=min(uiNewRightBound, uiRightBound);
	}

	for(TWordCollection::iterator itWord=m_SimpleWordCollection.begin(); itWord!=m_SimpleWordCollection.end(); itWord++)
	{
		//формируем позиции слова
		(*itWord)->MakePositions(uiEvalSentenceID, &m_CurrentPositions, uiLeftBound, uiRightBound);
		if(!(*itWord)->GetCurrentPositions()->empty()){
			///подсчитываем вес участвующих слов
			fTemp+=(*itWord)->GetWeight();
		}
	}

	//wchar_t buf[500];
	//swprintf(buf, L"MakeCommonPositions: uiLeftBound: %u, uiRightBound: %u, FullWeight: %f, CurrWeight: %f, Limit: %u, Part: %f", 
	//	uiLeftBound, uiRightBound, GetWeight(), fTemp, m_uiEvalLimitWeightPart, fTemp*100/GetWeight());
	//ToConsole(buf);

	if((unsigned int)(fTemp*100/GetWeight())<m_uiEvalLimitWeightPart){
		//если участвующие слова не набрали порога по весу
		ClearPositions();
	}
}

float CWordUnion::GetUnionWeight(unsigned int* puiHeadPos, unsigned int* puiTailPos)
{
	if(m_uiCurrentEvalPosition==EMPTY_VALUE) return 0.0;
	
	float fUnionWeight=0.0;
	float fWordsWeight=0.0;

	unsigned int uiMaxDirtyWordsCount=m_uiDirtyWordsCount+GetWordsCount();
	unsigned int uiCurrWordCount=0, uiHeadPos=m_uiCurrentEvalPosition, uiTailPos=m_uiCurrentEvalPosition;
	unsigned int uiLeftBound=m_uiCurrentEvalPosition>=uiMaxDirtyWordsCount?m_uiCurrentEvalPosition-uiMaxDirtyWordsCount:0;
	unsigned int uiRightBound=m_uiCurrentEvalPosition+uiMaxDirtyWordsCount;

	//проходим по словам, получаем для каждого слова позицию ближайшую к текущей
	//определяем длину интервала объединения слов и количество слов объединения в результате
	if(!m_GrandWordCollection.empty()){
		for(TWordCollection::iterator itWord=m_GrandWordCollection.begin(); itWord!=m_GrandWordCollection.end(); itWord++)
		{
			if(!(*itWord)->GetCurrentPositions()->empty()){
				//получаем позицию слова
				unsigned int uiTemp=(*itWord)->GetNearPosition(m_uiCurrentEvalPosition);
				//если позиция входит в границы, учитываем при подсчете веса 
				if(uiLeftBound<=uiTemp && uiRightBound>=uiTemp){
					//сдвигаем либо голову либо хвост интервала
					if(uiTemp<uiHeadPos) uiHeadPos=uiTemp;
					else if(uiTemp>uiTailPos) uiTailPos=uiTemp;
					//считаем количество слов
					uiCurrWordCount++;		
					///подсчитываем вес участвующих слов
					fWordsWeight+=(*itWord)->GetWeight();
				}
			}
		}

		//если обязательные слова не набрали веса общий вес не получицца
		if(fWordsWeight==0.0){
			return fWordsWeight;
		}
	}

	for(TWordCollection::iterator itWord=m_SimpleWordCollection.begin(); itWord!=m_SimpleWordCollection.end(); itWord++)
	{
		if(!(*itWord)->GetCurrentPositions()->empty()){
			//получаем позицию слова
			unsigned int uiTemp=(*itWord)->GetNearPosition(m_uiCurrentEvalPosition);
			//если позиция входит в границы, учитываем при подсчете веса 
			if(uiLeftBound<=uiTemp && uiRightBound>=uiTemp){
				//сдвигаем либо голову либо хвост интервала
				if(uiTemp<uiHeadPos) uiHeadPos=uiTemp;
				else if(uiTemp>uiTailPos) uiTailPos=uiTemp;
				//считаем количество слов
				uiCurrWordCount++;		
				///подсчитываем вес участвующих слов
				fWordsWeight+=(*itWord)->GetWeight();
			}
		}
	}

	//вычисляем вес объединения
	float fDistWeight=0.0;
	fWordsWeight=(fWordsWeight*100/GetWeight());
	fUnionWeight=m_uiWordsWeightKoef*fWordsWeight;

	unsigned int uiUnionIntervalLength=uiTailPos-uiHeadPos+1;
	if(uiCurrWordCount>1 && uiUnionIntervalLength>=uiCurrWordCount){
		fDistWeight=(float)m_uiDistanceWeightKoef*(uiCurrWordCount-1)/(float)(uiUnionIntervalLength-uiCurrWordCount+1);
		fUnionWeight+=fDistWeight;
		if(puiHeadPos) *puiHeadPos=uiHeadPos;
		if(puiTailPos) *puiTailPos=uiTailPos;
	}

	if(fDistWeight==0 && uiCurrWordCount>1) return 0.0;

	fUnionWeight*=m_uiUnionWeightKoef;

	//wchar_t buf[500];
	//swprintf(buf, L"Result: %f, DistWeight: %f, Words: %f, uiIntervalLength: %u, WordsCount: %u, TotalWordsCount: %u", 
	//	fUnionWeight, fDistWeight, fWordsWeight, uiUnionIntervalLength, uiCurrWordCount, GetWordsCount());
	//ToConsole(buf);

	return fUnionWeight;
}

//--------------------------------------------------------------------//

}
}
}
}
}
