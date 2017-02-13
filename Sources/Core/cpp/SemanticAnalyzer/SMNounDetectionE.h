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
				/// выделение существительных в тексте - с предлогом, и без него 
				class CSMNounDetectionE :
					public CSMSentenceExpert
				{
				public:
					CSMNounDetectionE(void);
					~CSMNounDetectionE(void);
					bool AnalizeSentence();
				private:
					/// поиск глагола перед текущим юнитом для англиского
					/** 
					\param uiDepth - глубина поиска - слов не более
					\param pUnit - юнит перед которым ищется глагол
					*/
					bool IsVerbBeforeE(UINT uiDepth);
					
					/// вычисление глубины поиска глагола
					UINT CalculateDepth(IUnit * pPrep,const UINT uiLimit);
					bool IsOneOf(UINT uiDepth);
				};

			}
		}
	}
}