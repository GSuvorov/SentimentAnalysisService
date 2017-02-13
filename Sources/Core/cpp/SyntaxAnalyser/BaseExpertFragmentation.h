//BaseExpertFragmentation.h
#pragma once

#include "BaseExpert.h"
#include "BoardElement.h"

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{

			class CBaseExpertFragmentation : public CBaseExpert
			{
			private:
				///Общие символы-делители предложения
				std::vector<std::wstring> m_CommonDivisors;
				///Левая скобка(круглая)
				std::wstring m_sLeftBracket;
				///Правая скобка(круглая)
				std::wstring m_sRightBracket;
				///Точка с запятой
				std::wstring m_sDivisor;
				///Параметр анализа скобок
				bool m_bBracketAnalyse;
			
			private:
				///Проверяет, является ли данный элемент союзом.
				virtual bool IsConj(BoardElements::BoardElementsIterator elem) const = 0;
			
			protected:
				///Конец предложения
				BoardElements::BoardElementsIterator m_last;
				///Установка параметра анализа скобок
				void SetBracketAnalyse(bool b) { m_bBracketAnalyse = b; };
			
			public:
				//Конструктор
				CBaseExpertFragmentation();
				//Деструктор
				virtual ~CBaseExpertFragmentation(){};
				///Проход по доске
				virtual bool PassBoard(BoardElements::CBoard & rBoard, BoardElements::PBLOCK pBlock);
			};

		}
	}
}