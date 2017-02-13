#pragma once
#include <vector>
#include <list>

namespace SS
{
	namespace Syntax
	{
		namespace BoardElements
		{
			class CBlock;
			class CBoardElement;
			class CBlockVersionElement;
			class CMorphoListElement;
						
			///Контейнер элементов доски
			typedef std::vector<CBoardElement*> BoardCollection;
			///Итератор для контейнера элементов доски
			typedef std::vector<CBoardElement*>::iterator BoardElementsIterator;
			///Указатель на блок
			typedef CBlock* PBLOCK;
			///Единица морфологии
			typedef CMorphoListElement  MORPHO;
			///Указатель на единицу морфологии
			typedef CMorphoListElement* PMORPHO;
			///Элемент гипотезы
			typedef CBlockVersionElement  VERELEMENT;
			///Указатель на элемент гипотезы
			typedef CBlockVersionElement* PVERELEMENT;

			///Размер контейнера морфологии
			#define SIZEMORPHO 24

			///Характеристики гипотезы, влияющие на ее приоритет
			struct SBlockVersionCharacteristics
			{
				///Количество головных элементов
				int m_headCount;
				///Количество элементов, которым не присвоена синтаксическая категория
				int m_unfilledCount;
				///Количество сказуемых
				int m_mainCount;
				///Количество подлежащих
				int m_SFinCount;
				///Количество дополнений к подлежащему
				int m_SComplCount;
				///Головным элементом является сказуемое
				bool m_isMainRoot;
				///Головным элементом является глагол
				bool m_isVerbRoot;
				///Гипотеза содержит подлежащее
				bool m_isSFin;
				///Гипотеза содержит Chain
				bool m_isChain;
				///Гипотеза содержит инфинитив между подлежащим и сказуемым
				bool m_isInfBetween;
				///Гипотеза содержит глагол
				bool m_isVerbMet;
				///Гипотеза содержит дополнение после глагола
				bool m_isAdjAfterVerb;
				///Гипотеза содержит дополнение до глагола, но не в начале гипотезы
				bool m_isAdjBeforeVerb;
				///Гипотеза содержит последовательность из глагола, дополнения и дополнения к подлежащему
				bool m_isVerbAdjSCompl;
				///Гипотеза содержит какой-либо Adjunct
				bool m_isAdjunct;
				///Количество Object в гипотезе
				int m_ObjectCount;
			};
		}
	}
}

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			///Перечисление подгружаемых баз синтаксических конструкций
			enum ESyntaxBase
			{
				sbStandart,
				sbRussianNQ
			};
		}
	}
}