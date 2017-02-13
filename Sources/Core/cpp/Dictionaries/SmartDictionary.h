#pragma once

namespace SS{ 
namespace Dictionary{

/** Работает со словарями имён собственных, имён нарицательных, новых слов и окнончаний.
	 Предоставляет методы по поиску различной информации о слове.
    Более подробно смю описание методов.

    Работает с разделяемыми между несколькими потоками объектами. О блокировке см. ниже
*/

//struct SSDStatistic
//{
//   static unsigned TotalFillWordInfo;
//   static unsigned SuccessfulFillWordInfo;
//   static unsigned TotalFillHypothesisInfo;
//   static unsigned SuccessfulFillHypothesisInfo;
//};

	class CSmartDictionary: 
		//public NonCopyable,
		//На текущий момент реализуется внешняя блокировка всех объектов класса во всех функциях при помощи умного указателя LockedPtr
		//В дальнейшем необходимо реализовавать отдельно блокировку на чтение и на запись только разделяемых с другими объектами классов,
		//т.к. подход реализованный на текущий момент заставляет ждать остальные потоки, пока один работает со словарём.
		public NoLockable<CSmartDictionary, SCriticalSectionImplStategy, CURRENT_THREAD_DEBUG_INFO>
	{
	public:
		typedef std::vector<SPartOfWord> TPOFVector;
		typedef std::list<SMorphoType> TMTList;
		typedef std::list<TWordInfo> TWIList;
		typedef std::basic_string<TCHAR> TString;
		typedef std::vector<SHypothesisInfo> THIVector;

		typedef std::vector<int> TWordBounds;

		// Информация о гипотезах, т.е. опредиляемых новых словах по данным которые уже есть в словарях
		struct SHypothesisInfo 
		{
			//Количество совпавших символов с конца
			int  m_ConcordanceChars;  

			struct WordInfo
			{
				SMorphoType m_MorphoType;           //Морфологическая информация
				TDictionaryIndex m_DictIndex;       //Индекс в словаре
				int m_EndingLen;                    //Длина окончания в слове, сколько символов с конца занимает окончание 
			};

			typedef std::list<WordInfo> TFormsInfoList;

			//Список всех найденных слов у которых количество совпавших с конца смволов =  m_ConcordanceChars
			TFormsInfoList m_FormsInfoList; 

		public:

			bool Empty() const { return m_FormsInfoList.empty(); }
			SHypothesisInfo(): m_ConcordanceChars(0) {}
			void Clear() { m_FormsInfoList.clear(); m_ConcordanceChars = 0; }

			//Отсортировать по IDType и оставить только уникальные IDType:m_EndingLen
			void UniqueIDTypeEndingLen();

			//Отсортировать по IDType и по IDForm
			void SortIDTypeIDForm();

			//Отсортировать по IDType и по IDForm и оставить только уникальные IDType:IDForm
			void UniqueIDTypeIDForm();

		};

		//Проверка, подходит ли найденая гипотеза или нет
		struct IDeterminationWordCheck
		{
			virtual ~IDeterminationWordCheck() {}

			//Каждая гипотеза перед тем как попасть в список для дальнейшей обработки проверяется этой функцией, и только если эта функция возвратит  
			//true попадает в список гипотез по слову
			//wcWord - Слово для которого производится поиск гипотез, в оригинальном регистре
			//wcWordLower - Слово для которого производится поиск гипотез, в пониженном регистре
			//DictionaryIndex, MorphoInfo, SpecialMorpho - Характеристики найденной гипотезы
			virtual bool IsDeterminationWord( const wchar_t* wcWord, const wchar_t* wcWordLower, bool IsFirstLetterUpperCase, TDictNumber DictionaryIndex, TMorphoInfo MorphoInfo, TSpecialMorpho SpecialMorpho, IAMConverterMorpho *pAMConverter ) const = 0;
		};

	public:
		typedef CMorphoTypesSearchCreateManager::TPointer TMorphoTypesSearchPtr;
		typedef CPartOfWordsSearchCreateManager::TPointer TPartOfWordsSearchPtr;
		typedef CSourceToTypeCreateManager::TPointer TSourceToTypePtr;

		typedef CMorphoTypesSearchCreateManager::TConstPointer TMorphoTypesSearchConstPtr;
		typedef CPartOfWordsSearchCreateManager::TConstPointer TPartOfWordsSearchConstPtr;
		typedef CSourceToTypeCreateManager::TConstPointer TSourceToTypeConstPtr;

	protected:
		mutable TMorphoTypesSearchPtr m_pMorphoTypesSearch;   //Поиск различной информации
		TPartOfWordsSearchPtr m_pNominalWS;						   //Имена нарицательные
		TPartOfWordsSearchPtr m_pProperWS;						   //Имена собственные 
		TPartOfWordsSearchPtr m_pEndingWS;						   //Окончания
		TSourceToTypePtr m_pNominalSourceToMorpho;			   //Нарицательные -> MorphoType
		TSourceToTypePtr m_pProperSourceToMorpho;			      //Собственные -> MorphoType
		Detail::CNewWordsCache m_NewWords;

		//Используются как буферы при поиске
		mutable TPOFVector m_EndingVecBuf;
		mutable TPOFVector m_SourceVecBuf; 
		mutable THIVector m_HIVectorBuf;

		//Индексы словарей
		TDictNumber m_NominalDictionaryIndex;
		TDictNumber m_ProperDictionaryIndex;
		TDictNumber m_NewWordsDictionaryIndex;

		IAMConverterMorpho *m_pAMConverter;

		int m_MinRecognizableWordLen;

	private:
		CTablePtr m_pNominalDT;
		CTablePtr m_pProperDT;
		CTablePtr m_pEndingDT;

	private:
		static const wchar_t * const szNominalTable;
		static const wchar_t * const szProperTable;
		static const wchar_t * const szEndingTable;
		static const wchar_t * const szNominalSourceToMorphoTable;
		static const wchar_t * const szProperSourceToMorphoTable;
		static const wchar_t * const szMorphoTypesTable;

	//Получение уникальных ID для новых слов (циклически при достижении максимально допустимого значение - сброс в 0)
	private:
		volatile long m_NextSourceID;

		int GetNextSourceID()
		{
			return _InterlockedIncrement(&m_NextSourceID);
		}

	private:

		/// Заполняет все поля SWordInfo
		static void FillSWordInfo( 
			const SMorphoType &MorphoType, 
			const std::wstring &wcWord,
			unsigned int IDSource,
			TDictNumber DictNumber,
			SWordInfo &WordInfo,
			bool FirstLetterUpperCase,
			bool isNewWord = false
			) 
		{
			WordInfo.m_wWord = wcWord;
			WordInfo.m_SpecialMorpho = MorphoType.m_SpecialMorpho;
			WordInfo.m_WordIndex.m_DictIndex.SetId(IDSource) ; 
			WordInfo.m_WordIndex.m_DictIndex.SetDictionaryNumber(DictNumber);
			WordInfo.m_WordIndex.m_DictIndex.SetFirstLetterUpperCase(FirstLetterUpperCase); 
			WordInfo.m_WordIndex.m_IDForm = MorphoType.m_IDForm; 
			WordInfo.m_MorphoTypeInfo = MorphoType.m_MorphoTypeInfo;
			WordInfo.m_NewWord = isNewWord;
		}

		/// По найденному IDEnding находит в словаре pSourseWS все возможные IDSource, для каждого IDSource находит IDType в pSourceToType и 
		/// проверяет есть ли комбинация IDType - IDEnding в таблице MorphoTypes если есть то возвращает TRUE
		///
		/// IDEnding - Индекс окончания
		/// pSourseWS - Словарь основ (по идее указатель константный, но дизайн CPartOfWordsSearch этого не позволяет)
		/// pSourceToType - Соотвествие Основы Типу морфообразвания
		bool IsFindWord(
			const wchar_t* wcWord,
			const SPartOfWord  &IDEnding,
			TPartOfWordsSearchPtr pSourseWS, 
			TSourceToTypeConstPtr pSourceToType
			) const;

		/// Делает тоже что и IsFindWords, но вместо того чтобы останавливаться на первом совпадении складывает  
		/// найденную информацию в TWIList. Это список списков, где внутренний список после вызова функции всегда размером 1, а внешний
		/// содержит все возможные комбинации  морфологической информации о слове.
		///
		/// wcWord - Слово приведённое к нижнему регистру
		/// wcWordOriginalCase - Слово в оригинальном регистре
		/// DictionaryIndex - Индекс словаря для занесения в SWordInfo
		/// WIList - список списков в котором и будут находится нужные элементы
		/// WordInfo - список в котором и будут находится нужные элементы
		void FillWordInfo(
			const wchar_t* wcWord,
			const wchar_t* wcWordOriginalCase,
			bool IsFirstLetterUpperCase,
			const SPartOfWord  &IDEnding,
			TPartOfWordsSearchPtr pSourseWS, 
			TSourceToTypeConstPtr pSourceToType,
			TDictNumber DictionaryIndex,
			TWIList &WIList
			) const;

		void FillWordInfo(
			const wchar_t* wcWord,
			const wchar_t* wcWordOriginalCase,
			int SearchPos,
			bool IsFirstLetterUpperCase,
			const SPartOfWord  &IDEnding,
			TPartOfWordsSearchPtr pSourseWS, 
			TSourceToTypeConstPtr pSourceToType,
			TDictNumber DictionaryIndex,
			TWordInfo &WordInfo
			) const;


		/// По найденному IDEnding находит в словаре новых слов основу и заполняет данные по ней
		/// wcWord - Слово приведённое к нижнему регистру
		/// wcWordOriginalCase - Слово в оригинальном регистре
		/// WIList - список списков в котором и будут находится нужные элементы
		void FillWordInfoFromNewWords(
			const wchar_t* wcWord,
			const wchar_t* wcWordOriginalCase,
			/*in*/bool IsFirstLetterUpperCase,
			const SPartOfWord  &IDEnding,
			TWIList &WIList
			) const;


		//Получение морфоинформации о слове основываясь
		bool CSmartDictionary::GetWordInfoOnlyNominal(
			const wchar_t* wcWord, 
			const wchar_t* wcWordOriginalCase,
			int SearchPos,
			bool IsFirstLetterUpperCase,
			TWordInfo &WordInfo
			) const;

		/// Разделение слова на подслова
		/// Для текущего окончания IDEnding, находит все возможные основы и если 
		/// остались ещё символы в wcWordOriginalCase, то находит все возмжные окончания для 
		/// оставшейся части и для каждого из них вызывает FindSubWordsBounds
		/// WordBounds - Список индексов границ слова (индекс первого символа в подслове, исключая индекс 0)
		bool FindSubWordsBounds( 		
			const wchar_t* wcWord, 
			const wchar_t* wcWordOriginalCase,
			const SPartOfWord  &IDEnding,
			TPartOfWordsSearchPtr pSourseWS, 
			TSourceToTypeConstPtr pSourceToType,
			TDictNumber DictionaryIndex,
			TWordBounds &WordBounds
			);


	protected:
		//Кореектирует IDEnding для передачи в поиск основ
		//LastCharIndex - Длина искомого слова - 1 
		static void CorrectIDEnding( SPartOfWord &IDEnding, int LastCharIndex )
		{
			//Нам необходимо скорректировать позицию в слове если нам вернулось пустое окончание
			//-1 Признак того что функция CPartOfWordsSearch::GetByWord нашла пустое окончание
			if( IDEnding.PositionInWord == -1 )
				IDEnding.PositionInWord = LastCharIndex;
			else
				--IDEnding.PositionInWord;	
		}

		//Проверяет что данные были загружены в память (т.е. вызван метод Load или ReLoad)
		bool CheckLoad() const
		{    
			if( m_pMorphoTypesSearch->Empty() ) return false;
			if( m_pNominalWS->Empty() ) return false;
			if( m_pProperWS->Empty() ) return false;
			if( m_pEndingWS->Empty() ) return false;
			if( m_pNominalSourceToMorpho->Empty() ) return false;
			if( m_pProperSourceToMorpho->Empty() ) return false;

			return true;

			//return  !m_pMorphoTypesSearch->Empty() && !m_pNominalWS->Empty() && !m_pProperWS->Empty() &&
			//        !m_pEndingWS->Empty() && !m_pNominalSourceToMorpho->Empty() && !m_pProperSourceToMorpho->Empty();
		}

		int GetMinRecognizableWordLen() const { return m_MinRecognizableWordLen; } 

	public:
		CSmartDictionary(): m_pAMConverter(0),
					        m_NominalDictionaryIndex(DATA_TYPE::NAMES::ednUnknown),
					        m_ProperDictionaryIndex(DATA_TYPE::NAMES::ednUnknown),
					        m_NewWordsDictionaryIndex(DATA_TYPE::NAMES::ednUnknown),
					        m_NextSourceID(0),
							m_MinRecognizableWordLen(0)
		{
		}

		virtual ~CSmartDictionary() {}

		//Инициализиреут пути и настраивает объекты, затем загружает данные вызовом. После вызова можно начинать искать
		/// pDataBase     - Используемая БД
		/// pAMConverter  - Конвертер  
		/// DBPath -        Путь к каталогу БД с языком который необходимо 
		///                 использовать в словаре
		/// ProperDictionaryIndex, NominalDictionaryIndex, NewWordsDictionaryIndex  - Индексы словарей для представления в TDictionaryIndex
		/// GetMinRecognizableWordLen - Минимальная длина распознавамого слова
		virtual void Init(
			DBMS::IDataBase *pDataBase,
			IAMConverterMorpho *pAMConverter, const TCHAR *DBPath, 
			TDictNumber NominalDictionaryIndex,
			TDictNumber ProperDictionaryIndex, 
			TDictNumber NewWordsDictionaryIndex,
			int MinRecognizableWordLen
			) = 0;

		/// проверка наличия слова в словаре
		/**
		Алгоритм:
		Проверяет есть точная комбинация [основа (собственные или нарицательные) + окончание] в словаре.
		Словарь новых слов при этом не используется
		*/
		bool IsExistWord(const wchar_t* wcWord);

		/// поиск полной морфологической информации по слову 
		/**
		Алгоритм:
		Поиск слов во словарях которые точно совпадают с wcWord и запись информации по них в
		p_l_WordInfo

		UseMainDictionary - Использовать для поиска основной словарь (нарицательные и собственные)
		UseNewWordsDictionary - Использовать для поиска словарь новых слов
		*/
		bool GetWordInfo/*ByWord*/(
			const wchar_t* wcWord, 
			const wchar_t* wcWordOriginalCase,
			/*in*/bool IsFirstLetterUpperCase,
			std::list<TWordInfo>* p_l_WordInfo,
			bool UseMainDictionary = true, bool UseNewWordsDictionary = true
			);

		bool GetWordInfoFromHypothesis(
			const wchar_t* wcWord, 
			const wchar_t* wcWordLower,
			bool IsFirstLetterUpperCase,
			SHypothesisInfo& HypothesisInfo,
			TWIList* p_l_WordInfo
			);

		/// Определение морфологической информации о слове на основне максимально похожего слова(слов).
		/// Возвр: Удалась ли попытка автоматического определения слова 
		/// wcWord - Опредиляемое слово
		/// wcWordLower - Опредиляемое слово приведённое к нижнему регистру
		/// HypothesisInfo - Информация по гипотезам
		/// DWC - Проверка гипотезы
		virtual bool DeterminationWordInfo(
			const wchar_t* wcWord, 
			const wchar_t* wcWordLower, 
			bool IsFirstLetterUpperCase,
			SHypothesisInfo &HypothesisInfo,
			const IDeterminationWordCheck &DWC
			) = 0;

		/// поиск полной морфологической информации по индексу слова
		bool GetWordInfo/*ByWordIndex*/(
			const SWordIndex* pWordIndex, 
			SWordInfo* pWordInfo
			);

		/// поиск слова по его индексу 
		/// wcWord - массив с количеством элементов MAX_CHARS_IN_WORD
		/**
		Алгоритм:
		Поиск слов во всех словарях (включая новых слов).
		*/
		bool GetWord(
			const SWordIndex* pWordIndex,
			wchar_t* wcWord
			);

		/// поиск основы слова слова (части слова без оканчания) по его индексу 
		/// wcWord - массив с количеством элементов MAX_CHARS_IN_WORD
		/**
		Алгоритм:
		Поиск основ во всех словарях (включая новых слов).
		*/
		bool GetWordSource(
			const TDictionaryIndex &DictIndex,
			wchar_t* wcWordSource
			);

		/// поиск словоформ слова по индексу основы
		/**
		Алгоритм:
		Поиск слов во всех словарях (включая новых слов).
		*/

		bool GetWordFormsByIDSource(
			const TDictionaryIndex* pDictionaryIndex,
			/*out*/std::list<TWordInfo>* p_l_WordInfo
			);

		/// Пустышка
		bool GetMorphoInfoByIDType(
			/*in*/SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber eDictionariesNumber,
			/*in*/SS::Dictionary::Types::TIDType oIDType, 
			/*in*/SS::Dictionary::Types::TIDForm oIDForm, 
			/*out*/SS::Dictionary::Types::TMorphoInfo* pMorphoInfo, 
			/*out*/SS::Dictionary::Types::TSpecialMorpho* pSpecialMorpho) {}


		///Добавляет слово в словарь новых слов 
		///Необходимо проверять что этого слова ещё нет. Т.к. иначе в словаре будет копия
		/// Source - Добавляемое слово (основа + окончание)
		/// SourceLower - Source приведённый в нижнему регистру
		/// EndingLen - Длина окончания слова
		/// DI  -  индекс только что добавленного слова.
		//void AddNewWord( const TString &Source, const TIDType IDType, int EndingLen, TDictionaryIndex &DI )
		//{
		//   TString SourceLower; ToLower( Source, SourceLower );

		//   AddNewWord( Source, SourceLower, IDType, EndingLen, DI );
		//}

		void AddNewWord( const TString &Source, const TString &SourceLower, const TIDType IDType, int EndingLen, TDictionaryIndex &DI );

		//Просто возвращает Меренапрявляет вызовы к CMorphoTypesSearch
		BOOL GetMorphoInfoByIDType(
			IN  SS::Dictionary::Types::TIDType oIDType, 
			IN  SS::Dictionary::Types::TIDForm oIDForm, 
			OUT SS::Dictionary::Types::TMorphoInfo * pMorphoInfo, 
			OUT SS::Dictionary::Types::TSpecialMorpho * pSpecialMorpho
			)
		{
			APL_ASSERT( CheckLoad() );
			return m_pMorphoTypesSearch->GetMorphoInfoByIDType( oIDType, oIDForm, pMorphoInfo, pSpecialMorpho );
		}

		///////////////////////////////////////////////////////////////////////////////
		//ILoad

		/// Загружает данные в память. Гарантированно перезагружает словари.
		bool ReLoad();

		/// Загружает данные в память если ещё не загружены
		bool Load();

		/// Очищает данные
		bool Clear();

		//Пересоздаёт деревья
		bool ToBinary();

		//Установить таблицу в которую будут записываться и считываться все слова.
		//Если pDataTable == 0, то поиск и добавление производятся только в памяти
		void SetNewWordsTable( DBMS::IDataTable *pDataTable ){ m_NewWords.SetTable( pDataTable, m_NewWordsDictionaryIndex ); }

		//Очистить кэш новых слов (если не прикреплена таблица к кэшу то данные по новым словам уничтожаются. 
		//Ессли прикреплена, то удаляются только данные в памяти)
		void ClearNewWordsCache(){ m_NewWords.Clear(); }

		///////////////////////////////////////////////////////////////////////////////
		// Определение морфологической информации составного слова состоящего из более 
		// мелких слов присутствующих в словаре. Между частями слова не может быть ни 
		// одного разделителя.
		///////////////////////////////////////////////////////////////////////////////
		bool GetCompoundWordInfo( 		
			const wchar_t* wcWord, 
			const wchar_t* wcWordOriginalCase,
			bool IsFirstLetterUpperCase,
			TWIList &WIList
			);


		//Функции используются только для отладки!!!
		TPartOfWordsSearchPtr GetNominalWS() { return  m_pNominalWS; }						   
		TPartOfWordsSearchPtr GetProperWS() { return  m_pProperWS;	}					    
		TPartOfWordsSearchPtr GetEndingWS() { return  m_pEndingWS; }						   
		TSourceToTypeConstPtr GetNominalSourceToMorpho() { return  m_pNominalSourceToMorpho; }			
		TSourceToTypeConstPtr GetProperSourceToMorpho() { return  m_pProperSourceToMorpho; }			
		Detail::CNewWordsCache *GetNewWords() { return &m_NewWords; }
		IAMConverterMorpho *GetAMConverter() const { APL_ASSERT_PTR(m_pAMConverter); return m_pAMConverter; }
	};


///////////////////////////////////////////////////////////////////////////////
// Опредиление гипотез на основе проверки всех слов словаря и поиска наибольшего
// совпадения
///////////////////////////////////////////////////////////////////////////////
class CStandartSmartDictionary: public CSmartDictionary
{
private:
   /// Поиск гиптез. 
   /// Находим все слова с длинной совпадения не меньше HypothesisInfo.m_ConcordanceChars и удовлетворяющих условиям
   /// опсаннным в DeterminationWordInfo (кроме 1). Если находится слово с количеством совпавших символов больше чем 
   /// HypothesisInfo.m_ConcordanceChars, то все прошлые гипотезы удаляются. Т.е. выбирается набор из наиболее 
   /// подходящих гипотез.
   void FillHypothesisInfo(
      const wchar_t* wcWord,
      const wchar_t* wcWordLower, //Слово в нижнем регистре
      bool IsFirstLetterUpperCase,
      int WordLen,  //Длина слова
      const SPartOfWord  &IDEnding,
      TPartOfWordsSearchPtr pSourseWS, 
      TSourceToTypeConstPtr pSourceToType,
      TDictNumber DictionaryIndex,
      SHypothesisInfo &HypothesisInfo,
      const IDeterminationWordCheck &DWC
   ) const;

public:
   //Инициализиреут пути и настраивает объекты, затем загружает данные вызовом. После вызова можно начинать искать
   /// pDataBase     - Используемая БД
   /// pAMConverter  - Конвертер  
	/// DBPath -        Путь к каталогу БД с языком который необходимо 
	///                 использовать в словаре
	///	ProperDictionaryIndex, NominalDictionaryIndex, NewWordsDictionaryIndex  - Индексы словарей для представления в TDictionaryIndex
	void Init(
      DBMS::IDataBase *pDataBase,
      IAMConverterMorpho *pAMConverter, const TCHAR *DBPath, 
		TDictNumber NominalDictionaryIndex,
		TDictNumber ProperDictionaryIndex, 
      TDictNumber NewWordsDictionaryIndex,
      int MinRecognizableWordLen
	)
   {  
      CSmartDictionary::Init( pDataBase, pAMConverter, DBPath, NominalDictionaryIndex, ProperDictionaryIndex, NewWordsDictionaryIndex, MinRecognizableWordLen ); 
   }

   /**
      Алгоритм:
         Функция не проверяет есть ли слово в словарях, а сразу пытается опредилить           
         наиболее похожие на него с конца слова (гипотезы) по следующим правилам (Кроме ограничений IDeterminationWordCheck):
            1. Слово длиной меньше 4-х букв не опредиляется автоматически
            2. Мы не учитываем гипотезы состоящие только из окончания, т.е. для того чтобы словарное слово являлось
               гипотезой необходимо чтобы совпадало не только окончание(можно пустое), но и хотябы один символ основы
   */
   bool DeterminationWordInfo(
      const wchar_t* wcWord, 
      const wchar_t* wcWordLower, 
      bool IsFirstLetterUpperCase,
      SHypothesisInfo &HypothesisInfo,
      const IDeterminationWordCheck &DWC
   );
};

///////////////////////////////////////////////////////////////////////////////
// Определение гипотез на основе заранее, построенного дерева вывода
// см. MinimalHypothesisTree.h
///////////////////////////////////////////////////////////////////////////////
class CMinimalHypothesisTreeSmartDictionary: public CSmartDictionary
{
   TPartOfWordsSearchPtr m_pNewWordsDeductionWS;						   //Минимальное дерево поиска
   CTablePtr m_pNewWordsDeductionDT;                                 

private:
   static const wchar_t * const szNewWordsDeductionTable;

public:
   //Инициализиреут пути и настраивает объекты, затем загружает данные вызовом. После вызова можно начинать искать
   /// pDataBase     - Используемая БД
   /// pAMConverter  - Конвертер  
   /// DBPath -        Путь к каталогу БД с языком который необходимо 
   ///                 использовать в словаре
   ///	ProperDictionaryIndex, NominalDictionaryIndex, NewWordsDictionaryIndex  - Индексы словарей для представления в TDictionaryIndex
   void Init(
      DBMS::IDataBase *pDataBase,
      IAMConverterMorpho *pAMConverter, const TCHAR *DBPath, 
      TDictNumber NominalDictionaryIndex,
      TDictNumber ProperDictionaryIndex, 
      TDictNumber NewWordsDictionaryIndex,
      int MinRecognizableWordLen
   );

   /**
      Алгоритм:
         Функция не проверяет есть ли слово в словарях, а сразу пытается опредилить           
         наиболее похожие на него с конца слова (гипотезы) по следующим правилам (Кроме ограничений IDeterminationWordCheck):
            1. Слово длиной меньше 4-х букв не опредиляется автоматически

         Мы находим максимальное совпадение с конца в дереве вывода и тем самым опредиляем IDType, после чего
         в качестве форм, выбираем только те окончания по которым есть наибольшее совпадение с конца (включая пустое 
         окончание, если оно присутствует у IDType)
   */
   bool DeterminationWordInfo(
      const wchar_t* wcWord, 
      const wchar_t* wcWordLower, 
      bool IsFirstLetterUpperCase,
      SHypothesisInfo &HypothesisInfo,
      const IDeterminationWordCheck &DWC
   );

};

///////////////////////////////////////////////////////////////////////////////
struct TStandartDeterminationWordCheck: public CSmartDictionary::IDeterminationWordCheck
{
   //Удовлетворяет ли гипотеза критериям:
   // Если Слово с большой буквы, то слово - существительное
   // Ecли Слово с маленькой буквы, то это не служебная часть речи
   bool IsDeterminationWord( const wchar_t* wcWord, const wchar_t* wcWordLower, bool IsFirstLetterUpperCase, TDictNumber DictionaryIndex, TMorphoInfo MorphoInfo, TSpecialMorpho SpecialMorpho, IAMConverterMorpho *pAMConverter ) const;
};

///////////////////////////////////////////////////////////////////////////////

struct TOnlyNounDeterminationWordCheck: public CSmartDictionary::IDeterminationWordCheck
{
   //Из всех ипотез оставляет только существительные
   bool IsDeterminationWord( const wchar_t* wcWord, const wchar_t* wcWordLower, bool IsFirstLetterUpperCase, TDictNumber DictionaryIndex, TMorphoInfo MorphoInfo, TSpecialMorpho SpecialMorpho, IAMConverterMorpho *pAMConverter ) const;
};

///////////////////////////////////////////////////////////////////////////////
// Адаптирует CSmartDictionary к интерфейсу IBasicMorphologyAnalyser
///////////////////////////////////////////////////////////////////////////////
class CSmartDictionaryAdapter: public Detail::IBasicMorphologyAnalyser
{
public:
   typedef auto_ptr_ex<CSmartDictionary> TSmartDictionaryPtr;
   
   enum TDictionaryTypes
   {
      EStandart,
      EMinimalHypothesisTree,
   };

   static TSmartDictionaryPtr CreateSmartDictionary( TDictionaryTypes DictionaryTypes )
   {
      switch(DictionaryTypes)
      {
      case EStandart:
         return TSmartDictionaryPtr(new CStandartSmartDictionary);
         break;

      case EMinimalHypothesisTree:
         return TSmartDictionaryPtr(new CMinimalHypothesisTreeSmartDictionary);
      };

      APL_ASSERT( false );
      return TSmartDictionaryPtr(0);
   }

private:
   TSmartDictionaryPtr m_SmartDictionary;

public:
   
   //Инициализиреут пути и настраивает объекты, затем загружает данные вызовом. После вызова можно начинать искать
   /// pDataBase     - Используемая БД
   /// pAMConverter  - Конвертер  
   /// DBPath -        Путь к каталогу БД с языком который необходимо 
   ///                 использовать в словаре
   ///	ProperDictionaryIndex, NominalDictionaryIndex, NewWordsDictionaryIndex  - Индексы словарей для представления в TDictionaryIndex
   void Init(
      DBMS::IDataBase *pDataBase,
      IAMConverterMorpho *pAMConverter, const TCHAR *DBPath, 
      TDictNumber NominalDictionaryIndex,
      TDictNumber ProperDictionaryIndex, 
      TDictNumber NewWordsDictionaryIndex,
      int MinRecognizableWordLen,
      TDictionaryTypes DictionaryTypes = EStandart
   )
   { 
      m_SmartDictionary = CreateSmartDictionary( DictionaryTypes );

      APL_ASSERT( m_SmartDictionary != 0 );

      m_SmartDictionary->Init(pDataBase, pAMConverter, DBPath, NominalDictionaryIndex, ProperDictionaryIndex, NewWordsDictionaryIndex, MinRecognizableWordLen );
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   //IMorphologyAnalyser

   /// проверка наличия слова в словаре
   bool IsExistWord(/*in*/const wchar_t* wcWord){ return m_SmartDictionary->IsExistWord( wcWord ); }

   /// поиск полной морфологической информации по слову 
   bool GetWordInfo/*ByWord*/(
      /*in*/const wchar_t* wcWord, 
      /*in*/const wchar_t* wcWordOriginalCase, 
      /*in*/bool IsFirstLetterUpperCase,
      /*out*/std::list<TWordInfo>* p_l_WordInfo
      )
   { return m_SmartDictionary->GetWordInfo( wcWord, wcWordOriginalCase, IsFirstLetterUpperCase, p_l_WordInfo ); }


   /// поиск полной морфологической информации по индексу слова
   bool GetWordInfo/*ByWordIndex*/(
      /*in*/SWordIndex* pWordIndex, 
      /*out*/SWordInfo* pWordInfo
      )
   { return m_SmartDictionary->GetWordInfo( pWordIndex, pWordInfo ); }

   /// поиск слова по его индексу 
   /// wcWord - массив с количеством элементов MAX_CHARS_IN_WORD
   bool GetWord(
      /*in*/SWordIndex* pWordIndex,
      /*out*/wchar_t* wcWord
      )
   { return m_SmartDictionary->GetWord( pWordIndex, wcWord ); }

   /// поиск основы слова слова (части слова без оканчания) по его индексу 
   /// wcWord - массив с количеством элементов MAX_CHARS_IN_WORD
   bool GetWordSource(
      const TDictionaryIndex &DictIndex,
      wchar_t* wcWordSource
   )
   { return m_SmartDictionary->GetWordSource( DictIndex, wcWordSource ); }

   /// поиск словоформ слова по индексу основы
   bool GetWordFormsByIDSource(
      /*in*/TDictionaryIndex* pDictionaryIndex,
      /*out*/std::list<TWordInfo>* p_l_WordInfo
      )
   { return m_SmartDictionary->GetWordFormsByIDSource( pDictionaryIndex, p_l_WordInfo ); }

   //Возвращает внутренний CSmartDictionary
   CSmartDictionary &GetOrigin() { APL_ASSERT( m_SmartDictionary != 0 ); return *m_SmartDictionary; }
   const CSmartDictionary &GetOrigin() const { APL_ASSERT( m_SmartDictionary != 0 ); return *m_SmartDictionary; }
};


} //namespace Dictionary
} //namespace SS
