#pragma once
#include "./smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace English
			{

				/// выделение дефиниций
				class CSMComplementE :
					public CSMSentenceExpert
				{
				public:
					CSMComplementE(void);
					~CSMComplementE(void);
					bool AnalizeSentence();
					bool Init(IText * pIText);

				private:
					SS::Dictionary::Types::TDictionaryIndex m_semName;
					/// обработка конструкций вида X+ "semabtic=represent,become"/is + Y
					/** 
					\param oSemanticFeature - коллекция семантики Х
					\return bool - истина - если конструкция имеет место
					*/
					bool ProcessToBeCompliment(SS::Core::Features::CSemanticFeature & oSemanticFeatureFirst,TBoxUnits & l_UnitsFirst);
					/// определяет семантику слова в выше означенной конструкции 
					/** 
					\param l_Units - слова из Х
					\param oSemanticFeature - семантика Х
					*/
					bool PersonBefore(TBoxUnits & l_Units,SS::Core::Features::CSemanticFeature & oSemanticFeature);
					/// детектирование конструкций вида X(person),Y,...
					/** 
					\param pPerson - указатель на Х
					\return bool - истина если конструкция есть в тексте
					*/
					bool AfterPersonInComma(IUnit * pPerson);
					/// детектирование конструкций вида   ...,Y, X(person)...
					/** 
					\param pPerson - указатель на Х
					\return bool - истина если конструкция есть в тексте
					*/
					bool BeforePersonInComma(IUnit * pPerson);
					/// детектирование конструкций вида   ...Y X(person)...
					/** 
					\param pPerson - указатель на Х
					\return bool - истина если конструкция есть в тексте
					*/
					bool BeforePersonSimple(IUnit * pPerson);
					/// поиск конструкций типа X+ "semabtic=represent,become"/is + Y
					void DetectVerbDefinitionConstructions();
					/// добавляет идекс синаксической категории в коллекцию индексов юнитов
					/** 
					\param l_Units - юниты
					\param oSyntaxCategories категория
					*/
					void AddSyntaxCategoryIndices(TBoxUnits & l_Units, const Types::SyntaxCategories & oSyntaxCategories);
					/// пропускает скобки и текст в скобках
					void SkipBrackets();


				};


			}
		}
	}
}