#include "StdAfx.h"
#include "../../cpp/[libs]/_Paths.h"

#using <mscorlib.dll>
#include <vcclr.h>
//#include <_vcclrit.h>

#include "..\..\cpp\ASCInterface\ILinguisticProcessor.h"
#include "..\..\cpp\ASSInterface\ILinguisticProcessorEx.h"
#include "..\..\cpp\ASCInterface\TCoommonTypes.h"
#include "..\..\cpp\ASSInterface\IQueryResultEx.h"
#include "..\..\cpp\ASSInterface\ITextBlockEx.h"

#include "..\..\cpp\ASSInterface\ICommonContainersEx.h"
#include "..\..\cpp\ASSInterface\IAMConverter.h"
#include "..\..\cpp\ASSInterface\ICoSeDi.h"

#include "..\..\cpp\ASSInterface\ILexicaLib.h"
#include "..\..\cpp\ASSInterface\ISyntaxAnalyzer.h"

#include "..\..\cpp\ASCInterface\IBaseManager.h"

#include ".\Search.h"
#include ".\Indexation.h"
#include ".\Query.h"

#include ".\LinguisticsKernelConroller.h"

#using <System.Data.dll>

using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace SS::LinguisticProcessor;
using namespace System;
using namespace System::IO;
using namespace System::Text;
using namespace System::Xml;
using namespace System::Xml::Linq;
using namespace System::Diagnostics;
using namespace Lingvistics;
using namespace Lingvistics::Types;
using namespace SemanticResolution;

SET_LOAD_MANAGER_IMPLEMENTATION;

using namespace SS::Interface::Core;
using namespace Lingvistics;

namespace LinguisticsKernel
{
/*  описание экспортируемой функции */
typedef HRESULT (*TCreateInstance)(const GUID* pGuid, void** pBase);

/*  интерефейс получения доступа к модулям  */
SS::Interface::Core::ResourceManagers::ILoadManager * m_pILoadManager = NULL;

SS::Interface::Core::AMConverter::IAMConverterSyntaxAposteriori* m_pAMConverterSyntaxAposteriori = NULL;
SS::Interface::Core::AMConverter::IAMConverterSemantic* m_pAMConverterSemantic = NULL;

LinguisticsKernelConroller::~LinguisticsKernelConroller(void)
{
	Close();
}

void LinguisticsKernelConroller::Close()
{
}

LinguisticsKernelConroller::LinguisticsKernelConroller(void) : m_pTreeGenerator(nullptr), m_bIsInside(true)
{
	SS_TRY
	{
/// финт ушами и танец с бубном
		System::AppDomain::CurrentDomain->DomainUnload += gcnew System::EventHandler(this, &LinguisticsKernel::LinguisticsKernelConroller::DomainUnload);

/// загрузка лоад манагера
		String^ sPath =  System::Reflection::Assembly::GetExecutingAssembly()->CodeBase;
		int idx = sPath->IndexOf( L"///" );
		sPath = sPath->Substring(idx + 3, sPath->Length - idx - 3);
		sPath = System::IO::Path::GetDirectoryName(sPath);	

		String^ sModule = (gcnew System::IO::FileInfo( System::IO::Path::Combine(sPath, gcnew String(LOADMANAGER_DLL_PATH)) ))->FullName;
			
		const wchar_t* module = (const wchar_t*)(Marshal::StringToHGlobalUni(sModule)).ToPointer();
		const wchar_t* path   = (const wchar_t*)(Marshal::StringToHGlobalUni(sPath  )).ToPointer();

		HMODULE hLoadManager = ::LoadLibrary(module);

		TCreateInstance pfnCreateInstance = (TCreateInstance) GetProcAddress(hLoadManager, "CreateInstance");
		if (!pfnCreateInstance)
		{
			throw (gcnew System::Exception(gcnew String(L"!pfnCreateInstance")));
		}

		const GUID Guid = CLSID_LoadManager;
		(*pfnCreateInstance)(&Guid, (void**) &m_pILoadManager);

		SetLoadManager(m_pILoadManager);

		m_pILoadManager->SetWorkPath(path);
/// конец загрузки лоад манагера

		Init();

        Marshal::FreeHGlobal(System::IntPtr((void*)module));
        Marshal::FreeHGlobal(System::IntPtr((void*)path));
		/// запустим выделение сущностей, чтобы проинициализировались морфологические словари
		GetXmlEntities("слово");
	}
	SS_CATCH(L"")
}

void LinguisticsKernelConroller::Init()
{
	SS_TRY
	{
		//::CoInitializeEx(NULL, COINIT_MULTITHREADED);
		//::CoInitialize(NULL);

		m_pTreeGenerator = gcnew CTreeGenerator(m_pILoadManager);

		/// загрузка остальных модулей и инициализация интерфейсов
		m_pAMConverterMorpho = 
			(SS::Interface::Core::AMConverter::IAMConverterMorpho*)
			m_pILoadManager->GetInterface( L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);

		m_pDictionary = 
			(SS::Interface::Core::Dictionary::IDictionary*)
			m_pILoadManager->GetInterface( DICTIONARIES_DLL_PATH, CLSID_IDictionary, IID_IDictionary);  
		if (!m_pDictionary->GetLoader()->Load())
		{
			SS_THROW(L"");
		}

		m_pAMConverterSyntax  = 
			(SS::Interface::Core::AMConverter::IAMConverterSyntax*)
			m_pILoadManager->GetInterface( L"./core/linguistics/AMConverter.dll", CLSID_AMConverterSyntax, IID_AMConverterSyntax);

		m_pAMConverterSyntaxAposteriori = 
			(SS::Interface::Core::AMConverter::IAMConverterSyntaxAposteriori*)
			m_pILoadManager->GetInterface( L"./core/linguistics/AMConverter.dll", CLSID_AMConverterSyntax, IID_AMConverterSyntaxAposteriori);

		m_pAMConverterSemantic = 
			(SS::Interface::Core::AMConverter::IAMConverterSemantic*)
			m_pILoadManager->GetInterface( L"./core/linguistics/AMConverter.dll", CLSID_AMConverterSemantic, IID_AMConverterSemantic);

		m_pIndexBaseInit = new CIndexBaseInit();
		m_pIndexBaseInit->SetLoadManager(m_pILoadManager);
		m_pIndexBaseInit->BeginSession();

		m_pLinguisticProcessor = 
			(SS::Interface::Core::ILinguisticProcessor*)
			m_pILoadManager->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_LinguisticProcessor, IID_LinguisticProcessor);

		if (!m_pIWorkRegister)
		{
			m_pIWorkRegister = (SS::Interface::Core::CommonServices::IWorkRegister*)
				m_pILoadManager->GetInterface( LOADMANAGER_DLL_PATH, CLSID_WorkRegister, IID_WorkRegister );

			if (!m_pIWorkRegister) 
			{
				SS_THROW(L"Ошибка при получении WorkRegister");
			}
		}

		SS::Core::Types::IndexationSearch::TLinguisticProcessorMode oMode;
		oMode.GetAnalyseDepthParams()->SetIsMorpho(true);
		oMode.GetAnalyseDepthParams()->SetIsSynonims(true);
		oMode.GetAnalyseDepthParams()->SetIsSemantic(true);
		m_pLinguisticProcessor->SetMode(&oMode);
		m_pLinguisticIndexation = m_pLinguisticProcessor->GetLinguisticIndexation();
  
		m_pQueryParams = new SS::Core::Types::IndexationSearch::TQueryParams();
		Utils::GlobalVariables::UserResourcesPath = gcnew String(m_pIWorkRegister->GetParameter(L"PathUserResources").c_str());
		m_pIndexation = gcnew SS::LinguisticProcessor::CIndexation(Utils::GlobalVariables::UserResourcesPath);
		m_pIndexation->logFile = logFile;
		m_pIndexation->SetLoadManager(m_pILoadManager);
		m_pIndexation->SetMode(&oMode);

		Linguistics::Core::TextContent::Parser = m_pIndexation->get_TextParser();
		m_pCoreferenceResolution = gcnew SemanticResolution::CoreferenceResolution();
		m_pSurfaceSyntaxAnalyzer = gcnew SurfaceSyntaxAnalyzer::SurfaceSyntaxAnalyzer();
		m_pAbbreviationResolver = gcnew CoreferenceResolving::AbbreviationResolver();
		m_pSemanticNetwork = gcnew SemanticResolution::SemanticNetwork();
		SetConfigurationParams();
		LoadDictionaries();
 
		m_pTreeComparator = gcnew CTreeComparator();
	}
	SS_CATCH(L"")
}

void LinguisticsKernelConroller::SetConfigurationParams()
{
	using namespace System::Configuration;
	using namespace System::Collections;

	Hashtable^ section = dynamic_cast<Hashtable^>(ConfigurationManager::GetSection("Lingvistics"));

	int themeLength = Int32::Parse(section[ gcnew String("MaxEntityLength") ]->ToString());
	m_pIndexation->SetEntityMaxLength(themeLength);

	_IsUseGeoNamesDict = (L"True" == section[ gcnew String("UseGeoNamesDictionary") ]->ToString());
}

void LinguisticsKernelConroller::LoadDictionaries()
{
    wstring path = m_pIWorkRegister->GetParameter(L"PathUserResources");
    vector<vector<wstring>> dictionaries;
    LoadDictionary(path + L"StopDictionaries\\reductions.dct", dictionaries);
	LoadDictionary(path + L"DigestDicts\\deleted_org_prefixes.dct", dictionaries);
	LoadDictionary(path + L"PatentHomonymy\\Noun.txt", dictionaries);
	LoadDictionary(path + L"PatentHomonymy\\Adjective.txt", dictionaries);
	LoadDictionary(path + L"NounVerbs\\nounverbs_no_ie.txt", dictionaries);
	LoadDictionary(path + L"NounVerbs\\nounverbs_ie_exclusion.txt", dictionaries);
	LoadDictionary(path + L"DigestDicts\\post.txt", dictionaries);
    m_pIndexation->LoadDictionaries(dictionaries);

	LoadProfileAdjectives(path + L"StopDictionaries");
	SemanticResolution::EntityFilter::LoadStopDictionaries();
}

void LinguisticsKernelConroller::LoadDictionary(wstring path, vector<vector<wstring>> &dictionaryCollection)
{
    HashSet<String^>^ hs = Utils::Dictionary::ReadAsHashSet(gcnew String(path.c_str()), System::Text::Encoding::Unicode);
    vector<wstring> resList(hs->Count);
	int i = 0;
    for each( String^ s in hs )
	{
		resList[ i ] = m_pIndexation->TOwstring(s->ToLower());
		i++;
	}
    dictionaryCollection.push_back(resList);
}

void LinguisticsKernelConroller::LoadProfileAdjectives(wstring path)
{
	const wchar_t* dictionaryName = L"adjectives.dct";
	System::Collections::Generic::Dictionary< String^, HashSet< String^ >^ >^ dictionary =
		Utils::Dictionary::ReadProfileDictionary( gcnew String(path.c_str()), gcnew String(dictionaryName), System::Text::Encoding::Unicode);
	System::Collections::Generic::Dictionary< String^, HashSet< String^ >^ >^ dictionary_2 = 
		gcnew System::Collections::Generic::Dictionary< String^, HashSet< String^ >^ >( dictionary->Count );
	for each ( System::Collections::Generic::KeyValuePair< String^, HashSet< String^ >^ > p in dictionary )
	{
		String^ key = p.Key;
		HashSet< String^ >^ hs = p.Value;

		HashSet<String^>^ allForms = gcnew HashSet<String^>();
		for each ( String^ s in hs )
		{
			List<String^>^ forms =
				m_pIndexation->GetAllWordForms(s, SS::Core::Features::Values::PartOfSpeechTypes.postAdjective);
			for (int k = 0, forms_len = forms->Count; k < forms_len; ++k)
			{
				allForms->Add(forms[ k ]->Trim()->ToUpper());
			}
		}
		dictionary_2->Add( key, allForms );
	}
	SemanticResolution::EntityFilter::LoadStopAdjectivesDictionary( dictionary_2 );
}

void LinguisticsKernelConroller::SetParams(bool bIsMorpho, bool bIsSemantic, bool bIsSyntax, bool bIsSynonims, bool bIsInside)
{
	SS_TRY
	{
		SS::Core::Types::IndexationSearch::TLinguisticProcessorMode oMode;
		oMode.GetAnalyseDepthParams()->SetIsMorpho(bIsMorpho);
		oMode.GetAnalyseDepthParams()->SetIsSemantic(bIsSemantic);      
		oMode.GetAnalyseDepthParams()->SetIsSyntax(bIsSyntax);      
		oMode.GetAnalyseDepthParams()->SetIsSynonims(bIsSynonims);    

		if (bIsSynonims) 
		{
			m_pQueryParams->m_SearchDepth = SS::Core::Types::IndexationSearch::sdSynonyms;
		}
		else
		{
			m_pQueryParams->m_SearchDepth = SS::Core::Types::IndexationSearch::sdNormal;
		}
		m_pIndexation->SetMode(&oMode);
		m_pQueryParams->m_GenerateQueryMode = SS::Core::Types::IndexationSearch::gqmString; 
		m_pLinguisticProcessor->SetMode(&oMode);
		m_pQuery->SetMode(&oMode);
	}
	SS_CATCH(L"")
}

void LinguisticsKernelConroller::DomainUnload(Object^ sender, EventArgs^ e)
{
	//__crt_dll_terminate();
};

void LinguisticsKernelConroller::Search( String^ sQuery, String^ sText, String^ sTextTitle,
                                         DataSet^% pResultDataSet, ArrayList^% pArrayList, bool bIsGlobalSearch)
{
	SS_TRY
	{
		SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult = NULL;
		m_pILoadManager->Create(L"./core/linguistics/CommonContainers.dll", CLSID_SearchResult, IID_SearchResult, (void**)&pSearchResult);
		m_pSearch->UpdateRelevance(sQuery, sText, sTextTitle, bIsGlobalSearch, pSearchResult, m_pQueryParams);
		//заполняем датасет результатами поиска
		FillDataSet(pSearchResult, pResultDataSet, pArrayList);
	}
	SS_CATCH(L"")
}

void LinguisticsKernelConroller::FillDataSet(SS::Interface::Core::CommonContainers::ISearchResult* pSearchResult, DataSet^% pResultDataSet, ArrayList^% pArrayList)
{
	SS_TRY
	{
		using namespace System::Data;

		DataTable^ pDataTable = gcnew DataTable("View");
		pResultDataSet = gcnew DataSet();
		pResultDataSet->Tables->Add(pDataTable);
		DataColumn^ pDataColumnText = gcnew DataColumn("Text");
		DataColumn^ pDataColumnSummaryRepeats  = gcnew DataColumn("SummaryRepeats");
		pDataTable->Columns->Add(pDataColumnText);
		pDataTable->Columns->Add(pDataColumnSummaryRepeats);
    
		using namespace SS::Interface::Core::CommonContainers;

		for (ITextBlock* pTextBlock = pSearchResult->GetFirstTextBlock(); pTextBlock;)
		{
			ITextBlockCut* pTextBlockCut = pTextBlock->GetFirstBlockCut();
			while (pTextBlockCut)
			{
				IIndexAndPosition*  pIndexAndPosition = pTextBlockCut->GetFirstIndexAndPosition();
				String^ sAnswer = gcnew String("");
				sAnswer = String::Concat(sAnswer, L"<FONT style=\"FONT: 12pt arial\">");
				while (pIndexAndPosition)
				{
					if (SS::Core::Types::IndexationSearch::hltQueryWord == pIndexAndPosition->GetHighlightType())
					{
						sAnswer = String::Concat(sAnswer, L"<font color=\"FF0000\">");
					}
					else if (SS::Core::Types::IndexationSearch::hltAnswerWord == pIndexAndPosition->GetHighlightType())
					{
						// #99cc00 
						sAnswer = String::Concat(sAnswer, L"<font color=\"#99cc00\">");
					}
					else
					{
						sAnswer = String::Concat(sAnswer, L"<font color=\"000000\">");
					}
					sAnswer = String::Concat(sAnswer, gcnew String(pIndexAndPosition->GetWord()) );
					sAnswer = String::Concat(sAnswer, L" ");
					sAnswer = String::Concat(sAnswer, L"</font>");

					pIndexAndPosition  = pTextBlockCut->GetNextIndexAndPosition();
				}
        
				DataRow^ pDataRow = pDataTable->NewRow();
				pDataTable->Rows->Add(pDataRow);
				pDataRow["Text"] = sAnswer;
				pDataRow["SummaryRepeats"] = (pTextBlockCut->GetRelevance() / 1000000).ToString();
				pTextBlockCut = pTextBlock->GetNextBlockCut();
			}
			pTextBlock = pSearchResult->GetNextTextBlock();
		}
	}
	SS_CATCH(L"")
}

SS::Interface::Core::BlackBox::IText* LinguisticsKernelConroller::GenerateITextIndexation(String^ sText, bool bOutside, bool IsGlobal)
{
	SS_TRY
	{
		wstring chars = m_pIndexation->TOwstring(sText);
		SS::Core::Types::IndexationSearch::TIndexationParams oIndexationParams;

		SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
		oNativeText.SetText(chars.c_str());
		m_pIndexation->Init( &oNativeText, &oIndexationParams, IsGlobal);
		return (m_pIndexation->GenerateNext(chars, bOutside));
	}
	SS_CATCH(L"")
}
  
bool LinguisticsKernelConroller::IsExistWord(String^ sText)
{
	bool bRetVal = false;
	pin_ptr< const wchar_t > pw = PtrToStringChars(sText);
	SS::Interface::Core::ILexicaPrimary* pLexicaPrimary = m_pLinguisticShowResults->GetLexicaPrimary();

	SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;
	oNativeText.SetText(pw);

	pLexicaPrimary->SetText(&oNativeText);

	int iExist = 0;
	int iNotExist = 0;
	while (pLexicaPrimary->LoadNext())
	{
		SS::Interface::Core::ILexemeBoard* pLexemeBoard = pLexicaPrimary->GetLexemeBoard();

		if (!pLexemeBoard->MoveFirst())
		{
			::MessageBox(NULL,  pw, L"Никаких слов не получено для лексического анализа", MB_OK);
			return false;
		}     
		do
		{
			if (m_pLinguisticShowResults->IsExistWord(pLexemeBoard->GetLexeme()))
			{
				iExist++;
			}
			else
			{
				iNotExist++;
			}

			bRetVal = false;
		}
		while (pLexemeBoard->MoveNext()); 
	}

	wchar_t szToMessage[100];
	wchar_t szExist[20];
	wchar_t szNotExist[20];

	_itow_s(iExist, szExist, 20, 10);
	_itow_s(iNotExist, szNotExist, 20, 10);

	wcscat(szToMessage , L"Найдено : ");
	wcscat(szToMessage , szExist);
	wcscat(szToMessage , L" слов. ");
	wcscat(szToMessage , L"Не найдено : ");
	wcscat(szToMessage , szNotExist);
	wcscat(szToMessage , L" слов. ");

	::MessageBox(NULL,  szToMessage , L"Результат работы", MB_OK);
	return bRetVal;
}

///анализирует запрос
SS::Interface::Core::BlackBox::IText* LinguisticsKernelConroller::GenerateITextQuery(String^ sQuery, SSearchMode^ pSearchMode, bool IsGlobal)
{
	SS_TRY
	{
		pin_ptr< const wchar_t > pw = PtrToStringChars(sQuery);
		SS::Core::Types::IndexationSearch::TSearchParams oSearchParams;
    
		if (IsGlobal)
		{
			m_pQueryParams->m_GenerateQueryMode = SS::Core::Types::IndexationSearch::gqmString;
		}
		else
		{
			m_pQueryParams->m_GenerateQueryMode = SS::Core::Types::IndexationSearch::gqmIndex;
		}

		m_pQueryParams->SetQuery(pw);
		return (m_pQuery->GenerateQueryIText(&oSearchParams, m_pQueryParams));
	}
	SS_CATCH(L"")
}

///анализирует запрос
SS::Interface::Core::BlackBox::Query::IQuery* LinguisticsKernelConroller::GenerateIQueryQuery(String^ sQuery, SSearchMode^ pSearchMode, bool IsGlobal)
{
	SS_TRY
	{
		pin_ptr< const wchar_t > pw = PtrToStringChars(sQuery);		
		SS::Core::Types::IndexationSearch::TSearchParams oSearchParams;
    
		m_pQueryParams->m_GenerateQueryMode = SS::Core::Types::IndexationSearch::gqmString;
		m_pQueryParams->SetQuery(pw);
  
		return (m_pQuery->GenerateQueryIQuery(&oSearchParams, m_pQueryParams));
	}
	SS_CATCH(L"")
}

///анализирует запрос и отрисовывает деревце зависимостей
void LinguisticsKernelConroller::GenerateTreeITextQuery(String^ sQuery, TreeView^ pTree, SSearchMode^ pSearchMode, bool IsGlobal)
{
	m_pTreeGenerator->GenerateTree1(GenerateITextQuery(sQuery, pSearchMode, IsGlobal), pTree, true);
}

///анализирует запрос, формирует на его основе трансофрмации и отрисовывает деревцо трансформаций
void LinguisticsKernelConroller::GenerateTreeIQueryQuery(String^ sQuery, TreeView^ pTree, SSearchMode^ pSearchMode, bool IsGlobal)
{
	m_pTreeGenerator->GenerateTree(GenerateIQueryQuery(sQuery, pSearchMode, IsGlobal), pTree);
}

///индексирует текст и отрисовывает деревце зависимостей
void LinguisticsKernelConroller::MakeShowResult(String^ sText, TreeView^ pTree)
{ 
	IsExistWord(sText);
}

///индексирует текст и отрисовывает деревце зависимостей
void LinguisticsKernelConroller::GenerateTreeITextIndexation(String^ sText, TreeView^ pTree, bool bOutside, bool IsGlobal)
{
	m_pTreeGenerator->GenerateTree(GenerateITextIndexation(sText, bOutside, IsGlobal), pTree, false);
}

void LinguisticsKernelConroller::CompareQueries(String^ sLeftQuery, ETreeType LeftQueryTreeType, SSearchMode^ pLeftSearchMode, String^ sRightQuery, ETreeType RightQueryTreeType, SSearchMode^ pRightSearchMode, TreeView^ pComparationTree, TreeView^ pCoincidenceTree, bool bIsGlobal)
{
  if(LeftQueryTreeType==ETreeType::ettText && RightQueryTreeType==ETreeType::ettText)
  {
    m_pTreeComparator->SetLeftWords(GenerateITextQuery(sLeftQuery, pLeftSearchMode, bIsGlobal));
    m_pTreeComparator->SetRightWords(GenerateITextQuery(sRightQuery, pRightSearchMode, bIsGlobal));
  }
  else if(LeftQueryTreeType==ETreeType::ettText && RightQueryTreeType==ETreeType::ettQuery)
  {
    m_pTreeComparator->SetLeftWords(GenerateITextQuery(sLeftQuery, pLeftSearchMode, bIsGlobal));
    m_pTreeComparator->SetRightWords(GenerateIQueryQuery(sRightQuery, pRightSearchMode, bIsGlobal));
  }
  else if(LeftQueryTreeType==ETreeType::ettText && RightQueryTreeType==ETreeType::ettQueryFinal)
  {
    m_pTreeComparator->SetLeftWords(GenerateITextQuery(sLeftQuery, pLeftSearchMode, bIsGlobal));
  }
  else if(LeftQueryTreeType==ETreeType::ettText && RightQueryTreeType==ETreeType::ettTextIndexation)
  {
    m_pTreeComparator->SetLeftWords(GenerateITextQuery(sLeftQuery, pLeftSearchMode, bIsGlobal));
    m_pTreeComparator->SetRightWords(GenerateITextIndexation(sRightQuery, false, bIsGlobal));
  }
  else if(LeftQueryTreeType==ETreeType::ettQuery && RightQueryTreeType==ETreeType::ettText)
  {
    m_pTreeComparator->SetLeftWords(GenerateIQueryQuery(sLeftQuery, pLeftSearchMode, bIsGlobal));
    m_pTreeComparator->SetRightWords(GenerateITextQuery(sRightQuery, pRightSearchMode, bIsGlobal));
  }
  else if(LeftQueryTreeType==ETreeType::ettQuery && RightQueryTreeType==ETreeType::ettQuery)
  {
    m_pTreeComparator->SetLeftWords(GenerateIQueryQuery(sLeftQuery, pLeftSearchMode, bIsGlobal));
    m_pTreeComparator->SetRightWords(GenerateIQueryQuery(sRightQuery, pRightSearchMode, bIsGlobal));
  }
  else if(LeftQueryTreeType==ETreeType::ettQuery && RightQueryTreeType==ETreeType::ettQueryFinal)
  {
    m_pTreeComparator->SetLeftWords(GenerateIQueryQuery(sLeftQuery, pLeftSearchMode, bIsGlobal));
  }
  else if(LeftQueryTreeType==ETreeType::ettQuery && RightQueryTreeType==ETreeType::ettTextIndexation)
  {
    m_pTreeComparator->SetLeftWords(GenerateIQueryQuery(sLeftQuery, pLeftSearchMode, bIsGlobal));
    m_pTreeComparator->SetRightWords(GenerateITextIndexation(sRightQuery, false, bIsGlobal));
  }
  else if(LeftQueryTreeType==ETreeType::ettQueryFinal && RightQueryTreeType==ETreeType::ettText)
  {
    m_pTreeComparator->SetRightWords(GenerateITextQuery(sRightQuery, pRightSearchMode, bIsGlobal));
  }
  else if(LeftQueryTreeType==ETreeType::ettQueryFinal && RightQueryTreeType==ETreeType::ettQuery)
  {
    m_pTreeComparator->SetRightWords(GenerateIQueryQuery(sRightQuery, pRightSearchMode, bIsGlobal));
  }
  else if(LeftQueryTreeType==ETreeType::ettQueryFinal && RightQueryTreeType==ETreeType::ettTextIndexation)
  {
    m_pTreeComparator->SetRightWords(GenerateITextIndexation(sRightQuery, false, bIsGlobal));
  }
  else
  {
    return;
  }
  m_pTreeComparator->Compare(sLeftQuery, pComparationTree, pCoincidenceTree);
  pComparationTree->ExpandAll();
  pCoincidenceTree->ExpandAll();
}

void LinguisticsKernelConroller::ReinitToneDicts(String^ typeName)
{
	m_pIndexation->ReinitToneDicts(typeName);
}

void LinguisticsKernelConroller::AddWordToDict(String^ typeName, int dict_code, String^ word)
{
	m_pIndexation->AddWordToDict(typeName, dict_code, word);  
}

void LinguisticsKernelConroller::GenerateDictLing(String^ typeName)
{
	m_pIndexation->GenerateDictLing(typeName);
}

XElement^ LinguisticsKernelConroller::GetXmlEntities(String^ sText)
{
	Linguistics::Core::TextContent^ content = gcnew Linguistics::Core::TextContent(sText, "", Linguistics::Core::ChildContentSelectorMode::None);
	EntitiesSelector^ selector = gcnew EntitiesSelector(m_pIndexation, &SS::LinguisticProcessor::CIndexation::SelectSimpleEntitiesOnContent);
	SelectEntitiesOnContent(content->ParentContent, content->BaseDate, selector);
	Linguistics::Core::IText^ rdf = content->GetTextObject();
	m_pSurfaceSyntaxAnalyzer->Analyze(rdf);
	return (rdf->ToXElement());
}

String^ LinguisticsKernelConroller::GetSentXMLDigest(String^ sText, ArrayList^ offsets, ArrayList^ themeIDs, String^ dictName)
{
	return (m_pIndexation->GenerateSentXMLDigest(sText, offsets, themeIDs, dictName));
}

String^ LinguisticsKernelConroller::GetHash(String^ sEntText)
{
	return (m_pIndexation->GenerateHash(sEntText, ""));
}

String^ LinguisticsKernelConroller::GetXmlDigest(String^ sText, String^ synonims, String^ korefs, ArrayList^ offsets, String^ sObjType)
{
    SS::Core::Types::IndexationSearch::TIndexationParams oIndexationParams;
    SS::Core::Types::IndexationSearch::TNativeTextForIndexation oNativeText;

	List<int>^ offsetList = gcnew List<int>();
	for (int i = 0; i < offsets->Count; ++i)
	{
		offsetList->Add(System::Int32::Parse(offsets[i]->ToString()));
	}

	wstring text = m_pIndexation->TOwstring(sText);
    m_pIndexation->TextPretreatment_Letters(text, LinguisticsKernel::SelectEntitiesMode::Simple);
    oNativeText.SetText(text.c_str());

    m_pIndexation->Init( &oNativeText, &oIndexationParams, false);
    String^ res = m_pIndexation->GenerateDigest(text, synonims, korefs, offsetList, sObjType);
    res = SemanticResolution::ServiceFunction::CorrectXML(res, "");
    return (res);
}

/*void LinguisticsKernelConroller::EntityInitFromFile()
{
    m_pIndexation->EssenceInitFromFile(m_pIWorkRegister->GetParameter(L"PathUserResources"));
}*/

/*String^ LinguisticsKernelConroller::EntityDetectingText(String^ srcText, bool applyNormalization)
{
    String^ res = m_pIndexation->EssenceDetectingText(srcText, applyNormalization);
    return SemanticResolution::ServiceFunction::CorrectXML(res);
}*/

String^ LinguisticsKernelConroller::TransliteringText(String^ srcText)
{
    return (m_pIndexation->TransliteringText(srcText));
}

/*bool LinguisticsKernelConroller::IsContainEntity(String^ str, String^ type)
{
    return m_pIndexation->IsContainEntity(str, type);
}*/

/*String^ LinguisticsKernelConroller::GetXmlDigest(
	String^ sText,
	String^ baseDate,
	List<int>* offsets,
	List<String^>^ korefs)
{
	return new String("");
	try
	{
		int textLength = sText->Length;
		if (offsets)
		{
			for(int i = 0; i < offsets->Count; ++i)
				if((offsets->Item[i] < 0) || (offsets->Item[i] > textLength))
					throw new System::ArgumentException("offsets");
		}

		m_pIndexation->SetJobProfile(SemanticResolution::JobProfile::Tonality);
		String^ xmlText = GetXmlFacts(sText, offsets, baseDate, true, true);
		xmlText = m_pCoreferenceResolution->ResolveCoreference(xmlText, korefs);
		xmlText = SentenceSeparation(xmlText);
		xmlText = SemanticResolution::ServiceFunction::RestructuringXML(xmlText);
		return xmlText;
	}
	catch (Exception* ex)
	{
		Close();
		throw ex;
	}
}

String^ LinguisticsKernelConroller::GetXmlDigestByCorrectedText(
	String^ xmlText,
	String^ baseDate,
	List<int>* offsets,
	List<String^>^ korefs)
{
	try
	{
		String^ text = SemanticResolution::CorrectedTextParser::GetCorrectedText(xmlText);
		String^ xml = GetXmlDigest(text, baseDate, offsets, korefs);
		SemanticResolution::CorrectedTextParser::AddCorrectedInformation(&xml);
		return xml;
	}
	catch (Exception* ex)
	{
		Close();
		throw ex;
	}
}*/

String^ LinguisticsKernelConroller::GetTextAllNormalForms(String^ text)
{
    return (m_pIndexation->GetTextAllNormalForms(text));
}

/*String^ LinguisticsKernelConroller::FinishCoreference(String^ xmlText)
{
    XmlDocument* document = new XmlDocument();
    document->PreserveWhitespace = true;
    StringReader* reader = new StringReader(xmlText);
    document->Load(reader);

    System::Collections::Generic::Dictionary<String^, XmlNode*>* entities = 
        new System::Collections::Generic::Dictionary<String^, XmlNode*>();
    FillEntitiesDictionary(entities, document->FirstChild);
    FormingNewValue(entities);

    StringWriter* writer = new System::IO::StringWriter();
    document->Save(writer);

    return writer->ToString();
}

void LinguisticsKernelConroller::FillEntitiesDictionary(
    System::Collections::Generic::Dictionary<String^, XmlNode*>* dictionary, 
    XmlNode* xml)
{
    if(xml->Name->Equals("THEMES"))
        return;

    if(xml->Attributes)
    {
        XmlAttribute* attr = xml->Attributes->get_ItemOf("ID");
        if(attr)
            dictionary->Add(attr->Value, xml);
        XmlNodeList* childs = xml->ChildNodes;
        for(int i = 0; i < childs->Count; ++i)
            FillEntitiesDictionary(dictionary, childs->Item(i));
    }
}

void LinguisticsKernelConroller::FormingNewValue(
    System::Collections::Generic::Dictionary<String^, XmlNode*>* dictionary)
{
    CFullname* entityName = new CFullname(SemanticResolutionController::GetCoreferenceResolution(), dictionary);
    List<XmlNode*>* list = gcnew List<XmlNode*>(dictionary->Values);
    for(int i = 0; i < list->Count; ++i)
    {
        XmlAttribute* fullname = list->Item[i]->Attributes->get_ItemOf(
            __box(BlockAttribute::FULLNAME)->ToString());
        if(fullname && (fullname->Value == ""))
        {
            fullname->Value = entityName->ToString(
                SemanticResolutionController::GetServiceFunction()->ToXElement(list->Item[i]), "N");
            list->Item[i]->Attributes->Remove(list->Item[i]->Attributes->get_ItemOf(
                __box(BlockAttribute::LINK)->ToString()));
            list->Item[i]->Attributes->Remove(list->Item[i]->Attributes->get_ItemOf(
                __box(BlockAttribute::EI)->ToString()));
        }
    }
}*/

String^ LinguisticsKernelConroller::GetMorphoXML(String^ word)
{
	throw (gcnew NotImplementedException());
	//---return (L"");
}

String^ LinguisticsKernelConroller::GetNormalWordForm(String^ word)
{
    return (m_pIndexation->GetNormalWordForm(word));
}

List<String^>^ LinguisticsKernelConroller::GetAllNormalWordForm(String^ word)
{
	return (m_pIndexation->GetAllNormalWordForm(word));
}

String^ LinguisticsKernelConroller::GetAllWordFormsXML(String^ word)
{
    return (m_pIndexation->GetAllWordFormsXML(word, SS::Core::Features::Values::PartOfSpeechTypes.Undefined));
}

List<String^>^ LinguisticsKernelConroller::GetAllWordForms(String^ word)
{
    return (m_pIndexation->GetAllWordForms(word, SS::Core::Features::Values::PartOfSpeechTypes.Undefined));
}

Tuple<List<String^>^, List<String^>^>^ LinguisticsKernelConroller::GetAllWordFormsWithPartsOfSpeech(String^ word)
{
	return (m_pIndexation->GetAllWordFormsWithPartsOfSpeech(word));
}

List<String^>^ LinguisticsKernelConroller::GetTokens(String^ srcText)
{
	return (m_pIndexation->GetTokens(srcText));
}

String^ LinguisticsKernelConroller::GetMorphoInfo(String^ srcText)
{
	return (m_pIndexation->GetMorphoInfo(srcText));
}

Tuple<System::Collections::Generic::IEnumerable<ThemeItem^>^, System::Collections::Generic::IEnumerable<LinkItem^>^>^
	LinguisticsKernelConroller::GetSemanticNetwork(XElement^ rdf, Linguistics::Coreference::ICoreferenceInfo^ coreferenceInfo, bool generateAllSubthemes)
{
	return (m_pSemanticNetwork->BuildSemanticNetwork(rdf, coreferenceInfo, generateAllSubthemes));
}

void LinguisticsKernelConroller::FilterRDF(XElement^% rdf)
{
	SemanticResolution::EntityFilter::FilterEntities(rdf, Utils::JobProfile::SemanticNetwork, false);
}

bool LinguisticsKernelConroller::IsTheme(XElement^ entity)
{
	return (SemanticResolution::SemanticNetwork::IsTheme(entity));
}

Linguistics::Core::IText^ LinguisticsKernelConroller::SelectEntities(String^ sText, String^ baseDate)
{
	Linguistics::Core::TextContent^ content = gcnew Linguistics::Core::TextContent(sText, baseDate, Linguistics::Core::ChildContentSelectorMode::All);
	EntitiesSelector^ selector = gcnew EntitiesSelector(m_pIndexation, &SS::LinguisticProcessor::CIndexation::SelectEntitiesOnContent);
	SelectEntitiesOnContent(content->ParentContent, content->BaseDate, selector);
	return (content->GetTextObject());
}

void LinguisticsKernelConroller::SelectEntitiesOnContent(Linguistics::Core::Content^ content, String^ baseDate, EntitiesSelector^ selector)
{
	selector(content, baseDate);
	array< Linguistics::Core::Content^ >^ children = content->GetChildren();
	for (int i = 0; i < children->Length; ++i)
	{
		SelectEntitiesOnContent(children[i], baseDate, selector);
	}
}

XElement^ LinguisticsKernelConroller::GetRDF_New(String^ text, bool afterSpellChecking, System::DateTime baseDate, int selectEntitiesMode)
{
	//try
	{
		SelectEntitiesMode selectMode = (SelectEntitiesMode) selectEntitiesMode;
		String^ date = baseDate.ToString("yyyy-MM-dd");
		XElement^ xmlText = nullptr;
		String^ sourceText = text;
		if (afterSpellChecking)
		{
			sourceText = SemanticResolution::CorrectedTextParser::GetCorrectedText(sourceText);
		}
		switch(selectMode)
		{
			case SelectEntitiesMode::Full:
				xmlText = GetRDF_New(sourceText, baseDate)->ToXElement();
				if (_IsUseGeoNamesDict)
				{
					SemanticResolution::EntityFilter::FilterGeographic(xmlText);
				}
				EntityFilter::FilterEntitiesByType(xmlText);
				ServiceFunction::NormalizeValue(xmlText);
				//xmlText = GetXmlFacts(sourceText, date, _IsUseGeoNamesDict);
				break;
			case SelectEntitiesMode::Simple:
				xmlText = GetXmlEntities(sourceText);
				break;
		}

		FilterRDF(xmlText);
		//m_pCoreferenceResolution->ResolveCoreference(&xmlText);
		SemanticResolution::AnaphoraResolution::ResolveAnaphoras(xmlText);
		if (afterSpellChecking)
		{
			SemanticResolution::CorrectedTextParser::AddCorrectedInformation(xmlText);
		}
		return (xmlText);
	}
	/*catch (Exception* ex)
	{
		Close();
		throw ex;
	}*/
}

Linguistics::Core::IText^ LinguisticsKernelConroller::GetRDF_New(String^ text, System::DateTime baseDate)
{
	String^ date = baseDate.ToString("yyyy-MM-dd");
	
	Linguistics::Core::IText^ rdf = SelectEntities(text, date);
	m_pSurfaceSyntaxAnalyzer->Analyze(rdf);
	m_pAbbreviationResolver->Resolve(rdf);
	return (rdf);
}

Tuple<XElement^, XElement^>^ LinguisticsKernelConroller::ProcessPTS(String^ xml, bool isFullRDF, String^ language)
{
	String^ textShiftTableName = "text";
	PTS::SimpleXmlParser^ parser = gcnew PTS::SimpleXmlParser();
	xml = parser->RemoveNamespacesAndAttributes(xml);
	List<PTS::TableBlock^>^ tables = parser->GetTablesFromText(xml);
	Tuple<String^, Linguistics::Core::IText^, List<PTS::TextRange^>^>^ result = ProcessPTSText(xml, isFullRDF, language, parser);
	parser->SaveShiftTableToRepository(textShiftTableName);
	int insertIndex = 0;
	for (int i = 0; i < tables->Count; ++i)
	{
		PTS::TableBlock^ table = tables[i];
		table->StartPosition -= parser->GetShift(textShiftTableName, table->StartPosition - 1);
		for (int j = insertIndex; j < result->Item3->Count; ++j)
		{
			if (result->Item3[j]->Range->StartPosition >= table->StartPosition)
			{
				break;
			}
			++insertIndex;
		}
		for (int j = 0; j < table->Rows->Length; ++j)
		{
			Tuple<String^, Linguistics::Core::IText^, List<PTS::TextRange^>^>^ tableResult =
				ProcessPTSText( (String^)table->Rows[j], isFullRDF, language, parser);
			if (tableResult->Item3->Count > 0)
			{
				for (int k = 0; k < tableResult->Item3->Count; ++k)
				{
					PTS::TextRange^ baseRange = tableResult->Item3[k];
					String^ leftContext = tableResult->Item1->Substring(0, baseRange->Range->StartPosition);
					String^ rightContext = tableResult->Item1->Substring(baseRange->Range->EndPosition);
					result->Item3->Insert(insertIndex, gcnew PTS::TextRange(baseRange->Range, leftContext, rightContext));
					++insertIndex;
				}
			}
		}
	}
	parser->ClearShiftTableRepository();
	XElement^ textRanges = gcnew XElement(Utils::Converter::ToXName("root"));
	for (int i = 0; i < result->Item3->Count; ++i)
	{
		textRanges->Add(result->Item3[i]->ToXElement());
	}
	XElement^ rdf = result->Item2->ToXElement();
	if (isFullRDF)
	{
		SemanticResolution::EntityFilter::FilterEntities(rdf, Utils::JobProfile::SemanticNetwork, true);
	}
	return (gcnew Tuple<XElement^, XElement^>(rdf, textRanges));
}

Tuple<String^, Linguistics::Core::IText^, List<PTS::TextRange^>^>^ LinguisticsKernelConroller::ProcessPTSText(
	String^ xml, bool isFullRDF, String^ language, PTS::SimpleXmlParser^ parser)
{
	PTS::NumberRangeSelector^ numberRangeSelector = gcnew PTS::NumberRangeSelector();
	List<SimpleEntities::SimpleEntity^>^ forFilter = gcnew List<SimpleEntities::SimpleEntity^>();
	List<SimpleEntities::SimpleEntity^>^ forSave = gcnew List<SimpleEntities::SimpleEntity^>();
	String^ text = parser->ParseAndSelectEntities(xml, language, forFilter, forSave);

	Linguistics::Core::TextContent^ content = gcnew Linguistics::Core::TextContent(text, "", Linguistics::Core::ChildContentSelectorMode::None);
	EntitiesSelector^ selector = nullptr;
	if (isFullRDF)
	{
		selector = gcnew EntitiesSelector(m_pIndexation, &SS::LinguisticProcessor::CIndexation::SelectSimpleEntitiesOnContent);
	}
	else
	{
		selector = gcnew EntitiesSelector(m_pIndexation, &SS::LinguisticProcessor::CIndexation::SelectCeterusEntitiesOnContent);
	}
	SelectEntitiesOnContent(content->ParentContent, content->BaseDate, selector);
	AddEntityListToContent(content->ParentContent, forFilter);
	AddEntityListToContent(content->ParentContent, forSave);
	Linguistics::Core::IText^ textObject = content->GetTextObject();
	List<PTS::TextRange^>^ ranges = numberRangeSelector->SelectTextRanges(textObject, language);
	if (isFullRDF)
	{
		m_pSurfaceSyntaxAnalyzer->Analyze(textObject);
	}
	return (gcnew Tuple<String^, Linguistics::Core::IText^, List<PTS::TextRange^>^>(text, textObject, ranges));
}

void LinguisticsKernelConroller::AddEntityListToContent(Linguistics::Core::Content^ content, List<SimpleEntities::SimpleEntity^>^ entities)
{
	List<Linguistics::Core::Entity^>^ entityList = gcnew List<Linguistics::Core::Entity^>();
	for (int i = 0; i < entities->Count; ++i)
	{
		entityList->Add(m_pIndexation->CreateSimpleEntity(entities[i]));
	}
	content->SetEntitiesForContentAndChildren(entityList, false);
}

}