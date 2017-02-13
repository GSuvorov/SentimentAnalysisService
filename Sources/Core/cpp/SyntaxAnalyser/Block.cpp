#include "stdafx.h"
#include "Block.h"

using namespace std;

namespace SS
{
	namespace Syntax
	{
		namespace BoardElements
		{
			///Максимальное количество гипотез, генерируемых при разборе
			static const unsigned int MaxVersionsForIndexationEng = 20;
			static const unsigned int MaxVersionsForIndexationRus = 250;
			static const unsigned int MaxVersionsForQuery = 10000;

			//============================МЕТОДЫ ДЛЯ РАБОТЫ СО СТАТИЧЕСКИМИ ГИПОТЕЗАМИ============================//

			///Создание статических гипотез
			void CBlock::CreateVersions()
			{
				SS_TRY
				{
					//Очистить хранилище статических гипотез
					m_Versions.clear();
					//Гипотеза
					CBlockVersion oVersion;
					//Установить гипотезу					
					m_oDynamicVersionGenerator.SetVersion(&oVersion);
					//Записать гипотезы
					for (bool b = m_oDynamicVersionGenerator.GetFirstVersion(); b; b = m_oDynamicVersionGenerator.GetNextVersion())
					{ m_Versions.push_back(oVersion); }
					//Установить текущую гипотезу
					m_itCurrentVersionStatic = m_Versions.begin();
				}
				SS_CATCH(L"")
			}

			///Переключение статических гипотез
			bool CBlock::SwitchVersion()
			{
				SS_TRY
				{
					if (m_itCurrentVersionStatic == m_Versions.end())
					{
						m_itCurrentVersionStatic = m_Versions.begin();
						return false;
					}
					//SetVersion( &(*m_itCurrentVersionStatic) );
					++m_itCurrentVersionStatic;
				}		
				SS_CATCH(L"")
				return true;
			}

			///Установить текущую статическую гипотезу
			void CBlock::SetCurrentVersion()
			{
				SS_TRY
				{
					if (UnionBlocks()) 
					{
						SetVersion(&(m_oBestVerBothBlock));
					}
					else
					{
						SetVersion(&(*m_itCurrentVersionStatic));
						m_itBestVersionStatic = m_itCurrentVersionStatic;
					}
				}
				SS_CATCH(L"")
			}

			///Определяет и устанавливает наиболее подходящую гипотезу разбора
			void CBlock::SetBestVersion()
			{
				SS_TRY
				{
					if (UnionBlocks()) 
					{ 
						SetVersion(&(m_oBestVerBothBlock)); 
					}
					else
					{
						m_itBestVersionStatic = min_element(m_Versions.begin(), m_Versions.end());
						SetVersion( &(*m_itBestVersionStatic) );
					}
				}
				SS_CATCH(L"")
			}

			///Выставить гипотезу по номеру
			void CBlock::SetHypothesis(unsigned int iNumber)
			{
				SS_TRY
				{
					if (m_Versions.empty()) 
					{ SS_THROW(__WFUNCTION__ AND L"Гипотез нет."); }

					if ( (iNumber < 1) || (iNumber > m_Versions.size()) )
					{ 
						SAVE_LOG(__WFUNCTION__ AND L"Гипотезы под таким номером не существует. Устанавливаю 1."); 
						iNumber = 1;
					}

					std::list<CBlockVersion>::iterator itVer;
					int iCurrNumber = 0;
					for (itVer = m_Versions.begin(); itVer != m_Versions.end(); ++itVer)
					{
						iCurrNumber++;
						if (iCurrNumber == iNumber)
						{
							m_itCurrentVersionStatic = itVer;
							m_itBestVersionStatic = m_itCurrentVersionStatic;
							return;
						}
					}
				}
				SS_CATCH(L"")
			}

			//================================МЕТОДЫ ДЛЯ РАБОТЫ С ДИНАМИЧЕСКИМИ ГИПОТЕЗАМИ================================//

			///Переключение динамических гипотез
			bool CBlock::SwitchVersionDynamic(ExpertModel::ParsingType type)
			{
				SS_TRY
				{
					//Если индексация для английского
					if (type == ExpertModel::ptFullParsingEnglish)
					{
						++m_iVersionCount;
						if (m_iVersionCount > MaxVersionsForIndexationEng)
						{
							m_iVersionCount = 0;
							return false;
						}
					}
					//Если индексация для русского
					else if (type == ExpertModel::ptFullParsingRussian)
					{
						++m_iVersionCount;
						if (m_iVersionCount > MaxVersionsForIndexationRus)
						{
							m_iVersionCount = 0;
							return false;
						}
					}
					//Если разбор запроса (русского или английского)
					else if ( (type == ExpertModel::ptQueryAnalyzeEnglish) || (type == ExpertModel::ptQueryAnalyzeRussian) )
					{
						++m_iVersionCount;
						if (m_iVersionCount > MaxVersionsForQuery)
						{
							m_iVersionCount = 0;
							return false;
						}
					}

					//Динамическая генерация
					if (m_iVersionCount == 1)
					{
						if (!m_oDynamicVersionGenerator.GetFirstVersion())
						{ return false;	}
					}
					else
					{
						if (!m_oDynamicVersionGenerator.GetNextVersion())
						{ 
							return false; 
						}
					}
					
					//Установка сгенерированной гипотезы
					SetVersion(&(m_oCurrentVersionDynamic));
				}							
				SS_CATCH(L"")
				return true;
			}

			///Установка лучшей динамической гипотезы
			void CBlock::SetBestVersionDynamic()
			{
				SS_TRY
				{
					if ( !m_oBestVersionDynamic.GetFirst() || (m_oCurrentVersionDynamic < m_oBestVersionDynamic) )
					{ m_oBestVersionDynamic = m_oCurrentVersionDynamic; }
				}
				SS_CATCH(L"")
			}

			///Окончательное выставление (динамической)лучшей гипотезы
			void CBlock::SetVersionFinal()
			{
				SS_TRY
				{
					if (UnionBlocks())
					{ SetVersion(&(m_oBestVerBothBlock)); }
					else
					{ SetVersion(&(m_oBestVersionDynamic));}
				}
				SS_CATCH(L"")
			}

			//======================================ОБЩИЕ МЕТОДЫ=========================================//

			///Инициализация блока новыми данными
			void CBlock::Init(BoardElementsIterator first, BoardElementsIterator last)
			{
				ClearData();
				m_first = first;
				m_last = last;
				m_oDynamicVersionGenerator.SetBlockRange(m_first, m_last);
				m_oDynamicVersionGenerator.SetVersion(&m_oCurrentVersionDynamic);
			}

			///Добавление блока
			void CBlock::AddBlock(CBlock* pBlockToAdd)
			{
				SS_TRY
				{	
					if (!pBlockToAdd) 
					{
						SAVE_LOG(SS_MESSAGE AND __WFUNCTION__ AND L"Отсутствует добавляемый блок.");
						return;
					}
					//Добавляемый блок
					m_pAddBlock = pBlockToAdd;

					if ( !m_oBestVerBothBlock.IsValid() ) 
					{
						SaveVerBoth(m_oBestVerBothBlock);
					}
					else
					{
						//SS_THROW(L"Такого не должно быть.");
						SaveVerBoth(m_oCurrVerBothBlock);
						if ( m_oCurrVerBothBlock < m_oBestVerBothBlock )
						{
							m_oBestVerBothBlock = m_oCurrVerBothBlock;
						}
					}
				}
				SS_CATCH(L"")
			}

			///Возвращает лучшую гипотезу блока на данный момент
			CBlockVersion & CBlock::GetBestVersion()
			{
				SS_TRY
				{			
					if (m_Versions.empty())
					{ return m_oBestVersionDynamic; }
					else
					{ return (*m_itBestVersionStatic); }
				}
				SS_CATCH(L"")
			}

			///Проверка блока на пустотность
			bool CBlock::Empty() const
			{
				return (m_first == m_last);
			}

			///Проверка принадлежности элемента данному блоку
			bool CBlock::Contain(BoardElementsIterator itElem) const
			{
				return ( (itElem >= m_first) && (itElem < m_last) );
			}

			//======================================ОБЩИЕ Private МЕТОДЫ========================================//

			///Очистить данные блока			
			void CBlock::ClearData()
			{
				m_oBestVersionDynamic.ClearAll();
				m_oCurrentVersionDynamic.ClearAll();
				m_oCurrVerBothBlock.ClearAll();
				m_oBestVerBothBlock.ClearAll();
				m_Versions.clear();

				m_pAddBlock = NULL;
				m_itCurrentVersionStatic = m_Versions.end();
				m_itBestVersionStatic = m_Versions.end();
				m_iVersionCount = 0;
				m_CountATExpert = 0;
			}
			
			///Проставляет всем элементам доски новую гипотезу
			void CBlock::SetVersion(CBlockVersion* pVersion)
			{
				SS_TRY
				{
                    BoardElementsIterator iElem = m_first;
					for (PVERELEMENT pElem = pVersion->GetFirst(); pElem && (iElem != m_last); pElem = pVersion->GetNext(), ++iElem)
					{ (*iElem)->SetVersion(pElem); }
				}
				SS_CATCH(L"")
			}
			
			///Реальное объединение двух блоков, если есть необходимость
			bool CBlock::UnionBlocks()
			{
				SS_TRY
				{
					//Если гипотеза пустая то выход (т е добавления блоков не случилось)
					if ( !m_oBestVerBothBlock.IsValid() )
					{ return false; }
					//Изменение границ блока
					m_first = m_pAddBlock->m_first;
					//Делаем добавляемый блок "пустым"
					m_pAddBlock->m_last = m_pAddBlock->m_first;
				}	
				SS_CATCH(L"")
				return true;
			}

			///Сохранение гипотезы с разбором обоих блоков, которые объединяются
			void CBlock::SaveVerBoth(CBlockVersion & oVersion)
			{
				SS_TRY
				{
					oVersion.ClearAll();
					BoardElementsIterator itElem;
					for (itElem = m_pAddBlock->GetFirst(); itElem != m_pAddBlock->GetLast(); ++itElem)
					{ oVersion.AddElement()->Assign((*itElem)->GetVerElem()); }
					for (itElem = m_first; itElem != m_last; ++itElem)
					{ oVersion.AddElement()->Assign((*itElem)->GetVerElem()); }
				}
				SS_CATCH(L"")
			}
			
		
		}
	}
}