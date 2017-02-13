#pragma once
#include "SMSentenceExpert.h"
namespace SS
{
	namespace Semantic
	{
		namespace PostAnalysisExperts
		{
			namespace PostSyntaxAnalysis
			{
				/// уточняет категорию обстоятельств
				class CSMSpecifyCategory :
					public CSMSentenceExpert
				{
				public:
					CSMSpecifyCategory(void);
					~CSMSpecifyCategory(void);
					bool AnalizeSentence();
					bool Init(IText * pIText);
				private:
					///семантический тип числа
					SS::Dictionary::Types::TDictionaryIndex m_semNumber;

					SS::Dictionary::Types::TDictionaryIndex m_questSemFirstName;
					SS::Dictionary::Types::TDictionaryIndex m_questSemLastName;

					/// таблица категория_юнит
					std::map<UINT,IUnit*> m_CategoryUnitTable;

					/// специализация обстоятельства 
					void SpecifyAjunct(IUnit * pHead);

					/// определяет допустимость лексемы
					/** 
					\param pUnit - лексема
					\return 	bool - истина - если допустимая
					*/
					bool IsLegalLexem(IUnit* pUnit);

					/// проверяет есть ли категория в списке допустимых  по фиче
					bool IsCategory(SS::Core::Features::CSyntaxFeatureAposteriori * pSyntaxFeatureAposteriori);

					/// проверяет на пустые слова и символы
					bool IsNotEmptyWordsAndSymbols(IUnit* pUnit);
					
					/// выставляет детям категорию Ajunct
					bool MoveToAjunctChilds(IUnit * pGroup);

					/// выставление категории PrepositionObject
					/** 
					\param pUnit - юнит
					\param bPassive - истина, если для разбора пассива
					*/
					bool MoveToPrepositionObject(IUnit * pUnit,bool bPassive);

					/// проверяет есть ли категория ObjectPreposition
					void CheckObjectPreposition(IUnit * pHead, bool bPassive);

					/// выставление категории пассиву
					void ProcessUnit(IUnit * pUnit, IUnit * pHead);

					/// возвращает истину, если данное слово(юнит) предлог
					//bool IsPretext(IUnit * pUnit);

					bool IsPassive(IUnit * pHead);

					/// выставляет Object вопросительной группе (для вопросов who, what)'
					void SetSyntaxQuestion();

					void PassiveProcess();

					void SpecifyCategories();
		
					void SetSyntaxSubjectObject();

					bool CheckSubjectInChilds(IUnit * pHead);
					bool CheckNameAndSetNameSemantic(IUnit * pUnit, IUnit * pQuestion);
					void AddPersonSemantic(IUnit * pUnit);

				};
			}
		}
	}
}