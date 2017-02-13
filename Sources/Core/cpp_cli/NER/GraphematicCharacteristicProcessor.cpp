#include "GraphematicCharacteristicProcessor.h"

using System::Collections::Generic::IEnumerable;
using System::Collections::Generic::List;
using namespace System::Text::RegularExpressions;

// Запустить обработку слова word
// @word - Слово для обработки
// @return - Результат обработки
IEnumerable<System::String^>^ NER::GraphematicCharacteristicProcessor::Run(System::String^ word)
{
    _digitsCount = 0;
    _capitalCount = 0;
    _defisCount = 0;
    _lowercaseLettersCount = 0;
    _pointCount = 0;
    _romanNumberCount = 0;

    CalculateCounts(word);

    System::String^ prop = GetProperty(word);

	List<System::String^>^ resultList = gcnew List<System::String^>();
	resultList->Add(prop);
	resultList->Add("O");

    return  resultList;
}

// Получить признак, соответствующий рассматриваемому слову
// @word - рассматриваемое слово
// @return - Признак
System::String^ NER::GraphematicCharacteristicProcessor::GetProperty(System::String^ word)
{
	bool isLatin = IsLatin(word);
	bool isFirstUpper = (word->Length > 1) && System::Char::IsUpper(word[0]);
    if ((_capitalCount != 0) && (_lowercaseLettersCount == 0) && (_pointCount == 0))
    {
        if (_digitsCount != 0)
            return NumCapital;
        if (word->Length == 1)
            return OneCapital;
        if (_romanNumberCount == word->Length)
            return LatCapital;
		if (isLatin)
            return AllLatCapital;
        if(_digitsCount == 0)
            return AllCapital;
    }

    if (_lowercaseLettersCount == 0)
    {
        if ((word->Length == 2) && (_capitalCount == 1) && (_pointCount == 1))
            return OneCapitalPoint;
        if ((_capitalCount > 1) && (_capitalCount + _pointCount == word->Length))
            return AllCapitalPoint;
    }

    if (isFirstUpper && (_capitalCount == 1) && (word->Length > 1) && (word[1] != '.') && (_defisCount == 0))
    {
        if (isLatin)
            return FirstLatCapital;
        return FirstCyrillicCapital;
    }

    if (isFirstUpper && (word->Length > 1) && (word[1] == '.'))
        return FirstCapitalPoint;

    if (IsLowercasePoint(word))
        return LowercasePoint;

    if ((word == "\"") || (word == "«") || (word == "“") || (word == "”") || (word == "»"))
        return Quote;

	if(isLatin && word->Length > 2 && System::Char::IsLower(word[0]) && (_capitalCount != 0) && (_pointCount == 0))
		return iProd;

    if ((_capitalCount != 0) && (_lowercaseLettersCount != 0))
        return MixCapitalPoint;

    return  Other;
}

// Является признаком LowercasePoint?
// @word - рассматриваемое слово
bool NER::GraphematicCharacteristicProcessor::IsLowercasePoint(System::String^ word)
{
    int firstPtIndex = word->IndexOf('.');
    if(firstPtIndex != -1)
    {
        int nextIndex = firstPtIndex + 1;
        if(nextIndex < word->Length && System::Char::IsUpper(word[nextIndex]))
		{
			for(int i = 0; i < word->Length; ++i)
			{
				if(!(System::Char::IsLetter(word[i]) && System::Char::IsLower(word[i])))
					return (word[i] == '.');
			}
		}
    }
    return false;
}

// Слово на латыни?
// @word - рассматриваемое слово
bool NER::GraphematicCharacteristicProcessor::IsLatin(System::String^ word)
{
    Regex^ regex = gcnew Regex("[a-zA-Z-]+");
    Match^ match = regex->Match(word);
    return match->Value->Length == word->Length;
}

// Римская цифра?
// @ch - Рассматриваемый символ
bool NER::GraphematicCharacteristicProcessor::IsRomanSymbol(wchar_t ch)
{
    if (ch == L'I' || ch == L'V' ||
        ch == L'X' || ch == L'L' ||
        ch == L'C' || ch == L'D' ||
        ch == L'M')
        return true;
    return false;
}

// Вычислить количество заглавных и прописных букв, цифр, римских цифр и точек в слове.
// @word - Рассматриваемое слово
void NER::GraphematicCharacteristicProcessor::CalculateCounts(System::String^ word)
{
    for (int i = 0; i < word->Length; ++i)
    {
        if (System::Char::IsDigit(word[i]))
            _digitsCount++;
        else if (System::Char::IsLower(word[i]))
            _lowercaseLettersCount++;
        else if (System::Char::IsUpper(word[i]))
        {
            _capitalCount++;
            if (IsRomanSymbol(word[i]))
                _romanNumberCount++;
        }
        else if (word[i] == '.')
            _pointCount++;
        else if (word[i] == '-')
            _defisCount++;
    }
}