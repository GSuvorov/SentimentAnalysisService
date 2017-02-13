#pragma once

#include "..\assinterface\iblackbox.h"


namespace SS
{
	namespace Semantic
	{
		namespace AnalysisExperts
		{
			namespace Question
			{

				///части речи
				enum EPOS {
					eposNon,/// неопределённая
					eposAux,/// вспомогательный глагол
					eposVerbMain,/// основная форма
					eposPrep,/// предлог
					eposToBe,/// форма глагола be
					eposTo,/// предлог to
					eposIn,/// предлог in
					eposFrom,/// предлог from
					eposLong,/// long
					eposQuestion,/// вопросительное слово
					eposDet/// определеитель
				};


				struct SMorphoDescription {
					/// морфологическая характеристика
					SS::Core::Features::CMorphoFeature  _oMorphoFeature;
					/// часть речи
					EPOS _ePOS;
					
					SMorphoDescription(){  _ePOS=eposNon; };
					SMorphoDescription(const SMorphoDescription & oDesc){ this->_ePOS=oDesc._ePOS; this->_oMorphoFeature.Assign(&oDesc._oMorphoFeature); };
				};

				/// организует хранилище морфологических характеристик слова и доступ к нему
				class CQuestionFrameWork
				{
				private:

					typedef std::map<SS::Interface::Core::BlackBox::IUnit*,SS::Core::Features::CMorphoFeature> TUMF;

					typedef std::map<SS::Interface::Core::BlackBox::IUnit*,SS::Core::Features::CSemanticFeature> TUSF;

					typedef std::map<SS::Interface::Core::BlackBox::IUnit*,SMorphoDescription> TUMD;

				public:
					CQuestionFrameWork(void);
					~CQuestionFrameWork(void);

					SS::Core::Features::CMorphoFeature * GetUnitMorphoFeature(SS::Interface::Core::BlackBox::IUnit* pUnit);
					void AddUnitMorphoFeature(SS::Interface::Core::BlackBox::IUnit* pUnit,SS::Core::Features::CMorphoFeature & oMorphoFeature );

					SS::Core::Features::CSemanticFeature * GetUnitSemanticFeature(SS::Interface::Core::BlackBox::IUnit* pUnit);
					void AddUnitSemanticFeature(SS::Interface::Core::BlackBox::IUnit* pUnit,SS::Core::Features::CSemanticFeature & oSemanticFeature );

					SMorphoDescription * GetUnitDescription(SS::Interface::Core::BlackBox::IUnit* pUnit);
					void AddUnitDescription(SS::Interface::Core::BlackBox::IUnit* pUnit,SMorphoDescription & oDescription );

					void Clear();

				private:
					TUMF m_UnitMorphoFeature;
					TUSF m_UnitSemanticFeature;
					TUMD m_UnitDescription;
				};

			}
		}
	}
}