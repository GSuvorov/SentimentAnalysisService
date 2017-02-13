// QueryResult.cpp
#include "StdAfx.h"
#include "queryresult.h"

namespace SS
{
	namespace Core
	{
		namespace CommonContainers
		{
			///Конструктор
			CQueryResult::CQueryResult() : m_pContainersFactory(NULL)
			{
				SS_TRY
				{
					m_Iter = m_lpQueryIndex.end();
				}
				SS_CATCH(L"")
			}

			///Деструктор
			CQueryResult::~CQueryResult()
			{
				Clear(); 
			}

			//МЕТОДЫ IBase
			ULONG CQueryResult::Release()
			{
				delete this;
				return 0;
			}

			HRESULT CQueryResult::QueryInterface(REFIID pIID, void** pBase)	//приведение к интерфейсу с поданным REFIID. 
				//pBase будет NULL, если данный интерфейс не поддерживается
			{
				if (pIID==IID_QueryResult)
				{
					*pBase = (IQueryResult*)this;
				}
				else if( pIID == IID_Base )
				{
					*pBase=static_cast< SS::Interface::IBase* >( this );
				}
				else 
					*pBase = NULL;
				return S_OK;
			}
			
			//другие методы
			void CQueryResult::Clear()						// очистка хранилища (удаляет все индексы)
			{
				SS_TRY

					m_oAbsentWords.clear();

					for (m_Iter = m_lpQueryIndex.begin(); m_Iter != m_lpQueryIndex.end(); m_Iter++)
					{
						if ((*m_Iter)) 
						{
							delete (*m_Iter);
						}						
					}

					m_lpQueryIndex.clear();
				SS_CATCH(L"")
			}
			
			IQueryIndex* CQueryResult::AddQueryIndex()		// добавление индекса в хранилище (добавит себе индекс и вернет указатель на него)
			{
				SS_TRY
				{
					IQueryIndex* pQueryIndex = m_pContainersFactory->CreateQueryIndex();
					m_lpQueryIndex.push_back(pQueryIndex);
					return pQueryIndex;
				}
				SS_CATCH(L"")
				return NULL;
			}
			
			IQueryIndex* CQueryResult::GetFirstQueryIndex()	// получение первого индекса коллекции (0 - если нет)
			{
				SS_TRY
				{
					if (m_lpQueryIndex.empty())
					{ return NULL; }
					m_Iter = m_lpQueryIndex.begin();
                    return (*m_Iter);
				}
				SS_CATCH(L"")
				return NULL;
			}
			
			IQueryIndex* CQueryResult::GetNextQueryIndex()	// получение очередного индекса коллекции (не забывать получение первого индекса) (0 - если закончились)
			{
				SS_TRY
					if (m_Iter == m_lpQueryIndex.end())
					{
						//SS_THROW(L" Не вызван метод GetFirstQueryIndex() ")
						SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L" Не вызван метод GetFirstQueryIndex() ");
						return NULL;
					}
					m_Iter++;
					if (m_Iter == m_lpQueryIndex.end())
					{
						return NULL;
					}
					return (*m_Iter);
				SS_CATCH(L"")
				return NULL;
			}

			/// вернет список отсутствующих слов
			SS::Core::Types::IndexationSearch::TAbsentWords* CQueryResult::GetAbsentWords()
			{
				SS_TRY
					return &m_oAbsentWords;
				SS_CATCH(L"")
			}

		}
	}
}