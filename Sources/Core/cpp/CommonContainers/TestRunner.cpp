//TestRunner.cpp

#include "StdAfx.h"
#include "TestRunner.h"

#ifdef _SS_UNITTESTS

#include <iostream>

namespace SS
{
	namespace UnitTests
	{
		namespace CommonContainers
		{

			//ÊÎÍÑÒÐÓÊÒÎÐÛ
			CTestRunner::CTestRunner() : m_pContainersTest(NULL)
			{
			}

			CTestRunner::~CTestRunner()
			{
				if (m_pContainersTest)
				{
					delete m_pContainersTest;
				}
			}

			//ÌÅÒÎÄ ITestRunner - à
			void CTestRunner::Run()
			{
				SS_TRY
				{	
					m_pContainersTest = new CContainersTest(GetLoadManager());
					
					//m_pContainersTest->TotalTestSearch();
					//m_pContainersTest->TotalTestIndexation();
					//m_pContainersTest->TestIndexationResult();
					//m_pContainersTest->TestSearchResult();
					//m_pContainersTest->TestSearchResultCommon();
					//m_pContainersTest->TestQueryResult();
					//m_pContainersTest->TestServiceIndex();
					//m_pContainersTest->TestTextFeature();
					//m_pContainersTest->TestTextBlockXML();
					m_pContainersTest->TestQueryIndexAttributes();
					//m_pContainersTest->TestIndexAndPosition();
					//m_pContainersTest->TestBlockCut();
					//m_pContainersTest->TestAddTextBlock();
				}		
				SS_CATCH(L"")
			}

			ULONG CTestRunner::Release()
			{
				delete this;
				return 0;
			}

			HRESULT CTestRunner::QueryInterface( REFIID pIID, void** pBase )
			{

				*pBase=NULL;
				if( pIID == IID_TestRunner )
				{
					*pBase=static_cast< SS::Interface::UnitTests::ITestRunner* >( this );
				}
				else if( pIID == IID_Base )
				{
					*pBase=static_cast< SS::Interface::IBase* >( this );
				}
				
				return S_OK;
			}


		}
	}
}

#endif //_SS_UNITTESTS
