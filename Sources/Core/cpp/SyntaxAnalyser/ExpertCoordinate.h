//ExpertCoordinate.h
#pragma once
#include "BaseExpert.h"

namespace SS
{
namespace Syntax
{
namespace ExpertClause
{
	using namespace ExpertModel;
	///Эксперт для разбора сложносочиненных предложений 
	class CExpertCoordinate : public CBaseExpert
	{
	public:
		///Конструктор
		CExpertCoordinate(){};
		///Деструктор
		~CExpertCoordinate(){};
		///Проход по доске 
		bool PassBoard(CBoard & rBoard, PBLOCK pBlock);
		///Возвращает текстовое название эксперта
		std::wstring GetName() const;
	};
}
}
}