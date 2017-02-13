#pragma once
#include ".\node_containers.h"
#include ".\consoleclient.h"
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
	class CComplexArgument;
}
}
}
}
}

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

///определяет логику операций со значениями
class CLogic : public CConsoleClient
{
public:
	///операторы используемые логикой
	enum EOperator
	{
		///неопределен
		eoUndefined,		
		///"и"
		eoAND,		
		///"или"
		eoOR,		
		///отрицание
		eoNOT,
		///левая скобка
		eoLeftBracket,
		///правая скобка
		eoRightBracket,
		///обнуление
		eoReset,
		///проверка на пустоту
		eoIsEmpty
	};

	///возможные идентификаторы логик
	enum ELogicID
	{
		///неопределеная
		elUndefined,
		///булева логика
		elBoolean,
		///логика расчета релевантности
		elRelevance,
		///логика заголовков
		elHeader,
		///fuzzy логика
		elFuzzy,
		///логика вычисления веса статистики по текстам
		elTextStatistic,
		///логика нечетких операций на уровне предложения
		elSentenceAdjust,
	};

	CLogic(ELogicID LogicID = elUndefined)
		:m_LogicID(LogicID), m_bLogicActive(true), 
		m_uiActivationLevels(0), m_bIdleCalculation(false),
		m_ucLastActivationLevel(0), m_ucActivationLevelsNumber(0){};
	virtual ~CLogic(void){ClearValuesCollection();};

	///выполняет операцию в соотвествии с оператором
	void Calculate(SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppChangedValue, 
		SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppValue, CLogic::EOperator Operator);
	///возвращает идентификатор логики
	ELogicID GetLogicID(void){return m_LogicID;};
	
	///инициализация логики перед вычислениями
	virtual void Init(CResultArgument* pResultArgument);
	///образует значение логики по значению аргумента
	virtual SS::Core::NdxSE::NdxProcessor::Containers::CValue* 
		CreateLogicValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue, 
		SS::Core::NdxSE::NdxProcessor::Extracting::CComplexArgument* pComplexArgument){TO_CONSOLE(L"Not ovveriden"); return NULL;};
	///образует значение аргумента по значению логики
	virtual void ToResultValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue, CResultArgument* pResultArgument){TO_CONSOLE(L"Not ovveriden");};

	///проверяет активность логики
	virtual bool IsActive(void){return m_bLogicActive;};
	///устанавливает уровень активности логики
	void SetActivationLevel(unsigned char ucActivationLevel);
	///устанавливает уровень активности логики
	void SetActivationLevels(unsigned char ucActivationLevelFirst, unsigned char ucActivationLevelLast);
	///проверяет на уровень активности логики
	bool IsActivationLevel(unsigned char ucLevel);
	///возвращает количество уровней активности логики
	unsigned char GetActivationLevelsNumber(void){return m_ucActivationLevelsNumber;};
	///возвращает номер последнего уровня активности логики
	unsigned char GetLastActivationLevel(void){return m_ucLastActivationLevel;};

	///выставляет флаг холостого прогона
	virtual void SetIdleCalculation(void){m_bIdleCalculation=true;};
	///сбрасывает флаг холостого прогона
	virtual void UnsetIdleCalculation(void){m_bIdleCalculation=false;};
	///возвращает флаг холостого прогона
	bool IsIdleCalculation(void){return m_bIdleCalculation;};

	///сериализация в строку
	void ToString(wstring& szValue, wstring szOffset);

protected:
	typedef vector<SS::Core::NdxSE::NdxProcessor::Containers::CValue*> TCreatedValuesCollection;

	///выполняет операцию AND
	virtual void MakeAnd(SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppChangedValue, 
		SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppValue){TO_CONSOLE(L"Not ovveriden");};
	///выполняет операцию OR
	virtual void MakeOr(SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppChangedValue, 
		SS::Core::NdxSE::NdxProcessor::Containers::CValue** ppValue){TO_CONSOLE(L"Not ovveriden");};
	///очищает коллекцию значений созданных логикой
	void ClearValuesCollection(void);
	///очищает коллекцию значений созданных логикой
	void AddToValuesCollection(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue);

	///идентификатор логики
	ELogicID m_LogicID;
	///флаг активности логики
	bool m_bLogicActive;
	///битовая карта уровней активности логики (не больше 32 уровней)
	unsigned int m_uiActivationLevels;
	///номер последнего уровня активности логики 
	unsigned char m_ucLastActivationLevel;
	///количество уровней активности логики 
	unsigned char m_ucActivationLevelsNumber;
	///флаг холостого прогона
	bool m_bIdleCalculation;
	///коллекция значений созданных логикой
	TCreatedValuesCollection m_CreatedValuesCollection;
};

//--------------------------------------------------------------------//

}
}
}
}
}
}