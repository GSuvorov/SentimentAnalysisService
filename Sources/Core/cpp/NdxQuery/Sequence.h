#pragma once
#include "..\ASDInterface\INDXQuery.h"
#include ".\types.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxQuery
{

///	Реализация ISequence
/**
	Класс представляет последовательность слов
*/
class CSequence :
	public SS::Interface::Core::NdxSE::ISequence
{
public:
	///	ctr
	CSequence(void);

	//	интерфейс
	virtual ~CSequence(void);
	virtual unsigned int GetWordCount();
	virtual SS::Interface::Core::NdxSE::IWord* GetWord(unsigned int uiIndex);
	virtual unsigned int GetUnionWordsBlocksCount();
	virtual SS::Interface::Core::NdxSE::SUnionWordsBlockInSequence GetUnionWordsBlock(unsigned int uiIndex);
	virtual BOOL	IsWordInUnion(unsigned int uiIndex, SS::Interface::Core::NdxSE::SUnionWordsBlockInSequence* pUnion);
	virtual	const wchar_t*	ToString();
	virtual void	DebugView();

	//	реализация
	///	добавление слова. Если NULL то не добавится
	void	AddWord(SS::Interface::Core::NdxSE::IWord*	Word);
	///	добавление описания union-блока
	void	AddUnionWordsBlock(SS::Interface::Core::NdxSE::SUnionWordsBlockInSequence const& UnionWordsBlock);
	///	стирает старое, переписывает в себя описания union-блоков из UnionBlockInfo
	void	AssignUnionWordsBlocks(v_UWB& UnionBlockInfo);
	///	удаление объекта по указателю
	static void	Del(ISequence*	Seq);
	///	очистка объекта
	void	Clear();

private:
	///	Вектор для хранения указателей на слова
	typedef std::vector<SS::Interface::Core::NdxSE::IWord*>	v_Word;
	///	Хранилище слов
	v_Word	m_Words;
	///	Вектор для хранения описателей union-блоков
	typedef std::vector<SS::Interface::Core::NdxSE::SUnionWordsBlockInSequence>	v_UWB;
	///	Хранилище union-последовательностей
	v_UWB	m_UnionWordsBlocks;
	///	Строковое представление для отладки
	std::wstring	m_AsString;
};

}
}
}
}


