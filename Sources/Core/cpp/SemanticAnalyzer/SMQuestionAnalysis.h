#pragma once

#include "./smsentenceexpert.h"

#include ".\questionframework.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{

				/// тип воропса по вопросительному слову
				enum EQuestionType
				{
					eqtNon,
					// English
					eqtEWhat,
					eqtEWhen,
					eqtEWhere,
					eqtEHow,
					eqtEWho,
					eqtEWhy,
					//Russian
					eqtRKto,
					eqtRChto,
					eqtRKogda,
					eqtRKuda,
					eqtROtkuda,
					eqtRGde,
					eqtRKakoi,
					eqtRKakova , 
					eqtRSkolko , 
					eqtRKak,
					eqtRKakManner,
					eqtRPochemu,
					eqtROther
				};



				/// общие механизмы анализа вопроса
				class CSMQuestionAnalysis :
					public CSMSentenceExpert
				{
				public:
					CSMQuestionAnalysis(void);
					~CSMQuestionAnalysis(void);
					bool Init(IText * pIText);

					friend class CQuestionBase;

				protected:
					CQuestionFrameWork m_Frame;

					SS::Dictionary::Types::TDictionaryIndex m_semNumber;
					SS::Dictionary::Types::TDictionaryIndex m_semName;
					SS::Dictionary::Types::TDictionaryIndex m_semBecome;
					SS::Dictionary::Types::TDictionaryIndex m_semVPossess;
					SS::Dictionary::Types::TDictionaryIndex m_semVBe;
					SS::Dictionary::Types::TDictionaryIndex m_semLastName;
					SS::Dictionary::Types::TDictionaryIndex m_semFirstName;
					/// описание вопроса
					struct SDescriptionInfo {
						/// указатель на вспомогательный глагол
						IUnit* _pAux;
						/// указатель на глагол tо be
						IUnit* _pToBe;
						/// указатель на  глагол
						IUnit* _pVerb;
						/// указатель на предлог
						IUnit* _pPrep;
						IUnit* _pQuestion;
						IUnit* _pMainEntity;
						IUnit* _pAdditionalEntity;
						IUnit* _pPredicate;
						/// инициализация
						void Init(){ _pPrep=NULL; _pAux=NULL; _pVerb=NULL; _pToBe=NULL; _pAdditionalEntity=_pMainEntity=_pQuestion=_pPredicate=NULL; };
						SDescriptionInfo(){ Init(); };
					} m_DInfo;/// 1 переменная этого типа


					SMorphoDescription * GetUnitDescription(SS::Interface::Core::BlackBox::IUnit* pUnit=NULL);
					void AddUnitDescription(SS::Interface::Core::BlackBox::IUnit* pUnit,SMorphoDescription & oDescription );


					/// определяет допустимость лексемы
					/** 
					\param pUnit - лексема
					\return 	bool - истина - если допустимая
					*/
					bool IsLegalLexem(IUnit* pUnit);

					/// обработка текущего предложения
					void ProcessSentence();

					/// добавление соответствующей семантики юниту
					void AddAllPlaceSemantic(IUnit * pHead);
					void AddAllOrganisationSemantic(IUnit * pHead);
					void AddAllTimeIntervalSemantic(IUnit * pHead);
					void AddAllPersonSemantic(IUnit * pHead, bool bCountry=true);
					void AddAllPersonSemanticInAnimate(IUnit * pHead);
					void AddAcroCitationSemantic(IUnit * pHead);

					/// устанавливает категорию в зависимости от предлога и типа обстоятельства
					/** 
					\param pHead - юнит которому проставить категорию
					\param pPrep - юнит с предлогом
					\param bPlace - истина, обстоятельство места; ложь, обстоятельство времени
					*/
					void SetSyntaxFiltered(IUnit* pHead,IUnit* pPrep,bool bPlace);

					/// добавляет семантический тип юниту
					void AddToBeIndex(IUnit* pHead,Types::TSVBeType tsType);
					/// добавляет семантический тип юниту
					void SetToBeIndex(IUnit* pHead,Types::TSVBeType tsType);

					/// обработка семантики глагола
					/** 
					\param pVerb - глагол
					\param oSemanticFeature - возвращаемая семантика
					\param l_SemValues - возвращаемая семантика
					\return 	bool - если есть семантика

					обрабатывается семантика - стоить, весить
					*/
					bool ProcessVerbSemantic(IUnit* pVerb,SS::Core::Features::CSemanticFeature& oSemanticFeature,list<SS::Dictionary::Types::TDictionaryIndex>& l_SemValues);

					/// выставелние дефиниций (who or name )
					/** 
					\param pHead - юнит которому надо выставить дефиницию
					\param oUnits - группа по которой выставляется дефиниция
					\param bNameDef - выставлена ли name_def
					\return 	bool - истина, если выставлена name or who def 
					*/
					bool CheckWhoCategory(IUnit * pHead, IUnit * pUnits,bool & bNameDef);
					/// указанный юнит обретает категорию adjunct 
					/** 
					\param pUnit - юнит 
					*/
					void SetSingleSyntax(IUnit* pUnit);

					/// устанавливает результат анализа вопроса - в eareGlobalKtoChtoWhoWhatQuestion, для вопросов с "определениями"
					/*void SetDefQuestion() 
					{ 
						//if (m_pBlackboard->GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamRTFQuery)
						//	m_pBlackboard->GetAnalyseParams()->SetAnalyseResultEx(SS::Core::Types::eareGlobalKtoChtoWhoWhatQuestion);	
					} ;*/

					void StrictQuery();

					void SimpleQuery();

					void MoveUnitToPredicate(IUnit * pUnit, IUnit * pAfter);



				};


			}
		}
	}
}