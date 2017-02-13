// SyntaxConstructions.cpp : Defines the entry point for the console application.
//

#pragma once

#include "stdafx.h"

#include ".\SyntaxConstructions.h"

#include "..\ASSInterface\TBaseNames.h"
//#include "..\ASSInterface\GUID.h"

#include ".\SyntaxConstructionManager.h"
#include ".\TestRunner.h"

SET_LOAD_MANAGER_IMPLEMENTATION;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

/// Создает класс по запросу пользователя модуля
/** 
	\param *pGuid - GUID класса, который необходимо создать
	\param **ppBase - указатель на интерфейс IBase, который реализует требуемый класс
	\return HRESULT - результат выполнения операции
 */
HRESULT CreateInstance(const GUID *pGuid, void **ppBase)
{
	if (*pGuid == CLSID_ISyntaxConstructionManagerEng)
	{
		*ppBase=(SS::Interface::IBase*)
		new SS::SyntaxConstructions::CSyntaxConstructionManager(SS::MANAGER::Constants::c_szSyntaxConstructionsEng);	
	}
	else if (*pGuid == CLSID_ISyntaxConstructionManagerRus)
	{
		*ppBase = (SS::Interface::IBase*)
		new SS::SyntaxConstructions::CSyntaxConstructionManager(SS::MANAGER::Constants::c_szSyntaxConstructionsRus);	
	}
	else if (*pGuid == CLSID_ISyntaxConstructionManagerRus_NQ)
	{
		*ppBase = (SS::Interface::IBase*)
		new SS::SyntaxConstructions::CSyntaxConstructionManager(SS::MANAGER::Constants::c_szSyntaxConstructionsRus_NQ);	
	}
	else if (*pGuid == CLSID_ISemanticConstructionManagerEng)
	{
		*ppBase = (SS::Interface::IBase*)
		new SS::SyntaxConstructions::CSyntaxConstructionManager(SS::MANAGER::Constants::c_szSemanticConstructionsEng);	
	}
	 else if (*pGuid == CLSID_ISemanticConstructionManagerRus)
	 {
		*ppBase = (SS::Interface::IBase*)
		new SS::SyntaxConstructions::CSyntaxConstructionManager(SS::MANAGER::Constants::c_szSemanticConstructionsRus);	
	 }
	else if (*pGuid == CLSID_TestRunner)
	{
		*ppBase = (SS::Interface::IBase*)
		new SS::UnitTests::SyntaxConstructions::CTestRunner();	
	}
	else
	{
		*ppBase = NULL;
	}
	return S_OK;
};