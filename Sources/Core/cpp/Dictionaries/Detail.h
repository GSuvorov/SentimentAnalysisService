#pragma once

namespace SS{ 
namespace Dictionary{

 /*  namespace Types {
      inline bool operator<( const TDictionaryIndex &DI1, const TDictionaryIndex &DI2 ){ return DI1.GetDictionaryIndex() < DI2.GetDictionaryIndex(); }
   }*/

   //Строковое описние TDictNumber
   const std::string &DictNumberDescription( TDictNumber DictNumber );
   
   //Строковое описние TDictionaryInde, без GetId
   std::string DictionaryIndexDescription( TDictionaryIndex DictIndex );

   //Строковое описние EAnalyseMods
   const std::string &AnalyseModsDescription( EAnalyseMods AnalyseMods );
   
   //Строковое описние ECurrentSystemMode
   const std::string &CurrentSystemModeDescription( ECurrentSystemMode CurrentSystemMode );


namespace Detail{

//Read/Write мьютекс блокирующий разделённые между пользователями объекты
class CSharedObjectMutexBase {};
typedef ClassLevelLockable<CSharedObjectMutexBase, SSWMRImplStategy, CURRENT_THREAD_DEBUG_INFO> TSharedObjectMutex;

typedef std::list<CMorphoFeature> TFeatureList;

//Заполнить pIndex информацией из WIList
void FillUnit( const std::list<TWordInfo> &WIList, IUnit *pUnit, IBlackBoxTextStorage *pBB );

//Заполнить pIndex информацией из WordInfo
void FillUnit( const TWordInfo &WordInfo, IUnit *pUnit, IBlackBoxTextStorage *pBB );

//Создать дочернии элементы pUnit и заполнить их информацией из WIList
void FillSubUnits( const std::list<TWordInfo> &WIList, IUnit *pUnit, IBlackBoxTextStorage *pBB );

//Заполнить pIndex информацией из MorhpoInfo, SpecialMorpho, IDForm 
void FillUnit( TMorphoInfo MorhpoInfo, TSpecialMorpho SpecialMorpho, TIDForm IDForm, const TDictionaryIndex &DI, IUnit *pUnit, IBlackBoxTextStorage *pBB );

//Заполнить pIndex информацией из FeatureList, IDForm 
void FillUnit( const TFeatureList &FeatureList, TIDForm IDForm, const TDictionaryIndex &DI, IUnit *pUnit, IBlackBoxTextStorage *pBB, IAMConverterMorpho *pAM );

//Скоректировать регистр символов, основываясm на информации из TDictionaryIndex
template< class String >
inline void CorrectWordCase( String &Word, const TDictionaryIndex &DI )
{
   if( DI.GetFirstLetterUpperCase() )
      CharUpperBuffW( &Word[0], 1 );
}

///////////////////////////////////////////////////////////////////////////////
// Преобразовать строку в нижний регистр, подготовить её для поиска и записать в 
// буффер LowerBuf (должен быть не меньше длины слова (Включая завершающий \'0') или,
// если размер слова не известен, максимально возможной длины Constants::MAX_CHARS_IN_WORD) 
// Производятся все необходимые конвертации символов.
// Len - Длина слова
// IsFirstLetterUpperCase - Первая буква слова в верхнем регистре
///////////////////////////////////////////////////////////////////////////////
void PrepareWordForSearch( const TCHAR *wcWord, TCHAR LowerBuf[], size_t &Len, bool &IsFirstLetterUpperCase );

//Класс предоставляет интерфейс для основных методов поиска. Фактически это ограниченный IMorphologyAnalyser
struct IBasicMorphologyAnalyser
{
   virtual ~IBasicMorphologyAnalyser() {}
   
   /// проверка наличия слова в словаре
   virtual bool IsExistWord(/*in*/const wchar_t* wcWord) = 0;

   /// поиск полной морфологической информации по слову
   /// wcWord - слово приведённое к нижнему регистру
   /// wcWordOriginalCase - Слово с оригинальным регистром
   /// IsFirstLetterUpperCase - Находится ли первая буква слова в верхнем регистре
   virtual bool GetWordInfo/*ByWord*/(
      /*in*/const wchar_t* wcWord, 
      /*in*/const wchar_t* wcWordOriginalCase,
      /*in*/bool IsFirstLetterUpperCase,
      /*out*/std::list<SS::Dictionary::Types::TWordInfo>* p_l_WordInfo 
   ) = 0;

   /// поиск полной морфологической информации по индексу слова
   virtual bool GetWordInfo/*ByWordIndex*/(
      /*in*/SS::Dictionary::Types::SWordIndex* pWordIndex, 
      /*out*/SS::Dictionary::Types::SWordInfo* pWordInfo
   ) = 0;

   /// поиск слова по его индексу 
   virtual bool GetWord(
      /*in*/SS::Dictionary::Types::SWordIndex* pWordIndex,
      /*out*/wchar_t* wcWord
   ) = 0;

   /// поиск основы слова слова (части слова без оканчания) по его индексу 
   virtual bool GetWordSource(
      /*in*/const SS::Dictionary::Types::TDictionaryIndex &DictIndex,
      /*out*/wchar_t* wcWordSource
   ) = 0;


   /// поиск словоформ слова по индексу основы
   virtual bool GetWordFormsByIDSource(
      /*in*/SS::Dictionary::Types::TDictionaryIndex* pDictionaryIndex,
      /*out*/std::list<SS::Dictionary::Types::TWordInfo>* p_l_WordInfo
   ) = 0;
};
   
/*
 Класс кеширует в памяти таблицы вида SourceToMorpho и позволяет искать Type по соответствующему Source
 Гарантируется то что вызов только константных методов безопасен в многопоточной среде
*/
class CSourceToType: public OBJECT_LEVEL_LOCKABLE_SYNC_DEP<CSourceToType, SCriticalSectionImplStategy, CURRENT_THREAD_DEBUG_INFO>
{
public:
	typedef unsigned short TTypeIndex;

private:
	///Мы будем представлять данные при помощи вектора индекс элемента в котором 
	///будет Source, а сами элементы будут Morpho. Размер векотра при этом будет 
	///индексом последнего элемента Source. При таком подходе возникает проблема 
	///в том что элемент может отсутствовать в БД, тогда в в качестве Morpho будет 
	///представлено заранее опредилённое значение заведо отсутсвующее в БД. При 
	///таком подходе так же могут возникнуть некотрые издержки в хранении элементов
	///в памяти, но мы ими пренебрегаем.
	typedef std::vector<TTypeIndex> TStorage;

private:
	TStorage m_Storage;
   DBMS::IDataBase *m_pDataBase;
   std::wstring m_TableName;

public:
   CSourceToType(): m_pDataBase(0){}
	
	///Вызывает Init
	CSourceToType(DBMS::IDataBase *pDataBase, const wchar_t *TableName ){ Init(pDataBase, TableName); }

	///Инициализирует данные, перед поиском необходимо ещё вызвать Load 
	void Init(DBMS::IDataBase * pDataBase, const wchar_t *TableName );

   //Загружены ли данные
   bool Empty() const { return m_Storage.empty(); }


   //Загружает данные из таблицы в память
   bool Load();

   //Загружает данные из таблицы в память при условии что они не были ещё загружены
   bool LoadIfNeed(){ if( Empty() ) return Load(); return true; }

	///Признак того что данные найдены не были
	static TTypeIndex EmptyIndex(){ return std::numeric_limits<TStorage::value_type>::max(); }

	///Поиск по данному IDSource значения IDType в закешированных заранее данных таблицы.
	///Если данные не найдены результатом будет EmptyIndex()
	TTypeIndex Find( size_t IDSource ) const
	{
		APL_ASSERT( !Empty() );

      if( IDSource >= m_Storage.size() )
			return EmptyIndex();

		return m_Storage[IDSource];
	}

	//Очищаем данные
	void Clear(){ m_Storage.clear(); }
};

/**
	Класс позволяет кешировать в памяти отношение IDType к IDForm и MorphoInfo из таблиц MorphoTypeToMorphoInfo и 
	MorphoTypes	прозрачно от пользователя. Т.е. при первой попытке поиска данных для IDType в из таблиц загружаются данные 
	и предосталяется по ним поиск. Если пользователь удаляет или добавляет даные то он должен сам следить за уведомлением объекта.
*/
class CIDTypeReferences
{
public:
	class TItem
	{
		//Отношение TMorphoInfo к Номеру записи в таблице
		typedef std::map<TMorphoInfo, UINT32> TMorphoInfoStorage;
		typedef std::map<TIDForm, UINT32> TIDFormStorage;
		
		mutable TMorphoInfoStorage  m_MorphoInfoStorage;
		mutable TIDFormStorage	m_IDFormStorage;

		friend CIDTypeReferences;

	public:
		static UINT32 UndefinedRecordID(){ return std::numeric_limits<UINT32>::max(); }
		
		//Количество MorphoInfoCount и IDFormCount
		size_t MorphoInfoCount() const { return m_MorphoInfoStorage.size(); }
		size_t IDFormCount() const { return m_IDFormStorage.size(); }

		//Есть ли MorphoInfo
		bool IsExistsMorphoInfo( const TMorphoInfo &MI ) const
		{ return m_MorphoInfoStorage.find(MI) != m_MorphoInfoStorage.end(); }

		//Есть ли IDForm
		bool IsExistsIDForm( const TIDForm &IDForm ) const
		{ return m_IDFormStorage.find(IDForm) != m_IDFormStorage.end(); }

		//Добавить элемент MI или IDForm и соответствующий номер записи
		void AddMorphoInfo( const TMorphoInfo &MI, UINT32 RecordNum = UndefinedRecordID() )
		{ APL_ASSERT( !IsExistsMorphoInfo(MI) ); m_MorphoInfoStorage.insert(TMorphoInfoStorage::value_type(MI,RecordNum)); }

		void AddIDForm( const TIDForm &IDForm, UINT32 RecordNum = UndefinedRecordID() )
		{ APL_ASSERT( !IsExistsIDForm(IDForm) ); m_IDFormStorage.insert(TIDFormStorage::value_type(IDForm, RecordNum)); }

		void DeleteMorphoInfo( const TMorphoInfo &MI )
		{ APL_ASSERT( IsExistsMorphoInfo(MI) ); m_MorphoInfoStorage.erase(MI); }
		
		void DeleteIDForm( const TIDForm &IDForm )
		{ APL_ASSERT( IsExistsIDForm(IDForm) ); m_IDFormStorage.erase(IDForm); }

		//Возвращает номер записи который был ранее сопоставлен MI или IDForm
		//Если вообще нет такой записи возвращается UndefinedRecordID()
		UINT32 GetMorphoInfoRecordID( const TMorphoInfo &MI ) const
    	{
			TMorphoInfoStorage::const_iterator I = m_MorphoInfoStorage.find(MI);
			return I == m_MorphoInfoStorage.end() ? UndefinedRecordID() : I->second;
		}

		UINT32 GetIDFormRecordID( const TIDForm &IDForm ) const
		{
			TIDFormStorage::const_iterator I = m_IDFormStorage.find(IDForm);
			return I == m_IDFormStorage.end() ? UndefinedRecordID() : I->second;
		}

	};

	typedef std::map<TIDType, TItem> TStorage;

	//Значения битового поля показывающие из каких таблиц считывать значения
	enum eUseTables
	{
		UTNotUseAnyTable = 0,
		UTMorphoTypes = 1 << 0,
		UTMorphoTypeToMorphoInfo = 1 << 1,
		UTAll = UTMorphoTypes | UTMorphoTypeToMorphoInfo
	};

private:
	//Поскольку кэширование происходит прозрачно от пользователя, нам необходимо сделать переменные mutable
	mutable TStorage m_Storage;
	mutable DBMS::IDataTable *m_pMorphoTypes;
	mutable DBMS::IDataTable *m_pMorphoTypeToMorphoInfo;
	int m_UseTables; //Из каких таблиц читать данные (остальные не используются)


private:
	//Заполнить TItem данными из таблиц m_pMorphoTypes и m_pMorphoTypeToMorphoInfo если для них установлены соответсвующие 
	//флаги в m_UseTables
	void FillItem( const TIDType &IDType, TItem &Item  ) const;

	//Осуществляет поиск напрямую вызывется из Find
	TItem &FindImpl( const TIDType &IDType ) const;

public:
	CIDTypeReferences(): m_pMorphoTypes(0), m_pMorphoTypeToMorphoInfo(0), m_UseTables(UTNotUseAnyTable) {}
	
	//Вызывает Init
	CIDTypeReferences( DBMS::IDataTable *pMorphoTypes,  DBMS::IDataTable *pMorphoTypeToMorphoInfo, int UseTables = UTAll )
	{ Init(pMorphoTypes, pMorphoTypeToMorphoInfo, UseTables); }

	//Инициализация  указателей на таблицы и очистка данных
	void Init( DBMS::IDataTable *pMorphoTypes,  DBMS::IDataTable *pMorphoTypeToMorphoInfo, int UseTables = UTAll )
	{
		m_UseTables = UseTables;
		m_pMorphoTypes = pMorphoTypes;
		m_pMorphoTypeToMorphoInfo = pMorphoTypeToMorphoInfo;
		m_Storage.clear();
		
		APL_ASSERT( (m_UseTables & UTMorphoTypes) == 0  || m_pMorphoTypes != 0  );
		APL_ASSERT( (m_UseTables & UTMorphoTypeToMorphoInfo) == 0 || m_pMorphoTypeToMorphoInfo != 0  );
	}

	//Осуществляет поиск связей IDType к IDForm и MorphoInfo. Читает данные из таблиц если ещё не прочитаны и 
	//предоставляет их пользователю
	TItem &Find( const TIDType &IDType ){ return FindImpl(IDType); }
	const TItem &Find( const TIDType &IDType ) const{ return FindImpl(IDType); }
};

///////////////////////////////////////////////////////////////////////////////
// Ковертирует римские в арабские числы и наоборот. 
// При записи римских чисел существует след ограничения:
// 1. Цифры в числе должны идти по неубыванию их значимости, т.е. X должно быть левее V.
//    Одинаковые цифры могут идти подряд. При этом учитываются также составные цифры (IV, IX, XL, XC, CD, CM)
// 2. Максимальное число, которое можно записать римскими цифрами, при помощи символов без чёрточек 
//    наверху, не нарушая правил Шварцмана (правил записи римских цифр) - 3999 (MMMCMXCIX) - больше 
//    трех не составных цифр писать нельзя.
// 3. Количество подряд идущих цифр должно быть меньше: 
//    floor(<десятичное значение следующего числа>/<десятичное значение текущего числа>) 
///////////////////////////////////////////////////////////////////////////////
class CRomanArabicConvertor
{
	//Информация по римским цифрам
   struct TRoman2ArabicBase
	{
		const TCHAR * const szRoman;        //Литеральное представление 
		unsigned int Arabic;                //Численное значение
      unsigned char MaxSequenceLength;    //Максимальное допустимое количество подряд идущих цифр
	};

	static const TRoman2ArabicBase rgRoman2ArabicBase[];

private:
   //Преобразование символа в нижний регистр (Не используется гарантируется, то 
   //что поиск будет работать с нижнем регистром 
   static TCHAR ToLowerImpl( TCHAR C ) { return C; }

public:
   //Максимальное римское число
   static unsigned int MaxRomanDigit(){ return 3999; }
   
   //Конвертирует арабское число N в римское szResult. Записывает в конце строки '\0'
   //BuffSize - Максимальный размер буфера (включая место под '\0')
   //Возвр: Указатель на конец строки (т.е. на '\0') при успешном пробразовании и 0 если 
   //       кончился буффер 
	static TCHAR *Arabic2Roman( unsigned int N, TCHAR *szResult, size_t BuffSize );

	//Конвертирует римское число [Begin, End) в арабское N. 
   //Возвр: первый символ который не удалось сконвертировать в арабское число (или End при удачной конвертации всех чисел)
	static const TCHAR *Roman2Arabic( const TCHAR *Begin, const TCHAR *End, unsigned int &N );
};

///////////////////////////////////////////////////////////////////////////////
// Конвертирует сокращённую запись порядковых числительных (1st, 2nd) в числа 
// и наоборот 
///////////////////////////////////////////////////////////////////////////////
class CNumericOrderingConverter
{
   static const TCHAR * const rgPostfix[];

private:
   //Возвращает указатель на строку которую надо добавить к числу, для того
   //чтобы получить сокращённую запись порядковых числительных
   static const TCHAR *GetLiteralString( unsigned int N );

public:
   //Конвертирует число N в cокращённую запись порядковых числительных szResult. 
   //Записывает в конце строки '\0'
   //BuffSize - Максимальный размер буфера (включая место под '\0')
   //Возвр: Указатель на конец строки (т.е. на '\0') при успешном пробразовании и 0 если 
   //       кончился буффер 
   static TCHAR *FromNumber( unsigned int N, TCHAR *szResult, size_t BuffSize );

   //Конвертирует cокращённую запись порядковых числительных [Begin, End) в число N. 
   //Возвр: первый символ который не удалось сконвертировать в  число
   //(или End при удачной конвертации всех чисел)
   static const TCHAR *ToNumber( const TCHAR *Begin, const TCHAR *End, unsigned int &N );
};


///////////////////////////////////////////////////////////////////////////////
// Реализует кеширование, поиск и редактирование данных в таблице новых слов.
// Кеширование организуется следующим образом:
//		При поиске, сначала проверяются данные в кеше, если там их нет то в таблице, если в
//		ней нашли то переносим слово в кеш.
//		При добавлении, добавляется как в кеш (если там ещё нет) так и на диск (если и там ещё нет).
//		При удалении удаляется как из кеша так и из таблицы.
//
// Позволяет работать только с кешом в памяти, не сбрасывая и не читая табличные данные.
//
// Позволяет искать по Source - строковому продставлению основы и по IDSource -
// индексу основы. Причём IDSource всегда должен быть уникальным, а Source может 
// повторяться и при поиске по Source возвращается несколько значений в общем случае
///////////////////////////////////////////////////////////////////////////////
class CNewWordsCache
{
public:
   typedef wchar_t TChar;
   typedef std::basic_string<TChar> TString;

   //Хранимые данные 
   class CSourceData
   {
      TDictionaryIndex m_IDSource; //идентификатор основы
      TIDType m_IDType;
      TString m_Source;
      TString m_SourceLower;       //Source маленькими буквами используется для поиска
   
   public:
      CSourceData():  m_IDType(0) {}

      const TDictionaryIndex &GetIDSource() const{ return m_IDSource; }
      const TIDType &GetIDType() const { return m_IDType; }
      const TString &GetSource() const{ return m_Source; }
      
      //Возвращает Source для будующего поиска (нижний регистр)
      const TString &GetSourceForFind() const{ /*return m_SourceLower;*/ return m_Source; }

      void SetIDSource(const TDictionaryIndex &V) { m_IDSource = V; } 
      void SetIDType(const TIDType &V) { m_IDType = V; } 
      void SetSource(const TString &V) { m_Source = V; ToLower( V, m_SourceLower); }

      //Установить Source с уже известным представлением Source маленькими буквами.
      //Работает быстрее чем SetSource
      void SetSourceWithSourceLower( const TString &Source, const TString &SourceLower )
      {  m_Source = Source; m_SourceLower = SourceLower; }

      struct TDictionaryIndexesSet: public std::binary_function<CSourceData, CSourceData, bool>
      {
         bool operator()( const CSourceData &SD1, const CSourceData &SD2 ) const
         {
            return SD1.GetIDSource().GetId() == SD2.GetIDSource().GetId() ||
               ( SD1.GetSource() == SD2.GetSource() && SD1.GetIDType() == SD2.GetIDType() );
         }
      };
   };

private:
   // Организуем кеширование, следующим образом: Будем хранить структуры CSourceData в списке и 
   // организуем два индекса по IDSource и Source при помощи hash_map в котором как значение будет 
   // использоваться итератор списка.
   // При этом будет выполнятся следующее правила: 
   // 1. Если в индексе по конкретному Source есть данные то в индексе по
   //    IDSource так же обязательно есть все данные для этих Source (Source не уникален). 
   // 2. Но если в индексе по IDSource есть данные то они не обязательно есть в индексе по Source(IDSource). 
   // Если убрать ограничение правила 2 то тогда придётся при поиске по IDSource искать ещё и по Source, для того чтобы
   // добавить все возможные значения Source, а не только то которое соотвтествует IDSource
   typedef std::list<CSourceData> TStorage;                                        //Список всех сохранённых в кэше даных

#if 0 //У hash_map в стандартной реализации VC резко падает производительность на определённых словах
   typedef stdext::hash_map<unsigned int, TStorage::iterator> TIDSourceIndex;       //Индекс по IDSource.GetID
   typedef stdext::hash_multimap<TString, TStorage::iterator> TSourceIndex;         //Индекс по Source
#else
   typedef std::map<unsigned int, TStorage::iterator> TIDSourceIndex;               //Индекс по IDSource.GetID
   typedef std::multimap<TString, TStorage::iterator> TSourceIndex;                 //Индекс по Source
#endif
   typedef ClassLevelLockable<CNewWordsCache> TMutex;                              //Блокирование таблицы при добавлении
   typedef std::pair<TSourceIndex::iterator, TSourceIndex::iterator> TSourceIndexIterPair;


public:
   //Класс используется при поиске по Source и позволяет абстрагироваться от конкретного
   //способа прохода по контейнеру, не является итератором в привычном смысле слова, хотя и 
   //реализуется на основе итераторов. 
   //TODO: Убрать курсор и сделать CSelectIterator 
   class CCursor
   {
      typedef std::pair<TSourceIndex::const_iterator, TSourceIndex::const_iterator> TImpl;
      TImpl m_Impl;
      
      explicit CCursor( const TImpl &Impl ): m_Impl(Impl) {}
      
      //Пустой ли интервал
      static bool Empty( const TImpl &Impl ){ return Impl.first == Impl.second; }

      friend class CNewWordsCache;
   public:
      CCursor() { m_Impl.first = m_Impl.second; }

      //Закончилася ли интервал
      bool End() const { return Empty(m_Impl); }
      void Next() {APL_ASSERT(!End()); ++m_Impl.first; } 
      const CSourceData &Get() const { APL_ASSERT(!End()); return *m_Impl.first->second; }
   };

private:
   //mutable поскольку у нас кеширование происходит прозрачно от пользователя 
   mutable TStorage m_Storage;
   mutable TIDSourceIndex m_IDSourceIndex;
   mutable TSourceIndex m_SourceIndex;
   DBMS::IDataTable *m_pTable;   //Таблица в бд в которой хранятся данные, может быть 0
   TDictNumber m_DictNumber;
   UINT32 m_NextAddID;           //Следующий индекс который необходимо присовить IDSource при добавлении записи
   TMutex m_Mutex;               //Мютекс уровня класса. Необходим для корректного добавления, при условии что несколько объектов используют одну таблицу


private:
   //Конвертировать строку в нижний регистр, Src и Dst могут совпадать
   static void ToLower( const TString &Src, TString &Dst )
   {
      std::vector<TChar> Buff( Src.begin(), Src.end() ); 
      CharLowerBuff( &Buff[0], static_cast<DWORD>(Buff.size()) );
      Dst.assign( Buff.begin(), Buff.end() );
   }

   //Ищет в DBMS таблице значение по IDSource
   bool FindInTableByIDSource( const TDictionaryIndex &IDSource, CSourceData &SourceData ) const;
     
   //Удаляет в DBMS таблице значение по IDSource
   bool DeleteInTableByIDSource( const TDictionaryIndex &IDSource );
   
   //Удаляет в DBMS таблице значение по Source
   //Source - приведёная к нижнему регистру строка
   bool DeleteInTableBySource( const TString &Source );

   //Добавляет данные в таблицу
   //Игнорирует SourceData.GetIDSource(). Генерит нужный индекс и добавляет по нему в таблицу.
   //Индекс записывается в m_NextAddID
   void AddToTable( const CSourceData &SourceData );

public:
   CNewWordsCache(): m_pTable(0), m_DictNumber(DATA_TYPE::NAMES::ednUnknown), m_NextAddID(1) {}
   
   //Вызываем SetTable
   explicit CNewWordsCache( DBMS::IDataTable *pTable, TDictNumber DictNumber ): m_pTable(0), m_DictNumber(DATA_TYPE::NAMES::ednUnknown), m_NextAddID(1)
   { SetTable(pTable, DictNumber); }
  
   //Если передан не нулевой указатель то класс будет искать в таблице, (см. описание класса) иначе если передавать 0
   //или вообще не вызывать функцию все изменения будет происходить только в памяти
   //DictNumber - Номер словаря с которым предполагается работать
   void SetTable( DBMS::IDataTable *pTable, TDictNumber DictNumber );
   
   //Уничтожает данные в памяти (очищает кеш)
   void Clear();
   
   //Поиск по IDSource, при неудачной попытке поиска возвращается 0, подробнее см. описание класса
   const CSourceData *FindByIDSource( const TDictionaryIndex &IDSource ) const;
   
   //Возвращает курсор всех сохранёных в кэше данных
   CCursor GetData() const
   {
      return CCursor(CCursor::TImpl(m_SourceIndex.end(), m_SourceIndex.end()));
   }

   //Поиск по Source, возвращает курсор найденных значений, подробнее см. описание класса
   //Отсутствие слова в словаре можнро проверить методом CCursor::End()
   CCursor FindBySource( const TString &Source ) const
   {  TString LString; ToLower( Source, LString ); return FindBySourceLower(LString); }

   //Делает тоже что и FindBySource, но требует чтобы Source был приведён к нижнему регистру, благодаря этому 
   //работает быстрее
   CCursor FindBySourceLower( const TString &Source ) const;

   //Удаление по IDSource, если есть такая запись, подробнее см. описание класса
   void DeleteByIDSource( const TDictionaryIndex &IDSource );
   
   //Удаление по Source, если есть такая запись, подробнее см. описание класса
   void DeleteBySource( const TString &Source )
   {  TString LString; ToLower( Source, LString ); DeleteBySourceLower(LString);  }

   //Делает тоже что и DeleteBySource, но требует чтобы Source был приведён к нижнему регистру, благодаря этому 
   //работает быстрее
   void DeleteBySourceLower( const TString &Source );

   //Добавление новых данных.Функция не проверяет 
   //наличие SourceData в словаре и вставляет в любом случае
   //SourceData.GetIDSource() игнорируется при добавлении, выбирается уникальный идентификатор и
   //после добавления его в таблицу записывается в SourceData
   //Прверяется 
   void Add( CSourceData &SourceData );
};

} //namespace Detail
} //namespace Dictionary
} //namespace SS


