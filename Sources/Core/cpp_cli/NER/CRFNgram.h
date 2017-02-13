#pragma once
#include "CRFAttribute.h"

namespace NER
{
	using System::Collections::Generic::IEnumerable;
	using System::Collections::Generic::List;
	using System::Collections::Generic::Dictionary;
    
	/// N-грамма
	ref class Ngram
	{
	private:
		/// Составные части N-граммы
		IEnumerable<NER::CRFAttribute^>^ _parts; 

	public:
		///Конструктор N-граммы
		///@param parts - Составные части N-граммы
		Ngram(IEnumerable<NER::CRFAttribute^>^ parts);

        /// Получить части N-граммы
		///@return Составные части N-граммы
        IEnumerable<NER::CRFAttribute^>^ GetParts();
	};
}