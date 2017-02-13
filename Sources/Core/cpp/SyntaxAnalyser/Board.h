#pragma once

#include "BoardElement.h"
#include "Block.h"
#include "BoardCommonTypes.h"
#include "../ASSInterface/IBlackBoxItems.h"
#include "../FeatureLibrary/define.h"
#include "../ASCInterface/IInterface.h"
#include "../ASSInterface/ICommon.h"
#include <vector>
#include <list>
#include <set>

namespace SS
{
	namespace Syntax
	{
		namespace BoardElements
		{
			///Доска. Служит внутренним представлением анализируемого предложения.
			class CBoard
			{
			private:
				///Все элементы доски
				std::vector<CBoardElement*>				m_elements;
				///Метка элемента являющегося последним для данного предложения
				std::vector<CBoardElement*>::iterator	m_MarkerLastElem;
				///Хранилище для блоков
				CContainer<CBlock, 10>					m_Blocks;
				///Синтаксическая фича
				SS::Core::Features::CSyntaxFeatureAposteriori m_oSyntaxFeatureAposteriori;
				///язык слова 
				SS::Core::Types::EAnalyseLanguage m_eLang;
				///Параметры анализа
				SS::Interface::Core::MainAnalyse::IAnalyseParams* m_pAnalyseParams;
			
			private:
				///Функция для поиска в дереве первого юнита с установленным SyntaxFlag
				void Init(SS::Interface::Core::BlackBox::IUnit* begin, unsigned int & uiLastPosition);
				///Проверка Unit-а, что бы он не был 256-м
				bool IsGood(SS::Interface::Core::BlackBox::IUnit* pUnit) const;
                /// Проверка на образование цикла
                bool IsCycle(CBoardElement* pElement, CBoardElement* pBorder = 0) const;
				
			public:
				///Конструктор
				CBoard();
				///Деструктор
				~CBoard();
				///Начальное заполнение доски элементами
				void FillBoard(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
				///Инициализация доски предложением из IText - а (возвращает true, если инициалзация прошла успешно)
				bool Initialize(SS::Interface::Core::BlackBox::ISentence* pSentence);
				/// Добавить блок
				PBLOCK AddBlock();
				///Возвращает указатель на первый блок
				PBLOCK GetFirstBlock() const
				{
					return m_Blocks.GetFirst();
				}
				///Возвращает указатель на следующий блок
				PBLOCK GetNextBlock() const
				{
					return m_Blocks.GetNext();
				}
				///Получить предыдущий блок от текущего в процессе перебора
				PBLOCK GetPrevBlock() const;
				///Удаление блоков из интервала, задаваемого итераторами
				void DeleteBlocks();
				///Сделать блок текущим
				PBLOCK SetCurrentBlock(PBLOCK pBlock) const;
				///Возвращает итератор, указывающий на позицию первого элемента доски
				BoardElementsIterator ElementsBegin()
				{
					return m_elements.begin();
				}
				///Возвращает итератор, указывающий на позицию после последнего элемента доски
				BoardElementsIterator ElementsEnd()
				{
					return m_MarkerLastElem;
				}
				///Вычисляет расстояние между элементами, учитывая только головные
				static int HeadDistance(BoardElementsIterator first, BoardElementsIterator last);
				///Создает гипотезы для всех элементов доски
				void CreateVersions();
				///Проставляет синтаксические категории юнитам и строит многоуровневое дерево
				void AssignSyntaxInfoToBlackBoxMultilayer();
				///Количество блоков
				unsigned int CountBlock() const;
				///Установка параметров анализа
				void SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams);
				///Удалить текущий блок
				void DeleteCurrentBlock();
				///Получить язык текста
				SS::Core::Types::EAnalyseLanguage GetLang() { return m_eLang; };
				///Получить родителя данного элемента
				CBoardElement* GetParent(CBoardElement* pElem);
			};
		}
	}
}