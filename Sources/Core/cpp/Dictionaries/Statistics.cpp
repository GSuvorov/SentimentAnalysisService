#include "stdafx.h"

namespace SS{ 
namespace Dictionary{
namespace Detail{

//template CSymbolCharacteristic<wchar_t, 10>;

///////////////////////////////////////////////////////////////////////////////
// class TConstData
///////////////////////////////////////////////////////////////////////////////
void TConstData::LoadFromResource( HMODULE hModule, const TCHAR *ResourceType, WORD ResourceID, TString2String &String2String )
{
   const wchar_t* pData, *pEnd;
   size_t Size;
   TString2String::value_type Value;

   if( 
      LoadResource(hModule, ResourceType, ResourceID, pData, Size) != ERROR_SUCCESS ||
      Size < APL_ARRSIZE(g_UnicodeFilePrefix) ||
      std::memcmp( pData, g_UnicodeFilePrefix, APL_ARRSIZE(g_UnicodeFilePrefix)) != 0 
   )
      APL_THROW(_T("Ошибка при чтении ресурса ResourceType = ") << ResourceType << _T(", ResourceID = ") << ResourceID );

   pEnd = pData + Size;
   pData += APL_ARRSIZE(g_UnicodeFilePrefix) / sizeof(*pData);
   String2String.clear();

   //Используем конечный автомат
   char State = 'a';
   const wchar_t *pBeginWord = pData;

   for( ; pData != pEnd; ++pData )
      switch(State)
      {
      case 'a': //четние первого слова
         if(*pData  == L'\t')
         {
            Value.first.assign( pBeginWord, pData );  
            pBeginWord = pData + 1;
            State = 'b';
         }
         break;
      
      case 'b': //четние второго слова
         if(*pData  == L'\r')
         {
            Value.second.assign( pBeginWord, pData ); 
            String2String.insert( Value );
            State = 'c';
         }
         break;
      
      case 'c': //Пропуск символа '\n'
         APL_ASSERT( *pData == L'\n' );
         pBeginWord = pData + 1;
         State = 'a';
         break;
      }
   
   //Если мы закончили в состоянии b то нам не встретился \r\n, завершаем строку
   if( State == 'b' )
   {
      Value.second.assign( pBeginWord, pData ); 
      String2String.insert( Value );
   }
   else if( pBeginWord != pData )
      APL_THROW(_T("Ошибка в данных ресурса ResourceType = ") << ResourceType << _T(", ResourceID = ") << ResourceID );    

   TString2String(String2String).swap(String2String);
}
///////////////////////////////////////////////////////////////////////////////

void TConstData::LoadFromResource( HMODULE hModule, const TCHAR *ResourceType, WORD ResourceID, TChar2Char &Char2Char )
{
   const wchar_t* pData, *pEnd;
   size_t Size;
   TChar2Char::value_type Value;

   if( 
      LoadResource(hModule, ResourceType, ResourceID, pData, Size) != ERROR_SUCCESS ||
      Size < APL_ARRSIZE(g_UnicodeFilePrefix) ||
      std::memcmp( pData, g_UnicodeFilePrefix, APL_ARRSIZE(g_UnicodeFilePrefix)) != 0 
   )
      APL_THROW(_T("Ошибка при чтении ресурса ResourceType = ") << ResourceType << _T(", ResourceID = ") << ResourceID );

   pEnd = pData + Size;
   pData += APL_ARRSIZE(g_UnicodeFilePrefix) / sizeof(*pData);
   Char2Char.clear();

   const wchar_t *pBeginWord = pData;

   for( ; pData != pEnd; ++pData )
   {
      while( pData != pEnd && *pData  != L'\r' ) ++pData;
      
      if( pData - pBeginWord != 3 || pBeginWord[1] != L'\t' )
         APL_THROW(_T("Ошибка в данных ресурса ResourceType = ") << ResourceType << _T(", ResourceID = ") << ResourceID );    

      Value.first = pBeginWord[0];
      Value.second = pBeginWord[2];
      Char2Char.insert( Value );

      if( pEnd - pData < 2 ) break;
     
      APL_ASSERT( pData[1] == L'\n' );
      pData += 2; //Пропускаем '\n'
      pBeginWord = pData;
   }

   TChar2Char(Char2Char).swap(Char2Char);
}



} //namespace Detail
} //namespace Dictionary
} //namespace SS
