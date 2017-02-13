#pragma once

#include <atlstr.h>
namespace SS
{
namespace LexicalAnalyzer
{
namespace Reparser
{

enum EReparseMode
{
	rmRestore,
	rmNumberWord,
};

struct SLexem
{
	SLexem(LPCTSTR sz, ELexemType lt, int l)
	{
		szText = sz;
		LexemType = lt;
		iLen = l;
	}

	ATL::CAtlString szText;
	ELexemType LexemType;
	int iLen;
};

class CResultBoard
{
public:
	~CResultBoard()
	{
		Clear();
	}

	void AddLexem(LPCTSTR szText, ELexemType LexemType, int iLen = -1)
	{
		if(iLen == -1)
			iLen = (int) _tcslen(szText);
		SLexem* pLexem = new SLexem(szText, LexemType, iLen);
		m_dqLexems.push_back(pLexem);
	}

	void RemoveFirst(int iCount)
	{
		while(iCount--)
		{
			delete m_dqLexems.front();
			m_dqLexems.pop_front();
		}

		m_it = m_dqLexems.begin();
	}

	SLexem* GetFirstLexem()
	{
		m_it = m_dqLexems.begin();
		return (m_it == m_dqLexems.end()) ? NULL : *m_it;
	}

	SLexem* GetNextLexem()
	{
		m_it++;
		return (m_it == m_dqLexems.end()) ? NULL : *m_it;
	}

	bool IsEmpty()
	{
		return m_dqLexems.empty();
	}

	void GetString(LPTSTR sz, int iCount)
	{
		*sz = 0;
		for(int i = 0; i < iCount; i++)
			_tcscat(sz, m_dqLexems[i]->szText);
	}

	int GetSize()
	{
		return (int) m_dqLexems.size();
	}

	void Clear()
	{
		for(TLexemCollection::iterator it = m_dqLexems.begin(); it != m_dqLexems.end(); it++)
			delete *it;
		m_dqLexems.clear();
		m_it = m_dqLexems.end();
	}

private:
	typedef deque<SLexem*> TLexemCollection;
	TLexemCollection m_dqLexems;
	TLexemCollection::iterator m_it;
};

struct IReparser
{
	virtual void Reparse(LPCTSTR szText, EReparseMode eMode, CResultBoard* pBoard) = 0;
};

}
}
}
