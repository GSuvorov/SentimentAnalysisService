//ExpertResumeQuery.h
#pragma once

#include "BaseExpert.h"

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			class CExpertResumeQuery : public CBaseExpert
			{
			private:
				std::vector<std::wstring> m_vConj;
			public:
				//Конструктор/деструктор
				CExpertResumeQuery();
				virtual ~CExpertResumeQuery();
				//Общедоступные методы
				///Производит итоговый анализ запроса, если true то 
				///выполнить разбор заново с разбиением на блоки 
				bool PassBoard(BoardElements::CBoard & rBoard, BoardElements::PBLOCK pCurrBlock);
				///Получить имя конструкции
				std::wstring GetName() const;
			};

		}
	}
}