#pragma once

namespace SS
{
namespace Constants
{

	//максимальное число слов в ЅЋќ ≈ (максимальное количество лексем в LexemeBoard-e)
	const int MAX_WORDS_IN_BLOCK = 50000;
	//макс кол-во символов в слове
	const int MAX_CHARS_IN_WORD = 50;
	//максимальное число слов в предложении
	const int MAX_WORDS_IN_SENTENCE = 255;
	//макс колво предложений в параграфе
	const long MAX_SENTS_IN_PARAGRAPH = 256;

	//макс значение числительного, дл€ которого проставл€етс€ морфологи€ из базы
	const long MAX_NUMERIC_VALUE = 0x1000000;
	//макс значение числительного, начинающегос€ с нулей
	const int MAX_FLOAT_NUMERIC_VALUE = 0x200000;
	//макс количество нулей в начале числительного
	const int MAX_ZERO_COUNT = 8;

	//максимальное число, воспринимаемое как год
	const int MAX_YEAR_NUMBER = 2500;

	const int MIN_YEAR_NUMBER = 1800;

	//число, на которое умножаетс€ релевантность
	const int RELEVANCE_MULTIPLIER = 1000000;
	//число, на которое умножаетс€ уровень нечеткости
	const int FUZZY_LEVEL_MULTIPLIER = 100000000;

	////часть DictionaryIndex-а, которую надо сравнивать в релевантности
	//const unsigned int SIGNIFICANT_INDEX_PART = 0x7FFFFFFF;

	namespace Dictionary
	{
		//ћинимальна€ длина слова морфологи€ которого ещЄ может разпознаватьс€
		const int MIN_MORPHO_RECOGNIZABLE_WORD_LEN_RUS = 4; //ƒл€ русского €зыка
		const int MIN_MORPHO_RECOGNIZABLE_WORD_LEN_ENG = 3; //ƒл€ английского €зыка

		//ћинимальное количество совпавших с конца букв у двух слов при котором 
		//можно сказать что по одному слову можно определить морфологию другого
		const int MIN_COINCIDENCE_FOR_MORPHO_RECOGNIZABLE = 2;
	}
}
}
