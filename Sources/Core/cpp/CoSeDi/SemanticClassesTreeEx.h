
#pragma once

#include "..\ASSInterface\ICoSeDi.h"
#include ".\SemanticClassesTree.h"

//namespace SS
//{
//	namespace CoSeDi
//	{
//		namespace SQL
//		{
//			class CSemanticClassesTreeEx : 
//				public SS::Interface::Core::CoSeDi::ISemanticClassesTree,
//				public SS::Core::CommonServices::CBaseCoreClass
//			{
//			public:
//				CSemanticClassesTreeEx(void);
//				~CSemanticClassesTreeEx(void);
//			public:
//				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
//				{
//					SS_TRY
//					{
//						SS::Core::CommonServices::CBaseCoreClass::SetLoadManager( pLoadManager);
//
//						m_oSemanticClassesTreeRus.SetLoadManager( pLoadManager);
//						m_oSemanticClassesTreeEng.SetLoadManager( pLoadManager);
//
//					}
//					SS_CATCH(L"");
//				}
//
//			public:
//				void	SetClass(/*in*/int iIDClass);
//				int		GetClass(/*in*/std::wstring * psClassName);
//				void	GetChilds(/*in*/int iLevel, /*out*/std::list<int>* p_list_Childs);
//				void	GetSame(/*out*/std::list<int>* p_list_Same);
//				int		GetParent();
//				void	GetParents(/*in*/int iLevel, /*out*/std::list<int>* p_list_Parents);
//				void	GetRoots(/*out*/std::list<int>* p_list_Root);
//				void	GetName(/*out*/ std::wstring * psClassName);
//				void	SetName(/*out*/ std::wstring * psClassName);	
//				int		CreateChild(/*in*/std::wstring * psClassName); 
//				void	DeleteClass();
//			protected:
//				SS::CoSeDi::SQL::CSemanticClassesTree m_oSemanticClassesTreeRus;
//				SS::CoSeDi::SQL::CSemanticClassesTree m_oSemanticClassesTreeEng;
//			};
//		};
//
//		namespace DBMS
//		{
//			class CSemanticClassesTreeEx : 
//				public SS::Interface::Core::CoSeDi::ISemanticClassesTree,
//				public SS::Core::CommonServices::CBaseCoreClass
//			{
//			public:
//				CSemanticClassesTreeEx(void);
//				~CSemanticClassesTreeEx(void);
//			public:
//				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
//				{
//					SS_TRY
//					{
//						SS::Core::CommonServices::CBaseCoreClass::SetLoadManager( pLoadManager);
//
//						m_oSemanticClassesTreeRus.SetLoadManager( pLoadManager);
//						m_oSemanticClassesTreeEng.SetLoadManager( pLoadManager);
//
//					}
//					SS_CATCH(L"");
//				}
//			public:
//				void	SetClass(/*in*/int iIDClass);
//				int		GetClass(/*in*/std::wstring * psClassName);
//				void	GetChilds(/*in*/int iLevel, /*out*/std::list<int>* p_list_Childs);
//				void	GetSame(/*out*/std::list<int>* p_list_Same);
//				int		GetParent();
//				void	GetParents(/*in*/int iLevel, /*out*/std::list<int>* p_list_Parents);
//				void	GetRoots(/*out*/std::list<int>* p_list_Root);
//				void	GetName(/*out*/ std::wstring * psClassName);
//				void	SetName(/*out*/ std::wstring * psClassName);	
//				int		CreateChild(/*in*/std::wstring * psClassName); 
//				void	DeleteClass();
//			protected:
//				SS::CoSeDi::DBMS::CSemanticClassesTree m_oSemanticClassesTreeRus;
//				SS::CoSeDi::DBMS::CSemanticClassesTree m_oSemanticClassesTreeEng;
//			};
//		};
//	};
//}
