#include "stdafx.h"
#include "BaseExpert.h"

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			//Конструктор
			CBaseExpert::CBaseExpert() : m_isCheckedApplication(false), m_bIsAmbiguity(false), m_isApplicable(false)
			{
			}
			
			//Изменить состояние
			void CBaseExpert::ClearApplicableState()
			{
				m_isCheckedApplication = false;
			}

			///Установить принадлежность к разрешению омонимии
			void CBaseExpert::SetAsAmbiguity()
			{
				m_bIsAmbiguity = true;
			}
		}
	}
}