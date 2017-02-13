//QueryIndex.h

#pragma once

#include "..\ASCInterface\ICommonContainers.h"

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{

			using SS::Interface::Core::CommonContainers::IQueryIndex;
			using SS::Interface::Core::CommonContainers::SQueryIndexAttributes;
			
			///"индекс запроса" инкапсулирует индекс запроса к индексному процессору
			class CQueryIndex : public IQueryIndex
			{
			private:
				unsigned int			m_QueryIndex;		// индекс запроса
				float					m_IndexWeight;		// вес индекса
				SQueryIndexAttributes	m_oAttributes;		// аттрибуты
				char					m_Operator;			// оператор слова
				unsigned int			m_LeftBrackets;		// кол-во скобок слева
				unsigned int			m_RightBrackets;	// кол-во скобок справа
				wchar_t*				m_wszWord;			// слово запроса
			public:
				//КОНСТРУКТОРЫ
				CQueryIndex();
				virtual ~CQueryIndex();
				//МЕТОДЫ IQueryIndex
				///получение слова запроса (если это запрос к глобальному движку)
				virtual const wchar_t* GetQueryWord();								
				///получение слова запроса (если это запрос к глобальному движку)
				virtual void SetQueryWord(const wchar_t* wszWord);				
				///получение индекса
				virtual unsigned int GetQueryIndex()							
				{
					return m_QueryIndex;
				}
				///установление индекса
				virtual void SetQueryIndex(unsigned int uiIndex)				
				{
					m_QueryIndex = uiIndex;
				}
				///возвращает вес индекса
				virtual float GetIndexWeight()									
				{
					return m_IndexWeight;
				}
				///устанавливает вес индекса
				virtual void SetIndexWeight(float fWeight)						
				{
					m_IndexWeight = fWeight;
				}
				///возвращает аттрибуты
				virtual SQueryIndexAttributes GetAttributes()					
				{
					return m_oAttributes;
				}
				///устанавливает аттрибуты
				virtual void SetAttributes(SQueryIndexAttributes oAttributes)	
				{
					m_oAttributes = oAttributes;
				}
				///возвращает оператор слова
				virtual const char GetOperator()								
				{
					return m_Operator;
				}
				///устанавливает оператор слова
				virtual void SetOperator(char chOperator)						
				{
					m_Operator = chOperator;
				}
				///получение кол-ва скобок слева
				virtual const unsigned int GetLeftBrackets()					
				{
					return m_LeftBrackets;
				}
				///получение кол-ва скобок справа
				virtual const unsigned int GetRightBrackets()					
				{
					return m_RightBrackets;
				}
				///установление кол-ва скобок слева
				virtual void SetLeftBrackets(unsigned int uiBracketCount)		
				{
					m_LeftBrackets = uiBracketCount;
				}
				///установление кол-ва скобок справа
				virtual void SetRightBrackets(unsigned int uiBracketCount)		
				{
					m_RightBrackets = uiBracketCount;
				}
			};


		}
	}
}