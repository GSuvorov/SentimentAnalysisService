#pragma once

#include ".\arg.h"
#include ".\logic.h"

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
class CResultArgument;

//--------------------------------------------------------------------//

///сложное значение, содержит несколько значений по логикам
class CComplexValue : public SS::Core::NdxSE::NdxProcessor::Containers::CValue
{
public:
	typedef vector< pair<SS::Core::NdxSE::NdxProcessor::Extracting::Logics::CLogic*, 
		SS::Core::NdxSE::NdxProcessor::Containers::CValue*> > TLogicValuesCollection;
	
	CComplexValue(void);
	virtual ~CComplexValue(void);

	///добавляет очередную логику к значению
	void AddLogic(SS::Core::NdxSE::NdxProcessor::Extracting::Logics::CLogic* pLogic);
	///заполняет сложное значение по простому
	void FillComplexValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue, 
		CComplexArgument* pComplexArgument);
	///заполняет аргумент-результат
	void FillResultArgument(CResultArgument* pResultArgument);
	///сбрасывает значение на пустое
	void Reset(void);
	///возвращает значение по идентификатору логики
	Containers::CValue* GetValueByLogic(Logics::CLogic::ELogicID LogicID);

	///выполняет операцию в соотвествии с оператором
	static void Calculate(CComplexValue* pChangedValue, CComplexValue* pValue, 
		SS::Core::NdxSE::NdxProcessor::Extracting::Logics::CLogic::EOperator Operator);

	///отображает содержимое объекта класса
	void View(void);
	
protected:
	///коллекция значение логик, доступ к элементам по идентификатору логики
	TLogicValuesCollection m_LogicValuesCollection;
};

//--------------------------------------------------------------------//

///аргумент - хранит сложное значение, используется в CCalcArgument
class CComplexArgument : public CArgument
{
public:
	CComplexArgument(unsigned int uiArgumentID = SS::Core::NdxSE::NdxProcessor::EMPTY_VALUE, float fKoeff = 1.0, SS::Interface::Core::CommonContainers::SQueryIndexAttributes ArgumentAttributes = SS::Interface::Core::CommonContainers::SQueryIndexAttributes());
	virtual ~CComplexArgument(void);

	///устанавливает значение аргумента
	void SetComplexValue(CComplexValue* pValue);
	///устанавливает значение аргумента, на входе простое значение
	/**по нему создается сложное, это функция используется Subster'ом,
	когда он переключает значения аргументов формулы*/
	void SetValue(SS::Core::NdxSE::NdxProcessor::Containers::CValue* pValue);
	///возвращает значение аргумента
	CComplexValue* GetValue(void){return (CComplexValue*)m_pCurrentValue;};
	///заполняет аргумент-результат
	void FillResultArgument(CResultArgument* pResultArgument);

	///возвращает стартовое значение
	Containers::CValue* GetStartValue(void){return m_pStartValue;};
	///возвращает последнее в стеке значение-источник
	Containers::CValue* GetBackSourceValue(void){return m_SourceValuesStack.empty()?NULL:m_SourceValuesStack.back();};
	///выбрасывает из стека крайнее значение
	void PopBackSourceValue(void);
	///кладет в стек значение-источник
	void PushBackSourceValue(Containers::CValue* pValue);
	///проверяет стек значений-источников на пустоту 
	bool IsEmptySourceValuesStack(void){return m_SourceValuesStack.empty();};
	///инитит стек значений-источников, устанавливает стартовое значение
	void InitSourceValuesStack(Containers::CValue* pValue);

	///возвращает идентификатор блока
	unsigned int GetBlockID(void){return m_uiBlockID;};
	///устанавливает идентификатор блока
	void SetBlockID(unsigned int uiValue){m_uiBlockID=uiValue;};

protected:
	typedef list<Containers::CValue*> TValuesStack;

	///стек значений-источников, крайнее значение стека является источником последующих
	TValuesStack m_SourceValuesStack;
	///стартовое значение, значения с которого начинаются вычисления
	Containers::CValue* m_pStartValue;
	///идентификатор блока
	unsigned int m_uiBlockID;
};

//--------------------------------------------------------------------//

}
}
}
}
}