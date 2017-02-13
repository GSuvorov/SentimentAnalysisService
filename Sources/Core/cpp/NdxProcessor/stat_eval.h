#pragma once

#include ".\index_stat_storage.h"
#include ".\collection_containers.h"
#include ".\index_container_nodes_s.h"
#include ".\arg_complex.h"
#include ".\arg_res.h"

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

///индекс статистического вычислителя
class CStatisticIndex
{
public:
	CStatisticIndex(unsigned int uiIndex):m_uiIndex(uiIndex), m_fInversDocumentFrequency(0.0), m_pCoordinateNode(NULL){};

	///добавляет позицию к эталонным предложениям
	void AddArgumentPosition(CComplexArgument* pComplexArgument, SS::Core::NdxSE::NdxProcessor::Containers::CCoordinateNode_4s* pCoordinateNode);
	///очищает хранилище координат
	void Clear(void){m_pCoordinateNode=NULL;};

	///возвращает значение индекса
	unsigned int GetIndex(void){return m_uiIndex;};
	///возвращает обратная частота индекса
	float GetInversDocumentFrequency(void){return m_fInversDocumentFrequency;};
	///устанавливает обратная частота индекса
	void SetInversDocumentFrequency(float fValue){m_fInversDocumentFrequency=fValue;};
	///возвращает текущее хранилище координат
	SS::Core::NdxSE::NdxProcessor::Containers::CCoordinateNode_4s* GetCoordinateContainer(void){return m_pCoordinateNode;};

protected:
	///значение индекса
	const unsigned int m_uiIndex;
	///обратная частота индекса
	float m_fInversDocumentFrequency;
	///текущее хранилище координат
	SS::Core::NdxSE::NdxProcessor::Containers::CCoordinateNode_4s* m_pCoordinateNode;
};

//--------------------------------------------------------------------//

///вычислитель статистической релевантности, содержит коллекцию индексов запроса, 
///доступ к индексу по его позиции в запросе
class CStatisticEvaluator
{
public:
	CStatisticEvaluator(SS::Core::NdxSE::NdxProcessor::Storages::CIndexStatisticStorage* pIndexStorage, unsigned int uiTotalTextsNumber);
	virtual ~CStatisticEvaluator(void);

	///добавляет индекс вычислителю
	void AddIndex(unsigned int uiIndex);
	///добавляет позицию к эталонным предложениям
	void AddArgumentPosition(CComplexArgument* pComplexArgument, SS::Core::NdxSE::NdxProcessor::Containers::CCoordinateNode_4s* pCoordinateNode);
	///вычисляет базовый номер текстов данного результата по истории результата
	unsigned int SetDocumentBasisIndex(unsigned int uiValue);
	///вычисляет
	float Evaluate(unsigned int uiEvalID);
	///очищает все
	void Clear(void);
protected:
	///вычисляет обратную встречаемость индекса в тексте
	float CalculateInversDocumentFrequency(unsigned int uiIndex);
	
	typedef map<unsigned int, CStatisticIndex*> TStatisticIndexCollection;
	///интервейс с индексом
	SS::Core::NdxSE::NdxProcessor::Storages::CIndexStatisticStorage* m_pIndexStorage;
	///текущий базовый номер текстов данного результата
	unsigned int m_uiCurrBasisTextsNumber;
	///количество текстов в базе
	const unsigned int m_uiTotalTextsNumber;
	///коллекция индексов
	TStatisticIndexCollection m_StatisticIndexCollection;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}