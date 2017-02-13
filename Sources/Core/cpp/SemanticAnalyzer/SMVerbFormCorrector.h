#pragma once
#include "SMSentenceExpert.h"
#include ".\smmanager.h"

namespace SS
{
	namespace Semantic
	{
		namespace PostAnalysisExperts
		{
			namespace QueryGeneration
			{
				/// коррекция формы глагола исходя из времени вспомагательного глагола
				class CSMVerbFormCorrector :
					public CSMSentenceExpert
				{
				public:
					CSMVerbFormCorrector(void);
					~CSMVerbFormCorrector(void);
					bool AnalizeSentence();
					bool Init(IText * pIText);
				private:
					/// лексемы соответствующие форам глагола to be
					set<wstring> m_ToBeForms;
					/// интерфейс к словарю
					SS::Interface::Core::Dictionary::IMorphologyAnalyser * m_pDictionaryInterface;
					
					/// коррекция формы глагола
					void CorrectForm(IUnit * pUnit, IUnit * pAux, bool bPast);

					bool IsVerb(IUnit * pUnit);

				};
			}

		}
	}
}