#include "StdAfx.h"
#include "../../cpp/[libs]/_Paths.h"
#using <mscorlib.dll>
//#include <vcclr.h>
//#include <_vcclrit.h>

#include ".\indexation.h"

#include <iostream>
#include <locale>

#include <crtdbg.h>
#include <assert.h>
#include <atlstr.h>

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace System::Text;
using namespace System::Xml;
using namespace System::Xml::Linq;

#using <System.Xml.dll>

/// тип словар€ тональности по умолчанию
const wstring DEFAULT_TONE_TYPE = L"ѕрочее";

SS::LinguisticProcessor::CIndexation::CIndexation(String^ pUserResourcesPath) : 
		m_pLexica(NULL), 
		m_pSemanticAnalyze(NULL),
        m_pTransliterationFrench(NULL),
        m_pMorphoAnalyzer(NULL),
		m_pBlackBoxTextStorage(NULL), 
		m_pText(NULL),
		m_pSearchResult(NULL),
		m_pAnalyseParams(NULL),
		m_pSyntaxAnalyser(NULL),
		m_pAMConverter(NULL),
		m_pDigestAnalyser(NULL),
		m_pLoadManager(NULL),
		logFile(nullptr)
{
	m_pMode = new SS::Core::Types::IndexationSearch::TLinguisticProcessorMode();
	m_pStringService = gcnew LinguisticsKernel::CStringService();

	m_pTextParser = gcnew TextParsing::TextParser(Path::Combine(pUserResourcesPath, "TextParser"));
	m_pTextParser->IsMultiThreaded = false;
	m_pSimpleEntitySelector = gcnew SimpleEntities::SimpleEntitySelector(Path::Combine(pUserResourcesPath, "EntityExpressions"));
	//---TEMPORARY UNUSED---// m_pNERProcessor = gcnew SS::LinguisticProcessor::NERProcessor(Path::Combine(pUserResourcesPath, "NER"));
	m_pSloganRegex = gcnew RegularExpressions::Regex("(\"[ј-я].*?\")|(Ђ[ј-я].*?ї)|(У[ј-я].*?Ф)|(<[ј-я].*?>)");

	_EnglishLetters = new wstring( L"AaBCcEeHKkMnOoPpTXxYy" );
    _RussianLetters = new wstring( L"ја¬—с≈еЌ кћпќо–р“’х”у" );
    _AllRussianLetters = new wstring( L"јаЅб¬в√гƒд≈е®Є∆ж«з»и…й кЋлћмЌнќоѕп–р—с“т”у‘ф’х÷ц„чЎшўщЏъџы№ьЁэёюя€" );
    _AllEnglishLetters = new wstring( L"AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz" );
	_YELetters = new wstring( L"®Є" );
	_YEReplaceLetters = new wstring( L"≈е" );
}

SS::LinguisticProcessor::CIndexation::~CIndexation()
{
	if (m_pText)
	{
		m_pText->ReleaseText();
	}
}

SS::Interface::Core::BlackBox::IObjectDescr* SS::LinguisticProcessor::CIndexation::GetObjectDescr()
{
	return (this->m_pBlackBoxTextStorage->CreateObjectDescr());
}

void SS::LinguisticProcessor::CIndexation::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
    using namespace SS::Interface::Core::Dictionary;

	SS_TRY
	{
		m_pLoadManager = pLoadManager;

		if (!m_pLexica)
		{			
			//logFile->WriteLine("LexicalAnalyzer.dll");
			//logFile->Flush();
			m_pLexica= 
				(SS::Interface::Core::LexicalAnalyze::ILexica*)
				GetLoadManager()->GetInterface(L"./core/linguistics/LexicalAnalyzer.dll", CLSID_Lexica, IID_Lexica);
		}
		if (!m_pSemanticAnalyze)
		{
			//logFile->WriteLine("SemanticAnalyzer.dll");
			//logFile->Flush();
			m_pSemanticAnalyze = 
				(SS::Interface::Core::ISemanticAnalyze*)
				GetLoadManager()->GetInterface(L"./core/linguistics/SemanticAnalyzer.dll", CLSID_SemanticAnalyzer, IID_SemanticAnalyzer);
		}

  		if (!m_pSyntaxAnalyser)
		{
			//logFile->WriteLine("SyntaxAnalyzer.dll");
			//logFile->Flush();
			m_pSyntaxAnalyser = 
				(SS::Interface::Core::ISyntaxAnalyzer*)
				GetLoadManager()->GetInterface(L"./core/linguistics/SyntaxAnalyzer.dll", CLSID_SyntaxAnalyzer, IID_SyntaxAnalyzer);
			//logFile->WriteLine("DigestAnalyzer.dll");
			//logFile->Flush();
			m_pDigestAnalyser = 
				(SS::Interface::Core::IDigestAnalyzer*)
				GetLoadManager()->GetInterface(L"./core/linguistics/DigestAnalyzer.dll", CLSID_DigestAnalyzer, IID_DigestAnalyzer);

			//logFile->WriteLine(DICTIONARIES_DLL_PATH);
			//logFile->Flush();
            IDictionary* pDict = (IDictionary*) 
		        pLoadManager->GetInterface( DICTIONARIES_DLL_PATH, CLSID_IDictionary, IID_IDictionary);
    	    m_pMorphoAnalyzer = pDict->GetMorphologyAnalyser();
   
			//logFile->WriteLine("AMConverter.dll");
			//logFile->Flush();
            m_pAMConverter = (SS::Interface::Core::AMConverter::IAMConverterMorpho*)
    	        pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);

			//logFile->WriteLine("Transliteration.dll");
			//logFile->Flush();
            m_pTransliterationFrench = 
                (SS::Interface::Core::ITransliteration*)
                GetLoadManager()->GetInterface(L"./core/linguistics/Transliteration.dll", CLSID_TransliterationFrench, IID_TransliterationFrench);
		}
		

		//logFile->WriteLine("MainAnalyse.dll");
		//logFile->Flush();
		SetAnalyseParams( 
			(SS::Interface::Core::MainAnalyse::IAnalyseParams*)
            GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams)
			);

		//logFile->WriteLine("CommonContainers.dll");
		//logFile->Flush();
		if (!m_pSearchResult)
		{
			m_pSearchResult = 
				(SS::Interface::Core::CommonContainers::ISearchResult*)
				GetLoadManager()->GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_SearchResult, IID_SearchResult);
		}

		if (!m_pBlackBoxTextStorage)
		{
			//logFile->WriteLine("BlackBox.dll");
			//logFile->Flush();
			m_pBlackBoxTextStorage = 
				(SS::Interface::Core::BlackBox::IBlackBoxTextStorage*)
				GetLoadManager()->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxTextStorage, IID_BlackBoxTextStorage);
			m_pText = m_pBlackBoxTextStorage->CreateText();
		}
	}
	//logFile->WriteLine("end1");
	//logFile->Flush();
	SS_CATCH(L"")
};

void SS::LinguisticProcessor::CIndexation::LoadDictionaries(vector<vector<wstring>> &dictionaries)
{
    m_pDigestAnalyser->LoadServiceDictionaries(dictionaries);
}

void SS::LinguisticProcessor::CIndexation::SetEntityMaxLength(int length)
{
	m_pDigestAnalyser->SetMaxEntityLength(length);
}

void SS::LinguisticProcessor::CIndexation::Init(
	  SS::Core::Types::IndexationSearch::TNativeTextForIndexation* pNativeText,
	  SS::Core::Types::IndexationSearch::TIndexationParams* pIndexationParams,
	  bool bIsGlobal)
{
	SS_TRY
	{
		SL_OUT_DEBUG_STR (__WFUNCTION__);

		m_pAnalyseParams->SetAnalyseMod(SS::Core::Types::eamIndexation);
		if (bIsGlobal)
		{
			m_pAnalyseParams->SetCurrentSystemMode(SS::Core::Types::ecsmGlobal);
		}
		else
		{
			m_pAnalyseParams->SetCurrentSystemMode(SS::Core::Types::ecsmLocal);
		}
		m_pAnalyseParams->GetAnalyseDepthParams()->SetIsMorpho	(m_pMode->GetAnalyseDepthParams()->IsMorpho());
		m_pAnalyseParams->GetAnalyseDepthParams()->SetIsSyntax	(m_pMode->GetAnalyseDepthParams()->IsSyntax());
		m_pAnalyseParams->GetAnalyseDepthParams()->SetIsSemantic(m_pMode->GetAnalyseDepthParams()->IsSemantic());
		m_pAnalyseParams->GetAnalyseDepthParams()->SetIsSynonims(m_pMode->GetAnalyseDepthParams()->IsSynonims());
		
		/*	лексический анализ	*/
		m_pLexica->SetAnalyseParams(m_pAnalyseParams);
		m_pLexica->SetText(pNativeText);
		/*	семантический анализ	*/
		m_pSemanticAnalyze->SetAnalyseParams(m_pAnalyseParams);
		m_pSyntaxAnalyser->SetAnalyseParams(m_pAnalyseParams);

		SL_OUT_DEBUG_STR (__WFUNCTION__);
	}
	SS_CATCH(L"")
}

SS::Interface::Core::BlackBox::IText* SS::LinguisticProcessor::CIndexation::GenerateNext(const wstring& text, bool bOutside)
{
	SS_TRY;

	// indexation
	m_pText->ClearText();

	if (!m_pLexica->LoadNext())
	{
		return (NULL);
	}
 	m_pLexica->AnalyzeNext(m_pText, &ConvertParagraphsArray(ParseTextForSentences(gcnew String(text.c_str()))));

	if (!bOutside)
	{
		m_pSyntaxAnalyser->AnalyzeNext(m_pText);
		return (m_pText);
	}

	SS_CATCH(L"");
	return (NULL);
}
/*const Char^ PtrToStringChars1(const String^ s) {

	String^ x = s;
	if ( x->Length != 0 ) {
		return (x[0]);
	}
	return (L'\0');
	
	const Byte *bp = reinterpret_cast<const Byte *>(s);
	if( bp != 0 ) {
		unsigned offset = Runtime::CompilerServices::RuntimeHelpers::OffsetToStringData;
    	bp += offset;
	}
    return reinterpret_cast<const Char*>(bp);
	
}*/

wstring SS::LinguisticProcessor::CIndexation::TOwstring(String^ s)
{
   s = SemanticResolution::ServiceFunction::ReplaceWrongSymbols(s);
   const wchar_t* chars = (const wchar_t*)(Marshal::StringToHGlobalUni(s)).ToPointer();
   wstring str = chars;
   Marshal::FreeHGlobal(IntPtr((void*)chars));
   return (str);
}
string SS::LinguisticProcessor::CIndexation::TOstring (String^ s)
{
   s = SemanticResolution::ServiceFunction::ReplaceWrongSymbols(s);
   const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
   string str = chars;
   Marshal::FreeHGlobal(IntPtr((void*)chars));   
   return (str);
}

void SS::LinguisticProcessor::CIndexation::ReinitToneDicts(String^ typeName)
{
	m_pDigestAnalyser->ReinitToneDicts(TOwstring(typeName));
}

 void SS::LinguisticProcessor::CIndexation::AddWordToDict(String^ typeName, int dict_code, String^ word)
 {
	m_pDigestAnalyser->AddWordToDict(TOwstring(typeName), dict_code, TOstring(word));
 }

 void SS::LinguisticProcessor::CIndexation::GenerateDictLing(String^ typeName)
 {
	 m_pDigestAnalyser->GenerateDictLing(TOwstring(typeName));
 }

String^ SS::LinguisticProcessor::CIndexation::GenerateDigest(
	const wstring& text, String^ synonims, String^ korefs, Generic::List<int>^ offsets, String^ sObjType)
{
	m_pText->ClearText();

	if (!m_pLexica->LoadNext())
	{
		return nullptr;
	}
 	m_pLexica->AnalyzeNext(m_pText, &ConvertParagraphsArray( ParseTextForSentences(gcnew String(text.c_str())) ));
	
	list<int> offs;
	for (int i = 0; i < offsets->Count; ++i)
	{
		offs.push_back(offsets[ i ]);
	}

	m_pDigestAnalyser->AnalyzeNextBeforeSynt(m_pText, offs);
	m_pSyntaxAnalyser->AnalyzeNext(m_pText);

	SS::Interface::Core::BlackBox::IObjectDescr* objDescr = this->m_pBlackBoxTextStorage->CreateObjectDescr();
	
	array< Char >^ chs = gcnew array< Char >(1); chs[0] = L'\n';
	array< String^ >^ syn_list = synonims->Split(chs);	
	for (int i = 0; i < syn_list->Length; i++)
	{
		if (syn_list[i]->Length > 1)
		{
			objDescr->AddSynonim(TOwstring(syn_list[i]));
		}
	}

	array< String^ >^ kor_list = korefs->Split(chs);
	for (int i = 0; i < kor_list->Length; i++)
	{
		if (kor_list[i]->Length > 1)
		{
			kor_list[i] = kor_list[i]->Trim();
			objDescr->AddKorefs(TOwstring(kor_list[i]));
		}
	}

	wstring objType = TOwstring(sObjType);
    if (objType.empty())
	{
        objType = DEFAULT_TONE_TYPE;
	}

	objDescr->SetType((wchar_t*)objType.c_str());

	m_pDigestAnalyser->AnalyzeNextAfterSynt(m_pText, objDescr, offs);

	wstring sd = m_pDigestAnalyser->GetDigestXML();

	String^ sDigest = gcnew String(sd.c_str());
	return (sDigest);
}

String^ SS::LinguisticProcessor::CIndexation::GenerateEntities(String^ srcText)
{	
	SS::Core::Types::IndexationSearch::TIndexationParams oIndexationParams;
	SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
	Generic::List<int>^ offsets = gcnew Generic::List<int>();

	wstring text = TOwstring(srcText);
    TextPretreatment_Letters(text, LinguisticsKernel::SelectEntitiesMode::Simple);
    oNativeText.SetText(text.c_str());        
    Init( &oNativeText, &oIndexationParams, false);
	m_pText->ClearText();
	if (!m_pLexica->LoadNext())
	{
		return nullptr;
	}
 	m_pLexica->AnalyzeNext(m_pText, &ConvertParagraphsArray(ParseTextForSentences(gcnew String(text.c_str()))), true, 0);
    wstring str = m_pDigestAnalyser->GetEntitiesXML((wchar_t*)text.c_str(), m_pText);

	srcText = gcnew String(str.c_str());
	srcText = SemanticResolution::ServiceFunction::CorrectXML(srcText, String::Empty);
	return (srcText);
}

int SS::LinguisticProcessor::CIndexation::HashWordCount(String^ strHash)
{
	StringBuilder^ sb = gcnew StringBuilder(strHash->Length);
	int count = 0;
	for (int i = 0; i < strHash->Length; i++)
	{
		if (Char::IsLetter(strHash[i]))
		{
			sb->Append(strHash);
		}
		else
		{
			if (sb->Length > 1)
			{
				count++;
			}
			sb->Remove(0, sb->Length);
		}
	}

	if (sb->Length > 0)
	{
		count++;
	}

	return count;
}

String^ SS::LinguisticProcessor::CIndexation::GenerateHash(String^ entText, String^ entTypeName)
{
	SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
	SS::Core::Types::IndexationSearch::TIndexationParams oIndexationParams;

	String^ strNew = String::Concat(gcnew String(L"и "), entText);
    wstring text = TOwstring(strNew);
    oNativeText.SetText(text.c_str());
		
	Init( &oNativeText, &oIndexationParams, false);

	m_pText->ClearText();
	if (!m_pLexica->LoadNext())
	{
		return nullptr;
	}
 	m_pLexica->AnalyzeNext(m_pText, &ConvertParagraphsArray(ParseTextForSentences(gcnew String(text.c_str()))));
		
	wstring str = m_pDigestAnalyser->GetEntityHash(text, m_pText);
 
	String^ delimStr = gcnew String(";");
    array< Char >^ delimiter = delimStr->ToCharArray();

	String^ sResult = gcnew String(str.c_str());
	array< String^ >^ arStr = sResult->Split(delimiter);

	int count1 = HashWordCount(entText);
	int count2 = 0;

	if (arStr->Length == 1) count2 = HashWordCount(sResult);
	else  count2 = HashWordCount(arStr[1]);

	if (count2 == 0) count2 = HashWordCount(sResult);
	if ((count2 == 1 && count1 > 1 && entText->IndexOf(L"\"") < 0 && entText->IndexOf(L"'") < 0 &&
		entText->IndexOf(L"Ђ") < 0) || count1 - count2 > 0)
	{
		sResult = entText;
	}
	else if (arStr->Length > 1)
	{
		sResult = arStr[0];
	}

	sResult = sResult->Trim();
	if (sResult->Length < 2)
	{
		sResult = entText;
	}

	sResult = sResult->Trim();
	sResult = sResult->ToUpper();
		
	return sResult;
}

Generic::List<String^>^ SS::LinguisticProcessor::CIndexation::ConvertArray(const vector<wstring>& list)
{
	Generic::List<String^>^ listArray = gcnew Generic::List<String^>( Math::Max( 1U, list.size() ) );
	for (vector<wstring>::const_iterator it = list.begin(); it != list.end(); ++it)
	{
		String^ str = gcnew String((*it).c_str());
		listArray->Add(str->ToUpper()->Trim());
	}
	return listArray;
}

String^ SS::LinguisticProcessor::CIndexation::GenerateFacts(Generic::List<int>^ offsets, String^ srcText, String^ baseDate, bool isTonality)
{
	using SS::Core::Types::CTextEntity;

	SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
	SS::Core::Types::IndexationSearch::TIndexationParams oIndexationParams;

    wstring text = TOwstring(srcText);
    //TextPretreatment_Letters(text, LinguisticsKernel::SelectEntitiesMode::Full);

	oNativeText.SetText(text.c_str());
	Init( &oNativeText, &oIndexationParams, false);

	m_pText->ClearText();
	String^ sResult;
	if (m_pLexica->LoadNext())
	{
 		m_pLexica->AnalyzeNext(m_pText, &ConvertParagraphsArray( ParseTextForSentences(gcnew String(text.c_str())) ));

		list<int> offs;
		for (int i = 0; i < offsets->Count; ++i)
		{
			offs.push_back(offsets[i]);
		}

		wstring str = m_pDigestAnalyser->GetFactXML(text, m_pText, offs, TOwstring(baseDate), isTonality);
		sResult = gcnew String(str.c_str());
	}
	sResult = SemanticResolution::ServiceFunction::CorrectXML(sResult, gcnew String(text.c_str()));
	return (sResult);
}

bool SS::LinguisticProcessor::CIndexation::EndsWithAny(String^ str, array< String^ >^ values)
{
	for (int i = 0; i < values->Length; ++i)
	{
		if (str->EndsWith(values[i]))
		{
			return true;
		}
	}
	return false;
}

bool SS::LinguisticProcessor::CIndexation::StartsWithAny(String^ str, array< String^ >^ values)
{
	for (int i = 0; i < values->Length; ++i)
	{
		if (str->StartsWith(values[i]))
		{
			return true;
		}
	}
	return false;
}

Generic::List<Linguistics::Core::Entity^>^ SS::LinguisticProcessor::CIndexation::SloganAnalyzing(Linguistics::Core::Content^ content)
{
	SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
	SS::Core::Types::IndexationSearch::TIndexationParams oIndexationParams;

	SS::Core::Features::CMorphoFeature oSloganFirstVerb;
	oSloganFirstVerb.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
	oSloganFirstVerb.m_NumberType = SS::Core::Features::Values::NumberType.ntPlural;
	oSloganFirstVerb.m_TimeType = SS::Core::Features::Values::TimeType.tmtFuture;
	oSloganFirstVerb.m_PersonType = SS::Core::Features::Values::PersonType.ptFirstFace;
	oSloganFirstVerb.m_AspectType = SS::Core::Features::Values::AspectType.atPerfective;
	SS::Core::Features::CMorphoFeature oSloganLastVerb;
	oSloganLastVerb.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postVerb;
	oSloganLastVerb.m_MoodType = SS::Core::Features::Values::MoodType.mtImperative;
	SS::Core::Features::CMorphoFeature oNoun;
	oNoun.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postNoun;
	SS::Core::Features::CMorphoFeature oPretext;
	oPretext.m_PartOfSpeechTypes = SS::Core::Features::Values::PartOfSpeechTypes.postOfficial;
	oPretext.m_OfficialType = SS::Core::Features::Values::OfficialType.otPretext;

	array< String^ >^ endsWithAnyArray = gcnew array< String^ > 
	{
		"!", "к ответу", "распустить", "долой", "на свалку истории", "всем",
		"дл€ всех", "да", "к уголовной ответственности", "в отставку", "под суд", "на службу народу", "нет", "в жизнь"
	};
	array< String^ >^ startsWithAnyArray = gcnew array< String^ > 
	{
		"все на", "долой", "за", "против", "нет", "да здравствует", "даешь", "слава", "мы за", "мы против", "прочь",
		"свободу", "спасите", "позор", "€ за", "€ против", "хватит", "довольно"
	};

	Generic::List<Linguistics::Core::Entity^>^ sloganList = GetSimpleEntities(m_pSimpleEntitySelector->SelectSimpleEntities(content->TextValue, m_pSloganRegex, Linguistics::Core::EntityType::Slogan.ToString()), 0);
	for (int i = 0; i < sloganList->Count; ++i)
	{
		Linguistics::Core::Entity^ slogan = sloganList[i];
		String^ sloganText = slogan->Text->Substring(1, slogan->Text->Length - 2);
		Linguistics::Core::Content^ sloganContent = gcnew Linguistics::Core::Content(sloganText);
		if (IsPossibleSlogan(sloganContent))
		{
			String^ lowerText = sloganText->ToLower();
			if (EndsWithAny(lowerText, endsWithAnyArray) || StartsWithAny(lowerText, startsWithAnyArray))
			{
				continue;
			}

			String^ word;
			if (lowerText->StartsWith("не"))
			{
				word = GetFirstWord(lowerText->Substring(2));
			}
			else
			{
				word = GetFirstWord(lowerText);
			}
			if (word)
			{
				if (IsMorpho(TOwstring(word), oSloganFirstVerb))
				{
					continue;
				}
				
				word = GetLastWord(lowerText);
				if (IsMorpho(TOwstring(word), oSloganLastVerb))
				{
					continue;
				}

				int dashIndex = -1;
				for (int j = 0; j < lowerText->Length; ++j)
				{
					Char ch = lowerText[j];
					bool isDash = (ch == L'-') || (ch == L'Ц') || (ch == L'Ч');
					if (isDash && (j > 0) && (j < lowerText->Length - 1))
					{
						bool isNearLetterOrDigit = Char::IsLetterOrDigit(lowerText[j - 1]) ||
							                       Char::IsLetterOrDigit(lowerText[j + 1]);
						if (!isNearLetterOrDigit)
						{
							dashIndex = j;
							break;
						}
					}
				}

				if ((dashIndex > 0) && IsMorpho(TOwstring(word), oNoun))
				{
					String^ lastWord = word;
					word = GetLastWord(lowerText->Substring(0, dashIndex));
					if (IsMorpho(TOwstring(word), oNoun))
					{
						continue;
					}
					else
					{
						String^ afterDashText = lowerText->Substring(dashIndex + 1);
						word = GetFirstWord(afterDashText);
						if (word)
						{
							int wordPosition = afterDashText->IndexOf(word);
							if (IsMorpho(TOwstring(word), oPretext) &&
								GetFirstWord(lowerText->Substring(dashIndex + 1 + wordPosition))->Equals(lastWord))
							{
								continue;
							}
						}
					}
				}
			}
		}
		sloganList->RemoveAt(i);
		--i;
	}
	content->UnionSentencesByEntities(sloganList);
	return (sloganList);
}

String^ SS::LinguisticProcessor::CIndexation::GetFirstWord(String^ str)
{
	int startPosition = -1;
	int length = 0;
	for (int i = 0; i < str->Length; ++i)
	{
		if (Char::IsLetter(str[i]))
		{
			if (startPosition < 0)
			{
				startPosition = i;
			}
			++length;
		}
		else if (startPosition >= 0)
		{
			break;
		}
	}
	return ((startPosition >= 0) ? str->Substring(startPosition, length) : nullptr);
}
String^ SS::LinguisticProcessor::CIndexation::GetLastWord(String^ str)
{
	int startPosition = -1;
	int length = 0;
	for (int i = str->Length - 1; i >= 0; --i)
	{
		if (Char::IsLetter(str[i]))
		{
			startPosition = i;
			++length;
		}
		else if (startPosition >= 0)
		{
			break;
		}
	}
	return ((startPosition >= 0) ? str->Substring(startPosition, length) : nullptr);
}

bool SS::LinguisticProcessor::CIndexation::IsPossibleSlogan(Linguistics::Core::Content^ slogan)
{
	Generic::IList<TextParsing::Paragraph^>^ paragraphList = Utils::Converter::ToList(slogan->GetParagraphs());
	int sentenceCount = 0;
	array< Char >^ chs = gcnew array< Char > {' '};
	for (int i = 0; i < paragraphList->Count; ++i)
	{
		TextParsing::Paragraph^ paragraph = paragraphList[i];
		for (int j = 0; j < paragraph->Sentences->Length; ++j)
		{
			++sentenceCount;
			array< String^ >^ words = ((TextParsing::Sentence^) paragraph->Sentences[j])->Text->Split(chs, StringSplitOptions::RemoveEmptyEntries);			
			if ((words->Length > 10) || (sentenceCount > 3))
			{
				return false;
			}
		}
	}
	return true;
}

bool SS::LinguisticProcessor::CIndexation::IsMorpho(const wstring& word, const SS::Core::Features::CMorphoFeature& oMorpho)
{
	list<SS::Dictionary::Types::TWordInfo> info;
	m_pMorphoAnalyzer->GetWordInfo(word.c_str(), &info, SS::Dictionary::Types::efzOnlySearch);
	for (std::list<SS::Dictionary::Types::TWordInfo>::iterator it = info.begin(); it != info.end(); ++it)
	{	
		SS::Core::Features::CMorphoFeature oMorphoFeature;
		SS::Dictionary::Types::SWordInfo* pwi1 = &(it->front());

        m_pAMConverter->EnCode(
			pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
			pwi1->m_MorphoTypeInfo.m_MorphoInfo,
			pwi1->m_SpecialMorpho,
			&oMorphoFeature);

		if (oMorpho.IsIntersectsWith(oMorphoFeature))
		{
			return true;
		}
	}
	return false;
}

String^ SS::LinguisticProcessor::CIndexation::GenerateSentXMLDigest(String^ sText, ArrayList^ offsets, ArrayList^ themeIDs, String^ dictName)
{
	SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
	SS::Core::Types::IndexationSearch::TIndexationParams oIndexationParams;

	sText = sText->Replace("\n", " ");
	sText = sText->Replace("\r", " ");

    wstring text = TOwstring(sText);
    oNativeText.SetText(text.c_str());
		
	Init( &oNativeText, &oIndexationParams, false);

    wstring dname = TOwstring(dictName);

	m_pText->ClearText();
	if (!m_pLexica->LoadNext())
	{
		return nullptr;
	}
 	m_pLexica->AnalyzeNext(m_pText, &ConvertParagraphsArray( ParseTextForSentences(gcnew String(text.c_str())) ));

	list<int> offs;
	for (int i = 0; i < offsets->Count ; ++i)
	{
		offs.push_back((int)(offsets[i]));
	}

	list<int> themeIds;
	for (int i = 0; i < themeIDs->Count; ++i)
	{
		themeIds.push_back((int)(themeIDs[i]));
	}
	
	wstring str = m_pDigestAnalyser->GetSentDigestXML(m_pText, offs, themeIds, dname);

	String^ sResult = gcnew String(str.c_str());
	return (sResult);
}

String^ SS::LinguisticProcessor::CIndexation::TransliteringText(String^ srcText)
{
    const wchar_t* chars = (const wchar_t*)(Marshal::StringToHGlobalUni(srcText)).ToPointer();
    m_pTransliterationFrench->SetDictionary(L"D:\\DKMS\\Lingvistic2008\\Sources\\Core\\Transliteration\\world brands.txt");
    wstring str = m_pTransliterationFrench->CorrectString(chars);
    String^ result = gcnew String(str.c_str());
    return result;
}

/*bool SS::LinguisticProcessor::CIndexation::ThemeFromThesaurus(
    wstring& lexem,
    int themeID,
    SS::SemanticNetwork::ThemesVectorCollection &themes,
    SS::SemanticNetwork::ConnectionsVectorCollections &connections,
    String^ thesaurusId,
    Generic::List<Thesaurus::PartOfSpeech*>* partsOfSpeech,
    Generic::List<String^>^ &thesaurusReplace,
    Generic::List<int>* &lexemHierarchy)
{
    using namespace System::Data;
    using namespace Thesaurus;
    
    bool result = false;

    int index = lexemHierarchy->IndexOf(themeID);
    if(index < 0)
        lexemHierarchy->Add(themeID);
    else
    /// зацикливание
    {
        return false;
    }

    String^ theme = "";
    /// попробуем найти текущую сущность в уже найденных темах в тезаурусе
    index = thesaurusReplace->IndexOf(lexem.c_str());
    if(index >= 0)
    {
        if(index % 2 == 0)
        /// такую тему уже замен€ли
        {
            theme = thesaurusReplace->Item[index + 1];
        }
        else
        /// на эту тему замен€ли
        {
            theme = thesaurusReplace->Item[index];
        }
    }
    else
    {
        List<String^>^ Synonyms = gcnew List<String^>();
        DataTable* table = Thesaurus::DSThesaurus::GetSynset(lexem.c_str(), thesaurusId);
        CorrectThemesTable(table, Synonyms, partsOfSpeech, 1, 3);
        if(Synonyms->Count == 1)
            theme = Synonyms->Item[0];
        else
        {
            DataTable* table2 = Thesaurus::DSThesaurus::GetSynsetBySynonym(lexem.c_str(), thesaurusId);
            CorrectThemesTable(table2, Synonyms, partsOfSpeech, 1, 4);
            if(Synonyms->Count == 1)
                theme = Synonyms->Item[0];
        }
        if(theme != "")
        {
            theme = theme->ToUpper();
#ifdef __SEMANTIC_NETWORK_LOG
            const wchar_t* pTheme = 
			    (const wchar_t*)(Marshal::StringToHGlobalUni(theme)).ToPointer();
            m_pSemanticNetwork->WriteToLog(lexem + L" -> " + pTheme);
#endif
        }
    }

    if(theme != "")
    /// удалось найти тему в тезаурусе
    {
        if(index < 0)
        {
            /// что заменили
            thesaurusReplace->Add(lexem.c_str());
            /// на что заменили
            thesaurusReplace->Add(theme);
        }

        wchar_t* pNormalization = 
            (wchar_t*)(Marshal::StringToHGlobalUni(theme)).ToPointer();
        result = true;
        lexem = pNormalization;
    }
    else
    {
        int newShift = 0;
        wstring prevLexem = lexem;
        int minLine = 0;
        for(SS::SemanticNetwork::ConnectionsVectorCollections::iterator it = connections.begin();
            it != connections.end();
            ++it)
        {
            if(((*it).GetConnectionType() == L"—лужебное") && ((*it).GetFirstThemeID() == themeID))
            {
                SS::SemanticNetwork::ThemesVectorCollection::iterator iter = themes.find((*it).GetSecondThemeID());
                if(iter != themes.end())
                {
                    wstring lowerTheme = (*iter).second.GetTheme().GetNormalization();
                    wstring prevLowerVersion = lowerTheme;
                    result = ThemeFromThesaurus(lowerTheme, (*iter).first, themes, connections, thesaurusId, 
                                                partsOfSpeech, thesaurusReplace, lexemHierarchy);
                    if(result && (lowerTheme != prevLowerVersion))
                    /// замену будем производить тогда, когда вложенна€ тема изменилась
                    {
                        int shift = 0;
                        int length = 0;
                        (*it).GetServiceConnectionParams(shift, length);
                        shift += newShift;
                        if(shift >= minLine)
                        {
                            wstring back = lexem.substr(shift + length, lexem.length() - (shift + length - 1));
                            if(shift > 0)
                                lexem = lexem.substr(0, shift - 1);
                            else
                                lexem = L"";
                            int prevLength = lexem.length();
                            
                            bool finish = false;
                            list<SS::Core::Features::Types::CaseType> caces;
                            for(int pos = lowerTheme.find(L" "), prevPos = 0;
                                !finish;
                                pos = lowerTheme.find(L" ", prevPos))
                            {
                                int cutLen = 0;
                                if(pos != wstring::npos)
                                    cutLen = pos - prevPos;
                                else
                                {
                                    cutLen = lowerTheme.length() - prevPos;
                                    finish = true;
                                }
                                wstring word = lowerTheme.substr(prevPos, cutLen);
                                if(lexem != L"")
                                {
                                    wstring res = GetGenitiveFrom(word);
                                    if(res != L"")
                                        word = res;
                                    lexem += L" " + word;
                                }
                                else
                                    lexem = word;
                                prevPos = pos + 1;
                            }
                            minLine = shift + lexem.length();
                            /// (длина нового вставленного куска) - (длина старого удаленного куска)
                            newShift += (lexem.length() - prevLength) - (length + 1);
                            lexem += back;
                        }
                    }
                }
            }
        }
        thesaurusReplace->Add(prevLexem.c_str());
        thesaurusReplace->Add(lexem.c_str());
    }
    lexemHierarchy->Remove(themeID);
    return result;
}*/

/*void SS::LinguisticProcessor::CIndexation::CorrectThemesTable(
    System::Data::DataTable* &table, 
    Generic::List<String^>^ &themes,
    Generic::List<Thesaurus::PartOfSpeech*>* partsOfSpeech,
    int partOfSpeechIndex,
    int themeIndex)
{
    if(!table)
        return;
    for(int i = 0; i < table->Rows->Count; ++i)
    {
        bool find = false;
        String^ partsOfSpeechID = table->Rows->Item[i]->ItemArray[partOfSpeechIndex]->ToString();
        for(int j = 0; j < partsOfSpeech->Count; ++j)
            if(partsOfSpeech->Item[j]->id->Equals(partsOfSpeechID))
                find = true;
        if(find)
            themes->Add(table->Rows->Item[i]->ItemArray[themeIndex]->ToString());
    }
}*/

wstring SS::LinguisticProcessor::CIndexation::GetGenitiveFrom(const wstring& word)
{
    wstring result = L"";
    if ((word != L"") && (word.length() < SS::Constants::MAX_CHARS_IN_WORD))
    {
        list<SS::Dictionary::Types::TWordInfo> info;
        m_pMorphoAnalyzer->GetWordInfo(word.c_str(), &info, SS::Dictionary::Types::efzOnlySearch);
        std::list<SS::Dictionary::Types::TWordInfo> infoDetect;
        for (std::list<SS::Dictionary::Types::TWordInfo>::iterator it = info.begin();
             (it != info.end()) && (result == L"");
             ++it
		    )
        {	
            SS::Core::Features::CMorphoFeature oMorphoFeature;			
            SS::Dictionary::Types::SWordInfo* pwi1 = &(it->front());

            infoDetect.clear();
            if (m_pMorphoAnalyzer->GetWordFormsByIDSource(&(it->front().m_WordIndex.m_DictIndex), &(infoDetect)))
            {
                for (std::list<SS::Dictionary::Types::TWordInfo>::iterator itWord = infoDetect.begin();
                     (itWord != infoDetect.end()) && (result == L"");
                     ++itWord
					)
                {
                    pwi1 = &(itWord->front());

                    m_pAMConverter->EnCode(
                        pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
                        pwi1->m_MorphoTypeInfo.m_MorphoInfo,
                        pwi1->m_SpecialMorpho,
                        &oMorphoFeature);

                    if (oMorphoFeature.m_CaseType.Equal(SS::Core::Features::Values::CaseType.ctGenitive) &&
                        oMorphoFeature.m_NumberType.Equal(SS::Core::Features::Values::NumberType.ntSingle))
                    {
                        result = (itWord->front()).m_wWord;
                    }
                }
            }
        }
        std::transform(result.begin(), result.end(), result.begin(), ::towupper);
    }
    return (result);
}

String^ SS::LinguisticProcessor::CIndexation::GetTextAllNormalForms(String^ text)
{
    XmlDocument^ xmlDoc = gcnew XmlDocument();
    XmlDeclaration^ xmlDeclaration = xmlDoc->CreateXmlDeclaration("1.0", "utf-8", "");
    XmlElement^ rootNode  = xmlDoc->CreateElement("TEXT");
    xmlDoc->InsertBefore(xmlDeclaration, xmlDoc->DocumentElement); 
    xmlDoc->AppendChild(rootNode);
    if (!String::IsNullOrEmpty( text ))
    {
        wstring srcText = TOwstring(text);
        SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
	    SS::Core::Types::IndexationSearch::TIndexationParams oIndexationParams;

        oNativeText.SetText(srcText.c_str());

	    Init( &oNativeText, &oIndexationParams, false);
	        
        m_pText->ClearText();
        if (!m_pLexica->LoadNext())
		{
	        return nullptr;
		}
        m_pLexica->AnalyzeNext(m_pText, &ConvertParagraphsArray(ParseTextForSentences( gcnew String(srcText.c_str()) )), true, LinguisticsKernel::SelectEntitiesMode::Simple);

        using namespace SS::Interface::Core::BlackBox;

        for (IParagraph* pPar = m_pText->GetFirstParagraph(); 
             pPar;
             pPar = pPar->GetNextParagraph())
        {
            for (ISentence* pSen = pPar->GetFirstSentence(); 
                 pSen;
                 pSen = pSen->GetNextSentence())
            {
                /// Unit-ы предложени€
                std::list<IUnit*> sentUnits;
                pSen->GetLinearUnitsFull(sentUnits);
                for (TUnitList::iterator it = sentUnits.begin(); it != sentUnits.end(); ++it)
                {
                    wstring mainWord = (*it)->GetWord();
                    if (!mainWord.empty())
                    {
                        std::list<SS::Dictionary::Types::TWordInfo> oLst;
                        m_pMorphoAnalyzer->GetWordInfo(mainWord.c_str(), &oLst, SS::Dictionary::Types::efzOnlySearch);

                        for (std::list<SS::Dictionary::Types::TWordInfo>::iterator iter = oLst.begin();
                             iter != oLst.end();
                             ++iter
							)
                        {
                            SS::Core::Features::CMorphoFeature oBaseFeature;
                            m_pAMConverter->EnCode(
	                            (*iter).front().m_WordIndex.m_DictIndex.GetDictionaryNumber(),
	                            (*iter).front().m_MorphoTypeInfo.m_MorphoInfo,
	                            (*iter).front().m_SpecialMorpho,					
	                            &oBaseFeature);

                            SS::Dictionary::Types::SWordInfo* pwi1 = &(iter->front());
		                    // ѕреобразует закодированные морфологические характеристики	
		                    std::list<SS::Dictionary::Types::TWordInfo> lWordInfoDetect;

		                    m_pMorphoAnalyzer->GetWordFormsByIDSource(&pwi1->m_WordIndex.m_DictIndex, &lWordInfoDetect);

                            wstring normalWord = L"";
                            wstring undef = L"";
                            SS::Core::Features::CMorphoFeature oMorphoFeature;
                            for (std::list<SS::Dictionary::Types::TWordInfo>::iterator it1 = lWordInfoDetect.begin();
                                 (normalWord == L"") && (it1 != lWordInfoDetect.end());
                                 ++it1
								)
                            {
                                m_pAMConverter->EnCode(
		                            (*it1).front().m_WordIndex.m_DictIndex.GetDictionaryNumber(),
		                            (*it1).front().m_MorphoTypeInfo.m_MorphoInfo,
		                            (*it1).front().m_SpecialMorpho,					
		                            &oMorphoFeature);

                                if (oMorphoFeature.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postNumeric) ||
                                    oMorphoFeature.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postPronoun) ||
                                    oMorphoFeature.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postOfficial))
                                {
                                    continue;
                                }

                                if (oMorphoFeature.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postVerb))
                                {
                                    if (oMorphoFeature.m_VerbForm.Equal(SS::Core::Features::Values::VerbForm.vfInfinitive))
									{
                                        normalWord = (*it1).front().m_wWord;
									}
                                    else if((undef == L"" ) && oMorphoFeature.m_VerbForm.IsUndefined())
									{
                                        undef = (*it1).front().m_wWord;
									}
                                }
                                else if((oMorphoFeature.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postNoun) ||
                                         oMorphoFeature.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postAdjective)) &&
                                         oMorphoFeature.m_GenderType.Equal(oBaseFeature.m_GenderType) &&
                                         oMorphoFeature.m_ShortType.IsUndefined())
                                {
                                    if (oMorphoFeature.m_CaseType.Equal(SS::Core::Features::Values::CaseType.ctNominative) &&
                                        oMorphoFeature.m_NumberType.Equal(SS::Core::Features::Values::NumberType.ntSingle))
                                    {
                                        normalWord = (*it1).front().m_wWord;
                                    }
                                    
                                    if (oMorphoFeature.m_CaseType.Equal(SS::Core::Features::Values::CaseType.ctNominative) &&
                                        oMorphoFeature.m_NumberType.IsUndefined())
                                    {
                                        undef = (*it1).front().m_wWord;
                                    }
                                    if ((undef == L"" ) && oMorphoFeature.m_CaseType.IsUndefined())
									{
                                        undef = (*it1).front().m_wWord;
									}
                                }
                                else if(oMorphoFeature.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postAdverb))
                                {
                                    normalWord = (*it1).front().m_wWord;
                                }
                            }

                            if (normalWord == L"")
							{
                                normalWord = undef;
							}

                            if (normalWord != L"")
                            {
                                XmlElement^ node = xmlDoc->CreateElement("WORD");
                                node->SetAttribute("PartOfSpeech", gcnew String(oMorphoFeature.m_PartOfSpeechTypes.GetValueDescription().c_str()) );
                                if (oMorphoFeature.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postNoun) ||
                                    oMorphoFeature.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postAdjective))
                                {
                                    node->SetAttribute("Gender", gcnew String(oMorphoFeature.m_GenderType.GetValueDescription().c_str()) );
                                }
                                node->SetAttribute("BaseForm", gcnew String(mainWord.c_str()) );
                                node->AppendChild(xmlDoc->CreateTextNode( gcnew String(normalWord.c_str()) ));
                                rootNode->AppendChild(node);
                            }
                        }
                    }
                }
            }
        }
    }
    StringWriter^ sw = gcnew StringWriter();
    XmlTextWriter^ tx = gcnew XmlTextWriter(sw);
    xmlDoc->WriteTo(tx);
    return (sw->ToString());
}

enum WordStatus
{
    eUnknow,        /// неизвестна семантика слова
    eNoUpper,       /// NominalName - слово должно быть записано в нижнем регистре
    eFirstUpper,    /// ProperName - перва€ буква должна быть заглавна€, остальные - маленькие
    eAllUpper,      /// Abbreviations - все слово в верхнем регистре
    eNotAllUpper    /// слово считаетс€ записанным правильно
};

int GetLastCharPosition(SS::Interface::Core::BlackBox::IUnit *pUnit, const wstring& text)
{
    int lastPosInText = pUnit->GetLastCharPositionNew(text.c_str());
    if (lastPosInText < 0)
    {
        SS::Interface::Core::BlackBox::IUnit* pLastChild = pUnit->GetRightChildUnit();
        if (pLastChild)
		{
            lastPosInText = pLastChild->GetLastCharPositionNew(text.c_str());
		}
    }
    return (lastPosInText);
}

void SS::LinguisticProcessor::CIndexation::TextPretreatment_Letters(wstring& text, LinguisticsKernel::SelectEntitiesMode mode)
{
	/// поставим пробелы вместо "\t"
    for (int pos = text.find(L'\t'); pos != wstring::npos; pos = text.find(L'\t'))
	{
	    text[pos] = ' ';
	}

	int text_len = text.length();
    for (unsigned int i = 0, text_len_minus_1 = text_len - 1; i < text_len; i++)
    {
        int letterIndex = ExistLetter( text[ i ], *_EnglishLetters );
        if (letterIndex >= 0)
        /// нашли английскую букву, написанием похожую на русскую
        {
            int index = -1;
            if ((i == 0) || !iswalpha( text[ i - 1 ] ))
            /// слева пробел
            {
                if (i == text_len_minus_1)
				{
                    continue;
				}
                index = i + 1;
            }
            else
			{
                index = i - 1;
			}
            if (ExistLetter( text[ index ], *_AllRussianLetters) >= 0)
			{
                text[ i ] = (*_RussianLetters)[ letterIndex ];
			}
        }
    }

	for (unsigned int i = 0; i < text_len; i++)
    {
        int letterIndex = ExistLetter( text[ i ], *_YELetters );
		if (letterIndex >= 0)
		{
			text[ i ] = (*_YEReplaceLetters)[ letterIndex ];
		}
    }

    SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
	SS::Core::Types::IndexationSearch::TIndexationParams oIndexationParams;

    oNativeText.SetText(text.c_str());		
	Init( &oNativeText, &oIndexationParams, false);

	m_pText->ClearText();
	if (m_pLexica->LoadNext())
	{
		m_pLexica->AnalyzeNextSentence(m_pText, true, mode);
	}

    //RussianLetterCorrect(text, russianLetters, englishLetters);

	wstring sourceText = text;
    UpperTextProcessing(text);

	SS::Interface::Core::BlackBox::IParagraph* pPar = m_pText->GetLastParagraph();
	if (pPar)
	{
		SS::Interface::Core::BlackBox::ISentence* pSen = pPar->GetLastSentence();
		if (pSen)
		{
			SS::Interface::Core::BlackBox::IUnit* pUnit = pSen->GetLastUnit();
			if (pUnit)
			{
				int lastPosition = GetLastCharPosition(pUnit, sourceText);
				text = text.substr(0, lastPosition);
			}
		}
	}
}

void SS::LinguisticProcessor::CIndexation::RussianLetterCorrect(wstring& text, const wstring& russianLetters, const wstring& englishLetters)
{
    using namespace SS::Interface::Core::BlackBox;

    for (IParagraph* pPar = m_pText->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
    {
        for (ISentence* pSen = pPar->GetFirstSentence(); pSen; pSen = pSen->GetNextSentence())
        {
            /// Unit-ы предложени€
            std::list<IUnit*> sentUnits;
            pSen->GetLinearUnitsFull(sentUnits);
            for (TUnitList::iterator it = sentUnits.begin(); it != sentUnits.end(); ++it)
            {
                IIndex* pIndex = (*it)->GetFirstIndex();
	            if (pIndex && pIndex->GetDictionaryIndex()->GetFirst().GetLanguage() == SS::Core::Types::ealEnglish)
                /// английскими остаютс€ слова, в которых остались английские символы
                {
                    int firstPosInText = (*it)->GetFirstCharPositionNew();
                    int lastPosInText = GetLastCharPosition(*it, text);

                    wstring word = text.substr(firstPosInText, lastPosInText - firstPosInText);
                    list<SS::Dictionary::Types::TWordInfo> info;
                    m_pMorphoAnalyzer->GetWordInfo(word.c_str(), &info, SS::Dictionary::Types::efzOnlySearch);
                    for (list<SS::Dictionary::Types::TWordInfo>::iterator iter = info.begin(); iter != info.end(); ++iter)
                    {
                        if (!(iter->front()).m_NewWord)
						{
                            continue;
						}

                        bool changed = false;
                        for (unsigned int j = 0; j < word.length(); ++j)
                        {
                            int letterIndex = ExistLetter(word[j], russianLetters);
                            if (letterIndex >= 0)
                            {
                                word[j] = englishLetters[letterIndex];
                                changed = true;
                            }
                        }
                        if (changed)
						{
                            text.replace(firstPosInText, lastPosInText - firstPosInText, word);
						}
                    }
                }
            }
        }
    }
}

int SS::LinguisticProcessor::CIndexation::ExistLetter(wchar_t letter, const wstring& letterList)
{
    int result = -1;
    for (unsigned int i = 0, len = letterList.length(); (result < 0) && (i < len); ++i)
	{
        if (letterList[i] == letter)
		{
            result = i;
		}
	}
    return (result);
}

void SS::LinguisticProcessor::CIndexation::UpperTextProcessing(wstring& text)
{
    using namespace SS::Interface::Core::BlackBox;

    int sentUnitsShift = 0;
    for (IParagraph* pPar = m_pText->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
    {
        for (ISentence* pSen = pPar->GetFirstSentence(); pSen; pSen = pSen->GetNextSentence())
        {
            int position = 0;
            /// Unit-ы предложени€
            std::list<IUnit*> sentUnits;
            pSen->GetLinearUnitsFull(sentUnits);
            /// предыдущий юнит записан верхним регистром
            bool isPrevUpper = false;
            int seriaIndex = -1;
            /// серии Unit-ов в верхнем регистре
            vector<vector<IUnit*>> serias;
            /// комбинаторные Unit-ы
            vector<bool> combinatories;
            for (TUnitList::iterator it = sentUnits.begin(); it != sentUnits.end(); ++it)
            /// получение серий Unit-ов в верхнем регистре
            {
				IUnit *pUnit = *it;
                wstring word = pUnit->GetWord();
                position = pUnit->GetPosition();
                if (!word.empty())
                {
                    int status = UpperTextProcessing_Test(pUnit);
                    bool isOpenQuote = IsOpenQuote(word);
                    if (isOpenQuote)
                    {
                        int pos = pUnit->GetFirstCharPosition();
                        if ((pos == text.length() - 1) || (text[pos + 1] == L' '))
						{
                            isOpenQuote = false;
						}
                    }
                    if ((status == eAllUpper) || isOpenQuote)
                    {
                        if (!isPrevUpper)
                        {
                            ++seriaIndex;
                            serias.resize(seriaIndex + 1);
                        }
                        serias[seriaIndex].push_back(pUnit);
                        isPrevUpper = true;
                    }
                    else
					{
                        isPrevUpper = false;
					}
                }
                else
                /// может быть Unit-ом комбинаторики
                {
                    bool isCombinatory = false;
                    for (IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
                    {
                        IDictionaryIndex* pDictionaryIndex = pIndex->GetDictionaryIndex();
                        if (pDictionaryIndex)
                        {
                            SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex = pDictionaryIndex->GetFirst();
	                        EIndexType oIndexType = oDictionaryIndex.GetIndexType();
                            if (oIndexType == eitCombinatoryIndex)
							{
                                isCombinatory = true;
							}
                        }
                    }
                    if (isCombinatory)
                    {
                        for (IUnit* pChild = pUnit->GetLeftChildUnit(); pChild; pChild = pChild->GetRightUnit())
                        {
                            int pos = pChild->GetPosition() - sentUnitsShift;
                            if (combinatories.size() <= pos)
							{
                                combinatories.resize(pos + 1);
							}
                            combinatories[pos] = true;
                        }
                    }
                }
            }
            if (!serias.empty())
			{
                UpperTextProcessing_UnitSeria(text, serias, combinatories, sentUnitsShift);
			}
            sentUnitsShift = position + 1;
        }
    }
}

int SS::LinguisticProcessor::CIndexation::UpperTextProcessing_Test(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
    wstring word = pUnit->GetWord();
    int result = eUnknow;
    if (iswalpha(word[0]))
    {
        bool bigWord = true;
        for (unsigned int i = 0, len = word.length(); i < len; ++i)
		{
			wchar_t ch = word[i];
            bigWord = bigWord && (iswupper(ch) || (ch == L'-'));
		}
        result = (bigWord) ? eAllUpper : eNotAllUpper;
    }
    return (result);
}

void SS::LinguisticProcessor::CIndexation::UpperTextProcessing_UnitSeria(
    wstring& text,
    vector<vector<SS::Interface::Core::BlackBox::IUnit*>> &serias,
    vector<bool> &combinatories, int shift)
{
    /// true, если надо обработать текст
    bool needProcess = false;
    for (vector<vector<SS::Interface::Core::BlackBox::IUnit*>>::iterator it = serias.begin(); it != serias.end(); ++it)
    /// цикл по сери€м
    {
        if ((*it).size() == 1)
		{
            continue;
		}
        std::vector<int> unitsStatus((*it).size(), eUnknow);
        int unitIndex = 0;
        /// число оишбок подр€д
        int numErrors = 0;
        for (vector<SS::Interface::Core::BlackBox::IUnit*>::iterator iter = (*it).begin(); 
             iter != (*it).end();
             ++iter, ++unitIndex
			)
        /// цикл по Unit-ам
        /// определение необходимости преобразовани€ серии
        {
            bool error = false;
            wstring word = (*iter)->GetWord();
            int position = (*iter)->GetPosition() - shift;
            bool isCombinatory = (position < combinatories.size()) ? combinatories[position] : false;
            UpperTextProcessing_Unit(*iter, unitsStatus[unitIndex], isCombinatory);
            if (word.length() > 6)
			{
                error = true;
			}

            if (error || !((unitsStatus[unitIndex] == eAllUpper) || (unitsStatus[unitIndex] == eUnknow)))
			{
                ++numErrors;
			}
            else if (numErrors > 0)
            {
                if (numErrors < 2)
				{
                    unitsStatus[unitIndex - 1] = eUnknow;
				}
                else
				{
                    needProcess = true;
				}
                numErrors = 0;
            }
        }
		needProcess = (numErrors >= 3);
        if (needProcess || ((*it).size() > 3))
        /// проведение преобразовани€
        {
            bool isPrevQuote = false;
            unitIndex = 0;
            for (vector<SS::Interface::Core::BlackBox::IUnit*>::iterator iter = (*it).begin(); 
                 iter != (*it).end();
                 ++iter, ++unitIndex
				)
            {
                if (unitsStatus[unitIndex] != eNotAllUpper)
                {
                    int status = unitsStatus[unitIndex];
                    if (isPrevQuote)
					{
                        status = eFirstUpper;
					}
                    UpperTextProcessing_UnitReplace(text, *iter, status);
                    wstring word = (*iter)->GetWord();
                    isPrevQuote = IsOpenQuote(word);
                }
            }
        }
    }
}

bool SS::LinguisticProcessor::CIndexation::IsOpenQuote(wstring& str)
{
    if (str.size() != 1)
	{
        return (false);
	}
    wchar_t ch = str[ 0 ];
    return ((ch == (wchar_t)0x201C) || (ch == L'У') || (ch == L'Ђ') || (ch == L'"') || (ch == L'\''));
}

void SS::LinguisticProcessor::CIndexation::UpperTextProcessing_Unit(SS::Interface::Core::BlackBox::IUnit* pUnit, int &unitStatus, bool isCombinatory)
{
    unsigned int position = pUnit->GetFirstCharPositionNew();
    wstring word = pUnit->GetWord();
    if (isCombinatory)
	{
        unitStatus = eFirstUpper;
	}
    else if (word != L"")
	{
        UpperTextProcessing_Word(word, unitStatus);
	}
}

void SS::LinguisticProcessor::CIndexation::UpperTextProcessing_UnitReplace(wstring& text, SS::Interface::Core::BlackBox::IUnit* pUnit, int unitStatus)
{
    wstring word = pUnit->GetWord();
    if (word != L"")
    {
        int shift = -1;
        switch (unitStatus)
        {
			case eFirstUpper:
                shift = 1;
                break;
            case eNoUpper:
                shift = 0;
                break;
            case eUnknow:
                if(word.length() > 6)
                    shift = 1;
                break;
            default: break;
        }
        if (shift >= 0)
        /// слово надо изменить
        {
            int firstPosInText = pUnit->GetFirstCharPositionNew();
            int lastPosInText = GetLastCharPosition(pUnit, text);
            word = text.substr(firstPosInText, lastPosInText - firstPosInText);
            std::transform(word.begin() + shift, word.end(), word.begin() + shift, ::towlower);
            text.replace(firstPosInText, word.length(), word);
        }
    }
}

void SS::LinguisticProcessor::CIndexation::UpperTextProcessing_Word(wstring& word, int &status)
{
    if ((word.length() < SS::Constants::MAX_CHARS_IN_WORD) && (word.length() > 1))
    {
        WordStatus oper = eUnknow;
        list<SS::Dictionary::Types::TWordInfo> info;
        m_pMorphoAnalyzer->GetWordInfo(word.c_str(), &info, SS::Dictionary::Types::efzOnlySearch);
        for (list<SS::Dictionary::Types::TWordInfo>::iterator iter = info.begin(); 
             iter != info.end();
             ++iter
			)
        {	
            SS::Core::Features::CMorphoFeature oMorphoFeature;			
            SS::Dictionary::Types::SWordInfo* pwi1 = &(iter->front());

            switch (pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber())
            {
                case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsRus:
                case SS::Dictionary::DATA_TYPE::NAMES::ednAbbreviationsEng: 
                    oper = eAllUpper;
                    break;
                case SS::Dictionary::DATA_TYPE::NAMES::ednRProperName:
                case SS::Dictionary::DATA_TYPE::NAMES::ednEProperName:
                    if(oper < eFirstUpper)
                        oper = eFirstUpper;
                    break;
                case SS::Dictionary::DATA_TYPE::NAMES::ednRNominalName:
                case SS::Dictionary::DATA_TYPE::NAMES::ednENominalName:
                    if(oper < eNoUpper)
                        oper = eNoUpper;
                    break;
            }
        }
        status = oper;
    }
}

wstring ToLower(const wstring& word)
{
	wstring result = L"";
	for (unsigned int i = 0, len = word.length(); i < len; ++i)
	{
		result += towlower(word[i]);
	}
	return (result);
}

wstring SS::LinguisticProcessor::CIndexation::GetNormalForm(SS::Dictionary::Types::TWordInfo& info)
{
	wstring normal = L"";
	std::list<SS::Dictionary::Types::TWordInfo> infoDetect;
	if (!info.front().m_NewWord &&
		m_pMorphoAnalyzer->GetWordFormsByIDSource(&(info.front().m_WordIndex.m_DictIndex), &(infoDetect)))
	{
		SS::Core::Types::EAnalyseLanguage eal = info.front().m_WordIndex.m_DictIndex.GetLanguage();
		Core::Features::Types::GenderType gender;
		Core::Features::Types::NumberType number;
		for (std::list<SS::Dictionary::Types::TWordInfo>::iterator it = infoDetect.begin();
			 it != infoDetect.end();
			 ++it
		    )
		{
			SS::Core::Features::CMorphoFeature oMorphoFeature;
			SS::Dictionary::Types::SWordInfo *pwi = &(it->front());

			m_pAMConverter->EnCode(
				pwi->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
				pwi->m_MorphoTypeInfo.m_MorphoInfo,
				pwi->m_SpecialMorpho,
				&oMorphoFeature);

			if (oMorphoFeature.m_PartOfSpeechTypes.Equal(Core::Features::Values::PartOfSpeechTypes.postVerb))
			{
				if (oMorphoFeature.m_VerbForm.Equal(Core::Features::Values::VerbForm.vfInfinitive))
				{
					normal = pwi->m_wWord.c_str();
				}
			}
			else if ((eal == SS::Core::Types::ealEnglish) &&
				     oMorphoFeature.m_PartOfSpeechTypes.Equal(Core::Features::Values::PartOfSpeechTypes.postAdjective))
			{
				if (oMorphoFeature.m_DegreeType.IsUndefined())
				{
					normal = pwi->m_wWord.c_str();
				}
			}
			else
			{
				if (oMorphoFeature.m_CaseType.Equal(Core::Features::Values::CaseType.ctNominative) ||
					oMorphoFeature.m_CaseType.Equal(Core::Features::Values::CaseType.ctCommon))
				{
					if (oMorphoFeature.m_NumberType.Equal(Core::Features::Values::NumberType.ntSingle))
					{
						if (!number.Equal(Core::Features::Values::NumberType.ntSingle))
						{
							gender.Undefine();
						}
						number.Assign(Core::Features::Values::NumberType.ntSingle);
						/// отдаетс€ приоритет мужскому роду
						if (oMorphoFeature.m_GenderType.Equal(Core::Features::Values::GenderType.gtMasculine))
						{
							return pwi->m_wWord.c_str();
						}
						else if (oMorphoFeature.m_GenderType.Equal(Core::Features::Values::GenderType.gtFemale))
						/// женский род
						{
							normal = pwi->m_wWord.c_str();
							gender.Assign(oMorphoFeature.m_GenderType);
						}
						else if (!gender.Equal(Core::Features::Values::GenderType.gtFemale))
						{
							normal = pwi->m_wWord.c_str();
						}
					}
					else if (!number.Equal(Core::Features::Values::NumberType.ntSingle))
					{
						number.Assign(oMorphoFeature.m_NumberType);
						if (oMorphoFeature.m_GenderType.Equal(Core::Features::Values::GenderType.gtMasculine))
						{
							normal = pwi->m_wWord.c_str();
							gender.Assign(oMorphoFeature.m_GenderType);
						}
						else if (!gender.Equal(Core::Features::Values::GenderType.gtMasculine) &&
							     oMorphoFeature.m_GenderType.Equal(Core::Features::Values::GenderType.gtFemale))
						/// женский род
						{
							normal = pwi->m_wWord.c_str();
							gender.Assign(oMorphoFeature.m_GenderType);
						}
						else if (!gender.Equal(Core::Features::Values::GenderType.gtMasculine) &&
							     !gender.Equal(Core::Features::Values::GenderType.gtFemale))
						{
							normal = pwi->m_wWord.c_str();
						}
					}
				}
			}
		}
	}
	return (normal);
}

String^ SS::LinguisticProcessor::CIndexation::GetNormalWordForm(String^ word)
{
    if (!word->Equals("") && (word->Length < SS::Constants::MAX_CHARS_IN_WORD))
    {
        wstring currentWord = TOwstring(word);
        list<SS::Dictionary::Types::TWordInfo> info;
		m_pMorphoAnalyzer->GetWordInfo(currentWord.c_str(), &info, SS::Dictionary::Types::efzOnlySearch);
		if (info.size() > 0)
		{
			wstring normal;
			int index = 0;
			bool *pPropers = new bool[ info.size() ];
			memset(pPropers, false, info.size() * sizeof(bool));
			
			for (std::list<SS::Dictionary::Types::TWordInfo>::iterator it = info.begin();
				 it != info.end();
				 ++it, ++index
				)
			{
				SS::Core::Features::CMorphoFeature oMorphoFeature;
				SS::Dictionary::Types::SWordInfo* pwi = &(it->front());
				if ((pwi->m_WordIndex.m_DictIndex.GetDictionaryNumber() == SS::Dictionary::DATA_TYPE::NAMES::ednEProperName) ||
					(pwi->m_WordIndex.m_DictIndex.GetDictionaryNumber() == SS::Dictionary::DATA_TYPE::NAMES::ednRProperName))
				{
					pPropers[index] = true;
					normal = GetNormalForm(*it);
					if (!normal.empty())
					{
						delete [] pPropers;
						return (gcnew String(normal.c_str()));
					}
				}
			}
			
			index = 0;
			for (std::list<SS::Dictionary::Types::TWordInfo>::iterator it = info.begin();
				 it != info.end();
				 ++it, ++index
				)
			{
				if (pPropers[index])
				{
					continue;
				}
				normal = GetNormalForm(*it);
				if (!normal.empty())
				{
					delete [] pPropers;
					return (gcnew String(normal.c_str()));
				}
			}

			delete [] pPropers;
		}
    }
    return (word);
}

Generic::List<String^>^ SS::LinguisticProcessor::CIndexation::GetAllNormalWordForm(String^ word)
{
	Generic::List<String^>^ normalForms = gcnew Generic::List<String^>();
	if (!word->Equals("") && (word->Length < SS::Constants::MAX_CHARS_IN_WORD))
	{
		List<String^>^ wordForms = GetAllWordForms(word, SS::Core::Features::Values::PartOfSpeechTypes.Undefined);
		HashSet<String^>^ uniqueForms = gcnew HashSet<String^>();
		wstring normal;
		for (int i = 0; i < wordForms->Count; ++i)
		{
			wstring currentWord = TOwstring(wordForms[i]);
			list<SS::Dictionary::Types::TWordInfo> info;
			m_pMorphoAnalyzer->GetWordInfo(currentWord .c_str(), &info, SS::Dictionary::Types::efzOnlySearch);
			bool isAny = false;
			for (std::list<SS::Dictionary::Types::TWordInfo>::iterator it = info.begin(); it != info.end(); ++it)
			{
				normal = GetNormalForm(*it);
				if (!normal.empty())
				{
					AddLineToHashSet(uniqueForms, gcnew String(normal.c_str()) );
					isAny = true;
				}
			}
			if (!isAny)
			{
				AddLineToHashSet(uniqueForms, gcnew String(currentWord.c_str()) );
			}
		}
		normalForms = Utils::Converter::ToList(uniqueForms);
	}
	return (normalForms);
}

String^ SS::LinguisticProcessor::CIndexation::GetAllWordFormsXML(String^ word, SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech)
{
    XmlDocument^ xmlDoc = gcnew XmlDocument();
    XmlDeclaration^ xmlDeclaration = xmlDoc->CreateXmlDeclaration("1.0", "utf-8", "");

    XmlElement^ rootNode  = xmlDoc->CreateElement("WORD");
    rootNode->SetAttribute("VALUE", SemanticResolution::ServiceFunction::ReplaceWrongSymbols(word));
    xmlDoc->InsertBefore(xmlDeclaration, xmlDoc->DocumentElement); 
    xmlDoc->AppendChild(rootNode);
    if (!String::IsNullOrEmpty(word) && (word->Length < SS::Constants::MAX_CHARS_IN_WORD))
    {
        wstring currentWord = TOwstring(word);
        list<SS::Dictionary::Types::TWordInfo> info;
        m_pMorphoAnalyzer->GetWordInfo(currentWord.c_str(), &info, SS::Dictionary::Types::efzFullAddNewWords);
        std::list<SS::Dictionary::Types::TWordInfo> infoDetect;
        for (std::list<SS::Dictionary::Types::TWordInfo>::iterator it = info.begin();
             it != info.end();
             ++it
			)
        {
            SS::Core::Features::CMorphoFeature oMorphoFeature;
            SS::Dictionary::Types::SWordInfo* pwi1 = &(it->front());
			/*if (pwi1->m_NewWord)
				continue;*/

			if (!partOfSpeech.IsUndefined())
			{
				m_pAMConverter->EnCode(
					pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
					pwi1->m_MorphoTypeInfo.m_MorphoInfo,
					pwi1->m_SpecialMorpho,
					&oMorphoFeature);

				if (!oMorphoFeature.m_PartOfSpeechTypes.IsIntersectsWith(&partOfSpeech))
				{
					continue;
				}
			}

            infoDetect.clear();
            if (m_pMorphoAnalyzer->GetWordFormsByIDSource(&(it->front().m_WordIndex.m_DictIndex), &(infoDetect)))
            {
                for (std::list<SS::Dictionary::Types::TWordInfo>::iterator itWord = infoDetect.begin();
                     itWord != infoDetect.end();
                     ++itWord
					)
                {
                    pwi1 = &(itWord->front());
					/*if (pwi1->m_NewWord)
						continue;*/

                    m_pAMConverter->EnCode(
                        pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
                        pwi1->m_MorphoTypeInfo.m_MorphoInfo,
                        pwi1->m_SpecialMorpho,
                        &oMorphoFeature);

                    XmlElement^ node = xmlDoc->CreateElement("FORM");
					if (oMorphoFeature.m_PartOfSpeechTypes.Equal(SS::Core::Features::Values::PartOfSpeechTypes.postOfficial))
					{
						node->SetAttribute("PartOfSpeech", gcnew String(oMorphoFeature.m_OfficialType.GetValueDescription().c_str()));
					}
					else
					{
						node->SetAttribute("PartOfSpeech", gcnew String(oMorphoFeature.m_PartOfSpeechTypes.GetValueDescription().c_str()));
					}
                    node->SetAttribute("Gender", gcnew String(oMorphoFeature.m_GenderType.GetValueDescription().c_str()));
                    node->SetAttribute("Number", gcnew String(oMorphoFeature.m_NumberType.GetValueDescription().c_str()));
                    node->SetAttribute("Case", gcnew String(oMorphoFeature.m_CaseType.GetValueDescription().c_str()));
                    node->AppendChild(xmlDoc->CreateTextNode( gcnew String((itWord->front()).m_wWord.c_str())) );
                    rootNode->AppendChild(node);                
                }
            }
        }
    }
    StringWriter^ sw = gcnew StringWriter();
    XmlTextWriter^ tx = gcnew XmlTextWriter(sw);
    xmlDoc->WriteTo(tx);
    return (sw->ToString());
}

Generic::List<String^>^ SS::LinguisticProcessor::CIndexation::GetAllWordForms(String^ word, SS::Core::Features::Types::PartOfSpeechTypes partOfSpeech)
{
	Generic::HashSet<String^>^ forms = gcnew Generic::HashSet<String^>();
	String^ xmlText = GetAllWordFormsXML(word, partOfSpeech);
	xmlText = SemanticResolution::ServiceFunction::CorrectXML(xmlText, "");
	XElement^ xml = XElement::Parse(xmlText);
	Generic::List<XElement^>^ elements = Utils::Converter::ToList(xml->Elements());
	AddLineToHashSet(forms, word);
	for (int i = 0; i < elements->Count; ++i)
	{
		AddLineToHashSet(forms, elements[i]->Value);
	}
	return Utils::Converter::ToList(forms);
}

Tuple<Generic::List<String^>^, Generic::List<String^>^>^
	SS::LinguisticProcessor::CIndexation::GetAllWordFormsWithPartsOfSpeech(String^ word)
{
	HashSet<String^>^ forms = gcnew HashSet<String^>();
	HashSet<String^>^ partsOfSpeech = gcnew HashSet<String^>();

	String^ xmlText = GetAllWordFormsXML(word, SS::Core::Features::Values::PartOfSpeechTypes.Undefined);
	xmlText = SemanticResolution::ServiceFunction::CorrectXML(xmlText, "");
	XElement^ xml = XElement::Parse(xmlText);
	Generic::List<XElement^>^ elements = Utils::Converter::ToList(xml->Elements());
	AddLineToHashSet(forms, word);
	for (int i = 0; i < elements->Count; ++i)
	{
		XElement^ element = elements[i];
		AddLineToHashSet(forms, element->Value);
		AddLineToHashSet(partsOfSpeech, element->Attribute(Utils::Converter::ToXName("PartOfSpeech"))->Value);
	}
	
	return (gcnew Tuple<List<String^>^, List<String^>^>(Utils::Converter::ToList(forms), Utils::Converter::ToList(partsOfSpeech)));
}

void SS::LinguisticProcessor::CIndexation::AddLineToHashSet(Generic::HashSet<String^>^ hash, String^ line)
{
	line = SemanticResolution::ServiceFunction::ReplaceWrongSymbols(line)->Trim()->ToLower();
	hash->Add(line);
}

Generic::List<String^>^ SS::LinguisticProcessor::CIndexation::GetTokens(String^ srcText)
{
	using namespace SS::Interface::Core::BlackBox;

	Generic::List<String^>^ list = gcnew Generic::List<String^>();
	vector<vector<wstring>> paragraphList = ConvertParagraphsArray(ParseTextForSentences(srcText));
	for (vector<vector<wstring>>::iterator it = paragraphList.begin(); it != paragraphList.end(); ++it)
	{
		vector<wstring> paragraph = *it;
		for (vector<wstring>::iterator itSent = paragraph.begin(); itSent != paragraph.end(); ++itSent)
		{
			SS::Core::Types::IndexationSearch::TIndexationParams oIndexationParams;
			SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
			wstring text = *itSent;
			oNativeText.SetText(text.c_str());
			Init( &oNativeText, &oIndexationParams, false);
			m_pText->ClearText();
			if (!m_pLexica->LoadNext())
			{
				return nullptr;
			}
 			m_pLexica->AnalyzeNext(m_pText, 0, true, 2);

			for (IParagraph *pPar = m_pText->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
			{
				for (ISentence *pSen = pPar->GetFirstSentence(); pSen; pSen = pSen->GetNextSentence())
				{
					TUnitList sentUnits;
					pSen->GetLinearUnitsFull(sentUnits);

					for (TUnitList::iterator itU = sentUnits.begin(); itU != sentUnits.end(); ++itU)
					{
						wstring token = L"";
						IUnit *pUnit = *itU;
						if (pUnit->GetParentUnit())
						{
							continue;
						}
						token = pUnit->GetWord();
						if (token.empty())
						{
							TUnitList childList = pUnit->GetChildList();
							for (TUnitList::iterator child = childList.begin(); child != childList.end(); ++child)
							{
								pUnit = *child;
								wstring word = pUnit->GetWord();
								token += word + L" ";
							}
						}
						list->Add( gcnew String(token.c_str()) );
					}
				}
			}
		}
	}
	return (list);
}

String^ SS::LinguisticProcessor::CIndexation::GetMorphoInfo(String^ word)
{
	using namespace SemanticResolution;

	XElement^ xml = gcnew XElement(Utils::Converter::ToXName("word"));
	xml->SetAttributeValue(Utils::Converter::ToXName("value"), word);
	
	wstring currentWord = TOwstring(word);
    list<SS::Dictionary::Types::TWordInfo> info;
    m_pMorphoAnalyzer->GetWordInfo(currentWord.c_str(), &info, SS::Dictionary::Types::efzOnlySearch);
    for (std::list<SS::Dictionary::Types::TWordInfo>::iterator it = info.begin(); it != info.end(); ++it)
	{
		SS::Core::Features::CMorphoFeature oMorphoFeature;
		SS::Dictionary::Types::SWordInfo* pwi1 = &(it->front());

        m_pAMConverter->EnCode(
			pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
			pwi1->m_MorphoTypeInfo.m_MorphoInfo,
			pwi1->m_SpecialMorpho,
			&oMorphoFeature);
		XElement^ form = gcnew XElement(Utils::Converter::ToXName("form"));
		form->SetAttributeValue(Utils::Converter::ToXName("partOfSpeech"),
			gcnew String(oMorphoFeature.m_PartOfSpeechTypes.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("officialType"),
			gcnew String(oMorphoFeature.m_OfficialType.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("verbType"),
			gcnew String(oMorphoFeature.m_VerbForm.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("numericType"),
			gcnew String(oMorphoFeature.m_NumericType.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("pronounClass"),
			gcnew String(oMorphoFeature.m_PronounClass.GetValueDescription().c_str()));

		form->SetAttributeValue(Utils::Converter::ToXName("animate"),
			gcnew String(oMorphoFeature.m_AnimInAnimType.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("aspect"),
			gcnew String(oMorphoFeature.m_AspectType.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("case"),
			gcnew String(oMorphoFeature.m_CaseType.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("comp"),
			gcnew String(oMorphoFeature.m_DegreeType.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("gender"),
			gcnew String(oMorphoFeature.m_GenderType.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("mood"),
			gcnew String(oMorphoFeature.m_MoodType.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("number"),
			gcnew String(oMorphoFeature.m_NumberType.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("person"),
			gcnew String(oMorphoFeature.m_PersonType.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("short"),
			gcnew String(oMorphoFeature.m_ShortType.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("tense"),
			gcnew String(oMorphoFeature.m_TimeType.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("transit"),
			gcnew String(oMorphoFeature.m_TransitionalType.GetValueDescription().c_str()));
		form->SetAttributeValue(Utils::Converter::ToXName("voice"),
			gcnew String(oMorphoFeature.m_VoiceType.GetValueDescription().c_str()));
		xml->Add(form);
	}
	return (xml->ToString());
}

Generic::List<TextParsing::Paragraph^>^ SS::LinguisticProcessor::CIndexation::ParseTextForSentences(String^ text)
{
	return (m_pTextParser->ParseText(text, false));
}

vector< vector< wstring > > SS::LinguisticProcessor::CIndexation::ConvertParagraphsArray(Generic::IList<TextParsing::Paragraph^>^ paragraphList)
{
	vector< vector< wstring > > result;
	for (int i = 0, paragraph_len = paragraphList->Count; i < paragraph_len; i++)
	{
		vector< wstring > paragraphSentences;
		TextParsing::Paragraph^ paragraph = paragraphList[ i ];
		for (int j = 0, sentences_len = paragraph->Sentences->Length; j < sentences_len; j++)
		{
			TextParsing::Sentence^ sentence = (TextParsing::Sentence^) paragraph->Sentences[ j ];

			if (sentence->IsProcessed)
			{
				String^ sentenceText = sentence->Text->TrimEnd(0);
				wstring sentence = TOwstring(sentenceText);
				paragraphSentences.push_back(sentence);
			}
		}
		result.push_back(paragraphSentences);
	}
	return (result);
}

void SS::LinguisticProcessor::CIndexation::SelectEntitiesOnContent(Linguistics::Core::Content^ content, String^ baseDate)
{
	SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
	SS::Core::Types::IndexationSearch::TIndexationParams oIndexationParams;

	List<Linguistics::Core::Entity^>^ slogans = SloganAnalyzing(content);
	List<Linguistics::Core::Entity^>^ result = GetSimpleEntities(content);
	content->ClearEntities();
	//---WTF-?!?!?!---// content->SetSentenceProcessedStatus();

	Generic::IList<TextParsing::Paragraph^>^ paragraphList = Utils::Converter::ToList(content->GetParagraphs());
	vector<vector<wstring>> paragraphs = ConvertParagraphsArray(paragraphList);
	wstring text = GetPretreatmentContentText(content, paragraphs, paragraphList);
	if (!text.empty())
	{
		oNativeText.SetText(text.c_str());
		Init(&oNativeText, &oIndexationParams, false);

		m_pText->ClearText();
		if (!m_pLexica->LoadNext())
		{
			return;
		}
 		m_pLexica->AnalyzeNext(m_pText, &paragraphs);

		//---TEMPORARY UNUSED---// m_pNERProcessor->Run(m_pText, text, m_pLexica);

		AddEntityListToContent(content, m_pDigestAnalyser->SelectEntities(text, m_pText, TOwstring(baseDate)));
	}
	AddEntityListToContent(content, result);
	AddEntityListToContent(content, slogans);
}

void SS::LinguisticProcessor::CIndexation::SelectSimpleEntitiesOnContent(Linguistics::Core::Content^ content, String^ baseDate)
{
	SelectSimpleEntitiesOnContent(content, baseDate, false);
}

void SS::LinguisticProcessor::CIndexation::SelectCeterusEntitiesOnContent(Linguistics::Core::Content^ content, String^ baseDate)
{
	SelectSimpleEntitiesOnContent(content, baseDate, true);
}

void SS::LinguisticProcessor::CIndexation::SelectSimpleEntitiesOnContent(Linguistics::Core::Content^ content, String^ baseDate, bool isSelectOnlyCeterus)
{
	SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
	SS::Core::Types::IndexationSearch::TIndexationParams oIndexationParams;

	Generic::IList<TextParsing::Paragraph^>^ paragraphList = Utils::Converter::ToList(content->GetParagraphs());
	vector<vector<wstring>> paragraphs = ConvertParagraphsArray(paragraphList);
	wstring text = GetPretreatmentContentText(content, paragraphs, paragraphList);
	if (!text.empty())
	{
		oNativeText.SetText(text.c_str());
		Init(&oNativeText, &oIndexationParams, false);

		m_pText->ClearText();
		if (!m_pLexica->LoadNext())
		{
			return;
		}
 		m_pLexica->AnalyzeNext(m_pText, &paragraphs, true, LinguisticsKernel::SelectEntitiesMode::Simple);

		AddEntityListToContent(content, m_pDigestAnalyser->SelectSimpleEntities(text, m_pText, isSelectOnlyCeterus));
	}
}

wstring SS::LinguisticProcessor::CIndexation::GetPretreatmentContentText(
	Linguistics::Core::Content^ content,
	vector<vector<wstring>>& paragraphs,
	Generic::IList< TextParsing::Paragraph^ >^ paragraphList)
{
	ATL::CAtlStringW result = L"";
	//---Generic::IList< TextParsing::Paragraph^ >^ paragraphList = Utils::Converter::ToList( content->GetParagraphs() );
	int position = 0;
	for (int i = 0, paragraphs_len = paragraphs.size(); i < paragraphs_len; i++)
	{
		TextParsing::Paragraph^ paragraph = paragraphList[ i ];
		for (int j = 0, sentences_len = paragraphs[i].size(); j < sentences_len; j++)
		{			
			TextParsing::Sentence^ sentence = (TextParsing::Sentence^) paragraph->Sentences[ j ];
			int fullSentenceStartPosition = paragraph->StartPosition + sentence->StartPosition;
			if (sentence->IsProcessed)
			{
				/*int length = fullSentenceStartPosition - position;
				if ( length < 0 )
				{
					return (result);
				}
				result += TOwstring(content->TextValue->Substring(position, length)).c_str();
				*/
				int length = Math::Max(0, fullSentenceStartPosition - position);
				result += TOwstring(content->TextValue->Substring(position, length)).c_str();
				position = fullSentenceStartPosition;
			
				wstring& s = paragraphs[i][j];
				TextPretreatment_Letters(s, LinguisticsKernel::SelectEntitiesMode::Full);
				result   += s.c_str();
				position += s.size();
			}
			else
			{
				position  = fullSentenceStartPosition;
				position += sentence->Text->Length;
			}
		}
	}
	return (result);
}

Generic::List<Linguistics::Core::Entity^>^ SS::LinguisticProcessor::CIndexation::GetSimpleEntities(Linguistics::Core::Content^ content)
{
	if (!content->IsPotencialParentPart)
	{
		array< String^ >^ pSentences = content->GetSentencesWithChildrenParts(true);
		List<Linguistics::Core::Entity^>^ result = GetSimpleEntities(pSentences);
		content->SetEntitiesForContentAndChildren(result, true);
	}
	return Utils::Converter::ToList(content->Entities);
}

Generic::List<Linguistics::Core::Entity^>^ SS::LinguisticProcessor::CIndexation::GetSimpleEntities( array< String^ >^ texts )
{
	Generic::List<Linguistics::Core::Entity^>^ result = gcnew Generic::List<Linguistics::Core::Entity^>( Math::Max( 1, texts->Length ) );
	
	int textStartPosition = 0;
	for (int i = 0; i < texts->Length; ++i)
	{
		result->AddRange(GetSimpleEntities(texts[i], textStartPosition));
		textStartPosition += texts[i]->Length;
	}

	return (result);
}

Generic::List<Linguistics::Core::Entity^>^ SS::LinguisticProcessor::CIndexation::GetSimpleEntities(String^ text, int textStartPosition)
{
	return (GetSimpleEntities(m_pSimpleEntitySelector->SelectSimpleEntities(text), textStartPosition));
}

Generic::List<Linguistics::Core::Entity^>^ SS::LinguisticProcessor::CIndexation::GetSimpleEntities(array< SimpleEntities::SimpleEntity^ >^ entities, int textStartPosition)
{
	Generic::List<Linguistics::Core::Entity^>^ result = gcnew Generic::List<Linguistics::Core::Entity^>( Math::Max( 1, entities->Length ) );
	for (int i = 0; i < entities->Length; ++i)
	{
		Linguistics::Core::Entity^ entity = CreateSimpleEntity(entities[i]);
		entity->MovePosition(-textStartPosition);
		result->Add(entity);
	}
	return (result);
}

Linguistics::Core::Entity^ SS::LinguisticProcessor::CIndexation::CreateSimpleEntity(SimpleEntities::SimpleEntity^ simpleEntity)
{
	using namespace Linguistics::Core;

	AdditionalEntityInfo^ entityInfo = gcnew AdditionalEntityInfo(simpleEntity->Type);
	entityInfo->Value = simpleEntity->Value;
	Entity^ result = gcnew Entity(simpleEntity->Text, simpleEntity->StartPosition, entityInfo);
	for (int i = simpleEntity->Children->Count - 1; i >= 0; --i)
	{
		result->AddChild(CreateSimpleEntity(simpleEntity->Children[i]));
	}
	return (result);
}

SS::Interface::Core::EntitiesCollection SS::LinguisticProcessor::CIndexation::ConvertEntityArray(Generic::List<Linguistics::Core::Entity^>^ entityList)
{
	SS::Interface::Core::EntitiesCollection entities;
	for (int i = 0; i < entityList->Count; ++i)
	{
		Linguistics::Core::Entity^ currentEntity = entityList[i];
		if (currentEntity->IsUse)
		{
			SS::Core::Types::CTextEntity entity;
			entity.set_Value(TOwstring(currentEntity->Value));
			entity.set_EntityType(TOwstring(currentEntity->Type->GetFullType()));
			entity.set_StartPosition(currentEntity->PositionInfo->Start);
			entity.set_EndPosition(currentEntity->PositionInfo->End);
			entities.push_back(entity);
		}
	}
	return (entities);
}

Generic::List<String^>^ SS::LinguisticProcessor::CIndexation::ConvertToArray(vector<wstring>& collection)
{
	Generic::List<String^>^ result = gcnew Generic::List<String^>( Math::Max( 1U, collection.size() ) );
	for (vector<wstring>::iterator it = collection.begin(); it != collection.end(); ++it)
	{
		result->Add( gcnew String((*it).c_str()) );
	}
	return (result);
}

Generic::List<Linguistics::Core::Entity^>^ SS::LinguisticProcessor::CIndexation::ConvertToArray(Linguistics::Core::Content^ content, vector<SS::Core::Types::CTextEntity>& collection, int shift)
{
	Generic::List<Linguistics::Core::Entity^>^ result = gcnew Generic::List<Linguistics::Core::Entity^>( Math::Max( 1U, collection.size() ) );
	for (vector<SS::Core::Types::CTextEntity>::iterator it = collection.begin(); it != collection.end(); ++it)
	{
		result->Add(GetEntity(content, *it, shift));
	}
	return result;
}

Linguistics::Core::Entity^ SS::LinguisticProcessor::CIndexation::GetEntity(Linguistics::Core::Content^ content, SS::Core::Types::CTextEntity& sourceEntity, int shift)
{
	Linguistics::Core::TextEntity^ textEntity = gcnew Linguistics::Core::TextEntity();
	textEntity->Value = gcnew String( sourceEntity.get_Value().c_str() );
	textEntity->Type = gcnew String( sourceEntity.get_EntityType().c_str() );
	textEntity->Start = sourceEntity.get_StartPosition() + shift;
	textEntity->End = sourceEntity.get_EndPosition() + shift;
	textEntity->Text = content->TextValue->Substring(textEntity->Start, textEntity->End - textEntity->Start);
	textEntity->MainNoun = gcnew String( sourceEntity.get_MainNoun().c_str() );
	textEntity->MainNounPrefix = gcnew String( sourceEntity.get_MainNounPrefix().c_str() );
	textEntity->FirstName = gcnew String( sourceEntity.get_FirstName().c_str() );
	textEntity->MiddleName = gcnew String( sourceEntity.get_MiddleName().c_str() );
	textEntity->LastName = gcnew String( sourceEntity.get_LastName().c_str() );
	textEntity->Name = gcnew String( sourceEntity.get_Name().c_str() );
	textEntity->Post = gcnew String( sourceEntity.get_Post().c_str() );
	textEntity->Language = gcnew String( sourceEntity.get_Language().c_str() );
	textEntity->IsPseudoPlace = sourceEntity.get_IsPseudoPlace();
	textEntity->LinkedPlaces = gcnew String( sourceEntity.get_LinkedPlaces().c_str() );
	textEntity->MorphoInfo = ConvertToArray(sourceEntity.get_MorphoInformation());
	textEntity->Adjectives = ConvertToArray(sourceEntity.get_Adjectives());
	textEntity->SyntaxChildren = ConvertToArray(content, sourceEntity.get_SubEntities(), shift);
	return (textEntity->CreateEntity());
}

void SS::LinguisticProcessor::CIndexation::AddEntityListToContent(Linguistics::Core::Content^ content, SS::Interface::Core::EntitiesCollection& entityList, int shift)
{
	int paragraphIndex = 0;
	int sentenceIndex = 0;
	Generic::IList<TextParsing::Paragraph^>^ paragraphList = Utils::Converter::ToList(content->GetParagraphs());
	TextParsing::Paragraph^ paragraph = paragraphList[0];
	TextParsing::Sentence^ nextSentence = (TextParsing::Sentence^) paragraph->Sentences[0];
	int nextSentenceStartPosition = 0;
	int notProcessedSentenceShift = 0;
	for (vector<SS::Core::Types::CTextEntity>::reverse_iterator it = entityList.rbegin(); it != entityList.rend(); ++it)
	{
		Linguistics::Core::Entity^ entity = GetEntity(content, *it, shift + notProcessedSentenceShift);
		if (entity->PositionInfo->Start >= nextSentenceStartPosition)
		/// сущность находитс€ в текущем предложении
		{
			if (!nextSentence->IsProcessed)
			{
				int length = nextSentence->Text->Length;
				notProcessedSentenceShift += length;
				entity->MovePosition(-length);
			}
			++sentenceIndex;
			if (sentenceIndex >= paragraph->Sentences->Length)
			{
				++paragraphIndex;
				sentenceIndex = 0;
				if (paragraphIndex < paragraphList->Count)
				{
					paragraph = paragraphList[paragraphIndex];
				}
				else
				{
					nextSentenceStartPosition = paragraph->StartPosition + nextSentence->EndPosition;
					paragraph = nullptr;
				}
			}
			if (paragraph)
			{
				nextSentence = (TextParsing::Sentence^) paragraph->Sentences[sentenceIndex];
				nextSentenceStartPosition = paragraph->StartPosition + nextSentence->StartPosition;
			}
			else
			{
				nextSentence = nullptr;
			}
		}
		content->AddEntity(entity, false);
	}
}

void SS::LinguisticProcessor::CIndexation::AddEntityListToContent(Linguistics::Core::Content^ content, SS::Interface::Core::EntitiesCollection& entityList)
{
	AddEntityListToContent(content, entityList, 0);
}

void SS::LinguisticProcessor::CIndexation::AddEntityListToContent(Linguistics::Core::Content^ content, Generic::List<Linguistics::Core::Entity^>^ entityList, int shift)
{
	for (int i = entityList->Count - 1; i >= 0; --i)
	{
		Linguistics::Core::Entity^ entity = entityList[i];
		if (entity->IsUse)
		{
			entity->MovePosition(-shift);
			content->AddEntity(entity, true);
		}
	}
}

void SS::LinguisticProcessor::CIndexation::AddEntityListToContent(Linguistics::Core::Content^ content, Generic::List<Linguistics::Core::Entity^>^ entityList)
{
	AddEntityListToContent(content, entityList, 0);
}
