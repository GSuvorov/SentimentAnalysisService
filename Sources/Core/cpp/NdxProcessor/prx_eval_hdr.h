#pragma once

#include ".\prx_eval.h"
#include ".\prx_eval_tbl.h"
#include ".\res_containers.h"
#include ".\hdr_containers.h"
#include ".\index_storage_hdr.h"
#include ".\index_storage_tbl.h"

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

//--------------------------------------------------------------------//

///вычислитель близости расположения с учетом результатов поиска в заголовках
///содержит ссылку на результат поиска по заголовкам
///вычисляет близость согласно этим результататам и дерева заголовков документа
///в вычислениях использует базовый вычислитель в зависимости от параметра
template <class TBaseEvaluator>
class CProximityEvaluator_Hdr : public TBaseEvaluator
{
public:
	CProximityEvaluator_Hdr(Containers::CFindResultCollection* pFindResultCollection, 
			Storages::CIndexStorageHeaders* pIndexStorageHeaders, 
			unsigned int uiEvaluatorID, 
			bool bCheckSequence, 
			SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory* pContainersFactory) :
		TBaseEvaluator(uiEvaluatorID, bCheckSequence, pContainersFactory),
		m_pFindResultCollection(pFindResultCollection), m_pHeadersTree(NULL), 
		m_pIndexStorageHeaders(pIndexStorageHeaders), m_uiCurrentHeadersWordsCount(0), 
		m_uiCurrentDocumentIndex(0), m_bAllWordsInHeader(false)
	{
	};

	virtual ~CProximityEvaluator_Hdr(void)
	{
		if(m_pHeadersTree) delete m_pHeadersTree;
	
		TIndexPositions::iterator itIndexPositions=m_IndexPositions.begin();
		while(itIndexPositions!=m_IndexPositions.end()){
			delete itIndexPositions->second;
			itIndexPositions++;
		}

	}

	///вычисляет вес по близости и порядку расположения индексов в предложении
	void Evaluate(unsigned int uiEvalID)
	{
		if(ISNULL(m_pIndexStorageHeaders)) return;
		if(ISNULL(m_pFindResultCollection)) return;
		
		m_uiCurrentDocumentIndex=m_uiDocumentBasisIndex+uiEvalID;
		//ToConsole(L"m_uiCurrentDocumentIndex", m_uiCurrentDocumentIndex);
		
		int a=0;
		if(m_uiCurrentDocumentIndex==43660){
			a=43660;
		}

		//получаем соответствующий результат поиска
		CFindResult* pFindResult=m_pFindResultCollection->GetFindResult(m_uiCurrentDocumentIndex);
		if(ISNULL(pFindResult)) return;

		//строим дерево заголовков документа
		FillHeadersTree(pFindResult);

		//обходим дерево, получаем ветви заголовков, определяем по нижним заголовкам необходимые
		//границы для расчета близости, определяем какие слова, должны находится в интервале, 
		//расчитываем близость с учетом определенных ограничений
		if(m_pHeadersTree->MoveFirstBranch()){

			SHeaderPositions HeaderPositions;
			CHeaderWords<CHeaderLogic::MaxBlockCount> TotalHeaderWords;
			CHeaderNode* pLowerHeaderNode=NULL; 

			do{
				//получаем идентификаторы слов текущей ветки и крайний узел ветки
				pLowerHeaderNode=m_pHeadersTree->GetCurrentLastHeaderAndWordsIDs(&TotalHeaderWords);
				if(!pLowerHeaderNode) break;

				ResetEvaluatedChildsNumber();

				//определяем какие слова должны участовать в вычислении близости
				m_bAllWordsInHeader=true;
				TChildCollection::iterator itSP=m_ChildCollection.begin();
				unsigned int i=0;
				m_uiCurrentHeadersWordsCount=0;
				while(itSP!=m_ChildCollection.end()){	
					if((*itSP)){
						(*itSP)->SetEvaluated(!TotalHeaderWords.IsWordExist(i));
						if(TotalHeaderWords.IsWordExist(i)) ++m_uiCurrentHeadersWordsCount;
						else m_bAllWordsInHeader=false;
					}
					++itSP; ++i;
				}
				
				if(m_uiCurrentHeadersWordsCount!=0){
					//заполняем границы по младшему заголовку
					m_pIndexStorageHeaders->FillHeaderPositions(m_uiCurrentDocumentIndex, 
						pLowerHeaderNode->GetMyself()->GetHeaderID(), &HeaderPositions);
					
					//устанавливаем границы и идентификатор нижнего заголовка
					m_CurrentBoundInterval.m_uiHeadPos=HeaderPositions.m_uiTailPosition;
					m_CurrentBoundInterval.m_uiTailPos=HeaderPositions.m_uiBoundPosition;
					CLevelHeaderID* pHeaderID=pLowerHeaderNode->GetMyself()->GetHeaderID();
					m_CurrentBoundInterval.m_HeaderID=(*pHeaderID);
					
					if(m_bAllWordsInHeader){
						//если все слова в заголовке, добавляем результирующий интервал принадлежащий заголовку
						AddInterval(m_CurrentBoundInterval.m_uiHeadPos, m_CurrentBoundInterval.m_uiHeadPos);
					}else{
						//вычисляем близость задданых слов в заданных границах
						TBaseEvaluator::Evaluate(uiEvalID);
					}
				}
			}while(m_pHeadersTree->MoveNextBranch());
		}else{
			//если заголовков нет, выполняем обычный поиск
			m_CurrentBoundInterval.m_uiHeadPos=0;
			m_CurrentBoundInterval.m_uiTailPos=EMPTY_VALUE;

			//вычисляем близость задданых слов в заданных границах
			TBaseEvaluator::Evaluate(uiEvalID);
		}
	}

protected:

	///возвращает вес интервала
	unsigned int GetIntervalWeight(Containers::CInterval* pInterval)
	{
		if(m_bAllWordsInHeader){
			//если все слова в заголвке валидную возвращаем релевантность
			//чтобы интервал не пропал
			return 100;
		}else{
			return TBaseEvaluator::GetIntervalWeight(pInterval);
		}
	}

	//возвращает вес определяющий насколько близок интервал к началу заголовка
	unsigned char GetHeaderDistanceWeight(unsigned int uiDistanceToHeader)
	{
		//вес интервала зависит от разницы фактичской длины и идеальной длины
		//иначе это количество промежуточных (незначащих) позиций интервала

		if((int)uiDistanceToHeader<0 || uiDistanceToHeader>m_uiMaxIntervalLength) return 0;
		
		//предел 128 грязных позиций
		if(uiDistanceToHeader==0)
			return 100;
		else
		if(uiDistanceToHeader<=2)
			return 98;
		else
		if(uiDistanceToHeader<=5)
			return 90;
		else
		if(uiDistanceToHeader<=7)
			return 85;
		else
		if(uiDistanceToHeader<=10)
			return 70;
		else
		if(uiDistanceToHeader<=12)
			return 65;
		else
		if(uiDistanceToHeader<=20)
			return 55;
		else
		if(uiDistanceToHeader<=40)
			return 30;
		else
		if(uiDistanceToHeader<=60)
			return 20;
		else
		if(uiDistanceToHeader<=80)
			return 10;
		else
		if(uiDistanceToHeader<=100)
			return 5;
		else
		if(uiDistanceToHeader<=m_uiMaxIntervalLength)
			return 2;
		else
			return 0;
	}
	
	///заполняет коллекцию индексов с позициями
	void FillIndexPositions(void)
	{
		if(m_IndexPositions.empty()){
			//перебираем эталонные позиции слов, получаем индексы слов, заносим индексам соответствующие позиции 
			TChildCollection::iterator itSP=m_ChildCollection.begin();
			Containers::CWord::TIndexPositionsCollection* pIndexesCollection=NULL;
			while(itSP!=m_ChildCollection.end()){	
				if((*itSP)){
					pIndexesCollection=(*itSP)->GetIndexesCollection();
					Containers::CWord::TIndexPositionsCollection::iterator itIndex=pIndexesCollection->begin();		
					while(itIndex!=pIndexesCollection->end()){
						//получаем индекс
						TIndexPositions::iterator itIndexPositions=m_IndexPositions.find(itIndex->first);
						if(itIndexPositions==m_IndexPositions.end()){
							itIndexPositions=m_IndexPositions.insert(TIndexPositions::value_type(itIndex->first, new TPositions())).first;
						}

						//кладем индексу позицию
						itIndexPositions->second->push_back((*itSP)->GetID());
					
						itIndex++;
					}
				}
				++itSP;
			}
		}
	}

	///заполняет дерево заголовков
	void FillHeadersTree(Containers::CFindResult* pFindResult)
	{
		if(ISNULL(pFindResult)) return;
		
		FillIndexPositions();

		if(m_pHeadersTree) delete m_pHeadersTree;

		//строим дерево заголовков документа
		m_pHeadersTree=new CHeadersTree(pFindResult->GetTextIndex());

		//перебираем индексы вычислетеля, получаем по ним, коллекци идентификаторов заголовков
		//из результата, добавляем к дереву позицию индекса (идентификатор слова) и коллекцию HID
		TIndexPositions::iterator itIndexPositions=m_IndexPositions.begin();
		CFindResult::THIDCollection* pHIDCollection=NULL;
		while(itIndexPositions!=m_IndexPositions.end()){
			//получаем коллекцию идентификаторов заголовков по индексу
			pHIDCollection=pFindResult->GetHIDCollection(itIndexPositions->first);
			if(pHIDCollection){
				m_pHeadersTree->AddNodes(itIndexPositions->second, pHIDCollection);
			}
			itIndexPositions++;
		}
		
		//if(m_uiCurrentDocumentIndex==17383)
		//m_pHeadersTree->View();
	}

	///коллекция результатов поиска
	Containers::CFindResultCollection* m_pFindResultCollection;
	///индексное хранилище заголовков
	Storages::CIndexStorageHeaders* m_pIndexStorageHeaders;

	///текущее дерево заголовков
	Containers::CHeadersTree* m_pHeadersTree;
	///текущий ограничительный интервал, результирующие интервалы не должны выходить за его границы
	Containers::CIntervalAndHeader m_CurrentBoundInterval;
	///количество слов в текущей ветке заголовков
	unsigned int m_uiCurrentHeadersWordsCount;
	///текущий индекс документа
	unsigned int m_uiCurrentDocumentIndex;
	///флаг того что все слова в заголовке
	bool m_bAllWordsInHeader;

	typedef vector<unsigned int> TPositions;
	typedef map<unsigned int, TPositions*> TIndexPositions;
	///коллекция индексов с позициями в трансформациях, актуальными для данного экземпляра вычислителя
	///в другом позиции индексов могут отличаться
	TIndexPositions m_IndexPositions;
};

//--------------------------------------------------------------------//

///вычислитель близости слов в текстах с учетом заголовков
class CProximityEvaluator_HdrText : public CProximityEvaluator_Hdr<CProximityEvaluator_Text>
{
public:
	CProximityEvaluator_HdrText(Containers::CFindResultCollection* pFindResultCollection, Storages::CIndexStorageHeaders* pIndexStorageHeaders, unsigned int uiEvaluatorID, bool bCheckSequence, SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory* pContainersFactory)
		:CProximityEvaluator_Hdr<CProximityEvaluator_Text>(pFindResultCollection, pIndexStorageHeaders, uiEvaluatorID, bCheckSequence, pContainersFactory){};

protected:
	///создает интервал результата
	Containers::CInterval* CreateInterval(unsigned int uiHeadPos, unsigned int uiTailPos);
};

//--------------------------------------------------------------------//

///вычислитель близости слов в текстах с учетом заголовков
class CProximityEvaluator_HdrTable : public CProximityEvaluator_Hdr<CProximityEvaluator_Table>
{
public:
	CProximityEvaluator_HdrTable(
		Storages::CIndexStorageTables* pIndexStorageTables,
		Containers::CFindResultCollection* pFindResultCollection, 
		Storages::CIndexStorageHeaders* pIndexStorageHeaders, 
		unsigned int uiEvaluatorID, bool bCheckSequence, 
		Containers::CContainersFactory* pContainersFactory)
		:CProximityEvaluator_Hdr<CProximityEvaluator_Table>(pFindResultCollection, pIndexStorageHeaders, uiEvaluatorID, bCheckSequence, pContainersFactory), m_pIndexStorageTables(pIndexStorageTables){};

protected:
	///создает результат
	Containers::CInterval* CreateInterval(unsigned int uiTableIndex, unsigned int uiRowIndex);
	
	///индексное хранилище таблиц
	Storages::CIndexStorageTables* m_pIndexStorageTables;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}
