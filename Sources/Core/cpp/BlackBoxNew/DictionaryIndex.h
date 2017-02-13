#pragma once

#include "..\ASSInterface\IBlackBoxDictionaryIndex.h"
using namespace SS::Interface::Core::BlackBox;
using namespace SS::Dictionary::Types;

#include <vector>


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CDictionaryIndex : public IDictionaryIndex
{
public:
	CDictionaryIndex();
	virtual ~CDictionaryIndex();

public:
	///управление значением составной/нет индекс
	void SetIsComposite(bool blValue);
	///составной/нет индекс
	bool IsComposite();
	///добавление индекса в иерархию
	void AppendIndex(TDictionaryIndex oIndex);
	///очистка иерархии индексов
	void Clear();
	///удаляет текущий индекс
	void RemoveIndex();

	///получение первого(верхнего) индекса иерархии
	bool GetFirst(TDictionaryIndex* pIndex);
	///получение следующего индекса иерархии
	bool GetNext(TDictionaryIndex* pIndex);
	///получение первого(верхнего) индекса иерархии
	TDictionaryIndex GetFirst();

private:
	TDictionaryIndex m_oIndex;
};

}
}
}
