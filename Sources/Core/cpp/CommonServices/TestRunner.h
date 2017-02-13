#pragma once
#ifdef _SS_UNITTESTS

namespace SS {
namespace UnitTests { 
namespace CommonServices {
namespace UserTextStorage {

class CTestRunner :	public Interface::UnitTests::ITestRunner
{

public:
	CTestRunner() {}
	
	///запуск тестов
	void Run();

   void MainTest();
   void XSLTTest();
	
public:
   RELEASE_IMPL()

	BEGIN_QI_MAP()
		QI_MAP_INHERIT_ENTRY( IID_TestRunner, Interface::UnitTests::ITestRunner )
      QI_MAP_INHERIT_ENTRY( IID_Base, Interface::IBase )
	END_QI_MAP()
};

} //namespace UserTextStorage 
} //namespace CommonServices 
} //namespace UnitTests 
} //namespace SS 

#endif //_SS_UNITTESTS
