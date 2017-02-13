#pragma once

#include "..\..\cpp\ASSInterface\ILexicaLib.h"
#include "..\..\cpp\ASSInterface\ITextBlockEx.h"

#include <vector>

using namespace NER;

namespace SS
{
namespace LinguisticProcessor
{
	public ref class NERProcessor 
	{
		ref class UnitTableItem
		{
		private:
			SS::Interface::Core::BlackBox::IUnit *_pUnit;
			int _lineIndex;

		public:
			UnitTableItem(SS::Interface::Core::BlackBox::IUnit *pUnit, int lineIndex)
			{
				_pUnit = pUnit;
				_lineIndex = lineIndex;
			}

			int GetLineIndex()
			{
				return _lineIndex;
			}

			SS::Interface::Core::BlackBox::IUnit* GetUnit()
			{
				return _pUnit;
			}
		};

	private:
		System::Collections::Generic::List<UnitTableItem^>^ _unitTable;
		std::vector<SS::Interface::Core::BlackBox::IUnit*>* _pCombinatoryUnits;
		NER::NamedEntityRecognizer^ m_RecognizerRUS;
		NER::NamedEntityRecognizer^ m_RecognizerENG;

	public:
		NERProcessor(System::String^ path);
		virtual ~NERProcessor();
		void Run(SS::Interface::Core::BlackBox::IText* pText, const wstring& plainText, SS::Interface::Core::LexicalAnalyze::ILexica* pLexica);

	private:
		void RunSentence(NER::NamedEntityRecognizer^ pRecognizer,
			SS::Interface::Core::BlackBox::ISentence* pSentence,
			const wstring& plainText, 
			SS::Interface::Core::LexicalAnalyze::ILexica* pLexica);

		/// получение модели для обработки предложения
		NER::NamedEntityRecognizer^ GetRecognizer(SS::Interface::Core::BlackBox::ISentence* pSentence);

		/// Сохранение семантики именованных сущностей
		/// semantic - коллекция семантики всех Unit-ов текста
		/// pLexica - объект для сохранения семантики
		void SaveNamedEntitiesSemantic(System::Collections::Generic::List<System::String^>^ semantic, SS::Interface::Core::LexicalAnalyze::ILexica* pLexica);

		void SetSemanticForCombinatoryUnits(SS::Interface::Core::LexicalAnalyze::ILexica* pLexica);
		void SetSemanticForCombinatoryUnit(SS::Interface::Core::BlackBox::IUnit* pParentUnit, SS::Interface::Core::LexicalAnalyze::ILexica* pLexica);

		/// Получение семантического типа из строки
		SS::Core::Features::Types::TSemanticType GetSemantic(System::String^ semantic);
		/// Проверка, что заданный юнит - именованный
		bool IsNamed(SS::Interface::Core::BlackBox::IUnit* pUnit);
		/// Проверка, что у юнита семантика части имени
		bool IsCombiPart(System::String^ semantic);

		void AppendUnitStringToLine(SS::Interface::Core::BlackBox::IUnit* &prevUnit, SS::Interface::Core::BlackBox::IUnit *pUnit, System::Text::StringBuilder^ stringBuilder, const wstring& plainText, int& stringNumber);
		bool IsCommaSeparator(wstring token);
		bool IsPunctuation(wstring token);
		bool MergeWithPrev(SS::Interface::Core::BlackBox::IUnit* &prevUnit, SS::Interface::Core::BlackBox::IUnit *pUnit, System::Text::StringBuilder^ stringBuilder, const wstring& plainText);
		System::String^ PrepareSentence(SS::Interface::Core::BlackBox::ISentence* pSentence, const wstring& plainText);
		TUnitList::iterator GetIteratorAfterLastWord(TUnitList& sentUnits);
		void PrepareMainPart(TUnitList& sentUnits, TUnitList::iterator& prevLastIt, System::Text::StringBuilder^ stringBuilder, const wstring& plainText, int& stringNumber);
		void PrepareEndPart(TUnitList& sentUnits, TUnitList::iterator& prevLastIt, System::Text::StringBuilder^ stringBuilder, int& stringNumber);
	};
}
}