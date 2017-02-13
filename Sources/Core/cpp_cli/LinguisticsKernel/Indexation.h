#pragma once

#include "..\..\cpp\ASSInterface\ILexicaLib.h"
#include "..\..\cpp\ASSInterface\ISyntaxAnalyzer.h"
#include "..\..\cpp\ASSInterface\IDigestManager.h"
#include "..\..\cpp\ASSInterface\ISemanticAnalyze.h"
#include "..\..\cpp\ASCInterface\ILinguisticProcessor.h"
#include "..\..\cpp\ASSInterface\ITextBlockEx.h"
#include "..\..\cpp\ASSInterface\ITransliteration.h"
#include "..\..\cpp\ASSInterface\IDictionaryManager.h"
#include "..\..\cpp\ASSInterface\IAMConverter.h"
#include "..\..\cpp\ASSInterface\Constants.h"
#include "NERProcessor.h"
#include "StringService.h"

namespace LinguisticsKernel
{
    public enum SelectEntitiesMode
    {
        Full = 0,
        Simple = 1
    };
}

namespace SS
{
namespace LinguisticProcessor
{
	using namespace System;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::IO;
	using namespace System::Runtime::InteropServices;
	using namespace System::Text;
	using namespace System::Xml;
	using namespace System::Xml::Linq;
	
public ref class CIndexation 
{
    SS::Interface::Core::Dictionary::IMorphologyAnalyser*	    m_pMorphoAnalyzer;
    SS::Interface::Core::AMConverter::IAMConverterMorpho*       m_pAMConverter;
    SS::Interface::Core::ITransliteration*                      m_pTransliterationFrench;

	SS::Interface::Core::LexicalAnalyze::ILexica*				m_pLexica;
	SS::Interface::Core::ISyntaxAnalyzer*						m_pSyntaxAnalyser;
	SS::Interface::Core::IDigestAnalyzer*						m_pDigestAnalyser;
	SS::Interface::Core::ISemanticAnalyze*						m_pSemanticAnalyze;

	SS::Interface::Core::BlackBox::IBlackBoxTextStorage*			m_pBlackBoxTextStorage;
	SS::Interface::Core::BlackBox::IText*							m_pText;
	SS::Core::Types::IndexationSearch::TLinguisticProcessorMode*	m_pMode;

	SS::Interface::Core::MainAnalyse::IAnalyseParams*			m_pAnalyseParams;
	SS::Interface::Core::ResourceManagers::ILoadManager*		m_pLoadManager;

	SS::Interface::Core::CommonContainers::ISearchResult*		m_pSearchResult;

	LinguisticsKernel::CStringService^ m_pStringService;

	TextParsing::TextParser^ m_pTextParser;
	SimpleEntities::SimpleEntitySelector^ m_pSimpleEntitySelector;
	System::Text::RegularExpressions::Regex^ m_pSloganRegex;
	//---TEMPORARY UNUSED---// SS::LinguisticProcessor::NERProcessor^ m_pNERProcessor;	

public:
	CIndexation(String^ pUserResourcesPath);
	~CIndexation(void);

	virtual void SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams)
	{
		m_pAnalyseParams = pAnalyseParams;
	}

	virtual void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
	/// устанавливает указатель на лоад мэнеджер
	virtual SS::Interface::Core::ResourceManagers::ILoadManager* GetLoadManager()
	{
		return m_pLoadManager;
	};

	/// инициирует процесс индексации очередного текста
	void Init
	(
		/// контейнер с индексируемым текстом
		SS::Core::Types::IndexationSearch::TNativeTextForIndexation* pNativeText,
		/// параметры индексации текста
		SS::Core::Types::IndexationSearch::TIndexationParams* pIndexationParams,
		bool bIsGlobal		
	);

    /// загрузка служебного словаря
    void LoadDictionaries(vector<vector<wstring>>& dictionaries);
	/// установка максимальной длины сущности
	void SetEntityMaxLength(int length);

	/// генерирует очередную порцию индекса
	/**
	вернет:
	0 - если есть еще части для анализа
	1 - если больше нет
	2 - если была ошибка
	*/
	SS::Interface::Core::BlackBox::IText* GenerateNext(const wstring& text, bool bOutside);
	SS::Interface::Core::BlackBox::IObjectDescr* GetObjectDescr();

	void ReinitToneDicts(String^ typeName);
	void AddWordToDict(String^ typeName, int dict_code, String^ word);
	void GenerateDictLing(String^ typeName);

	String^ GenerateDigest(const wstring& text, String^ synonims, String^ korefs, List<int>^ offsets, String^ sObjType);
	String^ GenerateEntities(String^ srcText);
	String^ GenerateFacts(List<int>^ offsets, String^ srcText, String^ baseDate, bool isTonality);

    String^ TransliteringText(String^ srcText);
    wstring GetGenitiveFrom(const wstring& word);

	String^ GenerateHash(String^ entText, String^ entTypeName);

    String^ GetAllWordFormsXML(String^ word, SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech);
	List<String^>^ GetAllWordForms(String^ word, SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech);
	Tuple<List<String^>^, List<String^>^>^ GetAllWordFormsWithPartsOfSpeech(String^ word);
	List<String^>^ GetAllNormalWordForm(String^ word);
    String^ GetNormalWordForm(String^ word);
    String^ GetTextAllNormalForms(String^ text);

    wstring TOwstring(String^ s);
	string TOstring(String^ s);

    /// предобработка текста
    void TextPretreatment_Letters(wstring& text, LinguisticsKernel::SelectEntitiesMode mode);

	bool IsMorpho(const wstring& word, const SS::Core::Features::CMorphoFeature &oMorpho);

	Generic::List<String^>^ GetTokens(String^ srcText);
	String^ GetMorphoInfo(String^ word);

	String^ GenerateSentXMLDigest(String^ sText, ArrayList^ offsets, ArrayList^ themeIDs, String^ dictName);

	int  HashWordCount(String^ strHash);

System::IO::StreamWriter^ logFile;

	void SelectEntitiesOnContent(Linguistics::Core::Content^ content, String^ baseDate);
	void SelectSimpleEntitiesOnContent(Linguistics::Core::Content^ content, String^ baseDate);
	void SelectCeterusEntitiesOnContent(Linguistics::Core::Content^ content, String^ baseDate);
	void SelectSimpleEntitiesOnContent(Linguistics::Core::Content^ content, String^ baseDate, bool isSelectOnlyCeterus);

	TextParsing::TextParser^ get_TextParser()
	{
		return m_pTextParser;
	}
	void SetMode(SS::Core::Types::IndexationSearch::TLinguisticProcessorMode* pMode)
	{
		m_pMode->GetAnalyseDepthParams()->SetIsMorpho(pMode->GetAnalyseDepthParams()->IsMorpho());
		m_pMode->GetAnalyseDepthParams()->SetIsSyntax(pMode->GetAnalyseDepthParams()->IsSyntax());
		m_pMode->GetAnalyseDepthParams()->SetIsSemantic(pMode->GetAnalyseDepthParams()->IsSemantic());
		m_pMode->GetAnalyseDepthParams()->SetIsSynonims(pMode->GetAnalyseDepthParams()->IsSynonims());
	}

	Linguistics::Core::Entity^ CreateSimpleEntity(SimpleEntities::SimpleEntity^ simpleEntity);

private:
    void RussianLetterCorrect(wstring& text, const wstring& russianLetters, const wstring& englishLetters);
    int ExistLetter(wchar_t letter, const wstring& letters);
    void UpperTextProcessing(wstring& text);
    bool IsOpenQuote(wstring& str);
    int UpperTextProcessing_Test(SS::Interface::Core::BlackBox::IUnit* pUnit);
    void UpperTextProcessing_UnitSeria(wstring& text, 
                                       vector<vector<SS::Interface::Core::BlackBox::IUnit*>>& serias,
                                       vector<bool> &combinatories, int shift);
    void UpperTextProcessing_Unit(SS::Interface::Core::BlackBox::IUnit* pUnit, int& unitStatus, bool isCombinatory);
    void UpperTextProcessing_UnitReplace(wstring& text, SS::Interface::Core::BlackBox::IUnit* pUnit, int unitStatus);
    void UpperTextProcessing_Word(wstring& word, int& status);

	wstring GetNormalForm(SS::Dictionary::Types::TWordInfo& info);

	void AddLineToHashSet(Generic::HashSet<String^>^ hash, String^ line);

	Generic::List<String^>^ ConvertArray(const vector<wstring>& list);

	/// разрезка текста на предложения
	List<TextParsing::Paragraph^>^ ParseTextForSentences(String^ text);
	SS::Interface::Core::EntitiesCollection ConvertEntityArray(List<Linguistics::Core::Entity^>^ entityList);
	vector<vector<wstring>> ConvertParagraphsArray(Generic::IList<TextParsing::Paragraph^>^ paragraphList);
	List<String^>^ ConvertToArray(vector<wstring>& collection);
	List<Linguistics::Core::Entity^>^ ConvertToArray(Linguistics::Core::Content^ content, vector<SS::Core::Types::CTextEntity>& collection, int shift);
	void AddEntityListToContent(Linguistics::Core::Content^ content, SS::Interface::Core::EntitiesCollection& entityList, int shift);
	void AddEntityListToContent(Linguistics::Core::Content^ content, SS::Interface::Core::EntitiesCollection& entityList);
	void AddEntityListToContent(Linguistics::Core::Content^ content, List<Linguistics::Core::Entity^>^ entityList, int shift);
	void AddEntityListToContent(Linguistics::Core::Content^ content, List<Linguistics::Core::Entity^>^ entityList);
	Linguistics::Core::Entity^ GetEntity(Linguistics::Core::Content^ content, SS::Core::Types::CTextEntity& sourceEntity, int shift);

	List<Linguistics::Core::Entity^>^ GetSimpleEntities(Linguistics::Core::Content^ content);
	List<Linguistics::Core::Entity^>^ GetSimpleEntities(cli::array< String^ >^ texts);
	List<Linguistics::Core::Entity^>^ GetSimpleEntities(String^ text, int textStartPosition);
	List<Linguistics::Core::Entity^>^ GetSimpleEntities(cli::array< SimpleEntities::SimpleEntity^ >^ entities, int textStartPosition);

	wstring GetPretreatmentContentText(Linguistics::Core::Content^ content, vector<vector<wstring>> &paragraphs, Generic::IList< TextParsing::Paragraph^ >^ paragraphList);
	bool IsPossibleSlogan(Linguistics::Core::Content^ slogan);

	bool EndsWithAny(String^ str, cli::array< String^ >^ values);
	bool StartsWithAny(String^ str, cli::array< String^ >^ values);

	String^ GetFirstWord(String^ str);
	String^ GetLastWord(String^ str);

	/// обработка слоганов
	List<Linguistics::Core::Entity^>^ SloganAnalyzing(Linguistics::Core::Content^ content);

	wstring* _EnglishLetters;
	wstring* _RussianLetters;
    wstring* _AllRussianLetters;
    wstring* _AllEnglishLetters;
	wstring* _YELetters;
	wstring* _YEReplaceLetters;
};
}
}
