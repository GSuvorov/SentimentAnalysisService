#pragma once

#include "BaseExpertFragmentation.h"
#include "../ASSInterface/IAMConverter.h"
#include <vector>
#include <string>

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			///Вспомогательный эксперт с жёстко прошитым правилом.
			///Эксперт выделяет в предложении блоки. 
			///Перед применением эксперта должен быть применен эксперт, детектирующий союзы.
			class CExpertFragmentationIndexation : public CBaseExpertFragmentation
			{
			private:
				///Части речи, бьющие предложение на блоки. Одиночные союзы.
				std::vector<std::wstring> m_conjSingle;
				///Части речи, бьющие предложение на блоки. Одиночные союзы, стоящие в начале предложения.
				std::vector<std::wstring> m_conjSingleBegin;
				///Части речи, бьющие предложение на блоки. Сложнные союзы.
				std::vector< std::vector< std::wstring > > m_conjMultiple;
				///Части речи, бьющие предложение на блоки. Одиночные местоимения не в начале предложения.
				std::vector<std::wstring> m_pronounSingleNotBegin;
				///Части речи, бьющие предложение на блоки. Одиночные наречия не в начале блока.
				std::vector<std::wstring> m_adverbSingleNotBegin;
				///Одиночная запятая
				std::wstring m_Divisor;
			
			private:
				///Проверяет, является ли данный элемент союзом.
				bool IsConj( BoardElements::BoardElementsIterator elem ) const;
				///Заполнение таблиц частей речи, бьющих на блоки.
				void InitDelim();
				///Распознавание одиночных союзов
				bool IsConjSingle(BoardElements::BoardElementsIterator elem) const;
				///Распознавание одиночных союзов не в начале предложения
				bool IsConjSingleBegin(BoardElements::BoardElementsIterator elem) const;
				///Распознавание сложных союзов
				bool IsConjMultiple(BoardElements::BoardElementsIterator elem) const;
				///Распознавание одиночных местоимений не в начале предложения
				bool IsPronounSingleNotBegin(BoardElements::BoardElementsIterator elem) const;
				///Распознавание одиночных наречий не в начале предложения
				bool IsAdverbSingleNotBegin(BoardElements::BoardElementsIterator elem) const;
			
			public:
				///Конструктор
				CExpertFragmentationIndexation();
				///Получение названия эксперта
				std::wstring GetName() const
				{
					return L"Fragmentation Expert Indexation";
				}
			};
		}
	}
}