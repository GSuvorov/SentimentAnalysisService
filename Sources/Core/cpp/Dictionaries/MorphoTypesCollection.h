#pragma once

namespace SS{ 
namespace Dictionary{
namespace Edit{

/**
	Класс позволяет редактировать таблицы MorphoTypeToMorphoInfo и 
	MorphoTypes
*/
class CMorphoTypesCollection
{
private:
   typedef ClassLevelLockable<CMorphoTypesCollection> TMutex; //Блокирование таблицы при добавлении
private:
	DBMS::IDataBase *m_pDataBase;
	CTablePtr m_pMorphoTypes;
	CTablePtr m_pMorphoTypeToMorphoInfo;
	TMutex m_Mutex;                  //Мютекс уровня класса. Необходим для корректного добавления, при условии что несколько объектов используют одну таблицу

private:
	static const wchar_t *szMorphoTypesTable;
	static const wchar_t *szMorphoTypeToMorphoInfoTable;

private:
	void Open();
	void Close();
   //В связи с тем что у одного IDType формы обязательно должны начинаться с нуля и идти по порядку, нельзя
   //оставлять "дыр" в порядковых номерах IDType
   //Функция востанавливает правильный порядок следования IDForm у некоторого IDType
   //IDType - тип у которого необходимо востанавливать формы
   void RestoreCorrectIDFormOrder( const TIDType &IDType  );

   //Заменить в строке RecordID таблицы m_pMorphoTypes значение IDForm на NewIDForm
   void ReplaceIDForm( UINT32 RecordID, TIDForm NewIDForm );

public:
	CMorphoTypesCollection() : m_pDataBase(NULL) {}

	///Иницализация таблиц, необходимо вызывать перед вызовом первого метода
	void Init( DBMS::IDataBase *pDataBase );


	/// удаляет текущий тип морфообразования
	/**
		Алгоритм:
			Удаляем все записи в таблице MorphoTypes у которых IDType равен текущему	
	*/
	void DeleteType(const TIDType &IDType);

	/// возвращает список элементов текущего типа морфообразования
	/**
		Алгоритм:
			Находим все записи в таблице MorphoTypes у которых IDType равен текущему и присоединяем к ней
			таблицу	MorphoTypeToMorphoInfo механизмом похожим на SQLский INNER JOIN
	*/
	void GetElements(const TIDType &IDType, std::list<SMorphoType>* p_l_MorphoTypes);

	/// добавляет список элементов
	/// Возвращает:
	///	  S_OK - успешном добавлении всех записей. 
	///     E_INVALIDARG - некоторые записи добавить не удалось
	/**
		Алгоритм:
			Для каждого p_l_MorphoTypes 
				Если нет записей в MorphoTypes с текущими IDType и IDForm
					Вставляем в MorphoTypes и MorphoTypeToMorphoInfo по одной записи
				Иначе если нет записи в таблице MorphoTypeToMorphoInfo с MorphoInfo равным текущему
					Вставляем одну запись в MorphoTypeToMorphoInfo
				Иначе
					Возвращаем E_INVALIDARG, но не прерываем добавление остальных записей
	*/
	HRESULT AddElements(std::list<SMorphoType>* p_l_MorphoTypes);

   ///Создаёт новый тип для списка элементов
   /// Возвращает:
   ///	  S_OK - успешном добавлении всех записей. 
   ///     E_INVALIDARG - некоторые записи добавить не удалось
   /**
      Алгоритм:
         Находит в таблице MorphoTypes (GetUserValue, SetUserValue) уникальный IDType и записываем его в
         каждый элемент списка p_l_MorphoTypes затем вызывает AddElements и возвращает то что вернул 
         AddElements
   */
   HRESULT CreateCollection(std::list<SMorphoType>* p_l_MorphoTypes);

	/// удаляет список элементов
	/// Возвращает:
	///		S_OK - успешном удалении всех записей. 
	///     E_INVALIDARG - некоторые записи удалить не удалось
	/**
		Алгоритм:
			Удаляем из таблицы MorphoTypes все записи с текущими IDType и IDForm при условии что для текущего IDType после 
			удаления ещё останутся записи с отличными от текущего IDForm. Если таких записей нет то возвращаем E_INVALIDARG, 
			но обработку остальных записей не приостанавливаем
	*/
	HRESULT DeleteElements(std::list<SMorphoType>* p_l_MorphoTypes);
};



///////////////////////////////////////////////////////////////////////////////
// Адаптирует CMorphoTypesCollection под интерфейс IMorphoTypesCollection
// Является компановщиком нескольких объектов CMorphoTypesCollection
///////////////////////////////////////////////////////////////////////////////
class CMorphoTypesCollectionAdapter: public IMorphoTypesCollection
{
public:
   //Структура хранит инициализационные данные для инстанцирования отдельных объектов CMorphoTypesCollection
   struct SInitStruct
   {
      //Данные для вызова CMorphoTypesCollection::Init
      DBMS::IDataBase *pDataBase;

      //Набок лючей которые можно сопоставить данной базе
      typedef std::vector<TDictNumber> TKeys;
      TKeys Keys;
   };

private:
   typedef CObjectsSelectorMultiKey<TDictNumber, CMorphoTypesCollection> TMorphoTypesCollections;

private:
   TMorphoTypesCollections m_MorphoTypesCollections;
   CMorphoTypesCollection *m_pCurMorphoTypesCollection;

public:
   CMorphoTypesCollectionAdapter(): m_pCurMorphoTypesCollection(0) {}

   //Создаёт и инициализивует внутренние объекты CMorphoTypesCollection при помощи итераторов [First, Last) ссылающихся на структуры SInitStruct
   //Предыдущие объекты удаляются
   template<class InputIterator>
      void Init( InputIterator First, InputIterator Last )
   {
      TMorphoTypesCollections::TAutoPtr AutoPtr;

      m_MorphoTypesCollections.Clear();
      m_pCurMorphoTypesCollection = 0;

      for( ; First != Last; ++First )
      {
         AutoPtr.reset( new CMorphoTypesCollection() );

         AutoPtr->Init(
            First->pDataBase
         );

         APL_ASSERT( !First->Keys.empty() );

         if( !m_MorphoTypesCollections.Add(First->Keys.begin(), First->Keys.end(), AutoPtr) )
         {
            m_MorphoTypesCollections.Clear();
            SS_THROW( L"Ошибка CMorphoTypesCollectionAdapter::Init" );
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   // IMorphoTypesCollection
   
   void SetDictionaryNumber(TDictNumber eDictionariesNumber);

   /// удаляет тип морфообразования
   void DeleteType( const SS::Dictionary::Types::TIDType &IDType );

   /// возвращает список элементов  типа морфообразования
   void GetElements(
      const SS::Dictionary::Types::TIDType &IDType,
      std::list<SS::Dictionary::Types::SMorphoType>* p_l_MorphoTypes
   );

   /// добавляет список элементов 
   /// Возвращает:
   ///	  S_OK - успешном добавлении всех записей. 
   ///     E_INVALIDARG - некоторые записи добавить не удалось
   HRESULT AddElements(std::list<SS::Dictionary::Types::SMorphoType>* p_l_MorphoTypes);

   ///Создаёт новый тип для списка элементов
   /// Возвращает:
   ///	  S_OK - успешном добавлении всех записей. 
   ///     E_INVALIDARG - некоторые записи добавить не удалось
   HRESULT CreateCollection(std::list<SS::Dictionary::Types::SMorphoType>* p_l_MorphoTypes);

   /// удаляет список элементов
   /// Возвращает:
   ///	  S_OK - успешном удалении всех записей. 
   ///     E_INVALIDARG - некоторые записи удалить не удалось
   HRESULT DeleteElements(std::list<SS::Dictionary::Types::SMorphoType>* p_l_MorphoTypes);
};

} //namespace Edit
} //namespace Dictionary
} //namespace SS
