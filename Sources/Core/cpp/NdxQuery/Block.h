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

///	Реализация IBlock
/**
	Класс представляет блок слов, входящих в супертрансформацию
*/
class CBlock :
	public SS::Interface::Core::NdxSE::IBlock
{
public:
	///	Конструктор
	CBlock(void);

	///	Деструктор
	virtual ~CBlock(void);

	///	Получение количества слов блока
	/**
		\return	Количество слов в блоке
	*/
	virtual unsigned int GetWordCount();

	///	Получение слова по номеру (zero-based)
	/**
		Не уничтожать выданный указатель!!!
		\param[in]	uiIndex	Номер слова
		\return	Слово
	*/
	virtual SS::Interface::Core::NdxSE::IWord* GetWord(unsigned int uiIndex);


	virtual	const wchar_t*		ToString();


	virtual void	DebugView();

	///	Добавление слова в блок
	/**
		\param[in]	Word	Указатель на добавляемое слово, не проверяется на NULL, не должен быть NULL
	*/
	void	AddWord(SS::Interface::Core::NdxSE::IWord*	Word);

	///	Удаление блока по указателю
	/**
		Используется в STL алгоритмах при работе с блоками
		\param[in]	Block	Указатель на удаляемый объект
	*/
	static	void	Del(SS::Interface::Core::NdxSE::IBlock*	Block);

	///	Сброс объекта
	/**
		Удаляет все хранимые слова, очищая объект
	*/
	void	Clear();

private:
	///	Вектор для хранения указателей на слова
	typedef std::vector<SS::Interface::Core::NdxSE::IWord*>	v_Word;
	///	Хранилище слов
	v_Word			m_Words;
	///	Строковое представление для отладки
	std::wstring	m_AsString;
};

}
}
}
}


