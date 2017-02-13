#include "stdafx.h"
#include "Board.h"
#include "../ASSInterface/IBlackBoxUnit.h"
#include "../ASSInterface/Constants.h"
#include "../ASSInterface/TDictionaryNames.h"

using namespace std;
using namespace SS::Interface::Core::BlackBox;
using namespace SS::Core::Features;
using namespace SS::Constants;

namespace SS
{
namespace Syntax
{
namespace BoardElements
{
	//Конструктор
	CBoard::CBoard() : m_pAnalyseParams(NULL), m_eLang(SS::Core::Types::EAnalyseLanguage::ealUndefined)
	{
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories = Values::SyntaxCategories.scSubjectFin;
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scMain);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunct);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scObject);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scSubjectComplement);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctTimeSource);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctTimeFixedPoint);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctTimeEnd);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctTimeDuration);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctPlaceSource);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctPlaceFixedPoint);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctPlaceEnd);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctGoal);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctCause);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctMeasure);
		m_oSyntaxFeatureAposteriori.m_SyntaxCategories.AssignOR(&Values::SyntaxCategories.scAjunctManner);
	}

	//Начальное заполнение доски элементами
	void CBoard::FillBoard(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
	{
		m_elements.reserve(MAX_WORDS_IN_SENTENCE);

		for (int i = 0; i < MAX_WORDS_IN_SENTENCE; i++)
		{
			CBoardElement* pBoardElement = new CBoardElement(pLoadManager);
			m_elements.push_back(pBoardElement);
		}
		m_MarkerLastElem = m_elements.begin();
	}

	//Инициализация доски предложением из IText - а (возвращает true, если инициалзация прошла успешно)
	bool CBoard::Initialize(ISentence* pSentence)
	{
		SS_TRY
		{
			//Установка языка	
			m_eLang = pSentence->GetLanguage();
			//Очистка хранилища блоков
			m_Blocks.Clear();
			//Установка маркера на первый элемент
			m_MarkerLastElem = m_elements.begin();
			//Позиция в предложении
			unsigned int uiPosition = 0;
			//Инициализация текущим предложением
			Init(pSentence->GetFirstUnit(), uiPosition);
			if ( ElementsBegin() != ElementsEnd() ) 
			{
				m_Blocks.Add()->Init( ElementsBegin(), ElementsEnd() );
				return true;
			}
			return false;
		}
		SS_CATCH(L"")
	}

	///Деструктор
	CBoard::~CBoard()
	{
		if (!m_elements.empty())
		{
			BoardElementsIterator Iter;
			for (Iter = m_elements.begin(); Iter != m_elements.end(); Iter++)
			{ delete (*Iter); }
			m_elements.erase( m_elements.begin(), m_elements.end() );
		}
	}

	//Проверка Unit-а, что бы он не был 256-м
	bool CBoard::IsGood(SS::Interface::Core::BlackBox::IUnit* pUnit) const
	{
		if ( m_MarkerLastElem == m_elements.end() )
		{ 
			SAVE_LOG(SS_WARNING AND __WFUNCTION__ AND L"Предложение имеет длину более 255 слов.");
			return false;
		}
		for (IIndex* pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
		{
			if (pIndex->GetFirstFeature()) 
			{ return true; }
		}
		return false;
	}

	//Поиск в дереве первого юнита с установленным SyntaxFlag
	void CBoard::Init(IUnit* begin, unsigned int & uiLastPosition)
	{
		SS_TRY
		{
			for (IUnit* topUnit = begin; topUnit != NULL; topUnit = topUnit->GetRightUnit())
			{
				if ( topUnit->GetSyntaxFlag() )
				{
					if ( IsGood(topUnit) )
					{
						if ((*m_MarkerLastElem)->Init(topUnit, m_eLang, uiLastPosition)) 
						{
							m_MarkerLastElem++;
							uiLastPosition++;
						}
					}
				}
				else
				{ 
					Init(topUnit->GetLeftChildUnit(), uiLastPosition); 
				}
			}
		}
		SS_CATCH(L"")
	}

	//Создает гипотезы для всех элементов доски
	void CBoard::CreateVersions()
	{
		SS_TRY
		{
			for (PBLOCK pB = m_Blocks.GetFirst(); pB; pB = m_Blocks.GetNext())
			{ pB->CreateVersions(); }
		}
		SS_CATCH(L"")
	}

	//Проставляет синтаксические категории юнитам и строит многоуровневое дерево
	void CBoard::AssignSyntaxInfoToBlackBoxMultilayer()
	{
		SS_TRY
		{
			if ( m_pAnalyseParams && (m_pAnalyseParams->GetSyntaxAnalysisMode() == SS::Core::Types::esamFullWithoutSyntaxTree) )
			{
				for (BoardElementsIterator i = ElementsBegin(); i != ElementsEnd(); ++i)
				{
					(*i)->SetFeatureToUnit();
					(*i)->RemoveUselessMorpho();
				}
				return;
			}
			
			SS::Interface::Core::BlackBox::IUnit* topLevelUnit = NULL;
			for (BoardElementsIterator i = ElementsBegin(); i != ElementsEnd(); ++i)
				(*i)->SetDone(false);
			//Проставление связей и лучшей синтаксической категории
			for (BoardElementsIterator i = ElementsBegin(); i != ElementsEnd(); ++i)
			{
				(*i)->SetFeatureToUnit();
				(*i)->RemoveUselessMorpho();

				if (!(*i)->IsHead() && !IsCycle(*i))
				{
					SS::Interface::Core::BlackBox::IUnit* unit = (*i)->GetUnit();
					unit->Pop();
					m_elements[(*i)->GetParentIndex()]->GetUnit()->AddUnitVertical(unit);
				}
				else
				{
					topLevelUnit = (*i)->GetUnit();
                    (*i)->SetHead(true);
				}
                (*i)->SetDone(true);
			}

			if (!topLevelUnit)
			{ return; }

			//for (BoardElementsIterator i = ElementsBegin(); i != ElementsEnd(); ++i)
			//{
			//	if ( !(*i)->IsHead() &&
			//		 ( m_oSyntaxFeatureAposteriori.m_SyntaxCategories.IsIntersectsWith(&(*i)->GetSyntaxFeature().m_SyntaxCategories) ) &&
			//		 ( m_elements[(*i)->GetParentIndex()]->GetSyntaxFeature().m_SyntaxCategories.Equal(Values::SyntaxCategories.scMain) ||
			//		   m_elements[(*i)->GetParentIndex()]->GetSyntaxFeature().m_SyntaxCategories.Equal(Values::SyntaxCategories.scChain) ) )
			//	{
			//		SS::Interface::Core::BlackBox::IUnit* unit = (*i)->GetUnit();
			//		unit->Pop();
			//		topLevelUnit->AddUnitHorizontal( unit );
			//	}
			//}
		}
		SS_CATCH(L"")
	}

    bool CBoard::IsCycle(CBoardElement* pElement, CBoardElement* pBorder/* = 0*/) const
    {
        bool cycle = false;
        if(!pElement->IsHead())
        {
            if(!pBorder)
                pBorder = pElement;
            CBoardElement *pParent = m_elements[pElement->GetParentIndex()];
            if(pParent == pBorder)
                cycle = true;
            else if(pParent->IsDone())
                cycle = IsCycle(pParent, pBorder);
        }
        return cycle;
    }

	//Удаление блоков из интервала, задаваемого итераторами
	void CBoard::DeleteBlocks()
	{
		m_Blocks.Clear();
	}

	//Вычисляет расстояние между элементами, учитывая только головные
	int CBoard::HeadDistance(BoardElementsIterator first, BoardElementsIterator last)
	{
		SS_TRY
		{
			if (first == last)
			{ return 0;	}

			int iDistance = 0;
			BoardElementsIterator itElem = first;

			for (itElem; itElem != last; itElem++)
			{
				if ((*itElem)->IsHead())
				{ iDistance++; }
			}
			return iDistance;
		}
		SS_CATCH(L"")
	}

	// Добавить блок
	PBLOCK CBoard::AddBlock()
	{
		PBLOCK pBlock = m_Blocks.Add();
		if (pBlock) return pBlock;
		SAVE_LOG(L"Количество блоков превысило установленый предел.");
		return NULL;
	}

	///Получить предыдущий блок от текущего в процессе перебора
	PBLOCK CBoard::GetPrevBlock() const
	{
		return m_Blocks.GetPrev();
	}

	///Сделать блок текущим
	PBLOCK CBoard::SetCurrentBlock(PBLOCK pBlock) const
	{
		return m_Blocks.SetCurrent(pBlock);
	}

	///Количество блоков
	unsigned int CBoard::CountBlock() const
	{
		return m_Blocks.Size();
	}

	///Установить AnalyseParams
	void CBoard::SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams)
	{
		m_pAnalyseParams = pAnalyseParams;
	}

	///Удалить текущий блок
	void CBoard::DeleteCurrentBlock()
	{
		m_Blocks.DeleteCurrent();
	}

	///Получить родителя данного элемента
	CBoardElement* CBoard::GetParent(CBoardElement* pElem)
	{
		CBoardElement* pElemPr = m_elements[pElem->GetParentIndex()];
		if (pElemPr == pElem) 
		{ return NULL; }
		return pElemPr;
	}
	
}
}
}