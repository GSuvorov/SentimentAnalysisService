#include "stdafx.h"
#include ".\Block.h"
#include ".\Word.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxQuery
{

CBlock::CBlock(void)
{
}

CBlock::~CBlock(void)
{
	Clear();
}

unsigned int CBlock::GetWordCount()
{
	return static_cast<unsigned int>(m_Words.size());
}

SS::Interface::Core::NdxSE::IWord* CBlock::GetWord(unsigned int uiIndex)
{
	//	Проверяем, не превышен ли индекс
	if (uiIndex >= m_Words.size()) return NULL;
	return m_Words[uiIndex];
}

void	CBlock::AddWord(SS::Interface::Core::NdxSE::IWord*	Word)
{
	if (Word == NULL)
	{
		ToConsole(L"ERROR!!!");
		return;
	}

	m_Words.push_back(Word);
}

void	CBlock::Del(IBlock*	Block)
{
	if (Block != NULL) delete Block;
}

void	CBlock::Clear()
{
	std::for_each(m_Words.begin(), m_Words.end(), CWord::Del);
	m_Words.clear();
}

const wchar_t* CBlock::ToString()
{
	m_AsString.clear();
	const unsigned int buffer_size = 1024;
	wchar_t	buf[buffer_size];
	unsigned int counter = 0;
	counter += _snwprintf(buf+counter, buffer_size-counter, L"\nBLOCK\n");

	for(v_Word::iterator
		itb_word = m_Words.begin(),
		ite_word = m_Words.end();
		itb_word != ite_word;
		++itb_word)
	{
		if (*itb_word == NULL)
		{
			counter += _snwprintf(buf+counter, buffer_size-counter, L"{invalid word}\n");
			continue;
		}

		counter += _snwprintf(buf+counter, buffer_size-counter, L"%s\n", (*itb_word)->ToString());
	}

	m_AsString.assign(buf);

	return m_AsString.c_str();
}


void	CBlock::DebugView()
{
	ToConsole(ToString());
}

}
}
}
}