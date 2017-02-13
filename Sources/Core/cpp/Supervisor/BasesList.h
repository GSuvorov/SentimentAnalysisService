#pragma once

#include "..\ASCInterface\services.h"
using namespace SS::Core::CommonServices;

#include "..\ASCInterface\IInterface.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASCInterface\ICommonServices.h"
using namespace SS::Interface::Core::CommonServices;

#include "..\ASSInterface\ISupervisor.h"
using namespace SS::Interface::Supervisor;

#include "IndexBase.h"

#include <vector>
#include <string>
using namespace std;


class CBasesList : 
		public CBaseCoreClass,
		public IDatabaseList
{
public:
	CBasesList();
	virtual ~CBasesList();

// IDatabaseList
public:
	virtual void SetLoadManager(ILoadManager* pLoadManager);
	virtual IIndexBase* GetFirstBase();
	virtual IIndexBase* GetNextBase();
	virtual IIndexBase* FindBase(LPCWSTR szDescription);
	virtual IIndexBase* CreateBase(LPCWSTR szDescription);

public:
	void OnDatabaseOpen(CIndexBase* pBase);
	bool OnDatabaseClose(CIndexBase* pBase);
	void OnDatabaseRemoved(CIndexBase* pBase);

public:
	void Clear();
	void FillDatabaseList();

private:
	void GetDatabaseParams(LPCWSTR szBaseDir, LPCWSTR szDir, LPCWSTR szFileName, CIndexBase* pBase);

private:
	IWorkRegister* m_pWorkRegister;
	INdxBaseManager* m_pNdxBaseManager;
	IBaseManager* m_pBaseManager;

	typedef vector<CIndexBase*> TBasesCollection;
	TBasesCollection m_vBases;
	TBasesCollection::iterator m_itCurrBase;

	wstring m_strDatabasePath;
	CIndexBase* m_pCurrBase;
};
