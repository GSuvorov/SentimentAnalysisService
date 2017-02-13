#include "stdafx.h"
#ifdef _SS_UNITTESTS

#include "PerfomanceTestRunner.h"
#include "../[libs]/TestHelper.h"
#include "TestAuxiliary.h"
#include "../[libs]/Progress.hpp"
#include "../[libs]/CoutConvert.h"

namespace SS{ 
namespace UnitTests{ 
namespace Dictionary{
using namespace std;


///////////////////////////////////////////////////////////////////////////////
// class CTestPerpformance
///////////////////////////////////////////////////////////////////////////////
CTestPerpformance::CTestPerpformance(ILoadManager *pLoadManager): 
   m_pLoadManager(pLoadManager)
{
   APL_ERROR(m_pLoadManager != 0);
   
   DBMS::IDBManager *pDBManager = (DBMS::IDBManager *)m_pLoadManager->GetInterface(L"./core/linguistics/DBMS.dll" , CLSID_DBMS, IID_DBManager);
   APL_ERROR( pDBManager != 0 );

   APL_ERROR( pDBManager->Init(L"..\\..\\..\\..\\Database\\Ndx_dbms.ini", L"..\\..\\..\\..\\Database\\") == S_OK );
   APL_ERROR( pDBManager->MoveToDataBase(L"DBMS") == S_OK );

   m_pDataBase = pDBManager->OpenDataBase();
   APL_ERROR( m_pDataBase != 0 );
}
///////////////////////////////////////////////////////////////////////////////

CTestPerpformance::~CTestPerpformance()
{
   m_pDataBase->Close();
}
///////////////////////////////////////////////////////////////////////////////

void CTestPerpformance::InitNDXDataTable( CTablePtr &pTable )
{
   APL_ERROR( pTable.Open(m_pDataBase,  L"NewFormRussian") == S_OK );
}
///////////////////////////////////////////////////////////////////////////////

void CTestPerpformance::FillNdxDBMS( const wstring &Dir, const wstring *ExtFirst, const wstring *ExtLast )
{
   CTablePtr pTable;
    
   InitNDXDataTable(pTable);
   pTable->Clear();

   INT32 dbIDSource = 0;
   wstring dbSource = L"Тест!";
   wstring dbSourceLower;
   INT32 dbIDType = 1;

   void *RecMap[4] = {&dbIDSource, &dbSource,  &dbSourceLower, &dbIDType};

   APL_ERROR( pTable->AddInit(RecMap) == S_OK );
   CFindHelper FindHelper( pTable );
   
   APL_ERROR( FindHelper.Init( 2, &dbSourceLower, RecMap ) == S_OK );

   CDirectoryFileList DirectoryFileList( Dir , ExtFirst, ExtLast );
   CFileWordReader FileWordReader;
   unsigned int WordsCount = 0;
   unsigned int UniqueWordsCount = 0;
   unsigned int FilesCount = 0;
   wstring Str;
   string Buf;

   cout << "\nЗаполенние DBMS..." << endl;
   ofstream flWordTest( "test.txt" );

   TTimer Timer;
   while( DirectoryFileList.Next() )
   {
      APL_ERROR( FileWordReader.OpenFile(Convert(DirectoryFileList.GetDir() + DirectoryFileList.GetFile(), string()).c_str()) );
           
      while( FileWordReader.Get(Buf) )
      {
         Convert( Buf, dbSource );
         ToLower( dbSource, dbSourceLower );

         if( FindHelper.FindOne() != S_OK )
         {
            //flWordTest << Buf.c_str();
            //flWordTest << endl;

            APL_ERROR( pTable->Add() == S_OK );
            ++UniqueWordsCount;
         }

         ++WordsCount;
      }

      ++FilesCount;
   }

   Timer.Stop();

   cout << "Было обработано:" << endl;
   cout << "Файлов: " << FilesCount << endl;
   cout << "Слов: " << WordsCount << endl;
   cout << "Из них уникальных : " << UniqueWordsCount << " (" << ((double)UniqueWordsCount / WordsCount) * 100.0 << "%)" << endl;
   cout << "Время : " << Timer << endl;
}
///////////////////////////////////////////////////////////////////////////////

void CTestPerpformance::FindNdxInDBMS( const std::wstring &Dir, const wstring *ExtFirst, const wstring *ExtLast )
{
   CTablePtr pTable;

   InitNDXDataTable(pTable);

   INT32 dbIDSource = 0;
   wstring dbSource = L"Тест!";
   wstring dbSourceLower;
   INT32 dbIDType = 1;

   void *RecMap[4] = {&dbIDSource, &dbSource,  &dbSourceLower, &dbIDType};

   APL_ERROR( pTable->AddInit(RecMap) == S_OK );
   CFindHelper FindHelper( pTable );

   APL_ERROR( FindHelper.Init( 2, &dbSourceLower, RecMap ) == S_OK );

   CDirectoryFileList DirectoryFileList( Dir , ExtFirst, ExtLast );
   CFileWordReader FileWordReader;
   unsigned int WordsCount = 0;
   unsigned int FilesCount = 0;
   wstring Str;
   string Buf;

   cout << "\nПоиск в таблице DBMS..." << endl;
   ofstream flWordTest( "test.txt" );
   bool IsFind;

   TTimer Timer;
   while( DirectoryFileList.Next() )
   {
      APL_ERROR( FileWordReader.OpenFile(Convert(DirectoryFileList.GetDir() + DirectoryFileList.GetFile(), string()).c_str()) );

      while( FileWordReader.Get(Buf) )
      {
         Convert( Buf, dbSource );
         ToLower( dbSource, dbSourceLower );
         
         IsFind = false;

         FindHelper.FindStop();

         while( FindHelper.Find() == S_OK ) IsFind = true;

         APL_ERROR( IsFind );

         ++WordsCount;
      }

      ++FilesCount;
   }

   Timer.Stop();

   cout << "Было обработано:" << endl;
   cout << "Файлов: " << FilesCount << endl;
   cout << "Слов: " << WordsCount << endl;
   cout << "Время : " << Timer << endl;
}
///////////////////////////////////////////////////////////////////////////////

void CTestPerpformance::FindDummy( const std::wstring &Dir, const wstring *ExtFirst, const wstring *ExtLast )
{
   CTablePtr pTable;

   InitNDXDataTable(pTable);

   INT32 dbIDSource = 0;
   wstring dbSource = L"Тест!";
   wstring dbSourceLower;
   INT32 dbIDType = 1;

   void *RecMap[4] = {&dbIDSource, &dbSource,  &dbSourceLower, &dbIDType};

   APL_ERROR( pTable->AddInit(RecMap) == S_OK );
   CFindHelper FindHelper( pTable );

   APL_ERROR( FindHelper.Init( 2, &dbSourceLower, RecMap ) == S_OK );

   CDirectoryFileList DirectoryFileList( Dir , ExtFirst, ExtLast );
   CFileWordReader FileWordReader;
   unsigned int WordsCount = 0;
   unsigned int FilesCount = 0;
   wstring Str;
   string Buf;

   cout << "\nПросто считываение всех слов..." << endl;
   ofstream flWordTest( "test.txt" );

   TTimer Timer;
   while( DirectoryFileList.Next() )
   {
      APL_ERROR( FileWordReader.OpenFile(Convert(DirectoryFileList.GetDir() + DirectoryFileList.GetFile(), string()).c_str()) );

      while( FileWordReader.Get(Buf) )
      {
         Convert( Buf, dbSource );
         ToLower( dbSource, dbSourceLower );

         ++WordsCount;
      }

      ++FilesCount;
   }

   Timer.Stop();

   cout << "Было обработано:" << endl;
   cout << "Файлов: " << FilesCount << endl;
   cout << "Слов: " << WordsCount << endl;
   cout << "Время : " << Timer << endl;
}
///////////////////////////////////////////////////////////////////////////////

void CTestPerpformance::FindNdxInCWewWordsCache( const std::wstring &Dir, const wstring *ExtFirst, const wstring *ExtLast )
{
   CTablePtr pTable;

   InitNDXDataTable(pTable);

   INT32 dbIDSource = 0;
   wstring dbSource = L"Тест!";
   wstring dbSourceLower;
   INT32 dbIDType = 1;

   void *RecMap[4] = {&dbIDSource, &dbSource,  &dbSourceLower, &dbIDType};

   APL_ERROR( pTable->AddInit(RecMap) == S_OK );
   CFindHelper FindHelper( pTable );

   APL_ERROR( FindHelper.Init( 2, &dbSourceLower, RecMap ) == S_OK );


   CDirectoryFileList DirectoryFileList( Dir, ExtFirst, ExtLast );
   CFileWordReader FileWordReader;
   unsigned int WordsCount = 0;
   unsigned int FilesCount = 0;
   wstring Str;
   string Buf;
   Detail::CNewWordsCache NewWordsCache( pTable.Get(), DATA_TYPE::NAMES::ednUnknown);

   cout << "\nПоиск в таблице DBMS c приминением кэша CWewWordsCache..." << endl;
   ofstream flWordTest( "test.txt" );

   TTimer Timer;
   while( DirectoryFileList.Next() )
   {
      APL_ERROR( FileWordReader.OpenFile(Convert(DirectoryFileList.GetDir() + DirectoryFileList.GetFile(), string()).c_str()) );

      while( FileWordReader.Get(Buf) )
      {
         Convert( Buf, dbSource );
         ToLower( dbSource, dbSourceLower );

         APL_ERROR( !NewWordsCache.FindBySourceLower(dbSourceLower).End() );

         ++WordsCount;
      }

      ++FilesCount;
   }

   Timer.Stop();

   cout << "Было обработано:" << endl;
   cout << "Файлов: " << FilesCount << endl;
   cout << "Слов: " << WordsCount << endl;
   cout << "Время : " << Timer << endl;
}
///////////////////////////////////////////////////////////////////////////////



void CTestPerpformance::LoadToMemoryFileText( const std::string &FileName, TStringsForTest &Buf )
{
   CFileWordReader FileWordReader;

   APL_ERROR( FileWordReader.OpenFile(FileName.c_str()) );

   Buf.clear();

   cout << "Загрузка файла: " << FileName << endl;
   
   TTimer LoadFileTime;

#if 0 //Файлы не Юникод
   string CurString;
   CFileProgres<> PD( FileWordReader.GetStream() );

   while( FileWordReader.Get(CurString) )
   {
      if( CurString.size() >= Constants::MAX_CHARS_IN_WORD )
         CurString.resize( Constants::MAX_CHARS_IN_WORD - 1);
      
      Buf.push_back(TStringsForTest::value_type());
      Convert( CurString, Buf.back() );
      ToLower( Buf.back(), Buf.back() );

      PD.Update();
   }

   PD.EndTask();
#else //Файлы либо Unicode либо нет
   wstring CurString;
   CUnicodeFileReader UnicodeFileReader;

   APL_ERROR( UnicodeFileReader.Open(Convert(FileName, wstring()).c_str()) == CUnicodeFileReader::EOROk );

   while( UnicodeFileReader.Read(CurString, CUnicodeFileReader::SNotAlphaNumericCharacter()) )
   {
      if( CurString.empty() )
         continue;
  
      if( CurString.size() >= Constants::MAX_CHARS_IN_WORD )
         CurString.resize( Constants::MAX_CHARS_IN_WORD - 1);
      
      Buf.push_back(CurString);
      ToLower( Buf.back(), Buf.back() );
   }
#endif

   //std::copy( Buf.begin(), Buf.end(), ostream_iterator<wstring>( cout, "\n" ) );
   //for( TStringsForTest::iterator I = Buf.begin(); I != Buf.end(); ++I ) {cout << *I << "\n"; }

   LoadFileTime.Stop();

   cout << endl << "Слов: " << (unsigned int)Buf.size() << " Время загрузки: " << LoadFileTime << endl;
}
///////////////////////////////////////////////////////////////////////////////

void CTestPerpformance::LoadToMemoryFileStrings( const std::string &FileName, TStringsForTest &Buf )
{
   string CurLine;
   string CurString;

   Buf.clear();
   ifstream flIn( FileName.c_str() );

   APL_ERROR( flIn );

   cout << "Загрузка файла: " << FileName << endl;
   
   CFileProgres<> PD( flIn );

   TTimer LoadFileTime;
   while( getline(flIn, CurLine) && !CurLine.empty() )
   {
      stringstream Stream( CurLine );
      Stream >> CurString;

      APL_ERROR( CurString.size() < Constants::MAX_CHARS_IN_WORD );

      Buf.push_back(TStringsForTest::value_type());
      Convert( CurString, Buf.back() );

      PD.Update();
   }

   LoadFileTime.Stop();
   PD.EndTask();

   cout << endl << "Слов: " << (unsigned int)Buf.size() << " Время загрузки: " << LoadFileTime << endl;
}
///////////////////////////////////////////////////////////////////////////////

void CTestPerpformance::TestIDictionaryTreeImpl( IDictionary *pDictionary, const TStringsForTest &StringsForTest )
{
   APL_ERROR(pDictionary != 0);

   CMALanguagesManager *pMA = APL_DYNAMIC_CAST( CMALanguagesManager *, pDictionary->GetMorphologyAnalyser() );
   CSmartDictionary *pSD = pMA->GetSmartDictionary( InitData::Languages::English );
   IAMConverterMorpho *pAMConverter = pSD->GetAMConverter();

   APL_ERROR( pAMConverter );

   CPartOfWordsSearchCreateManager::TPointer pPartOfWordsSearch = pSD->GetEndingWS();

   typedef std::list<SS::Dictionary::Types::TWordInfo> TWIList;
   TWIList DictData;
   std::vector<SPartOfWord> POFVector;

   unsigned int FindWords;
   const unsigned int TotalWords = (unsigned int)StringsForTest.size();

   SS::Interface::Core::BlackBox::IBlackBoxTextStorage *pBlackBoxTextStorage =
      (SS::Interface::Core::BlackBox::IBlackBoxTextStorage *)
      pDictionary->GetLoadManager()->GetInterface(L"./core/linguistics/BlackBox.dll", CLSID_BlackBoxTextStorage, IID_BlackBoxTextStorage);


   pDictionary->GetMorphologyAnalyser()->NextText();

   APL_ERROR( SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS) );

   if( false )
   {
      cout << "Очищаем базу новых слов" << endl;

      SS::Interface::Core::ResourceManagers::IBaseManager* pBaseManager;

      pBaseManager = (SS::Interface::Core::ResourceManagers::IBaseManager*)
         m_pLoadManager->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_BaseManager);

      APL_ERROR(pBaseManager != 0);

      APL_ERROR( pBaseManager->ClearBase() == S_OK );
      
      pBaseManager->OpenBase( L"NewWordForms1" );
   }

   if( false )
   {
      typedef std::map<std::wstring, int> TStringMap;
      TStringMap StringMap;
      cout << "Скорость работы map: ";

      FindWords = 0;
      int i = 0;
      TTimer Timer;
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         StringMap.insert( std::make_pair(*I, i++) );
         //DEBUG_MSG( "Длина: " << (unsigned)I->length() << " = " << Convert(*I, std::string()) );
      }
      
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         if( StringMap.find( *I ) != StringMap.end() )
            ++FindWords;
      }

      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }

   if( false )
   {
      typedef stdext::hash_map<std::wstring, int> TStringMap;
      TStringMap StringMap;
      cout << "Скорость работы hash_map: ";

      FindWords = 0;
      int i = 0;
      TTimer Timer;
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         StringMap.insert( std::make_pair(*I, i++) );
      }

      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         if( StringMap.find( *I ) != StringMap.end() )
            ++FindWords;
      }

      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }


   if( false )
   {
      cout << "CMALanguagesManager::GetWordInfo(..., efzBaseDictionary): ";
      FindWords = 0;
      TTimer Timer;
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         DictData.clear();
         //try
         //{
            if( pDictionary->GetMorphologyAnalyser()->GetWordInfo(I->c_str(), &DictData,  efzBaseDictionary) )
               ++FindWords;
         //}
         //catch(...)
         //{
         //   cout << endl << APL_LINE << *I << endl << APL_LINE;

         //   throw;
         //}
      }
      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }

   if( false )
   {
      pDictionary->GetMorphologyAnalyser()->NextText();

      cout << "CMALanguagesManager::GetWordInfo(..., efzOnlySearch): ";
      FindWords = 0;
      //unsigned TotalSize = 0;
      TTimer Timer;
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         DictData.clear();
         if( pDictionary->GetMorphologyAnalyser()->GetWordInfo(I->c_str(), &DictData,  efzOnlySearch) )
         {
            //TotalSize += DictData.size();
            ++FindWords;
         }
         //else
         //   cout << *I << endl;
      }
      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
      //cout << 
      //   "SuccessfulFillWordInfo / TotalFillWordInfo " <<  SSDStatistic::SuccessfulFillWordInfo << "/" << SSDStatistic::TotalFillWordInfo << 
      //   " = " << (double)SSDStatistic::SuccessfulFillWordInfo / SSDStatistic::TotalFillWordInfo * 100. << "% " << 
      //   " TotalSize = " << TotalSize << " TotalSize / FindWords  = " << (double)TotalSize / FindWords <<
      //   endl;
   }

   if( false )
   {
      cout << "CMALanguagesManager::GetWordInfo(..., efzAbbreviation): ";

      FindWords = 0;
      TTimer Timer;
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         DictData.clear();
         if( pDictionary->GetMorphologyAnalyser()->GetWordInfo(I->c_str(), &DictData,  efzAbbreviation) )
         {
            ++FindWords;
         }

      }
      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }

   if( false )
   {
      cout << "CMALanguagesManager::GetWordInfo(..., efzBaseAndAbbreviation): ";

      FindWords = 0;
      TTimer Timer;
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         DictData.clear();
         if( pDictionary->GetMorphologyAnalyser()->GetWordInfo(I->c_str(), &DictData,  efzBaseAndAbbreviation) )
         {
            ++FindWords;
         }
      }
      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }

   if( true )
   {
      pDictionary->GetMorphologyAnalyser()->NextText();

      cout << "CMALanguagesManager::GetWordInfo(..., efzFullAddNewWords): ";
      FindWords = 0;
      TTimer Timer;
      //unsigned TotalSize = 0;

      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {    
         DictData.clear();
         if( pDictionary->GetMorphologyAnalyser()->GetWordInfo(I->c_str(), &DictData,  efzFullAddNewWords) )
         {
            //TotalSize += DictData.size();
            ++FindWords;
         }
      }
      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }

   if( false )
   {
      pDictionary->GetMorphologyAnalyser()->NextText();

      cout << "CMALanguagesManager::GetWordInfo(..., efzFullAddNewWordsCompound): ";
      FindWords = 0;
      TTimer Timer;
      //unsigned TotalSize = 0;

      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {    
         DictData.clear();
         if( pDictionary->GetMorphologyAnalyser()->GetWordInfo(I->c_str(), &DictData,  efzFullAddNewWordsCompound) )
         {
            //TotalSize += DictData.size();
            ++FindWords;
         }
      }
      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }


   if( false )
   {
      pDictionary->GetMorphologyAnalyser()->NextText();

      cout << "CMALanguagesManager::GetWordInfo(IUnit, efzFullAddNewWordsCompound) создание 50000 IUnit: ";
      FindWords = 0;
      IText *pText = pBlackBoxTextStorage->CreateText();
      IParagraph *pParagraph = 0;
      ISentence *pSentence = 0;
      IUnit *pUnit = 0;
      const size_t MaxUnitCount = 50000;
      size_t UnitCount = MaxUnitCount;

      TTimer Timer;

      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {    
         if( UnitCount >= MaxUnitCount )
         {
            pText->ClearText();
            pParagraph = pBlackBoxTextStorage->CreateParagraph();
            pSentence = pBlackBoxTextStorage->CreateSentence(); 
            pParagraph->AddSentence( pSentence );
            pText->AddParagraph( pParagraph );

            UnitCount = 0;
         }
         else
            ++UnitCount;
         
         pUnit = pBlackBoxTextStorage->CreateUnit();
         pSentence->AddUnit( pUnit );

         pUnit->SetWord( I->c_str() );
//         if( pDictionary->GetMorphologyAnalyser()->GetWordInfo(I->c_str(), pUnit,  efzFullAddNewWordsCompound) )
         {
            //TotalSize += DictData.size();
            ++FindWords;
         }
      }
      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }

   if( false )
   {
      pDictionary->GetMorphologyAnalyser()->NextText();

      cout << "CMALanguagesManager::GetWordInfo(IUnit, efzFullAddNewWordsCompound) использование одного IUnit: ";
      FindWords = 0;
      IUnit *pUnit = pBlackBoxTextStorage->CreateUnit();
      IIndex *pIndex = 0;

      TTimer Timer;

      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {    
         pIndex = pUnit->GetFirstIndex();

         for(;pIndex != 0; pIndex = pUnit->GetFirstIndex())
            pIndex->ReleaseIndex();

         pUnit->ClearUnit();

         pUnit->SetWord( I->c_str() );
//         if( pDictionary->GetMorphologyAnalyser()->GetWordInfo(I->c_str(), pUnit,  efzFullAddNewWordsCompound) )
         {
            //TotalSize += DictData.size();
            ++FindWords;
         }
      }
      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }


   if( false )
   {
      pDictionary->GetMorphologyAnalyser()->NextText();

      cout << "CMALanguagesManager::GetWord(...): ";
      typedef std::list<SWordIndex> TWordIndexList;
      TWordIndexList TestList;

      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         DictData.clear();
         if( pDictionary->GetMorphologyAnalyser()->GetWordInfo(I->c_str(), &DictData,  efzFullAddNewWords) )
         {
            for( TWIList::iterator J = DictData.begin(); J != DictData.end(); ++J )
               for( TWordInfo::iterator K = J->begin(); K != J->end(); ++K )
                  TestList.push_back( K->m_WordIndex );
         }
      }

      WCHAR wString[SS::Constants::MAX_CHARS_IN_WORD];
      FindWords = 0;
      TTimer Timer;

      for( TWordIndexList::iterator I = TestList.begin(); I != TestList.end(); ++I )
      {    
         if( pDictionary->GetMorphologyAnalyser()->GetWord(&*I, wString) )
         {
            //TotalSize += DictData.size();
            ++FindWords;
         }
      }
      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }
        
   if( false )
   {
      cout << "CMALanguagesManager::IsExistWord(...): ";
      FindWords = 0;
      TTimer Timer;
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         if( pDictionary->GetMorphologyAnalyser()->IsExistWord(I->c_str()) )
            ++FindWords;
      }
      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }

   if( false )
   {
      cout << "CPartOfWordsSearch::GetByForm(..., FALSE): ";
      TTimer Timer;
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         POFVector.clear();
         pPartOfWordsSearch->GetByForm(I->c_str(), INT(I->size()- 1), &POFVector, FALSE);
      }
      Timer.Stop();
      cout << Timer << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }
  
   if( false )
   {
      cout << "CSmartDictionary::IsExistWord(...): ";
      FindWords = 0;
      TTimer Timer;
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         if( pSD->IsExistWord(I->c_str()) )
            ++FindWords;
      }
      Timer.Stop();
      cout << Timer << " совпадений: " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }

   if( false )
   {
      cout << "CSmartDictionary::DeterminationWordInfo(...): ";
      FindWords = 0;
      CSmartDictionary::SHypothesisInfo HypothesisInfo;
      TStandartDeterminationWordCheck SDWC;

      TTimer Timer;
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         if( pSD->DeterminationWordInfo(I->c_str(), I->c_str(), false, HypothesisInfo, SDWC ) )
            ++FindWords;
      }
      Timer.Stop();
      cout << Timer << " совпадений: " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }

   if( false )
   {
      cout << "CMALanguagesManager::GetDeterminationWordInfo(...): ";
      FindWords = 0;
      CSmartDictionary::SHypothesisInfo HypothesisInfo;
      TTimer Timer;
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         if( pMA->GetDeterminationWordInfo(I->c_str(), HypothesisInfo ) )
            ++FindWords;
      }
      Timer.Stop();
      cout << Timer << " совпадений: " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }


   if( false )
   {
      cout << "CSmartDictionary::GetWordInfo(..., true, false) Только дерево: ";
      FindWords = 0;
      TTimer Timer;
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         DictData.clear();
         if( pSD->GetWordInfo(I->c_str(), I->c_str(), false, &DictData, true, false) )
            ++FindWords;
      }
      Timer.Stop();
      cout << Timer << " совпадений " << FindWords << "/" << TotalWords<< " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }

   if( false )
   {
      cout << "CSmartDictionary::GetWordInfo(..., true, true) Новые слова и дерево: ";
      FindWords = 0;
      TTimer Timer;
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         DictData.clear();
         if( pSD->GetWordInfo(I->c_str(), I->c_str(), false, &DictData, true, true) )
            ++FindWords;
      }
      Timer.Stop();
      cout << Timer << " совпадений " << FindWords << "/" << TotalWords<< " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }

   if( false )
   {
      cout << "ToLower + SymbolCharacteristic: ";
      TCHAR LowerBuf[Constants::MAX_CHARS_IN_WORD];
      CSymbolCharacteristicCreateManager::TConstPointer pSC = CSymbolCharacteristicCreateManager::Create();

      TTimer Timer;
      const wchar_t *wcWord;
      size_t Len;
      unsigned int n = 0;

      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {
         wcWord = I->c_str();
         Len = std::wcslen( wcWord );

         std::memcpy( LowerBuf, wcWord, (Len + 1) * sizeof(TCHAR) ); 
         CharLowerBuff( LowerBuf, static_cast<DWORD>(Len) );

         typedef CSymbolCharacteristicCreateManager::TCreateType::TItem TDictsCharacteristic;
         TDictsCharacteristic DictsCharacteristic(pSC->GetFlags( wcWord ));

         size_t i, j;

         for( i = 0; i < InitData::Languages::COUNT; ++i )       
            for( j = 0; j < InitData::LangDepDict::COUNT; ++j )            
               if( DictsCharacteristic.test( InitData::Convert(static_cast<InitData::Languages::eLanguages>(i), static_cast<InitData::LangDepDict::eLangDepDict>(j) )) )
                  ++n;
              
         for( i = InitData::FirstUndepDict(); i < InitData::ObjectID::DICTS_COUNT; ++i )
            if( DictsCharacteristic.test(i) )
               ++n;
      }

      Timer.Stop();
      cout << Timer << " скорость " << (double)TotalWords / (Timer.InSec()) << " tmp "  << n << endl;
   }

   if( false )
   {   
      pDictionary->GetMorphologyAnalyser()->NextText();

      cout << "Подготовка CMALanguagesManager::GetWordInfo(..., efzFullAddNewWords): ";
      typedef std::list<TWIList> TAllMorphoData;
      TAllMorphoData AllMorphoData;

      FindWords = 0;
      SWordIndex WordIndex;
      TTimer Timer;
      //unsigned TotalSize = 0;

      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {    
         AllMorphoData.push_back( TAllMorphoData::value_type() );

         if( pDictionary->GetMorphologyAnalyser()->GetWordInfo(I->c_str(), &AllMorphoData.back(),  efzFullAddNewWords) )
         {
            //TotalSize += DictData.size();
            ++FindWords;
         }
      }
      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;

      TTimer FullTimer;
      
      for( TAllMorphoData::iterator I = AllMorphoData.begin(); I != AllMorphoData.end(); ++I )
         DictData = *I;
      
      FullTimer.Stop();

      TTimer ShortTimer;

      for( TAllMorphoData::iterator I = AllMorphoData.begin(); I != AllMorphoData.end(); ++I )
         for( TWIList::iterator J = I->begin(); J != I->end(); ++J )
            for( TWordInfo::iterator K = J->begin(); K != J->end(); ++K )
               WordIndex = K->m_WordIndex;

      ShortTimer.Stop();

      cout << "Время на копирование TWIList: "<< FullTimer << " скорость " << (double)TotalWords / (FullTimer.InSec()) << endl;
      cout << "Время на копирование SWordIndex: "<< ShortTimer << " скорость " << (double)TotalWords / (ShortTimer.InSec()) << endl;
   }

   if( false )
   {   
      pDictionary->GetMorphologyAnalyser()->NextText();

      cout << "Подготовка CMALanguagesManager::GetWordInfo(..., efzFullAddNewWords): ";
      typedef std::list<TWIList> TAllMorphoData;
      TWIList TmpListList, TmpListListAssign;
      SS::Dictionary::Types::TWordInfo TmpList;
      std::vector<SS::Dictionary::Types::SWordInfo> TmpVector;
      TAllMorphoData::iterator I;
      TWIList::iterator J;
      TWordInfo::iterator K;
      size_t DataLength = 0;

      TmpVector.reserve(10);

      TAllMorphoData AllMorphoData;

      FindWords = 0;
      SWordIndex WordIndex;
      TTimer Timer;
      //unsigned TotalSize = 0;

      for( TStringsForTest::const_iterator IString = StringsForTest.begin(); IString !=  StringsForTest.end(); ++IString )
      {    
         AllMorphoData.push_back( TAllMorphoData::value_type() );

         if( pDictionary->GetMorphologyAnalyser()->GetWordInfo(IString->c_str(), &AllMorphoData.back(),  efzFullAddNewWords) )
         {
            //TotalSize += DictData.size();
            ++FindWords;
         }
      }

      for( I = AllMorphoData.begin(); I != AllMorphoData.end(); ++I )
      {
         for( J = I->begin(); J != I->end(); ++J )
            DataLength += J->size();
      }

      Timer.Stop();

      cout << Timer << " совпадений " << FindWords << "/" << TotalWords << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;

      TTimer ListListAssignTimer;

      for( I = AllMorphoData.begin(); I != AllMorphoData.end(); ++I )
      {  
         TmpListListAssign = *I;
      }

      ListListAssignTimer.Stop();
    
      

      TTimer ListListTimer;

      for( I = AllMorphoData.begin(); I != AllMorphoData.end(); ++I )
      {  
         TmpListList.clear();
         
         for( J = I->begin(); J != I->end(); ++J )
         {
            TmpListList.push_back( TWIList::value_type() );

            for( K = J->begin(); K != J->end(); ++K )
               TmpListList.back().push_back(*K);
         }
      }

      ListListTimer.Stop();

      TTimer TmpVectorTimer;

      for( I = AllMorphoData.begin(); I != AllMorphoData.end(); ++I )
      {
         //TmpVector.clear();
         TmpVector.resize(0);

         for( J = I->begin(); J != I->end(); ++J )
            for( K = J->begin(); K != J->end(); ++K )
               TmpVector.push_back(*K);
      }

      TmpVectorTimer.Stop();


      TTimer ListTimer;

      for( I = AllMorphoData.begin(); I != AllMorphoData.end(); ++I )
      {
         TmpList.clear();
         for( J = I->begin(); J != I->end(); ++J )
            for( K = J->begin(); K != J->end(); ++K )
               TmpList.push_back(*K);
      }
 
       ListTimer.Stop();

       TTimer AMConverterTimer;

       for( I = AllMorphoData.begin(); I != AllMorphoData.end(); ++I )
       {
          for( J = I->begin(); J != I->end(); ++J )
             for( K = J->begin(); K != J->end(); ++K )
             {
               CMorphoFeature MF;
               pAMConverter->EnCode( K->m_WordIndex.m_DictIndex.GetDictionaryNumber(), K->m_MorphoTypeInfo.m_MorphoInfo, K->m_SpecialMorpho, &MF );
             }               
       }

       AMConverterTimer.Stop();

      cout << "Время на копирование списка списков оператором =: "<< ListListAssignTimer << " скорость " << (double)TotalWords / (ListListAssignTimer.InSec()) << endl;
      cout << "Время на копирование списка списков: "<< ListListTimer << " скорость " << (double)TotalWords / (ListListTimer.InSec()) << endl;
      cout << "Время на копирование списка: "<< ListTimer << " скорость " << (double)TotalWords / (ListTimer.InSec()) << endl;
      cout << "Время на копирование вектора (capacity = " << (UINT)TmpVector.capacity() << " ): " << TmpVectorTimer << " скорость " << (double)TotalWords / (TmpVectorTimer.InSec()) << endl;
      cout << "Время на получение CMorphoFeature каждого элемента при помощи AMConverter'а: " << AMConverterTimer << " скорость " << (double)DataLength / (AMConverterTimer.InSec()) << " Всего эелементов: " << (unsigned int)DataLength << endl;
   }


   if( false )
   {   
      cout << "Подготовка CMALanguagesManager::SCurWordInfo::Fill: ";
      
      TTimer Timer;

      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {    
         pMA->GetCurWordInfo().Fill(I->c_str());
      }
      Timer.Stop();

      cout << Timer << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
      
      cout << "Подготовка CMALanguagesManager::SCurWordInfo::Fill_NEW: ";

      Timer.Start();
      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {    
         pMA->GetCurWordInfo().Fill_NEW(I->c_str());
      }
      Timer.Stop();

      cout << Timer << " скорость " << (double)TotalWords / (Timer.InSec()) << endl;
   }

   TTimer Delta;
   size_t TotalLen = 0;
   for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      TotalLen += I->size();
   Delta.Stop();

   cout      << "Обработано слов:       " << TotalWords << endl;
   cout      << "Погрешность: " <<  Delta.InSec() << " Макс скорость: " << (double)TotalWords / Delta.InSec() << endl;

   APL_ERROR( SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS) );

   cout << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestPerpformance::TestIDictionaryTree(const std::wstring &Dir, const std::wstring *ExtFirst, const std::wstring *ExtLast )
{
   cout << APL_LINE << "BEGIN TestIDictionaryTree " << GetTimeStampString() << endl;

   cout << "Попробуем взять IDictionary" << endl;
   IDictionary *pDictionary = GetIDictionary(m_pLoadManager);

   APL_ERROR(pDictionary != 0);

   CDirectoryFileList DirectoryFileList( Dir , ExtFirst, ExtLast );
   TStringsForTest StringsForTest;

   cout << "Начинаем тестирование" << endl;

   while( DirectoryFileList.Next() )
   {
      LoadToMemoryFileStrings(Convert(DirectoryFileList.GetDir() +  DirectoryFileList.GetFile(), string()), StringsForTest);
      TestIDictionaryTreeImpl(pDictionary, StringsForTest );
   }

   cout << "END TestIDictionaryTree " << GetTimeStampString() << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestPerpformance::TestIDictionaryTreeFromText(const std::wstring &Dir, const std::wstring *ExtFirst, const std::wstring *ExtLast )
{
   cout << APL_LINE << "BEGIN TestIDictionaryTree " << GetTimeStampString() << endl;

   cout << "Попробуем взять IDictionary" << endl;
   IDictionary *pDictionary = GetIDictionary(m_pLoadManager);

   APL_ERROR(pDictionary != 0);

   CDirectoryFileList DirectoryFileList( Dir, ExtFirst, ExtLast );
   TStringsForTest StringsForTest;

   cout << "Начинаем тестирование" << endl;

   while( DirectoryFileList.Next() )
   {
      //OutputDebugString( DirectoryFileList.GetFile().c_str() );
      
      LoadToMemoryFileText(Convert(DirectoryFileList.GetDir() +  DirectoryFileList.GetFile(), string()), StringsForTest);
      TestIDictionaryTreeImpl(pDictionary, StringsForTest );
   }

   cout << "END TestIDictionaryTree " << GetTimeStampString() << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestPerpformance::TestPerfForDir( const std::wstring &Dir, const std::wstring *ExtFirst, const std::wstring *ExtLast )
{
   cout << APL_LINE << "BEGIN TestPerfForDir " << GetTimeStampString() << endl;

   typedef std::list<SS::Dictionary::Types::TWordInfo> TWIList;

   cout << "Попробуем взять IDictionary" << endl;
   IDictionary *pDictionary = GetIDictionary(m_pLoadManager);

   APL_ERROR(pDictionary != 0);

   CDirectoryFileList DirectoryFileList( Dir, ExtFirst, ExtLast );
   TStringsForTest StringsForTest;
   TTimer Timer;
   double TimeTotal = 0;
   TWIList DictData;
   unsigned int FindWords;
   unsigned int TotalWordsFile;
   unsigned int TotalWordsAllFiles = 0;
   unsigned int FilesCount = 0;

   cout << "Начинаем тестирование" << endl;

   while( DirectoryFileList.Next() )
   {
      LoadToMemoryFileText(Convert(DirectoryFileList.GetDir() +  DirectoryFileList.GetFile(), string()), StringsForTest);
      
      pDictionary->GetMorphologyAnalyser()->NextText();
      TotalWordsFile = (unsigned int)StringsForTest.size();

      cout << "CMALanguagesManager::GetWordInfo(..., efzFullAddNewWords): ";
      FindWords = 0;
      //unsigned TotalSize = 0;
      
      Timer.Start();

      for( TStringsForTest::const_iterator I = StringsForTest.begin(); I !=  StringsForTest.end(); ++I )
      {    
         DictData.clear();
         if( pDictionary->GetMorphologyAnalyser()->GetWordInfo(I->c_str(), &DictData,  efzFullAddNewWords) )
         {
            //TotalSize += DictData.size();
            ++FindWords;
         }
      }
      Timer.Stop();

      TotalWordsAllFiles += TotalWordsFile;
      TimeTotal += Timer.InSec();
      ++FilesCount;

      cout << Timer << " совпадений " << FindWords << "/" << TotalWordsFile << " скорость " << (double)TotalWordsFile / (Timer.InSec()) << endl;
   }
   
   std::stringstream Stream;

   Stream << "Каталог: " << Dir << " Файлов: " << FilesCount << " Время всех файлов: " << TimeTotal << " Общее количество слов: " << TotalWordsAllFiles << " Производительность: " << (double)TotalWordsAllFiles / TimeTotal;
   cout << endl << endl << Stream.str() << endl; 
   DEBUG_MSG( Stream.str() );

   cout << "END TestPerfForDir " << GetTimeStampString() << endl << APL_LINE;
}

} //namespace Dictionary
} //namespace UnitTests
} //namespace SS

#endif //_SS_UNITTESTS
