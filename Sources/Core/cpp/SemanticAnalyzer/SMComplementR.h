#pragma once
#include "smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Russian
			{
				/// выделение в тексте дефиниций
				class CSMComplementR : public CSMSentenceExpert
				{
				public:
					CSMComplementR(void);
					~CSMComplementR(void);
					bool AnalizeSentence();
					bool Init(IText * pIText);
				private:
					/// глагольна€ семантика 
					SS::Dictionary::Types::TDictionaryIndex m_semRepresent;
					/*SS::Dictionary::Types::TDictionaryIndex m_semName;*/
					/// обработка конструкций вида X+ "semabtic=represent,become"/is + Y
					/** 
					\param oSemanticFeature - коллекци€ семантики ’
					\return bool - истина - если конструкци€ имеет место
					*/
					bool ProcessToBeCompliment(SS::Core::Features::CSemanticFeature & oSemanticFeatureFirst,TBoxUnits & l_UnitsFirst,bool bVerbRB);
					bool InitExpert();
					/// определ€ет семантику слова в выше означенной конструкции 
					/** 
					\param l_Units - слова из ’
					\param oSemanticFeature - семантика ’
					*/
					bool PersonBefore(TBoxUnits & l_Units,SS::Core::Features::CSemanticFeature & oSemanticFeature);
					/// детектирование конструкций вида X(person),Y,...
					/** 
					\param pPerson - указатель на ’
					\return bool - истина если конструкци€ есть в тексте
					*/
					bool AfterPersonInComma(IUnit * pPerson);
					/// детектирование конструкций вида   ...,Y, X(person)...
					/** 
					\param pPerson - указатель на ’
					\return bool - истина если конструкци€ есть в тексте
					*/
					bool BeforePersonInComma(IUnit * pPerson);
					/// детектирование конструкций вида   ...Y X(person)...
					/** 
					\param pPerson - указатель на ’
					\return bool - истина если конструкци€ есть в тексте
					*/
					bool BeforePersonSimple(IUnit * pPerson);
					/// поиск конструкций типа X+ "semabtic=represent,become"/is + Y
					void DetectVerbDefinitionConstructions(bool bTire=false);
					/// проверка что юнит содержит слово, написанное латинскими буквами
					bool IsLatinWord(IUnit * pUnit=NULL);
					///проверка наличи€ предлога перед словом 
					/** 
					\param uiDepth - глубина поиска предлога - не более чем слов
					\return 		bool - истина если предлог есть
					*/
					bool IsPrepositionBefore(UINT uiDepth);

					/// проверка согласованности слов по падежу, роду, числу
					/** 
					\param pFirst - первое слово
					\param pSecond - вотрое слово
					\param bCheckGender=false - провер€ть согласованность по роду
					\param bSkipInstrumentalCaseSecond=false - пропускать согласованность по падежу, если второе слово в творительном падеже
					\return 		bool - истина - если слова согласованы
					*/
					bool IsCoordinatedCaseGenderNumber(IUnit * pFirst, IUnit * pSecond,bool bCheckGender=false,bool bSkipInstrumentalCaseSecond=false);
					/// добавл€ет идекс синаксической категории в коллекцию индексов юнитов
					/** 
					\param l_Units - юниты
					\param oSyntaxCategories категори€
					*/
					void AddSyntaxCategoryIndices(TBoxUnits & l_Units, const Types::SyntaxCategories & oSyntaxCategories);

				};
			}
		}
	}
}