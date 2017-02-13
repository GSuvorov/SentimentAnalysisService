// IndexAndPosition.h
#pragma once
#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASSInterface\Constants.h"
#include "..\ASSInterface\IBlackBoxUnit.h"
#include "..\ASSInterface\IBlackBox.h"
#include "ContainersFactory.h"

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{
			using SS::Interface::Core::BlackBox::IUnit;
			using SS::Interface::Core::CommonContainers::IServiceIndex;
			using SS::Interface::Core::CommonContainers::IIndexationIndex;
			using SS::Interface::Core::BlackBox::IBlackBoxTextStorage;
			using SS::Core::Types::IndexationSearch::TLinguisticProcessorMode;
			
			/// "индекс на позиции" индекс на одной позиции в тексте
			/**
			включает в себя индексируемый и сохраняемый индексы, располагающиеся на одной позиции текстового блока.
			используется при индексации и при поиске
			*/ 
			class CIndexAndPosition : public SS::Interface::Core::CommonContainers::IIndexAndPosition
			{
			private:
				std::wstring			  m_ws;
				IServiceIndex*			  m_pServiceIndex;
				IIndexationIndex*		  m_pIndexationIndex;
				IUnit*					  m_pUnit;
				IBlackBoxTextStorage*	  m_pBlackBoxTextStorage;
				TLinguisticProcessorMode* m_pLinguisticProcessorMode;
				///Флаг выставляется, если необходимо удалять Unit в деструкторе 
				bool m_bDelUnit;
				///Фабрика контейнеров
				CContainersFactory* m_pContainersFactory;

			private:
				///Создание юнита
				void CreateUnit();
				
			public:
				//КОНСТРУКТОРЫ
				CIndexAndPosition(IBlackBoxTextStorage* pBlackBoxTextStorage, TLinguisticProcessorMode* pLinguisticProcessorMode);
				virtual	~CIndexAndPosition();
				///ДРУГИЕ МЕТОДЫ
				///Получ позиции индекса в текст блоке
				virtual unsigned int GetPosition();	 				
				///Установ позиции индекса в текст блоке
				virtual void SetPosition(unsigned int uiPosition);	
				///Вернет сохраняемый индекс
				virtual IServiceIndex* GetServiceIndex();			
				///Вернет индексируемый индекс
				virtual IIndexationIndex* GetIndexationIndex();		
				///Установление слова
				virtual void SetWord(const wchar_t* szWord);					
				///Вернет само слово (для вывода пользователю)
				virtual const wchar_t* GetWord();
				///Вернет тип подсветки индекса
				virtual SS::Core::Types::IndexationSearch::EHighlightType GetHighlightType();
				///Установит тип подсветки индекса
				virtual void SetHighlightType(SS::Core::Types::IndexationSearch::EHighlightType hlt);
				///Получение порядкового номера в тексте первого символа слова
				virtual unsigned int GetFirstCharPosition();
				///Установление порядкового номера в тексте первого символа слова
				virtual void SetFirstCharPosition(unsigned int uiPosition);
				///Вернет тип подсветки индекса
				virtual unsigned int GetIsInAnswerSentence();
				///Установит тип подсветки индекса
				virtual void SetIsInAnswerSentence(unsigned int bIn);
			
				///ДРУГИЕ
				///Получение XML - строки содержимого контейнера
				std::wstring* GetXML();
				///Установка соответствующего юнита
				void SetUnit(IUnit* pUnit);
				///Получить указатель на юнит
				IUnit* GetUnit();
				///Содержит ли класс хотя-бы один индекс (метод - не интерфейсный)
				bool IsValid();
				///Приказывает не удалять созданый Unit
				void NotDeleteUnit();
				///Установить фабрику контейнеров
				void SetContainersFactory(CContainersFactory* pContainersFactory){ m_pContainersFactory = pContainersFactory; };
			};


		}
	}
}