#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\proximityevaluator_test.h"
#include ".\test_const.h"
#include ".\console.h"
#include ".\storage_manager.h"
#include ".\arg_res_int.h"
#include ".\arg_complex.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxProcessor::Extracting::CProximityEvaluator_test);

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Extracting
{

using namespace SS::Core::NdxSE::NdxProcessor::Extracting;
using namespace SS::Core::NdxSE::NdxProcessor::Extracting::Logics;
using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Core::NdxSE::NdxProcessor::Containers;
using namespace SS::Core::NdxSE::NdxProcessor::Storages;
using namespace SS::Interface::Core::CommonContainers;
using namespace SS::Interface::Core::NdxSE;

//--------------------------------------------------------------------//

void CProximityEvaluator_test::Test(void)
{
	UT_STOP_LOGING
	
	//получаем менеджер индексной базы
	INdxBaseManager* pNdxBaseManager=(INdxBaseManager*)GetLoadManager()->
	GetInterface(L"./core/NdxBaseManager.dll", CLSID_NdxBaseManager, IID_NdxBaseManager);
	
	pNdxBaseManager->ResetNdxBaseStructure();
	//добавляем базе индекс с нужной структурой
	pNdxBaseManager->AddNdxBaseUnit(L"TEXT", escText_WC);	

	//устанавливаем путь к базе
	pNdxBaseManager->SetBasePath(GetUTWorkingPath());
	//открываем базу
	pNdxBaseManager->OpenBase(L"test_prx_base");

	//создаем менеджер хранилищ
	CStorageManager* pStorageManager=new CStorageManager();
	pStorageManager->Init(pNdxBaseManager);
	pStorageManager->SetExtractingMode();
	
	FillQueryResult();

	//UT_START_LOGING
	
	CContainersFactory* pContainersFactory=pStorageManager->GetIndexStorage(L"TEXT")->GetContainersFactory();

	CRelevanceLogic* pRelevanceLogic=new CRelevanceLogic();
	pRelevanceLogic->SetActivationLevel(0);
	//устанавливаем учет близости расположения слов
	pRelevanceLogic->UseTextProximity(pContainersFactory, true, 0, 0);
	//устанавливаем логике запрос
	pRelevanceLogic->SetQuery(GetPrmQueryResult());
	CResultArgumentWithIntervals* pResultArgument=new CResultArgumentWithIntervals(0, 0.0);
	pResultArgument->GetMyself()->SetValue(pContainersFactory->CreateCompositeValue(0, 0));
	pResultArgument->GetMyself()->GetValue()->SetOrt(0);

	pRelevanceLogic->Init(pResultArgument);

	FillProximityEvaluator(pRelevanceLogic, pContainersFactory);
	
	pRelevanceLogic->ToResultValue(NULL, pResultArgument);


	delete pResultArgument;
	delete pRelevanceLogic;

	//удаляем менеджер хранилищ
	if(pStorageManager) delete pStorageManager;

	//закрываем базу
	pNdxBaseManager->CloseBase();

	UT_START_LOGING
}

void CProximityEvaluator_test::FillProximityEvaluator(CRelevanceLogic* pRelevanceLogic, CContainersFactory* pContainersFactory)
{
	if(ISNULL(pRelevanceLogic)) return;
	if(ISNULL(pContainersFactory)) return;

	//SQueryIndexAttributes QueryIndexAttributes;
	//QueryIndexAttributes.m_TransformationID=0;
	//QueryIndexAttributes.m_IndexPosition=0;
	//CComplexArgument* pComplexArgument=NULL;
	//CSearchBmNode_W* pBitMapNode_W=NULL;
	//CPosCnt* pPosCnt=NULL;
	//unsigned int uiQueryIndex;

	////--------------------------
	//uiQueryIndex=41;
	//QueryIndexAttributes.m_IndexPosition++;
	//pComplexArgument=new CComplexArgument(uiQueryIndex, 0.0, QueryIndexAttributes);
	//pBitMapNode_W=new CSearchBmNode_W(0xff, 0, 0, pContainersFactory);
	//pBitMapNode_W->SetOrt(0);
	//pPosCnt=pBitMapNode_W->AddPosCnt(0);
	//pPosCnt->SetOrt(1);
	//pPosCnt->SetOrt(14);
	//pPosCnt->SetOrt(26);
	//pPosCnt->SetOrt(53);
	////pBitMapNode_W->View();
	//pRelevanceLogic->CreateLogicValue(pBitMapNode_W, pComplexArgument);
	////--------------------------

	////--------------------------
	//uiQueryIndex=35;
	//QueryIndexAttributes.m_IndexPosition++;
	//pComplexArgument=new CComplexArgument(uiQueryIndex, 0.0, QueryIndexAttributes);
	//pBitMapNode_W=new CSearchBmNode_W(0xff, 0, 0, pContainersFactory);
	//pBitMapNode_W->SetOrt(0);
	//pPosCnt=pBitMapNode_W->AddPosCnt(0);
	//pPosCnt->SetOrt(10);
	//pPosCnt->SetOrt(38);
	//pPosCnt->SetOrt(87);
	////pBitMapNode_W->View();
	//pRelevanceLogic->CreateLogicValue(pBitMapNode_W, pComplexArgument);
	////--------------------------

	////--------------------------
	//uiQueryIndex=190;
	//QueryIndexAttributes.m_IndexPosition++;
	//pComplexArgument=new CComplexArgument(uiQueryIndex, 0.0, QueryIndexAttributes);
	//pBitMapNode_W=new CSearchBmNode_W(0xff, 0, 0, pContainersFactory);
	//pBitMapNode_W->SetOrt(0);
	//pPosCnt=pBitMapNode_W->AddPosCnt(0);
	//pPosCnt->SetOrt(21);
	//pPosCnt->SetOrt(58);
	//pPosCnt->SetOrt(67);
	//pPosCnt->SetOrt(91);
	////pBitMapNode_W->View();
	//pRelevanceLogic->CreateLogicValue(pBitMapNode_W, pComplexArgument);
	////--------------------------
}

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS