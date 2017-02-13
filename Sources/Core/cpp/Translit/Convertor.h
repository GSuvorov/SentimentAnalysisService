#pragma once

#include "..\ASSInterface\ITranslit.h"
using namespace SS::Interface;

#include "..\ASCInterface\IInterface.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASCInterface\services.h"
using namespace SS::Core::CommonServices;

#include <string>
using namespace std;

#include "DirectConvertor.h"
#include "Orthographer.h"


class CConvertor : public ITranslit
{
public:
	CConvertor();
	virtual ~CConvertor();

// IBase
public:
	virtual HRESULT QueryInterface(REFIID refIID, void** pBase);
	virtual ULONG Release();

// ITranslit
public:
	virtual void Rus2Lat(LPCWSTR szText, wstring* pResult);
	virtual void Lat2Rus(LPCWSTR szText, wstring* pResult);
	virtual void CheckOrthography(LPCWSTR szText, wstring* pResult);

private:
	CDirectConvertor* m_pDirectConvertor;
	COrthographer* m_pOrthographer;
};
