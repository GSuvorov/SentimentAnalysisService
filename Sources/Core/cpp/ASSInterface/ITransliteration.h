#pragma once 

#include "..\ASCinterface\iinterface.h"

// {2B9B7AC5-DE3D-4d25-A0E0-BDD37DB12009}
static const GUID CLSID_TransliterationFrench = 
{ 0x2b9b7ac5, 0xde3d, 0x4d25, { 0xa0, 0xe0, 0xbd, 0xd3, 0x7d, 0xb1, 0x20, 0x9 } };

// {91AF271A-1599-42d1-BCB4-7974A0EFCBE5}
static const GUID IID_TransliterationFrench = 
{ 0x91af271a, 0x1599, 0x42d1, { 0xbc, 0xb4, 0x79, 0x74, 0xa0, 0xef, 0xcb, 0xe5 } };

#include <string>

namespace SS
{
	namespace Interface
	{
		namespace Core
		{
            using std::wstring;
            /// Интерфейс для транслитерации языка
            class ITransliteration : virtual public SS::Interface::IBase
            {
                public:
                    /// транслитерация строки
                    /// str - строка
                    /// retval wstring - странслитерированная строка
                    virtual wstring CorrectString(const wstring &str) = 0;

                    /// установление словаря для нетранслитерируемых слов
                    /// path - путь к словарю
                    virtual void SetDictionary(const wstring &path) = 0;
            };
        }
    }
}