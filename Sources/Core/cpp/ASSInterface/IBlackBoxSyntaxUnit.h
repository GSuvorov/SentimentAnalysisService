#pragma once

#include "..\FeatureLibrary\define.h"
#include "IBlackBoxDictionaryIndex.h"

namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace BlackBox
			{
				///инкапсулятор синтаксических характеристик (результата разбора)
				class ISyntaxFeature
				{
				public:
					///устанавливает синтаксическую характеристику
					virtual void SetSyntaxCategory(unsigned int uiSyntaxCategory) = 0;
					///устанавливает синтаксическую характеристику
					virtual void SetSyntaxFeatureApriori(SS::Core::Features::CSyntaxFeatureApriori * pSyntaxFeatureApriori) = 0;
					///устанавливает синтаксическую окончательную характеристику 
					virtual void SetSyntaxFeatureAposteriori(SS::Core::Features::CSyntaxFeatureAposteriori * pSyntaxFeatureAposteriori) = 0;
					///возвращает синтаксическую характеристику
					virtual unsigned int GetSyntaxCategory(void) = 0;
					///возвращает синтаксическую окончательную характеристику 
					virtual SS::Core::Features::CSyntaxFeatureAposteriori * GetSyntaxFeatureAposteriori()=0;
					///возвращает синтаксическую  характеристику 
					virtual SS::Core::Features::CSyntaxFeatureApriori * GetSyntaxFeatureApriori()=0;
					///устанавливает что фича вопрос
					virtual void SetIsQuestion(void) = 0;
					///устанавливает что фича не вопрос
					virtual void SetIsNotQuestion(void) = 0;
					///проверяет является ли фича вопросом
					virtual unsigned int IsQuestion(void) = 0;
				};


				///инкапсулятор ЮНИТА, являющегося составной частью результата синтаксического разбора предложения
				/**в данный момент не используется*/
				class ISyntaxUnit
				{
				public:
					virtual ~ISyntaxUnit(){};
					///получение синтаксических характеристик
					virtual ISyntaxFeature* GetSyntaxFeature() = 0;
					///проверка на вхождение индекса в область индексов юнита
					virtual bool IsUnitIndex(IDictionaryIndex* pDictionaryIndex);
				};

			}
		}
	}
}
