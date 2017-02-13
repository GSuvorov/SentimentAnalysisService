#pragma once
#ifdef _SS_UNITTESTS

#include "..\ASDInterface\INDXQuery.h"
#include "..\ASCInterface\IBaseManager.h"
#include "..\ASCInterface\ILinguisticProcessor.h"

#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>


namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxQuery
{

///	Тестовый класс интерфейса IParametrizedQuery
class CParametrizedQuery_test: public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( CParametrizedQuery_test );
		CPPUNIT_TEST( MainTest );
	CPPUNIT_TEST_SUITE_END();

public:
	CParametrizedQuery_test():
		m_pQuery(NULL), m_pLinguisticProcessor(NULL),
		m_pPrmQuery(NULL), m_pBaseManager(NULL)
	{}
	///	Тест
	void	MainTest();

private:
	SS::Interface::Core::CommonContainers::IQueryResult*	m_pQuery;
	SS::Interface::Core::ILinguisticProcessor*			m_pLinguisticProcessor;
	SS::Interface::Core::NdxSE::IParametrizedQuery*		m_pPrmQuery;
	SS::Interface::Core::ResourceManagers::IBaseManager* m_pBaseManager;

	typedef vector<wstring>	VectorQueries;
	///	Считывает запросы из файла
	void GetQueries(wstring& _wsFilePath, VectorQueries& _Queries);
};

}
}
}
}
#endif	//	_SS_UNITTESTS