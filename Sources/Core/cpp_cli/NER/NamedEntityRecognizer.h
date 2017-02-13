#pragma once
#include "CRFTemplate.h"
#include "CRFProcessor.h"
#include "CRFConverter.h"
#include "Sentence.h"

namespace NER
{
	using System::Collections::Generic::List;

    /// Обработчик именованных сущностей. Обработка с использованием сторонней библиотеки CRFSuit
	public ref class NamedEntityRecognizer
	{
	private:
		NER::CRFProcessor^ m_processor;
		NER::CRFTemplate^ m_template;
		NER::CRFConverter^ m_converter;

        ///Путь к промежуточному результату
		static System::String^ m_tempResult = System::IO::Path::Combine(Utils::GlobalVariables::UserResourcesPath, "NER", "tempResultNER.txt");

	public:
		NamedEntityRecognizer(System::String^ modelPath, System::String^ templatePath);

		/// Запустить обработчик
		/// @param sourceString - Текст разбитый на слова
		/// @return todo: Выходной формат пока не определен
		List<System::String^>^ Run(System::String^ sourceString);

	private:
		/// Получить шаблон 
		/// @param templatePath - путь к файлу шаблона
		/// @return - Шаблон
		NER::CRFTemplate^ GetTemplate(System::String^ templatePath);

		/// Получение предложений с дополнительными характеристиками для слов
		/// @param sourceString - Исходный формат слов
		/// @return - список предложений
		static List<NER::Sentence^>^ GetSentencesWithAdditionalInfo(System::String^ sourceString);

		/// Запись промежуточного результата в файл
		/// @param destenationFilePath - Путь нразначения
		/// @param stringToWrite - Строка для записи
		/// Временный функционал
		void WriteCrfResultToFile(System::String^ destenationFilePath, System::String^ stringToWrite);

		/// Проверить применимость шаблона к промежуточному результату
		/// @param crfTemplate - Шаблон
		/// @param sentences - Промежуточный результат
		static void CheckTemplateAndResult(NER::CRFTemplate^ crfTemplate, List<NER::Sentence^>^ sentences);

		/// Проверить правильность шаблона
		/// @param crfTemplate - Шаблон
		static void CheckTemplateOnly(NER::CRFTemplate^ crfTemplate);
	};
}
