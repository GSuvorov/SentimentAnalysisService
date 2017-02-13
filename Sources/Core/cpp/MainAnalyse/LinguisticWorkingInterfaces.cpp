#include "StdAfx.h"
#include ".\LinguisticWorkingInterfaces.h"
#include "..\[libs]\_Paths.h"

namespace SS
{
namespace LinguisticProcessor
{

CLinguisticWorkingInterfaces::CLinguisticWorkingInterfaces(void) : 
	m_pDictionary(NULL)
  , m_pLexica(NULL)
  , m_pSyntaxAnalyzer(NULL)
  , m_pSemanticAnalyze(NULL)
  , m_pRelevanceEvaluation(NULL)
  , m_pBlackBoxTextStorage(NULL)
  , m_pText(NULL)
  , m_pTextFeatureText(NULL)
  , m_pQuery(NULL)
  , m_pBlackBoxQueryStorage(NULL)
  , m_pLinguisticRelevance(NULL)
  , m_pCoSeDiManager(NULL)
  , m_pQueryDeformer(NULL) 
{
	m_oMode.m_bIsLiteMode = false;
}

CLinguisticWorkingInterfaces::~CLinguisticWorkingInterfaces(void)
{
	if(m_pText)
		m_pText->ReleaseText();
	if(m_pTextFeatureText)
		m_pTextFeatureText->ReleaseText();
	if(m_pQuery)
		m_pQuery->Release();

}

void CLinguisticWorkingInterfaces::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	SS_TRY
	{
		SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);

		// анализаторы
		m_pDictionary = 
			(SS::Interface::Core::Dictionary::IDictionary*)
			GetLoadManager()->GetInterface( DICTIONARIES_DLL_PATH, CLSID_IDictionary, IID_IDictionary);
		m_pLexica= 
			(SS::Interface::Core::LexicalAnalyze::ILexica*)
			GetLoadManager()->GetInterface(L"./core/linguistics/LexicalAnalyzer.dll", CLSID_Lexica, IID_Lexica);
		m_pSemanticAnalyze = 
			(SS::Interface::Core::ISemanticAnalyze*)
			GetLoadManager()->GetInterface(L"./core/linguistics/SemanticAnalyzer.dll", CLSID_SemanticAnalyzer, IID_SemanticAnalyzer);

		m_pRelevanceEvaluation = 
			(SS::Interface::Core::Rev::IRelevanceEvaluation*)
			GetLoadManager()->GetInterface(L"./core/linguistics/TransformGenerator.dll", CLSID_RevModule, IID_RelevanceEvaluation);

		m_pLinguisticRelevance = 
			(SS::Interface::Core::Rev::ILinguisticRelevance*)
			GetLoadManager()->GetInterface(L"./core/linguistics/LinguisticRev.dll", CLSID_LinguisticRelevance, IID_LinguisticRelevance);

		m_pCoSeDiManager = 
			(SS::Interface::Core::CoSeDi::ICoSeDiManager*)
			GetLoadManager()->GetInterface(L"./core/linguistics/CoSeDi.dll", CLSID_ICoSeDiManager, IID_ICoSeDiManager);

		m_pCoSeDiManager->GetLoader()->Load();

		// параметры анализа
		SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams = NULL;
		pAnalyseParams = 
			(SS::Interface::Core::MainAnalyse::IAnalyseParams*)
			GetLoadManager()->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams);
		SetAnalyseParams(pAnalyseParams);

		// хранилища результатов анализа
		if(!m_pBlackBoxTextStorage)
		{
			m_pBlackBoxTextStorage = 
				(SS::Interface::Core::BlackBox::IBlackBoxTextStorage*)
				GetLoadManager()->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxTextStorage, IID_BlackBoxTextStorage);
			m_pText				= m_pBlackBoxTextStorage->CreateText();
			m_pTextFeatureText	= m_pBlackBoxTextStorage->CreateText();
		}
		if(!m_pBlackBoxQueryStorage)
		{
			m_pBlackBoxQueryStorage = 
				(SS::Interface::Core::BlackBox::IBlackBoxQueryStorage*)
				GetLoadManager()->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxQueryStorage, IID_BlackBoxQueryStorage);
			m_pQuery = m_pBlackBoxQueryStorage->CreateQuery();
		}

		SS::Interface::Core::CommonServices::ISettingsServer* pSettingsServer =
			(SS::Interface::Core::CommonServices::ISettingsServer*)
			GetLoadManager()->GetInterface(L"./core/GSServer.dll", CLSID_SettingsServer , IID_SettingsServer);
		bool bUseUseQueryDeformer = false;
		try
		{
///			fdgh
			//bUseUseQueryDeformer = //(pSettingsServer->GetBoolean(L"LinguisticProcessor::UseQueryDeformer") != 0 );			
		}
		catch(SS::Interface::Core::CommonServices::IException * pException)
		{
			SAVE_LOG(SS_ERROR AND pException->ToString().c_str() AND __WFUNCTION__);
		}
		catch(...)
		{
			SAVE_LOG(SS_ERROR AND L"Ошибка при обращении к файлу настроек bUseUseQueryDeformer" AND __WFUNCTION__ )
		}

		try
		{
			//m_oMode.m_bIsLiteMode = (pSettingsServer->GetBoolean(L"LinguisticProcessor::LiteMode") != 0);			
		}
		catch(SS::Interface::Core::CommonServices::IException * pException)
		{
			SAVE_LOG(SS_ERROR AND pException->ToString().c_str() AND __WFUNCTION__);
		}
		catch(...)
		{
			SAVE_LOG(SS_ERROR AND L"Ошибка при обращении к файлу настроек m_bIsLiteMode" AND __WFUNCTION__ )
		}

		if(bUseUseQueryDeformer)
		{
			m_pQueryDeformer = 
				(SS::Interface::QueryDeformer::IQueryDeformer*)
				GetLoadManager()->GetInterface(L"\\core\\Linguistics\\QueryDeformer.dll", CLSID_QueryDeformer, IID_QueryDeformer);
		}
		
		if(!m_oMode.m_bIsLiteMode)
		{
			m_pSyntaxAnalyzer = 
				(SS::Interface::Core::ISyntaxAnalyzer*)
				GetLoadManager()->GetInterface(L"./core/linguistics/SyntaxAnalyzer.dll", CLSID_SyntaxAnalyzer, IID_SyntaxAnalyzer);

		}

	}
	SS_CATCH(L"")
}

}
}
