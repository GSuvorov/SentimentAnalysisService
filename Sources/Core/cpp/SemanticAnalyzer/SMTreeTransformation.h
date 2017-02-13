#pragma once
#include "SMSentenceExpert.h"
#include "../ASSInterface/ISemanticAnalyze.h"

namespace SS
{
	namespace Semantic
	{
		namespace PostAnalysisExperts
		{
			namespace PostSyntaxAnalysis
			{
				/// трансфорация иерархического дерева синтаксического разбора в линейное с учётом уровня иерархии
				class CSMTreeTransformation :
					public CSMSentenceExpert
				{
				public:
					CSMTreeTransformation(void);
					~CSMTreeTransformation(void);
					bool AnalizeSentence();
					bool Init(IText * pIText);
				private:
					set<wstring> m_Words;
					set<wstring> m_OtherWords;
					float m_fWeight;
					UINT m_uiRrecursionDepth;
					UINT m_uiLevel;
					bool m_bIsFirstLastName;
					//enum PositionOfHead {smaller,greater};
					//unsigned m_PositionOfHead: 1;
					//unsigned m_RemoveAll: 1;
					//unsigned m_CombineLevAndPos: 1;
					//PositionOfHead GetPositionOfHead() {if(m_PositionOfHead) return greater; else return smaller;};
					bool ProcessUnit(IUnit * pUnit, list<IUnit*> & l_Units, const Types::SyntaxCategories & oSyntaxCategories, UINT uiLevel);
					bool IsContainRejectUnimportant(IUnit* pChild, /*IUnit * pHead ,*/UINT uiLevel);
					bool DetectQuestWords(IUnit* pUnit);
					bool IsWord(IUnit * pUnit, bool bQWords);
					bool IsFirstLastNameSemantic();
					void SetWeightFirstLastName(IUnit * pUnit);
					bool FillUnitsInQuotationsList(IUnit* pUnit);
					bool IsSetWeightFirstLastNameChilds(IUnit * pUnit);
					void SetWeightNoFirstLastName(IUnit * pUnit);
					void FillUnits(IUnit* pUnit);
				};

				bool IUnitPtrLess(IUnit * pLeft,IUnit * pRight);
			}
		}
	}
}