#include "StdAfx.h"
#include ".\arg_calc.h"
#include ".\eq_calc.h"
#include ".\index_container_nodes_s.h"

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
 
using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;
using namespace SS::Core::NdxSE::NdxProcessor::Extracting::Logics;

//--------------------------------------------------------------------//

CCalcArgument::CCalcArgument(CCalcEquation* pOwnerEquation, unsigned int uiArgumentID, CLogic::EOperator Operator, float fKoeff, SS::Interface::Core::CommonContainers::SQueryIndexAttributes ArgumentAttributes)
:CBinTreeNode<CComplexArgument*>(new CComplexArgument(uiArgumentID, fKoeff, ArgumentAttributes)), 
m_Operator(Operator), m_pOwnerEquation(pOwnerEquation)
{
	if(ISNULL(m_pOwnerEquation)) return;
	
}

CCalcArgument::~CCalcArgument(void)
{
}

void CCalcArgument::Calculate(CResultArgument* pResult)
{
	//if(ISNULL(pResult)) return;
	if(ISNULL(m_pMyself)) return;
	
	//инитимся левым ребенком
	if(GetLeftChild()){
		GetLeftChild()->Calculate(pResult);	
		if(m_pMyself && IsArgumentIntermediate() && GetLeftChild()->GetMyself()){
			//если аргумент промежуточный то после вычисления он сбрасывается
			m_pMyself->SetComplexValue(GetLeftChild()->GetMyself()->GetValue());

			if(GetLeftChild()->IsArgumentIntermediate()){
				GetLeftChild()->GetMyself()->SetComplexValue(NULL);
			}
		}else{
			TO_CONSOLE(L"Equation filling error!!!");
		}
	}else{
		//m_pMyself->GetValue()->View();
	}

	//вычисляемся с правым ребенком
	if(GetRightChild()){
		GetRightChild()->Calculate(pResult);	
		if(GetRightChild()->GetMyself()){
			//проводим вычисления в соответствии с оператором
			CComplexValue::Calculate(
				m_pMyself->GetValue(), 
				GetRightChild()->GetMyself()->GetValue(), 
				m_Operator);

			if(GetRightChild()->IsArgumentIntermediate()){
				//если аргумент промежуточный то после вычисления он сбрасывается
				GetRightChild()->GetMyself()->SetComplexValue(NULL);
			}else{
				//для непромежуточного аргумента сбрасываем содержимое значения
				if(GetRightChild()->GetMyself()->GetValue())
					GetRightChild()->GetMyself()->GetValue()->Reset();
				//GetRightChild()->GetMyself()->GetValue()->View();
			}
		}
	}
	
	//теперь в m_pMyself лежит результат вычислений
	//if(m_pMyself->GetValue())
	//	m_pMyself->GetValue()->View();

	if(IsRoot() && m_pMyself){
		//если аргумент рутовый заполняем результат
		m_pMyself->FillResultArgument(pResult);
		//сбрасываем вычисленное значение
		m_pMyself->GetValue()->Reset();
		m_pMyself->SetComplexValue(NULL);
		//pResult->GetValue()->View();
	}
}

CCalcArgument* CCalcArgument::AddChild(CCalcArgument* pChildArgument)
{
	if(ISNULL(pChildArgument)) return NULL;
	
	if(!m_pLeftChild && !m_pRightChild){
		//если левый и правый узел чисты делаем левым ребенком
		return (CCalcArgument*)CBinTreeNode<CComplexArgument*>::AddChild(pChildArgument, true);
	}

	if(m_pLeftChild && !m_pRightChild){
		//если левый занят а правый узел чист делаем правым ребенком
		return (CCalcArgument*)CBinTreeNode<CComplexArgument*>::AddChild(pChildArgument, false);
	}

	//в остальных случаях ниче не делаем
	return NULL;
}

CLogic::EOperator CCalcArgument::GetOperator(wstring& szValue)
{
	if(szValue.empty()) return CLogic::eoUndefined;

	switch(szValue[0]){
	case '&':
		return CLogic::eoAND;
		break;
	case '|':
		return CLogic::eoOR;
		break;
	case '~':
		return CLogic::eoNOT;
		break;
	case '(':
		return CLogic::eoLeftBracket;
		break;
	case ')':
		return CLogic::eoRightBracket;
		break;
	default:
		return CLogic::eoUndefined;
		break;
	}
}

bool CCalcArgument::IsOperatorPriorityNotHigher(wstring& szValue)
{
	return IsOperatorPriorityNotHigher(GetOperator(szValue));
}

bool CCalcArgument::IsOperatorPriorityNotHigher(CLogic::EOperator Operator)
{
	//у рутового узла приоритет всегда ниже
	if(IsRoot()) return false;

	switch(m_Operator){
	case CLogic::EOperator::eoLeftBracket:
		return false;
		break;
	case CLogic::eoNOT:
		return (Operator!=CLogic::eoLeftBracket);
		break;
	case CLogic::eoAND:
		return (Operator!=CLogic::eoLeftBracket && Operator!=CLogic::eoNOT);
		break;
	case CLogic::eoOR:
		return (Operator!=CLogic::eoLeftBracket && Operator!=CLogic::eoNOT && Operator!=CLogic::eoAND);
		break;
	case CLogic::EOperator::eoRightBracket:
		return true;
		break;
	default:
		return (Operator!=CLogic::eoUndefined);
		break;
	}

}

CCalcArgument* CCalcArgument::InsertCalcArgument(unsigned int uiArgumentID, CLogic::EOperator Operator, float fWeight, SS::Interface::Core::CommonContainers::SQueryIndexAttributes Attributes, CCalcArgument** ppInsertedArgument)
{
	if(ISNULL(m_pOwnerEquation)) return NULL;

	CCalcArgument* pInsertedArgument=NULL;
	CCalcArgument* pRetArgument=NULL;

	if(Operator==CLogic::eoUndefined){
		//если входной оператор не определен
		if(!ChildsOccupied()){
			//простое добавление ребенка к аргументу,
			//если есть место в детях создаем ребенка 
			pInsertedArgument=AddChild(new CCalcArgument(m_pOwnerEquation, uiArgumentID, CLogic::eoUndefined, fWeight, Attributes));
			//создаем пустое сложное значение
			pInsertedArgument->GetMyself()->SetComplexValue(m_pOwnerEquation->GetEquationLogic()->CreateComplexValue());
			pRetArgument=this;
		}else{
			//если места нет добавлять аргумент без оператора никак нельзя
			pInsertedArgument=NULL;
			pRetArgument=NULL;
		}
	}else{
		//если входной оператор определен
		if(m_Operator==CLogic::eoUndefined){
			//если оператор аргумента не определен (то есть у него нет зависимых узлов)
			//просто добавляем новый узел и выставляем аргументу его оператор 
			SetOperator(Operator);
			pInsertedArgument=AddChild(new CCalcArgument(m_pOwnerEquation, uiArgumentID, CLogic::eoUndefined, fWeight, Attributes));
			
			if(uiArgumentID==SS::Core::NdxSE::NdxProcessor::EMPTY_VALUE){
				pRetArgument=pInsertedArgument;
			}else{
				//создаем пустое сложное значение
				pInsertedArgument->GetMyself()->SetComplexValue(m_pOwnerEquation->GetEquationLogic()->CreateComplexValue());
				pRetArgument=this;
			}
		}else{
			//если оператор аргумента определен 
			//придется манипулировать узлами

			//создаем промежуточный аргумент
			CCalcArgument* pTempArgument=new CCalcArgument(m_pOwnerEquation);

			//если у аргумента уже есть оператор
			//проверям приоритет оператора аргумента и токена источника
			if(IsOperatorPriorityNotHigher(Operator)){
				//если приоритет оператора аргумента выше или тот же чем у токена источника
				
				//отдаем ему детей текущего аргумента вместе с оператором
				pTempArgument->AddChild(GetLeftChild());

				//делаем правым ребенком текущий токен источника, если он не пустой
				if(uiArgumentID!=EMPTY_VALUE){
					pInsertedArgument=pTempArgument->AddChild(new CCalcArgument(m_pOwnerEquation, uiArgumentID, CLogic::eoUndefined, fWeight, Attributes));
					//создаем пустое сложное значение
					pInsertedArgument->GetMyself()->SetComplexValue(m_pOwnerEquation->GetEquationLogic()->CreateComplexValue());
				}else{
					pTempArgument->AddChild(GetRightChild());
					pInsertedArgument=NULL;
				}

				pTempArgument->SetOperator(GetOperator());
				RemoveChilds();

				//делаем промежуточный аргумент ребенком текущего
				AddChild(pTempArgument);
				//текущему аргументу выставляем оператор токена источника
				SetOperator(Operator);
				//текущий аргумент так и остается текущим								
				pRetArgument=this;
			}else{
				//если приоритет оператора аргумента ниже или такой же как у токена источника
				//делаем левым ребенком текущий токен источника, если он не пустой
				if(uiArgumentID!=EMPTY_VALUE){
					pInsertedArgument=pTempArgument->AddChild(new CCalcArgument(m_pOwnerEquation, uiArgumentID, CLogic::eoUndefined, fWeight, Attributes));
					//создаем пустое сложное значение
					pInsertedArgument->GetMyself()->SetComplexValue(m_pOwnerEquation->GetEquationLogic()->CreateComplexValue());
				}else{
					pTempArgument->AddChild(GetRightChild());
					pInsertedArgument=NULL;
				}

				//промежуточному аргументу выставляем оператор токена источника
				pTempArgument->SetOperator(Operator);
				//делаем промежуточный аргумент правым ребенком текущего
				RemoveRightChild();
				AddChild(pTempArgument);
				//промежуточный аргумент становится текущим
				pRetArgument=pTempArgument;
			}
		}
	}
	
	if(ppInsertedArgument)
		(*ppInsertedArgument)=pInsertedArgument;

	return pRetArgument;
}

CCalcArgument* CCalcArgument::InsertCalcArgument(wstring* pszArgument, float fWeight, SS::Interface::Core::CommonContainers::SQueryIndexAttributes Attributes)
{
	/*
	Правила формирования древовидной формулы:
	1. Формула состоит из аргументов (чисел, идентификаторов) и промежуточных аргументов 
	(промежуточный результат вычисления двух аргументов 
	в соответствии с данной операцией - содержит внутри себя оператор)
	2. Операторы заданы в соответствии с перечислением CLogic::EOperator
	3. Возвращаемый фокус может быть только на промежуточном аргументе
	4. Аргумент всегда добавляется к текущему промежуточному ребенком
	5. Промежуточный аргумент к текущему промежуточному добавляется в соотвествии с приоритетом своего оператора
	6. Чем выше приоритет операции промежуточного аргумента, тем ниже он должен находится в дереве формулы
	(вычисления идут снизу вверх, т.е. он будет вычислен раньше)
	7. Левая скобка создает пустой промежуточный аргумент, имеет наивысший приоритет на остальными
	8. Правая скобка перемещает фокус наверх на ближайший родитель промеужточного аргумента с левой скобкой 
	(и как бы закрывает скобку, ниже опускаеться больше нельзя)
	*/

	if(ISNULL(pszArgument)) return this;
	if(pszArgument->empty()) return this;

	unsigned int uiArgumentID=EMPTY_VALUE;
	CLogic::EOperator Operator;

	//определяем тип токена
	Operator=GetOperator(*pszArgument);
	if(Operator==CLogic::EOperator::eoUndefined){
		//если это не оператор считаем что на входе аргумент
		//приводим его к числу
		uiArgumentID=_wtoi(pszArgument->c_str());
		//добавляем его к текущему узлу
		if(!ChildsOccupied()){
			//простое добавление ребенка к аргументу,
			//если есть место в детях создаем ребенка 
			CCalcArgument* pChildArgument=AddChild(new CCalcArgument(m_pOwnerEquation, uiArgumentID, CLogic::eoUndefined, fWeight, Attributes));
			//создаем пустое сложное значение
			pChildArgument->GetMyself()->SetComplexValue(m_pOwnerEquation->GetEquationLogic()->CreateComplexValue());
		}else{
			//если места нет это ошибка
			TO_CONSOLE(L"CCalcArgument ChildsOccupied Error!!!");
		}
		return this;
	}else if(Operator==CLogic::EOperator::eoRightBracket){
		//если правая скобка, просто смещаем фокус наверх на левую скобку
		CCalcArgument* pTempArgument=this;
		while(pTempArgument->GetOperator()!=CLogic::EOperator::eoLeftBracket){
			pTempArgument=pTempArgument->GetParent();
			if(!pTempArgument){
				TO_CONSOLE(L"Expression error RightBracket without LeftBracket");
				pTempArgument=this;
				break;
			}
		}

		pTempArgument->SetOperator(CLogic::EOperator::eoRightBracket);
		return pTempArgument->GetParent();

	}else if(Operator==CLogic::EOperator::eoLeftBracket){
		//если левая скобка, просто добавляем ее к текущему аргументу и переводим фокус
		//создаем промежуточный аргумент
		CCalcArgument* pTempArgument=new CCalcArgument(m_pOwnerEquation, uiArgumentID, Operator, fWeight, Attributes);
		////создаем пустое сложное значение
		//pTempArgument->GetMyself()->SetComplexValue(m_pOwnerEquation->GetEquationLogic()->CreateComplexValue());

		AddChild(pTempArgument);

		return pTempArgument;
	}else{
		//если это оператор вставляем его в формулу в соответствии с его приоритетом
		
		//создаем промежуточный аргумент
		CCalcArgument* pTempArgument=new CCalcArgument(m_pOwnerEquation, uiArgumentID, Operator, fWeight, Attributes);
		////создаем пустое сложное значение
		//pTempArgument->GetMyself()->SetComplexValue(m_pOwnerEquation->GetEquationLogic()->CreateComplexValue());

		//определяем приоритет оператора на текущим
		if(IsOperatorPriorityNotHigher(Operator)){
			//приоритет входного оператора не выше
			//он становится родителем текущего аргумента, делая его своим левым ребенком


			CCalcArgument* pParentArgument=GetParent();
			CCalcArgument* pLeftChildArgument=pParentArgument->GetLeftChild();
			CCalcArgument* pRightChildArgument=pParentArgument->GetRightChild();
			if(pRightChildArgument==this){ 
				pParentArgument->RemoveChilds();
				pParentArgument->AddChild(pLeftChildArgument);
				pParentArgument->AddChild(pTempArgument);
				pTempArgument->AddChild(pRightChildArgument);	
			}else if(pLeftChildArgument==this && !pRightChildArgument){
				pParentArgument->RemoveChilds();
				pParentArgument->AddChild(pTempArgument);
				pTempArgument->AddChild(pLeftChildArgument);	
			}else{
				TO_CONSOLE(L"Impossible case");
				delete pTempArgument;
				return this;
			}


		}else{
			//приоритет входного оператора выше
			//он становится правым ребенком текущего аргумнета, делая своим левым ребенком, его бывшего правого
			
			CCalcArgument* pLeftChildArgument=GetLeftChild();
			CCalcArgument* pRightChildArgument=GetRightChild();
			RemoveChilds();
			if(pRightChildArgument){ 
				AddChild(pLeftChildArgument);
				pTempArgument->AddChild(pRightChildArgument);	
			}else if(pLeftChildArgument){
				pTempArgument->AddChild(pLeftChildArgument);	
			}

			AddChild(pTempArgument);
		}
		
		return pTempArgument;
	}

	return this;
}

bool CCalcArgument::IsSpare(void)
{
	if(IsArgumentIntermediate() && (GetOperator()==CLogic::eoUndefined || GetOperator()==CLogic::eoRightBracket)){
		//если аргумент промежуточный и с непределенным оператором
		if(!(GetLeftChild() && GetRightChild())){
			//если у аргумента меньше двух детей
			//он лишний!!!
			return true;
		}
	}

	return false;
}

void CCalcArgument::OptimizeCalcEquation(void)
{
	//запускаем отпимизацию левой ветки
	if(GetLeftChild()){
		GetLeftChild()->OptimizeCalcEquation();	
	}

	//запускаем отпимизацию правой ветки
	if(GetRightChild()){
		GetRightChild()->OptimizeCalcEquation();	
	}
	
	if(IsSpare() && !IsRoot()){
		//если данный аргумент лишний, он удаляет себя
		CCalcArgument* pChild=NULL;
		CCalcArgument* pParent=GetParent();
		
		if(GetLeftChild())
			pChild=GetLeftChild();
		else if(GetRightChild())
			pChild=GetRightChild();

		//отдаем родителю своего ребенка
		if(pParent->GetLeftChild()==this){
			pParent->m_pLeftChild=pChild;
		}else if(pParent->GetRightChild()==this){
			pParent->m_pRightChild=pChild;
		}
		
		//удаляем себя
		RemoveChilds();
		delete this;
	}
}

void CCalcArgument::ToString(wstring& szValue, wstring szOffset)
{
	szOffset.append(L" ");	
	szValue.append(L"\n");
	szValue.append(szOffset);
	szValue.append(L"|-->");

	if(!m_pParent)
		szValue.append(L"root ");
	else
		m_pMyself->ToString(szValue);

	switch(m_Operator){
	case CLogic::eoAND:
		szValue.append(L"Op-AND");
		break;
	case CLogic::eoOR:
		szValue.append(L"Op-OR");
		break;
	case CLogic::eoNOT:
		szValue.append(L"Op-NOT");
		break;
	case CLogic::eoLeftBracket:case CLogic::eoRightBracket:
		szValue.append(L"Op-BR");
		break;
	default:
		szValue.append(L"Op-UDF");
		break;
	}

	if(m_pLeftChild)
		((CCalcArgument*)(m_pLeftChild))->ToString(szValue, szOffset);
	if(m_pRightChild)
		((CCalcArgument*)(m_pRightChild))->ToString(szValue, szOffset);
}

//--------------------------------------------------------------------//

CArgumentsGroup::~CArgumentsGroup(void)
{
	for(unsigned int i=0; i<m_ArgumentCollection.size(); i++){
		delete m_ArgumentCollection[i];
	}
	m_ArgumentCollection.clear();
}

float CArgumentsGroup::GetWeight(void)
{
	if(!m_ArgumentCollection.empty()){
		return m_fTotalArgWeight/m_ArgumentCollection.size();
	}
	return 0;
}

CComplexArgument* CArgumentsGroup::AddArgument(unsigned int uiArgumentID, float fWeight, SS::Interface::Core::CommonContainers::SQueryIndexAttributes Attributes)
{
	if(ISNULL(m_pEquationLogic)) return NULL;
	
	//ToConsole(L"BlockID", GetBlockID());
	//ToConsole(L"ArgumentID", uiArgumentID);

	CComplexArgument* pComplexArgument=new CComplexArgument(uiArgumentID, fWeight, Attributes);
	pComplexArgument->SetBlockID(GetID());
	pComplexArgument->SetComplexValue(m_pEquationLogic->CreateComplexValue());
	m_ArgumentCollection.push_back(pComplexArgument);
	m_fTotalArgWeight+=fWeight;

	return pComplexArgument;
}


void CArgumentsGroup::Calculate(unsigned int uiEvalID, CComplexValue* pResultValue)
{
	if(ISNULL(pResultValue)) return;
	
	CComplexArgument* pComplexArgument=NULL;
	Containers::CCoordinateNode_4s* pCoordinateNode=NULL;
	Containers::CValue* pChildNode=NULL;

	//подливаем к входному значению все по ИЛИ
	for(unsigned int i=0; i<m_ArgumentCollection.size(); i++){
		pComplexArgument=m_ArgumentCollection[i];
		pChildNode=NULL;

		//получаем порождающее значение
		if(pComplexArgument->IsEmptySourceValuesStack()){
			//если стек значений пустой имеем первую итерацию 
			//текущее значени использутеся при вычислении
			pChildNode=pComplexArgument->GetStartValue();
		}else{
			//если стек значений не пустой, получаем крайнее значение стека
			pCoordinateNode=dynamic_cast<CCoordinateNode_4s*>(pComplexArgument->GetBackSourceValue());
			//подымаем дочернее значение
			pChildNode=dynamic_cast<CValue*>(pCoordinateNode->MoveUpChildNode(uiEvalID));
		}

		if(pChildNode){
			//если требуемый узел есть вычисляем

			//устанавливаем аргументу поднятое значение
			pComplexArgument->SetValue(pChildNode);
			//ToConsole(L"ArgumentID", pComplexArgument->GetArgumentID());
			//pComplexArgument->GetValue()->GetValueByLogic(Logics::CLogic::ELogicID::elBoolean)->View();
			//вычисляем
			CComplexValue::Calculate(pResultValue, pComplexArgument->GetValue(), Logics::CLogic::eoOR);
		}

		//значение которое участовало в вычисления кладем в стек, оно станет следующим пораждающим
		pComplexArgument->PushBackSourceValue(pChildNode);
	}
}

void CArgumentsGroup::SynhronizeArguments(void)
{
	//перебираем аргументы кладем в стек пустые значения
	for(unsigned int i=0; i<m_ArgumentCollection.size(); i++){
		m_ArgumentCollection[i]->PushBackSourceValue(NULL);
	}
}

void CArgumentsGroup::ToString(wstring& szValue)
{
	szValue.clear();
	wchar_t buf[100];

	swprintf(buf, L"Block: \n\tID:%u \n\tWeight: %.3f \n\tObl: %u \n\t", GetID(), GetWeight(), (unsigned int)GetObligatory());
	szValue.append(buf);

	for(unsigned int i=0; i<m_ArgumentCollection.size(); i++){
		wsprintf(buf, L"%u ", m_ArgumentCollection[i]->GetArgumentID());
		szValue.append(buf);
	}

	szValue.append(L"\n");
}

//--------------------------------------------------------------------//

CCalcBlock::~CCalcBlock(void)
{
	TArgGroupCollection::iterator itGroup=m_ArgGroupCollection.begin();
	while(itGroup!=m_ArgGroupCollection.end()){
		delete itGroup->second;
		++itGroup;
	}

	TArgGroupList::iterator itNotOblGroup=m_NotOblArgGroupCollection.begin();
	while(itNotOblGroup!=m_NotOblArgGroupCollection.end()){
		delete (*itNotOblGroup);
		++itNotOblGroup;
	}
}

void CCalcBlock::AddGroup(CArgumentsGroup* pArgumentsGroup)
{
	if(ISNULL(pArgumentsGroup)) return;

	if(!pArgumentsGroup->GetObligatory()){
		m_NotOblArgGroupCollection.push_back(pArgumentsGroup);
		return;
	}

	float fWeight=pArgumentsGroup->GetWeight();
	if(fWeight==0.0) return;
	
	unsigned int uiWeight=(unsigned int)(fWeight*1000);

	TArgGroupCollection::iterator itGroup=m_ArgGroupCollection.find(uiWeight);
	while(!(itGroup==m_ArgGroupCollection.end())){
		itGroup=m_ArgGroupCollection.find(++uiWeight);
	}
	
	//wchar_t buf[100];
	//swprintf(buf, L"\nIDW: %u\n", uiWeight);
	//ToConsole(buf);

	//добавляем блок в коллекцию
	if(!m_ArgGroupCollection.insert(TArgGroupCollection::value_type(uiWeight, pArgumentsGroup)).second){
		TO_CONSOLE(L"Cann't insert block");
	};
}

void CCalcBlock::Calculate(unsigned int uiEvalID, CComplexValue* pResultValue)
{
	if(ISNULL(pResultValue)) return;
	if(ISNULL(m_pEquationLogic)) return;

	//ToConsole(L"uiEvalID", uiEvalID);
	
	CComplexValue* pValue=m_pEquationLogic->CreateComplexValue();
	TArgGroupCollection::iterator itGroup=m_ArgGroupCollection.begin();
	Containers::CValue* pTempValue=NULL;
	bool bCalculationBreaked=false;
	while(itGroup!=m_ArgGroupCollection.end()){
		if(!bCalculationBreaked){
			//если вычисленияя не прерваны вычисляем значение блока
			itGroup->second->Calculate(uiEvalID, pValue);

			//itGroup->second->View();
			//pValue->GetValueByLogic(Logics::CLogic::ELogicID::elBoolean)->View();

			//сливаем полученное с общим значением
			if(itGroup==m_ArgGroupCollection.begin())		
				CComplexValue::Calculate(pResultValue, pValue, Logics::CLogic::eoOR);
			else
				CComplexValue::Calculate(pResultValue, pValue, Logics::CLogic::eoAND);

			//проверяем текущий результат, если он пустой дальше вычислять не имеет смысла
			pTempValue=pResultValue->GetValueByLogic(Logics::CLogic::ELogicID::elBoolean);
			if(!pTempValue || pTempValue->IsEmpty()){
				//прерываем вычисления
				bCalculationBreaked=true;
			}

			pValue->Reset();
		}else{
			//если вычисления прерваны, дополняем стеки аргументов блока пустыми значения
			//для синхронизации
			itGroup->second->SynhronizeArguments();
		}
		
		++itGroup;
	}

	TArgGroupList::iterator itNotOblGroup=m_NotOblArgGroupCollection.begin();
	while(itNotOblGroup!=m_NotOblArgGroupCollection.end()){
		if(!bCalculationBreaked){
			//если вычисленияя не прерваны вычисляем значение блока
			pValue->Reset();
			(*itNotOblGroup)->Calculate(uiEvalID, pValue);
			
			if(m_ArgGroupCollection.size()==0){
				//если в блоке нет обязательных аргументов, формируем результат из необязательных
				//сливаем полученное с общим значением
				CComplexValue::Calculate(pResultValue, pValue, Logics::CLogic::eoOR);
			}
		}else{
			//если вычисления прерваны, дополняем стеки аргументов блока пустыми значения
			//для синхронизации
			(*itNotOblGroup)->SynhronizeArguments();
		}
		++itNotOblGroup;
	}

	//ToConsole(L"Result:");
	//pResultValue->GetValueByLogic(Logics::CLogic::ELogicID::elBoolean)->View();
}

void CCalcBlock::SynhronizeArguments(void)
{
	TArgGroupCollection::iterator itGroup=m_ArgGroupCollection.begin();
	while(itGroup!=m_ArgGroupCollection.end()){
		//если вычисления прерваны, дополняем стеки аргументов блока пустыми значения
		//для синхронизации
		itGroup->second->SynhronizeArguments();
		++itGroup;
	}

	TArgGroupList::iterator itNotOblGroup=m_NotOblArgGroupCollection.begin();
	while(itNotOblGroup!=m_NotOblArgGroupCollection.end()){
		(*itNotOblGroup)->SynhronizeArguments();
		++itNotOblGroup;
	}

}

void CCalcBlock::ToString(wstring& wszValue)
{
	wstring wsBlockValue;
	wszValue.clear();
	wszValue.append(L"//------Bof-CCalcBlock\n");

	TArgGroupCollection::iterator itGroup=m_ArgGroupCollection.begin();
	while(itGroup!=m_ArgGroupCollection.end()){
		wchar_t buf[100];
		swprintf(buf, L"\nIDW: %u\n", itGroup->first);
		wszValue.append(buf);
		itGroup->second->ToString(wsBlockValue);
		wszValue.append(wsBlockValue);

		++itGroup;
	}

	wszValue.append(L"\nNot Obligatory Blocks:\n");
	TArgGroupList::iterator itNotOblGroup=m_NotOblArgGroupCollection.begin();
	while(itNotOblGroup!=m_NotOblArgGroupCollection.end()){
		(*itNotOblGroup)->ToString(wsBlockValue);
		wszValue.append(wsBlockValue);
		++itNotOblGroup;
	}

	wszValue.append(L"//------Eof-CCalcBlock\n");
}

//--------------------------------------------------------------------//

}
}
}
}
}