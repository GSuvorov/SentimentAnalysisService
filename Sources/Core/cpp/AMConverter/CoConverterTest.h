#pragma once

#ifdef _SS_UNITTESTS


#include "./CoConverter.h"


namespace SS
{
	namespace UnitTests
	{
		namespace AMConverter
		{
			
			/// отвечает за тестирование функциональности конвертера
			class CCoConverterTest 
			{

			public:
				/// ¬ыполн€ет тестирование кодировани€ набора семантических характеристик в последовательность чисел
				void TestMorpho(void);
				bool TestSemanticSetXML(void);
			};

			
		};

	};
};

#endif