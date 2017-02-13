#pragma once

#include "BaseExpert.h"
#include "BoardElement.h"
#include "../ASSInterface/IAMConverter.h"
#include <list>
#include <string>

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			using namespace SS::Core::Features;
			
			///Ёксперт св€зывает однородные члены предложений, наход€щиес€ в разных блоках.
			class CGenericExpertUniformBinderEng : public CBaseExpert
			{
			protected:
				///—лова и знаки препинани€, раздел€ющие однородные члены предложени€
				std::list<std::wstring> m_conjunctions;
				///ƒл€ запоминани€ значени€ фичи и последующего восстановлени€
				CSyntaxFeatureAposteriori m_oSyntaxFeatureAposteriori;
			
			public:
				/// онструктор
				CGenericExpertUniformBinderEng();
				///ѕроход по доске
				virtual bool PassBoard(BoardElements::CBoard & rBoard, BoardElements::PBLOCK pBlock);
				///ѕолучение названи€ эксперта
				virtual std::wstring GetName() const
				{
					return L"Uniform members binder";
				}
			};
			
			///Ёксперт св€зывает однородные члены предложений, наход€щиес€ в разных блоках.
			class CGenericExpertUniformBinderFinalEng : public CGenericExpertUniformBinderEng
			{
			private:
				///¬озвращает true если анализ необходимо проводить только по Noun текущего (правого, второго) блока
				bool AnalisNounOnly(BoardElements::PBLOCK pBlockPrev, BoardElements::BoardElementsIterator itElemSecond) const;
			
			public:
				///ѕроход по доске
				virtual bool PassBoard(BoardElements::CBoard & rBoard, BoardElements::PBLOCK pBlock);
			};
		}
	}
}