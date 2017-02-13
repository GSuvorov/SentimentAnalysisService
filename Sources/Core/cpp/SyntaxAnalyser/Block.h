#pragma once

#include "BlockVersion.h"
#include "BoardElement.h"
#include "ParsingType.h"
#include "DynamicVersionGenerator.h"

namespace SS
{
	namespace Syntax
	{
		namespace BoardElements
		{
			///Блок элементов доски. Определяет простую часть сложного предложения. 
			class CBlock
			{
			private:
				///ДАННЫЕ ДЛЯ СТАТИЧЕСКОЙ ГЕНЕРАЦИИ ГИПОТЕЗ
				///Список всех гипотез разбора данного блока
				std::list<CBlockVersion> m_Versions;
				///Текущая версия
				std::list<CBlockVersion>::iterator m_itCurrentVersionStatic;
				///Наиболее подходящая версия
				std::list<CBlockVersion>::iterator m_itBestVersionStatic;
				///Коллекция для инициализации итераторов
				std::list<CBlockVersion> m_NullCollection;
				
				///ДАННЫЕ ДЛЯ ДИНАМИЧЕСКОЙ ГЕНЕРАЦИИ ГИПОТЕЗ
				///Лучшая на данный момент гипотеза
				CBlockVersion m_oBestVersionDynamic;
				///Текущая гипотеза
				CBlockVersion m_oCurrentVersionDynamic;
				
				///ОБЩИЕ ДАННЫЕ
				///Счетчик количества сгенерированных гипотез
				unsigned int m_iVersionCount;
				///Добавляемый блок
				CBlock* m_pAddBlock;
				///Границы блока
				BoardElementsIterator m_first, m_last;
				///Текущая гипотеза объединённых блоков
				CBlockVersion m_oCurrVerBothBlock;
				///Лучшая гипотеза разбора объединённых блоков
				CBlockVersion m_oBestVerBothBlock;
				///Обьект динамической генерации гипотез
				CDynamicVersionGenerator m_oDynamicVersionGenerator;
				///Количество экспертов с анализом АТ примененных к данному блоку
				unsigned int m_CountATExpert;
				
			private:
				///Проставляет всем элементам доски новую гипотезу
				void SetVersion(CBlockVersion* pVersion);
				///Реальное добавление блока (работает совместно с AddBlock())
				bool UnionBlocks();
				///Сохранение гипотезы с разбором обоих блоков, которые объединяются
				void SaveVerBoth(CBlockVersion & oVersion);
				///Очистить данные блока
				void ClearData();
											
			public:
				///Конструктор
				CBlock(){};
				///Деструктор
				~CBlock(){};
								
				///МЕТОДЫ ДЛЯ РАБОТЫ СО СТАТИЧЕСКИМИ ГИПОТЕЗАМИ
				///Разбиение на гипотезы
				void CreateVersions();
				///Переключение статических гипотез
				bool SwitchVersion();
				///Определяет и устанавливает наиболее подходящую гипотезу разбора
				void SetBestVersion();
				///Установить текущую статическую гипотезу
				void SetCurrentVersion();
				///Закончился ли проход по гипотезам
				bool VersionsFinish() {	return m_itCurrentVersionStatic == m_Versions.end(); };
				///Выставить гипотезу по номеру
				void SetHypothesis(unsigned int iNumber);
				///Возвращает количество гипотез
				int VersionNumber() { return (int)m_Versions.size(); }
				///Возвращает индекс текущей гипотезы
				int BestVersionIndex()
				{
					if (m_Versions.empty())
					{ return 0;	}
					else
					{
						if (m_itBestVersionStatic != m_Versions.end()) 
						{ return (int)( std::distance(m_Versions.begin(), m_itBestVersionStatic) + 1 );	}
						return 0;
					}
				}
				
				///МЕТОДЫ ДЛЯ РАБОТЫ С ДИНАМИЧЕСКИМИ ГИПОТЕЗАМИ
				///Динамическое создание гипотез
				bool SwitchVersionDynamic(ExpertModel::ParsingType type);
				///Установка лучшей динамической гипотезы
				void SetBestVersionDynamic();
				///Окончательное выставление (динамической)лучшей гипотезы
				void SetVersionFinal();
			
				///ОБЩИЕ МЕТОДЫ
				///Инициализация блока новыми данными
				void Init(BoardElementsIterator first, BoardElementsIterator last);
				///Возвращает итератор, указывающий на первый элемент блока
				BoardElementsIterator GetFirst() { return m_first; };
				///Возвращает итератор, указывающий на позицию после последнего элемента блока
				BoardElementsIterator GetLast()	{ return m_last; };
				///К блоку в начало добавляются элементы другого блока. Добавляемый блок должен быть полностью разобран.
				void AddBlock(CBlock* pBlockToAdd);
				///Возвращает лучшую гипотезу блока на данный момент
				CBlockVersion & GetBestVersion();
				///Проверка блока на пустотность
				bool Empty() const;
				///Проверка принадлежности элемента данному блоку
				bool Contain(BoardElementsIterator itElem) const;
				///Увеличение на 1 количества применившихся экспертов с АТ
				void IncCountATExpert() { ++m_CountATExpert; };
				///Вернуть количество применившихся экспертов с АТ
				unsigned int GetCountATExpert() { return m_CountATExpert; };
			};
		}
	}
}