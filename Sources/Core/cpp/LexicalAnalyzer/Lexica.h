#pragma once

#include ".\LexicaPrimary.h"
using namespace SS::Interface::Core::MainAnalyse;
using namespace SS::Interface::Core::BlackBox;

#include "..\ASCInterface\ITextFeature.h"
using namespace SS::Interface::Core;
using namespace SS::Interface::Core::CommonContainers;

#include "DocStructureLoader.h"
#include "Unit.h"
#include "IndexLoader.h"
#include "InternalRepresentation.h"

#include "IRLoader.h"
#include "Walker.h"

#include "../ASCInterface/ILinguisticProcessor.h"
#include "../ASSInterface/ILexicaLib.h"


namespace SS
{
namespace LexicalAnalyzer
{

class CLexica : 
		virtual public CLexicaPrimary,
		virtual public SS::Interface::Core::LexicalAnalyze::ILexica
{
public:
	CLexica();
	virtual ~CLexica();

// IBase
public:
	virtual HRESULT QueryInterface(REFIID refIID, void** pBase);
	virtual ULONG Release();

// ILexicaPrimary
public:
	virtual void SetText(TNativeTextForIndexation* pNativeText, unsigned int uiStartPosition = 0);
	virtual unsigned int LoadNext(bool bIgnoreParagraphs = false);
	virtual SS::Interface::Core::ILexemeBoard* GetLexemeBoard();

// ILexica
public:
	virtual void SetLoadManager(ILoadManager* pLoadManager);
	/// установка параметров анализа
	virtual void SetAnalyseParams(IAnalyseParams* pAnalyseParams);
	/// анализ текста, содержащегося в подключенном хранилище лексем и запись результата в БлэкБокс
	virtual void AnalyzeDocStructure(IText* pText);
	/// анализ текста, содержащегося в подключенном хранилище лексем и запись результата в БлэкБокс
	virtual void AnalyzeNext(IText* pText, vector<vector<wstring>> *pParagraphs, bool isSimple = false, int mode = 1);
	virtual void AnalyzeNextSentence(SS::Interface::Core::BlackBox::IText* pText, bool isSimple = false, int mode = 1);
	/// заполняет синонимы
	virtual void FillSynonyms(IText* pText);
	/// установка семантического типа для юнита
	virtual void SetSemanticType(IUnit* pUnit, SS::Core::Features::Types::TSemanticType semanticType);
	/// Проверка, что юнит имеет заданный семантический тип
	virtual bool IsSemanticType(IUnit* pUnit, SS::Core::Features::Types::TSemanticType semanticType);

	/// получение первого предложения абзаца. Если абзац пустой - переходим к следующему.
	wstring* GetFirstSentenceOfParagraph(vector<vector<wstring>> *pParagraphs, vector<vector<wstring>>::iterator &pIt, vector<wstring>::iterator &sIt);
    /// анализ необходимости объединения предложений
    bool AnalyzeSentencesJoin(ISentence* pPrevSent, ISentence* pCurSent);

    /// объединение предложений
    void JoinSentences(IUnit* pLeft, IUnit* pForAdd);
	
// CLexicaPrimary
private:
	bool AddSentence(CIRLoader::EBreakReason br, int maxUnitCount, bool isParagraphEnd, bool isSimple, int mode);
	/// if we are in query mode and the sentence is quoted, set "strict query" param and return true
	bool IsStrictQuery();
	/// checks if current sentence contains interrogative word
	bool IsOnlyKeywordsSentence();
	/// fills pText in "strict query" mode
	void AnalyzeStrictQuery(IText* pText);
	/// sets IsQuestion to query words
	void SetQuestionWords(IText* pText);
	void AnalizeNewQuestionStructures(IText* pText);

	void MarkRegions(IR::CSentence *pSent);
	void MarkUnknownPersons(IR::CSentence *pSent);
	void EngInRusEvristicJoin(IR::CSentence *pSent);
	void EngDeleteSemanticPersonWithoutProper(IR::CSentence *pSent);
	void EngEvristicJoinByAndProper(IR::CSentence *pSent);
	void SpecialPersonEvristicJoin(IR::CSentence *pSent);

	void EvristicFindPLaces(IR::CSentence *pSent);

private:
    /// создание объединенного юнита
    void CreateUnionUnit(IR::CSentence *pSent, EAnalyseLanguage eal, 
        bool &isEngFind, int &nProcessed, int &nInserted);

	IBlackBoxTextStorage* m_pStorage;

	CDocStructureLoader m_oDocStructureLoader;
	CIndexLoader m_oIndexLoader;
	CUnitChecker m_oUnitChecker;

	CIRLoader m_oIRLoader;
	Walker::CWalker m_oWalker;

	/// current sentence buffer
	IR::CSentence m_oSent;
	/// helper for filling IText
	CParagraph m_oPara;

	bool m_bSearch;
	bool m_bQuery;
	bool m_bLite;
	bool m_bUseQueryDeformer;

	CIndexLoader::TIndexSet m_sQueryWordsIDSources;
};

}
}
