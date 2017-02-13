#include "StdAfx.h"
#include ".\settings_operator.h"

USES_LOAD_MANAGER;

namespace SS
{
namespace CommonServices
{

using namespace SS::Interface::Core::CommonServices;

//----------------------------------------------------------------//

void CSettingsOperator::SetWorkPath(const wchar_t* sPath)
{
	if(!sPath){
		SS_THROW(L"GSServer::SetWorkPath - sPath is NULL");
		return;
	}
	m_wsSettingsFilePath.assign(sPath);
	if(*m_wsSettingsFilePath.rbegin()!=92 && *m_wsSettingsFilePath.rbegin()!=47 )
		m_wsSettingsFilePath.append(L"\\");

	m_wsSettingsFilePath.append(L"ss.dat");	
}

void CSettingsOperator::FlushToDisk(void)
{
	SaveSettings();
}

//----------------------------------------------------------------//

}
}