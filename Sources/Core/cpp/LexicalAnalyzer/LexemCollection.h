#pragma once

#include "..\ASCInterface\defines.h"
#include "..\ASCInterface\IInterface.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASSInterface\Iterators.h"
using namespace SS::Iterators;

#include "..\ASCInterface\TCoommonTypes.h"
using namespace SS::Core::Types::IndexationSearch;

#include "Unit.h"
#include "IndexLoader.h"

#include "LexemeBoard.h"
#include "IReparser.h"


namespace SS
{
namespace LexicalAnalyzer
{

/////////////////////////////////////////////////////////////////////////////////////
// CLexemCollection

class CLexemCollection
{
public:
	CLexemCollection();
	~CLexemCollection();

public:
	void Init(IText* pText, Reparser::IReparser* pReparser, CParagraph* pBackupPara, CIndexLoader* pIndexLoader);
	void Init(CLexemeBoard* pBoard, Reparser::IReparser* pReparser);

	bool IsEnd();
	unsigned int MoveFirst();
	unsigned int MoveNext();
	ATL::CAtlString GetString(int nCount, int nToLeft);
	ELexemType GetLexemType(int nToLeft = 0);
	LPCTSTR GetLexemString(int nToLeft);
	UINT GetLexemPosition(int nToLeft);
	UINT GetLexemSymbolPosition(int nToLeft);
	IUnit* GetUnit(int nToLeft);
	void SetHeaderLevelBegin(int iHeaderLevel);
	int GetHeaderLevelBegin();
	void SetHeaderLevelEnd(int iHeaderLevel);
	int GetHeaderLevelEnd();

	void GetData(int nToLeft, IUnit** ppUnit, LPCTSTR* pszLexem, ELexemType* plt, UINT* puiPosition, 
		UINT* puiSymbolPosition);

private:
	int m_iCurrUnit;
	CLexemeBoard* m_pBoard;

	Reparser::IReparser* m_pReparser;
	Reparser::CResultBoard m_oResultBoard;

	typedef vector<IUnit*> TUnitVector;
	TUnitVector m_vUnits;
};

}
}
