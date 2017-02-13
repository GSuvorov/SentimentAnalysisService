#include "stdafx.h"
#include "..\ASCInterface\ICommonContainers.h"
#include ".\Word.h"


namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxQuery
{
///	маска табличной необязательности
const unsigned char		mask_ObligatoryFlag = 0x01;
///	маска ответа
const unsigned char		mask_AnswerFlag = 0x02;
///	пустая строка
static const wchar_t*	wszEmptyString = L"";

CWord::CWord(void)
:m_Flags(0), m_wszWord(NULL), m_LinguisticWeight(0.0)
{
	//	создаём структурку, в зависимости от её дефолтных значений инициализируем слово
	SS::Interface::Core::CommonContainers::SQueryIndexAttributes	Attribs;
	SetObligatoryFlag(Attribs.m_Obligatory != 0);
	SetAnswerFlag(Attribs.m_bAnswerIndex != 0);
	SetAsString(wszEmptyString);
}

CWord::~CWord(void)
{
	if (m_wszWord != NULL)
	{
		delete[] m_wszWord;
		m_wszWord = NULL;
	}
}

unsigned int CWord::GetMorphoIndexCount()
{
	return static_cast<unsigned int>(m_Indexes.m_MorphoIndexes.size());
}

unsigned int CWord::GetMorphoIndex(unsigned int _uiIndex)
{
#ifdef _DEBUG
	if (_uiIndex >= m_Indexes.m_MorphoIndexes.size()) SS_THROW(L"\n\nCWord::GetIndex: invalid Morpho index\n\n");
#endif
	return m_Indexes.m_MorphoIndexes[_uiIndex];
}


unsigned int CWord::GetSyntaxIndexCount()
{
	return static_cast<unsigned int>(m_Indexes.m_SyntaxIndexes.size());
}

unsigned int CWord::GetSyntaxIndex(unsigned int _uiIndex)
{
#ifdef _DEBUG
	if (_uiIndex >= m_Indexes.m_SyntaxIndexes.size()) SS_THROW(L"\n\nCWord::GetIndex: invalid Syntax index\n\n");
#endif
	return m_Indexes.m_SyntaxIndexes[_uiIndex];
}



unsigned int CWord::GetSemanticIndexCount()
{
	return static_cast<unsigned int>(m_Indexes.m_SemanticIndexes.size());
}

unsigned int CWord::GetSemanticIndex(unsigned int _uiIndex)
{
#ifdef _DEBUG
	if (_uiIndex >= m_Indexes.m_SemanticIndexes.size()) SS_THROW(L"\n\nCWord::GetIndex: invalid Semantic index\n\n");
#endif
	return m_Indexes.m_SemanticIndexes[_uiIndex];
}


float CWord::GetLinguisticWeight()
{
	return m_LinguisticWeight;
}

BOOL CWord::GetObligatoryFlag()
{
	return ((m_Flags & mask_ObligatoryFlag) != 0);
}

void CWord::SetObligatoryFlag(bool const	_bValue)
{
	if (_bValue)
		m_Flags |= mask_ObligatoryFlag;
	else
		m_Flags &= ~mask_ObligatoryFlag;
}

BOOL CWord::GetAnswerFlag()
{
	return ((m_Flags & mask_AnswerFlag) != 0);
}

const wchar_t*	CWord::GetAsString()
{
	return m_wszWord;
}

void CWord::SetAnswerFlag(bool const _bValue)
{
	if (_bValue)
		m_Flags |= mask_AnswerFlag;
	else
		m_Flags &= ~mask_AnswerFlag;
}

void	CWord::SetAsString(const wchar_t*	_wszWord)
{
	if (m_wszWord != NULL) delete[] m_wszWord;
	if (_wszWord == NULL)
	{
		m_wszWord = new wchar_t[wcslen(wszEmptyString)+1];
		wcscpy(m_wszWord, wszEmptyString);
	}
	else
	{
		m_wszWord = new wchar_t[wcslen(_wszWord)+1];
		wcscpy(m_wszWord, _wszWord);
	}
}

void	CWord::AddMorphoIndex(unsigned int _Index)
{
	m_Indexes.m_MorphoIndexes.push_back(_Index);
}

void	CWord::AddSyntaxIndex(unsigned int _Index)
{
	m_Indexes.m_SyntaxIndexes.push_back(_Index);
}

void	CWord::AddSemanticIndex(unsigned int _Index)
{
	m_Indexes.m_SemanticIndexes.push_back(_Index);
}

void	CWord::SetLinguisticWeight(float const _weight)
{
	m_LinguisticWeight = _weight;
}

void	CWord::Del(IWord*	_Word)
{
	if (_Word != NULL) delete _Word;
}

void	CWord::Fill(SWordIndexes const& _rhs)
{
	m_Indexes.assign(_rhs);
}

const wchar_t* CWord::ToString()
{
	m_AsString.assign(L"<NULL>");

	if (m_Indexes.Empty()) return m_AsString.c_str();

	const wchar_t*	wcszEmptyString = L"-no word-";
	const unsigned int buffer_size = 1024;
	wchar_t	buf[buffer_size];
	for (unsigned int i = 0; i < 16; ++i) buf[i] = L'.';
	unsigned int counter = 0;
	counter += _snwprintf(
		buf+counter,
		buffer_size-counter,
		L"<%16s |LingWeight = %#10.2f | FLAGS: fOBL =%3u , fANS =%3u>",
		(m_wszWord != NULL) ? (m_wszWord) : (wcszEmptyString),
		(double)m_LinguisticWeight,
		(unsigned int)GetObligatoryFlag(),
		(unsigned int)GetAnswerFlag());

	counter += _snwprintf(
		buf+counter, buffer_size-counter, L"\nmorpho indexes  :");
	for(v_uint::iterator
		itb_ind = m_Indexes.m_MorphoIndexes.begin(),
		ite_ind = m_Indexes.m_MorphoIndexes.end();
		itb_ind != ite_ind;
		++itb_ind)
	{
		counter += _snwprintf(
			buf+counter, buffer_size-counter,
			L"%10u ",
			(unsigned int)(*itb_ind));
	}

	counter += _snwprintf(
		buf+counter, buffer_size-counter, L"\nsyntax indexes  :");
	for(v_uint::iterator
		itb_ind = m_Indexes.m_SyntaxIndexes.begin(),
		ite_ind = m_Indexes.m_SyntaxIndexes.end();
		itb_ind != ite_ind;
	++itb_ind)
	{
		counter += _snwprintf(
			buf+counter, buffer_size-counter,
			L"%10u ",
			(unsigned int)(*itb_ind));
	}

	counter += _snwprintf(
		buf+counter, buffer_size-counter, L"\nsemantic indexes:");
	for(v_uint::iterator
		itb_ind = m_Indexes.m_SemanticIndexes.begin(),
		ite_ind = m_Indexes.m_SemanticIndexes.end();
		itb_ind != ite_ind;
		++itb_ind)
	{
		counter += _snwprintf(
			buf+counter, buffer_size-counter,
			L"%10u ",
			(unsigned int)(*itb_ind));
	}

	//counter += _snwprintf(
	//	buf+counter,
	//	buffer_size-counter,
	//	L"\n");

	m_AsString.assign(buf);

	return m_AsString.c_str();
}


void	CWord::DebugView()
{
	ToConsole(ToString());
}


}
}
}
}