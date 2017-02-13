#pragma once

namespace SS{ 
namespace Dictionary{
namespace Virtual{

/**
	Виртуальный словарь целых чисел беззнаковых чисел от 0 до 2^24 - 1.
*/
class CNumberDictionary: public Detail::IBasicMorphologyAnalyser 	  
{
	//Парметры которые мы узнаём у IAMConverterMorpho
	struct 
   {
      int MorphoInfo;
	   int PartOfSpeech;
   } m_rgMorphoParamsNormal[2], m_rgMorphoParamsFloating[1];

private:
	/// Заполняет все поля SWordInfo
	void FillSWordInfoNormal( 
		const std::wstring &wcWord,
		unsigned int Number,
      unsigned int ZeroCount,
      TIDForm IDForm,
		SWordInfo &WordInfo
		) 
	{
      WordInfo.m_wWord = wcWord;
      WordInfo.m_WordIndex.m_IDForm = IDForm; 
      WordInfo.m_MorphoTypeInfo.m_IDType = 0; 		

      APL_ASSERT( IDForm < APL_ARRSIZE(m_rgMorphoParamsNormal) && IDForm >= 0 );
      WordInfo.m_SpecialMorpho = m_rgMorphoParamsNormal[IDForm].PartOfSpeech;
      WordInfo.m_MorphoTypeInfo.m_MorphoInfo = m_rgMorphoParamsNormal[IDForm].MorphoInfo;
      WordInfo.m_WordIndex.m_DictIndex.SetId(Number); 
		WordInfo.m_WordIndex.m_DictIndex.SetDictionaryNumber(DATA_TYPE::NAMES::ednNumeric); 
      
      WordInfo.m_WordIndex.m_DictIndex.SetFirstLetterUpperCase( false );
	}


   void FillSWordInfoFloating( 
      const std::wstring &wcWord,
      unsigned int Number,
      unsigned int ZeroCount,
      TIDForm IDForm,
      SWordInfo &WordInfo
      ) 
   {

      WordInfo.m_wWord = wcWord;
      WordInfo.m_WordIndex.m_IDForm = IDForm; 
      WordInfo.m_MorphoTypeInfo.m_IDType = 0; 		

      APL_ASSERT( IDForm < APL_ARRSIZE(m_rgMorphoParamsFloating) && IDForm >= 0 );
      WordInfo.m_SpecialMorpho = m_rgMorphoParamsFloating[IDForm].PartOfSpeech;
      WordInfo.m_MorphoTypeInfo.m_MorphoInfo = m_rgMorphoParamsFloating[IDForm].MorphoInfo;
      WordInfo.m_WordIndex.m_DictIndex.SetId( (ZeroCount << 21) | Number ); 
      WordInfo.m_WordIndex.m_DictIndex.SetDictionaryNumber(DATA_TYPE::NAMES::ednFloatPointNumeric); 

      WordInfo.m_WordIndex.m_DictIndex.SetFirstLetterUpperCase( false );
   }

   void FillSWordInfoNormal( 
      const std::wstring &wcWord,
      TDictionaryIndex DictIndex,
      TIDForm IDForm,
      SWordInfo &WordInfo
   ) 
   {
      APL_ASSERT( IDForm < APL_ARRSIZE(m_rgMorphoParamsNormal) && IDForm >= 0 );
      
      WordInfo.m_wWord = wcWord;
      WordInfo.m_SpecialMorpho = m_rgMorphoParamsNormal[IDForm].PartOfSpeech;
      WordInfo.m_WordIndex.m_IDForm = IDForm; 
      WordInfo.m_MorphoTypeInfo.m_IDType = 0; 		
      WordInfo.m_MorphoTypeInfo.m_MorphoInfo = m_rgMorphoParamsNormal[IDForm].MorphoInfo;
      WordInfo.m_WordIndex.m_DictIndex.SetId(DictIndex.GetId()); 
      WordInfo.m_WordIndex.m_DictIndex.SetDictionaryNumber(DictIndex.GetDictionaryNumber()); 
      WordInfo.m_WordIndex.m_DictIndex.SetFirstLetterUpperCase(false); 
   }
   
   void FillSWordInfoFloating( 
      const std::wstring &wcWord,
      TDictionaryIndex DictIndex,
      TIDForm IDForm,
      SWordInfo &WordInfo
      ) 
   {
      APL_ASSERT( IDForm < APL_ARRSIZE(m_rgMorphoParamsFloating) && IDForm >= 0 );

      WordInfo.m_wWord = wcWord;
      WordInfo.m_SpecialMorpho = m_rgMorphoParamsFloating[IDForm].PartOfSpeech;
      WordInfo.m_WordIndex.m_IDForm = IDForm; 
      WordInfo.m_MorphoTypeInfo.m_IDType = 0; 		
      WordInfo.m_MorphoTypeInfo.m_MorphoInfo = m_rgMorphoParamsFloating[IDForm].MorphoInfo;
      WordInfo.m_WordIndex.m_DictIndex.SetId(DictIndex.GetId()); 
      WordInfo.m_WordIndex.m_DictIndex.SetDictionaryNumber(DictIndex.GetDictionaryNumber()); 
      WordInfo.m_WordIndex.m_DictIndex.SetFirstLetterUpperCase(false); 
   }


	///Заполняет буфер строковой интерпритацией индекса в словаре, т.е. переводит индекс в строку
	static bool GetWordByDictIndex(
		const TDictionaryIndex &Index,
		wchar_t* wcWord
	);


public:
	CNumberDictionary() {}
	explicit CNumberDictionary( IAMConverterMorpho *pAMConverter ) { Init(pAMConverter); }

	//Инициализация даных класса необходимо вызывать  перед использованием
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
