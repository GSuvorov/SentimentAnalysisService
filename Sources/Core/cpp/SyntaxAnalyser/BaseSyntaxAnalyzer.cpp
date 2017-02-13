#include "stdafx.h"
#include "BaseSyntaxAnalyzer.h"
#include "ParsingType.h"

#ifdef _SS_UNITTESTS
	#include "../CommonContainers/UnitTests/AdditionalTest.h"
#endif //_SS_UNITTESTS

using namespace SS::Interface::Core;

namespace SS
{
namespace Syntax
{

///Конструктор
CBaseSyntaxAnalyzer::CBaseSyntaxAnalyzer(SS::Syntax::ExpertModel::ESyntaxBase eBase) 
	:
	m_ExpertController(eBase)
{	
}


ULONG CBaseSyntaxAnalyzer::Release()
{
	delete this;
	return 0;
}

HRESULT CBaseSyntaxAnalyzer::QueryInterface(REFIID pIID, void** pBase)
{
	if (pIID == IID_SyntaxAnalyzer)
	{
		*pBase = static_cast<SS::Interface::Core::ISyntaxAnalyzer*> (this);
	}
	else if (pIID == IID_Base)
	{
		*pBase = static_cast<SS::Interface::IBase*>( this );
	}
	else
	{
		*pBase = NULL;
	}
	return S_OK;
}

void CBaseSyntaxAnalyzer::Init()
{
	SS_TRY
	{
		if (GetLoadManager() == NULL)
		{ SS_THROW(L"Непроинициализированный LoadManager");	}
		
		m_ExpertController.GetLoader()->Load();
	}
	SS_CATCH(L"")
}

///Создание для каждого пришедшего юнита Синтаксической фичи по умолчанию
void CBaseSyntaxAnalyzer::CreateSyntax(SS::Interface::Core::BlackBox::ISentence* pSentence) const
{
	SS_TRY
				
		SS::Interface::Core::BlackBox::IUnit* pCurrUnit = pSentence->GetFirstUnit();
		SS::Interface::Core::BlackBox::IUnit* pRightUnit = NULL;
		SS::Interface::Core::BlackBox::IUnit* pChildUnit = NULL;
		//ПЕРЕБОР Unit - ов
		while (pCurrUnit)
		{
			pChildUnit = pCurrUnit->GetLeftChildUnit();
			if (pChildUnit)
			{
				pCurrUnit = pChildUnit;
				continue;	
			}
			
			//Just creating feature
			pCurrUnit->CreateSyntaxFeature();
			
			pRightUnit = pCurrUnit->GetRightUnit();
			while (pRightUnit == NULL)
			{
				pCurrUnit = pCurrUnit->GetParentUnit();
				if (pCurrUnit == NULL)
				{ break; }
				
				//Just creating feature
				pCurrUnit->CreateSyntaxFeature();

				pRightUnit = pCurrUnit->GetRightUnit();
			}
			pCurrUnit = pRightUnit;
		}
		//КОНЕЦ переб Unit - ов
	
	SS_CATCH(L"")
}

void CBaseSyntaxAnalyzer::AnalyzeNext(SS::Interface::Core::BlackBox::IText* text)
{
	for (BlackBox::IParagraph *pPar = text->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
	{
		for (BlackBox::ISentence *pSen = pPar->GetFirstSentence(); pSen; pSen = pSen->GetNextSentence())
		{
			if (pSen->isEnabled()==0 || pSen->GetProcessed())
				continue; // предложение не нужно для синтанализа

			AnalyzeNext(pSen);
		}
	}
}

void CBaseSyntaxAnalyzer::AnalyzeNext(SS::Interface::Core::BlackBox::ISentence* sentence)
{
	SS_TRY
	{
#ifdef _SS_UNITTESTS
		//SAVE_LOG(L"ВХОД СИНТАКСИСА:");
		//SS::UnitTests::CAdditionalTest oAdditionalTest;
		//oAdditionalTest.SetLoadManager(GetLoadManager());
		//oAdditionalTest.SetOneWordAnalyse(L"born");
		//oAdditionalTest.AnalyseText(text);
#endif //_SS_UNITTESTS
		
		if (!GetAnalyseParams()) 
		{ SS_THROW(L"Не установлены AnalyseParams"); }

		if (GetAnalyseParams()->GetAnalyseMod() == SS::Core::Types::eamStrictQuery)
		{ return; }
		
		if (GetLoadManager() == NULL)
		{ SS_THROW(L"Непроинициализированный LoadManager"); }

		//Тип разбора
		ExpertModel::ParsingType pType;

        //Creating... SyntaxFeature 
		CreateSyntax(sentence);

		if (SS::Core::Types::ealEnglish == sentence->GetLanguage())
		{
			switch (GetAnalyseParams()->GetAnalyseMod())
			{
				case (SS::Core::Types::eamQuery):
				case (SS::Core::Types::eamStrictQuery):
				case (SS::Core::Types::eamQueryAndSearchString):
				case (SS::Core::Types::eamRejectUnimportant):
				case (SS::Core::Types::eamOnlyKeyWords):
					pType = ExpertModel::ptQueryAnalyzeEnglish;
					break;
				case (SS::Core::Types::eamIndexation):
				case (SS::Core::Types::eamSearch):
								
					//#ifdef _SS_UNITTESTS
					pType = ExpertModel::ptFullParsingEnglish;
					//#else
					//pType = ExpertModel::ptMorphoResolveEnglish;
					//#endif
								
					break;
				default:
					SS_THROW(L"Неподдерживаемый режим синтаксического разбора");
			}
		}
		else if (SS::Core::Types::ealRussian == sentence->GetLanguage())
		{
			switch (GetAnalyseParams()->GetAnalyseMod())
			{
				case (SS::Core::Types::eamQuery):
				case (SS::Core::Types::eamStrictQuery):
				case (SS::Core::Types::eamQueryAndSearchString):
				case (SS::Core::Types::eamRejectUnimportant): 
				case (SS::Core::Types::eamOnlyKeyWords):
					pType = ExpertModel::ptQueryAnalyzeRussian;
					break;
				case (SS::Core::Types::eamIndexation):
				case (SS::Core::Types::eamSearch):
																
					//#ifdef _SS_UNITTESTS
					pType = ExpertModel::ptFullParsingRussian;
					//#else
					//pType = ExpertModel::ptMorphoResolveRussian;
					//#endif
																
					break;
				default:
					SS_THROW(L"Неподдерживаемый режим синтаксического разбора");
			}
		}
		else
		{
			return;
			//SS_THROW(L"Неподдерживаемый язык для синтаксического разбора");
		}

			//SS_START_TICK(L"m_ExpertController.SetSentence()");
		m_ExpertController.SetSentence(sentence);				
			//SS_STOP_TICK(L"m_ExpertController.SetSentence()");
			//SS_START_TICK(L"m_ExpertController.ParseSentenceDynamic()");
		m_ExpertController.ParseSentenceDynamic(pType);
			//SS_STOP_TICK(L"m_ExpertController.ParseSentenceDynamic()");
		//Сброс флага индексации для всех пустых индексов в предложении.
			//SS_START_TICK(L"ResetIndexationFlags");
		ResetIndexationFlags(sentence->GetFirstUnit(), sentence->GetLastUnit());

		sentence->SetProcessed(SS::Interface::Core::BlackBox::eProcessed::Syntax);
		//SS_STOP_TICK(L"ResetIndexationFlags");

#ifdef _SS_UNITTESTS
		//SAVE_LOG(L"ВЫХОД СИНТАКСИСА:");
		//oAdditionalTest.AnalyseText(text);
#endif //_SS_UNITTESTS		
	}
	SS_CATCH(L"")
}

void CBaseSyntaxAnalyzer::ResetIndexationFlags( BlackBox::IUnit* first, BlackBox::IUnit* last )
{
	SS_TRY
	{
		for   (BlackBox::IUnit* topUnit = first; topUnit != NULL; topUnit = topUnit->GetRightUnit())
		{
			for (BlackBox::IIndex* index = topUnit->GetFirstIndex(); index != NULL; index = topUnit->GetNextIndex())
			{
				if (index->GetFirstFeature() == NULL)
				{ index->SetIndexationFlag(false); }
			}
			ResetIndexationFlags(topUnit->GetLeftChildUnit(), topUnit->GetRightChildUnit());
		}
	}
	SS_CATCH( L"" )
}
//переопределение от IBase
void CBaseSyntaxAnalyzer::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	SS_TRY
	{
		SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
		m_ExpertController.SetLoadManager(pLoadManager);
	}
	SS_CATCH(L"")
}

//переопределяю от CAnalyseParams
void CBaseSyntaxAnalyzer::SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams)
{
	SS_TRY
	{
		CAnalyseParams::SetAnalyseParams(pAnalyseParams);
		m_ExpertController.SetAnalyseParams(pAnalyseParams);
		Init();
	}
	SS_CATCH(L"")
}

}
}