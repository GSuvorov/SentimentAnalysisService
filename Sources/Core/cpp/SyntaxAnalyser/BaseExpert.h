#pragma once

#include "Board.h"
#include <atlbase.h>
#include <atldbcli.h>

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			using namespace BoardElements;
			
			/// Базовый класс для всех экспертов
			class CBaseExpert
			{
			public:
				///Конструктор
				CBaseExpert();
				///Деструктор
				virtual ~CBaseExpert(){};
				/// Проход по доске 
				virtual bool PassBoard(CBoard & board, PBLOCK pBlock) = 0;
				/// Возвращает текстовое название эксперта. 
				virtual std::wstring GetName() const = 0;
				///Устанавливает флаг применимости эксперта
				void ClearApplicableState();
				///Установить принадлежность к разрешению омонимии
				void SetAsAmbiguity();
			
			protected:
				///Показывает, применим ли эксперт
				bool m_isApplicable;
				///Показывает, проверен ли эксперт на применимость
				bool m_isCheckedApplication;
				///Принадлежность к разрешению омонимии
				bool m_bIsAmbiguity;
			};
		}
	}
}