#pragma once

#include "./TDictionaryManager.h"


namespace SS
{
namespace Interface
{
namespace Core
{
namespace BlackBox
{

/// инкапсулятор индекса, который используется для индексации/поиска
	class IDictionaryIndex
	{
	public:
		///управление значением составной/нет индекс
		virtual void SetIsComposite(bool blValue) = 0;
		///составной/нет индекс
		virtual bool IsComposite() = 0;
		///добавление индекса в иерархию
		virtual void AppendIndex(SS::Dictionary::Types::TDictionaryIndex oIndex) = 0;
		///очистка иерархии индексов
		virtual void Clear() = 0;
		///удаляет текущий индекс
		virtual void RemoveIndex() = 0;

		///получение первого(верхнего) индекса иерархии
		virtual bool GetFirst(SS::Dictionary::Types::TDictionaryIndex* pIndex) = 0;
		///получение следующего индекса иерархии
		virtual bool GetNext(SS::Dictionary::Types::TDictionaryIndex* pIndex) = 0;
		
		///получение первого(верхнего) индекса иерархии
		virtual SS::Dictionary::Types::TDictionaryIndex GetFirst() = 0;
	};

}
}
}
}
