//Container.h
#pragma once

#include <vector>

namespace SS
{
	namespace Syntax
	{
		namespace BoardElements
		{
			///Шаблонный класс реализующий операции с набором данных фиксированой длинны
			/**
			   Если Т класс, то он должен иметь конструктор без параметров.
			   Всего можно добавить 10 элементов (10 раз вызвать Add()) т.к места виртуально удалённых 
			   не восстанавливаются.
			*/
			template<typename T, unsigned int MAX_COUNT = 10>
			class CContainer
			{
			private:
				struct SPair 
				{
					T oT;
					bool bInUse;
					SPair() : bInUse(false)/*, oT()*/ {};
				};
				///Хранилище элементов
				mutable std::vector<SPair> m_vElements;
				///Текущий элемент
				mutable typename std::vector<SPair>::iterator m_itCurr;
				///Коллекция для инициализации итераторов
				mutable std::vector<SPair> m_NullCollection;
				///Элемент за последним
				typename std::vector<SPair>::iterator m_itEnd;

			private:
				///Поиск первого используемого элемента
				bool SetUseElement() const;

			public:
				///Конструктор
				CContainer();
				///Конструктор копирования
				CContainer(const CContainer<T, MAX_COUNT> & c);
				///Деструктор
				~CContainer(){};
				///Оператор=
				void operator=(const CContainer<T, MAX_COUNT> & c);
				///Виртуальное добавление элемента
				T* Add();
				///Получить указатель на первый элемент
				T* GetFirst() const;
				///Получить следующий элемент
				T* GetNext() const;
				///Получить следующий элемент от поданого
				T* GetNextForAddress(const T* pT) const;
				///Установить текущим поданый элемент
				T* SetCurrent(const T* pT) const;
				///Получение предыдущего элемента от текущего(без изменения итераторов)
				T* GetPrev() const;
				///Виртуальное удаление текущего элемента из контейнера
				void DeleteCurrent();
				///Очистка контейнера
				void Clear();
				///Восстановление по адресу
				void RestoreForAddress(const T* pT);
				///Возвращает текущий размер контейнера
				unsigned int Size() const;
				///Возвращает true если контейнер пуст
				bool Empty() const;
			};

			///Конструктор
			template<typename T, unsigned int MAX_COUNT>
			CContainer<T, MAX_COUNT>::CContainer()
			{
				m_itCurr =  m_vElements.end();
				m_vElements.reserve(MAX_COUNT);
				for (unsigned int i = 0; i < MAX_COUNT; ++i)
				{
					SPair oPair;
					m_vElements.push_back(oPair);
				}
				m_itEnd = m_vElements.begin();
			}

			///Конструктор копирования
			template<typename T, unsigned int MAX_COUNT>
			CContainer<T, MAX_COUNT>::CContainer(const CContainer<T, MAX_COUNT> & c) 
			{
				m_itCurr = m_vElements.end();
				unsigned int iIndexEnd  = (unsigned int)distance(c.m_vElements.begin(), c.m_itEnd);
				m_vElements = c.m_vElements;
				m_itEnd = m_vElements.begin() + iIndexEnd;
			}

			///Оператор=
			template<typename T, unsigned int MAX_COUNT>
			void CContainer<T, MAX_COUNT>::operator=(const CContainer<T, MAX_COUNT> & c)
			{
				m_itCurr = m_vElements.end();
				unsigned int iIndexEnd  = (unsigned int)distance(c.m_vElements.begin(), c.m_itEnd);
				m_vElements = c.m_vElements;
				m_itEnd = m_vElements.begin() + iIndexEnd;
			}

			///Поиск первого используемого элемента
			template<typename T, unsigned int MAX_COUNT>
			bool CContainer<T, MAX_COUNT>::SetUseElement() const
			{
				for (m_itCurr; m_itCurr != m_itEnd; ++m_itCurr)
				{
					if (m_itCurr->bInUse) 
					{ return true; }
				}
				return false;
			}

			///Виртуальное добавление элемента
			template<typename T, unsigned int MAX_COUNT>
			T* CContainer<T, MAX_COUNT>::Add()
			{
				//Если больше добавлять нельзя
				if (m_itEnd == m_vElements.end())
				{ return NULL; }
				
				m_itEnd->bInUse = true;
				T* pT = &(m_itEnd->oT);
				++m_itEnd;

				return pT;
			}

			///Получить указатель на первый используемый элемент
			template<typename T, unsigned int MAX_COUNT>
			T* CContainer<T, MAX_COUNT>::GetFirst() const
			{
				m_itCurr = m_vElements.begin();
				
				if (SetUseElement())
				{ return &(m_itCurr->oT); }
				
				return NULL;
			}

			///Получить следующий элемент
			template<typename T, unsigned int MAX_COUNT>
			T* CContainer<T, MAX_COUNT>::GetNext() const
			{
				if (m_itCurr ==  m_vElements.end())
				{ return NULL; }

				++m_itCurr;
				if (SetUseElement())
				{ return &(m_itCurr->oT); }

				m_itCurr =  m_vElements.end();
				return NULL;
			}

			///Виртуальное удаление текущего элемента из контейнера
			template<typename T, unsigned int MAX_COUNT>
			void CContainer<T, MAX_COUNT>::DeleteCurrent()
			{
				if (m_itCurr !=  m_vElements.end())
				{ m_itCurr->bInUse = false;	}
			}

			///Очистка контейнера
			template<typename T, unsigned int MAX_COUNT>
			void CContainer<T, MAX_COUNT>::Clear()
			{
				typename std::vector<SPair>::iterator it;
				for (it = m_vElements.begin(); it != m_itEnd; ++it)
				{ it->bInUse = false; }
				m_itEnd = m_vElements.begin();
				m_itCurr =  m_vElements.end();
			}

			///Восстановление по адресу
			template<typename T, unsigned int MAX_COUNT>
			void CContainer<T, MAX_COUNT>::RestoreForAddress(const T* pT)
			{
				typename std::vector<SPair>::iterator it;
				for (it = m_vElements.begin(); it != m_itEnd; ++it)
				{ 
					if ( &(it->oT) == pT )
					{ it->bInUse = true; return; }
				}
			}

			///Возвращает текущий размер контейнера
			template<typename T, unsigned int MAX_COUNT>
			unsigned int CContainer<T, MAX_COUNT>::Size() const
			{
				typename std::vector<SPair>::const_iterator it;
				unsigned int iSize = 0;
				for (it = m_vElements.begin(); it != m_itEnd; ++it)
				{ 
					if (it->bInUse)
					{ ++iSize; }
				}
				return iSize;
			}

			///Возвращает true если контейнер пуст
			template<typename T, unsigned int MAX_COUNT>
			bool CContainer<T, MAX_COUNT>::Empty() const
			{
				return (Size() == 0);
			}

			///Получить следующий элемент от поданого
			template<typename T, unsigned int MAX_COUNT>
			T* CContainer<T, MAX_COUNT>::GetNextForAddress(const T* pT) const
			{
				for (m_itCurr = m_vElements.begin(); m_itCurr != m_itEnd; ++m_itCurr)
				{ 
					if ( &(m_itCurr->oT) == pT )
					{ break; }
				}
				
				if (m_itCurr == m_itEnd)
				{ return NULL; }

				return GetNext();
			}

			///Установить текущим поданый элемент
			template<typename T, unsigned int MAX_COUNT>
			T* CContainer<T, MAX_COUNT>::SetCurrent(const T* pT) const
			{
				for (m_itCurr = m_vElements.begin(); m_itCurr != m_itEnd; ++m_itCurr)
				{ 
					if ( &(m_itCurr->oT) == pT )
					{ break; }
				}
				if (m_itCurr == m_itEnd)
				{ SS_THROW(L"Элемента не существует."); }
				return &(m_itCurr->oT);
			}

			///Получение предыдущего элемента от текущего(без изменения итераторов)
			template<typename T, unsigned int MAX_COUNT>
			T* CContainer<T, MAX_COUNT>::GetPrev() const
			{
				typename std::vector<SPair>::iterator it = m_itCurr;
				while (it != m_vElements.begin()) 
				{ 
					--it;
					if (it->bInUse) return &(it->oT); 
				}
				return NULL;
			}
			
		}
	}
}
















