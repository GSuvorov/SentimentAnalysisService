#pragma once
#ifdef _SS_UNITTESTS

#include "TestAuxiliary.h"
#include "../[libs]/CoutConvert.h"

namespace SS{ 
namespace UnitTests{ 
namespace Dictionary{
	

class CTestSmartDictionary
{
public:

   //Сохраняет всю информацию о результатах теста CSmartDictionary. Для того чтобы в нескольких потоках
   //попытаться получить эту информацию и сравнить с эталонной
   struct CTestStorage
   {
      struct SWordInfo
      {
         std::wstring Str; //Слово для поверки
         bool Exist;       //Найдено ли слово в словорях
         bool AutomaticDetermination;   //Удалось ли автоматически определить морфологическую информацию
         CSmartDictionary::SHypothesisInfo HypothesisInfo; //Гипотезы
         std::list<TWordInfo> WIList; //Список из GetWordInfo
         std::list<SWordByIDSource> WordByIDSourceList;

         SWordInfo(): Exist(false), AutomaticDetermination(false) {}
      };

      typedef std::list<SWordInfo> TWordInfoList;

      TWordInfoList WordInfoList;
   };
   
   typedef std::list<std::string> TWordsList;
   typedef CSmartDictionary::SHypothesisInfo::WordInfo THypothesisWordInfo;

private:
   DBMS::IDataBase *m_pDataBase;
   ILoadManager *m_pLoadManager;

      
private:
   struct ThreadsData
   {
      CTestSmartDictionary *pTSD;
      int ThreadNum;
   };

   static const size_t ThreadsCout = 20;
   ThreadsData rgThreadsData[ThreadsCout];
   volatile LONG Shutdown;

   HANDLE rghThreads[ThreadsCout];

private:
   CTestStorage SampleTestStorage;
   TWordsList WL;
   CSmartDictionaryAdapter::TDictionaryTypes m_DictionaryType;
   InitData::Languages::eLanguages m_Language;

private:
   static DWORD WINAPI TestThread(PVOID pvParam); 

   void CreateThreads();
   void StopThreads();
   static void NoThreadTestImpl(CSmartDictionary *pSD, std::istream & InStream, std::ostream &OutStream);
   static void ThreadTestSaveData( LockedPtr<CSmartDictionary> pSD, CTestStorage & TS, const TWordsList & WL, volatile LONG *pShutdown = 0);
   void Construct( LockedPtr<CSmartDictionary> pDictionary );
   auto_ptr_ex<CSmartDictionary> GreateDictionary();


public:
   CTestSmartDictionary( DBMS::IDataBase *pDataBase, InitData::Languages::eLanguages Lang, ILoadManager *pLoadManager )
	   : m_pDataBase(pDataBase), m_pLoadManager(pLoadManager), Shutdown(FALSE), m_Language(Lang), 
	     m_DictionaryType(CSmartDictionaryAdapter::TDictionaryTypes::EStandart)
   {
      for( size_t i = 0; i < ThreadsCout; ++i )
      {
         rgThreadsData[i].pTSD = this;
         rgThreadsData[i].ThreadNum = (int)i;
         rghThreads[i] = 0;
      }
   }

   void ThreadTest(CSmartDictionaryAdapter::TDictionaryTypes DictionaryType = CSmartDictionaryAdapter::EStandart);
   void NoThreadTest(CSmartDictionaryAdapter::TDictionaryTypes DictionaryType = CSmartDictionaryAdapter::EStandart);
};
///////////////////////////////////////////////////////////////////////////////

class CTestIDictionary
{
public:

   //Сохраняет всю информацию о результатах теста. Для того чтобы в нескольких потоках
   //попытаться получить эту информацию и сравнить с эталонной
   struct CTestStorage
   {
      struct SWordInfo
      {
         std::wstring Str;         //Слово для поверки
         bool IsExistResult;       //То что вернул метод IsExist
         bool GetWordInfoResult;   //То что вернул метод GetWordInfo

         std::list<TWordInfo> WIList; //Список из GetWordInfo
         std::list<SWordByIDSource> WordByIDSourceList;

         SWordInfo(): IsExistResult(false), GetWordInfoResult(false) {}
      };

      typedef std::list<SWordInfo> TWordInfoList;

      TWordInfoList WordInfoList;
   };
   typedef std::list<std::string> TWordsList;

private:
   struct ThreadsData
   {
      CTestIDictionary *pTSD;
      int ThreadNum;
   };

   static const size_t ThreadsCout = 20;
   static const size_t WriteThreadsCount = 2;

   ThreadsData rgThreadsData[ThreadsCout];
   volatile LONG Shutdown;

   HANDLE rghThreads[ThreadsCout];

private:
   ILoadManager *m_pLoadManager;
   CTestStorage SampleTestStorage;
   TWordsList WL;
   CLoadManagerCreater m_LoadManagerCreater;

private:

   void TestNominal( int ThreadNum );
   void TestProper( int ThreadNum );
   static DWORD WINAPI TestThread(PVOID pvParam); 

   static void NoThreadTestImpl(IMorphologyAnalyser *pMA, CUnicodeFileReader &FR, std::ostream &OutStream, IOneRoot *pOneRoot = 0 );
   static void ThreadTestSaveData(IMorphologyAnalyser *pMA, CTestStorage &TS, const TWordsList &WL, volatile LONG *pShutdown = 0 );
   void CreateThreads(  );
   void StopThreads();

public:
   CTestIDictionary( ILoadManager *pLoadManager );
   void NoThreadTest();
   void ThreadTest();
   void TestFullWordsIDictionary( std::ofstream &OutStream );
   void TestGetWordForIUnit();
};

} //namespace Dictionary
} //namespace UnitTests
} //namespace SS

#endif //_SS_UNITTESTS
