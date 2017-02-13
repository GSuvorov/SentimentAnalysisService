#pragma once
#ifdef _SS_UNITTESTS
#include "TestAuxiliary.h"
#include "../[libs]/CoutConvert.h"

namespace SS{ 
namespace UnitTests{ 
namespace Dictionary{
	
///////////////////////////////////////////////////////////////////////////////
// Тестирование многопоточности при помощи простых объектов синхронизации 
// из ThreadAuxiliary.h
// Создаёт несколько потоков каждый из которых либо уменьшает либо увеличивает 
// общую для всех потоков переменную.
///////////////////////////////////////////////////////////////////////////////
class CTestMultiThreaded: 
   public ObjectLevelLockable<CTestMultiThreaded>
   //public NoLockable<CTestMultiThreaded>
{
   int m_N;

public:

   struct ThreadsData
   {
      bool ForwardDireaction;
      LockedPtr<CTestMultiThreaded> pObject;
      int Num;
   };

   static const size_t ThreadsCout = 20;
   ThreadsData rgThreadsData[ThreadsCout];
   volatile LONG Shutdown;

   HANDLE rghThreads[ThreadsCout];

public:

   CTestMultiThreaded():m_N(0), Shutdown(FALSE) 
   {
      for( size_t i = 0; i < ThreadsCout; ++i )
      {
         rgThreadsData[i].ForwardDireaction = i % 2 == 0;
         rgThreadsData[i].pObject.Set(this);
         rghThreads[i] = 0;
         rgThreadsData[i].Num = (int)i;
      }
   }

   void TestInc( int ThreadNum )
   {
      Console.Write( ThreadNum, " TestInc BEGIN: ", m_N );
      
      Sleep(1 * ThreadNum);

      ++m_N;

      Sleep(2 * ThreadNum);
      
      Console.Write( ThreadNum, " TestInc END: ", m_N );
   }

   void TestDec( int ThreadNum )
   {
      Console.Write( ThreadNum, " TestDec BEGIN: ", m_N );
      Sleep(3 * ThreadNum);
      
      --m_N;

      Sleep(4 * ThreadNum);
      Console.Write( ThreadNum, " TestDec END: ", m_N );
  }

   static DWORD WINAPI TestThread(PVOID pvParam) 
   {
      ThreadsData *pTD = reinterpret_cast<ThreadsData *>(pvParam);

      Console.Write( "Создаём поток № ", pTD->Num );

      while( !pTD->pObject->Shutdown )
      {
         if( pTD->ForwardDireaction )
            pTD->pObject->TestInc(pTD->Num);
         else
            pTD->pObject->TestDec(pTD->Num);
      }
      
      Console.Write( "Завершаем поток № ", pTD->Num );

      return 0;
   }

   void CreateThreads()
   {
      APL_ASSERT( Shutdown == FALSE );
      DWORD dwThreadID;

      for( size_t i = 0; i < ThreadsCout; ++i )
         rghThreads[i] = chBEGINTHREADEX(NULL, 0, TestThread, (PVOID)&rgThreadsData[i], 0, &dwThreadID);   
   }

   void StopThreads()
   {
      APL_ASSERT( Shutdown == FALSE );
      
      InterlockedExchange(&Shutdown, TRUE);

      WaitForMultipleObjects(ThreadsCout, rghThreads, TRUE, INFINITE);
      
      for( size_t i = 0; i < ThreadsCout; ++i )
         CloseHandle(rghThreads[i]);
   }

};
///////////////////////////////////////////////////////////////////////////////

class CTestRunner :	public Interface::UnitTests::ITestRunner
{
	struct TIArabic2Roman
	{
		char szRoman[100];
		unsigned int Arabic;
	};

private:
	void ToBinaryTest();
	void TestIMorphologyAnalyser(Detail::IBasicMorphologyAnalyser *pMA, std::istream & InStream, std::ostream &OutStream);
	void TestCBaseDictionary();
	void TestNumericDictionary();
	void TestRomanNumericDictionary();
	void TestSymbolsDictionary();
	void TestCMorphoTypesCollection(std::istream & InStream, std::ostream &OutStream);
	void TestDBMS(std::istream & InStream, std::ostream &OutStream);
	void TestDBMS_Smart(std::istream & InStream, std::ostream &OutStream);
	void TestCSource(std::istream & InStream, std::ostream &OutStream);
   void TestCAbbreviationSource(std::istream & InStream, std::ostream &OutStream);
   void TestCOneRoot(std::istream & InStream, std::ostream &OutStream);
	void TestRomanDigits(std::istream & InStream, std::ostream &OutStream);
   void TestCNewWordsCache(std::istream & InStream, std::ostream &OutStream);
   void TestCNewWordsCacheImpl(DBMS::IDataTable *pTable, TDictNumber DictNumber, bool AddData, std::vector<unsigned int> &AddedIndexex);
   void FillAbbreviationsTable();
   void TestCAbbreviationsDictionaryBase();
   void TestThreads();
   void TestIDictionary();
   void TestIDictionaryMultiThreaded();
   void TestSmartDictionary(InitData::Languages::eLanguages Lang, CSmartDictionaryAdapter::TDictionaryTypes DictionaryType = CSmartDictionaryAdapter::EStandart);
   void TestSmartDictionaryMultiThreaded(InitData::Languages::eLanguages Lang, CSmartDictionaryAdapter::TDictionaryTypes DictionaryType = CSmartDictionaryAdapter::EStandart);
   void TestCSymbolCharacteristic();
   void TestCPartOfWord();
   void TestCDirectoryFileList();
   void TestFullWordsIDictionary();
   void TestCNumberCombinatory();

   void TestPerfCWordsCashe();
   void TestPerfDictTree();
   void TestPerfDictTreeText();

   void CreateUndifinedNewWordsFile( const std::string &InFileName, const std::string &OutFileName );

   void BuildMinimalHypothesisTree();
   void TestUnicodeFileReader();
   void TestMinimalHypothesisTree();
   void TestSourceDelete();
   void TestNewWordsList();
   void TestSearchParam();
   void TestFormDeleate();
   void TestPerfDictDir();
   void TestGetWordForIUnit();
   
   void CreateCompoundFile();

   void Test();
   void Menu();

private:
	DBMS::IDataBase *m_pDataBase;
   DBMS::IDataBase *m_pDataBaseRus;
   DBMS::IDataBase *m_pDataBaseAbbreviations;

	SS::Interface::Core::ResourceManagers::ILingvoBaseManager* m_pLingvoBaseManager;

public:
	///запуск тестов
	void Run();
	
	///освобождение
	ULONG Release();


	BEGIN_QI_MAP()
		QI_MAP_INHERIT_ENTRY( IID_TestRunner, Interface::UnitTests::ITestRunner )
        QI_MAP_INHERIT_ENTRY( IID_Base, Interface::IBase )
	END_QI_MAP()
};

} //namespace Dictionary
} //namespace UnitTests
} //namespace SS

#endif //_SS_UNITTESTS
