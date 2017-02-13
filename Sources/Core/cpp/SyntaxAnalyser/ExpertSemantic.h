//ExpertSemantic.h
#pragma once
#include "Board.h"

namespace SS
{
namespace Syntax
{
namespace ExpertSemantic
{

	///Эксперт для анализа семантики
	class CExpertSemantic
	{
	private:
		///Удаление значения Undefine из значений семантической фичи
		void DeleteUndefine(SS::Core::Features::CSemanticFeature* pSemFeature);
		///Правило для выделения необходимой нам части из семантики TMetro
		void DetectMetro(SS::Syntax::BoardElements::CBoard & oBoard);

	public:
		///Конструктор
		CExpertSemantic(){};
		///Деструктор
		~CExpertSemantic(){};
		///Проход по доске
		bool PassBoard(SS::Syntax::BoardElements::CBoard & oBoard);
	};

}
}
}


