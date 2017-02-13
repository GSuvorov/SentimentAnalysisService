#include "stdafx.h"
#include "exception.h"

namespace SS
{
namespace Core
{
namespace CommonServices
{

SS::Interface::Core::CommonServices::IExceptionParams* CException::Add()
{
	SS::Core::Types::Enumerators::TListEnumerator<SS::Core::CommonServices::CExceptionParams>::Add(CExceptionParams());
	return GetCurrent();
};
		
SS::Interface::Core::CommonServices::IExceptionParams* CException::GetCurrent()
{
	return SS::Core::Types::Enumerators::TListEnumerator<SS::Core::CommonServices::CExceptionParams>::GetCurrent();
};

std::wstring& CException::ToString()
{
	m_wsExceptions = L"";
	m_wsExceptions.reserve(1024);
	
	wchar_t wszBuffer[128];

	Reset();
	do
	{
		m_wsExceptions += GetCurrent()->GetMessage();
		m_wsExceptions += L'\n';
		m_wsExceptions += GetCurrent()->GetFunction();
		//m_wsExceptions += L":";
		//m_wsExceptions += GetCurrent()->GetFile();
		//m_wsExceptions += L":";
		//m_wsExceptions += _ltow(GetCurrent()->GetLine(), wszBuffer,10);
		m_wsExceptions += L"\n";
		m_wsExceptions += GetCurrent()->GetFile();
		m_wsExceptions += L"(";
		m_wsExceptions += _ltow(GetCurrent()->GetLine(), wszBuffer,10);
		m_wsExceptions += L")";
	}while(MoveNext());
	m_wsExceptions += L'\n';

	return m_wsExceptions;
};

HRESULT CException::QueryInterface(REFIID pIID, void** pBase)
{
	*pBase = NULL;

	if (pIID == IID_Exception)
		*pBase = (SS::Interface::Core::CommonServices::IException*) this;
	else if (pIID == IID_Base)
		*pBase = (SS::Interface::IBase*) this;

	return (*pBase) ? S_OK : S_FALSE;
}; 

ULONG CException::Release()
{
	delete this;
	return NULL;
};

}
}
}

