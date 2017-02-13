//#pragma once
//
//#include "TListEnumerator.h"
//#include <string>
//
//namespace SS
//{
//namespace Core
//{
//namespace Types
//{
//namespace Search
//{
//
//	/// Обеспечивает хранение слова и его индекса основы
//	class TWord
//	{
//
//	protected:
//		/// Слово
//		std::string m_sWord;
//		/// Индекс основы слова
//		unsigned int m_uiSourceIndexOfWord;
//	public:
//
//		TWord() :
//		  m_uiSourceIndexOfWord(NULL)
//		{
//
//		};
//
//		TWord(unsigned int uiSourceIndexOfWord) :
//		  m_uiSourceIndexOfWord(uiSourceIndexOfWord)
//		{
//
//		};
//
//		///Устанавливает слово TWord::m_sWord
//		void SetWord(const char* pszWord)
//		{
//			m_sWord = pszWord;
//		};
//
//		///Возвращает слово TWord::m_sWord
//		const char* GetWord()
//		{
//			return m_sWord.c_str();
//		};
//
//		///Устанавливает индекс основы слова TWord::m_uiSourceIndexOfWord
//		void SetSourceIndexOfWord(unsigned int uiSourceIndexOfWord)
//		{
//			m_uiSourceIndexOfWord = uiSourceIndexOfWord;
//		};
//
//		///Возвращает индекс основы слова TWord::m_uiSourceIndexOfWord
//		unsigned int GetSourceIndexOfWord()
//		{
//			return m_uiSourceIndexOfWord;
//		};
//	};
//
//
//	/// Группа синонимов
//	/** наряду с коллекцией синонимов содержит главное слово данной коллекции 
//	(одно из значений исходного слова, для которого и были получены синонимы),
//	его индекс и параметр, показывающий, была-ли выбрана данная группа*/
//	class TGroupSynonyms : public SS::Core::Types::TListEnumerator<TWord>,
//		public TWord
//	{
//		/// Переменная определяет, участвует ли данная группа в поиске
//		bool m_blIsChecked;
//
//	public:
//
//		TGroupSynonyms(unsigned int uiSourceIndexOfWord) : TWord(uiSourceIndexOfWord)
//		{
//		};
//
//		TGroupSynonyms() : m_blIsChecked(false)
//		{
//
//		}
//		/// Устанавливает переменную TGroupSynonyms::m_blIsChecked
//		void SetIsChecked(bool blIsChecked)
//		{
//			m_blIsChecked = blIsChecked;	
//		};
//
//		/// Возвращает переменную TGroupSynonyms::m_blIsChecked
//		bool GetIsChecked()
//		{
//			return m_blIsChecked;
//		};
//
//		bool operator == (const TGroupSynonyms& value)
//		{
//			return (value.m_uiSourceIndexOfWord == m_uiSourceIndexOfWord) ? true : false;
//		};
//	};
//
//	/// Содержит коллекцию групп синонимов TGroupSynonyms,
//	/** а также исходное слово, для которого были получены все синонимы */
//	class TSynonymsSplit : public SS::Core::Types::TListEnumerator<TGroupSynonyms>,
//		public TWord
//	{
//	public:
//		
//	};
//
//	typedef SS::Core::Types::TListEnumerator<TSynonymsSplit> TSynonymsSplits;
//}
//}
//}
//}
