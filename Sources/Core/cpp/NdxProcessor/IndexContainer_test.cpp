#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\indexcontainer_test.h"
#include ".\index_container.h"
#include ".\test_const.h"
#include ".\console.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SS::UnitTests::NdxSE::NdxProcessor::Containers::CIndexContainer_test);

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

using namespace SS::Core::NdxSE::NdxProcessor::Containers;
using namespace SS::Core::NdxSE::NdxProcessor::Storages;
using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Interface::Core::NdxSE;

//--------------------------------------------------------------------//
CIndexStorage* CIndexContainer_test::CreateIndex(ENdxStorageConfig IndexConfig)
{
	//получаем менеджер индексной базы
	m_pNdxBaseManager=(INdxBaseManager*)GetLoadManager()->
	GetInterface(L"./core/NdxBaseManager.dll", CLSID_NdxBaseManager, IID_NdxBaseManager);
	
	m_pNdxBaseManager->ResetNdxBaseStructure();
	//добавляем базе индекс с нужной структурой
	m_pNdxBaseManager->AddNdxBaseUnit(L"TEXT", IndexConfig);	

	//устанавливаем путь к базе
	m_pNdxBaseManager->SetBasePath(GetUTWorkingPath());
	//открываем базу
	m_pNdxBaseManager->OpenBase(L"ind_cnt_test_base");
	//оочищаем базу
	m_pNdxBaseManager->ClearBase();

	//создаем менеджер хранилищ
	m_pStorageManager=new CStorageManager();
	m_pStorageManager->Init(m_pNdxBaseManager);
	m_pStorageManager->SetIndexingMode();

	return m_pStorageManager->GetIndexStorage(L"TEXT");
}

void CIndexContainer_test::DeleteIndex(void)
{
	//удаляем менеджер хранилищ
	if(m_pStorageManager) delete m_pStorageManager;

	//закрываем базу
	m_pNdxBaseManager->CloseBase();
	
	m_pNdxBaseManager=NULL;
	m_pStorageManager=NULL;
}

void CIndexContainer_test::TestAll(void)
{
	UT_STOP_LOGING
	UT_START_LOGING
	
	CIndexStorage* pIndexStorage=NULL;

	////тестируем
	//pIndexStorage=CreateIndex(escAbsSentence);
	//if(!(ISNULL(pIndexStorage))) Test(pIndexStorage->GetContainersFactory());
	//DeleteIndex();

	//тестируем
	pIndexStorage=CreateIndex(escText_WC_Huge);
	if(!(ISNULL(pIndexStorage))) Test(pIndexStorage->GetContainersFactory());
	DeleteIndex();

	UT_START_LOGING
}

void CIndexContainer_test::Test(CContainersFactory* pContainersFactory)
{
	if(ISNULL(pContainersFactory)) return;

	unsigned char ucLevel=0;
	unsigned int Buffer[10];
	
	CCoordinateContainer_4i<3> CoordinateContainer(pContainersFactory);
	
	unsigned int i=sizeof(CCoordinateContainer_4i<3>);

	Buffer[1]=0;
	Buffer[4]=0;
	CoordinateContainer.AddCoordinate(Buffer, 10);

	Buffer[1]=0;
	Buffer[4]=1;
	CoordinateContainer.AddCoordinate(Buffer, 10);

	Buffer[1]=0;
	Buffer[4]=5;
	CoordinateContainer.AddCoordinate(Buffer, 10);

	Buffer[1]=50;
	Buffer[4]=12;
	CoordinateContainer.AddCoordinate(Buffer, 10);

	Buffer[1]=50;
	Buffer[4]=25;
	CoordinateContainer.AddCoordinate(Buffer, 10);

	Buffer[1]=53;
	Buffer[4]=26;
	CoordinateContainer.AddCoordinate(Buffer, 10);

	Buffer[1]=66;
	Buffer[4]=7;
	CoordinateContainer.AddCoordinate(Buffer, 10);

	Buffer[1]=66;
	Buffer[4]=43;
	CoordinateContainer.AddCoordinate(Buffer, 10);
	
	//BmTree.View();
}

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS