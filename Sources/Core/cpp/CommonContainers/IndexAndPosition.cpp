// IndexAndPosition.cpp
#include "StdAfx.h"
#include "indexandposition.h"
#ifdef _SS_UNITTESTS
#include <iostream>
#endif

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{

			//Конструктор
			CIndexAndPosition::CIndexAndPosition(IBlackBoxTextStorage* pBlackBoxTextStorage, TLinguisticProcessorMode* pLinguisticProcessorMode) 
				: 
				m_pIndexationIndex(NULL), m_pServiceIndex(NULL), 
				m_pUnit(NULL), m_pBlackBoxTextStorage(pBlackBoxTextStorage),
				m_pLinguisticProcessorMode(pLinguisticProcessorMode),
				m_bDelUnit(false),
				m_pContainersFactory(NULL)
			{}
			
			//Деструктор
			CIndexAndPosition::~CIndexAndPosition()
			{
				if (m_pServiceIndex)
				{ delete m_pServiceIndex; }
				if (m_pIndexationIndex)
				{ delete m_pIndexationIndex; }
				if (m_pUnit && m_bDelUnit) 
				{ m_pUnit->ReleaseUnitHorizontal(); }
			}
			
			//Вернет сохраняемый индекс
			IServiceIndex* CIndexAndPosition::GetServiceIndex()			
			{
				SS_TRY
				{
					if (m_pServiceIndex)
					{ return m_pServiceIndex; }
					if (!m_pUnit) CreateUnit();
					m_pServiceIndex = m_pContainersFactory->CreateServiceIndex(m_pUnit, m_pLinguisticProcessorMode, m_pBlackBoxTextStorage);
					return m_pServiceIndex;
				}
				SS_CATCH(L"")
			}

			//Вернет индексируемый индекс
			IIndexationIndex* CIndexAndPosition::GetIndexationIndex()	
			{
				SS_TRY
				{
					if (m_pIndexationIndex)
					{ return m_pIndexationIndex; }
					if (!m_pUnit) CreateUnit();
					m_pIndexationIndex = m_pContainersFactory->CreateIndexationIndex(m_pUnit, m_pLinguisticProcessorMode, m_pBlackBoxTextStorage);
					return m_pIndexationIndex;
				}
				SS_CATCH(L"")
			}

			//Получение позиции индекса в текст блоке
			unsigned int CIndexAndPosition::GetPosition()
			{ 
				if (m_pUnit) 
				{ return m_pUnit->GetPosition(); }
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Position не установлен."); 
				return 0;
			}
			
			//Установка позиции индекса в текст блоке
			void CIndexAndPosition::SetPosition(unsigned int uiPosition)	
			{
				if (!m_pUnit) CreateUnit();
                m_pUnit->SetPosition(uiPosition);
			}

			//Вернет само слово (для вывода пользователю)
			const wchar_t* CIndexAndPosition::GetWord()
			{
				if (m_pUnit)
				{ return m_pUnit->GetWord(); }
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Word не установлен.");
				return NULL;
			}
			
			//Установка слова
			void CIndexAndPosition::SetWord(const wchar_t* szWord)				
			{
				SS_TRY
				{
					if (!m_pUnit) CreateUnit();
					m_pUnit->SetWord(szWord);
				}
				SS_CATCH(L"")
			}

			//Вернет тип подсветки индекса
			SS::Core::Types::IndexationSearch::EHighlightType CIndexAndPosition::GetHighlightType()			
			{
				if (m_pUnit)
				{ return m_pUnit->GetHighlightType(); }
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"HighlightType не установлен.");
				return SS::Core::Types::IndexationSearch::hltNone;
			}

			//Установит тип подсветки индекса
			void CIndexAndPosition::SetHighlightType(SS::Core::Types::IndexationSearch::EHighlightType hlt)	
			{
				if (!m_pUnit) CreateUnit();
				m_pUnit->SetHighlightType(hlt);
			}

			//Получение порядкового номера в тексте первого символа слова
			unsigned int CIndexAndPosition::GetFirstCharPosition()		
			{
				if (m_pUnit)
				{ return m_pUnit->GetFirstCharPosition(); }
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"FirstCharPosition не установлен.");
				return 0;
			}
			
			//Установление порядкового номера в тексте первого символа слова
			void CIndexAndPosition::SetFirstCharPosition(unsigned int uiPosition)
			{
				if (!m_pUnit) CreateUnit();
				m_pUnit->SetFirstCharPosition(uiPosition);
			}
			
			//Вернет тип подсветки индекса
			unsigned int CIndexAndPosition::GetIsInAnswerSentence()
			{
				if (m_pUnit)
				{ return m_pUnit->GetIsInAnswerSentence(); }
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"IsInAnswerSentence не установлен.");
				return 0;
			}
			
			//Установит тип подсветки индекса
			void CIndexAndPosition::SetIsInAnswerSentence(unsigned int bIn)
			{
				if (!m_pUnit) CreateUnit();
				m_pUnit->SetIsInAnswerSentence(bIn);
			}

			//Получение XML - строки содержимого контейнера
			std::wstring* CIndexAndPosition::GetXML()
			{
				SS_TRY
				{
					if (!m_pUnit)
					{ return NULL; }
					
					int iBase = 10;
					wchar_t Buffer[12];		// 4-х байтовое число занимает не более 11 цифр... 12-й эл-т для 0-символа
					unsigned int uiIndex;
					m_ws.clear();
					
					m_ws = L"<IIndexAndPosition>";
					m_ws += L"<Position>";
					_itow(m_pUnit->GetPosition(), Buffer, iBase);
					m_ws += Buffer;
        			m_ws += L"</Position>";
					m_ws += L"<FirstCharPosition>";
					_itow(m_pUnit->GetFirstCharPosition(), Buffer, iBase);
					m_ws += Buffer;
					m_ws += L"</FirstCharPosition>";
					m_ws += L"<Word>";
					if (m_pUnit->GetWord())
					{ m_ws += m_pUnit->GetWord(); }
					m_ws += L"</Word>";
					m_ws += L"<HighlightType>";
					_itow(m_pUnit->GetHighlightType(), Buffer, iBase);
					m_ws += Buffer;
					m_ws += L"</HighlightType>";
					m_ws += L"<IIndexationIndex>";
					if (m_pIndexationIndex)
					{
						uiIndex = m_pIndexationIndex->GetFirstIndex();
						while (uiIndex)
						{
							_itow(uiIndex, Buffer, iBase);
							m_ws += Buffer;
							uiIndex = m_pIndexationIndex->GetNextIndex();
						}
					}
					m_ws += L"</IIndexationIndex>";
					m_ws += L"<IServiceIndex>";
					if (m_pServiceIndex)
					{
						uiIndex = m_pServiceIndex->GetViewIndex()->m_DictionaryIndex;
						_itow(uiIndex, Buffer, iBase);
						m_ws += Buffer;
					}
					m_ws += L"</IServiceIndex>";
					m_ws += L"</IIndexAndPosition>";
					return &m_ws;
				}
				SS_CATCH(L"")
			}

			///Установка соответствующего юнита
			void CIndexAndPosition::SetUnit(IUnit* pUnit)
			{
				m_pUnit = pUnit;
			}

			///Получить указатель на юнит
			IUnit* CIndexAndPosition::GetUnit()
			{
				return m_pUnit;
			}

			///Создание юнита
			void CIndexAndPosition::CreateUnit()
			{
				m_pUnit = m_pBlackBoxTextStorage->CreateUnit();
				m_pUnit->SetPosition(0);
				m_bDelUnit = true;
			}

			//Содержит ли класс хотя-бы один индекс (метод - не интерфейсный)
			bool CIndexAndPosition::IsValid()
			{
				SS::Interface::Core::BlackBox::IIndex*			pIndex = NULL;
				SS::Interface::Core::BlackBox::IDictionaryIndex* pDictionaryIndex = NULL;
				SS::Dictionary::Types::TDictionaryIndex			oInd;

				pIndex = m_pUnit->GetFirstIndex();
				if (!pIndex) 
					return false;
				pDictionaryIndex = pIndex->GetDictionaryIndex();
				if (!pDictionaryIndex) 
					return false;
				if ( !(pDictionaryIndex->GetFirst(&oInd)) )
					return false;

				return true;
			}

			///Приказывает не удалять созданый Unit
			void CIndexAndPosition::NotDeleteUnit()
			{
				m_bDelUnit = false;	
			}

		}
	}
}