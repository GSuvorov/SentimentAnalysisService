#include "stdafx.h"
#include ".\SuperTemplate.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxQuery
{

CSuperTemplate::CSuperTemplate(void)
:m_AndBlock(NULL)
{}

CSuperTemplate::~CSuperTemplate(void)
{
	Clear();
}

SS::Interface::Core::NdxSE::IBlock* CSuperTemplate::GetANDBlock()
{
	return m_AndBlock.get();
}

unsigned int CSuperTemplate::GetORBlockCount()
{
	return static_cast<unsigned int>(m_OrBlocks.size());
}

SS::Interface::Core::NdxSE::IBlock* CSuperTemplate::GetORBlock(unsigned int uiIndex)
{
	//	Проверяем превышение индекса
	if (uiIndex >= m_OrBlocks.size())
	{
		ToConsole(L"ERROR!!! invalid index");
		return NULL;
	}

	return m_OrBlocks[uiIndex];
}

void	CSuperTemplate::SetANDBlock(SS::Interface::Core::NdxSE::IBlock*	pBlock)
{
	m_AndBlock.reset(pBlock);
}

void	CSuperTemplate::AddORBlock(SS::Interface::Core::NdxSE::IBlock*	pBlock)
{
	if (pBlock != NULL)
		m_OrBlocks.push_back(pBlock);
	else
		ToConsole(L"ERROR!!! NULL assigning");
}

void	CSuperTemplate::Clear()
{
	m_AndBlock.reset();
	//	Уничтожаем все OR-блоки
	std::for_each(m_OrBlocks.begin(), m_OrBlocks.end(), CBlock::Del);
	m_OrBlocks.clear();
}

const wchar_t* CSuperTemplate::ToString()
{
	const unsigned int buffer_size = 1024*10;
	wchar_t	buf[buffer_size];
	unsigned int counter = 0;
	counter += _snwprintf(buf+counter, buffer_size-counter, L"\n[[ SUPER-TEMPLATE ]]\n");
	
	//	AND-block
	counter += _snwprintf(buf+counter, buffer_size-counter, L"[AND-BLOCK]");
	if (m_AndBlock.get() != NULL) counter += _snwprintf(buf+counter, buffer_size-counter, L"%s", m_AndBlock->ToString());

	//	OR-blocks
	counter += _snwprintf(buf+counter, buffer_size-counter, L"\n.\n[OR-BLOCKS]");
	for(v_Block::iterator
		itb_block = m_OrBlocks.begin(),
		ite_block = m_OrBlocks.end();
		itb_block != ite_block;
		++itb_block)
	{
		counter += _snwprintf(buf+counter, buffer_size-counter, L"%s", (*itb_block)->ToString());
	}

	m_AsString.assign(buf);

	return m_AsString.c_str();
}


void	CSuperTemplate::DebugView()
{
	ToConsole(ToString());
}


}
}
}
}

