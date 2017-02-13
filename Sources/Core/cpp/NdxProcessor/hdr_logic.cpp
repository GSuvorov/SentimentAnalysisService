#include "StdAfx.h"

#include ".\hdr_logic.h"
#include ".\arg_complex.h"

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

using namespace SS::Interface::Core::NdxSE;
using namespace SS::Interface::Core::CommonContainers;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;
using namespace SS::Core::NdxSE::NdxProcessor::Extracting;
using namespace SS::Core::NdxSE::NdxProcessor::Storages;

//--------------------------------------------------------------------//

CHeaderLogic::CHeaderLogic(void)
:CLogic(CLogic::elHeader)
{
}

CHeaderLogic::~CHeaderLogic(void)
{
	TIndexCollection::iterator itIndex=m_IndexCollection.begin();
	while(itIndex!=m_IndexCollection.end()){
		delete itIndex->second;
		itIndex++;
	}
	m_IndexCollection.clear();
}

void CHeaderLogic::SetQuery(IParametrizedQuery* pPrmQueryResult)
{
	if(ISNULL(pPrmQueryResult)) return;
}

void CHeaderLogic::Init(CResultArgument* pResultArgument)
{
	CLogic::Init(pResultArgument);
	
	if(IsActive()){
		TIndexCollection::iterator itIndex=m_IndexCollection.begin();
		while(itIndex!=m_IndexCollection.end()){
			itIndex->second->clear();
			itIndex++;
		}
	}
}


CValue* CHeaderLogic::CreateLogicValue(CValue* pValue, CComplexArgument* pComplexArgument)
{
	try{

		if(IsIdleCalculation()) return NULL;

		//логику проверяем на активность
		if(!IsActive()) return NULL;
		
		//добавляем значения блока
		if(pValue){
			TIndexCollection::iterator itIndex=m_IndexCollection.find(pComplexArgument->GetArgumentID());
			while(itIndex==m_IndexCollection.end()){
				itIndex=m_IndexCollection.insert(TIndexCollection::value_type(pComplexArgument->GetArgumentID(), new TCoordinateNodeCollection())).first;
			}
			itIndex->second->push_back(dynamic_cast<CCoordinateNode_4s*>(pValue));
		}

		return NULL;
	}
	catch(...)
	{
		ERR_TO_CONSOLE(erTryCatch, L"CHeaderLogic::CreateLogicValue failed!!!");
		throw;
	}
}

void CHeaderLogic::ToResultValue(CValue* pValue, CResultArgument* pResultArgument)
{
	try{
		if(IsIdleCalculation()){ 
			return;
		}

		//логику проверяем на активность
		if(!IsActive()) return ;

		//ToConsole(L"CHeaderLogic::ToResultValue");
		
		if(ISNULL(pResultArgument)) return;
		if(ISNULL(pResultArgument->GetMyself())) return;
		if(ISNULL(pResultArgument->GetMyself()->GetValue())) return;
		
		START_TICK(L"CHeaderLogic::ToResultValue");
		
		//получаем значение результата
		CCompositeValue* pCompositeValue=pResultArgument->GetMyself()->GetValue();
		//pCompositeValue->View();
		
		CResultArgument_HID* pResultArgument_HID=static_cast<CResultArgument_HID*>(pResultArgument);
		//получаем веса ортов значения
		CArgumentEx::TValueOrtsWeightsCollection* 
			pValueOrtsWeightsCollection=pResultArgument->GetMyself()->GetValueOrtsWeights();

		//выставляем веса ортам значения
		unsigned int uiEvalID=0;
		if(pCompositeValue->MoveFirstOrt()){
			do{
				//получаем идентификатор для вычисления
				uiEvalID=pCompositeValue->GetCurrentOrt();
				//ToConsole(L"EvalID", uiEvalID);	
				
				//наполняем аргумент результатами
				FillResult(uiEvalID, pResultArgument_HID);

			}while(pCompositeValue->MoveNextOrt());
		}

		STOP_TICK(L"CHeaderLogic::ToResultValue");
	}
	catch(...)
	{
		ERR_TO_CONSOLE(erTryCatch, L"CHeaderLogic::ToResultValue failed!!!");
		throw;
	}

}

void CHeaderLogic::FillResult(unsigned int uiEvalID, CResultArgument_HID* pResultArgument_HID)
{
	if(ISNULL(pResultArgument_HID)) return;

	pResultArgument_HID->AddResultBlocks();

	//в цикле проходим по блокам перебираем их значения
	//получаем идентификаторы заголовков и заносим их в результат с идентификатором блока
	CPosBaCntSearch_Blocked* pHeaderCnt=NULL;
	TIndexCollection::iterator itIndex=m_IndexCollection.begin();
	while(itIndex!=m_IndexCollection.end()){
		TCoordinateNodeCollection::iterator itCoordinateNode=itIndex->second->begin();
		while(itCoordinateNode!=itIndex->second->end()){
			pHeaderCnt=dynamic_cast<CPosBaCntSearch_Blocked*>((*itCoordinateNode)->MoveUpChildNode(uiEvalID));
			if(pHeaderCnt){
				//если контайнер не пустой заносим заголовки, в коллекцию
				if(pHeaderCnt->MoveFirstOrt())
				do{
					pResultArgument_HID->AddIndexHID(itIndex->first, pHeaderCnt->GetCurrentOrt());
				}while(pHeaderCnt->MoveNextOrt());
			}
			++itCoordinateNode;
		}
		itIndex++;
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}
}
