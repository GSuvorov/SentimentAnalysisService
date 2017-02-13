#pragma once

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>
#include ".\UnitTestsUsings.h"
#include ".\cnt_factory.h"
#include ".\storage_manager.h"

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

//--------------------------------------------------------------------//

class CIndexContainer_test : public CPPUNIT_NS::TestFixture,
		public SS::UnitTests::NdxSE::NdxProcessor::CUnitTestsUsings

{
	CPPUNIT_TEST_SUITE(CIndexContainer_test);
	//CPPUNIT_TEST(TestAll);
	CPPUNIT_TEST_SUITE_END();

public:
	void TestAll(void);

protected:
	///создает индекс нужного типа
	SS::Core::NdxSE::NdxProcessor::Storages::CIndexStorage* CreateIndex(SS::Interface::Core::NdxSE::ENdxStorageConfig IndexConfig);
	///удаляет индекс
	void DeleteIndex(void);

	void Test(SS::Core::NdxSE::NdxProcessor::Containers::CContainersFactory* pContainersFactory);

	//создаем менеджер хранилищ
	SS::Core::NdxSE::NdxProcessor::Storages::CStorageManager* m_pStorageManager;
	//менеджер индексной базы
	SS::Interface::Core::NdxSE::INdxBaseManager* m_pNdxBaseManager;

};

//--------------------------------------------------------------------//

}
}
}
}
}

#endif //_SS_UNITTESTS
