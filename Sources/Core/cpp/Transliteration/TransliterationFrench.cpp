#include "TransliterationFrench.h"
#include <algorithm>
#include <fstream>
#include <iostream>
using std::wifstream;
using std::ifstream;
using std::getline;
using std::ios;

namespace SS
{
    namespace Transliteration
    {
        wstring CTransliterationFrench::m_sRussianAlphabet = L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
        wstring CTransliterationFrench::m_sFrenchVowels = L"aàâeéëèêiîïoôœuûy";
        wstring CTransliterationFrench::m_sFrenchConsonants = L"bcçdfghjklmnpqrstvwxz";
        vector<wstring> CTransliterationFrench::m_sFrenchLetterCombinations = InitFrenchLetterCombinations();

        vector<wstring> CTransliterationFrench::InitFrenchLetterCombinations()
        {
            vector<wstring> col;
            col.push_back(L"a"); col.push_back(L"à"); col.push_back(L"â");
            col.push_back(L"ai");
            col.push_back(L"aim"); col.push_back(L"ain");
            col.push_back(L"am");
            col.push_back(L"au");
            col.push_back(L"ault");
            col.push_back(L"ay");
            col.push_back(L"b");
            col.push_back(L"c");
            col.push_back(L"ç");
            col.push_back(L"cc");
            col.push_back(L"ch");
            col.push_back(L"ck"); col.push_back(L"cqu");
            col.push_back(L"ct"); col.push_back(L"cts");
            col.push_back(L"d");
            col.push_back(L"de"); col.push_back(L"des");
            col.push_back(L"e");
            col.push_back(L"é");
            col.push_back(L"ë");
            col.push_back(L"è"); col.push_back(L"ê");
            col.push_back(L"eau");
            col.push_back(L"ei"); col.push_back(L"eî");
            col.push_back(L"eil"); col.push_back(L"eille");
            col.push_back(L"ein");
            col.push_back(L"em");
            col.push_back(L"en");
            col.push_back(L"eu"); col.push_back(L"eû");
            col.push_back(L"euil");
            col.push_back(L"ey");
            col.push_back(L"f");
            col.push_back(L"g");
            col.push_back(L"ge");
            col.push_back(L"gg");
            col.push_back(L"gn");
            col.push_back(L"gu");
            col.push_back(L"h");
            col.push_back(L"i"); col.push_back(L"î"); col.push_back(L"ï");
            col.push_back(L"ier"); col.push_back(L"ieu");
            col.push_back(L"il");
            col.push_back(L"ill");
            col.push_back(L"ille");
            col.push_back(L"im");
            col.push_back(L"in");
            col.push_back(L"j");
            col.push_back(L"k");
            col.push_back(L"l");
            col.push_back(L"ld");
            col.push_back(L"lh");
            col.push_back(L"ll");
            col.push_back(L"llier");
            col.push_back(L"llon");
            col.push_back(L"m");
            col.push_back(L"n");
            col.push_back(L"o"); col.push_back(L"ô");
            col.push_back(L"œ"); col.push_back(L"œu");
            col.push_back(L"oë");
            col.push_back(L"oi"); col.push_back(L"oî"); col.push_back(L"oie");
            col.push_back(L"oin");
            col.push_back(L"om");
            col.push_back(L"ou"); col.push_back(L"où"); col.push_back(L"oû");
            col.push_back(L"oy");
            col.push_back(L"p");
            col.push_back(L"ph");
            col.push_back(L"qu");
            col.push_back(L"que"); col.push_back(L"ques");
            col.push_back(L"r");
            col.push_back(L"rr");
            col.push_back(L"s");
            col.push_back(L"sc");
            col.push_back(L"sch");
            col.push_back(L"ss");
            col.push_back(L"t");
            col.push_back(L"tieu"); col.push_back(L"tieux");
            col.push_back(L"tion");
            col.push_back(L"tioux");
            col.push_back(L"u"); col.push_back(L"û");
            col.push_back(L"um");
            col.push_back(L"un");
            col.push_back(L"v"); col.push_back(L"w");
            col.push_back(L"x");
            col.push_back(L"y");
            col.push_back(L"ym"); col.push_back(L"yn");
            col.push_back(L"z");
            return col;
        }
        
        CTransliterationFrench::CTransliterationFrench()
        {
        }

        CTransliterationFrench::~CTransliterationFrench()
        {
            if(!m_DictionaryWords.empty())
                m_DictionaryWords.clear();
        }

        CTransliterationFrench::CTransliterationFrench(const CTransliterationFrench &obj)
        {
            m_DictionaryWords = obj.m_DictionaryWords;
        }

        CTransliterationFrench& CTransliterationFrench::operator =(const CTransliterationFrench &obj)
        {
            m_DictionaryWords = obj.m_DictionaryWords;
            return *this;
        }

        ULONG CTransliterationFrench::Release()
	    {
		    delete this;
		    return 0;
	    }

	    HRESULT CTransliterationFrench::QueryInterface(REFIID pIID, void** pBase)
	    {
		    if (pIID == IID_TransliterationFrench)
		    {
                *pBase = static_cast<SS::Interface::Core::ITransliteration*> (this);
		    }
		    else if (pIID == IID_Base)
		    {
			    *pBase = static_cast<SS::Interface::IBase*>( this );
		    }
		    else
		    {
			    *pBase = NULL;
		    }
		    return S_OK;	
	    }

        void CTransliterationFrench::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
	    {
            
            SS_TRY
		    {
                SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
            }
            SS_CATCH(L"")
        }

        void CTransliterationFrench::SetDictionary(const wstring &path)
        {
            ifstream dictionary(path.c_str());
            if(dictionary)
            {
                dictionary.seekg(0, ios::end);
                int length = dictionary.tellg();
                dictionary.seekg(0, ios::beg);
                char *pBuf = new char[length + sizeof(wchar_t)];
                //if(pBuf)
                //{
                    memset(pBuf + length, 0, sizeof(wchar_t));
                    dictionary.read(pBuf, length);
                    wstring dictionaryText(reinterpret_cast<wchar_t*>(pBuf));
                    vector<wstring> brands;
                    SplitString(dictionaryText.erase(0, 1), L"&-. ", true, brands);
                    bool parity = true;
                    for(vector<wstring>::iterator it = brands.begin(); it != brands.end(); ++it)
                    {
                        parity = !parity;
                        if(parity)
                            continue;
                        m_DictionaryWords.push_back(*it);
                    }
                //}
                dictionary.close();
            }
        }

        wstring CTransliterationFrench::CorrectString(const wstring &str)
        {
            wstring result;
            wstring strMap = L"";
            strMap.resize(str.length());
            for(vector<wstring>::iterator it = m_DictionaryWords.begin();
                it != m_DictionaryWords.end();
                ++it)
            {
                for(int pos = str.find(*it); pos != wstring::npos; pos = str.find(*it, pos + 1))
                {
                    if(((pos == 0) || !isGoodSymb(str[pos - 1], L"", true)) &&
                       ((pos == str.length()) || !isGoodSymb(str[pos + (*it).length()], L"", true)))
                    /// слева и справа нет букв или цифр
                    {
                        for(unsigned int i = pos; i < pos + (*it).length(); ++i)
                            strMap[i] = 1;
                    }
                }
            }

            vector<wstring> words;
            SplitString(str, L"", false, words);
            int controlSum = 0;
            for(vector<wstring>::iterator it = words.begin(); it != words.end(); ++it)
            {
                bool isWord = isGoodSymb((*it)[0], L"", false);
                if(isWord && !strMap[controlSum])
                    result += WordTransliteration(*it);
                else
                    result += *it;
                controlSum += (*it).length();
            }
            return result;
        }

        void CTransliterationFrench::SplitString(const wstring &str, const wstring addSymbs, 
                                                 bool containDigits, vector<wstring> &parts)
        {
            int currentWordStart = 0;
            bool isWord = isGoodSymb(str[0], addSymbs, containDigits);
            for(unsigned int i = 0; i <= str.length(); ++i)
            {
                bool condition = false;
                if(i < str.length())
                    condition = isGoodSymb(str[i], addSymbs, containDigits);
                if((i == str.length()) || (condition != isWord))
                /// пошла другая серия символов
                {
                    wstring word = str.substr(currentWordStart, i - currentWordStart);
                    parts.push_back(word);
                    currentWordStart = i;
                    isWord = !isWord;
                }
            }
        }

        bool CTransliterationFrench::isGoodSymb(const wchar_t symb, const wstring addSymbs,
                                                bool containDigits)
        {
            bool result = iswalpha(symb) > 0;
            if(containDigits && !result)
                result = iswdigit(symb) > 0;
            for(wstring::const_iterator it = addSymbs.begin(); !result && (it != addSymbs.end()); ++it)
                if(*it == symb)
                    result = true;
            return result;
        }

        wstring CTransliterationFrench::WordTransliteration(const wstring &word)
        {
            wstring resultWord = word;
            unsigned int wordLength = word.length();
            wchar_t *pWord = new wchar_t[wordLength + 1];
            memset(pWord + wordLength, 0, sizeof(wchar_t));
            int upperCount = 0;
            for(unsigned int i = 0; i < wordLength; ++i)
            {
                if(iswupper(word[i]))
                    ++upperCount;
                pWord[i] = towlower(word[i]);
            }
            
            bool russian = false;
            for(unsigned int i = 0; (!russian) && (i < m_sRussianAlphabet.length()); ++i)
                if(pWord[0] == m_sRussianAlphabet[i])
                    russian = true;
            if(!russian)
            /// требуется транслитерация
            {
                m_CurrentTransliteringWord = pWord;
                /// текущая транслитерация слова
                wstring transliterationWord = L"";
                /// текущее буквосочетание
                wstring curLetters = L"";
                /// подходящие комбинации
                vector<wstring> letterCombinations = m_sFrenchLetterCombinations;
                vector<wstring> prevStep;
                vector<wstring> prevCombinations;
                /// учитывать размер буквосочетания при подборе альтернатив
                bool considerSize = false;
                for(unsigned int j = 0; j < wordLength; ++j)
                {
                    curLetters += pWord[j];
                    prevStep = letterCombinations;
                    if(j == wordLength - 1)
                        considerSize = true;
                    CorrectVectorOfPossibleCombinations(letterCombinations, curLetters, considerSize);
                    if((letterCombinations.size() == 1) && considerSize)
                    {
                        if(TransliterationCombinationByRules(transliterationWord, curLetters, 
                                                             j - curLetters.length() + 1))
                        /// буквосочетание обработано
                        {
                            prevCombinations = m_sFrenchLetterCombinations;
                        }
                        else
                        {
                            prevCombinations = m_sFrenchLetterCombinations;
                            vector<wstring>::iterator it = prevCombinations.begin();
                            for(it = prevCombinations.begin();
                                it != prevCombinations.end();
                                ++it)
                            /// найдем текущее буквосочетание
                            {
                                if(*it == curLetters)
                                    break;
                            }
                            prevCombinations.erase(it);
                            j -= curLetters.length();
                        }
                        considerSize = false;
                        curLetters = L"";
                        letterCombinations = prevCombinations;
                    }
                    else if(letterCombinations.size() == 0)
                    {
                        j -= 2;
                        if(curLetters.length() > 1)
                            curLetters.resize(curLetters.length() - 2);
                        else
                            break;
                        considerSize = true;
                        letterCombinations = prevStep;
                    }
                }
                resultWord = L"";
                /// возвращаем правильный регистр букв
                for(unsigned int i = 0; i < transliterationWord.length(); ++i)
                {
                    if((upperCount == 1) && iswupper(word[0]) && (i == 0))
                        resultWord += towupper(transliterationWord[i]);
                    else if(upperCount == wordLength)
                        resultWord += towupper(transliterationWord[i]);
                    else
                        resultWord += transliterationWord[i];
                }
            }
            return resultWord;
        }

        void CTransliterationFrench::CorrectVectorOfPossibleCombinations(vector<wstring> &com,
                                                                         const wstring &word,
                                                                         bool consideSize)
        {
            vector<wstring>::iterator it = com.begin();
            while(it != com.end())
            {
                bool coincides = true;
                if((*it).length() < word.length())
                    coincides = false;
                if(consideSize && ((*it).length() != word.length()))
                    coincides = false;
                for(unsigned int j = 0; coincides && (j < word.length()); ++j)
                    if((*it)[j] != word[j])
                        coincides = false;
                if(!coincides)
                    it = com.erase(it);
                else
                    ++it;
            }
        }

        bool CTransliterationFrench::TransliterationCombinationByRules(wstring &collectedWord, 
                                                                       const wstring &combination,
                                                                       int position)
        {
            bool notDone = false;
            int wordEnd = m_CurrentTransliteringWord.length() - 1;
            int combinationEnd = position + combination.length() - 1;

            wstring prevLetter = L"";
            if(position)
                prevLetter = m_CurrentTransliteringWord[position - 1];
            wstring nextLetter = L"";
            if(combinationEnd != m_CurrentTransliteringWord.length())
                nextLetter = m_CurrentTransliteringWord[combinationEnd + 1];
            wstring prevComb = (position > 1) ? m_CurrentTransliteringWord.substr(position - 2, 2)
                                              : prevLetter;

            bool prevIsVowel = (m_sFrenchVowels.find(prevLetter) != wstring::npos);
            bool nextIsVowel = (m_sFrenchVowels.find(nextLetter) != wstring::npos);

            if(combination == L"b")
                collectedWord += L"б";
            else if((combination == L"v") || (combination == L"w"))
                collectedWord += L"в";
            else if((combination == L"de") || (combination == L"des"))
                collectedWord += L"де";
            else if((combination == L"è") || (combination == L"ê"))
                collectedWord += L"е";
            else if((combination == L"ck") || (combination == L"cqu") || (combination == L"k") ||
                    (combination == L"qu"))
                collectedWord += L"к";
            else if(combination == L"l")
                collectedWord += L"л";
            else if((combination == L"ein") || (combination == L"n"))
                collectedWord += L"н";
            else if(combination == L"gn")
                collectedWord += L"нь";
            else if((combination == L"au") || (combination == L"eau") || (combination == L"o") ||
                    (combination == L"ô"))
                collectedWord += L"о";
            else if((combination == L"ë") || (combination == L"oë"))
                collectedWord += L"оэ";
            else if(combination == L"r")
                collectedWord += L"р";
            else if(combination == L"rr")
                collectedWord += L"рр";
            else if(combination == L"ç")
                collectedWord += L"с";
            else if(combination == L"ss")
                collectedWord += L"сс";
            else if((combination == L"tieu") || (combination == L"tieux"))
                collectedWord += L"тье";
            else if((combination == L"oi") || (combination == L"oî") || (combination == L"oie") || 
                    (combination == L"oy"))
                collectedWord += L"уа";
            else if(combination == L"oin")
                collectedWord += L"уэн";
            else if((combination == L"f") || (combination == L"ph"))
                collectedWord += L"ф";
            else if((combination == L"ch") || (combination == L"sch"))
                collectedWord += L"ш";
            else if((combination == L"u") || (combination == L"û"))
                collectedWord += L"ю";
            else if(combination == L"h")
                collectedWord += L"";
            else if((combination == L"a") || (combination == L"à") || (combination == L"â"))
            {
                bool done = false;
                if(combination == L"a")
                {
                    if((prevComb == L"gn") || (prevComb == L"ll") || (prevLetter == L"y") ||
                       (prevLetter == L"i"))
                    {
                        collectedWord += L"я";
                        done = true;
                    }
                }
                if(!done)
                    collectedWord += L"а";
            }
            else if(combination == L"ai")
            {
                if((position == 0) || prevIsVowel)
                    collectedWord += L"э";
                else
                    collectedWord += L"е";
            }
            else if((combination == L"aim") || (combination == L"ain"))
            {
                if((position == 0) || (!prevIsVowel && (prevLetter != L"b") && (prevLetter != L"p") && 
                                       (prevLetter != L"f") && (prevLetter != L"m")))
                    collectedWord += L"эн";
                else if((prevLetter == L"b") || (prevLetter == L"p"))
                    collectedWord += L"эм";
                else
                    collectedWord += L"ен";
            }
            else if(combination == L"am")
            {
                if((combinationEnd == wordEnd) || 
                   (!prevIsVowel && (prevLetter != L"b") && (prevLetter != L"p") && (prevLetter != L"f") && 
                    (prevLetter != L"m")))
                    collectedWord += L"ан";
                else if((prevLetter == L"b") || (prevLetter == L"p"))
                    collectedWord += L"ам";
                else
                    notDone = true;
            }
            else if(combination == L"ault")
            {
                if(combinationEnd == wordEnd)
                    collectedWord += L"о";
                else
                    notDone = true;
            }
            else if(combination == L"ay")
            {
                if((!nextIsVowel) && (combinationEnd >= wordEnd - 1))
                    collectedWord += L"еи";
                else if((position == 0) || prevIsVowel)
                    collectedWord += L"э";
                else
                    collectedWord += L"е";
            }
            else if(combination == L"c")
            {
                if((prevLetter == L"n") && (combinationEnd == wordEnd))
                    collectedWord += L"";
                else if((nextLetter == L"a") || (nextLetter == L"o") || (nextLetter == L"u") ||
                        !nextIsVowel || (combinationEnd == wordEnd))
                    collectedWord += L"к";
                else if((nextLetter == L"e") || (nextLetter == L"i") || (nextLetter == L"y"))
                    collectedWord += L"с";
                else
                    notDone = true;
            }
            else if(combination == L"cc")
            {
                if((nextLetter == L"a") || (nextLetter == L"o") || (nextLetter == L"u") || !nextIsVowel)
                    collectedWord += L"кк";
                else if((nextLetter == L"e") || (nextLetter == L"i") || (nextLetter == L"y"))
                    collectedWord += L"кс";
                else
                    notDone = true;
            }
            else if((combination == L"ct") || (combination == L"cts"))
            {
                if((combinationEnd == wordEnd) && (position >= 3))
                   collectedWord += L"";
                else
                    notDone = true;
            }
            else if(combination == L"d")
            {
                if(combinationEnd == wordEnd)
                    collectedWord += L"";
                else
                    collectedWord += L"д";
            }
            else if(combination == L"e")
            {
                if((combinationEnd == wordEnd) || ((nextLetter == L"s") && (combinationEnd == wordEnd - 1)) ||
                   (prevIsVowel != nextIsVowel))
                    collectedWord += L"";
                else if((position == 0) && (prevIsVowel))
                    collectedWord += L"э";
                else
                    collectedWord += L"е";
            }
            else if(combination == L"é")
            {
                if((position == 0) || prevIsVowel)
                    collectedWord += L"э";
                else
                    notDone = true;
            }
            else if((combination == L"ei") || (combination == L"eî"))
            {
                if(position == 0)
                    collectedWord += L"э";
                else if(!prevIsVowel)
                    collectedWord += L"е";
                else
                    notDone = true;
            }
            else if((combination == L"eil") || (combination == L"eille"))
            {
                if(combinationEnd == wordEnd)
                    collectedWord += L"ей";
                else
                    notDone = true;
            }
            else if(combination == L"em")
            {
                if((!nextIsVowel && (nextLetter != L"b") && (nextLetter != L"p")) ||
                   (combinationEnd == wordEnd))
                    collectedWord += L"ан";
                else if((nextLetter == L"b") || (nextLetter == L"p"))
                    collectedWord += L"ам";
                else if((position == 0) && nextIsVowel)
                    collectedWord += L"эм";
                else
                    collectedWord += L"ем";
            }
            else if(combination == L"en")
            {
                if(!nextIsVowel || (combinationEnd == wordEnd))
                    collectedWord += L"ан";
                else if((position == 0) && nextIsVowel)
                    collectedWord += L"эм";
                else
                    collectedWord += L"ем";
            }
            else if((combination == L"eu") || (combination == L"eû"))
            {
                if((position == 0) || prevIsVowel)
                    collectedWord += L"эн";
                else
                    collectedWord += L"ё";
            }
            else if(combination == L"euil")
            {
                if(combinationEnd == wordEnd)
                    collectedWord += L"ей";
                else
                    notDone = true;
            }
            else if(combination == L"ey")
            {
                if((position == 0) || prevIsVowel)
                    collectedWord += L"э";
                else
                    collectedWord += L"е";
            }
            else if(combination == L"g")
            {
                if((nextLetter == L"e") || (nextLetter == L"i") || (nextLetter == L"y"))
                    collectedWord += L"ж";
                else if((combinationEnd == wordEnd) && !prevIsVowel)
                    collectedWord += L"";
                else if((nextLetter == L"a") || (nextLetter == L"o") || (nextLetter == L"u") ||
                        (!prevIsVowel && (prevLetter != L"n")))
                    collectedWord += L"г";
                else
                    notDone = true;
            }
            else if(combination == L"ge")
            {
                if((nextLetter == L"a") || (nextLetter == L"o") || (nextLetter == L"u"))
                    collectedWord += L"ж";
                else
                    notDone = true;
            }
            else if(combination == L"gg")
            {
                if((nextLetter == L"a") || (nextLetter == L"o") || !nextIsVowel)
                    collectedWord += L"г";
                else if(nextLetter == L"e")
                    collectedWord += L"гж";
                else
                    notDone = true;
            }
            else if(combination == L"gu")
            {
                if((nextLetter == L"e") || (nextLetter == L"i") || (nextLetter == L"y"))
                    collectedWord += L"г";
                else
                    notDone = true;
            }
            else if((combination == L"i") || (combination == L"î") || (combination == L"ï"))
            {
                bool done = false;
                if(combination == L"i")
                {
                    if(prevIsVowel != nextIsVowel)
                    {
                        collectedWord += L"ь";
                        done = true;
                    }
                }
                if(!done)
                    collectedWord += L"и";
            }
            else if((combination == L"ier") || (combination == L"ieu"))
            {
                if(prevIsVowel)
                    collectedWord += L"йе";
                else if((combinationEnd == wordEnd) && !prevIsVowel)
                    collectedWord += L"ье";
                else
                    notDone = true;
            }
            else if(combination == L"il")
            {
                if(prevIsVowel)
                    collectedWord += L"й";
                else
                    notDone = true;
            }
            else if(combination == L"ill")
            {
                if(prevIsVowel && nextIsVowel)
                    collectedWord += L"й";
                else if(prevIsVowel != nextIsVowel)
                    collectedWord += L"ий";
                else if((prevLetter == L"m") || (prevLetter == L"v"))
                    collectedWord += L"л";
                else
                    notDone = true;
            }
            else if(combination == L"ille")
            {
                if((combinationEnd == wordEnd) && prevIsVowel)
                    collectedWord += L"й";
                else if((prevLetter == L"m") || (prevLetter == L"v"))
                    collectedWord += L"иль";
                else
                    notDone = true;
            }
            else if((combination == L"im") || (combination == L"ym"))
            {
                if(nextIsVowel || (nextLetter == L"m"))
                    collectedWord += L"им";
                else if((nextLetter == L"b") || (nextLetter == L"p"))
                    collectedWord += L"эм";
                else
                    collectedWord += L"эн";
            }
            else if((combination == L"in") || (combination == L"yn"))
            {
                if(nextIsVowel || (nextLetter == L"n"))
                    collectedWord += L"ин";
                else if((position == 0) && !nextIsVowel)
                    collectedWord += L"эн";
                else
                    collectedWord += L"ен";
            }
            else if(combination == L"ld")
            {
                if((prevComb == L"au") || (prevComb == L"ou"))
                    collectedWord += L"";
                else
                    notDone = true;
            }
            else if(combination == L"lh")
            {
                if(prevLetter == L"i")
                    collectedWord += L"й";
                else if(prevIsVowel)
                    collectedWord += L"л";
                else
                    notDone = true;
            }
            else if(combination == L"ll")
            {
                if((prevLetter == L"i") && nextIsVowel)
                    collectedWord += L"й";
                else
                    notDone = true;
            }
            else if(combination == L"llier")
            {
                if(prevIsVowel && (prevLetter != L"i"))
                    collectedWord += L"лье";
                else
                    notDone = true;
            }
            else if(combination == L"llon")
            {
                if(prevIsVowel && (prevLetter != L"i"))
                    collectedWord += L"льон";
                else
                    notDone = true;
            }
            else if(combination == L"m")
            {
                if(nextIsVowel || (nextLetter == L"b") || (nextLetter == L"p") || (nextLetter == L"m"))
                    collectedWord += L"м";
                else if(((combinationEnd == wordEnd) &&
                          prevIsVowel) || !nextIsVowel)
                    collectedWord += L"н";
                else
                    notDone = true;
            }
            else if((combination == L"œ") || (combination == L"œu"))
            {
                if((position == 0) || prevIsVowel)
                    collectedWord += L"э";
                else
                    collectedWord += L"ё";
            }
            else if(combination == L"om")
            {
                if(nextIsVowel || (nextLetter == L"b") || (nextLetter == L"p") || (nextLetter == L"m"))
                    collectedWord += L"ом";
                else if((combinationEnd >= wordEnd - 1) || !nextIsVowel)
                    collectedWord += L"он";
                else
                    notDone = true;
            }
            else if((combination == L"ou") || (combination == L"où") || (combination == L"oû"))
            {
                if((prevComb == L"gn") || (prevComb == L"ll") || (prevLetter == L"i"))
                    collectedWord += L"ю";
                else
                    collectedWord += L"у";
            }
            else if(combination == L"p")
            {
                if((combinationEnd >= wordEnd - 1) && (position >= 3))
                    collectedWord += L"";
                else
                    collectedWord += L"п";
            }
            else if((combination == L"que") || (combination == L"ques"))
            {
                if(combinationEnd == wordEnd)
                    collectedWord += L"к";
                else
                    notDone = true;
            }
            else if(combination == L"s")
            {
                if(prevIsVowel && nextIsVowel)
                    collectedWord += L"з";
                else if(combinationEnd == wordEnd)
                    collectedWord += L"";
                else
                    collectedWord += L"с";
            }
            else if(combination == L"sc")
            {
                if((nextLetter == L"e") || (nextLetter == L"i") || (nextLetter == L"y"))
                    collectedWord += L"с";
                else
                    collectedWord += L"ск";
            }
            else if(combination == L"t")
            {
                if(combinationEnd == wordEnd)
                    collectedWord += L"";
                else
                    collectedWord += L"т";
            }
            else if(combination == L"tion")
            {
                if(combinationEnd >= wordEnd - 2)
                    collectedWord += L"сьен";
                else
                    notDone = true;
            }
            else if(combination == L"tioux")
            {
                if(combinationEnd >= wordEnd - 2)
                    collectedWord += L"сью";
                else
                    notDone = true;
            }
            else if(combination == L"um")
            {
                if(nextIsVowel || (nextLetter == L"m"))
                    collectedWord += L"юм";
                else if((nextLetter == L"b") || (nextLetter == L"p"))
                    collectedWord += L"эм";
                else
                    collectedWord += L"юн";
            }
            else if(combination == L"un")
            {
                if(nextIsVowel || (nextLetter == L"n"))
                    collectedWord += L"юн";
                else
                    collectedWord += L"эн";
            }
            else if(combination == L"x")
            {
                if((prevComb == L"au") || (prevComb == L"eau") || (prevComb == L"eu"))
                    collectedWord += L"";
                else
                    collectedWord += L"кс";
            }
            else if(combination == L"y")
            {
                if(((position == 0) && nextIsVowel) || (prevIsVowel != nextIsVowel))
                    collectedWord += L"й";
                else if(((position == 0) && !nextIsVowel) || (!prevIsVowel & !nextIsVowel))
                    collectedWord += L"и";
                else
                    notDone = true;
            }
            else if(combination == L"z")
            {
                if(combinationEnd == wordEnd)
                    collectedWord += L"";
                else
                    collectedWord += L"з";
            }

            return !notDone;
        }
    }
}