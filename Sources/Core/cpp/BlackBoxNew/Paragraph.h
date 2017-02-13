#pragma once

#include "RootItem.h"

#include "..\ASSInterface\IBlackBoxItems.h"
using namespace SS::Interface::Core::BlackBox;

#include "..\ASCInterface\TCoommonTypes.h"
using namespace SS::Core::Types::IndexationSearch;

#include <string>
using namespace std;


namespace SS
{
namespace Core
{
namespace BlackBox
{

class CParagraph : 
		public CMiddleItem,
		public IParagraph
{
public:
	CParagraph();
	virtual ~CParagraph();

public:
	virtual void AddSentence(ISentence* pSentence);
	virtual IParagraph* GetNextParagraph();
	virtual IParagraph* GetPrevParagraph();
	virtual ISentence* GetFirstSentence();
	virtual ISentence* GetLastSentence();
	virtual void ClearParagraph();
	virtual IParagraph* ReleaseParagraph();
	virtual void SetIndexTypeName(const wchar_t* wszName);
	virtual const wchar_t* GetIndexTypeName();
	virtual void SetRelevance(int iRelevance);
	virtual int GetRelevance();
	virtual ETextCutType GetType();
	virtual void SetType(ETextCutType eTextCutType);
	virtual unsigned int GetLevel();
	virtual void SetLevel(unsigned int uiLevel);
	virtual unsigned int GetNumber();
	virtual void SetNumber(unsigned int uiNumber);

private:
	unsigned int m_uiLevel;
	unsigned int m_uiNumber;
	unsigned int m_iRelevance;
	ETextCutType m_eTextCutType;
	wstring m_strIndexTypeName;
};

}
}
}
