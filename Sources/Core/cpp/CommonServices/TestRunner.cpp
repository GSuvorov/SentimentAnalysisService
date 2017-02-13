#include "stdafx.h"
#ifdef _SS_UNITTESTS

#include "TestRunner.h"

#include <iterator>
#include "..\[libs]\TestHelper.h"
#include "..\[libs]\CoutConvert.h"



namespace SS {
namespace UnitTests{ 
namespace CommonServices {
namespace UserTextStorage {

using namespace std;

void CTestRunner::MainTest()
{
   cout << APL_LINE << "BEGIN MainTest" << endl;

#ifndef NDEBUG
   #define PATH_PREFIX _T("..\\..\\..\\..\\Bin.debug\\")
#else
   #define PATH_PREFIX _T("..\\..\\..\\..\\Bin\\")
#endif

   typedef auto_ptr_ex<IUsersManager, auto_ptr_ex_release_strategy> TUsersManagerPtr;   
   typedef auto_ptr_ex<IText, auto_ptr_ex_release_strategy> TTextPtr;   
   typedef auto_ptr_ex<ITextsManager, auto_ptr_ex_release_strategy> TTextsManagerPtr;   
   
   cout << "Загружаем CommonServices.dll" << endl;
   CLoadManagerCreater LMCommonServices( PATH_PREFIX _T("Core\\Linguistics\\CommonServices.dll") );
   TStringsList SL;

   cout << "Получаем интерфейсы" << endl;

   TUsersManagerPtr UsersManagerPtr = LMCommonServices.GreateInstance<IUsersManager>( CLSID_UsersManager, IID_UsersManager, GetLoadManager() );
   TTextPtr TextPtr = LMCommonServices.GreateInstance<IText>( CLSID_Text, IID_Text, GetLoadManager() );
   TTextsManagerPtr TextsManagerPtr = LMCommonServices.GreateInstance<ITextsManager>( CLSID_TextsManager, IID_TextsManager, GetLoadManager() );

   
   TPrivateUserInfo PrivateUserInfo;
   PrivateUserInfo.SetDescription( _T("SDescription") );
   PrivateUserInfo.SetUserName( _T("UserName") );

   UsersManagerPtr->Init( /*_T("..\\UserTextStorageData.ini"), _T("..\\DATA\\")*/ );
   UsersManagerPtr->AddUser( PrivateUserInfo );

   TextsManagerPtr->Open( PrivateUserInfo.GetUserID(), PrivateUserInfo.GetPassword(), UsersManagerPtr.get() );
   TextsManagerPtr->AddText( _T("Test"), TextPtr.get() );
   TextsManagerPtr->AddText( _T("Test2"), TextPtr.get() );
   TextsManagerPtr->GetUserTextNames( SL );

   std::copy( SL.begin(), SL.end(), ostream_iterator<TStringsList::value_type>( cout, "\n" ) );

   TextsManagerPtr->GetText( _T("Test"), TextPtr.get() );

   TextPtr->SetTextPart( _T("Part1"), _T("Текст части 1"), FFUNICODE );
   TextPtr->SetTextPart( _T("Part1Ansi"), _T("Текст части 1"), FFANSI );

   wstring Test, Test2;
   
   TextPtr->GetTextPart( _T("Part1"), Test );
   TextPtr->GetTextPart( _T("Part1Ansi"), Test2 );

   APL_ERROR( Test == Test2 );

   cout << Test << endl;

   TextPtr->GetTextPartNames(SL);

   cout << "Список частей текстов: " << endl;
   std::copy( SL.begin(), SL.end(), ostream_iterator<TStringsList::value_type>( cout, "\n" ) );

   UsersManagerPtr->DeleteUser( PrivateUserInfo.GetUserID() );

   cout << "END MainTest" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::XSLTTest()
{
   cout << APL_LINE << "BEGIN XSLTTest" << endl;

#ifndef NDEBUG
   #define PATH_PREFIX _T("..\\..\\..\\..\\Bin.debug\\")
#else
   #define PATH_PREFIX _T("..\\..\\..\\..\\Bin\\")
#endif

   typedef auto_ptr_ex<IXMLOperation, auto_ptr_ex_release_strategy> TXMLOperationPtr;  

   cout << "Загружаем CommonServices.dll" << endl;
   CLoadManagerCreater LMCommonServices( PATH_PREFIX _T("Core\\Linguistics\\CommonServices.dll") );

   TXMLOperationPtr XMLOperationPtr = LMCommonServices.GreateInstance<IXMLOperation>( CLSID_XMLOperation, IID_XMLOperation, GetLoadManager() );

   std::wstring XMLIn, XMLOut, XSLT;

   APL_ERROR( ReadUnicodeFile(L"..\\..\\..\\..\\Sources\\Core\\TextStatisticsProcessor\\XML\\TransformForCSV.xslt", XSLT) == ERROR_SUCCESS );
   APL_ERROR( ReadUnicodeFile(L"..\\..\\TextStatisticsProcessorTest\\Debug\\Result.xml",XMLIn ) == ERROR_SUCCESS );

   XMLOperationPtr->TransformXML(XMLIn, XSLT, XMLOut);

   APL_ERROR( WriteUnicodeFile(L"Result.csv", XMLOut) == ERROR_SUCCESS );

   cout << "END XSLTTest" << endl << APL_LINE;
}
///////////////////////////////////////////////////////////////////////////////

void CTestRunner::Run()
{	
	cout << "Вход CTestRunner::Run() дата и время сборки модуля " << __TIMESTAMP__ << endl;
   
   #ifdef NDEBUG
      cout << "NDEBUG подключён" << endl;
   #else
      cout << "NDEBUG не подключён" << endl;
   #endif

	APL_TRY()
	{  
      XSLTTest();
      MainTest();   
  	}			
	APL_CATCH()

	cout << "Выход CTestRunner::Run()" << endl;
}
///////////////////////////////////////////////////////////////////////////////


//void CTestRunner::Menu()
//{
//   struct SInfo
//   {
//      string Name;
//
//      void (CTestRunner::*FuncPtr)();
//   };
//
//   SInfo Data[] = {
//      {
//         "Многопоточный тест на каталоге \"..\\MultiTest\\\"",
//         &CTestRunner::MainTest
//      },
//      {
//         "Обработать файл и сохранить статистические данные в XML",
//         &CTestRunner::SaveToXMLFile
//      }
//
//   };
//
//   int i;
//
//   int CurN = 0;
//
//   for(;;) 
//   {
//      cout << APL_LINE;
//      for( i = 0; i < APL_ARRSIZE(Data); ++i ) 
//      {
//         cout << i + 1 << " - "<< Data[i].Name << endl;
//      }
//
//      cout << i + 1 << " - "<< "Выход" << endl;
//      cout << APL_LINE;
//      cout << "Что делать? :";
//      
//      //Очистка буфера
//      ClearIStreamBuff( cin );
//
//      if( cin >> CurN )
//      {
//         if( CurN > 0 && CurN <= APL_ARRSIZE(Data) )
//            (this->*Data[CurN - 1].FuncPtr)();
//         else if( CurN == APL_ARRSIZE(Data) + 1 )
//            return;
//         else
//            cout << "Чаво, Чаво???" << endl;
//      }
//      else
//      {
//         cout << "Чаво, Чаво???" << endl;
//      }
//   } 
//}

} //namespace UserTextStorage 
} //namespace CommonServices 
} //namespace UnitTests 
} //namespace SS 

#endif //_SS_UNITTESTS
