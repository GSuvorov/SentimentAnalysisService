#include "stdafx.h"

namespace SS{ 
namespace Dictionary{
namespace InitData{

   HMODULE g_hModuleCurrentDLL = 0;
   
   const WORD DictionaryToResId[ObjectID::DICTS_COUNT] = 
   { 
      IDR_ENGLISH_BASE_CHARACTERS, IDR_ENGLISH_ABBREVIATION_CHARACTERS,
      IDR_RUSSIAN_BASE_CHARACTERS, IDR_RUSSIAN_ABBREVIATION_CHARACTERS,
      
	  IDR_RUSSIAN_ORDERING,
      IDR_ARABIC_NUMBERS_CHARACTERS, IDR_ROMAN_NUMBERS_CHARACTERS,
      IDR_NUMERIC_ORDERING_CHARACTERS, IDR_SYMBOLS_CHARACTERS
   };

   const wchar_t DictionaryResClass[] = L"SymbolCharacteristic";
   const wchar_t ConstDataResClass[] = L"ConstData";
  
} //namespace InitData
} //namespace Dictionary
} //namespace SS

///////////////////////////////////////////////////////////////////////////////