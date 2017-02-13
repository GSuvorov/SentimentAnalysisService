#include "stdafx.h"
#include ".\Word.h"
#include ".\Sequence.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxQuery
{

CSequence::CSequence(void)
{
}

CSequence::~CSequence(void)
{
	Clear();
}

unsigned int CSequence::GetWordCount()
{
	return static_cast<unsigned int>(m_Words.size());
}

SS::Interface::Core::NdxSE::IWord* CSequence::GetWord(unsigned int uiIndex)
{
#ifdef _DEBUG
	if (uiIndex >= m_Words.size()) SS_THROW(L"CSequence::GetWord invalid uiIndex");
#endif
	return m_Words[uiIndex];
}

unsigned int CSequence::GetUnionWordsBlocksCount()
{
	return static_cast<unsigned int>(m_UnionWordsBlocks.size());
}

SS::Interface::Core::NdxSE::SUnionWordsBlockInSequence CSequence::GetUnionWordsBlock(unsigned int uiIndex)
{
#ifdef _DEBUG
	if (uiIndex >= m_UnionWordsBlocks.size()) SS_THROW(L"CSequence::GetUnionWordBlock: invalid uiIndex");
#endif
	return m_UnionWordsBlocks[uiIndex];
}

void	CSequence::AddUnionWordsBlock(SS::Interface::Core::NdxSE::SUnionWordsBlockInSequence const& UnionWordsBlock)
{
	m_UnionWordsBlocks.push_back(UnionWordsBlock);
}

void	CSequence::AssignUnionWordsBlocks(v_UWB& UnionBlockInfo)
{
	m_UnionWordsBlocks.assign(UnionBlockInfo.begin(), UnionBlockInfo.end());
}

void CSequence::AddWord(SS::Interface::Core::NdxSE::IWord*	Word)
{
	if (Word == NULL)
	{
		ToConsole(L"ERROR!!! NULL assigning");
		return;
	}

	m_Words.push_back(Word);
}

void	CSequence::Del(ISequence*	Seq)
{
	if (Seq != NULL) delete Seq;
}

void	CSequence::Clear()
{
	std::for_each(m_Words.begin(), m_Words.end(), CWord::Del);
	m_Words.clear();
	m_UnionWordsBlocks.clear();
}

BOOL CSequence::IsWordInUnion(unsigned int uiIndex, SS::Interface::Core::NdxSE::SUnionWordsBlockInSequence* pUnion)
{
	//	индекс слишком большой
	if (uiIndex >= (unsigned int)m_Words.size())
	{
		ToConsole(L"\n.\n.\nERROR!!! Слишком большой номер слова!\n.\n.\n.");
		return FALSE;
	}

	//	проходим по юнион блокам, смотрим принадлежит ли слово одному из них
	for(v_UWB::iterator
		itb_ub = m_UnionWordsBlocks.begin(),
		ite_ub = m_UnionWordsBlocks.end();
		itb_ub != ite_ub;
		++itb_ub)
	{
		if ((uiIndex >= itb_ub->uiFirstWordNumber) && (uiIndex <= itb_ub->uiLastWordNumber))
		{
			if (pUnion != NULL) *pUnion = *itb_ub;
			return TRUE;
		}
	}

	return FALSE;
}

const wchar_t* CSequence::ToString()
{
	m_AsString.assign(L"\n{no sequence}\n.");

	if (m_Words.empty()) 
	{
		return m_AsString.c_str();
	}

	const unsigned int buffer_size = 10*1024;
	wchar_t	buf[buffer_size];
	unsigned int counter = 0;
	counter += _snwprintf(buf+counter, buffer_size-counter, L"\n.\nSEQUENCE\n");

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

	if (m_UnionWordsBlocks.empty())
	{
		m_AsString.assign(buf);
		return m_AsString.c_str();
	}

	for(v_UWB::iterator
		itb_uwb = m_UnionWordsBlocks.begin(),
		ite_uwb = m_UnionWordsBlocks.end();
		itb_uwb != ite_uwb;
		++itb_uwb)
	{
		counter += _snwprintf(
			buf+counter,
			buffer_size-counter,
			L"\n.\nUNION-BLOCK, %u'th of %u, positions in sequence: [%u,%u]\n",
			(unsigned int)(itb_uwb-m_UnionWordsBlocks.begin()),
			(unsigned int)m_UnionWordsBlocks.size(),
			itb_uwb->uiFirstWordNumber,
			itb_uwb->uiLastWordNumber);

		for(unsigned int i = itb_uwb->uiFirstWordNumber;
			i <= itb_uwb->uiLastWordNumber;
			++i)
		{
			if (i >= m_Words.size())
			{
				counter += _snwprintf(buf+counter, buffer_size-counter, L"\n.\nERROR!!! invalid word index in UWB description:\n");
				counter += _snwprintf(buf+counter, buffer_size-counter, L"\nm_Words.size() = %u\n", (unsigned int)m_Words.size());
				counter += _snwprintf(buf+counter, buffer_size-counter, L"itb_uwb->uiFirstWordNumber = %u\n", itb_uwb->uiFirstWordNumber);
				counter += _snwprintf(buf+counter, buffer_size-counter, L"itb_uwb->uiLastWordNumber = %u\n", itb_uwb->uiLastWordNumber);
				counter += _snwprintf(buf+counter, buffer_size-counter, L"i = %u\n", i);
				continue;
			}

			counter += _snwprintf(buf+counter, buffer_size-counter, L"%s\n", m_Words[i]->ToString());
		}
		counter += _snwprintf(
			buf+counter,
			buffer_size-counter, L"\n");
	}

	m_AsString.assign(buf);

	return m_AsString.c_str();
}


void	CSequence::DebugView()
{
	ToConsole(ToString());
}

}
}
}
}

