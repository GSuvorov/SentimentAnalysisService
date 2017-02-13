#include "StdAfx.h"
#include ".\semanticanalyze.h"


namespace SS
{
	namespace Core
	{
		CSemanticAnalyze::CSemanticAnalyze(void) : m_pIText(NULL)
		{
		}

		CSemanticAnalyze::~CSemanticAnalyze(void)
		{
		}

		HRESULT CSemanticAnalyze::QueryInterface(REFIID pIID, void** pBase)
		{
			if(pIID == IID_SemanticAnalyzer){
				*pBase = (SS::Interface::Core::ISemanticAnalyze*)this;
			}
			else if(pIID == IID_Base){
				*pBase = (SS::Interface::IBase*)this;
			}
			else
			{
				*pBase = NULL;
			}
			return S_OK;
		}

		ULONG CSemanticAnalyze::Release()
		{
			delete this;
			return 0;
		}
		//void CSemanticAnalyze::Init(
		//	///параметры анализа
		//	SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams)
		//{
		//	//m_pAnalyseParams=pAnalyseParams;
		//	SetAnalyseParams(pAnalyseParams);

		//}
		void CSemanticAnalyze::SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams)
		{
			SS_TRY
			{
				SS::Interface::Core::Common::CAnalyseParams::SetAnalyseParams(pAnalyseParams);

				if(m_SemanticAnalyzeManager.GetDictionaryInterface())
					m_SemanticAnalyzeManager.GetDictionaryInterface()->SetAnalyseParams(GetAnalyseParams());

				m_SemanticAnalyzeManager.SetAnalyseParams(GetAnalyseParams());
			}
			SS_CATCH(L"")
		}

		///анализ очередной части текста и занесение результата в pText
		bool CSemanticAnalyze::AnalyseNext(
			///это куда записывать результат
			SS::Interface::Core::BlackBox::IText* pText)
		{
			SS_TRY
			{
				if(!GetAnalyseParams()->GetAnalyseDepthParams()->IsSemantic())
					return true;

				if(GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamIndexation||
					GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamSearch) 
				{
					m_SemanticAnalyzeManager.IndexateText(pText);
				}
				if(GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamOnlyKeyWords ||
					GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamQuery ||
					GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamRejectUnimportant)
				{
					m_SemanticAnalyzeManager.QuestionAnalysis(pText);
				}
				return true;
			}
			SS_CATCH(L"")
		}

		bool CSemanticAnalyze::QueryToSearchString(SS::Interface::Core::BlackBox::IText* pQuery, std::wstring * pSearchString)
		{
			SS_TRY
			{
				m_SemanticAnalyzeManager.CollectQuestion(pQuery, pSearchString);
				return true;
			}
			SS_CATCH(L"")
		}

		bool CSemanticAnalyze::SyntaxQuestionPostProcess(SS::Interface::Core::BlackBox::IText* pQuery)
		{
			SS_TRY
			{
				if(GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamQuery ||
						GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamRejectUnimportant ||
						GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamOnlyKeyWords)
				{		
					m_SemanticAnalyzeManager.AnalizeQuestion(pQuery);
				}

				return true;
			}
			SS_CATCH(L"")
		}
		void CSemanticAnalyze::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
		{
			SS_TRY
			{
				CBaseCoreClass::SetLoadManager(pLoadManager);
				m_SemanticAnalyzeManager.SetLoadManager(pLoadManager);
			}
			SS_CATCH(L"")
		}


	}
}