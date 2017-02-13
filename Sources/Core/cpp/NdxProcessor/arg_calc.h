#pragma once

#include ".\arg.h"
#include ".\logic.h"
#include ".\node_containers.h"
#include ".\arg_complex.h"
#include ".\lgc_usings.h"

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

class CCalcEquation;
 
//--------------------------------------------------------------------//

///аргумент предназначеный для вычислений логических выражений
class CCalcArgument : public SS::Core::NdxSE::NdxProcessor::Containers::CBinTreeNode<CComplexArgument*>
{
public:
	CCalcArgument(CCalcEquation* pOwnerEquation, 
		unsigned int uiArgumentID = SS::Core::NdxSE::NdxProcessor::EMPTY_VALUE, 
		Logics::CLogic::EOperator Operator = Logics::CLogic::eoUndefined, 
		float fKoeff = 1.0, 
		SS::Interface::Core::CommonContainers::SQueryIndexAttributes ArgumentAttributes = SS::Interface::Core::CommonContainers::SQueryIndexAttributes());
	virtual ~CCalcArgument(void);

	///вычисляет значение детей 
	void Calculate(CResultArgument* pResult);
	///добавляет ребенка данному узлу
	CCalcArgument* AddChild(CCalcArgument* pChildArgument);
	///возвращает родительский узел
	CCalcArgument* GetParent(void){return ((CCalcArgument*)(SS::Core::NdxSE::NdxProcessor::Containers::CBinTreeNode<CComplexArgument*>::GetParent()));};
	///возвращает левого ребенка
	CCalcArgument* GetLeftChild(void){return ((CCalcArgument*)(SS::Core::NdxSE::NdxProcessor::Containers::CBinTreeNode<CComplexArgument*>::GetLeftChild()));};
	///возвращает правого ребенка
	CCalcArgument* GetRightChild(void){return ((CCalcArgument*)(SS::Core::NdxSE::NdxProcessor::Containers::CBinTreeNode<CComplexArgument*>::GetRightChild()));};

	///устанавливает оператор аргумента
	void SetOperator(Logics::CLogic::EOperator Operator){m_Operator=Operator;};
	///устанавливает оператор аргумента по строке
	void SetOperator(wstring& szValue){SetOperator(GetOperator(szValue));};
	///возвращает оператор аргумента
	Logics::CLogic::EOperator GetOperator(void){return m_Operator;};
	///возвращает оператор из строке
	Logics::CLogic::EOperator GetOperator(wstring& szValue);
	///сравнение приоритета оператора аргумента со входным
	bool IsOperatorPriorityNotHigher(Logics::CLogic::EOperator Operator);
	///сравнение приоритета оператора аргумента со входным
	bool IsOperatorPriorityNotHigher(wstring& szValue);
	///вставляет аргумент в дерево
	CCalcArgument* InsertCalcArgument(unsigned int uiArgumentID, Logics::CLogic::EOperator Operator, float fWeight, SS::Interface::Core::CommonContainers::SQueryIndexAttributes Attributes, CCalcArgument** ppInsertedArgument);
	///вставляет аргумент в дерево
	CCalcArgument* InsertCalcArgument(wstring* pszArgument, float fWeight = 1.0, SS::Interface::Core::CommonContainers::SQueryIndexAttributes Attributes = SS::Interface::Core::CommonContainers::SQueryIndexAttributes());
	///проверяет аргумент на промежуточный
	bool IsArgumentIntermediate(void){return (m_pMyself->GetArgumentID()==SS::Core::NdxSE::NdxProcessor::EMPTY_VALUE)?true:false;};
	///проверяет является ли аргумент лишним
	bool IsSpare(void);
	///оптимизирует своих детей-аргументов
	void OptimizeCalcEquation(void);

	///сериализация в строку
	void ToString(wstring& szValue, wstring szOffset);

protected:
	///оператор по которому сливаются дети
	Logics::CLogic::EOperator m_Operator;
	//формула владелец аргумента
	CCalcEquation* m_pOwnerEquation;
};

//--------------------------------------------------------------------//

///группа аргументов, содержит коллекцию аргментов
///при вычислении значения аргументов объединяются по ИЛИ
class  CArgumentsGroup : public CConsoleClient
{
public:
	CArgumentsGroup(unsigned int uiID, Logics::CCompositeLogic* pEquationLogic, bool bObligatory)
		:m_pEquationLogic(pEquationLogic), m_fTotalArgWeight(0.0), m_uiID(uiID), m_bObligatory(bObligatory){};
	~CArgumentsGroup(void);

	///возвращает вес группы
	float GetWeight(void); 
	///добавляет аргумент в группу
	CComplexArgument* AddArgument(unsigned int uiArgumentID, float fWeight, SS::Interface::Core::CommonContainers::SQueryIndexAttributes Attributes);
	///вычисляет значение группы аргументов
	void Calculate(unsigned int uiEvalID, CComplexValue* pResultValue);
	///выполняет работу по синхронизаци аргументов группы, используется в формуле
	void SynhronizeArguments(void);
	///проверяет на пустоту
	bool IsEmpty(void){return m_ArgumentCollection.empty();};
	///возвращает идентификатор группы
	unsigned int GetID(void){return m_uiID;};
	///возвращает флаг обязательности группы
	bool GetObligatory(void){return m_bObligatory;};

	///сериализация в строку
	void ToString(wstring& szValue);

protected:
	typedef vector<CComplexArgument*> TArgumentCollection;

	///логика используемая в вычислениях
	Logics::CCompositeLogic* m_pEquationLogic;
	///коллекция аргументов группы
	TArgumentCollection m_ArgumentCollection;
	///общий вес аргументнов
	float m_fTotalArgWeight;
	///идентификатор группы
	const unsigned int m_uiID;
	///флаг обязательности группы
	const bool m_bObligatory;
};

//--------------------------------------------------------------------//

///вычислительный блок, содержит группы аргументов сливает их по И
///группа обладяет весом который определяет его положение среди дргуих групп,
///и поэтому очередность при вычислении 
class CCalcBlock : public CConsoleClient
{
public:
	CCalcBlock(Logics::CCompositeLogic* pEquationLogic)
		:m_pEquationLogic(pEquationLogic){};
	virtual ~CCalcBlock(void);
	
	///добавляет группу аргументов в блок
	void AddGroup(CArgumentsGroup* pArgumentsGroup);
	///возвращает логику используемую в вычислениях
	Logics::CCompositeLogic* GetEquationLogic(void){return m_pEquationLogic;};

	///вычисляет значение группы аргументов
	void Calculate(unsigned int uiEvalID, CComplexValue* pResultValue);
	///выполняет работу по синхронизаци аргументов
	void SynhronizeArguments(void);

	///сериализация в строку
	void ToString(wstring& wszValue);

protected:
	typedef map<unsigned int, CArgumentsGroup*> TArgGroupCollection;
	typedef list<CArgumentsGroup*> TArgGroupList;
	
	///логика используемая в вычислениях
	Logics::CCompositeLogic* m_pEquationLogic;
	///коллекция групп формулы
	TArgGroupCollection m_ArgGroupCollection;
	///коллекция не обязательнных групп формулы
	TArgGroupList m_NotOblArgGroupCollection;
};

//--------------------------------------------------------------------//

}
}
}
}
}