#pragma once
#include "CRFTemplate.h"
#include "Sentence.h"

namespace NER
{
    /// Конвертор в формат CRF
	ref class CRFConverter
	{
	private:
		/// Обозначение начала предложения (в формате SRFSuit)
		static System::String^ BeginOfSentence = "__BOS__";
		/// Обозначение конца предложения (в формате SRFSuit)
        static System::String^ EndOfSentence = "__EOS__";

        Dictionary<System::String^,int>^ _attributeIndexTable;
        System::Text::StringBuilder^ _stringBuilder;
		NER::CRFTemplate^ m_crfTemplate;
		 
	internal: 
		///Конструктор конвертора
		CRFConverter(NER::CRFTemplate^ crfTemplate);

	public:
		/// Преобразование в формат CFRSuit
		///@param sentences - предложения с характеристиками слов
		///@return Результат в формате CFRSuit
		System::String^ ToCRFFormat(System::Collections::Generic::List<NER::Sentence^>^ sentences);

	private:
        /// Может ли быть применен шаблон к заданному слову предложения
        /// @param sentence - Предложение
        /// @param wordIndex - Индекс слова
        /// @param attributeTemplate - Шаблон аттрибутов
		static bool CanTemplateBeApplied(NER::Sentence^ sentence, int wordIndex, IEnumerable<NER::CRFAttribute^>^ attributeTemplate);
	
		/// Применить шаблон
        /// @param sentance - Предложение
        /// @param currentIndex - Индекс слова
        /// @param attributeTemplate - Шаблон аттрибутов
		void ApplyTemplate(NER::Sentence^ sentence, int wordIndex, IEnumerable<NER::CRFAttribute^>^ attributeTemplate);

		/// Конвертировать предложение в CRF формат
		/// @param crfTemplate - Шаблон
		/// @param sentance - предложение
		void ConvertSentenceToCRF(NER::Sentence^ sentence);

		/// Проинициализировать словарь индексов аттрибутов
        void InitializeAttributeIndexTable();
	};
}



