#include "StdAfx.h"

#include ".\smmanager.h"

#include "SMPostSyntax.h"

#include ".\smexpert.h"
#include ".\smquestionparser.h"
#include ".\smquerygenerator.h"
#include ".\smspecifycategory.h"
#include ".\smwordsremover.h"
#include ".\smsyntaxcategoriespulldown.h"
#include ".\smverbformcorrector.h"
#include ".\smsemanticinserter.h"
#include ".\smindexremover.h"
#include ".\smtreetransformation.h"

#include ".\SMAdverbialModifierE.h"
#include ".\SMFeatureCompute.h"
#include ".\SMCitation.h"
#include ".\SMComplexName.h"
#include ".\SMPlaceSpecification.h"
#include ".\SMMeasure.h"
#include ".\SMComplementE.h"
#include ".\SMGenetive.h"
#include ".\SMNounDetectionE.h"
#include ".\SMPassiveE.h"
#include ".\SMSubjectObjectE.h"
#include ".\SMAdverbialModifierR.h"
#include ".\SMInderectObjectR.h"
#include ".\SMPassiveR.h"
#include ".\SMSubjectR.h"
#include ".\SMPassiveR.h"
#include ".\SMSubjectR.h"
#include ".\SMComplementR.h"
#include ".\SMDependentClause.h"
#include ".\SMCauseMannerE.h"
#include ".\SMCauseMannerR.h"
#include ".\SMExAnaphoraE.h"
#include ".\SMExAnaphoraR.h"
#include "SMQuestionAnalysisE.h"
#include "SMQuestionAnalysisR.h"
#include "..\[libs]\_Paths.h"

namespace SS
{
	namespace Semantic
	{
		CSMManager::CSMManager(void) 
			: m_pOfficialVirtualSyntax(NULL), m_pVirtualDictionaries(NULL), m_pDictionaryInterface(NULL)
		{			
			m_QuestionParse.SetExpertsSMManager(this);
			m_QuestionGenerator.SetExpertsSMManager(this);
			m_IndaxationGroup.SetExpertsSMManager(this);
			m_QuestionGroup.SetExpertsSMManager(this);
		}
		CSMManager::~CSMManager(void)
		{
		}
		void CSMManager::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
		{
			CBaseCoreClass::SetLoadManager(pLoadManager);
			SS_TRY
			{
				if(!pLoadManager) 
					SS_THROW(_T(" нет LoadManager "));

				m_pDictionaryInterface = 
					((SS::Interface::Core::Dictionary::IDictionary*)
					GetLoadManager()->GetInterface( DICTIONARIES_DLL_PATH, CLSID_IDictionary, IID_IDictionary))->GetMorphologyAnalyser();

				m_pIAMCMorpho =
					(SS::Interface::Core::AMConverter::IAMConverterMorpho*)
					GetLoadManager()->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);

				m_pIAMCSemantic = 
					(SS::Interface::Core::AMConverter::IAMConverterSemantic*)
					GetLoadManager()->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterSemantic, IID_AMConverterSemantic);

				if(!m_pOfficialVirtualSyntax)
				{
					m_pOfficialVirtualSyntax = new SS::Semantic::VirtualDictionaries::COfficialVirtualSyntaxDictionary();
				}
				m_pIBlackBoxFactory=(SS::Interface::Core::BlackBox::IBlackBoxTextStorage*)GetLoadManager()->GetInterface(L"./core/linguistics/BlackBox.dll",CLSID_BlackBoxTextStorage,IID_BlackBoxTextStorage);
			}
			SS_CATCH(_T("ошибка инициализации!"));
		}
		void CSMManager::AnalizeQuestion(SS::Interface::Core::BlackBox::IText * pIText)
		{	
			SS_TRY
			{
				if(GetAnalyseParams())
					m_QuestionParse.SetAnalyseParams(GetAnalyseParams());
				m_QuestionParse.Analize(pIText);
			}
			SS_CATCH(L"")
		}
		void CSMManager::CollectQuestion(SS::Interface::Core::BlackBox::IText * pIText, std::wstring * pSearchString)
		{
			SS_TRY
			{
				std::list<CSMExpert*>::iterator i;
				m_sQuery.clear();
				m_QuestionGenerator.Analize(pIText);

				*pSearchString = m_sQuery;

				//char buf[1000];

				//wcstombs(buf,m_sQuery.c_str(),1000);

				//pIText->SetTagString(buf);

				return;
				
			}
			SS_CATCH(L"")
		}
		void CSMManager::IndexateText(SS::Interface::Core::BlackBox::IText * pIText)
		{
			SS_TRY
			{
				m_IndaxationGroup.Analize(pIText);
			}
			SS_CATCH(L"")
		}
		void CSMManager::QuestionAnalysis(SS::Interface::Core::BlackBox::IText * pIText)
		{
			SS_TRY
			{
				if(GetAnalyseParams())
					m_QuestionGroup.SetAnalyseParams(GetAnalyseParams());
				m_QuestionGroup.Analize(pIText);
			}
			SS_CATCH(L"")
		}


	}
}