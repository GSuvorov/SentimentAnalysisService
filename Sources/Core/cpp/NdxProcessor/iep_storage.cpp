#include "StdAfx.h"
#include ".\iep_storage.h"
#include "console.h"
#include "const.h"


namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Storages
{

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

CIndexEntryPointTable::CIndexEntryPointTable(INdxTableStorage* pNdxTableStorage)
:m_pNdxTableStorage(pNdxTableStorage)
{
	if(ISNULL(m_pNdxTableStorage)) return;

	if(!m_pNdxTableStorage->IsOpen()){
		TO_CONSOLE(L"NdxTableStorage not opened");
		ToConsole(m_pNdxTableStorage->GetDataUnitName());
		return;
	}

	m_uiIndex=0;
	m_uiEntryPoint=0;
	m_uiDF;

	m_pToDataAdd[0]=NULL;
	m_pToDataAdd[1]=NULL;
	m_pToDataAdd[2]=NULL;

	m_pToDataGet[0]=NULL;
	m_pToDataGet[1]=NULL;
	m_pToDataGet[2]=NULL;

	m_pToDataModify[0]=NULL;
	m_pToDataModify[1]=NULL;
	m_pToDataModify[2]=NULL;
	
	//инитимся
	InitFind(&m_uiEntryPoint, &m_uiDF);
	InitModify(&m_uiEntryPoint, &m_uiDF);
	InitAdd();
}

CIndexEntryPointTable::~CIndexEntryPointTable(void)
{
}

bool CIndexEntryPointTable::InitFind(void* pvOffset, void* pvDF)
{
	m_pToDataGet[0]=NULL;
	m_pToDataGet[1]=pvOffset;
	m_pToDataGet[2]=pvDF;
	return m_pNdxTableStorage->InitFind(L"Index", &m_uiIndex, m_pToDataGet)==S_OK?true:false;
}

unsigned int CIndexEntryPointTable::GetIndexEntryPoint(unsigned int uiIndex)
{
	if(uiIndex==EMPTY_SOURCE) return EMPTY_OFFSET;

	m_uiIndex=uiIndex;

	if(m_pNdxTableStorage->FindOne()==S_OK) 
		return m_uiEntryPoint;
	else
		return EMPTY_OFFSET;  
}

unsigned int CIndexEntryPointTable::GetIndexDF(unsigned int uiIndex)
{
	if(uiIndex==EMPTY_SOURCE) return 0;

	m_uiIndex=uiIndex;

	if(m_pNdxTableStorage->FindOne()==S_OK) 
		return m_uiDF;
	else
		return 0;  
}

bool CIndexEntryPointTable::GetIndexData(unsigned int uiIndex, unsigned int* puiOffset, unsigned int* puiDF)
{
	if(uiIndex==EMPTY_SOURCE){
		*puiOffset=EMPTY_OFFSET;
		*puiDF=0;
		return false;
	}

	m_uiIndex=uiIndex;
	
	if(m_pNdxTableStorage->FindOne()==S_OK){
		*puiDF=m_uiDF;	
		*puiOffset=m_uiEntryPoint;
		return true;
	}

	*puiOffset=EMPTY_OFFSET;
	*puiDF=0;
	return false;     
}

bool CIndexEntryPointTable::SetIndexEntryPoint(unsigned int uiIndex, unsigned int uiEntryPoint)
{
	return SetIndexEntryPoint(uiIndex, uiEntryPoint, 0);
}


bool CIndexEntryPointTable::SetIndexEntryPoint(unsigned int uiIndex, unsigned int uiEntryPoint, unsigned int uiIndexDF)
{
	HRESULT hr;

	try{
		if(uiIndex==EMPTY_VALUE) return false;

		m_uiIndex=uiIndex;
		m_uiEntryPoint=uiEntryPoint;
		m_uiDF=uiIndexDF;
		
		//сначало пытаемся изменить запиcь
		hr=m_pNdxTableStorage->FindOneAndChange();
		switch(hr){
		case S_OK:
			return true;
			break;
		case S_FALSE:
			if(m_pNdxTableStorage->Add()!=S_OK){
				ERR_TO_CONSOLE(erDBMS, L"Cann't add record");
				return false;
			}else{
				return true;
			}
			break;
		default:
			ERR_TO_CONSOLE(erDBMS, L"Cann't FindOneAndChange");
			return false;
			break;
		}
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return false;
	}
}

bool CIndexEntryPointTable::InitAdd(void)
{
	m_pToDataAdd[0]=&m_uiIndex;
	m_pToDataAdd[1]=&m_uiEntryPoint;
	m_pToDataAdd[2]=&m_uiDF;
	return m_pNdxTableStorage->InitAdd(m_pToDataAdd)==S_OK?true:false;
}

bool CIndexEntryPointTable::InitModify(void* pvOffset, void* pvDF)
{
	m_pToDataModify[0]=NULL;
	m_pToDataModify[1]=pvOffset;
	m_pToDataModify[2]=pvDF;

	return m_pNdxTableStorage->InitModify( m_pToDataModify)==S_OK?true:false;
}

bool CIndexEntryPointTable::ModifyIndexDataRecord(unsigned int uiIndex, unsigned int uiEntryPoint, unsigned int uiDF)
{
	return SetIndexEntryPoint(uiIndex, uiEntryPoint, uiDF);
}

bool CIndexEntryPointTable::ModifyIndexRecordDF(unsigned int uiIndex, unsigned int uiDF)
{
	HRESULT hr;

	try{
		if(uiIndex==EMPTY_VALUE) return false;

		m_uiIndex=uiIndex;
		m_uiDF=uiDF;
		
		//сначало пытаемся изменить запиcь
		hr=m_pNdxTableStorage->FindOneAndChange();
		switch(hr){
		case S_OK:
			return true;
			break;
		case S_FALSE:
			//добавляем запись с пустой точкой входа
			m_uiEntryPoint=EMPTY_VALUE;
			if(m_pNdxTableStorage->Add()!=S_OK){
				ERR_TO_CONSOLE(erDBMS, L"Cann't add record");
				return false;
			}else{
				return true;
			}
			break;
		default:
			ERR_TO_CONSOLE(erDBMS, L"Cann't FindOneAndChange");
			return false;
			break;
		}
	}catch(...){
		ERR_TO_CONSOLE(erTryCatch, L"");
		return false;
	}
}

bool CIndexEntryPointTable::InitScanOffsetDF()
{
	m_uiEntryPoint=0;
	m_pToDataGet[0]=&m_uiIndex;
	m_pToDataGet[1]=&m_uiEntryPoint;
	m_pToDataGet[2]=&m_uiDF;

	return m_pNdxTableStorage->InitScan(0, m_pToDataGet)==S_OK?true:false;
}

bool CIndexEntryPointTable::MoveNext(unsigned int* puiSourceIndex, unsigned int* puiDF )
{
	if ( m_pNdxTableStorage->Scan() != S_OK ) return false;
	*puiSourceIndex=m_uiIndex;
	*puiDF=m_uiDF;
	return true;
}

//--------------------------------------------------------------------//

}
}
}
}
}
