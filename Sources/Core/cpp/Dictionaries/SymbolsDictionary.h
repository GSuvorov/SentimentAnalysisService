#pragma once

namespace SS{ 
namespace Dictionary{
namespace Virtual{

/**
	Словарь распозанёт комбинацию "<BR>" и отдельный символ
*/
class CSymbolsDictionary: public Detail::IBasicMorphologyAnalyser  
{
	static const unsigned int ciBRIndex;
	static const wchar_t *const csBR;

private:
	//Парметры которые мы узнаём у IAMConverterMorpho
	int m_MorphoInfo;
	int m_PartOfSpeech;


private:
	/// Заполняет все поля SWordInfo
	void FillSWordInfo( 
		const std::wstring &wcWord,
		unsigned int Number,
		SWordInfo &WordInfo,
      bool FirstLetterUpperCase
		) 
	{
		WordInfo.m_wWord = wcWord;
		WordInfo.m_WordIndex.m_DictIndex.SetId(Number) ; 
		WordInfo.m_WordIndex.m_DictIndex.SetDictionaryNumber(DATA_TYPE::NAMES::ednSymbols);
      WordInfo.m_WordIndex.m_DictIndex.SetFirstLetterUpperCase(FirstLetterUpperCase); 
		WordInfo.m_WordIndex.m_IDForm = 0; 
		WordInfo.m_MorphoTypeInfo.m_IDType = 0;
      
      if( Number >= _T('А') && Number <= _T('я') )
      {
         WordInfo.m_SpecialMorpho = WordInfo.m_MorphoTypeInfo.m_MorphoInfo = 0;
      }
      else
      {
         WordInfo.m_SpecialMorpho = m_PartOfSpeech;
		   WordInfo.m_MorphoTypeInfo.m_MorphoInfo = m_MorphoInfo;
      }
	}

	///Заполняет буфер строковой интерпритацией индекса в словаре, т.е. переводит индекс в строку
	static bool GetWordByDictIndex(
		const TDictionaryIndex &Index,
		wchar_t* wcWord
	);										   

	//Преобразовывает если возможно wcWord в символ
	static bool ConvertToSymbol( const wchar_t* wcWord, unsigned int &N );

public:
	CSymbolsDictionary(): m_MorphoInfo(0), m_PartOfSpeech(0) {}
	explicit CSymbolsDictionary( IAMConverterMorpho *pAMConverter ): m_MorphoInfo(0), m_PartOfSpeech(0) { Init(pAMConverter); }

	//Инициализация даных класса
	void Init( IAMConverterMorpho *pAMConverter );
			  
	
	///////////////////////////////////////////////////////////////////////////////
	//IMorphologyAnalyser

	/// проверка наличия слова в словаре
	bool IsExistWord(/*in*/const wchar_t* wcWord);

   /// поиск полной морфологической информации по слову 
   bool GetWordInfo/*ByWord*/(
      /*in*/const wchar_t* wcWord, 
      /*in*/const wchar_t* wcWordOriginalCase,
      /*in*/bool IsFirstLetterUpperCase,
      /*out*/std::list<TWordInfo>* p_l_WordInfo
      );

	/// поиск полной морфологической информации по индексу слова
	bool GetWordInfo/*ByWordIndex*/(
		/*in*/SWordIndex* pWordIndex, 
		/*out*/SWordInfo* pWordInfo
		);


	/// поиск слова по его индексу 
	/// wcWord - массив с количеством элементов MAX_CHARS_IN_WORD
	bool GetWord(
		/*in*/SWordIndex* pWordIndex,
		/*out*/wchar_t* wcWord
		);

   /// поиск основы слова слова (части слова без оканчания) по его индексу 
   /// wcWord - массив с количеством элементов MAX_CHARS_IN_WORD
   bool GetWordSource(
      /*in*/const TDictionaryIndex &DictIndex,
      /*out*/wchar_t* wcWordSource
   );

	/// поиск словоформ слова по индексу основы
	bool GetWordFormsByIDSource(
		/*in*/TDictionaryIndex* pDictionaryIndex,
		/*out*/std::list<TWordInfo>* p_l_WordInfo
		);
};



} //namespace Virtual
} //namespace Dictionary
} //namespace SS
