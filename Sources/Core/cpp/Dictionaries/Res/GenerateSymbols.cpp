// GenerateSymbols.cpp : Defines the entry point for the console application.
// Програмка создаёт файлы со всеми возможными символами определённого языка

#include "stdafx.h"
#include <iostream>
#include <fstream>

using namespace std;
struct TSymbolsIntervals
{
   wchar_t Begin;
   wchar_t End;
};

void CreateFile( char *FileName, const TSymbolsIntervals *First, const TSymbolsIntervals *Last )
{
   ofstream flOut( FileName, ios_base::out | ios_base::trunc | ios_base::binary );
   wchar_t Cur;
   char Prefix[] = { 0xFF, 0xFE };

   flOut.write( Prefix , sizeof(Prefix)/sizeof(*Prefix) );

   for( ;First != Last; ++First )
   {
      for( Cur = First->Begin; Cur < First->End; ++Cur )
         flOut.write( reinterpret_cast<char *>(&Cur) , sizeof(wchar_t)  );
      
      flOut.write( reinterpret_cast<char *>(&Cur) , sizeof(wchar_t)  );
   }
}

int _tmain(int argc, _TCHAR* argv[])
{
   {
      const TSymbolsIntervals SI[] = {
         { L'A', L'Z' },
         { L'a', L'z' }
      };
      
      CreateFile( "EnglishBaseCharacters.txt", SI, SI + sizeof(SI)/sizeof(*SI) );
   }
   
   {
      const TSymbolsIntervals SI[] = {
         { L'А', L'Я' },
         { L'а', L'я' }
      };

      CreateFile( "RussianBaseCharacters.txt", SI, SI + sizeof(SI)/sizeof(*SI) );
   }

   {
      const TSymbolsIntervals SI[] = {
         { L'A', L'Z' },
         { L'a', L'z' },
         { L'0', L'9' },
         { L'.', L'.' },
         { L'/', L'/' },
     };

      CreateFile( "EnglishAbbreviationCharacters.txt", SI, SI + sizeof(SI)/sizeof(*SI) );
   }

    {
      const TSymbolsIntervals SI[] = {
         { L'А', L'Я' },
         { L'а', L'я' },
         { L'0', L'9' },
         { L'.', L'.' },
         { L'/', L'/' },
    };

      CreateFile( "RussianAbbreviationCharacters.txt", SI, SI + sizeof(SI)/sizeof(*SI) );
   }


    {
       const TSymbolsIntervals SI[] = {
          { 0, 65535 },
       };

       CreateFile( "SymbolsCharacters.txt", SI, SI + sizeof(SI)/sizeof(*SI) );
    }

    {
       const TSymbolsIntervals SI[] = {
          { L'0', L'9' },
       };

       CreateFile( "ArabicNumbersCharacters.txt", SI, SI + sizeof(SI)/sizeof(*SI) );
    }
 
    {
       const TSymbolsIntervals SI[] = {
          { L'I', L'I' },
          { L'V', L'V' },
          { L'X', L'X' },
          { L'L', L'L' },
          { L'C', L'C' },
          { L'D', L'D' },
          { L'M', L'M' },
          { L'i', L'i' },
          { L'v', L'v' },
          { L'x', L'x' },
          { L'l', L'l' },
          { L'c', L'c' },
          { L'd', L'd' },
          { L'm', L'm' },
     };

       CreateFile( "RomanNumbersCharacters.txt", SI, SI + sizeof(SI)/sizeof(*SI) );
    }

   cout << "Ok";
   cin.get();
   return 0;
}

