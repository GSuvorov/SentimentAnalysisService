#include "StdAfx.h"
#include ".\prx_eval_hdr.h"

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

Containers::CInterval* CProximityEvaluator_HdrText::CreateInterval(unsigned int uiHeadPos, unsigned int uiTailPos)
{
	if(m_CurrentBoundInterval.m_uiHeadPos<=uiHeadPos && m_CurrentBoundInterval.m_uiTailPos>=uiTailPos){
		//если инетрвал попадает в границы дейстивия текущего заголовка создаем результат
		CIntervalAndHeader* pIntervalAndHeader=new CIntervalAndHeader(uiHeadPos, uiTailPos);
		pIntervalAndHeader->m_HeaderID=m_CurrentBoundInterval.m_HeaderID;
		
		//добавляем вес определяющий насколько близок интервал к началу заголовка
		pIntervalAndHeader->m_uiWeight=GetHeaderDistanceWeight(pIntervalAndHeader->m_uiHeadPos-m_CurrentBoundInterval.m_uiHeadPos);

		//добавляем вес определяющий сколько слов находится в заголовке а сколько в тексте
		pIntervalAndHeader->m_uiWeight+=m_uiCurrentHeadersWordsCount;

		//if(m_uiCurrentDocumentIndex==17383){
		//	pIntervalAndHeader->View();
		//	pIntervalAndHeader->m_HeaderID.View();
		//}
		return pIntervalAndHeader;
	}
	
	return NULL;
};

//--------------------------------------------------------------------//

Containers::CInterval* CProximityEvaluator_HdrTable::CreateInterval(unsigned int uiTableIndex, unsigned int uiRowIndex)
{
	if(ISNULL(m_pIndexStorageTables)) return NULL;

	Containers::STablePositions TablePositions;
	if(!m_bAllWordsInHeader){
		//от табличного вычислителя приходят индекс таблицы и строки
		//по ним определяем позиции таблицы, если таблица подходит по границам к заголовку
		m_pIndexStorageTables->FillTablePositions(m_uiCurrentDocumentIndex, uiTableIndex, &TablePositions, uiRowIndex, uiRowIndex);
	}else{
		//все слова в заголовке сюда передаются позиции конца заголовка
		TablePositions.m_uiHeadPosition=uiTableIndex;
		TablePositions.m_uiTailPosition=uiRowIndex;
		uiTableIndex=EMPTY_VALUE;
		uiRowIndex=EMPTY_VALUE;
	}

	if(m_CurrentBoundInterval.m_uiHeadPos<=TablePositions.m_uiHeadPosition && m_CurrentBoundInterval.m_uiTailPos>=TablePositions.m_uiTailPosition){
		//если инетрвал попадает в границы дейстивия текущего заголовка создаем результат
		CIntervalAndHeader* pIntervalAndHeader=new CIntervalAndHeader(uiTableIndex, uiRowIndex);
		pIntervalAndHeader->m_HeaderID=m_CurrentBoundInterval.m_HeaderID;
		
		//добавляем вес определяющий насколько близок интервал к началу заголовка
		pIntervalAndHeader->m_uiWeight=GetHeaderDistanceWeight(TablePositions.m_uiHeadPosition-m_CurrentBoundInterval.m_uiHeadPos);

		//добавляем вес определяющий сколько слов находится в заголовке а сколько в тексте
		pIntervalAndHeader->m_uiWeight+=m_uiCurrentHeadersWordsCount;
		
		//pIntervalAndHeader->View();
		return pIntervalAndHeader;
	}
	
	return NULL;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}
