#pragma once

#include "..\..\cpp\ASSInterface\IBlackBox.h"
#include "StringService.h"

namespace LinguisticsKernel
{
	public ref class CWord
	{
	public:
		enum class EIndexType
		{
			eitMorpho=0,	
			eitSyntax=1,	
			eitSemantic=2,	
			eitSynonim=3,	
		};

		CWord(SS::Interface::Core::BlackBox::IUnit* pUnit, CStringService^ pStringService);
		CWord(SS::Interface::Core::BlackBox::Query::IQueryUnit* pQueryUnit, bool bQuestion, CStringService^ pStringService);
		~CWord(void);

		///сравнение юнита дерева разбора
		void Compare(CWord^ pWord, System::Windows::Forms::TreeNode^ pTNWord, System::Windows::Forms::TreeNode^ pCommonTNWord);
		///извлекает колекции индексов у слова
		void ExtractIndexes(CWord^ pWord);
		///возвращает массив индексов по типу
		System::Collections::ArrayList^ GetIndexesByType(CWord::EIndexType IndexType) {
				return safe_cast<System::Collections::ArrayList^>(m_parrIndexes[ (int) IndexType ]);
		};
		///добавляет индексы слова к узлу дерева
		void AddToNodeIndexes(System::Windows::Forms::TreeNode^ pTN);
		///добавляет слово к узлу дерева
		System::Windows::Forms::TreeNode^ AddWordToNode(System::Windows::Forms::TreeNode^ pTN);

		///возвращает строковое представление
		property System::String^ Word {
			System::String^ get() {
				 return m_psWord;
			};
		};
		///возвращает флаг вопросительного слова
		///устанавливает флаг вопросительного слова
		property bool IsQuestion {
			bool get() {
				 return m_bQuestion;
			};
			void set(bool value) {
				m_bQuestion = value;
			};
		};		
		///возвращает коллекцию слов совпавших с данным
		property System::Collections::ArrayList^ SameWords{
			System::Collections::ArrayList^ get() {
				 return m_parrSameWords;
			};
		};

	protected:
		///сравнение коллекций индексов
		void Compare(System::Collections::ArrayList^ pLeftIndexes, System::Collections::ArrayList^ pRightIndexes, System::Collections::ArrayList^ pSameIndexes, System::Collections::ArrayList^ pDifIndexes);
		///добавляет колекции индексов слова к узлу дерева
		void AddToNode(System::Collections::ArrayList^ pSameIndexes, System::Collections::ArrayList^ pDifIndexes, System::Windows::Forms::TreeNode^ pTN, CWord::EIndexType IndexType);
		///добавляет индекс к узлу дерева
		void AddIndexToNode(System::UInt32 Index, CWord::EIndexType IndexType, System::Drawing::Color ForeColor, System::Windows::Forms::TreeNode^ pTN);

		///строковое представление
		System::String^ m_psWord;
		///коллекция индексов
		System::Collections::ArrayList^ m_parrIndexes;
		///флаг вопросительного слова
		bool m_bQuestion;
		///коллекция слов совпавших с данным
		System::Collections::ArrayList^ m_parrSameWords;
		///преобразователь индексов в строки
		CStringService^ m_pStringService;
		///цвет слова при отображении
		System::Drawing::Color m_WordColor;
	};


	public ref class CTreeComparator
	{
	public:
		CTreeComparator(void);
		~CTreeComparator(void);
		
		///конвретирует тексты в массивы слов
		void SetLeftWords(SS::Interface::Core::BlackBox::IText* pLeftText){if(pLeftText) Convert(pLeftText, m_pLeftWords);};
		///конвретирует тексты в массивы слов
		void SetRightWords(SS::Interface::Core::BlackBox::IText* pRightText){if(pRightText) Convert(pRightText, m_pRightWords);};
		///конвретирует тексты в массивы слов
		void SetLeftWords(SS::Interface::Core::BlackBox::Query::IQuery* pLeftQuery){if(pLeftQuery) Convert(pLeftQuery, m_pLeftWords);};
		///конвретирует тексты в массивы слов
		void SetRightWords(SS::Interface::Core::BlackBox::Query::IQuery* pRightQuery){if(pRightQuery) Convert(pRightQuery, m_pRightWords);};
		///сравнение результатов анализов двух запросов
		void Compare(System::String^ pQuery, System::Windows::Forms::TreeView^ pComparationTree, System::Windows::Forms::TreeView^ pCoincidenceTree);
		
	protected:
		///конвертирует IText в массив CWord
		void Convert(SS::Interface::Core::BlackBox::IText* pText, System::Collections::ArrayList^ pWords);
		///конвертирует IQuery в массив CWord
		void Convert(SS::Interface::Core::BlackBox::Query::IQuery* pQuery, System::Collections::ArrayList^ pWords);
		
		///коллекция коллекций слов с левой панели
		System::Collections::ArrayList^ m_pLeftWords;
		///коллекция коллекций слов с правой панели
		System::Collections::ArrayList^ m_pRightWords;
		///преобразователь индексов в строки
		CStringService^ m_pStringService;
	};

}