#include "stdafx.h"
#include "ExpertController.h"

namespace SS
{
namespace Syntax
{
namespace ExpertModel
{
using namespace SS::Syntax::BoardElements;
CGroupManager CExpertController::m_oGroupManager;

//Конструктор
CExpertController::CExpertController(ESyntaxBase eBase) : m_isVersioned(false), m_bInitBoard(false), m_bOneHyp(false), m_iNumHyp(0)
{
	m_oGroupManager.SetBase(eBase);
}

///Возвращает ининтерфейс для работы с загрузкой экспертов
SS::Interface::Core::Common::ILoad* CExpertController::GetLoader()
{
	return &(m_oGroupManager);
}

//переопределяю от IBase
void CExpertController::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
	m_oBoard.FillBoard(pLoadManager);
	m_oGroupManager.SetLoadManager(pLoadManager);
}

//Устанавливает анализируемое предложение для тестовых целей
void CExpertController::SetSentenceTest( SS::Interface::Core::BlackBox::ISentence* pSentence, ParsingType pType)
{
	SS_TRY
		m_oGroupManager.SetParsingType(pType);
		m_itCurrentGroupPtr = m_oGroupManager.GetBeginGroup();
		m_itCurrentExpertPtr = (*m_itCurrentGroupPtr)->begin();
		m_isVersioned = false;
		SetSentence(pSentence);
	SS_CATCH(L"")
}

//Устанавливает анализируемое предложение
void CExpertController::SetSentence(SS::Interface::Core::BlackBox::ISentence* pSentence)
{
	SS_TRY
		m_bInitBoard = m_oBoard.Initialize(pSentence);
	SS_CATCH(L"")
}

void CExpertController::ParseSentenceDynamic(ParsingType pType)
{
	SS_TRY
		if (m_bInitBoard) 
		{
			m_oGroupManager.ParseSentenceDynamic( &(m_oBoard), pType );
		}
	SS_CATCH(L"")
}

void CExpertController::PerformStep()
{
	SS_TRY
	{	
		if ( (*m_itCurrentGroupPtr)->IsAmbiguity() )
		{
			for (PBLOCK pBlock = m_oBoard.GetFirstBlock(); pBlock; pBlock = m_oBoard.GetNextBlock())
			{ (*m_itCurrentExpertPtr)->PassBoard(m_oBoard, pBlock); }
			(*m_itCurrentExpertPtr)->ClearApplicableState();
		}
		else
		{
			if ( !m_isVersioned )
			{
				m_oBoard.CreateVersions();
				m_isVersioned = true;
			}
			
			for (PBLOCK pBlock = m_oBoard.GetFirstBlock(); pBlock; pBlock = m_oBoard.GetNextBlock())
			{
				if (m_bOneHyp)
				{
					pBlock->SetHypothesis(m_iNumHyp);
					(*m_itCurrentExpertPtr)->PassBoard(m_oBoard, pBlock);
					(*m_itCurrentExpertPtr)->ClearApplicableState();
					pBlock->SetCurrentVersion();
				}
				else
				{
					while ( pBlock->SwitchVersion() )
					{
						(*m_itCurrentExpertPtr)->PassBoard(m_oBoard, pBlock);
						(*m_itCurrentExpertPtr)->ClearApplicableState();
					}
					pBlock->SetBestVersion();
				}
			}
		}

		m_itCurrentExpertPtr++;

		if ( m_itCurrentExpertPtr == (*m_itCurrentGroupPtr)->end() )
		{
			m_itCurrentGroupPtr++;
			
			if ( m_itCurrentGroupPtr == m_oGroupManager.GetEndGroup() )
			{ m_itCurrentGroupPtr = m_oGroupManager.GetBeginGroup(); }
		
			m_itCurrentExpertPtr = (*m_itCurrentGroupPtr)->begin();
		}
	}
	SS_CATCH(L"")
}

void CExpertController::PerformGroup()
{
	SS_TRY
	{
		if ( (*m_itCurrentGroupPtr)->IsAmbiguity() )
		{
			while ( m_itCurrentExpertPtr != (*m_itCurrentGroupPtr)->end() )
			{
				//Для фрагментации
				if ( (*m_itCurrentExpertPtr)->GetName() == L"Fragmentation Expert Query" )
				{
					m_itCurrentExpertPtr++;
					if ( m_itCurrentExpertPtr == (*m_itCurrentGroupPtr)->end() )
					{
						m_itCurrentGroupPtr++;
						m_itCurrentExpertPtr = (*m_itCurrentGroupPtr)->begin();
						return;
					}
				}
				for (PBLOCK pBlock = m_oBoard.GetFirstBlock(); pBlock; pBlock = m_oBoard.GetNextBlock())
				{ (*m_itCurrentExpertPtr)->PassBoard(m_oBoard, pBlock); }
				
				(*m_itCurrentExpertPtr)->ClearApplicableState();
				m_itCurrentExpertPtr++;
			}
		}
		else
		{
			if ( !m_isVersioned )
			{
				m_oBoard.CreateVersions();
				m_isVersioned = true;
			}

			CExpertGroup::TExpertPtrItr tpStartExpert = m_itCurrentExpertPtr;
			for (PBLOCK pBlock = m_oBoard.GetFirstBlock(); pBlock; pBlock = m_oBoard.GetNextBlock())
			{
				if (m_bOneHyp)
				{ pBlock->SetHypothesis(m_iNumHyp);	}

				m_itCurrentExpertPtr = tpStartExpert;
				for (; m_itCurrentExpertPtr != (*m_itCurrentGroupPtr)->end(); ++m_itCurrentExpertPtr)
				{
					if (m_bOneHyp)
					{
						(*m_itCurrentExpertPtr)->PassBoard(m_oBoard, pBlock);
						(*m_itCurrentExpertPtr)->ClearApplicableState();
					}
					else
					{
						while ( pBlock->SwitchVersion() )
						{
							(*m_itCurrentExpertPtr)->PassBoard(m_oBoard, pBlock);
							(*m_itCurrentExpertPtr)->ClearApplicableState();
						}
					}
				}
				
				if (m_bOneHyp) 
				{ pBlock->SetCurrentVersion(); }
				else
				{ pBlock->SetBestVersion();	}
			}
		}

		m_itCurrentGroupPtr++;

		if ( m_itCurrentGroupPtr == m_oGroupManager.GetEndGroup() )
		{ m_itCurrentGroupPtr = m_oGroupManager.GetBeginGroup(); }
		
		m_itCurrentExpertPtr = (*m_itCurrentGroupPtr)->begin();
	}
	SS_CATCH(L"")
}

void CExpertController::PerformVersion()
{
	SS_TRY
	{

		if ( (*m_itCurrentGroupPtr)->IsAmbiguity() )
		{ 
			return; 
		}
		else
		{
			for (PBLOCK pBlock = m_oBoard.GetFirstBlock(); pBlock; pBlock = m_oBoard.GetNextBlock())
			{
				if ( (!pBlock->VersionsFinish()) && (pBlock->SwitchVersion()) ) 
				{
					(*m_itCurrentExpertPtr)->PassBoard(m_oBoard, pBlock);
					(*m_itCurrentExpertPtr)->ClearApplicableState();
				}
			}
		}
	}
	SS_CATCH(L"")
}

void CExpertController::SetParams(bool bOneHyp, int iNumHyp)
{
	SS_TRY
	{
		if ( (bOneHyp != m_bOneHyp) || (iNumHyp != m_iNumHyp) )
		{
			m_itCurrentGroupPtr = m_oGroupManager.GetBeginGroup();
			
			m_itCurrentExpertPtr = (*m_itCurrentGroupPtr)->begin();
			m_isVersioned = false;
			
			//запись в класс
			m_bOneHyp = bOneHyp;
			m_iNumHyp = iNumHyp;
		}
	}
	SS_CATCH(L"")
}

//Установить AnalyseParams
void CExpertController::SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams)
{
	m_oGroupManager.SetAnalyseParams(pAnalyseParams);
	m_oBoard.SetAnalyseParams(pAnalyseParams);
}


}
}
}







