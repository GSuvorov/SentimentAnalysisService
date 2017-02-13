#pragma once
//#include "smexpert.h"
#include "smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace IndexationExperts
			{
			
				/// поиск анафоры в тексте, базовый класс
				class CSMExAnaphora :
					public CSMSentenceExpert//CSMExpert
				{
				public:
					CSMExAnaphora(void);
					~CSMExAnaphora(void);
					
					//bool Analize();
					bool AnalizeSentence();
					
					bool Init(IText * pIText);
				protected:
					/// целевые слова - то что ищем в тексе  ( личные местоимения ) 
					set<wstring> m_AimWords;
					///поиск и замена морфологической информации местоимения на соответсятвующую ему морфологию имени собственного
					/** 
					\param uiWordCount - кол-во допустимых слов
					\param oInfo - морфология на которую заменять
					*/
					void ReplaceProNoun(UINT & uiWordCount, IUnit * poInfo, const TBoxUnits l_units);
					/// определяет что слово подходит под замену
					/** 
					\param sWord - слово 
					\return 		bool - истина, если подходит
					*/
					bool IsAimWord(const TCHAR * sWord);

					/// определяет что текущее слово - имя или агент
					virtual bool IsNameAgent()=0;

					/// копирует все индексы текущему юниту
					void CopyUnit(IUnit * pUnit, bool bMorphoOnly);

					/// собирает детей, если имена или деятели
					void CollectListOfNameAgents(TBoxUnits & l_Units);

					/// определяет, что юнит имеет семантику имени
					bool IsSemanticName(IUnit * pUnit);
					
					/// определяет на соответствие рода текущего агента и местоимения
					bool IsValidGender(IUnit * pAgent);
				};
			}
		}
	}
}

