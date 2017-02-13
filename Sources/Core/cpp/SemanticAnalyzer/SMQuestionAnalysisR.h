#pragma once
#include "smquestionanalysis.h"
#include "QuestionBase.h"


namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{
				namespace Russian
				{


					/// разбор вопроса для русского языка
					class CSMQuestionAnalysisR :
						public CSMQuestionAnalysis
					{
					public:
						CSMQuestionAnalysisR(void);
						~CSMQuestionAnalysisR(void);
						bool AnalizeSentence();
						bool Init(IText * pIText);
						bool Init(CSMManager * pManager, SDescriptionInfo & DInfo);
						virtual bool AnalyseQuestion(EQuestionType eQType);
					protected:
						/// таблица индексов основ_тип вопроса
						map<UINT,EQuestionType> m_SourceTypeTable;
						/// слова для удаления по индексам основ
						set<UINT> m_s_WordToRemove;
						/// глаголы для удаления по индексам основ
						set<UINT> m_s_VerbToRemove;
						/// слова - признаки сложных вопросительных
						set<UINT> m_s_RusComplexQ;

						CSMQuestionAnalysisR * m_pRQKogda;
						CSMQuestionAnalysisR * m_pRQChto;
						CSMQuestionAnalysisR * m_pRQKakoi;
						CSMQuestionAnalysisR * m_pRQKto;
						CSMQuestionAnalysisR * m_pRQSkolko;

						/// выставелние дефиниций (who or name )
						/** 
						\param pHead - юнит которому надо выставить дефиницию
						\param oUnits - группа по которой выставляется дефиниция
						\param bNameDef - выставлена ли name_def
						\return 	bool - истина, если выставлена name or who def 
						*/
						bool CheckWhoCategory(IUnit * pHead, IUnit * pUnits,bool & bNameDef);
						/// ОПРЕДЕЛЯЕТ ТИП ВОПРОСА  для русского
						/** 
						\param tDescription - описание предложения
						\param CExQuestionParser::TMorphoDescription::iterator - итератор по описанию предложения. уже проиницилизирован !
						\return 	CExQuestionParser::EQuestionType - тип вопроса
						*/
						EQuestionType DetectQuestionTypeRussian();
						void MarkUnits(EQuestionType eQuestionType);
						void FillGroups();

						bool IsMorphologyCoordinated(IUnit * pU1, IUnit * pU2 , bool bCase, bool bGender , bool bNumber , bool bScipInstrumentalCase=false );

						bool CheckSubjectFin(IUnit * pHead);

						Types::SyntaxCategories LegacyCategoryToEntity(IUnit * pUnit, IUnit * pVerb);

						void MoveToAjunct(IUnit * pGroup);

						/// устанавливает результат анализа вопроса - в eareGlobalKtoChtoWhoWhatQuestion, для вопросов с "определениями"
						void SetDefQuestion() 
						{ 
							if(GetAnalyseParams())
							{
								if((GetAnalyseParams()->GetAnalyseMod()==SS::Core::Types::eamQuery)&&
									(GetAnalyseParams()->GetCurrentSystemMode() == SS::Core::Types::ecsmGlobal))
									GetAnalyseParams()->SetAnalyseResultEx(SS::Core::Types::IndexationSearch::TSearchParams::saEncyclopedia);
							}else
							{
								SAVE_LOG(SS_ERROR AND L"GetAnalyseParams() == NULL" AND __WFUNCTION__);
							}
						} ;

					};


				}
			}
		}
	}
}