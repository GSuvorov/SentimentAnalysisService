#pragma once
#include <string>
#include <vector>

// если включить crfsuite.hpp то будет multiple definition problem
// Видимо из-за специфики работы линкера в проекте Managed C++
namespace CRFSuite
{
	class Tagger;
}

namespace NER
{
	/// Класс, непосредственно работающий с библиотекой CRFSuit
	public ref class CRFProcessor
	{
		public:
			///Конструктор
			///@param pathToModel - полныйы путь к файлу модели 
			CRFProcessor(System::String^ pathToModel);
			///Запустить обработку NER.CRF
			///@param inputCrfString - Строка в входном для CRF формате
			///@return - результат
			std::vector<std::string> Run(System::String^ inputCrfString);
		private:
			// Преобразование .NET строки в std::string
			std::string TOstring(System::String^ s);
			// Преобразование .NET строки в std::wstring
			std::wstring TOwstring (System::String^ s);
			/// Получение значения атрибута
			std::string GetAttributeInUTF8(System::String^ s);
			/// Разделение строкие по указанному разделителю
			array<System::String^>^ SplitString(System::String^ s, System::String^ separator);
		private:
			///Объект класса CRFSuite::Tagger (см. библиотеку CRFSuite)
			CRFSuite::Tagger* m_tagger;
	};
}
