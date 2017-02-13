#pragma once

#include "BlockVersionElement.h"
#include "BoardCommonTypes.h"
#include "Container.h"
#include "../ASSInterface/Constants.h"

namespace SS
{
	namespace Syntax
	{
		namespace BoardElements
		{
			///Гипотеза разбора блока.
			class CBlockVersion
			{
			private:
				///Хранилище для элементов гипотезы
				CContainer<VERELEMENT, 50/*SS::Constants::MAX_WORDS_IN_SENTENCE*/> m_VersionElements;
				///Характеристики гипотезы
				SBlockVersionCharacteristics m_Characteristic;
				
			private:	
				///Заполнение характеристик
				void FillCharacteristic();
				///Количество головных элементов, выделенных в результате разбора.
				int GetHeadCount();
				///Количество незаполненных в результате разбора элементов
				int GetUnfilledCount();

			public:
				///Возвращает первый элемент гипотезы
				PVERELEMENT GetFirst() const;
				///Возвращает следующий элемента гипотезы
				PVERELEMENT GetNext() const;
				///Добавление элемента гипотезы
				PVERELEMENT AddElement();
				///Оператор сравнения гипотез по числу головных элементов
				bool operator<(CBlockVersion & version);
				///Оператор присваивания.
				CBlockVersion & operator=(const CBlockVersion & Version);
				///Удаление всех элементов гипотезы
				void ClearAll();
				///Заполнение характеристик гипотезы
				const SBlockVersionCharacteristics* GetVersionCharacteristics();
				///Проверка валидности гипотезы
				bool IsValid();
			};

		}
	}
}