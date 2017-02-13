#pragma once
#include "..\ASDInterface\INDXQuery.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxQuery
{

///	Реализация IVariant
/**
	Представляет собой вариант запроса
*/
class CVariant :
	public SS::Interface::Core::NdxSE::IVariant
{
public:
	///	Конструктор
	CVariant(void);

	///	Деструктор
	virtual ~CVariant(void);

	///	Возвращает число последовательностей
	/**
		\return	Число последовательностей
	*/
	virtual unsigned int GetSequenceCount();

	///	Возвращает последовательность по номеру. Если номер неправильный возвращает NULL
	/**
		\param[in]	uiIndex	Номер последовательности
		\return	Последовательность
	*/
	virtual SS::Interface::Core::NdxSE::ISequence* GetSequence(unsigned int uiIndex);

	virtual	const wchar_t*	ToString();

	virtual void	DebugView();

	///	Добавляет последовательность
	/**
		\param[in]	Seq	Добавляемая последовательность. Указатель не проверяется на NULL, не должен быть NULL
	*/
	void	AddSequence(SS::Interface::Core::NdxSE::ISequence*	Seq);

	///	Уничтожает указатель на IVariant
	/**
		Уничтожает объект по указателю, используется в алгоритмах STL
		\param[in]	Var	Удаляемый указатель
	*/
	static void Del(SS::Interface::Core::NdxSE::IVariant* Var);

	///	Очищает объект
	/**
		Уничтожает хранимые последовательности
	*/
	void	Clear();

private:
	typedef std::vector<SS::Interface::Core::NdxSE::ISequence*>	v_Sequences;
	///	Хранилище последовательностей
	v_Sequences				m_Sequences;
	///	Строковый вид слова для отладки
	std::wstring			m_AsString;
};

///	Тип контейнера для хранения IVariant*
typedef std::vector<SS::Interface::Core::NdxSE::IVariant*>	v_Variant;

}
}
}
}


