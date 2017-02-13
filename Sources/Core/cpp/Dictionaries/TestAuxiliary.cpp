#include "stdafx.h"
#include "TestAuxiliary.h"
#include "Dictionary.h"
#include "../[libs]/_Paths.h"

namespace SS{ 
namespace UnitTests{ 
namespace Dictionary{


} //namespace Dictionary
} //namespace UnitTests
} //namespace SS

namespace SS{ 
namespace Dictionary{


void PrepereLingvoBaseManager( SS::Interface::Core::ResourceManagers::ILoadManager *pLoadManager )
{
   SS::Interface::Core::ResourceManagers::IBaseManager* pBaseManager;

   pBaseManager = (SS::Interface::Core::ResourceManagers::IBaseManager*)
      pLoadManager->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_BaseManager);

   APL_ASSERT(pBaseManager != 0);

#if 1
   SS::Interface::Core::CommonServices::IWorkRegister* pWorkRegister =
      ((SS::Interface::Core::CommonServices::IWorkRegister*) 
      pLoadManager->GetInterface(L"./core/LoadManager.dll",CLSID_WorkRegister,IID_WorkRegister));

   std::wstring m_wsDataBasePath = pWorkRegister->GetParameter(L"PathDataBase");
   //m_wsDataBasePath = _T("c:\\SS\\SS\\DatabaseTMP\\");

   pBaseManager->SetBasePath(m_wsDataBasePath.c_str());
#endif
}
///////////////////////////////////////////////////////////////////////////////

IDictionary *GetIDictionary( SS::Interface::Core::ResourceManagers::ILoadManager *pLoadManager )
{
   PrepereLingvoBaseManager( pLoadManager );

   SS::Interface::Core::ResourceManagers::IBaseManager* pBaseManager;

   pBaseManager = (SS::Interface::Core::ResourceManagers::IBaseManager*)
      pLoadManager->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_BaseManager);

   APL_ASSERT(pBaseManager != 0);

   pBaseManager->OpenBase(L"NewWordForms1");

   IDictionary *pDictionary = (IDictionary *)
      pLoadManager->GetInterface( DICTIONARIES_DLL_PATH, CLSID_IDictionary, IID_IDictionary );

   APL_ASSERT(pDictionary != 0);

   if( true )
   {
      SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams = 
			(SS::Interface::Core::MainAnalyse::IAnalyseParams*)
			pLoadManager->GetInterface(L"./core/linguistics/MainAnalyse.dll", CLSID_AnalyseParams, IID_AnalyseParams);

      APL_ERROR( pAnalyseParams != 0 );
      //pAnalyseParams->SetAnalyseMod( eamRTFIndexation );
      //pAnalyseParams->SetAnalyseMod( eamRTFQuery );   //Используем однокореные слова
      //pAnalyseParams->SetAnalyseMod( eamRTFIndexation );
      pAnalyseParams->SetAnalyseMod( eamIndexation );
      pAnalyseParams->SetCurrentSystemMode( ecsmLocal );

      pDictionary->GetMorphologyAnalyser()->SetAnalyseParams( pAnalyseParams );
   }

   return pDictionary;
}
///////////////////////////////////////////////////////////////////////////////

} //namespace Dictionary
} //namespace SS



