#include "stdafx.h"
#include "Dictionary.h"

namespace SS{ 
namespace Dictionary{

///////////////////////////////////////////////////////////////////////////////
// class CDictionary
///////////////////////////////////////////////////////////////////////////////
CDictionary::CDictionary():
   m_pMorphoEngDB(0), m_pMorphoRusDB(0), m_pAbbreviationEngDB(0),
   m_pAbbreviationRusDB(0), m_pNdxDB(0)
{
  
}
///////////////////////////////////////////////////////////////////////////////

ILoad* CDictionary::GetLoader()
{
	return &m_MALanguagesManager;
}
///////////////////////////////////////////////////////////////////////////////

IOneRoot* CDictionary::GetOneRoot()
{
	return &m_OneRootAdapter;
}
///////////////////////////////////////////////////////////////////////////////

IMorphoTypesCollection* CDictionary::GetMorphoTypesCollection()
{
	return &m_MorphoTypesCollectionAdapter;
}
///////////////////////////////////////////////////////////////////////////////

ISource* CDictionary::GetSource()
{
	return &m_SourceAdapter;
}
///////////////////////////////////////////////////////////////////////////////

ISourceEx* CDictionary::GetAbbreviationSource()
{
   return &m_AbbreviationSourceAdatper;
}

///////////////////////////////////////////////////////////////////////////////
IWordsList* CDictionary::GetWordList()
{
   return &m_NewWordsList;
}
///////////////////////////////////////////////////////////////////////////////

IMorphologyAnalyser* CDictionary::GetMorphologyAnalyser()
{
	return &m_MALanguagesManager;
}
///////////////////////////////////////////////////////////////////////////////

void CDictionary::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
   SS_TRY
   {
      IDictionary::SetLoadManager( pLoadManager );

      if( GetLoadManager() == 0 )
      {
         //SS_THROW( L"CDictionary::SetLoadManager уже был вызван" );
         return;
      }
      
      InitObjects( pLoadManager );
   }
   SS_CATCH(L"")
}
///////////////////////////////////////////////////////////////////////////////

void CDictionary::InitObjects( SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager )
{
   using namespace DATA_TYPE::NAMES;

   APL_ASSERT_PTR( pLoadManager );
   
   SS::Interface::Core::ResourceManagers::ILingvoBaseManager* pLingvoBaseManager = 
      (SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)
          pLoadManager->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);

   if( pLingvoBaseManager == 0 )
      SS_THROW( L"Ошибка при получении LingvoBaseManager" );
#if 1
   SS::Interface::Core::ResourceManagers::IConnectionManager *pCM = 
      (SS::Interface::Core::ResourceManagers::IConnectionManager*)
      GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_ConnectionManager, IID_ConnectionManager);

   if( pCM == 0 )
      SS_THROW(L"Ошибка получения IConnectionManager");

   pCM->Register( this );
#endif

   SS::Interface::Core::BlackBox::IBlackBoxTextStorage *pBlackBoxTextStorage =
      (SS::Interface::Core::BlackBox::IBlackBoxTextStorage *)
         pLoadManager->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxTextStorage, IID_BlackBoxTextStorage);
   
   if( pBlackBoxTextStorage == 0 )
      SS_THROW(L"Ошибка получения IBlackBoxTextStorage");


   APL_ASSERT( 
      m_pMorphoEngDB == 0 && m_pMorphoRusDB == 0 && m_pAbbreviationEngDB == 0 &&
      m_pAbbreviationRusDB == 0 
   );

   m_DBMSPtrsManager.SetLingvoBaseManager(pLingvoBaseManager);
   m_DBMSPtrsManager.Clear();
   m_NewWordsDBMSPtrsManager.SetLingvoBaseManager(pLingvoBaseManager);
   m_NewWordsDBMSPtrsManager.Clear();

   m_DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szMorphoEng, &m_pMorphoEngDB );
   m_DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szMorphoRus, &m_pMorphoRusDB );
   m_DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szAbbreviationEng, &m_pAbbreviationEngDB );
   m_DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szAbbreviationRus, &m_pAbbreviationRusDB );
   m_NewWordsDBMSPtrsManager.Register( SS::MANAGER::Constants::c_szNdxBase, &m_pNdxDB );

   //pLingvoBaseManager->CreateOpenAndRegister( SS::MANAGER::Constants::c_szMorphoEng, &m_pMorphoEngDB );

   m_DBMSPtrsManager.Open();

   //Находим путь к базе данных
   SS::Interface::Core::CommonServices::IWorkRegister * pIWorkRegister = 
      (SS::Interface::Core::CommonServices::IWorkRegister*)
      pLoadManager->GetInterface(L"./core/LoadManager.dll", CLSID_WorkRegister, IID_WorkRegister);

   if( pIWorkRegister == 0 )
      SS_THROW(L"Ошибка при получении WorkRegister");

   std::wstring wDataBasePath = 
      pIWorkRegister->GetParameter(L"PathDataBase") ;
   

   //Инициализационный код НАЧАЛО
   const Edit::CSourceAdapter::SInitStruct SourceAdapterInitData[] = {
      { m_pMorphoEngDB, ednENominalName, _T("NominalSourceForm"), _T("NominalSourceToMorpho") },
      { m_pMorphoEngDB, ednEProperName, _T("ProperSourceForm"),   _T("ProperSourceToMorpho") },
      { m_pMorphoRusDB, ednRNominalName, _T("NominalSourceForm"), _T("NominalSourceToMorpho") },
      { m_pMorphoRusDB, ednRProperName,  _T("ProperSourceForm"),  _T("ProperSourceToMorpho") }
   };

   const Edit::CAbbreviationSourceAdatper::SInitStruct AbbreviationSourceAdapterInitData[] = {
      { m_pAbbreviationEngDB, m_pMorphoEngDB, ednENominalName,     _T("NominalSingleAbbreviations") },
      { m_pAbbreviationEngDB, m_pMorphoEngDB, ednEProperName,      _T("ProperSingleAbbreviations") },
      { m_pAbbreviationEngDB, m_pMorphoEngDB, ednAbbreviationsEng, _T("MultipleAbbreviations") },
      { m_pAbbreviationEngDB, m_pMorphoEngDB, ednECombinatory,     _T("CombinatorySingleAbbreviations") },
      
      { m_pAbbreviationRusDB, m_pMorphoRusDB, ednRNominalName,     _T("NominalSingleAbbreviations") },
      { m_pAbbreviationRusDB, m_pMorphoRusDB, ednRProperName,      _T("ProperSingleAbbreviations") },
      { m_pAbbreviationRusDB, m_pMorphoRusDB, ednAbbreviationsRus, _T("MultipleAbbreviations") },
      { m_pAbbreviationRusDB, m_pMorphoRusDB, ednRCombinatory,     _T("CombinatorySingleAbbreviations") },
   };


   const Edit::COneRootAdapter::SInitStruct OneRootInitData[] = {
      { m_pMorphoEngDB, ednENominalName, _T("OneRoot") /*_T("NominalOneRoot")*/ },
      { m_pMorphoEngDB, ednEProperName,  _T("OneRoot") /*_T("ProperOneRoot")*/ },
      { m_pMorphoRusDB, ednRNominalName, _T("OneRoot") /*_T("NominalOneRoot")*/ },
      { m_pMorphoRusDB, ednRProperName,  _T("OneRoot") /*_T("ProperOneRoot")*/ },
   };

   const DictionariesNumber EngDictionariesNames[] = {
      ednENominalName, ednEProperName, ednAbbreviationsEng,
         ednECombinatory, ednUnknownEng, ednSynonimsEng
   };

   const DictionariesNumber RusDictionariesNames[] = {
      ednRNominalName, ednRProperName, ednAbbreviationsRus,
         ednRCombinatory, ednUnknownRus, ednSynonimsRus
   };

   const Edit::CMorphoTypesCollectionAdapter::SInitStruct CMorphoTypesCollectionInitData[] = {
      { m_pMorphoRusDB, Edit::CMorphoTypesCollectionAdapter::SInitStruct::TKeys(RusDictionariesNames, RusDictionariesNames + APL_ARRSIZE(RusDictionariesNames)) },
      { m_pMorphoEngDB, Edit::CMorphoTypesCollectionAdapter::SInitStruct::TKeys(EngDictionariesNames, EngDictionariesNames + APL_ARRSIZE(EngDictionariesNames)) }
   };

   const CMALanguagesManager::SLanguage MALanguagesManagerInitData[] = {
      {
         m_pMorphoEngDB,
         { wDataBasePath + L"MorphoEnglish", {ednENominalName, ednEProperName, ednUnknownEng}, CSmartDictionaryAdapter::EMinimalHypothesisTree, Constants::Dictionary::MIN_MORPHO_RECOGNIZABLE_WORD_LEN_ENG },
         { m_pAbbreviationEngDB, {
                  {ednENominalName, _T("NominalSingleAbbreviations")}, 
                  {ednEProperName, _T("ProperSingleAbbreviations")},
                  {ednAbbreviationsEng, _T("MultipleAbbreviations")},
                  {ednECombinatory, _T("CombinatorySingleAbbreviations")}
              } 
         },
         { EngDictionariesNames, EngDictionariesNames + APL_ARRSIZE(EngDictionariesNames) },
         { L"NewFormEnglish" }
      },
      {
         m_pMorphoRusDB,
         { wDataBasePath + L"MorphoRussian", {ednRNominalName, ednRProperName, ednUnknownRus}, CSmartDictionaryAdapter::EStandart, Constants::Dictionary::MIN_MORPHO_RECOGNIZABLE_WORD_LEN_RUS },
         { m_pAbbreviationRusDB, { 
                  {ednRNominalName, _T("NominalSingleAbbreviations")}, 
                  {ednRProperName, _T("ProperSingleAbbreviations")},
                  {ednAbbreviationsRus, _T("MultipleAbbreviations")},
                  {ednRCombinatory, _T("CombinatorySingleAbbreviations")}
               }
         },
         { RusDictionariesNames, RusDictionariesNames + APL_ARRSIZE(RusDictionariesNames) },
         {  L"NewFormRussian" }
      }
  };
   //Инициализационный код КОНЕЦ

   m_SourceAdapter.Init( SourceAdapterInitData, SourceAdapterInitData + APL_ARRSIZE( SourceAdapterInitData ) );
   m_AbbreviationSourceAdatper.Init( AbbreviationSourceAdapterInitData, AbbreviationSourceAdapterInitData + APL_ARRSIZE( AbbreviationSourceAdapterInitData ) );
   m_OneRootAdapter.Init( OneRootInitData, OneRootInitData + APL_ARRSIZE( OneRootInitData ) );
   m_MorphoTypesCollectionAdapter.Init( CMorphoTypesCollectionInitData, CMorphoTypesCollectionInitData + APL_ARRSIZE(CMorphoTypesCollectionInitData) );

   SS::Interface::Core::AMConverter::IAMConverterMorpho  *pAMConverter = 
      (SS::Interface::Core::AMConverter::IAMConverterMorpho*)
      pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);

   if( pAMConverter == 0 )
      SS_THROW(L"Ошибка при получении AMConverterMorpho");

   m_MALanguagesManager.Init( MALanguagesManagerInitData, MALanguagesManagerInitData + APL_ARRSIZE(MALanguagesManagerInitData), pAMConverter, &m_OneRootAdapter, pBlackBoxTextStorage );

   //Возможно база новых слов уже быта открыта
   const wchar_t *NameActiveStorage = pLingvoBaseManager->GetNameActiveStorage();
   bool InitNdxTables = NameActiveStorage != NULL && *NameActiveStorage != L'\0';

   if( InitNdxTables )
      Open();
}
///////////////////////////////////////////////////////////////////////////////

bool CDictionary::Open()
{
   using namespace DATA_TYPE::NAMES;
   
   SS_TRY
   {  
      APL_ASSERT( GetLoadManager() );
      
      SS::Interface::Core::ResourceManagers::ILingvoBaseManager* pLingvoBaseManager = 
         (SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)
         GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);
      
      const wchar_t *NameActiveStorage = pLingvoBaseManager->GetNameActiveStorage();
      bool InitNdxTables = NameActiveStorage != NULL && *NameActiveStorage != L'\0';

      if( !InitNdxTables )
         SS_THROW( L"База новых слов не была открыта" );

      m_NewWordsDBMSPtrsManager.Open();

      m_MALanguagesManager.OpenNewWordsTables( m_pNdxDB );

      const CNewWordsList::SInitStruct NewWordsListInitData[] = {
         { m_pNdxDB, m_pMorphoEngDB, ednUnknownEng, _T("NewFormEnglish") },
         { m_pNdxDB, m_pMorphoRusDB, ednUnknownRus, _T("NewFormRussian") },
      };

      m_NewWordsList.Init( NewWordsListInitData, NewWordsListInitData + APL_ARRSIZE( NewWordsListInitData ) );

      return true;
   }
   SS_CATCH(L"")

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CDictionary::Close()
{
   SS_TRY
   {
      m_MALanguagesManager.CloseNewWordsTables(); 
      m_NewWordsList.CloseTables();
      m_NewWordsDBMSPtrsManager.Close();
                                              
      return true;
   }
   SS_CATCH(L"")

   return false;
}
///////////////////////////////////////////////////////////////////////////////

void CDictionary::SetAnalyseParams( SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams )
{
   SS_TRY
   {
      IDictionary::SetAnalyseParams(pAnalyseParams);
      m_MALanguagesManager.SetAnalyseParams(pAnalyseParams);
   }
   SS_CATCH(L"")

}


} //namespace Dictionary
} //namespace SS
