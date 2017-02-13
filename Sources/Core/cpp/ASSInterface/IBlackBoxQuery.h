#pragma once

#include "IBlackBoxItems.h"
#include "IBlackBoxDictionaryIndex.h"
#include "../FeatureLibrary/define.h"
#include "./CObligatory.h"

#include "TDictionaryManager.h"


namespace SS
{
namespace Interface
{
namespace Core
{
namespace BlackBox
{
namespace Query
{

	class ITransformation;
	class IGroup;
	class IQueryUnit;
	class IQueryUnitIndex;
	class IQueryUnitIndexCollection;

	///типы трансформации
	enum ETransformationType
	{
		///неопределен
		ettUndefined,
		///простая
		ettSimple,
		///транзитивная
		ettTransitive,
		///нетранзитивная
		ettIntransitive,
		///определительная
		ettDefinitive
	};

	///хранилище трансформаций (всех вариантов желаемых ответов)
	class IQuery
	{
	public:
		///добавление транчформации в конец
		virtual void AddTransformation(ITransformation* pTransformation) = 0;

		///получение указателя на первую транчформацию
		virtual ITransformation* GetFirstTransformation() = 0;
		///получение указателя на последнюю транчформацию
		virtual ITransformation* GetLastTransformation() = 0;

		///проверка на пустоту
		virtual bool IsEmpty() = 0;

		///очистка хранилища, включая обнуление переменных и удаление всех трансформаций
		virtual void Clear() = 0;
		///удаление группы
		virtual void Release() = 0;
	};

	///хранилище групп объектов 
	/**хранилище групп объектов (шаблонов ответов, предикатов, сущностей),  
	которые должны присутствовать в текущей трансформации (варианта желаемого ответа)*/
	class ITransformation : public CObligation
	{
	public:
		///добавление группы в конец
		virtual void AddGroup(IGroup* pGroup) = 0;

		///получение указателя на следующую трансформацию
		virtual ITransformation* GetNextTransformation() = 0;
		///получение указателя на предыдующую трансформацию
		virtual ITransformation* GetPrevTransformation() = 0;

		///получение указателя на первую группу
		virtual IGroup* GetFirstGroup() = 0;
		///получение указателя на последнюю группу
		virtual IGroup* GetLastGroup() = 0;

		///очистка трансформации, включая обнуление переменных и удаление всех групп
		virtual void Clear() = 0;
		///удаление трансоформации, вернет указатель на следующюю(справа) трансформацию
		virtual ITransformation* Release() = 0;
		
		///проверка на пустоту
		virtual bool IsEmpty() = 0;
	};

	///хранилище объектов, входящих в одну из групп объектов 
	/**хранилище объектов, входящих в одну из групп объектов (шаблонов ответов, предикатов, сущностей),  
	которые должны присутствовать в текущей трансформации (варианта желаемого ответа)*/
	class IGroup : public CObligation
	{
	public:
		///добавление юнита в конец
		virtual void AddQueryUnit(IQueryUnit* pQueryUnit) = 0;

		///получение указателя на следующую группу
		virtual IGroup* GetNextGroup() = 0;
		///получение указателя на предыдующую группу
		virtual IGroup* GetPrevGroup() = 0;

		///получение указателя на первый юнит
		virtual IQueryUnit* GetFirstQueryUnit() = 0;
		///получение указателя на последний юнит
		virtual IQueryUnit* GetLastQueryUnit() = 0;

		///очистка группы, включая обнуление переменных и удаление всех юнитов
		virtual void Clear() = 0;
		///удаление группы, вернет указатель на следующюю(справа) группу
		virtual IGroup* Release() = 0;

		///возвращает синтаксические характеристики
		virtual ISyntaxFeature* GetSyntaxFeature() = 0;

		///проверка на пустоту
		virtual bool IsEmpty() = 0;

		///устанавливает идентификатор последовательности в которую входит юнит
		virtual void SetSequenceID(unsigned int uiSequenceID) = 0;
		///возвращает идентификатор последовательности в которую входит юнит
		virtual unsigned int GetSequenceID() = 0;
	};

	///объект, входящий в одну из групп объектов 
	/**представляет объект, входящий в одну из групп объектов (шаблонов ответов, предикатов, сущностей),  
	которые должны присутствовать в текущей трансформации (варианта желаемого ответа)*/
	class IQueryUnit : public CObligation
	{
	public:
		///получение указателя на следующий юнит
		virtual IQueryUnit* GetNextQueryUnit() = 0;
		///получение указателя на предыдущий юнит
		virtual IQueryUnit* GetPrevQueryUnit() = 0;

		///очистка юнита, включая обнуление переменных
		virtual void Clear() = 0;
		///удаление юнита, вернет указатель на следующий(справа) юнит
		virtual IQueryUnit* ReleaseQueryUnit() = 0;

		///добавление коллекции
		virtual void AddQueryUnitIndexCollection(IQueryUnitIndexCollection* pQueryUnitIndexCollection) = 0;
		///получение указателя на первую коллекцию
		virtual IQueryUnitIndexCollection* GetFirstQueryUnitIndexCollection() = 0;
		///получение указателя на последнюю коллекции
		virtual IQueryUnitIndexCollection* GetLastQueryUnitIndexCollection() = 0;

		///получение указателя на коллекцию определенного типа
		virtual IQueryUnitIndexCollection* GetQueryUnitIndexCollection(SS::Interface::Core::BlackBox::EIndexType IndexCollectionType) = 0;

		///проверка на пустоту
		virtual bool IsEmpty() = 0;
		
		///устанавливает идентификатор последовательности в которую входит юнит
		virtual void SetSequenceID(unsigned int uiSequenceID) = 0;
		///возвращает идентификатор последовательности в которую входит юнит
		virtual unsigned int GetSequenceID() = 0;

		///получение стрингового значения слова
		virtual const wchar_t* GetWord() = 0;
		///установление стрингового значения слова
		virtual void SetWord(const wchar_t* szWord) = 0;

		///проверка наличия номера юнита (для модуля релевантности)
		virtual bool IsNumber(int iNumber) = 0;
		///добавление номера юнита
		virtual void AddNumber(int iNumber) = 0;
		///очистка списка номеров
		virtual void ClearNumbers() = 0;
	};

	///коллекция индексов юнита запроса
	class IQueryUnitIndexCollection : public CObligation
	{
	public:
		///получение указателя на следующий индекс
		virtual IQueryUnitIndexCollection* GetNextQueryUnitIndexCollection() = 0;
		///получение указателя на предыдущий индекс
		virtual IQueryUnitIndexCollection* GetPrevQueryUnitIndexCollection() = 0;

		///добавление индекса юнита
		virtual void AddQueryUnitIndex(IQueryUnitIndex* pQueryUnitIndex) = 0;
		///получение указателя на первый индекс юнита
		virtual IQueryUnitIndex* GetFirstQueryUnitIndex() = 0;
		///получение указателя на последний индекс юнита
		virtual IQueryUnitIndex* GetLastQueryUnitIndex() = 0;

		///проверка наличия индекса в коллекции (для оптимизации поиска) - полные индексы
		virtual IQueryUnitIndex* ContainsIndexFull(SS::Dictionary::Types::TDictionaryIndex oIndex) = 0;
		///проверка наличия индекса в коллекции (для оптимизации поиска) - индексы без старшего бита
		virtual IQueryUnitIndex* ContainsIndex(SS::Dictionary::Types::TDictionaryIndex oIndex) = 0;

		///очистка коллекции, включая обнуление переменных и удаление списка индексов
		virtual void Clear() = 0;
		///удаление коллекции, вернет указатель на следующую(справа) коллекцию
		virtual IQueryUnitIndexCollection* Release() = 0;

		///устанавливает тип коллекции
		virtual void SetIndexCollectionType(SS::Interface::Core::BlackBox::EIndexType IndexType) = 0;
		///возвращает тип коллекции
		virtual SS::Interface::Core::BlackBox::EIndexType GetIndexCollectionType() = 0;

		///проверка на пустоту
		virtual bool IsEmpty() = 0;
	};

	///индекс юнита запроса, кроме всего остального содержит вес
	class IQueryUnitIndex : public CObligation
	{
	public:
		///получение указателя на следующий индекс
		virtual IQueryUnitIndex* GetNextQueryUnitIndex() = 0;
		///получение указателя на предыдущий индекс
		virtual IQueryUnitIndex* GetPrevQueryUnitIndex() = 0;

		///устанавливает вес индекса
		virtual void SetIndex(SS::Dictionary::Types::TDictionaryIndex oIndex) = 0;
		///возвращает вес индекса
		virtual SS::Dictionary::Types::TDictionaryIndex& GetIndex() = 0;

		///удаление индекса, вернет указатель на следующий(справа) индекс
		virtual IQueryUnitIndex* Release() = 0;

		/// добавдение словоформы в список
		virtual void AddIDForm(short shForm) = 0;
		/// получение первой словоформы
		virtual bool GetFirstIDForm(short* pshForm) = 0;
		///получение следующей словоформы
		virtual bool GetNextIDForm(short* pshForm) = 0;

		///проверка наличия id form-ы в коллекции (для оптимизации поиска)
		virtual bool ContainsIDForm(short idForm) = 0;
	};

}
}
}
}
}
