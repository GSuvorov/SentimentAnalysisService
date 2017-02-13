#pragma once
#include "./Console.h"
#include "./Word.h"
#include "./Block.h"
#include "./Sequence.h"
#include "./Variant.h"
#include "./SuperTemplate.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxQuery
{

///	класс для вкладывания объектов друг в друга
///	объекты более высокого порядка должны создаваться и заполняться только
///	если есть чем их заполнять
///	variant	sequence	word
///			block		word
class CObjectInserter
{
public:
	///	конструктор
	__forceinline	CObjectInserter(
		CSuperTemplate*	_pST,
		v_Variant*		_pvVariants)
		:m_pST(_pST), m_pvVariants(_pvVariants)
	{
		if (m_pST == NULL) throw std::wstring(L"ERROR!!!: _pST == NULL !!!");
		if (m_pvVariants == NULL) throw std::wstring(L"ERROR!!!: _pvVariants == NULL !!!");
	}

	///	деструктор
	__forceinline	~CObjectInserter()
	{
		//	если остался не сложенный в SuperTemplate объект, это ошибка
		if (m_apWord.get()		!= NULL		||
			m_apAndBlock.get()	!= NULL		||
			m_apOrBlock.get()	!= NULL		||
			m_apSequence.get()	!= NULL		||
			m_apVariant.get()	!= NULL)
				ToConsole(L"\n.\nERROR!!!: _pST == NULL !!!\n.\n.");
	}

	///	выдает текущее слово (всегда не NULL)
	__forceinline CWord*	current_word()
	{
		//	если нечего выдать то создаём
		if (m_apWord.get() == NULL) m_apWord.reset(new CWord());
		//	выдаём
		return m_apWord.get();
	}

	///	выдает текущую последовательность может быть NULL
	__forceinline CSequence*	current_sequence()
	{
		return m_apSequence.get();
	}

	///	складывает слово в AND-блок
	__forceinline void	Word2AndBlock()
	{
		//	если нечего класть то дальше ничего не делаем
		if (m_apWord.get() == NULL) return;
		//	если некуда класть то создаём
		if (m_apAndBlock.get() == NULL) m_apAndBlock.reset(new CBlock());
		//	добавляем
		m_apAndBlock->AddWord(m_apWord.release());
	}

	///	складывает слово в OR-блок
	__forceinline void	Word2OrBlock()
	{
		//	если нечего класть то дальше ничего не делаем
		if (m_apWord.get() == NULL) return;
		//	если некуда класть то создаём
		if (m_apOrBlock.get() == NULL) m_apOrBlock.reset(new CBlock());
		//	добавляем
		m_apOrBlock->AddWord(m_apWord.release());
	}

	///	складывает слово в последовательность
	__forceinline void	Word2Sequence()
	{
		//	если нечего класть то дальше ничего не делаем
		if (m_apWord.get() == NULL) return;
		//	если некуда класть то создаём
		if (m_apSequence.get() == NULL) m_apSequence.reset(new CSequence());
		//	добавляем
		m_apSequence->AddWord(m_apWord.release());
	}

	///	складывает AND-блок в SuperTemplate
	__forceinline void	AndBlock2ST()
	{
		//	если нечего класть то дальше ничего не делаем
		if (m_apAndBlock.get() == NULL) return;
		m_pST->SetANDBlock(m_apAndBlock.release());
	}

	///	складывает OR-блок в SuperTemplate
	__forceinline void	OrBlock2ST()
	{
		//	если нечего класть то дальше ничего не делаем
		if (m_apOrBlock.get() == NULL) return;
		//	складываем
		m_pST->AddORBlock(m_apOrBlock.release());
	}

	///	складывает последовательность в вариант
	__forceinline void	Sequence2Variant()
	{
		//	если нечего складывать то выходим
		if (m_apSequence.get() == NULL) return;
		//	если некуда складывать то создаём
		if (m_apVariant.get() == NULL) m_apVariant.reset(new CVariant());
		//	складываем
		m_apVariant->AddSequence(m_apSequence.release());
	}

	///	складываем вариант в SuperTemplate
	__forceinline void	Variant2ST()
	{
		//	если нечего складывать то выходим
		if (m_apVariant.get() == NULL) return;
		//	складываем
		m_pvVariants->push_back(m_apVariant.release());
	}

private:
	///	указатель на SuperTemplate
	CSuperTemplate*				m_pST;
	///	указатель на вектор вариантов
	v_Variant*					m_pvVariants;
	///	смарт поинтеры для объектов
	std::auto_ptr<CWord>		m_apWord;
	std::auto_ptr<CBlock>		m_apAndBlock;
	std::auto_ptr<CBlock>		m_apOrBlock;
	std::auto_ptr<CSequence>	m_apSequence;
	std::auto_ptr<CVariant>		m_apVariant;
};


}
}
}
}