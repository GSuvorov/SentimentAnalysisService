//IndexationIndex.cpp

#include "StdAfx.h"
#include ".\indexationindex.h"

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{
			//Конструктор
			CIndexationIndex::CIndexationIndex( IUnit* pUnit, TLinguisticProcessorMode* pLinguisticProcessorMode, 
												IBlackBoxTextStorage* pBlackBoxTextStorage) 
												: 
												m_pUnit(pUnit), m_pLinguisticProcessorMode(pLinguisticProcessorMode),
												m_pBlackBoxTextStorage(pBlackBoxTextStorage), m_pCurrIndex(NULL)
			{
			}

			//Деструктор
			CIndexationIndex::~CIndexationIndex()
			{
			}

			//Получение первого индекса коллекции (0 - если нет)
			unsigned int CIndexationIndex::GetFirstIndex()
			{
				SS_TRY
				{
					m_pCurrIndex = m_pUnit->GetFirstIndex();
					return GetIndex();
				}
				SS_CATCH(L"")
			}

			//Получение очередного индекса коллекции (0 - если закончились)
			unsigned int CIndexationIndex::GetNextIndex()
			{
				SS_TRY
				{
					if(m_pLinguisticProcessorMode->m_bIsLiteMode)
						return 0;

					m_pCurrIndex = m_pUnit->GetNextIndex();
					return GetIndex();
				}
				SS_CATCH(L"")
			}

			//Очистка коллекции (удалит все индексы)
			void CIndexationIndex::Clear()							
			{
				//m_pUnit->ClearUnit();
				SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Метод не реализован.");
			}

			//Добавление индекса в коллекцию
			void CIndexationIndex::AddIndex(unsigned int uiIndex)
			{
				SS_TRY
				{
					SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Использование функции нежелательно.");

					SS::Interface::Core::BlackBox::IIndex*			pIndex = NULL;
					SS::Interface::Core::BlackBox::IDictionaryIndex* pDictionaryIndex = NULL;
					SS::Interface::Core::BlackBox::IFeature*			pFeature = NULL;
					SS::Dictionary::Types::TDictionaryIndex			oInd;

					pIndex = m_pBlackBoxTextStorage->CreateIndex();
					pDictionaryIndex = pIndex->GetDictionaryIndex();
					oInd.SetDictionaryIndex(uiIndex);
					pDictionaryIndex->AppendIndex(oInd);

					//pFeature = m_pBlackBoxTextStorage->CreateFeature();
					//pFeature->SetMorphoIndex(oViewIndex.m_IDForm);
					//pIndex->AddFeature(pFeature);

					m_pUnit->AddIndex(pIndex);
				}
				SS_CATCH(L"")
			}

			//Оценить применимость типа индекса к заданному режиму
			bool CIndexationIndex::ApplyLinguisticProcessorMode(SS::Dictionary::Types::TDictionaryIndex & oIndex)
			{
				SS_TRY
				{
					if (oIndex.GetIndexType() == SS::Interface::Core::BlackBox::eitMorphoIndex)
					{
						if ( ((m_pLinguisticProcessorMode->GetAnalyseDepthParams())->IsMorpho()) )
							return true; 	
					}
					else if (oIndex.GetIndexType() == SS::Interface::Core::BlackBox::eitSemanticIndex)
					{
						if ( ((m_pLinguisticProcessorMode->GetAnalyseDepthParams())->IsSemantic()) )
							return true;
					}
					else if (oIndex.GetIndexType() == SS::Interface::Core::BlackBox::eitSyntaxIndex)
					{
						if ( ((m_pLinguisticProcessorMode->GetAnalyseDepthParams())->IsSyntax()) )
							return true;
					}
					else if (oIndex.GetIndexType() == SS::Interface::Core::BlackBox::eitSynonymIndex)
					{
						if ( ((m_pLinguisticProcessorMode->GetAnalyseDepthParams())->IsSynonims()) )
							return true;
					}
					
					return false;
				}
				SS_CATCH(L"")
			}

			//Получить очередной индекс
			unsigned int CIndexationIndex::GetIndex()
			{
				SS_TRY
				{
					SS::Interface::Core::BlackBox::IDictionaryIndex*	pDictionaryIndex = NULL;
					SS::Dictionary::Types::TDictionaryIndex			oInd;

					for (m_pCurrIndex; m_pCurrIndex; m_pCurrIndex = m_pUnit->GetNextIndex())
					{
						if ( !(pDictionaryIndex = m_pCurrIndex->GetDictionaryIndex()) )
							SS_THROW(L"Отсутствует IDictionaryIndex." AND m_pUnit->GetWord());

						if ( !(pDictionaryIndex->GetFirst(&oInd)) ) 
						{ 
							//SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Отсутствует первый TDictionaryIndex, хотя IDictionaryIndex существует!" AND pUnit->GetWord());
							return 0;
						}

						//анализ параметров
						if (ApplyLinguisticProcessorMode(oInd))
						{ return oInd.GetDictionaryIndexWithoutFlags(); }
					}
					return 0;
				}
				SS_CATCH(L"")
			}

		}
	}
}

