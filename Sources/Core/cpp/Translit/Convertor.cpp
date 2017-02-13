#include "stdafx.h"
#include "Convertor.h"


/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////////////

CConvertor::CConvertor() :
		m_pDirectConvertor(NULL),
		m_pOrthographer(NULL)
{
}

CConvertor::~CConvertor()
{
	delete m_pDirectConvertor;
	delete m_pOrthographer;
}

/////////////////////////////////////////////////////////////////////////////////////
// IBase interface

HRESULT CConvertor::QueryInterface(REFIID refIID, void** pBase)
{
	if(refIID == IID_Translit)
		*pBase = (ITranslit*) this;
	else if(refIID == IID_Base)
		*pBase = (SS::Interface::IBase*) this;
	else
		*pBase = NULL;

	return S_OK;
}

ULONG CConvertor::Release()
{
	delete this;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
// CConvertor interface

void CConvertor::Rus2Lat(LPCWSTR szText, wstring* pResult)
{
	SS_TRY;

	SS_CATCH(L"");
}

void CConvertor::Lat2Rus(LPCWSTR szText, wstring* pResult)
{
	SS_TRY;

	if(!m_pDirectConvertor)
	{
		m_pDirectConvertor = new CDirectConvertor;
		m_pDirectConvertor->SetLoadManager(GetLoadManager());
	}
	m_pDirectConvertor->Lat2Rus(szText, pResult);

	SS_CATCH(L"");
}

void CConvertor::CheckOrthography(LPCWSTR szText, wstring* pResult)
{
	SS_TRY;

	if(!m_pOrthographer)
	{
		m_pOrthographer = new COrthographer;
		m_pOrthographer->SetLoadManager(GetLoadManager());
	}
	m_pOrthographer->CheckOrthography(szText, pResult);

	SS_CATCH(L"");
}
