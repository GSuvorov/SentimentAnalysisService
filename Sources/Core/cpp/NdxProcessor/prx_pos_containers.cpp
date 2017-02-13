#include "StdAfx.h"
#include ".\prx_pos_containers.h"
#include ".\pos_ba_containers_s.h"
#include ".\index_stat_storage.h"
#include <math.h>

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

CLoggedIndexContainer* CWord::m_pLoggedIndexContainer=NULL;

//--------------------------------------------------------------------//

CWord::CWord(unsigned char ucPositionID, CContainersFactory* pContainersFactory, const wchar_t* wszWord, float fWordWeight)
:CFactoryClient<CContainersFactory>(pContainersFactory),
m_ucPositionID(ucPositionID), m_wsWord(wszWord), m_uiCurrentEvalPosition(EMPTY_VALUE), m_fWordWeight(fWordWeight)
{
}

CWord::~CWord(void)
{
}

void CWord::AddIndex(unsigned int uiIndex)
{
	TIndexPositionsCollection::iterator itIndex=m_IndexCollection.find(uiIndex);
	if(itIndex==m_IndexCollection.end()){
		m_IndexCollection.insert(TIndexPositionsCollection::value_type(uiIndex, (CCoordinateNode_4s*)0));
	}else{
		TO_CONSOLE(L"Index already exist!!!");
	}
}

void CWord::AddArgumentPosition(CComplexArgument* pComplexArgument, CCoordinateNode_4s* pCoordinateNode)
{
	if(ISNULL(pComplexArgument)) return;

	unsigned int uiIndex=pComplexArgument->GetArgumentID();

	TIndexPositionsCollection::iterator itIndex=m_IndexCollection.find(uiIndex);
	if(itIndex!=m_IndexCollection.end()){
		//кладем в коллекцию данного индекса битовую карту позиций
		itIndex->second=pCoordinateNode;
	}
}

unsigned int CWord::PrepareMakeCommonPositions(unsigned int uiEvalSentenceID)
{
	if(!MakeIndexPositionsCollection(uiEvalSentenceID)) return 0;
	
	//удаляем лишние контайнеры
	RemoveEqualPosConataners();
	
	//вычисляем размерный коэффициент
	return EvaluateRangeId();	
}

bool CWord::MakeIndexPositionsCollection(unsigned int uiEvalSentenceID)
{
	if(uiEvalSentenceID==EMPTY_VALUE) return false;
	//ToConsole(L"uiEvalSentenceID", uiEvalSentenceID);
	
	m_IndexPositionsCollection.clear();
	//создаем общую коллекцию контайнеров позиций для индексов данной CWord
	//с учетом допущения "все индексы на позиции по ИЛИ"
	CPosCnt* pCurrPosCnt=NULL;
	TIndexPositionsCollection::iterator itIndexPos=m_IndexCollection.begin(); 
	while(itIndexPos!=m_IndexCollection.end()){
		if(itIndexPos->second){
			//получаем карту по вычисляемому индексу предложения
			pCurrPosCnt=dynamic_cast<CPosCnt*>(itIndexPos->second->MoveUpChildNode(uiEvalSentenceID));
			if(pCurrPosCnt){
				//pCurrPosCnt->View();
				m_IndexPositionsCollection.push_back(pCurrPosCnt);
			}
		}
		itIndexPos++;
	}

	return true;
}

void CWord::RemoveEqualPosConataners(void)
{
	//проходим по коллекции и удаляем одинаковые контайнеры позиций
	TIndexPositionsList::iterator itPosCnt=m_IndexPositionsCollection.begin();
	while(itPosCnt!=m_IndexPositionsCollection.end()){
		
		TIndexPositionsList::iterator itPosCntTemp=itPosCnt;
		itPosCntTemp++;
		while(itPosCntTemp!=m_IndexPositionsCollection.end()){
			if(((CPosBaCntSearch*)(*itPosCnt))->IsEqual((CPosBaCntSearch*)(*itPosCntTemp))){
				itPosCntTemp=m_IndexPositionsCollection.erase(itPosCntTemp);
			}else
				++itPosCntTemp;
		}
	
		++itPosCnt;
	}
}

unsigned int CWord::EvaluateRangeId(void)
{
	if(m_IndexPositionsCollection.empty()) return 0;

	unsigned int uiRangeId=0;
	unsigned int uiTemp=0;
	TIndexPositionsList::iterator itPosCnt=m_IndexPositionsCollection.begin();
	CPosBaCntSearch* pPosCnt=NULL;
	while(itPosCnt!=m_IndexPositionsCollection.end()){
		pPosCnt=(CPosBaCntSearch*)(*itPosCnt);
		//(*itPosCnt)->View();
		//unsigned int uiTemp1=pPosCnt->GetFirstPosition();
		//unsigned int uiTemp2=pPosCnt->GetLastPosition();
		unsigned int uiTemp3=pPosCnt->GetOrtsNumber();
		uiTemp=/*uiTemp2-uiTemp1+*/uiTemp3;
		if(uiTemp==0) ++uiTemp;
		uiRangeId+=uiTemp;
		++itPosCnt;
	}
	uiRangeId/=(unsigned int)m_IndexPositionsCollection.size();

	return uiRangeId;
}
void CWord::MakeCommonPositions(unsigned int uiEvalSentenceID, 
										  unsigned int uiLeftBound, unsigned int uiRightBound)
{
	MakePositions(uiEvalSentenceID, &m_CurrentPositions, uiLeftBound, uiRightBound);
}

void CWord::MakePositions(unsigned int uiEvalSentenceID, TCommonPositions* pResultPositions,
	unsigned int uiLeftBound, unsigned int uiRightBound)
{

	if(uiEvalSentenceID==EMPTY_VALUE) return;
	if(ISNULL(pResultPositions)) return;

	//!!!коллекцию перед формированием не очищаем так, формирование вызывается несколько раз подряд
	//pResultPositions->clear();

	//ToConsole(L"uiEvalSentenceID", uiEvalSentenceID);
	
	//создаем общую коллекцию контайнеров позиций для индексов данной CWord
	//с учетом допущения "все индексы на позиции по ИЛИ"
	CPosBaCntSearch_Blocked* pCurrPosCnt=NULL;
	TIndexPositionsList::iterator itPosCnt=m_IndexPositionsCollection.begin();
	while(itPosCnt!=m_IndexPositionsCollection.end()){
		//получаем карту по вычисляемому индексу предложения
		pCurrPosCnt=(CPosBaCntSearch_Blocked*)(*itPosCnt);
		//pCurrPosCnt->View();
		
		if((uiLeftBound==EMPTY_VALUE && uiRightBound==EMPTY_VALUE ) ||
		((pCurrPosCnt->GetFirstPosition()<=uiLeftBound && pCurrPosCnt->GetLastPosition()>=uiLeftBound) ||
			(pCurrPosCnt->GetFirstPosition()<=uiRightBound && pCurrPosCnt->GetLastPosition()>=uiRightBound)) ||
			(pCurrPosCnt->GetFirstPosition()>=uiLeftBound  && pCurrPosCnt->GetLastPosition()<=uiRightBound)
			){
			//если одна из границ находится в интервале позиций контайнера или границы захватывают интервал
			//, работаем иначе нет смысла

			unsigned int uiPos=0;
			if(uiLeftBound!=EMPTY_VALUE) uiPos=uiLeftBound;
			START_TICK(L"MoveLowerBoundOrt")
			bool bRes=pCurrPosCnt->MoveLowerBoundOrt(uiPos);
			STOP_TICK(L"MoveLowerBoundOrt")

			//ToConsole(L"uiLeftBound", uiLeftBound);
			//ToConsole(L"uiRightBound", uiRightBound);
			//pCurrPosCnt->View();

			START_TICK(L"MoveNextOrt")
			if(bRes)
			do{
				uiPos=pCurrPosCnt->GetCurrentOrt();
	

				//!!!выполняется ТОЛЬКО при проведении верификационного тестирования индексов!!!
				if(m_pLoggedIndexContainer){
					m_pLoggedIndexContainer->CurrentIndexCheckCoordinates(uiEvalSentenceID, uiPos);
				}

				if((uiLeftBound==EMPTY_VALUE && uiRightBound==EMPTY_VALUE ) || 
					(uiPos>=uiLeftBound && uiPos<=uiRightBound)){
					//если позиция входит в заданные границы добаваляем ее
					pResultPositions->insert(uiPos);
					if(pResultPositions!=(&m_CurrentPositions))
						m_CurrentPositions.insert(uiPos);	
				}else if(uiPos>uiRightBound){
					break;
				}
			}while(pCurrPosCnt->MoveNextOrt());
			STOP_TICK(L"MoveNextOrt")
		}
		++itPosCnt;
	}
}

unsigned int CWord::CurrentPositionsPopFront(void)
{
	if(m_CurrentPositions.empty()) return EMPTY_VALUE;

	unsigned int uiPos=(unsigned int)*m_CurrentPositions.begin();
	m_CurrentPositions.erase(m_CurrentPositions.begin());
	return uiPos;
}

unsigned int CWord::GetNearPosition(unsigned int uiCenterPosition)
{
	if(m_CurrentPositions.empty()) return EMPTY_VALUE;

	unsigned int uiRightPos=EMPTY_VALUE;
	unsigned int uiLeftPos=EMPTY_VALUE;

	TCommonPositions::iterator itTemp=m_CurrentPositions.lower_bound(uiCenterPosition);
	if(itTemp!=m_CurrentPositions.end()){
		uiRightPos=*itTemp;
		if(itTemp==m_CurrentPositions.begin()) return uiRightPos;
	}

	--itTemp;
	uiLeftPos=*itTemp;
	
	if(uiRightPos-uiCenterPosition>uiCenterPosition-uiLeftPos){
		return uiLeftPos;
	}else{
		return uiRightPos;
	}
}

void CWord::Clear(void)
{
	TIndexPositionsCollection::iterator itIndexPos=m_IndexCollection.begin(); 
	while(itIndexPos!=m_IndexCollection.end()){
		itIndexPos->second=NULL;
		itIndexPos++;
	}
	m_CurrentPositions.clear();
}

void CWord::ClearPositions(void)
{
	m_CurrentPositions.clear();
}

void CWord::View(void)
{
	wstring szValue;
	wchar_t buf[500];
	szValue.append(L"//---------Bof-CWord\n");
	swprintf(buf, L"Str: %s\n Ind: ", m_wsWord.c_str());
	szValue.append(buf);

	TIndexPositionsCollection::iterator itIndex=m_IndexCollection.begin();
	while(itIndex!=m_IndexCollection.end()){
		wsprintf(buf, L"%u ", (unsigned int)itIndex->first);
		szValue.append(buf);
		itIndex++;
	}
	szValue.append(L"\n//---------Eof-CWord");

	ToConsole(szValue.c_str());
}

//--------------------------------------------------------------------//

CWordInSequence::CWordInSequence(unsigned char ucPositionID, 
	CContainersFactory* pContainersFactory, const wchar_t* wszWord, float fWordWeight, unsigned char ucSequenceID, unsigned char ucPosInSequence)
:CWord(ucPositionID, pContainersFactory, wszWord, fWordWeight), m_ucSequenceID(ucSequenceID), 
m_ucPosInSequence(ucPosInSequence), m_bEvaluated(true), m_Obligatory(false)
{
}

CWordInSequence::~CWordInSequence(void)
{
}

void CWordInSequence::View(void)
{
	wstring szValue;
	wchar_t buf[500];
	szValue.append(L"//---------Bof-CWordInSequence\n");
	swprintf(buf, L"Str: %s\n ID: %u, Seq: %u, PosInSeq: %u, Obl: %u \nInd: ", m_wsWord.c_str(), GetID(), 
		(unsigned int)m_ucSequenceID,  (unsigned int)m_ucPosInSequence, (unsigned int)m_Obligatory);
	szValue.append(buf);

	TIndexPositionsCollection::iterator itIndex=m_IndexCollection.begin();
	while(itIndex!=m_IndexCollection.end()){
		wsprintf(buf, L"%u ", (unsigned int)itIndex->first);
		szValue.append(buf);
		itIndex++;
	}

	szValue.append(L"Positions:\n");
	TCommonPositions::iterator itPos=m_CurrentPositions.begin();
	while(itPos!=m_CurrentPositions.end()){
		wsprintf(buf, L"%u ", (unsigned int)(*itPos));
		szValue.append(buf);
		itPos++;
	}

	szValue.append(L"\n//---------Eof-CWordInSequence");
	ToConsole(szValue.c_str());
}

//--------------------------------------------------------------------//

unsigned int CWordWithStatistic::m_uiCurrentDocumentNormKoef=EMPTY_VALUE;
unsigned int CWordWithStatistic::m_uiTotalDocumentsNumber=EMPTY_VALUE;
unsigned int CWordWithStatistic::m_uiCurrentEvalID=EMPTY_VALUE;

CWordWithStatistic::CWordWithStatistic(unsigned char ucPositionID, CContainersFactory* pContainersFactory, const wchar_t* wszWord, float fWordWeight, bool bAnswerWord)
:CWordInSequence(ucPositionID, pContainersFactory, wszWord, fWordWeight, 0, 0), m_fTotalWordWeight((float)EmptyWeight), 
m_bAnswerWord(bAnswerWord), m_fAbsFrequencyWeight((float)EmptyWeight), 
m_fWordsLingvoWeightKoef(pContainersFactory->GetSearchSettings()->m_SmartSearchSettings.m_fWordsLingvoWeightKoef)
{
}

void CWordWithStatistic::AddIndex(unsigned int uiIndex)
{
	CWord::AddIndex(uiIndex);

	unsigned int uiEntryPoint, uiAbsFrequency, uiTotalWordsCount;
	//получаем частоту встречаемости индекса в коллекции документов
	GetFactory()->GetIndexStorage()->GetIndexEntryPointTable()->GetIndexData(uiIndex, &uiEntryPoint, &uiAbsFrequency);
	//получаем общее слов в коллекции
	uiTotalWordsCount=((Storages::CIndexStatisticStorage*)GetFactory()->GetIndexStorage())->GetDocNormStorage()->GetTotalWordsCount();
	//определяем частотный вес индекса
	float fIndexWeight=((float)uiAbsFrequency/(float)uiTotalWordsCount);
	//нормируем
	fIndexWeight/=GetFactory()->GetSearchSettings()->m_SmartSearchSettings.m_fWordsNormKoef;

	//частоный вес слова берем по максимальному весу индекса
	m_fAbsFrequencyWeight=max(m_fAbsFrequencyWeight, fIndexWeight);
}

float CWordWithStatistic::CalculateIndexIDF(unsigned int uiIndex, unsigned int uiTotalDocumentsNumber)
{
	float fTemp=0.0;
	if(uiIndex==EMPTY_VALUE) return fTemp;
	if(ISNULL(uiTotalDocumentsNumber)) return fTemp;

	//если аргумент не промежуточный, достаем инфу по текстам - частоту встречаемости
	unsigned int uiEntryPoint, uiDocumentFrequency;

	//получаем частоту встречаемости индекса в коллекции документов
	GetFactory()->GetIndexStorage()->GetIndexEntryPointTable()->GetIndexData(uiIndex, &uiEntryPoint, &uiDocumentFrequency);

	if(uiDocumentFrequency && uiTotalDocumentsNumber){
		//если значения нормальные вычисляем обратную встречаемость индекса в тексте
		//ToConsole(L"DocumentFrequency", uiDocumentFrequency);
		fTemp=(float)uiTotalDocumentsNumber/(float)uiDocumentFrequency;
	}

	return fTemp;
}

void CWordWithStatistic::EvaluateWeight(unsigned int uiEvalID)
{
	//вес определеятся:
	//1. частотный вес слова в коллекции
	//2. частоный вес слова в документе.
	//вес тем больше чем реже слов в общей коллекции и чаще в данном документе
    
	if(ISNULL(m_uiCurrentDocumentNormKoef)) return;
	
	//норма документа - это общее количество слов в документе
	float fDocumentWordsCount=(float)(m_uiCurrentDocumentNormKoef);
	float fWordWeightInDocument=0;
	float fIndexWeight=0;

	//проходим по всем индескам слова, получаем их хранилища позиций, вычисляем веса
	TIndexPositionsCollection::iterator itIndex=m_IndexCollection.begin();
	while(itIndex!=m_IndexCollection.end()){
		//вычисляем веса индексов и суммируем их
		float fIndexDocumentFrequency=0.0;
		float fFreqIncreaseKoef=0.0;
		if(itIndex->second!=NULL){
			CPosCnt* pPosCnt=dynamic_cast<CPosCnt*>(itIndex->second->GetChild(uiEvalID));
			if(pPosCnt){
				//получаем частоту индекса в данном документе
				fIndexDocumentFrequency=(float)pPosCnt->GetOrtsNumber();
				
				//получаем частотный вес индекса в документе
				fIndexWeight=fIndexDocumentFrequency/fDocumentWordsCount;
				//нормируем
				fIndexWeight/=GetFactory()->GetSearchSettings()->m_SmartSearchSettings.m_fWordsNormKoef;

				//частотный вес слова в документе, берем по максмальному весу индекса
				fWordWeightInDocument=max(fWordWeightInDocument, fIndexWeight);
			}
		}
		itIndex++;
	}
	
	float fTemp=logf(fWordWeightInDocument+1)*1000;
	if(fTemp<1) fTemp=2;
	float fTemp1=(1/(min(m_fAbsFrequencyWeight+(m_fWordWeight*m_fWordsLingvoWeightKoef), 1)))+1;

	//вычисляем общий вес слова
	//m_fTotalWordWeight=logf(pow(fTemp, logf(fTemp1)));
	m_fTotalWordWeight=logf(fTemp1);

	//wchar_t buf[256];
	//swprintf(buf, L"Word: %u-%s, ResultWeight: %f, WordWeight: %f, FreqWeight: %f, InDocWeight: %f", 
	//	GetID(), GetString(), m_fTotalWordWeight, m_fWordWeight, m_fAbsFrequencyWeight, fWordWeightInDocument);
	//ToConsole(buf);
}

float CWordWithStatistic::GetWeight(void)
{
	if(m_fTotalWordWeight==EmptyWeight){
		if(m_uiCurrentEvalID==EMPTY_VALUE){
			TO_CONSOLE(L"m_uiCurrentEvalID is empty");
			return m_fTotalWordWeight;
		}
		//вычисляем вес слова
		EvaluateWeight(m_uiCurrentEvalID);
	}
	return m_fTotalWordWeight;
}

//--------------------------------------------------------------------//

}
}
}
}
}
