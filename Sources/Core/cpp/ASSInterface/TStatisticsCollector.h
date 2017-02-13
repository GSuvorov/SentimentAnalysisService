#pragma once

#include "..\\libs\\Serialization.hpp"
#include "IXML.h"
#include <list>

namespace SS
{
namespace Statistics
{
namespace Types
{
	using SS::Lib::SetSerializeXMLString;
   using SS::Lib::CWriteArchive;


   //Получить по классу Object в котором определен метод Serialize XML строку OutXML с 
   //названием корневого тега RootName
   //Отличается от SS::Lib::SetSerializeXMLString тем что использует 
   //точность 3 знака после запятой
   template< class Object, class CharT >
   void GetSerializeXMLString( Object &Object, const CharT *RootName, std::basic_string<CharT> &OutXML )
   {
      std::basic_stringstream<CharT> Stream;
      CWriteArchive<CharT> Archive(Stream);
      Stream.precision(3);
      Stream.flags( std::ios::fixed );
      Archive.Save( RootName, Object );
      OutXML = Stream.str();
   }

	
	/// текст (используется для подачи текста запроса, также служит базовым контейнером для текста индексации)
	class TNativeSymbols
	{
		const wchar_t* m_wcSymbols;
	public:
		TNativeSymbols() : m_wcSymbols(0)	{};
		/// вернет текст
		const wchar_t* GetSymbols()
		{
			return m_wcSymbols;
		}
		/// примет текст
		void SetSymbols(const wchar_t* wc)
		{
			m_wcSymbols = wc;
		}
	};


	class TResultType
	{
		int		m_iGlobal;
		int		m_iUnique;
		int		m_iCombinations;
		double	m_dPersent;

	public:

		TResultType() : m_iGlobal(0), m_iUnique(0), m_iCombinations(0), m_dPersent(0.0) {}

		int    GetGlobalNumber() { return m_iGlobal; }
		void   SetGlobalNumber(int iGlobal) { m_iGlobal = iGlobal; }

		int    GetUniqueNumber() { return m_iUnique; }
		void   SetUniqueNumber(int iUnique) { m_iUnique = iUnique; }

		int    GetCombinationsNumber() { return m_iCombinations; }
		void   SetCombinationsNumber(int iCombinations) { m_iCombinations = iCombinations; }

		double GetResultsInPercent() { return m_dPersent; }
		void   SetResultsInPercent(double	dPersent) { m_dPersent = dPersent; }

		template< class Archive > void Serialize( Archive &A ) {
			A.Element( _T("Global"),       m_iGlobal );
			A.Element( _T("Unique"),       m_iUnique );
			A.Element( _T("Combinations"), m_iCombinations );
			A.Element( _T("Persent"),      m_dPersent );
		}
	};

	class TResultType_GP
	{
		int		m_iGlobal;
		double	m_dPersent;

	public:

		TResultType_GP() : m_iGlobal(0), m_dPersent(0.0) {}

		int    GetGlobalNumber() { return m_iGlobal; }
		void   SetGlobalNumber(int iGlobal) { m_iGlobal = iGlobal; }

		double GetResultsInPercent() { return m_dPersent; }
		void   SetResultsInPercent(double	dPersent) { m_dPersent = dPersent; }

		template< class Archive > void Serialize( Archive &A ) {
			A.Element( _T("Global"),       m_iGlobal );
			A.Element( _T("Persent"),      m_dPersent );
		}
	};

	/// Информация о подстроке
	struct TSymbolsSequenceInfo   
	{
		/// Подстрока
		std::wstring Sequence;

		/// Колличества
		TResultType Values;

		void GetXML(std::wstring * pwXML) {       GetSerializeXMLString(*this, _T("SymbolsSequenceInfo"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("SymbolsSequenceInfo"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.Element( _T("Sequence"), Sequence );
			A.Group(   _T("Values"),     Values );
		}
	};

	/// Список с информацией о подстроке
	struct SSymbolsSequenceInfoList   
	{
		std::list< TSymbolsSequenceInfo >           m_ListOfSymbolsSequenceInfo;		
		std::list< TSymbolsSequenceInfo >::iterator m_ListItr;

		void GetXML(std::wstring * pwXML) {       GetSerializeXMLString(*this, _T("SymbolsSequenceInfoList"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("SymbolsSequenceInfoList"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.STLContainerGroup( _T("SymbolsSequenceInfoList"), _T("Item"), m_ListOfSymbolsSequenceInfo );
		}
	};


	/// Информация о части речи
	struct TPOSInfo   
	{
		/// Слово
		std::wstring Word;

		/// Часть речи
		std::wstring POSName;

		/// Колличество
		UINT         Number;

		void GetXML(std::wstring * pwXML) {       GetSerializeXMLString(*this, _T("POSInfo"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("POSInfo"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.Element( _T("Word"),    Word );
			A.Element( _T("POSName"), POSName );
			A.Element( _T("Number"),  Number );
		}
	};

	/// Список с информацией о частях речи
	struct SPOSInfoList   
	{
		std::list< TPOSInfo >           m_ListOfPOSInfo;		
		std::list< TPOSInfo >::iterator m_ListItr;		

		void GetXML(std::wstring * pwXML) {       GetSerializeXMLString(*this, _T("POSInfoList"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("POSInfoList"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.STLContainerGroup( _T("POSInfo"), _T("Item"), m_ListOfPOSInfo );
		}
	};

	// ---------------------------------------------------------------------------------

	// NEW 20_04_06 (begin)
	/// Информация порядке слов в тексте и их частях речи
	struct TOrderInfo   
	{
		/// Слово
		std::wstring Word;

		/// Часть речи
		std::wstring POSName;

		void GetXML(std::wstring * pwXML) {       GetSerializeXMLString(*this, _T("OrderInfo"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("OrderInfo"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.Element( _T("Word"),    Word );
			A.Element( _T("POSName"), POSName );
		}
	};

	/// Список с информацией о порядке слов в тексте и их частях речи
	struct SOrderInfoList   
	{
		std::list< TOrderInfo >           m_ListOfOrderInfo;		
		std::list< TOrderInfo >::iterator m_ListItr;		

		void GetXML(std::wstring * pwXML) {       GetSerializeXMLString(*this, _T("OrderInfoList"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("OrderInfoList"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.STLContainerGroup( _T("OrderInfo"), _T("Item"), m_ListOfOrderInfo );
		}
	};
	// NEW 20_04_06 (end)



	/// параметры содежащие информацию о колличестве определенных частей речи в тексте
	class TPartOfSpeachParameters : public SS::Interface::Common::IXMLStorage
	{
		TResultType m_VerbsAll;
		TResultType m_VerbsActive;
		TResultType m_VerbsPassive;
		TResultType m_VerbsActiveIndef;
		TResultType m_VerbsTransitive;
		TResultType m_VerbsIntransitive;
		TResultType m_VerbsTransitIndef;
		TResultType m_VerbsPast;
		TResultType m_VerbsPresent;
		TResultType m_VerbsFuture;
		TResultType m_VerbsUndefTime;

		TResultType m_VerbsParticiple;
		TResultType m_VerbsDeepr;
		TResultType m_VerbsPersonal;
		TResultType m_VerbsInfinitive;
		TResultType m_VerbsImperative;

		TResultType m_NounsAll;
		TResultType m_NounsNominal;
		TResultType m_NounsProper;
		TResultType m_NounsAnimate;
		TResultType m_NounsInanimate;
		TResultType m_NounsUndefAnimate;

		TResultType m_Adjectives;
		TResultType m_Numerals;
		TResultType m_Numbers;
		TResultType m_Adverbs;
		TResultType m_Pronouns;
		TResultType m_Prepositions;
		TResultType m_Conjunctions;
		TResultType m_Particles;
		TResultType m_Interjections;
		TResultType m_Predicatives;
		TResultType m_Symbols;
		TResultType m_DifferentPartOfSpeaches;
//		TResultType m_ForeignWords;
		TResultType m_UnidentifiedWords;

	public:

		/// Число всех глаголов                    
		TResultType* GetVerbsAll()      { return &m_VerbsAll; }

		/// Число активных глаголов          
		TResultType* GetVerbsActive()   { return &m_VerbsActive; }

		/// Число пассивных глаголов          
		TResultType* GetVerbsPassive()  { return &m_VerbsPassive; }

		/// Число глаголов с неопределенной активностью          
		TResultType* GetVerbsActiveIndef()   { return &m_VerbsActiveIndef; }

		/// Число переходных глаголов          
		TResultType* GetVerbsTransitive()   { return &m_VerbsTransitive; }

		/// Число непереходных глаголов          
		TResultType* GetVerbsIntransitive()  { return &m_VerbsIntransitive; }

		/// Число глаголов с неопределенной переходностью 
		TResultType* GetVerbsTransitIndef()   { return &m_VerbsTransitIndef; }

		/// Число глаголов прошедшего времени
		TResultType* GetVerbsPast()  { return &m_VerbsPast; }

		/// Число глаголов настоящего времени
		TResultType* GetVerbsPresent()  { return &m_VerbsPresent; }

		/// Число глаголов будущего времени
		TResultType* GetVerbsFuture()  { return &m_VerbsFuture; }

		/// Число глаголов с неопределенным временем
		TResultType* GetVerbsUndefTime()  { return &m_VerbsUndefTime; }

		// NEW 3_04_06 (begin)

			// Число глаголов в форме причастия
			TResultType* GetVerbsParticiple()  { return &m_VerbsParticiple; }

			// Число глаголов в форме деепричастия
			TResultType* GetVerbsDeepr()  { return &m_VerbsDeepr; }

			// Число глаголов в личной форме 
			TResultType* GetVerbsPersonal()  { return &m_VerbsPersonal; }

			// Число глаголов в форме инфинитива
			TResultType* GetVerbsInfinitive()  { return &m_VerbsInfinitive; }

			// Число глаголов в поселительной форме 
			TResultType* GetVerbsImperative()  { return &m_VerbsImperative; }

		// NEW (end)

		/// Число существительных          
		TResultType* GetNounsAll()         { return &m_NounsAll; }

		/// Число нарицательных существительных          
		TResultType* GetNounsNominal()   { return &m_NounsNominal; }

		/// Число собственных существительных          
		TResultType* GetNounsProper()   { return &m_NounsProper; }

		/// Число одушевленных существительных          
		TResultType* GetNounsAnimate()   { return &m_NounsAnimate; }

		/// Число неодушевленных существительных          
		TResultType* GetNounsInanimate()   { return &m_NounsInanimate; }

		/// Число существительных без одушевленности         
		TResultType* GetNounsUndefAnimate()   { return &m_NounsUndefAnimate; }

		/// Число прилагательных          
		TResultType* GetAdjectives()    { return &m_Adjectives; }

		/// Число числительных          
		TResultType* GetNumerals()      { return &m_Numerals; }

		/// Число чисел          
		TResultType* GetNumbers()      { return &m_Numbers; }

		/// Число наречий               
		TResultType* GetAdverbs()       { return &m_Adverbs; }

		/// Число местоимений           
		TResultType* GetPronouns()      { return &m_Pronouns; }

		/// Число предлогов             
		TResultType* GetPrepositions()  { return &m_Prepositions; }

		/// Число союзов                
		TResultType* GetConjunctions()  { return &m_Conjunctions; }

		/// Число частиц                
		TResultType* GetParticles()     { return &m_Particles; }

		/// Число междометий            
		TResultType* GetInterjections() { return &m_Interjections; }

		/// Число предикативов          
		TResultType* GetPredicatives()  { return &m_Predicatives; }

		/// Число символов        
		TResultType* GetSymbols()       { return &m_Symbols; }

		/// Число слов относящихся к разным частям речи
		TResultType* GetDifferentPartOfSpeaches()  { return &m_DifferentPartOfSpeaches; }

//		/// Число иностранных слов
//		TResultType* GetForeignWords() { return &m_ForeignWords; }

		/// Число неопознанных частей речи
		TResultType* GetUnidentifiedWords() { return &m_UnidentifiedWords; }

		//IXMLStorage
		void GetXML(std::wstring * pwXML) { GetSerializeXMLString(*this, _T("PartOfSpeachParameters"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("PartOfSpeachParameters"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.Group( _T("VerbsAll"),                m_VerbsAll );

			A.Group( _T("VerbsActive"),             m_VerbsActive );
			A.Group( _T("VerbsPassive"),            m_VerbsPassive );
			A.Group( _T("VerbsActiveInfef"),        m_VerbsActiveIndef );

			A.Group( _T("VerbsTransitive"),         m_VerbsTransitive );
			A.Group( _T("VerbsIntransitive"),       m_VerbsIntransitive );
			A.Group( _T("VerbsTransitIndef"),       m_VerbsTransitIndef );

			A.Group( _T("VerbsPast"),               m_VerbsPast );
			A.Group( _T("VerbsPresent"),            m_VerbsPresent );
			A.Group( _T("VerbsFuture"),             m_VerbsFuture );
			A.Group( _T("VerbsUndefTime"),          m_VerbsUndefTime );

			A.Group( _T("VerbsParticiple"),         m_VerbsParticiple );
			A.Group( _T("VerbsDeepr"),              m_VerbsDeepr );
			A.Group( _T("VerbsPersonal"),           m_VerbsPersonal );
			A.Group( _T("VerbsInfinitive"),         m_VerbsInfinitive );
			A.Group( _T("VerbsImperative"),         m_VerbsImperative );

			A.Group( _T("NounsAll"),                m_NounsAll );
			A.Group( _T("NounsNominal"),            m_NounsNominal );
			A.Group( _T("NounsProper"),             m_NounsProper );
			A.Group( _T("NounsAnimate"),            m_NounsAnimate );
			A.Group( _T("NounsInanimate"),          m_NounsInanimate );
			A.Group( _T("NounsUndefAnimate"),       m_NounsUndefAnimate );

			A.Group( _T("Adjectives"),              m_Adjectives );
			A.Group( _T("Numerals"),                m_Numerals );
			A.Group( _T("Numbers"),                 m_Numbers );
			A.Group( _T("Adverbs"),                 m_Adverbs );
			A.Group( _T("Pronouns"),                m_Pronouns );
			A.Group( _T("Prepositions"),            m_Prepositions );
			A.Group( _T("Conjunctions"),            m_Conjunctions );
			A.Group( _T("Particles"),               m_Particles );
			A.Group( _T("Interjections"),           m_Interjections );
			A.Group( _T("Predicatives"),            m_Predicatives );
			A.Group( _T("Symbols"),                 m_Symbols );
			A.Group( _T("DifferentPartOfSpeaches"), m_DifferentPartOfSpeaches );
//			A.Group( _T("Foreign"),                 m_ForeignWords );
			A.Group( _T("UnidentifiedWords"),       m_UnidentifiedWords );
			
		}

	};


	class TQuantitiveTextParameters : public SS::Interface::Common::IXMLStorage
	{
		TResultType m_ParagraphsCount;
		TResultType m_SentensesCount;
		TResultType m_WordsCount;
		TResultType m_WordsUnCommonCount;
		TResultType m_EndingsCount;
		TResultType m_LettersAllCount;
		TResultType m_LettersVowelsCount;
		TResultType m_LettersConsonantsCount;
//		TResultType m_LettersOther;
	public:

		/// Число параграфов                    
		TResultType* GetParagraphsCount()        { return &m_ParagraphsCount; }

		/// Число предложений 
		TResultType* GetSentensesCount()         { return &m_SentensesCount; }

		/// Число слов                    
		TResultType* GetWordsCount()             { return &m_WordsCount; }

		/// Число слов, выходящих за рамки среднестатистической компетенции
		TResultType* GetWordsUnCommonCount()     { return &m_WordsUnCommonCount; }

		/// Число окончаний                    
		TResultType* GetEndingsCount()           { return &m_EndingsCount; }

		/// Число букв и знаков слов
		TResultType* GetLettersAllCount()        { return &m_LettersAllCount; }

		/// Число гласных букв
		TResultType* GetLettersVowelsCount()     { return &m_LettersVowelsCount; }

		/// Число согласных букв
		TResultType* GetLettersConsonantsCount() { return &m_LettersConsonantsCount; }

		/// Число знаков слов ( не букв )
//		TResultType* GetLettersOtherCount()      { return &m_LettersOther; }

		/// Указатель на массив для 32-х русских букв
		// TResultType* GetArrOf32RusLetters()   { return m_ArrOf32RusLetters; };

		/// Указатель на массив индексов для 32-х русских букв
		// int * GetArrOf32RusIndexes()          { return m_ArrOf32RusIndexes; };


		//IXMLStorage
		void GetXML(std::wstring *pwXML) { GetSerializeXMLString(*this, _T("QuantitiveTextParameters"), *pwXML); }
		void SetXML(const std::wstring *pwXML) { SetSerializeXMLString(*this, _T("QuantitiveTextParameters"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.Group( _T("ParagraphsCount"),        m_ParagraphsCount );
			A.Group( _T("SentensesCount"),         m_SentensesCount );
			A.Group( _T("WordsCount"),             m_WordsCount );
			A.Group( _T("WordsUnCommonCount"),     m_WordsUnCommonCount );
			A.Group( _T("EndingsCount"),           m_EndingsCount );
			A.Group( _T("LettersAllCount"),        m_LettersAllCount );
			A.Group( _T("LettersVowelsCount"),     m_LettersVowelsCount );
			A.Group( _T("LettersConsonantsCount"), m_LettersConsonantsCount );
//			A.Group( _T("LettersOther"),           m_LettersOther );
		}
	};

	class TAverageLengthTextParameters : public SS::Interface::Common::IXMLStorage
	{
		double m_dParagraphLengthInSentences;
		double m_dSentenceLengthInWords;
		double m_dWordLengthInSyllables;
		double m_dWordLengthInSymbols;
		double m_dSyllableLengthInSymbols;
	public:
		/// Средняя длина параграфа, выраженная в предложениях
		double GetParagraphLengthInSentences(void) { return m_dParagraphLengthInSentences; }

		/// Средняя длина параграфа, выраженная в предложениях
		void SetParagraphLengthInSentences(double dParagraphLengthInSentences) { m_dParagraphLengthInSentences = dParagraphLengthInSentences; }

		/// Средняя длина предложения, выраженная в словах
		double GetSentenceLengthInWords(void) { return m_dSentenceLengthInWords; }

		/// Средняя длина предложения, выраженная в словах
		void SetSentenceLengthInWords(double dSentenceLengthInWords) { m_dSentenceLengthInWords = dSentenceLengthInWords; }

		/// Средняя длина слова, выраженная в слогах
		double GetWordLengthInSyllables(void) { return m_dWordLengthInSyllables; }

		/// Средняя длина слова, выраженная в слогах
		void SetWordLengthInSyllables(double dWordLengthInSyllables) { m_dWordLengthInSyllables = dWordLengthInSyllables; }

		/// Средняя длина слова, выраженная в символах
		double GetWordLengthInSymbols(void) { return m_dWordLengthInSymbols; }

		/// Средняя длина слова, выраженная в символах
		void SetWordLengthInSymbols(double dWordLengthInSymbols)  { m_dWordLengthInSymbols = dWordLengthInSymbols; }

		/// Средняя длина слога, выраженная в символах
		double GetSyllableLengthInSymbols(void) { return m_dSyllableLengthInSymbols; }
		/// Средняя длина слога, выраженная в символах

		void SetSyllableLengthInSymbols(double dSyllableLengthInSymbols) { m_dSyllableLengthInSymbols = dSyllableLengthInSymbols; }

		//IXMLStorage
		void GetXML(std::wstring * pwXML) { GetSerializeXMLString(*this, _T("AverageLengthTextParameters"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("AverageLengthTextParameters"), *pwXML); } 

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.Element( _T("ParagraphLengthInSentences"), m_dParagraphLengthInSentences );
			A.Element( _T("SentenceLengthInWords"), m_dSentenceLengthInWords );
			A.Element( _T("WordLengthInSyllables"), m_dWordLengthInSyllables );
			A.Element( _T("WordLengthInSymbols"), m_dWordLengthInSymbols );
			A.Element( _T("SyllableLengthInSymbols"), m_dSyllableLengthInSymbols );
		}
	};

	class TTextIndexes : public SS::Interface::Common::IXMLStorage
	{
		double m_dFleschKincaidGradeLevel;
		double m_dFleschReadingEase;
		double m_dFogIndex;
		double m_dErtelIndex;
		double m_dTreigerIndex;
		double m_dKODIndex;
		double m_dRhythmIndex;
		double m_dReflectiveIndex;

		// !!! Z-Indexes

//		double m_dZIndexSOS;
//		double m_dZIndexSs;
//		double m_dZIndexDe;
//		double m_dZIndexSi;
//		double m_dZIndexPr;
//		double m_dZIndexSu;
//		double m_dZIndexFl;
		double m_dZIndexGl;
		double m_dZIndexSu;
		double m_dZIndexAd;
		double m_dZIndexMe;
		double m_dZIndexAv;
		double m_dZIndexUIR;
		double m_dZIndexULM;
		double m_dZIndexNe;
		double m_dZIndexPr;
//		double m_dZIndexIR;
//		double m_dZIndexDF;
		double m_dZIndexLR;
		double m_dZIndexLT;

	public:

		/// Индекс Флеша
		double GetFleschKincaidGradeLevel(void)   { return m_dFleschKincaidGradeLevel; }
		void   SetFleschKincaidGradeLevel(double dFleschKincaidGradeLevel)   { m_dFleschKincaidGradeLevel = dFleschKincaidGradeLevel; }

		/// Индекс Флеша, определяющий простоту чтения
		double GetFleschReadingEase(void)   { return m_dFleschReadingEase; }
		void   SetFleschReadingEase(double dFleschReadingEase)   { m_dFleschReadingEase = dFleschReadingEase; }

		/// Индекс Фрога
		double GetFog(void)   { return m_dFogIndex; }
		void   SetFog(double dFogIndex)   { m_dFogIndex = dFogIndex; }

		/// Коэффициент Трейгера (КТ) - отношение количества глаголов к количеству прилагательных
		double GetTreigerIndex(void)   { return m_dTreigerIndex; }
		void   SetTreigerIndex(double dTreigerIndex)   { m_dTreigerIndex = dTreigerIndex; }

		/// Коэффициент опредмеченности действия (КОД) - отношение количества глаголов к количеству существительных
		double GetKODIndex(void)   { return m_dKODIndex; }
		void   SetKODIndex(double dKODIndex)   { m_dKODIndex = dKODIndex; }

		/// КД, или индекс прегнантности Эртеля, — отношение количества слов и выражений А-экспрессии (с высоким уровнем прегнантности) к общему количеству слов в единице текста.
		double GetErtelIndex(void)   { return m_dErtelIndex; }
		void   SetErtelIndex(double dErtelIndex)   { m_dErtelIndex = dErtelIndex;	}

		/// Коэффициент ритмичности (отношение числа слогов к числу слов)
		double GetRhythmIndex(void)   { return m_dRhythmIndex; }
		void   SetRhythmIndex(double dRhythmIndex)   { m_dRhythmIndex = dRhythmIndex;	}

		/// Индекс рефлексивности
		double GetReflectiveIndex(void)   { return m_dReflectiveIndex; }
		void   SetReflectiveIndex(double dReflectiveIndex)   { m_dReflectiveIndex = dReflectiveIndex; }


		// !!! Z-Indexes

		/// Показатель степени общей сложности слов. Вычислялся как отношение количества морфем к количеству слов в тексте. По данному показателю хорошо дифференцировались письменные и устные тексты по всем испытуемым, и соответственно подготовленная и неподготовленная речь. Письменная подготовленная речь оказалась в среднем почти в два раза сложнее по морфемному составу слов, чем устная неподготовленная речь. 
		//double GetZIndexSOS(void)   { return m_dZIndexSOS; }
		//void   SetZIndexSOS(double dZIndexSOS)   { m_dZIndexSOS = dZIndexSOS;	}

		/// Показатель словосложения. Определялся отношением числа корневых морфем к длине текста. Данный индекс может быть использован для дифференциации спонтанного и подготовленного устного текста.
		//double GetZIndexSs(void)   { return m_dZIndexSs; }
		//void   SetZIndexSs(double dZIndexSs)   { m_dZIndexSs = dZIndexSs;	}

		/// Показатель деривации. Определяется отношением числа словообразовательных морфов к числу словоформ в тексте. В письменных текстах и подготовленной устной речи деривационные морфемы преобладали в количественном отношении, чем в спонтанной и устной форме речи.
		//double GetZIndexDe(void)   { return m_dZIndexDe; }
		//void   SetZIndexDe(double dZIndexDe)   { m_dZIndexDe = dZIndexDe;	}

		/// Индекс словоизменения. Вычислялся как отношение числа словоизменительных морфов к числу словоупотреблений. Наибольшие различия по данному показателю проявляются между подготовленной письменной подготовленной и устной спонтанной речью.
		//double GetZIndexSi(void)   { return m_dZIndexSi; }
		//void   SetZIndexSi(double dZIndexSi)   { m_dZIndexSi = dZIndexSi;	}

		/// Индекс  префиксации. Определялся как отношение числа префиксов к числу словоформ в тексте (характеризует положение аффикса относительно корневого морфа). Наибольшие значения данного индекса характерны для письменной и подготовленной речи, наименьшие – для устной и неподготовленной речи.
		//double GetZIndexPr(void)   { return m_dZIndexPr; }
		//void   SetZIndexPr(double dZIndexPr)   { m_dZIndexPr = dZIndexPr;	}

		/// Индекс суффиксации. Определяется отношением числа суффиксов (словообразовательных и словоизменительных) к числу словоформ в тексте. По данному индексу устойчиво различаются  письменная и подготовленная речь от устной  неподготовленной речи.
		//double GetZIndexSu(void)   { return m_dZIndexSu; }
		//void   SetZIndexSu(double dZIndexSu)   { m_dZIndexSu = dZIndexSu;	}

		/// Индекс флективности. Вычислялся отношением числа флективных морфов к числу словоформ в тексте. По данному параметру устойчиво дифференцировались тексты письменной и устной речи, подготовленной и спонтанной речи.
		//double GetZIndexFl(void)   { return m_dZIndexFl; }
		//void   SetZIndexFl(double dZIndexFl)   { m_dZIndexFl = dZIndexFl;	}

		/// Индекс глагольности. Определяется отношением всех глагольных форм (предикативные формы, императив, инфинитив, причастие и деепричастие) к общему числу всех словоформ в тексте. По этому параметру устная спонтанная речь в среднем значительно превышала письменную и подготовленную устную речь.
		double GetZIndexGl(void)   { return m_dZIndexGl; }
		void   SetZIndexGl(double dZIndexGl)   { m_dZIndexGl = dZIndexGl;	}

		/// Индекс субстантивности. Определяется  отношением числа встречаемых в тексте имен существительных к общему числу словоформ в тексте. Письменные и подготовленные устные тексты значительно отличались по числу имен  существительных от устных неподготовленных текстов.
		double GetZIndexSu(void)   { return m_dZIndexSu; }
		void   SetZIndexSu(double dZIndexSu)   { m_dZIndexSu = dZIndexSu;	}

		/// Индекс адъективности. Определяется как отношение числа имен прилагательных к общему числу словоформ в тексте. Общее среднее значение составляет 0,08. По данному параметру письменная и подготовленная речь почти значительно превышали устную и неподготовленную речь.
		double GetZIndexAd(void)   { return m_dZIndexAd; }
		void   SetZIndexAd(double dZIndexAd)   { m_dZIndexAd = dZIndexAd;	}

		/// Индекс местоименности (дейктичности). Определялся как отношение числа местоименных слов (дейктических элементов: личные, неопределенные и отрицательные местоимения-существительные; притяжательные, указательные, вопросительные местоименные прилагательные; личные, возвратные, указательные местоименные наречия; местоименные числительные),  к общему числу словоформ в тексте. Устные разновидности неподготовленной речи существенно превышают письменные подготовленные тексты.
		double GetZIndexMe(void)   { return m_dZIndexMe; }
		void   SetZIndexMe(double dZIndexMe)   { m_dZIndexMe = dZIndexMe;	}

		/// Индекс автосемантичности. Определяется как отношение полнозначных слов к общему числу словоформ в тексте. Знаменательные полнозначные слова занимают в письменных текстах почти половину его объема, вторую половину которого составляют дейктические и служебные элементы. Устные неподготовленные тексты менее насыщены полнозначной лексикой, чем письменные и подготовленные тексты.
		double GetZIndexAv(void)   { return m_dZIndexAv; }
		void   SetZIndexAv(double dZIndexAv)   { m_dZIndexAv = dZIndexAv;	}

		/// Индекс употребления именительного (родительного) падежа имени существительного и местоимения существительного. Определяется как отношение количества форм именительного (родительного) падежа к общему числу всех падежных форм существительных и местоимений существительных в тексте. Этот параметр устойчиво дифференцирует письменную и устную речь. Доля именительного падежа  среди всех падежных форм в устной неподготовленной речи значительно больше, чем в письменной. При этом подготовленная речь в два раза более насыщена граммемой родительного падежа, чем спонтанная, аналогично письменная речь по сравнению с устной речью.
		double GetZIndexUIR(void)   { return m_dZIndexUIR; }
		void   SetZIndexUIR(double dZIndexUIR)   { m_dZIndexUIR = dZIndexUIR;	}

		/// Индекс употребления личных местоимений. Определяется как отношение количества личных местоимений к количеству всех дейктических элементов в тексте. Устная речь по данному параметру более насыщена личными местоимениями, чем письменная.
		double GetZIndexULM(void)   { return m_dZIndexULM; }
		void   SetZIndexULM(double dZIndexULM)   { m_dZIndexULM = dZIndexULM;	}

		/// Индекс негации. Определяется отношением числа элементов текста с семой отрицания к числу всех словоформ текста. Наименьшие значения по данному параметру отмечены в письмах официально-делового стиля речи, а  наибольше в диалогической спонтанной речи. 
		double GetZIndexNe(void)   { return m_dZIndexNe; }
		void   SetZIndexNe(double dZIndexNe)   { m_dZIndexNe = dZIndexNe;	}

		/// Индекс предикативности. Определяется отношением числа предикативных  единиц к длине текста в словоформах. В устной неподготовленной речи значения данного параметра наивысшие, наименьшие в письменной речи официально-делового стиля.
		double GetZIndexPr(void)   { return m_dZIndexPr; }
		void   SetZIndexPr(double dZIndexPr)   { m_dZIndexPr = dZIndexPr;	}

		/// Индекс интонационной расчлененности. Определяется отношением числа синтагм к общей длине текста в словоформах. Максимальное значение проявляется для не подготовленного устного текста в форме диалога.
		//double GetZIndexIR(void)   { return m_dZIndexIR; }
		//void   SetZIndexIR(double dZIndexIR)   { m_dZIndexIR = dZIndexIR;	}

		/// Индекс длины фразы. Определяется отношением числа слов в тексте к количеству выделенных в нем синтагм с интонацией завершенности.  В неподготовленной устной речи фраза в среднем короче, чем в подготовленной речи монологического характера.
		//double GetZIndexDF(void)   { return m_dZIndexDF; }
		//void   SetZIndexDF(double dZIndexDF)   { m_dZIndexDF = dZIndexDF;	}

		/// Индекс лексического разнообразия. Определяется как отношение числа разных лексем в тексте к числу всех словоформ. Квантитативный анализ текстов в отношении одного слова затруднен из-за несопоставимости лексиконов, обслуживающих разные тематические сферы и функциональные стили речи.  Наиболее высокие значения данного индекса проявляются в подготовленной и письменной речи. В спонтанной речи коммуникативный акт осуществляется в условиях острого дефицита времени, поэтому  требования к лексической и синтаксической селективности снижаются, что приводит к минимальным значениям данного показателя.  В то же время необходимо учитывать, что особенностью данного показателя является обратная зависимость его значений от объема текста в словоформах. С увеличением объема показатель снижается, с уменьшением – напротив увеличивается. Поэтому его целесообразно использовать для сравнения тематически сходных текстов примерно равных по объему словоупотреблений.
		double GetZIndexLR(void)   { return m_dZIndexLR; }
		void   SetZIndexLR(double dZIndexLR)   { m_dZIndexLR = dZIndexLR;	}

		/// Индекс лексической тривиальности. Определяется суммированием частотных значений использованных слов. В спонтанной устной речи не менее половины объема текста занимает первая сотня наиболее частых слов русского языка. Неподготовленные  устные и письменные тексты значительно более тривиальны в лексическом отношении, чем подготовленные. В целом устная речь в диалогической форме более тривиальна, чем письменная речь или подготовленный устный монолог.
		double GetZIndexLT(void)   { return m_dZIndexLT; }
		void   SetZIndexLT(double dZIndexLT)   { m_dZIndexLT = dZIndexLT;	}


		//IXMLStorage
		void GetXML(std::wstring * pwXML) { GetSerializeXMLString(*this, _T("TextIndexes"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("TextIndexes"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.Element( _T("FleschKincaidGradeLevel"), m_dFleschKincaidGradeLevel );
			A.Element( _T("FleschReadingEase"),       m_dFleschReadingEase );
			A.Element( _T("FogIndex"),                m_dFogIndex );
			A.Element( _T("ErtelIndex"),              m_dErtelIndex );
			A.Element( _T("TreigerIndex"),            m_dTreigerIndex );
			A.Element( _T("KODIndex"),                m_dKODIndex );
			A.Element( _T("RhythmIndex"),             m_dRhythmIndex );
			A.Element( _T("ReflectiveIndex"),         m_dReflectiveIndex );

			// !!! Z-Indexes

//			A.Element( _T("ZIndexSOS"),               m_dZIndexSOS);
//			A.Element( _T("ZIndexSs"),                m_dZIndexSs);
//			A.Element( _T("ZIndexDe"),                m_dZIndexDe);
//			A.Element( _T("ZIndexSi"),                m_dZIndexSi);
//			A.Element( _T("ZIndexPr"),                m_dZIndexPr);
//			A.Element( _T("ZIndexSu"),                m_dZIndexSu);
//			A.Element( _T("ZIndexFl"),                m_dZIndexFl);
			A.Element( _T("ZIndexGl"),                m_dZIndexGl);
			A.Element( _T("ZIndexSu"),                m_dZIndexSu);
			A.Element( _T("ZIndexAd"),                m_dZIndexAd);
			A.Element( _T("ZIndexMe"),                m_dZIndexMe);
			A.Element( _T("ZIndexAv"),                m_dZIndexAv);
			A.Element( _T("ZIndexUIR"),               m_dZIndexUIR);
			A.Element( _T("ZIndexULM"),               m_dZIndexULM);
			A.Element( _T("ZIndexNe"),                m_dZIndexNe);
			A.Element( _T("ZIndexPr"),                m_dZIndexPr);
//			A.Element( _T("ZIndexIR"),                m_dZIndexIR);
//			A.Element( _T("ZIndexDF"),                m_dZIndexDF);
			A.Element( _T("ZIndexLR"),                m_dZIndexLR);
			A.Element( _T("ZIndexLT"),                m_dZIndexLT);

		}
	};

	class IStatisticsParameters : public SS::Interface::Common::IXMLStorage
	{

	private:

		TTextIndexes                  m_oTextIndexes;
		TAverageLengthTextParameters  m_oAverageLengthTextParameters;
		TPartOfSpeachParameters       m_oPartOfSpeachParameters;
		TQuantitiveTextParameters     m_oQuantitiveTextParameters;

		SSymbolsSequenceInfoList      m_oUnCommonWords;         // Слова выходящие за рамки среднестат. компетенции

		//SSymbolsSequenceInfoList      m_oBigrammsByAlphabet;    // Биграммы по алфавиту
		//SSymbolsSequenceInfoList      m_oTrigrammsByAlphabet;   // Триграммы по алфавиту 
		//SSymbolsSequenceInfoList      m_oBigrammsByQuantity;    // Биграммы по кол-ву
		//SSymbolsSequenceInfoList      m_oTrigrammsByQuantity;   // Триграммы по кол-ву
		//SSymbolsSequenceInfoList      m_oLettersAlphabet;       // Буквы по алфавиту
		//SSymbolsSequenceInfoList      m_oLettersOrdered;        // Буквы по встречаемости

		SSymbolsSequenceInfoList      m_oBigramms;              // Биграммы по алфавиту
		SSymbolsSequenceInfoList      m_oTrigramms;             // Триграммы по алфавиту 
		SSymbolsSequenceInfoList      m_oLetters;               // Буквы по алфавиту

		SSymbolsSequenceInfoList      m_oSymbolList;            // nSymbol - Число символов ( ! " , . - )
		SPOSInfoList                  m_oPOSInfoList;           // Информация о частях речи
		SOrderInfoList                m_oOrderInfoList;         // Информация о порядке слов в тексте и их частях речи

	public:

		TTextIndexes                 * GetTextIndexes()                   { return &m_oTextIndexes; }
		TAverageLengthTextParameters * GetAverageLengthTextParameters()   { return &m_oAverageLengthTextParameters; }
		TPartOfSpeachParameters      * GetPartOfSpeachParameters()        { return &m_oPartOfSpeachParameters; }
		TQuantitiveTextParameters    * GetQuantitiveTextParameters()      { return &m_oQuantitiveTextParameters; }

		SSymbolsSequenceInfoList     * GetUnCommonWords()                 { return &m_oUnCommonWords; }

		//SSymbolsSequenceInfoList     * GetBigrammsByAlphabet()            { return &m_oBigrammsByAlphabet; }
		//SSymbolsSequenceInfoList     * GetTrigrammsByAlphabet()           { return &m_oTrigrammsByAlphabet; }
		//SSymbolsSequenceInfoList     * GetBigrammsByQuantity()            { return &m_oBigrammsByQuantity; }
		//SSymbolsSequenceInfoList     * GetTrigrammsByQuantity()           { return &m_oTrigrammsByQuantity; }
		//SSymbolsSequenceInfoList     * GetLettersAlphabet()               { return &m_oLettersAlphabet; }
		//SSymbolsSequenceInfoList     * GetLettersOrdered()                { return &m_oLettersOrdered; }

		SSymbolsSequenceInfoList     * GetBigramms()                      { return &m_oBigramms; }
		SSymbolsSequenceInfoList     * GetTrigramms()                     { return &m_oTrigramms; }
		SSymbolsSequenceInfoList     * GetLetters()                       { return &m_oLetters; }

		SSymbolsSequenceInfoList     * GetSymbolList()                    { return &m_oSymbolList; }
		SPOSInfoList                 * GetPOSInfoList()                   { return &m_oPOSInfoList; }

		// NEW 20_04_06 
		SOrderInfoList               * GetOrderInfoList()                 { return &m_oOrderInfoList; }

		//IXMLStorage
		void GetXML(std::wstring * pwXML) { GetSerializeXMLString(*this, _T("StatisticsParameters"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("StatisticsParameters"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A )    {
			A.Group( _T("TextIndexes"),                 m_oTextIndexes );
			A.Group( _T("AverageLengthTextParameters"), m_oAverageLengthTextParameters );
			A.Group( _T("PartOfSpeachParameters"),      m_oPartOfSpeachParameters );
			A.Group( _T("QuantitiveTextParameters"),    m_oQuantitiveTextParameters );

			A.Group( _T("UnCommonWords"),               m_oUnCommonWords );

			//A.Group( _T("BigrammsByAlphabet"),          m_oBigrammsByAlphabet ); 
			//A.Group( _T("TrigrammsByAlphabet"),         m_oTrigrammsByAlphabet );
			//A.Group( _T("BigrammsByQuantity"),          m_oBigrammsByQuantity ); 
			//A.Group( _T("TrigrammsByQuantity"),         m_oTrigrammsByQuantity );
			//A.Group( _T("LettersAlphabet"),             m_oLettersAlphabet );
			//A.Group( _T("LettersOrdered"),              m_oLettersOrdered );

			A.Group( _T("Bigramms"),                    m_oBigramms ); 
			A.Group( _T("Trigramms"),                   m_oTrigramms );
			A.Group( _T("Letters"),                     m_oLetters );

			A.Group( _T("SymbolList"),                  m_oSymbolList );
			A.Group( _T("POSInfoList"),                 m_oPOSInfoList );
			A.Group( _T("OredrInfoList"),               m_oOrderInfoList );
	  }
	};

// ------------------------------------------------------------------

	/// Информация о частоте встречаемости слов
	struct TWordFrequencyInfo   
	{
		/// Слово
		std::wstring    sWord;

		/// Код части речи. См. файл <IStatisticsCollector.h>
		UINT            iPOS;

		/// Колличества
		TResultType_GP  oValues;

		void GetXML(std::wstring * pwXML) {       GetSerializeXMLString(*this, _T("WordFrequencyInfo"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("WordFrequencyInfo"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.Element( _T("Word"),   sWord );
			A.Element( _T("POS"),    iPOS );
			A.Group(   _T("Values"), oValues );
		}
	};

	/// Список с информацией о частоте встречаемости слов
	struct SWordFrequencyInfoList   
	{
		std::list< TWordFrequencyInfo >           m_WordFrequencyInfoList;		
		std::list< TWordFrequencyInfo >::iterator m_ListItr;

		void GetXML(std::wstring * pwXML) {       GetSerializeXMLString(*this, _T("WordFrequencyInfoList"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("WordFrequencyInfoList"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.STLContainerGroup( _T("WordFrequencyInfoList"), _T("Item"), m_WordFrequencyInfoList );
		}
	};

	/// Класс с информацией о частоте встречаемости слов
	class IWordFrequencyParameters : public SS::Interface::Common::IXMLStorage
	{

	private:

		SWordFrequencyInfoList m_oWordFrequencyInfoList;  // Список с информацией о частоте встречаемости слов
		UINT                   m_iGlobalNumberOfWords;    // Общее число слов

	public:

		SWordFrequencyInfoList * GetWordFrequencyInfoList(VOID)       { return &m_oWordFrequencyInfoList; }
		UINT                     GetGlobalNumberOfWords(VOID)   { return(m_iGlobalNumberOfWords); }
		VOID                     SetGlobalNumberOfWords(UINT iNum)   { m_iGlobalNumberOfWords = iNum; }


		//IXMLStorage
		void GetXML(std::wstring * pwXML) { GetSerializeXMLString(      *this, _T("WordFrequencyParameters"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("WordFrequencyParameters"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A )    {
			A.Group(   _T("WordFrequencyInfoList"),                 m_oWordFrequencyInfoList );
			A.Element( _T("GlobalNumberOfWords"),                   m_iGlobalNumberOfWords );
	  }
	};


// ------------------------------------------------------------------

	/// Информация о семантике слова
	struct TSemanticsInfo   
	{
		/// Слово
		std::wstring    sWord;

		/// Семантический класс
		UINT32          iSemClass;

		/// Колличества
		TResultType_GP  oValues;

		void GetXML(std::wstring * pwXML) {       GetSerializeXMLString(*this, _T("SemanticsInfo"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("SemanticsInfo"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.Element( _T("Word"),     sWord );
			A.Element( _T("SemClass"), iSemClass );
			A.Group(   _T("Values"),   oValues );
		}
	};

	/// Список с информацией о частоте встречаемости слов
	struct SSemanticsInfoList   
	{
		std::list< TSemanticsInfo >           m_SemanticsInfoList;		
		std::list< TSemanticsInfo >::iterator m_ListItr;

		void GetXML(std::wstring * pwXML) {       GetSerializeXMLString(*this, _T("SemanticsInfoList"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("SemanticsInfoList"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A ) 
		{
			A.STLContainerGroup( _T("SemanticsInfoList"), _T("Item"), m_SemanticsInfoList );
		}
	};

	/// Класс с информацией о частоте встречаемости слов
	class ISemanticsParameters : public SS::Interface::Common::IXMLStorage
	{

	private:

		SSemanticsInfoList m_oSemanticsInfoList;  // Список с информацией о частоте встречаемости слов
		UINT               m_iGlobalNumberOfWords;    // Общее число слов

	public:

		SSemanticsInfoList * GetSemanticsStatisticsInfoList(VOID)       { return &m_oSemanticsInfoList; }
		UINT                 GetGlobalNumberOfWords(VOID)   { return(m_iGlobalNumberOfWords); }
		VOID                 SetGlobalNumberOfWords(UINT iNum)   { m_iGlobalNumberOfWords = iNum; }


		//IXMLStorage
		void GetXML(std::wstring * pwXML) { GetSerializeXMLString(      *this, _T("SemanticsParameters"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("SemanticsParameters"), *pwXML); }

		template< class Archive >
		void Serialize( Archive &A )    {
			A.Group(   _T("SemanticsInfoList"),   m_oSemanticsInfoList );
			A.Element( _T("GlobalNumberOfWords"), m_iGlobalNumberOfWords );
	  }
	};


	// -------------------------------------------------------


	/// 
	struct SMarkerInfo
	{
		std::wstring    sMarkerName;
		UINT            iMarkerCnt;
		DOUBLE          iMarkerFreq;

		void GetXML(std::wstring * pwXML) {       GetSerializeXMLString( *this, _T("MarkerInfo"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString( *this, _T("MarkerInfo"), *pwXML); }

		template< class Archive > void Serialize( Archive &A ) {
			A.Element( _T("MarkerName"), sMarkerName );
			A.Element( _T("MarkerCnt"),  iMarkerCnt );
			A.Element( _T("MarkerFreq"), iMarkerFreq );
		}
	};

	/// Поддерево сем. класса с маркерами
	struct TSemClassSubTree   
	{

		std::wstring                              m_ClassName;        // 
		UINT                                      m_Index;            // IDSource класса ( полный - с номером словаря)
		std::wstring                              m_ClassShortName;   //
		UINT                                      m_MarkersInBaseNum; //
		UINT                                      m_MarkersNum;       //
		UINT                                      m_MarkersNumGlobal;
		UINT                                      m_MarkersRecurNum;
		UINT                                      m_MarkersRecurNumGlobal;
		DOUBLE                                    m_MarkersFreqGlobal; 
		std::list  <SMarkerInfo>                  m_MarkersList;
//		std::list  <SMarkerInfo>::iterator        m_MarkersListItr;
		UINT                                      m_SubClassesNum;
		std::list< TSemClassSubTree >             m_SubClassesTree;		
//		std::list< TSemClassSubTree >::iterator   m_SubClassesTreeItr;

		void GetXML(std::wstring * pwXML)       { GetSerializeXMLString(*this, _T("SemClassSubTree"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString(*this, _T("SemClassSubTree"), *pwXML); }

		template< class Archive > void Serialize( Archive &A )   {
			A.Element(           _T("ClassName"),                  m_ClassName );             // Имя класса
			A.Element(           _T("Index"),                      m_Index );                 // IDSource класса ( полный - с номером словаря)
			A.Element(           _T("ClassShortName"),             m_ClassShortName );        // Короткок имя класса
			A.Element(           _T("SubClassesNum"),              m_SubClassesNum );         // Число подклассов 
			A.STLContainerGroup( _T("SubClassesTree"), _T("Item"), m_SubClassesTree );        // STLContainerGroup
			A.Element(           _T("MarkersInBaseNum"),           m_MarkersInBaseNum );      // Общее число маркеров данного класса в базе
			A.Element(           _T("MarkersNum"),                 m_MarkersNum );            // Число уникальных маркеров данного класса в тексте ()
			A.Element(           _T("MarkersNumGlobal"),           m_MarkersNumGlobal );      // Число всех       маркеров данного класса в тексте
//			A.Element(           _T("MarkersRecurNum"),            m_MarkersRecurNum );       // Число уникальных маркеров данного класса с подклассами в тексте
//			A.Element(           _T("MarkersRecurNumGlobal"),      m_MarkersRecurNumGlobal ); // Число всех       маркеров данного класса с подклассами в тексте
			A.Element(           _T("MarkersFreqGlobal"),          m_MarkersFreqGlobal );     // Отнощение числа всех маркеров данного класса в тексте к длине текста (как <MarkerFreq> в <struct SMarkerInfo>, только для всех встретившихся) 
			A.STLContainerGroup( _T("MarkersList"),    _T("Item"), m_MarkersList );           // STLContainerGroup
		}
	};

	/// Корневой класс дерева сем. класа с маркерами
	class ISemClassSubTreeWithMarkers : public SS::Interface::Common::IXMLStorage
	{

	private:

		TSemClassSubTree m_RootSemClass;

	public:

		TSemClassSubTree * GetSemClassSubTreeWithMarkers(VOID)       { return &m_RootSemClass; }

		//IXMLStorage
		void GetXML(std::wstring * pwXML)       { GetSerializeXMLString( *this, _T("SemClassSubTreeWithMarkers"), *pwXML); }
		void SetXML(const std::wstring * pwXML) { SetSerializeXMLString( *this, _T("SemClassSubTreeWithMarkers"), *pwXML); }

		template< class Archive > void Serialize( Archive &A )   {
			A.Group( _T("RootSemClass"), m_RootSemClass );
	  }
	};

}
}
}