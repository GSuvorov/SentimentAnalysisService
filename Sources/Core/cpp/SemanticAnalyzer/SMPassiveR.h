#pragma once
#include "./smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Russian
			{
			/// выделение пассивных конструкций языка
			class CSMPassiveR :
				public CSMSentenceExpert
			{
			public:
				CSMPassiveR(void);
				~CSMPassiveR(void);
				bool AnalizeSentence();
			private:
				//SS::Core::Features::CMorphoFeature m_MorphoFeature;
				/// тип слова для обработки
				enum EWordType
				{
					ewtSkip, /// пропустить
					ewtStop,/// остановиться, прекратить разбор
					ewtProcess /// обработать
				};
				bool InitExpert();
				/// определяет что слово не может быть пропущено
				/** 
				\param pWord - слово
				\return 		bool - истина, если слово тербует остановки
				*/
				bool IsBreakWord(IUnit * pWord);
				/// поиск влево от текущей позиции
				/** 
				\return	bool - истина, если было выставлен субъект/объект
				*/
				bool FindLeft();
				/// поиск вправо от текущей позиции
				/** 
				\return	bool - истина, если было выставлен субъект/объект
				*/
				bool FindRight();
				
				/// истина, если предлог и существительное согласованы по падежу
				/** 
					\param pPretext - предлог
					\param pNoun - существиетльное
					\return 				bool
				*/
				bool IsCoordinatedPrepWithNounByCase(IUnit * pPretext, IUnit * pNoun);
				/// определяет тип слова
				/** 
					\param pWord - слово 
					\return 				EWordType - тип 
				*/
				EWordType GetWordType(IUnit * pWord);
				/// устанавливает индекс слову 
				/** 
					\param pUnit слово 
				*/
				void SetIndex(IUnit * pUnit);

			};
			}
		}
	}
}