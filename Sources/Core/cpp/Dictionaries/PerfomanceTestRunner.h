#pragma once
#ifdef _SS_UNITTESTS

namespace SS{ 
namespace UnitTests{ 
namespace Dictionary{

class CTestPerpformance
{
   ILoadManager *m_pLoadManager;
   DBMS::IDataBase *m_pDataBase;
   

   typedef std::list<std::wstring> TStringsForTest;
private:
   void InitNDXDataTable( CTablePtr &pTable );
   
   //Загружает в Buf все строки из файла FileName в след. формате.
   //<Слово попадает в Buf><пробельные символы><что то ещё>endl   
   void LoadToMemoryFileStrings( const std::string &FileName, TStringsForTest &Buf );
   
   //Загружает в Buf все слова из файла тесксового файла FileName
   void LoadToMemoryFileText( const std::string &FileName, TStringsForTest &Buf );

   void TestIDictionaryTreeImpl( IDictionary *pDictionary, const TStringsForTest &StringsForTest );

public:
   explicit CTestPerpformance(ILoadManager *pLoadManager);
   ~CTestPerpformance();

   //Считывает из каталога все файлы cpp, включая подкаталоги и записывает  все слова в них в 
   //таблицу DBMS
   void FillNdxDBMS( const std::wstring &Dir, const std::wstring *ExtFirst, const std::wstring *ExtLast );

   //Поиск напрямую в таблице DBMS
   void FindNdxInDBMS( const std::wstring &Dir, const std::wstring *ExtFirst, const std::wstring *ExtLast );

   //Просто проход по словам  в файлах
   void FindDummy( const std::wstring &Dir, const std::wstring *ExtFirst, const std::wstring *ExtLast );
   
   //Поиск в таблице DBMS c приминением кэша CWewWordsCache
   void FindNdxInCWewWordsCache( const std::wstring &Dir, const std::wstring *ExtFirst, const std::wstring *ExtLast );

   void TestIDictionaryTree(const std::wstring &Dir, const std::wstring *ExtFirst, const std::wstring *ExtLast );
   void TestIDictionaryTreeFromText(const std::wstring &Dir, const std::wstring *ExtFirst, const std::wstring *ExtLast );

   void TestPerfForDir( const std::wstring &Dir, const std::wstring *ExtFirst, const std::wstring *ExtLast );
};

} //namespace Dictionary
} //namespace UnitTests
} //namespace SS

#endif //_SS_UNITTESTS
