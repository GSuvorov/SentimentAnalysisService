#pragma once

#include "../ASCInterface/IInterface.h"
#include "./ICommon.h"
#include "./TDictionaryManager.h"


// {05e6e5e0-6b5e-4b2e-955c-51de5af6a45a} 
static const GUID CLSID_IDictionary = 
{ 0x05e6e5e0, 0x6b5e, 0x4b2e, { 0x95, 0x5c, 0x51, 0xde, 0x5a, 0xf6, 0xa4, 0x5a } };

// {d9a37c0a-aabd-47c9-9e90-1b136fd40439} 
static const GUID IID_IDictionary = 
{ 0xd9a37c0a, 0xaabd, 0x47c9, { 0x9e, 0x90, 0x1b, 0x13, 0x6f, 0xd4, 0x04, 0x39 } };

// {34D63D88-DDF4-4e1d-B84C-E01FE12EB1F4}
static const GUID CLSID_IMinimalHypothesisTreeBuilder = 
{ 0x34d63d88, 0xddf4, 0x4e1d, { 0xb8, 0x4c, 0xe0, 0x1f, 0xe1, 0x2e, 0xb1, 0xf4 } };

// {B12A11BB-20B5-4e9f-8E8E-97E34D1336EC}
static const GUID IID_IMinimalHypothesisTreeBuilder = 
{ 0xb12a11bb, 0x20b5, 0x4e9f, { 0x8e, 0x8e, 0x97, 0xe3, 0x4d, 0x13, 0x36, 0xec } };


namespace SS
{
namespace Interface
{
namespace Core
{
namespace Dictionary
{

//Позволяет по существующим словам словаря создать дерево по которому можно определить
//морфоинформацию для слов, отсутствующих в словаре. 
//Сохраняет дерево в таблицу DBMS::IDataTable 
class IMinimalHypothesisTreeBuilder: public virtual SS::Interface::IBase
{
public:
   /// деструктор
	virtual ~IMinimalHypothesisTreeBuilder() {};
	
	/// Инициализация (очиска) дерева. Удалить все предварительно добавленные слова и морфоинформацию.
	virtual bool Init( SS::Dictionary::Types::TDictNumber DictNumber ) = 0;

	/// Добавление в дерево слова с набором морфоинформации (список IDType).
	//Необходимо вызвать для каждого уникального слова словаря и морфоинформации по нему.
	//    pwReverseWord - Добавляемое слово. Представлено в инвертированном виде (вместо "слон" необходимо передать "нолс")
	//    pTypesList - Набор IDType которые могут быть выведены по слову Str
	//    Вовращает: Удалось ли добавить слово (если такое слово уже есть в дереве или если все IDType 
   //               в pTypesList не являются продуктивными, добавление не происходит)
	virtual bool AddReverseWord( const std::wstring* pwReverseWord, const SS::Dictionary::Types::TTypesList* pTypesList ) = 0;

	/// построение дерева и сохранение его в таблицу 
	virtual void Build() = 0;
};
/// интерфейс работы с типами морфообразования

/// интерфейс работы с наполнением типов морфообразования
class IMorphoTypesCollection
{
public:
	virtual ~IMorphoTypesCollection(){};

	virtual void SetDictionaryNumber(SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber eDictionariesNumber) = 0;
						  
	/// удаляет тип морфообразования
	virtual void DeleteType( const SS::Dictionary::Types::TIDType &IDType ) = 0;

	/// возвращает список элементов  типа морфообразования
	virtual void GetElements(
		const SS::Dictionary::Types::TIDType &IDType,
		std::list<SS::Dictionary::Types::SMorphoType>* p_l_MorphoTypes
	) = 0;
	
	/// добавляет список элементов 
	/// Возвращает:
	///		S_OK - успешном добавлении всех записей. 
	///     E_INVALIDARG - некоторые записи добавить не удалось
	virtual HRESULT AddElements(std::list<SS::Dictionary::Types::SMorphoType>* p_l_MorphoTypes) = 0;

   ///Создаёт новый тип для списка элементов
   /// Возвращает:
   ///	  S_OK - успешном добавлении всех записей. 
   ///     E_INVALIDARG - некоторые записи добавить не удалось
   virtual HRESULT CreateCollection(std::list<SS::Dictionary::Types::SMorphoType>* p_l_MorphoTypes) = 0;

	
	/// удаляет список элементов
	/// Возвращает:
	///		S_OK - успешном удалении всех записей. 
	///     E_INVALIDARG - некоторые записи удалить не удалось
	virtual HRESULT DeleteElements(std::list<SS::Dictionary::Types::SMorphoType>* p_l_MorphoTypes) = 0;
};


/// интерфейс работы с однокоренными словами
class IOneRoot 
{
public:
	virtual ~IOneRoot(){};
public:
	/// устанавливает индекс слова
	virtual unsigned int SetDictionaryIndex(SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex) = 0;
	/// возвращает список однокоренных слов для текущего индекса основы
	virtual void Get(std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_IDSource) = 0;
	/// добавляет однокоренные слова к текущему
	virtual void Add(std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_IDSource) = 0;
	/// удаляет однокоренные слова у текущего слова
	virtual void Delete(std::list<SS::Dictionary::Types::TDictionaryIndex>* p_l_IDSource) = 0;
};

/// интерфейс работы с основами слов
class ISource
{
public:
	virtual ~ISource(){};
public:
	/// устанавливает индекс слова
	/// возвращает 0 - когда некорректный индекс и 1 когда корректный
	virtual unsigned int SetDictionaryIndex(SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex) = 0;
	/// возвращает всю доступную информацию об основе
	virtual unsigned int Get(SS::Dictionary::Types::SSource* pSource) = 0;
	/// Добавлят всю доступную информацию для основы
	virtual void Add(SS::Dictionary::Types::SSource* pSource) = 0;
	/// удаляет всю информацию об основе
	virtual void Delete() = 0;
};

/// Расширенный интерфейс работы с основыми слов

class ISourceEx: public ISource
{
public:
   virtual ~ISourceEx(){};

   /// Изменяет информацию пл теущему IDSource
   virtual void Update(SS::Dictionary::Types::SSource* pSource) = 0;
};


class IMorphologyAnalyser : 
    public virtual SS::Interface::Core::Common::CAnalyseParams
{
public:
	virtual ~IMorphologyAnalyser(){};
public:
	/// проверка наличия слова в словаре
   virtual bool IsExistWord(/*in*/const wchar_t* wcWord) = 0;


   ///// поиск полной морфологической информации по слову 
   ///// pUnit приходит на сход созданным,
   ///// задача морфологического анализатора заполнить индексы IUnit::AddIndex() IIndex::SetDictionaryIndex()
   ///// и заполнить IIndex::IFeature (метод AddFeature())
   ///// один морфологический индекс может содержать несколько разных IFeature (хранит информацию MorphoIndex() = IDForm,MorphoInfo(),PartOfSpeech())
   ///// создание любых контейнеров через IBlackBoxTextStorage
   ///// если слово составное, то у IUnit необходимо создать детей AddUnitVertical()AddUnitHorizontal()
   //virtual bool GetWordInfo/*ByWord*/(	
	  ///*in*/ const wchar_t* szWord,
   //   /*in out*/	SS::Interface::Core::BlackBox::IUnit * pUnit,
   //   /*in*/SS::Dictionary::Types::EFindZone eFindZone = SS::Dictionary::Types::efzFullAddNewWords
   //   ) = 0;


   /// поиск полной морфологической информации по слову 
	virtual bool GetWordInfo/*ByWord*/(
		/*in*/const wchar_t* wcWord, 
      /*out*/std::list<SS::Dictionary::Types::TWordInfo>* p_l_WordInfo, 
      /*in*/SS::Dictionary::Types::EFindZone eFindZone = SS::Dictionary::Types::efzFullAddNewWords
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
	
   /// поиск основы слова (части слова без оканчания) по его индексу 
   virtual bool GetWordSource(
      /*in*/const SS::Dictionary::Types::TDictionaryIndex &DictIndex,
      /*out*/wchar_t* wcWordSource
   ) = 0;

   /// поиск словоформ слова по индексу основы
    virtual bool GetWordFormsByIDSource(
	/*in*/SS::Dictionary::Types::TDictionaryIndex* pDictionaryIndex,
	/*out*/std::list<SS::Dictionary::Types::TWordInfo>* p_l_WordInfo) = 0;
	
   /// возвращает морфологическую информацию по типу морфообразования
	virtual void GetMorphoInfoByIDType(
	/*in*/SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber eDictionariesNumber,
	/*in*/SS::Dictionary::Types::TIDType oIDType, 
	/*in*/SS::Dictionary::Types::TIDForm oIDForm, 
	/*out*/SS::Dictionary::Types::TMorphoInfo* pMorphoInfo, 
	/*out*/SS::Dictionary::Types::TSpecialMorpho* pSpecialMorpho) = 0;
public:
	/// сообщает морфологическому словарю о том, необходимо переходить к анализу очередного текста
	virtual void NextText() = 0;
};

/// интерфейс работы со списками слов различных словарей
class IWordsList
{
public:
	virtual ~IWordsList(){};
public:
	/// выставляет номер словаря, из которого необходимо получать список слов
	virtual void SetDictionaryNumber(/*in*/SS::Dictionary::DATA_TYPE::NAMES::DictionariesNumber eDictionariesNumber) = 0;
	/// вернет колличество слов данного типа
	virtual unsigned int GetCount() = 0;
	/// перейдет к первому слову в списке (в порядке занесения).
	virtual unsigned int MoveFirst() = 0;
	/// перейде к следующему слову в списке (в порядке занесения).
	/// если результат функции true следовательно слово есть, если false то слов больше нет
	virtual unsigned int MoveNext(/*out*/SS::Dictionary::Types::TWordInfo * pWordInfo) = 0;
	/// вернет текущее слово
	virtual void GetWord(/*out*/SS::Dictionary::Types::TWordInfo * pWordInfo) = 0;

};

/// интерфейс управления морфологическим словарем
class IDictionary: 
	public virtual SS::Interface::IBase,
    public virtual SS::Interface::Core::Common::CAnalyseParams
{
public:
	virtual ~IDictionary(){};
public:
	/// возвращает интерфейс работы с однокоренными словами
	virtual SS::Interface::Core::Common::ILoad* GetLoader() = 0;
	
   /// возвращает интерфейс работы с однокоренными словами
	virtual IOneRoot* GetOneRoot() = 0;
	
   /// возвращает интерфейс работы с типами морфообразования
	virtual IMorphoTypesCollection* GetMorphoTypesCollection() = 0;
	
   /// возвращает интерфейс получения и редактирования информации об основах слов
   /// в базовых словарях имён собственных и нарицательных
	virtual ISource* GetSource() = 0;
	
   /// возвращает интерфейс получения и редактирования информации об основах слов
   /// словарей аббревиатур.
   /// Слово может храниться под одним и тем же идентификатором как в базовом словаре 
   /// так и в словаре аббревиатур, поэтому при удалении необходимо использовать
   /// оба интерфейса.
   virtual ISourceEx* GetAbbreviationSource() = 0;

   /// возвращает интерфейс морфологического анализа
	virtual IMorphologyAnalyser* GetMorphologyAnalyser() = 0;
	
   /// возвращает интерфейс для получения списков слов определенного типа,
	/// где eDictionariesNumber - номер словаря, для которого необходимо вывести списки слов
	virtual IWordsList* GetWordList() = 0;
};

}
}
}
}