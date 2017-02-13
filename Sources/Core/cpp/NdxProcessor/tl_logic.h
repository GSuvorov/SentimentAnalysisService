#pragma once
#include ".\f_logic.h"
#include ".\index_stat_storage.h"
#include ".\iep_storage.h"
//#include "../assinterface/tndxsearchstructs.h"
//#include ".\docnormtable.h"

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

///логика уровня текстов учет стастики по текстам
class CTextLevelLogic : public CFuzzyLogic
{
public:
	CTextLevelLogic(unsigned int uiAllTextsNumber, 
		SS::Core::NdxSE::NdxProcessor::Storages::CIndexStatisticStorage* pIndexStorage);

	///образует значение логики по значению аргумента
	SS::Core::NdxSE::NdxProcessor::Containers::CValue* CreateLogicValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue, CComplexArgument* pComplexArgument);
	///образует значение аргумента по значению логики
	void ToResultValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue, CResultArgument* pResultArgument);

protected:
	///вычисляет обратную встречаемость индекса в тексте
	void CalculateInversDocumentFrequency(CComplexArgument* pComplexArgument);
	///вычисляет базовый номер текстов данного результата по истории результата
	unsigned int CalculateTextsBaseNumberFromResultHistory(CResultArgument* pResultArgument);
	///вычисляет обратную частоту встречаемости  терма в коллекции логарифм количества документов в котором встретился данный терм делённый на общее количество документов в коллекции
	float ComputeInversDocumentFrequency(unsigned int uiDocumentsNumber, unsigned int uiDocumentFrequency);


	///количество текстов в базе
	const unsigned int m_uiAllTextsNumber;
	///обратная встречаемость индекса в тексте
	float m_fInversDocumentFrequency;

	///информация по индексированию координат
	SS::Core::NdxSE::NdxProcessor::Storages::CIndexStatisticStorage* m_pIndexStorage;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}