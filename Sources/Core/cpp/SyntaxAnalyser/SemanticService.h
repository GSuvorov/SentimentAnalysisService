//SemanticService.h
#pragma once
#include "../ASSInterface/Construction.h"
#include "../ASSInterface/IAMConverter.h"
#include "../ASSInterface/IBlackBox.h"
#include "Container.h"

namespace SS
{
namespace Syntax
{
namespace BoardElements
{
	///Класс выполняет функции по работе с семантикой элемента
	class CSemanticService
	{
	private:
		///Пометка семантического данного для IText
		enum EMark
		{
			///Ничего не делать
			mUndefined,
			///Добавить
			mAdd,
			///Удалить
			mDelete
		};
		///Индекс и его семантика
		struct SSemanticData
		{
			SS::Interface::Core::BlackBox::IIndex* pIndex;
			SS::Core::Features::CSemanticFeature	oSemanticFeature;
			EMark oMark;
			SSemanticData() : pIndex(NULL), oMark(mUndefined){ oSemanticFeature.Undefine(); };
		};
		///Необходимое количество семантики
		enum { SIZESEMANTIC = 10 };
		///Контейнер семантики
		CContainer<SSemanticData, SIZESEMANTIC> m_oSemanticContainer;
		///Обобщенная семантика элемента
		SS::Core::Features::CSemanticFeature m_oSemanticFeature;
		///Юнит которому принадлежит данная семантика
		SS::Interface::Core::BlackBox::IUnit* m_pUnit;
		///Семантический конвертор
		SS::Interface::Core::AMConverter::IAMConverterSemantic* m_pAMConverterSemantic;
		///Для создания дополнительных семантических индексов
		SS::Interface::Core::BlackBox::IBlackBoxTextStorage* m_pBlackBoxTextStorage;
		///Было ли измемнение семантики
		bool m_IsModified;

	private:
		///Конструктор копирования
		CSemanticService(const CSemanticService & v);
		///Удалить
		void Remove(SS::SyntaxConstructions::Types::CConstructionObject* pObject);
		///Выбрать
		void Select(SS::SyntaxConstructions::Types::CConstructionObject* pObject);
		///Назначить
		void Appoint(SS::SyntaxConstructions::Types::CConstructionObject* pObject);
		///Добавить
		void Add(SS::SyntaxConstructions::Types::CConstructionObject* pObject);
		///Унаследовать семантику от заданного элемента
		void Inherit(const CSemanticService* pSemanticServiceBase);
		///Проверка указателей
		bool BadPtr();
		///Сгенерировать общую семантику
		void GenerateSemantic();
		///Проставляем undefine где 0
		void UndefineSemantic();
		///Обнуляем фичу
		void ZeroSemantic();
		///Количество семантики без помеченых на удаление
		unsigned int Size();

	public:
		///Конструктор
		CSemanticService();
		///Оператор=
		void operator=(const CSemanticService & v);
		///Деструктор
		~CSemanticService(){};
		///Применить элемент конструкции к данному юниту
		void ApplyConstructionObject( SS::SyntaxConstructions::Types::CConstructionObject* pObject,
									  const CSemanticService* pSemanticServiceBase = NULL );
		///Применить изменения семантики к BlackBox
		void UpdateBlackBox();
		///Получить семантическую фичу
		SS::Core::Features::CSemanticFeature & GetSemanticFeature();
		///Установить юнит
		void SetUnit(SS::Interface::Core::BlackBox::IUnit* pUnit);
		///Установить семантический конвертер
		void SetSemanticConverter(SS::Interface::Core::AMConverter::IAMConverterSemantic* pAMConverterSemantic);
		///Установить BlackBox фабрику
		void SetTextStorage(SS::Interface::Core::BlackBox::IBlackBoxTextStorage* pBlackBoxTextStorage);
		///Добавить семантику
		void AddSemantic( SS::Interface::Core::BlackBox::IIndex* pIndex,
						  SS::Core::Features::CSemanticFeature*  pSemanticFeature,
						  EMark oMark = mUndefined );
		///Очистить список и данные
		void Clear();
	};

}
}
}