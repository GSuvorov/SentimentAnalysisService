#pragma once

namespace SS
{
namespace Interface
{
namespace Core
{
namespace Common
{

	/// Определяет основные операции с коллекциями
	class IEnumerator
	{
	public:
		///Возвращает true, если коллекция пустая
		virtual unsigned int Empty() = 0;

		///Возвращает число элементов коллекции
		virtual unsigned int Count() = 0;

		///Очищает коллекцию
		virtual void Clear() = 0;

		///Устанавливает указатель на начало коллекции 
		virtual unsigned int Reset() = 0;

		///Устанавливает указатель на следующий элемент коллекции
		virtual unsigned int MoveNext() = 0;
	};
}
}
}
}
