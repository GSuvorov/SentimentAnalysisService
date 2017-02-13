#pragma once
#include "smsentenceexpert.h"

namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace IndexationExperts
			{
				/// выделение придаточных предложений в тексте
				class CSMDependentClause :
					public CSMSentenceExpert
				{
				public:
					CSMDependentClause(void);
					~CSMDependentClause(void);
					bool AnalizeSentence();

					bool Init(IText * pIText);
				private:
					/// список основ
					typedef list<UINT> TSourceList;
					/// основы времени
					TSourceList m_SingleTime;
					/// основы места
					TSourceList m_SinglePlace;
					/// основы причины
					TSourceList m_SingleCause;
					/// основы способа
					TSourceList m_SingleManner;
					/// список составных союзов времени
					list<TSourceList> m_MultiTime;
					/// список составных союзов способа
					list<TSourceList> m_MultiManner;
					/// список составных союзов причины
					list<TSourceList> m_MultiCause;
					/// семантика места 
					list<SS::Dictionary::Types::TDictionaryIndex/*SS::Dictionary::Types::TDictionaryIndex*/> m_SemanticPlace;
					/// семантика времени
					list<SS::Dictionary::Types::TDictionaryIndex/*SS::Dictionary::Types::TDictionaryIndex*/> m_SemanticTime;

					/// возвращает истину - если предложение заканчивается не на вопросительный знак
					bool TestNotQuestion();
					/// истина, если текущее слово - простой союз
					/** 
						\param oSingle - список основ простых союзов
					*/
					bool FindSingleConj(TSourceList & oSingle);
					/// истина, если текущее слово ( и его окружение) - составной союз
					/** 
						\param oMulti - список основ составных союзов
					*/
					bool FindMultiConj(list<TSourceList> & oMulti);
					/// добавление тек слову категории места
					void AddPlaceCategory();
					/// добавление тек слову категории времени
					void AddTimeCategory();
					/// добавление тек слову категории способа
					void AddMannerCategory();
					/// добавление тек слову категории причины
					void AddCauseCategory();
					/// истина, если в индексе есть основа
					/**  
						\param pIndex - индекс 
						\param uiSourceIndex - основа
					*/
					bool IsIndexExist(IIndex * pIndex, UINT uiSourceIndex);

				};

			}
		}
	}
}