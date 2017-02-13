//QueryIndex.cpp

#include "StdAfx.h"
#include ".\queryindex.h"

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{


			//конструкторы
			CQueryIndex::CQueryIndex() : m_wszWord(NULL), 
										 m_QueryIndex(0), 
										 m_LeftBrackets(0), 
										 m_RightBrackets(0),
										 m_IndexWeight(0),
										 m_Operator(0)
			{
			}

			CQueryIndex::~CQueryIndex()
			{
				SS_TRY
					if (m_wszWord)
					{
						delete [] m_wszWord;
					}
				SS_CATCH(L"")
			}

			//другие методы
			const wchar_t* CQueryIndex::GetQueryWord()							// получение слова запроса (если это запрос к глобальному движку)
			{
				SS_TRY
					if (m_wszWord)
					{
						return m_wszWord;
					}
					return '\0';
				SS_CATCH(L"")
			}
			
			void CQueryIndex::SetQueryWord(const wchar_t* wszWord)			// получение слова запроса (если это запрос к глобальному движку)
			{
				SS_TRY
					if (m_wszWord)
					{
						delete [] m_wszWord;
						m_wszWord = NULL;
					}
					if (wszWord)
					{
						int len;
						len = (int) wcslen(wszWord);
						m_wszWord = new wchar_t[len + 1];
						wcscpy(m_wszWord, wszWord);
					}
				SS_CATCH(L"")
			}

		}
	}
}