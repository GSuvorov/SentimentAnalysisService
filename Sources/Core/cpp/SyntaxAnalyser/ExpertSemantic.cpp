//ExpertSemantic.cpp
#include "StdAfx.h"
#include ".\expertsemantic.h"
#include "../ASCInterface/TDictionaryIndex.h"

namespace SS
{
namespace Syntax
{
namespace ExpertSemantic
{

	///Правило для выделения необходимой нам части из семантики TMetro
	void CExpertSemantic::DetectMetro(SS::Syntax::BoardElements::CBoard & oBoard)
	{
		SS_TRY
		{
			//Пространство фичи
			using namespace SS::Core::Features;
			//Необходимое значение для м.р
			Types::TMetro oMetroM;
			//Необходимое значение для ж.р
			Types::TMetro oMetroF;
			//Заполнение для м.р
			oMetroM.Assign(Values::TMetroType.smtMetroTupik);
			oMetroM.AssignOR(&Values::TMetroType.smtMetroProspekt);
			oMetroM.AssignOR(&Values::TMetroType.smtMetroProezd);
			oMetroM.AssignOR(&Values::TMetroType.smtMetroPereulok);
			oMetroM.AssignOR(&Values::TMetroType.smtMetroBulvar);
			oMetroM.AssignOR(&Values::TMetroType.Undefined);
			//Заполнение для ж.р
			oMetroF.Assign(Values::TMetroType.smtMetroAlleya);
			oMetroF.AssignOR(&Values::TMetroType.smtMetroLiniya);
			oMetroF.AssignOR(&Values::TMetroType.smtMetroNaberezhnaya);
			oMetroF.AssignOR(&Values::TMetroType.smtMetroPloshad);
			oMetroF.AssignOR(&Values::TMetroType.smtMetroStreet);
			oMetroF.AssignOR(&Values::TMetroType.Undefined);
			//Результирующее значение
			unsigned int uiValue = 0;

			//Перебор элементов
			SS::Syntax::BoardElements::BoardElementsIterator it(oBoard.ElementsBegin());
			for (; it != oBoard.ElementsEnd(); ++it)
			{
				//Псевдоним для морфо-фичи
				CMorphoFeature & rMorphoFeature = (*it)->GetVerElem()->m_pMorpho->GetMorphoFeature();
				//Псевдоним для семантик-фичи
				CSemanticFeature & rSemanticFeature = (*it)->GetSemanticFeature();
				//Пропускная проверка
				if (rSemanticFeature.m_SPCADMetro.IsUndefined())
				{ continue;	}
				//Проверка рода
				if (rMorphoFeature.m_GenderType.Equal(Values::GenderType.gtMasculine))
				{
					uiValue = rSemanticFeature.m_SPCADMetro.GetValue() & oMetroM.GetValue();
					if (uiValue) 
					{ 
						rSemanticFeature.m_SPCADMetro.SetValue(uiValue);
						(*it)->RemoveUselessSemantic();
					}
				}
				else if (rMorphoFeature.m_GenderType.Equal(Values::GenderType.gtFemale))
				{
					uiValue = rSemanticFeature.m_SPCADMetro.GetValue() & oMetroF.GetValue();
					if (uiValue) 
					{ 
						rSemanticFeature.m_SPCADMetro.SetValue(uiValue);
						(*it)->RemoveUselessSemantic();
					}
				}
			}
		}
		SS_CATCH(L"")
	}
	
	///Удаление значения Undefine из значений семантической фичи
	void CExpertSemantic::DeleteUndefine(SS::Core::Features::CSemanticFeature* pSemFeature)
	{
		SS_TRY
		{
			//Количество фич
			const unsigned int Count = pSemFeature->GetFeatureCount();
			//Значение фичи
			unsigned int iValue = 0;
			//Перебор фич
			for (unsigned int i = 0; i < Count; ++i)
			{
				//Если фича не undefined
				if ( !pSemFeature->GetFeature(i)->IsUndefined() )
				{ 
					//Получить значение
					iValue = pSemFeature->GetFeature(i)->GetValue();
					//Если фича вообще не имеет никакого значения
					if (iValue == 0) 
					{
						//Ставим undefined
						iValue = 1;
					}
					else
					{
						//Убираем undefined. Обнуляем нулевой бит
						iValue = iValue >> 1;
						iValue = iValue << 1;
					}
					//Установить значение
					pSemFeature->GetFeature(i)->SetValue(iValue);
				}
			}
		}
		SS_CATCH(L"")
	}
	
	///Проход по доске
	bool CExpertSemantic::PassBoard(SS::Syntax::BoardElements::CBoard & oBoard)
	{
		SS_TRY
		{
			//Проверка языка
			if (oBoard.GetLang() != SS::Core::Types::ealRussian)
			{ return false; }
			
			//Родитель
			SS::Syntax::BoardElements::CBoardElement* pBoardElementPr = NULL;
			
			//Перебор элементов
			SS::Syntax::BoardElements::BoardElementsIterator it(oBoard.ElementsBegin());
			for (; it != oBoard.ElementsEnd(); ++it)
			{
				pBoardElementPr = oBoard.GetParent(*it);
				if (pBoardElementPr == NULL)
				{ continue; }
				//Берем в парент только общеее
				pBoardElementPr->GetSemanticFeature().AssignAND(&((*it)->GetSemanticFeature()));
				DeleteUndefine(&pBoardElementPr->GetSemanticFeature());
				pBoardElementPr->RemoveUselessSemantic();
			}
			//Правило работы с семантикой Metro
			DetectMetro(oBoard);
		}
		SS_CATCH(L"")
		return true;
	}

}
}
}