#pragma once
#include "stdafx.h"
#include "../ASSInterface/ITransliteration.h"
#include <vector>

namespace SS
{
    namespace Transliteration
    {
        using std::wstring;
        using std::vector;

        class CTransliterationFrench : public SS::Interface::Core::ITransliteration
        {
            public:
                CTransliterationFrench();
                virtual ~CTransliterationFrench();
                CTransliterationFrench(const CTransliterationFrench &obj);
                CTransliterationFrench& operator=(const CTransliterationFrench &obj);

                /// освобождение
			    ULONG Release();
			    /// приведение к интерфейсу с поданным REFIID. 
                /// pBase будет NULL, если данный интерфейс не поддерживаетс€
			    HRESULT QueryInterface(REFIID pIID, void** pBase);
                ///переопредел€ю от IBase
			    void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);

                wstring CorrectString(const wstring &str);

                void SetDictionary(const wstring &path);

            private:
                
                void SplitString(const wstring &str, const wstring addSymbs, bool containDigits,
                                 vector<wstring> &parts);

                /// проверка подходит ли символ выдел€емой последовательности
                bool isGoodSymb(const wchar_t symb, const wstring addSymbs, bool containDigits);

                /// транслитераци€ слова, если это требуетс€
                wstring WordTransliteration(const wstring &word);

                /// удаление из com неподход€щих под word комбинаций
                void CorrectVectorOfPossibleCombinations(vector<wstring> &com, const wstring &word,
                                                         bool consideSize);

                /// транслитерированна€ комбинаци€ по правилам
                bool TransliterationCombinationByRules(wstring &collectedWord, const wstring &combination,
                                                       int position);

                /// инициализаци€ m_sFrenchLetterCombinations
                static vector<wstring> InitFrenchLetterCombinations();

                /// текущее транслитерируемое слово
                wstring m_CurrentTransliteringWord;

                /// словарные слова, которые не подлежат транслитерации
                vector<wstring> m_DictionaryWords;

                /// русский алфавит
                static wstring m_sRussianAlphabet;
                /// гласные французского алфавита
                static wstring m_sFrenchVowels;
                /// согласные французского алфавита
                static wstring m_sFrenchConsonants;
                /// французские буквосочетани€
                static vector<wstring> m_sFrenchLetterCombinations;
        };
    }
}