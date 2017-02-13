#pragma once
#include "GraphematicCharacteristicProcessor.h"

namespace NER
{
	using System::Collections::Generic::List;

	ref class Word
	{
		private:
			/// анализатор графематических характеристик
			static NER::GraphematicCharacteristicProcessor^ _processor = gcnew NER::GraphematicCharacteristicProcessor();

			/// графематические характеристики
			List<System::String^>^ _graphematicCharacteristics;
		
		public:
			/// конструктор
			/// value - слово
			Word(System::String^ value)
			{
				_graphematicCharacteristics = gcnew List<System::String^>();
				_graphematicCharacteristics->Add(value);
				for each (System::String^ characteristic in _processor->Run(value))
					_graphematicCharacteristics->Add(characteristic);
			}

			/// получения коллекции графематических характеристик
			List<System::String^>^ GetGraphematicCharacteristics() { return _graphematicCharacteristics; }
	};

	ref class Sentence
	{
		private:
			/// список слов
			List<Word^>^ _words;

		public:
			/// конструктор
			Sentence()
			{
				_words = gcnew List<Word^>();
			}

			/// добавление слова
			void AddWord(Word^ word)
			{
				_words->Add(word);
			}

			/// получение списка слов
			List<Word^>^ GetWords() { return _words; }
	};
}