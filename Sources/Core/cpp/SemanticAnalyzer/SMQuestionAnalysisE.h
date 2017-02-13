#pragma once
#include "smquestionanalysis.h"
#include "MoAmbResolver.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{
				namespace English
				{

					class CSMQuestionAnalysisE :
						public CSMQuestionAnalysis
					{
					public:
						CSMQuestionAnalysisE(void);
						~CSMQuestionAnalysisE(void);
						bool AnalizeSentence();
						bool Init(IText * pIText);
					private:

						UINT m_uiToBeSourceIndex;

						CMoAmbResolver m_MoAmbResolver;

						/// таблица - слово_тип вопроса
						map<wstring,EQuestionType> m_StringQTypeTableEng;

						EQuestionType DetectQuestionType();
						void MarkUnits(EQuestionType eQuestionType);
						bool IsBe(IUnit * pUnit=NULL);

						void FillGroups();

						/// поиск существительного с семантикой меры + предлог
						/** 
						\param oUnits - список юнитов
						\param oSemanticFeature - семантические типы
						\param l_SemValues - список семантических типов
						\return 	bool - истина, если у существиетльного есть семантика

						удаляет слова перед первым существительным из сущности
						*/
						bool FindMeasurePrep(IUnit * pUnits,SS::Core::Features::CSemanticFeature& oSemanticFeature,list<long>& l_SemValues);
						/// выставелние дефиниций (who or name )
						/** 
						\param pHead - юнит которому надо выставить дефиницию
						\param oUnits - группа по которой выставляется дефиниция
						\param bNameDef - выставлена ли name_def
						\return 	bool - истина, если выставлена name or who def 
						*/
						bool CheckWhoCategory(IUnit * pHead, IUnit * pUnits,bool & bNameDef);
						/// ищет пассив в вопросе и убирает субъекты где надо
						void CorrectPassiveEnglish();

					};


				}
			}
		}
	}
}