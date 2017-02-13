//SyntaxTest.cpp

#include "stdafx.h"
#ifdef _SS_UNITTESTS
#include "SyntaxTest.h"
#include <iostream>
#include <fstream>
#include "../../ASCInterface/IBaseManager.h"
#include "../Container.h"
#include "../../CommonContainers/UnitTests/AdditionalTest.h"

namespace SS
{
namespace UnitTests
{
namespace Syntax
{
	//конструктор
	CSyntaxTest::CSyntaxTest(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
	{
		SS_TRY
		{
			m_pLoadManager = pLoadManager;
			m_pLexic = NULL;
			m_pSyntax = NULL;
			m_pSemantic = NULL;
			m_pQueryParams = NULL;
			m_pIndexationParams = NULL;
			//Запуск магического кода
			RunDictionariesMagicCode(m_pLoadManager);
			//Получить лексический анализатор
			m_pLexic = (SS::Interface::Core::LexicalAnalyze::ILexica*)
			m_pLoadManager->GetInterface(L"./core/linguistics/LexicalAnalyzer.dll", CLSID_Lexica, IID_Lexica);
			//Получить синтаксический анализатор
			m_pSyntax = (SS::Interface::Core::ISyntaxAnalyzer*)
			m_pLoadManager->GetInterface(L"./core/linguistics/SyntaxAnalyzer.dll",	CLSID_SyntaxAnalyzer, IID_SyntaxAnalyzer);
			//Получить семантический анализатор
			m_pSemantic = (SS::Interface::Core::ISemanticAnalyze*)
			m_pLoadManager->GetInterface(L"./core/linguistics/SemanticAnalyzer.dll", CLSID_SemanticAnalyzer, IID_SemanticAnalyzer);
			//Получить параметры анализа
			m_pLoadManager->Create(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams, (void**) &m_pQueryParams);
			m_pLoadManager->Create(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams, (void**) &m_pIndexationParams);
			

			if ( (!m_pLexic) || (!m_pSyntax) || (!m_pQueryParams) || (!m_pIndexationParams) || (!m_pSemantic) )
			{ std::cout << "CSyntaxTest::CSyntaxTest anything == NULL \n"; }
			
			//Установить параметры запроса
			m_pQueryParams->SetCurrentSystemMode(SS::Core::Types::ecsmLocal);
			m_pQueryParams->SetAnalyseMod( SS::Core::Types::eamQuery );
			m_pQueryParams->GetAnalyseDepthParams()->SetIsMorpho(true);
			m_pQueryParams->GetAnalyseDepthParams()->SetIsSemantic(true);
			m_pQueryParams->GetAnalyseDepthParams()->SetIsSyntax(true);
			m_pQueryParams->GetAnalyseDepthParams()->SetIsSynonims(true);
			
			//установить параметры индексации
			m_pIndexationParams->SetCurrentSystemMode(SS::Core::Types::ecsmLocal);
			m_pIndexationParams->SetAnalyseMod( SS::Core::Types::eamIndexation );
			m_pIndexationParams->GetAnalyseDepthParams()->SetIsMorpho(true);
			m_pIndexationParams->GetAnalyseDepthParams()->SetIsSemantic(true);
			m_pIndexationParams->GetAnalyseDepthParams()->SetIsSyntax(true);
			m_pIndexationParams->GetAnalyseDepthParams()->SetIsSynonims(true);
		}
		SS_CATCH(L"")
	}

	//Магический код
	void CSyntaxTest::RunDictionariesMagicCode(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
	{
		SS::Interface::Core::CommonServices::IWorkRegister* pWorkRegister =
		(SS::Interface::Core::CommonServices::IWorkRegister*) 
		pLoadManager->GetInterface(L"./core/LoadManager.dll",CLSID_WorkRegister,IID_WorkRegister);

		const std::wstring wszDataBaseName = L"NewWordForms1";

		std::wstring m_wsDataBasePath = pWorkRegister->GetParameter(L"PathDataBase");
		std::wstring m_wsDataBasePathRemove = m_wsDataBasePath;
		m_wsDataBasePathRemove += L"IndexedTexts\\ndx\\";
		m_wsDataBasePathRemove += wszDataBaseName;
		m_wsDataBasePathRemove += L"\\";
		SS::Interface::Core::ResourceManagers::IBaseManager* pBaseManager = 
		(SS::Interface::Core::ResourceManagers::IBaseManager*)
		pLoadManager->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_BaseManager);

		if(!pBaseManager)
		{ SS_THROW(L"!m_pBaseManager"); }

		pBaseManager->SetBasePath(m_wsDataBasePath.c_str());
		pBaseManager->OpenBase(wszDataBaseName.c_str());
		pBaseManager->ClearBase();
	}

	//Получение IText* из установленной строки
	SS::Interface::Core::BlackBox::IText* CSyntaxTest::GetText(std::wstring str, EWorkType type)
	{
		SS_TRY
		{
			SS::Core::Types::IndexationSearch::TNativeTextForIndexation nativeText;
			nativeText.SetText(str.c_str());
			if (type == eIndexation) 
			{ 
				m_pLexic->SetAnalyseParams(m_pIndexationParams);
				m_pSyntax->SetAnalyseParams(m_pIndexationParams);
				m_pSemantic->SetAnalyseParams(m_pIndexationParams);
			}
			else if (type == eQuery)
			{ 
				m_pLexic->SetAnalyseParams(m_pQueryParams);
				m_pSyntax->SetAnalyseParams(m_pQueryParams);
				m_pSemantic->SetAnalyseParams(m_pQueryParams);
			}
			else
			{ std::cout << "ОШИБКА!!! Не установлены параметры анализа.\n"; exit(0); }
			m_pLexic->SetText(&nativeText);

			//Создаем TextStorage
			SS::Interface::Core::BlackBox::IBlackBoxTextStorage* pTextStorage =
			(SS::Interface::Core::BlackBox::IBlackBoxTextStorage*) 
			m_pLoadManager->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxTextStorage, IID_BlackBoxTextStorage);
			//Создаем Text
			SS::Interface::Core::BlackBox::IText* pText = pTextStorage->CreateText();
			
			//Лексический анализ
			if (m_pLexic->LoadNext()) 
			{ m_pLexic->AnalyzeNext(pText); }
					
			return pText;
		}
		SS_CATCH(L"")
	}
	
	//Общий тест
	void CSyntaxTest::TestCommon()
	{
		using namespace SS::Interface::Core::BlackBox;
		
		//Лексический анализ
		IText* pText = GetText(L"What are people born 1965 through 1980 called ?", eQuery);
		//ПОСЛЕ ЛЕКСИЧЕСКОГО
		SAVE_LOG(L"ПОСЛЕ ЛЕКСИЧЕСКОГО АНАЛИЗА");
		CAdditionalTest oAdditionalTest;
		oAdditionalTest.SetLoadManager(m_pLoadManager);
		oAdditionalTest.SetOneWordAnalyse(L"1980");
		oAdditionalTest.AnalyseText(pText);

		//Семантический анализ
		m_pSemantic->AnalyseNext(pText);
		//ПОСЛЕ СЕМАНТИЧЕСКОГО
		SAVE_LOG(L"ПОСЛЕ СЕМАНТИЧЕСКОГО АНАЛИЗА");
		oAdditionalTest.AnalyseText(pText);
		
		//Синтаксический анализ
		m_pSyntax->AnalyzeNext(pText);
		//ПОСЛЕ СИНТАКСИЧЕСКОГО
		SAVE_LOG(L"ПОСЛЕ СИНТАКСИЧЕСКОГО АНАЛИЗА");
		oAdditionalTest.AnalyseText(pText);

		std::cout << "ready.";
	}

	//Тестирует скорость разбора для индексации
	void CSyntaxTest::IndexationSpeedTest()
	{
		SS_TRY
		{
			std::wstring bigText;
			std::wstring::iterator iterStr;
			SS::Interface::Core::BlackBox::IText* pText;
			SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams = NULL;
			
			m_pLoadManager->Create(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams, (void**)&pAnalyseParams);
			if (!pAnalyseParams)
			{ SS_THROW(L"CSyntaxTest::IndexationSpeedTest() Не удалось создать AnalyseParams");	}

			pAnalyseParams->SetCurrentSystemMode(SS::Core::Types::ecsmGlobal);
			pAnalyseParams->SetAnalyseMod(SS::Core::Types::eamIndexation);
			//pAnalyseParams->SetAnalyseMod(eamQuery);
			pAnalyseParams->GetAnalyseDepthParams()->SetIsMorpho(true);
			pAnalyseParams->GetAnalyseDepthParams()->SetIsSemantic(true);
			pAnalyseParams->GetAnalyseDepthParams()->SetIsSyntax(true);
			pAnalyseParams->GetAnalyseDepthParams()->SetIsSynonims(true);

			
			SS::Interface::Core::BlackBox::IBlackBoxTextStorage* pTextStorage;
			m_pLoadManager->Create(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxTextStorage, IID_BlackBoxTextStorage, (void**)&pTextStorage);
					
			std::ifstream fin("D:\\SS\\SS\\UnitTestsBin\\Core\\SyntaxAnalyserTest\\Text\\BigText2.txt");
			//ifstream fin("D:\\SS\\SS\\UnitTestsBin\\Core\\SyntaxAnalyserTest\\Text\\TextQuery.txt");
			//ifstream fin("D:\\SS\\SS\\UnitTestsBin\\Core\\SyntaxAnalyserTest\\Text\\SmallText.txt"); 
		
			char chSymb;
			while ( fin.get(chSymb) )
			{ bigText.push_back(chSymb); }
									
			SS::Core::Types::IndexationSearch::TNativeTextForIndexation NativeText;
			NativeText.SetText( bigText.c_str() );
			
			m_pLexic->SetAnalyseParams(pAnalyseParams);
			m_pLexic->SetText(&NativeText);
			pText = pTextStorage->CreateText();
			
			m_pSyntax->SetAnalyseParams(pAnalyseParams);
						
			while (m_pLexic->LoadNext())
			{
				std::cout << "-";
				m_pLexic->AnalyzeNext(pText);
				
				std::cout << "=";
				SS_START_TICK(L"Syntax->Analyze");
				m_pSyntax->AnalyzeNext(pText);
				SS_STOP_TICK(L"Syntax->Analyze");
				
				pText->ClearText();
			}
									
			SS_VIEW_TICK(L"Syntax->Analyze");
			//SS_VIEW_TICK(L"m_ExpertController.SetSentence()");
			//SS_VIEW_TICK(L"m_ExpertController.ParseSentenceDynamic()");
			//SS_VIEW_TICK(L"ResetIndexationFlags");
		}			
		SS_CATCH(L"")
	}

	//Тест для шаблона контейнера
	void CSyntaxTest::TestContainer()
	{
		std::cout << "TestContainer \n";
		int iCount = 10;
		SS::Syntax::BoardElements::CContainer<int, 10> oContainer;

		int* pi = NULL;
		int* pi1 = NULL;

		for (int i = 0; i < 10; i++)
		{ 
			pi = oContainer.Add();
			(*pi) = i;
		}					
		
		for (pi = oContainer.GetFirst(); pi; pi = oContainer.GetNext())
		{ 
			if ( ((*pi) == 3) )
			{ 
				pi1 = pi;
				oContainer.DeleteCurrent(); 
			}
		}
	
		for (pi = oContainer.GetFirst(); pi; pi = oContainer.GetNext())
		{ std::cout << *pi << '\n';	}

		oContainer.RestoreForAddress(pi1);
		
		for (pi = oContainer.GetFirst(); pi; pi = oContainer.GetNext())
		{ std::cout << *pi << '\n';	}

		oContainer.Clear();
		oContainer.DeleteCurrent();
		pi = oContainer.Add();
		(*pi) = 100;
	
		int iii = 0;
	}

}
}
}

#endif
