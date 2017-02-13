#pragma once
#include ".\smmanager.h"
#include "SMSentenceExpert.h"
namespace SS
{
	namespace Semantic
	{
		namespace PostAnalysisExperts
		{
			namespace QueryGeneration
			{
				/// генерирует строку запроса поисковой системе
				class CSMQueryGenerator :
					public CSMSentenceExpert
				{
				public:
					CSMQueryGenerator(void);
					~CSMQueryGenerator(void);
					bool AnalizeSentence();
					bool Init(IText * pIText);
					/// возващает строку запроса коллектора
					wstring GetResultQuery(){ return m_sResultQuery; };
				private:
					/// строка с запросом коллектора
					wstring m_sResultQuery;
					SS::Interface::Core::Dictionary::IMorphologyAnalyser * m_pDictionaryInterface;
					/// формы глагола to be
					set<wstring> m_ToBeForms;
					/// стоп слова
					set<wstring> m_StopWords;

					/// вопрос - в строку коллектора
					void ToString();
					/// проверяет что слово - стоп слово
					bool IsStopWord(const TCHAR * sWord);
					//void QuestionProcess(IUnit * pUnit);
					void FillWord(IUnit* pUnit, wstring & wWord);
					bool IsRusDef(IUnit* pUnit);
					///Формирование строки запроса
					void ToStringAll();
					///Проверка юнита что он вопросительное слово
					bool IsQuestion(IUnit* pUnit);

				};
			}
		}
	}
}