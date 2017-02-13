#include "StdAfx.h"
#include ".\smexpertgroup.h"

namespace SS
{
	namespace Semantic
	{

		CSMExpertGroup::CSMExpertGroup(TCHAR * scDescription)
		{
			m_szDescription=scDescription;
		}

		CSMExpertGroup::~CSMExpertGroup(void)
		{
			DeleteExperts();
		}
		bool CSMExpertGroup::Analize(BlackBox::IText * pIText)
		{
			SS_TRY
			{
				for(m_CurrentExpert=m_Experts.begin(); m_CurrentExpert!=m_Experts.end(); m_CurrentExpert++)
				{
					(*m_CurrentExpert)->Init(pIText);
					(*m_CurrentExpert)->SetAnalyseParams(GetAnalyseParams());
					(*m_CurrentExpert)->Analize();

				}
				return true;
			}
			SS_CATCH(L"")
		}

		void CSMExpertGroup::AddExpert( CSMExpert * pExpert )
		{
			SS_TRY
			{
				if(pExpert)
				{
					m_Experts.push_back(pExpert);
				}
			}
			SS_CATCH(L"")
		}
		void CSMExpertGroup::SetExpertsSMManager(CSMManager * pManager)
		{
			SS_TRY
			{
				for(m_CurrentExpert=m_Experts.begin(); m_CurrentExpert!=m_Experts.end(); m_CurrentExpert++)
				{
					(*m_CurrentExpert)->SetSMManager(pManager);
				}
			}
			SS_CATCH(L"")
		}
		void CSMExpertGroup::DeleteExperts()
		{
			SS_TRY
			{
				for(m_CurrentExpert=m_Experts.begin(); m_CurrentExpert!=m_Experts.end(); m_CurrentExpert++)
				{
					delete (*m_CurrentExpert);
					(*m_CurrentExpert) = NULL;
				}
				m_Experts.clear();
			}
			SS_CATCH(L"")
		}



//////////////////////////////////////////////////////////////////////////

	}
}