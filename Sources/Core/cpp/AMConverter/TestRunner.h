#pragma once


#include ".\CoConverterTest.h"


namespace SS
{
	namespace UnitTests
	{
		namespace AMConverter
		{
			class CTestRunner : public SS::Interface::UnitTests::ITestRunner
			{
#ifdef _SS_UNITTESTS
				SS::UnitTests::AMConverter::CCoConverterTest oCoConverterTest;
#endif

         private:
            void TestConstructionObject();
            void TestFeature( const SS::Core::Features::CMorphoFeature &MF );
            void ShowFeature( int MorphoInfo, int SpecialMorpho, SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber DictNumber = SS::Dictionary::DATA_TYPE::NAMES::ednENominalName );
            void RandomFillFeatureCollection( SS::Core::Features::CBaseFeatureCollection &FC );
            void TestSyntaxConstruction();
            void TestAMConverter();

			public:
				CTestRunner(void);
				~CTestRunner(void);

				//запуск тестов
				void Run();
				//освобождение
				ULONG Release();
				//приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
				HRESULT QueryInterface(REFIID pIID, void** pBase); 
			};
		}
	}
}

