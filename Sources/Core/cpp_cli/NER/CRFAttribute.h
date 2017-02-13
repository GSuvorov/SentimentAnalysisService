#pragma once

namespace NER
{
	using System::Collections::Generic::IEnumerable;
	using System::Collections::Generic::List;
	using System::Collections::Generic::Dictionary;

    /// Аттрибут линейного CRF алгоритма
	ref class CRFAttribute
	{
	private:
		System::String^ _attributeName; // Название аттрибута
        int _position; // Позиция

	internal:
		///Конструктор
		///@param attributeName - Название аттрибута
		///@param position - Позиция 
		CRFAttribute(System::String^ attributeName, int position);
	
	public:
        /// Получить название аттрибута
        System::String^ GetAttributeName();

        /// Получить индекс позиции аттрибута
		///@return Индекс позиции аттрибута
        int GetPosition();
	};
}