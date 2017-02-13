#pragma once

#include "..\ASSInterface\IBlackBoxItems.h"
#include "..\ASSInterface\IBlackBox.h"
using namespace SS::Interface::Core::BlackBox;

#include "RangeList.h"
#include "LexemeBoard.h"
#include "IndexLoader.h"
#include "DocStructureReader.h"


namespace SS
{
namespace LexicalAnalyzer
{

class CDocStructureLoader
{
public:
	CDocStructureLoader();

public:
	void Init(IBlackBoxTextStorage* pStorage, CIndexLoader* pIndexLoader);
	void ParseDocStructure(BSTR bstrDocStructure);
	void AnalyzeDocStructure(CLexemeBoard* pBoard, IText* pText);

private:
	void NextWord();

private:
	CRangeList m_oRangeList;
	IText* m_pText;
	CLexemeBoard* m_pBoard;
	IBlackBoxTextStorage* m_pStorage;
	CIndexLoader* m_pIndexLoader;
};

}
}
