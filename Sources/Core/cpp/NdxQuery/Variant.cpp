#include "stdafx.h"
#include ".\Variant.h"
#include ".\Sequence.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxQuery
{


CVariant::CVariant(void)
{
}

CVariant::~CVariant(void)
{
	Clear();
}

unsigned int CVariant::GetSequenceCount()
{
	return static_cast<unsigned int>(m_Sequences.size());
}

SS::Interface::Core::NdxSE::ISequence* CVariant::GetSequence(unsigned int uiIndex)
{
	if (uiIndex >= m_Sequences.size()) return NULL;
	return m_Sequences[uiIndex];
}

void	CVariant::AddSequence(SS::Interface::Core::NdxSE::ISequence*	Seq)
{
	if (Seq == NULL)
	{
		ToConsole(L"ERROR!!!");
		return;
	}
	m_Sequences.push_back(Seq);
}

void CVariant::Del(IVariant* Var)
{
	if (Var != NULL) delete Var;
}

void	CVariant::Clear()
{
	std::for_each(m_Sequences.begin(), m_Sequences.end(), CSequence::Del);
	m_Sequences.clear();
}

const wchar_t* CVariant::ToString()
{
	m_AsString.assign(L"\n.\nERROR!!! CVariant is empty\n.");

	if (m_Sequences.empty()) return m_AsString.c_str();

	const unsigned int buffer_size = 1024*10;
	wchar_t	buf[buffer_size];
	unsigned int counter = 0;
	counter += _snwprintf(buf+counter, buffer_size-counter, L"\n.\n[VARIANT]:");

	for(v_Sequences::iterator
		itb_seq = m_Sequences.begin(),
		ite_seq = m_Sequences.end();
		itb_seq != ite_seq;
		++itb_seq)
	{
		if (*itb_seq == NULL)
		{
			counter += _snwprintf(buf+counter, buffer_size-counter, L"\n{invalid sequence}\n.");
			continue;
		}

		counter += _snwprintf(buf+counter, buffer_size-counter, L"%s", (*itb_seq)->ToString());
	}

	m_AsString.assign(buf);

	return m_AsString.c_str();
}

void	CVariant::DebugView()
{
	ToConsole(ToString());
}


}
}
}
}

