#pragma once

namespace SS{ 
namespace Dictionary{
namespace Virtual{

/**
	Виртуальный словарь чисел записанных в формате 1st, 2nd и т.д.
*/
class CNumericOrderingDictionary: public Detail::IBasicMorphologyAnalyser 	  
{
	//Парметры которые мы узнаём у IAMConverterMorpho
   struct 
   {
      int MorphoInfo;
      int PartOfSpeech;
   } m_rgMorphoParams[1];

private:
	/// Заполняет все поля SWordInfo
	void FillSWordInfo( 
		const std::wstring &wcWord,
		unsigned int Number,
      TIDForm IDForm,
		SWordInfo &WordInfo,
      bool FirstLetterUpperCase
   ) 
	{
      APL_ASSERT( IDForm < APL_ARRSIZE(m_rgMorphoParams) && IDForm >= 0 );
      
      WordInfo.m_wWord = wcWord;
		WordInfo.m_SpecialMorpho = m_rgMorphoParams[IDForm].PartOfSpeech;
		WordInfo.m_WordIndex.m_DictIndex.SetId(Number) ; 
		WordInfo.m_WordIndex.m_DictIndex.SetDictionaryNumber(DATA_TYPE::NAMES::ednOrderingEnglish);
      WordInfo.m_WordIndex.m_DictIndex.SetFirstLetterUpperCase(FirstLetterUpperCase); 
		WordInfo.m_WordIndex.m_IDForm = IDForm; 
		WordInfo.m_MorphoTypeInfo.m_IDType = 0;
		WordInfo.m_MorphoTypeInfo.m_MorphoInfo = m_rgMorphoParams[IDForm].MorphoInfo;
	}

	///Заполняет буфер строковой интерпритацией индекса в словаре, т.е. переводит индекс в строку
	static bool GetWordByDictIndex(
		const TDictionaryIndex &Index,
		wchar_t* wcWord
	);


public:
	CNumericOrderingDictionary() {}
	explicit CNumericOrderingDictionary( IAMConverterMorpho *pAMConverter ) { Init(pAMConverter); }

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
