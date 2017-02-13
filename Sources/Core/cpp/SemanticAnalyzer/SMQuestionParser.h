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
			/// обработка синтаксического разбора
			class CSMQuestionParser :
				public CSMSentenceExpert
			{
			public:
				CSMQuestionParser(void);
				~CSMQuestionParser(void);

				bool AnalizeSentence();
				bool Init(IText * pIText);

			private:
				/// стоп слова
				set<wstring> m_StopWords;
				/// таблица категория_юнит
				typedef map<UINT,IUnit*> TCategoryUnitTable;
				/// тип вопроса
				enum EQuestionType
				{
					eqtNon,
					eqtWhat,
					eqtWhen,
					eqtWhere,
					eqtHow,
					eqtWho,
					eqtWhy
				};
				
				///семантический тип числа
				SS::Dictionary::Types::TDictionaryIndex m_questSemNumber;
				///семантический тип имени
				SS::Dictionary::Types::TDictionaryIndex m_questSemFirstName;
				///семантический тип 2-го имени
				SS::Dictionary::Types::TDictionaryIndex m_questSemLastName;
				///семантический тип национальности
				SS::Dictionary::Types::TDictionaryIndex m_questSemNationality;
				///семантический тип бикам
				//SS::Dictionary::Types::TDictionaryIndex m_questSemBecome;*/
				/// таблица вопросительное слово_тип вопроса
				map<wstring,CSMQuestionParser::EQuestionType> m_QuestionTypeEng;
				/// лексемы соответствующие формам глагола to be
				set<wstring> m_ToBeLiteral;
				/// таблица категория_юнит
				TCategoryUnitTable m_CategoryUnitTable;

				/// сценарий обработки вопроса английского языка
				/** 
					\param pSentence - предложение с вопросом
					\return 			bool - истина, если вопрос обработан корректно
				*/
				bool SelectQuestionEnglish(ISentence * pSentence);
				/// обработка предложния
				/** 
					\param pSentence - предложение 
				*/
				void ProcessSentence(ISentence * pSentence);
				/// определение категории дефиниции 
				/** 
					\param pHead - юнит, которому проставлять категорию
					\param pGroup - группа на основании которой категория проставляется
					\return bool - истина, если категория проставлена
				*/
				bool CheckWhoCategory(IUnit * pHead, IUnit * pGroup);

				bool CheckWhoCategoryProcess(IUnit * pHead, IUnit * pUnit, IUnit * pGroup=NULL);

				/// определение категории дефиниции 
				/** 
				\param pHead - юнит, которому проставлять категорию
				\param pGroup - группа на основании которой категория проставляется
				\return bool - истина, если категория проставлена
				*/
				bool CheckWhatCategory(IUnit * pHead, IUnit * pGroup);

				bool CheckWhatCategoryProcess(IUnit * pHead, IUnit * pUnit, IUnit * pGroup=NULL);

				/// убирает семантические индексы из группы
				/** 
					\param pGroup - группа
				*/
				void PopSemantic(IUnit * pGroup);

				/// обработка обстаятельств меры
				/** 
					\param pSentence - предложение
					\param pGroup - группа обстоятельства меры
					\param eQType - тип воропса
				*/
				void AdjunctMeasureProcess(ISentence * pSentence, IUnit * pGroup,CSMQuestionParser::EQuestionType eQType);
				/// обработка категории субъекта
				/** 
					\param pSentence - предложение
					\param pGroup - группа
					\param eQType - тип вопроса
				*/
				void SubjectFinProcess(ISentence * pSentence,IUnit * pGroup,CSMQuestionParser::EQuestionType eQType);
				/// обработка категории SubjectComplement
				/** 
				\param pGroup - группа
				\param eQType - тип вопроса
				*/
				void SubjectComplementProcess(IUnit * pGroup,CSMQuestionParser::EQuestionType eQType);
				/// обработка категории объекта
				/** 
				\param pSentence - предложение
				\param pGroup - группа
				\param eQType - тип вопроса
				*/
				void ObjectProcess(ISentence * pSentence, IUnit * pGroup,CSMQuestionParser::EQuestionType eQType);
				/// обработка категории AjunctTimeDuration
				/** 
				\param pSentence - предложение
				\param pGroup - группа
				\param eQType - тип вопроса
				*/
				void AjunctTimeDurationProcess(ISentence * pSentence,IUnit * pGroup,CSMQuestionParser::EQuestionType eQType);
				/// обработка категории Ajunct 
				/**
				\param pSentence - предложение
				\param pGroup - группа
				\param eQType - тип вопроса
				*/
				void AjunctProcess(ISentence * pSentence, IUnit * pGroup,CSMQuestionParser::EQuestionType eQType);

				/// обработка запроса по ключевым словам
				bool KeyQuestionProcess();
				/// поиск и вынесение предлога из группы
				void FindAndMovePretext(ISentence * pSentence,IUnit * pGroup);

				void MoveToAgjunct(ISentence * pSentence,IUnit * pUnit);

				bool CheckManyMuch(IUnit * pGroup);

				bool CheckAimSemSyntAndMoveToAjunct(ISentence * pSentence,IUnit * pGroup, const Types::SyntaxCategories & oCategory, CSMQuestionParser::EQuestionType eQType);

				/// добавляет семантический индекс юниту 
				/** 
				\param pUnit - юнит
				\param oSemType - семантический тип
				*/
				void AddSemanticIndex(IUnit * pUnit, SS::Dictionary::Types::TDictionaryIndex & oSemType);
				/// добавляет юниту все семантические индексы соответствующие семантике времени
				void AddAllTimeIntervalSemantic(IUnit * pUnit);
				/// добавляет юниту все семантические индексы соответствующие семантике места
				void AddAllPlaceSemantic(IUnit * pUnit);
				/// добавляет юниту все семантические индексы соответствующие семантике персоны
				void AddAllPersonSemantic(IUnit * pUnit);
				/// добавляет юниту все семантические индексы соответствующие семантике персоны, но только имена!
				void AddAllPersonNameSemantic(IUnit * pUnit);
				/// поддержка надстройки по ключевым словам для вопроса what
				/// возвращает true, если в группе была семантика Person
				bool SuperstructureSupport(IUnit * pGroup);
				/// возвращает истину, есл иданное слово(юнит) существительное
				bool IsNoun(IUnit * pUnit);

				bool IsSemanticUndef(SS::Core::Features::CSemanticFeature oSemanticFeature, bool & bPersonUndef, bool & bMeasureUndef);

				void SetDefQuestion() 
				{ 
					if(GetAnalyseParams())
					{
						if((GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamQuery) &&
							(GetAnalyseParams()->GetCurrentSystemMode() == SS::Core::Types::ecsmGlobal))
							GetAnalyseParams()->SetAnalyseResultEx(SS::Core::Types::IndexationSearch::TSearchParams::saEncyclopedia);
					}else
					{
						SAVE_LOG(SS_ERROR AND L"GetAnalyseParams() == NULL" AND __WFUNCTION__);
					}
					//if (m_pBlackboard->GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamRTFQuery)
					//	m_pBlackboard->GetAnalyseParams()->SetAnalyseResultEx(SS::Core::Types::eareGlobalKtoChtoWhoWhatQuestion);	
				} ;
				bool IsFindCall(IUnit * pUnit);
				bool IsFindMainDie();
				void CorrectFuncForPersonServiceSem(IUnit * pHead, IUnit * pUnit,SS::Core::Features::CSemanticFeature oSemanticFeature);

			};
			}
		}
	}
}