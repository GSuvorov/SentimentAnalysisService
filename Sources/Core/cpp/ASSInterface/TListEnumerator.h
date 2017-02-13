#pragma once

#include "IEnumerator.h"
#include <list>
#include <algorithm>

namespace SS
{
namespace Core
{
namespace Types
{

	/// Реализует базовые операции с коллекциями на основе списка
	template <class T> class TListEnumerator : public SS::Interface::Core::Common::IEnumerator
	{
		/*! \typedef TCollection
		*  \brief    Определяет коллекцию элементов на основе списка и типа-параметра T данного шаблонного класса
		*/
		typedef std::list<T> TCollection;

		/// Коллекция элементов на основе списка и типа-параметра T данного шаблонного класса
		TCollection m_lstCollection;

		/// Указатель текущего элемента коллекции
		TListEnumerator<T>::TCollection::iterator it_currElement;

		/// Устанавливает указатель текущего элемента на начало коллекции)
		void Init()
		{
			it_currElement = m_lstCollection.begin();	
		};

	public:

		/// Инициализирует данные класса (устанавливает указатель текущего элемента на начало коллекции)
		TListEnumerator()
		{
			Init();
		};

		///Возвращает true, если коллекция пустая
		unsigned int Empty()
		{
			return m_lstCollection.empty();	
		};

		///Возвращает число элементов коллекции
		unsigned int Count()
		{
		
			return (unsigned int)m_lstCollection.size();
		};

		///Очищает коллекцию
		void Clear()
		{
			m_lstCollection.clear();
		};

		///Устанавливает указатель на начало коллекции 
		unsigned int Reset()
		{
			Init();
			return (m_lstCollection.empty()) ? false : true;
		};

		///Устанавливает указатель на следующий элемент коллекции
		unsigned int MoveNext()
		{
		
			if ((m_lstCollection.empty()) ||
				(it_currElement == --m_lstCollection.end())) 
				return false;
			else
			{
				it_currElement ++;
				return true;
			};
		};

		///Добавляет элемент в коллекцию
		void Add(T Element)
		{
			m_lstCollection.push_back(Element);
			it_currElement = --m_lstCollection.end();
		};

		///Возвращает текущий элемент коллекции (NULL, если она пустая)
		T* GetCurrent()
		{
			return ((m_lstCollection.empty()) ||
				(it_currElement == m_lstCollection.end())) ?
				NULL : &(*it_currElement);
		};

		void Find(T value)
		{
			it_currElement = std::find(m_lstCollection.begin(), m_lstCollection.end(), value);	
		};
	};
}
}
}
