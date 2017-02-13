//ExpertDetectQuestionQuery.h

#pragma once

#include "BaseExpert.h"

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{

			///Эксперт применяется для определения вопросительного слова, только в "запроса" 
			class CExpertDetectQuestionQuery : public CBaseExpert
			{
			private:
				//Контейнер вопросительных слов
				std::vector<std::wstring> m_vQuestions;
			public:
				//Конструктор
				CExpertDetectQuestionQuery();
				//Деструктор
				virtual ~CExpertDetectQuestionQuery();
				//Общедоступные методы
				///Возвращает true, если в предложении первым или вторым словом является вопрос
				bool PassBoard(BoardElements::CBoard & rBoard, BoardElements::PBLOCK pBlock);
				///Получить имя конструкции
				std::wstring GetName() const;
			};

		}
	}
}