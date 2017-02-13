#pragma once

#ifdef _SS_UNITTESTS

#include <cppunit/extensions/HelperMacros.h>

namespace SS
{
	namespace UnitTests
	{
		namespace CoSeDi
		{

			class CCoSeDiManagerTest : public SS::Core::CommonServices::CBaseCoreClass
			{
			public:
				void FullTest(void);

			private:

				void FullTestSemanticsInDBMS(void);
				void TestManyLoadAMConverter(void);
				void TestLoadAMConverter(void);
				void TestLoad(void);
				void TestCombinatory(void);
				
				void TestSemantic(void);
				void TestSemanticByWord(void);
				
				void SemanticClassesTreeTest(void);
				void SemanticClassesTreeTest1(void);
				void SemanticClassesTreeTest2(void);				
				
				void TestSemanticIDSourceTree(void);

				void TestFetures(void);
				
				
				void TestSyntax(void);


			};
		};
	};
};

#endif