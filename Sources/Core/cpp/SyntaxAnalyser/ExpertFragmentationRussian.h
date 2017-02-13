//ExpertFragmentationRussian.h
#pragma once

#include "BaseExpertFragmentation.h"
#include <vector>

namespace SS
{
namespace Syntax
{
namespace ExpertModel
{
	///Осуществляет деление на блоки в русском языке
	class CExpertFragmentationRusInd : public CBaseExpertFragmentation
	{
	private:
		///Одиночный делитель на блоки(запятая)
		std::wstring m_Divisor;
		///Символы деления предложения на блоки
		std::vector<std::wstring> m_Symbols;
		///Наречия деления предложения наблоки
		std::vector<std::wstring> m_Adverbs;

	private:
		///Проверяет наличие элемента деления на блоки
		bool IsConj(BoardElements::BoardElementsIterator elem) const;
		///Проверка наличия символа
		bool IsSymbol(BoardElements::BoardElementsIterator elem) const;
		///Проверка наличия связки символов
		bool IsMultiSimbol(BoardElements::BoardElementsIterator elem) const;
		///Проверка наличия наречия
		bool IsAdverb(BoardElements::BoardElementsIterator elem) const;

	public:
		///Конструктор
		CExpertFragmentationRusInd();
		///Деструктор
		virtual ~CExpertFragmentationRusInd(){};
		///Возвращает текстовое название эксперта 
		std::wstring GetName() const;
	};

}
}
}