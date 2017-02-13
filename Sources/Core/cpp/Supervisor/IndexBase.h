#pragma once

#include "..\ASSInterface\ISupervisor.h"
using namespace SS::Interface::Supervisor;

#include "..\ASDInterface\INdxBaseManager.h"
using namespace SS::Interface::Core::NdxSE;

#include "..\ASCInterface\IBaseManager.h"
using namespace SS::Interface::Core::ResourceManagers;

#include <string>
using namespace std;

class CBasesList;


class CIndexBase : public IIndexBase
{
public:
	CIndexBase(CBasesList* pParent, INdxBaseManager* pNdxBaseManager, IBaseManager* pBaseManager,
		LPCWSTR szDatabasePath, LPCWSTR szName);
	virtual ~CIndexBase();

public:
	void Init(LPCWSTR szDescr, BOOL bKnBase);
	virtual bool Open();
	virtual void Close();
	virtual void Remove();
	virtual LPCWSTR GetPath();
	virtual LPCWSTR GetName();
	virtual LPCWSTR GetDescription();
	virtual bool SetDescription(LPCWSTR szDescription);
	virtual BOOL IsKnowledgeBase();
	virtual bool SetKnowledgeBase(BOOL bSet);

private:
	wstring m_strName;
	wstring m_strDescr;
	BOOL m_bKnBase;

	INdxBaseManager* m_pNdxBaseManager;
	IBaseManager* m_pBaseManager;
	wstring m_strDatabasePath;
	CBasesList* m_pParent;
};
