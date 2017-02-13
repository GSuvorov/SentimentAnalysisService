#pragma once

#include "..\ASCInterface\services.h"
using namespace SS::Core::CommonServices;

#include "..\ASCInterface\IInterface.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASCInterface\ILinguisticProcessor.h"
using namespace SS::Interface::Core;

#include "..\ASCInterface\ICommonContainers.h"
using namespace SS::Interface::Core::CommonContainers;

#include "..\ASDInterface\INdxSearchEngine.h"
using namespace SS::Interface::Core::NdxSE;

#include "..\ASSInterface\IPluginManager.h"
using namespace SS::Interface::DocumentPlugins;

#include "..\ASSInterface\ISupervisor.h"
using namespace SS::Interface::Supervisor;

#include <string>
using namespace std;


class CIndexationEngine : 
		public CBaseCoreClass,
		public IIndexationEngine
{
public:
	CIndexationEngine();
	virtual ~CIndexationEngine();

public:
	virtual void SetLoadManager(ILoadManager* pLoadManager);
	virtual bool Indexate(LPCWSTR szPath);

private:
	bool IndexateFile(LPCWSTR szFile);
	void IndexateFileBlock(LPCWSTR szText, LPCWSTR szBlockType);
	void LoadPlugins();
	LPCWSTR GetFieldName(EPluginBlockType eType);

private:
	INdxSearchEngineIndexation* m_pNdxProcessor;
	ILinguisticIndexation* m_pLinguisticIndexation;
	IIndexationResult* m_pResult;
	IPluginManager* m_pPluginManager;
	wstring m_strText;
};
