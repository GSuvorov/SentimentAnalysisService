#pragma once

#include "..\ASSInterface\ILexicaLib.h"
#include "..\ASCInterface\ILinguisticProcessor.h"
#include <atlstr.h>
using namespace SS::Interface::Core;


#ifdef UNICODE
#	define GetLexemeT GetLexeme
#else
#	define GetLexemeT GetLexemeA
#endif


namespace SS
{
namespace LexicalAnalyzer
{

enum ELexemType
{
	accUndefined,

	accWord = 0x11,

	accSentBreak,
	accParaBreak,

	accNumber,

	accAbbrev,
	accAcronymCitation,
	accLQuote,
	accRQuote,
	accInitial,

	accComma,
	accApostroph,
	accAmpersand,
	accSmallWord,

	accDefis,
	accTire,

	accSign,
	accPoint,
	accSlash,

	accBreak,

	accLongNumber,
	accFirstComplexNumberPart,
	accComplexNumberPart,

	accColon,
	accEngWord,
	accURLPart,
	accAt,

	accUnclosedSentence,
	accTitleBreak,

	accOrdinalNumber,
	accNumberWord,
	accBrokenWord,
	accRomanNumber,

	accLeftBracket,
	accRightBracket,

	accURL
};

class CLexemeBoard : public SS::Interface::Core::ILexemeBoard
{
public:
	CLexemeBoard(unsigned int uiCapacity = MAX_WORDS_IN_BLOCK);
	~CLexemeBoard();

public:
	/// вернет емкость хранилища (измеряется в максимальном количестве единовременно загружаемых лексем текста)
	virtual unsigned int GetCapacity();
	/// вернет количество загруженных лексем текста
	virtual unsigned int GetCount();
	/// переход на первую лексему, вернет 0, если лексем нет, вернет 1 если есть
	virtual unsigned int MoveFirst();
	/// переход на следующую лексему, вернет 0, если находится на последней лексеме, , вернет 1 если не на последней
	virtual unsigned int MoveNext();
	///	получение лексемы
	virtual LPCWSTR GetLexeme();
	virtual LPCSTR GetLexemeA();
	/// получение абсолютного номера текущей лексемы в тексте (нумерация начинается с 0)
	virtual unsigned int GetAbsoluteIndex();
	/// получение абсолютного номера первого символа текущей лексемы
	virtual unsigned int GetAbsoluteSymbolIndex();

public:
	ELexemType GetLexemType();
	bool IsEnd();

	bool MoveLast();
	bool MovePrev();

	void Reset();
	bool AddLexeme(LPCTSTR szText, ELexemType LexemType, int iAbsPosition, int iAbsCharPosition);

	ELexemType GetFirstLexem();
	ELexemType GetLastLexem();

	unsigned int GetFirstLexemPosition();
	unsigned int GetLastLexemPosition();

	ELexemType GetLexemType(int nToLeft);
	LPCTSTR GetLexemString(int nToLeft);
	UINT GetLexemPosition(int nToLeft);
	UINT GetLexemSymbolPosition(int nToLeft);

	void SetLexemeType(ELexemType eType);

	void SetHeaderLevelBegin(int iHeaderLevel);
	int GetHeaderLevelBegin();

	void SetHeaderLevelEnd(int iHeaderLevel);
	int GetHeaderLevelEnd();

private:
	struct SLexem
	{
		SLexem() : iAbsPosition(0), iAbsCharPosition(0), LexemType(ELexemType::accUndefined), iHeaderLevelBegin(0), iHeaderLevelEnd(0)
		{
		}

		ATL::CAtlString szText;
		int iAbsPosition;
		int iAbsCharPosition;
		ELexemType LexemType;
		int iHeaderLevelBegin;
		int iHeaderLevelEnd;
	};

	typedef vector<SLexem*> TLexemVect;

private:
	TLexemVect m_vLexems;
	int m_iCurrLexem;
	unsigned int m_uiTop;
};

}
}
