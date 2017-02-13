#pragma once
#include "..\ASDInterface\INDXQuery.h"
#include ".\Block.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxQuery
{

///	Реализация ISuperTemplate
/**
	Супертрансформация. Содержит один набор AND-блоков и наборы OR-блоков.
*/
class CSuperTemplate :
	public SS::Interface::Core::NdxSE::ISuperTemplate
{
public:
	///	Конструктор
	CSuperTemplate(void);

	///	Деструктор
	virtual ~CSuperTemplate(void);

	///	Получение блока AND
	///	Выдаёт указатель на AND-блок, может выдавать NULL если AND-блок не существует
	///	\return	Указатель на AND-блок
	virtual SS::Interface::Core::NdxSE::IBlock* GetANDBlock();
	
	///	Выдаёт количество OR-блоков
	///	\return	Количество OR-блоков
	virtual unsigned int GetORBlockCount();

	///	Получение блока по номеру (если номер неправильный, вернётся NULL)
	///	Если номер правильный, никогда не вернётся NULL
	///	\param[in]	uiIndex	Номер OR-блока
	///	\return	Указатель на OR-блок
	virtual SS::Interface::Core::NdxSE::IBlock* GetORBlock(unsigned int uiIndex);

	virtual	const wchar_t*	ToString();

	///	Отладочный вывод всего объекта
	virtual void	DebugView();

	///	Задаёт AND-блок. Если он раньше существовал то уничтожается
	///	\param[in]	Block	устанавливаемый AND-блок
	void	SetANDBlock(SS::Interface::Core::NdxSE::IBlock*	pBlock);

	///	Добавляет OR-блок
	///	\param[in]	Block	Блок, добавляемый в хранилище. Если Block == NULL то не добавляется.
	void	AddORBlock(SS::Interface::Core::NdxSE::IBlock*	pBlock);

	///	Очистка объекта
	///	Уничтожает AND-блок и OR-блоки
	void	Clear();

private:
	///	AND-блок
	std::auto_ptr<SS::Interface::Core::NdxSE::IBlock>	m_AndBlock;
	///	Вектор для хранения указателей на блоки
	typedef std::vector<SS::Interface::Core::NdxSE::IBlock*>	v_Block;
	///	Хранилище указателей на OR-блоки
	v_Block											m_OrBlocks;
	///	Строковое представление для отладки
	std::wstring	m_AsString;
};

}
}
}
}


