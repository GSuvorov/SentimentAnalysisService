//ExpertClause.h
#pragma once
#include "BaseExpert.h"

namespace SS
{
namespace Syntax
{
namespace ExpertClause
{
	using namespace ExpertModel;
	using namespace SS::Core::Features;
	///Ёксперт дл€ разбора предложений имеющих придаточное определительное предложение c where/when/why
	class CExpertClauseRelative : public CBaseExpert
	{
	private:
		///¬озможные слова в начале придаточного предложени€
		std::vector<std::wstring> m_Words;
		///ƒл€ запоминани€ значени€ фичи и последующего восстановлени€
		CSyntaxFeatureAposteriori m_oSyntaxFeatureAposterioriSecondMember;
		///ƒл€ запоминани€ значени€ фичи и последующего восстановлени€
		CSyntaxFeatureAposteriori m_oSyntaxFeatureAposterioriFirstMember;

	public:
		/// онструктор
		CExpertClauseRelative();
		///ƒеструктор
		~CExpertClauseRelative(){};
		///ѕроход по доске 
		bool PassBoard(CBoard & rBoard, PBLOCK pBlock);
		///¬озвращает текстовое название эксперта
		std::wstring GetName() const;
	};

	///Ёксперт дл€ разбора предложений имеющих придаточное обсто€тельственное предложение
	class CExpertClauseAdjunct : public CBaseExpert
	{
	private:
		///¬озможные слова в начале основного или придаточного предложени€
		std::vector<std::wstring> m_Words;
		///ƒл€ запоминани€ значени€ фичи и последующего восстановлени€
		CSyntaxFeatureAposteriori m_oSyntaxFeatureAposteriori;
		///ƒл€ запоминани€ значени€ фичи и последующего восстановлени€
		CSyntaxFeatureAposteriori m_oSyntaxFeatureAposterioriNeed;
		///”казывает на активный блок (тот который содержит вопросительное слово)
		enum EActiveBlock
		{
			eFirst,
			eSecond
		} m_eActiveBlock;
		///–асчитать синтаксическую категорию
		unsigned int CalcSyntaxCategory(BoardElementsIterator itElem);

	public:
		/// онструктор
		CExpertClauseAdjunct();
		///ƒеструктор
		~CExpertClauseAdjunct(){};
		///ѕроход по доске 
		bool PassBoard(CBoard & rBoard, PBLOCK pBlock);
		///¬озвращает текстовое название эксперта
		std::wstring GetName() const;
	};

}
}
}