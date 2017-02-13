#include "StdAfx.h"
#include ".\data_unit.h"
#include ".\data_storages_factory.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

CDataUnit::CDataUnit(CDataUnitFactory* pDataStorageFactory)
:m_wszDataUnitName(L"undefined"), 
m_wszDataUnitExt(L".udf"), 
m_bIsOpen(false), m_pDataStorageFactory(pDataStorageFactory)
{
	if(ISNULL(m_pDataStorageFactory)) return;
	m_pDataStorageFactory->Register(this);
}

CDataUnit::~CDataUnit()
{
}

HRESULT CDataUnit::QueryInterface(REFIID pIID, void** pBase)
{
	if(pIID==IID_NdxDataUnit){
		*pBase=(INdxDataUnit*)this;
	}else{
		*pBase=NULL;
	}

	return S_OK;
}

ULONG CDataUnit::Release(void)
{
	if(m_pDataStorageFactory){
		m_pDataStorageFactory->UnRegister(this);
	}else{
		ISNULL(m_pDataStorageFactory);
	}

	delete this;
	return 0;
}

void CDataUnit::Create(const wchar_t* pszDataUnitName, const wchar_t* pszDataUnitExt)
{
	m_wszDataUnitName.assign(pszDataUnitName);
	m_wszDataUnitExt.assign(pszDataUnitExt); 
	m_bIsOpen=false;
}

bool CDataUnit::IsOpen(void)
{
	return m_bIsOpen;
}

const wchar_t*	CDataUnit::GetDataUnitName(void)
{
	return m_wszDataUnitName.c_str();
}

const wchar_t* CDataUnit::GetDataUnitExt(void)
{
	return m_wszDataUnitExt.c_str();
}

void CDataUnit::SetBackupSettings(SBackupSettings::ECompressMode CompressMode, SBackupSettings::EWriteMode WriteMode)
{
	m_BackupSettings.m_CompressMode=CompressMode;
	m_BackupSettings.m_WriteMode=WriteMode;
}

//--------------------------------------------------------------------//

}
}
}
}