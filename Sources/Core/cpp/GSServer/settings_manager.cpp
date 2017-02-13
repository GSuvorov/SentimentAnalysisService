#include "StdAfx.h"
#include ".\settings_manager.h"
#include "..\LoadManager\WorkRegister.h"

USES_LOAD_MANAGER;


namespace SS
{
namespace CommonServices
{

using namespace SS::Interface::Core::CommonServices;

//----------------------------------------------------------------//

CSettingsManager::CSettingsManager(void)
:m_bSettingsLoaded(false)
{
	m_LastFileTime.dwHighDateTime = 0;
	m_LastFileTime.dwLowDateTime  = 0;
}

CSettingsManager::~CSettingsManager(void)
{
	ClearSettings();
}

void CSettingsManager::MakeSettingFilePath(std::wstring* pwsFilePath)
{
	if(!pwsFilePath){
		SS_THROW(L"GSServer::MakeSettingFilePath - pwsFilePath is NULL");
		return;
	}

	if(m_wsSettingsFilePath.size()==0){

		if(!GetLoader()){
			SS_THROW(L"cann't GetLoadManager");
			return;
		}

		SS::Interface::Core::CommonServices::IWorkRegister* pWorkRegister=NULL;
		pWorkRegister = (SS::Interface::Core::CommonServices::IWorkRegister*)
					GetLoader()->GetInterface(L"./core/LoadManager.dll", CLSID_WorkRegister, IID_WorkRegister);
		
		if(pWorkRegister){
			std::wstring wsName=L"WorkPath";
			std::wstring wsInfo;
			wsInfo = pWorkRegister->GetParameter(wsName.c_str());
			m_wsSettingsFilePath.assign(wsInfo);
			m_wsSettingsFilePath.append(L"\\ss.dat");	
		}else{
			SS_THROW(L"Cann't load WorkRegister");
		}
	}
	
	pwsFilePath->assign(m_wsSettingsFilePath);
}

void CSettingsManager::OperateParameter(bool bGetOrSet, const wchar_t* wszName, void* pValue, EParameterType peType, bool bIsArray)
{
	if(!pValue)
		SS_THROW(L"pValue is NULL");

	CBasisSetting* pSetting=GetSetting(wszName);
	if(!pSetting)
		SS_THROW(L"Setting by name: " AND wszName AND L" does not exist");

	bool bResult;
	if(bGetOrSet)
		bResult=pSetting->GetSettingValue((void*)pValue, peType, bIsArray);
	else
		bResult=pSetting->SetSettingValue((void*)pValue, peType, bIsArray);
	
	if(!bResult)
		SS_THROW(L"OperateParameter for setting by name: " AND wszName AND L" not successed, possible values types not equal. Check input setting type");
}

void CSettingsManager::ClearSettings(void)
{
	TSettingsCollection::iterator itSetting=m_SettingsCollection.begin();
	while(itSetting!=m_SettingsCollection.end()){
		delete itSetting->second;
		itSetting++;
	}
	m_SettingsCollection.clear();
}

CBasisSetting* CSettingsManager::GetSetting(const wchar_t* wszSettingName)
{
	if(!CheckUpToDate())
		SS_THROW(L"CheckUpToDate not succeeded");

	TSettingsCollection::iterator itSetting=m_SettingsCollection.find(wszSettingName);
	if(itSetting!=m_SettingsCollection.end()){
		return itSetting->second;
	}
	return NULL;
}

bool CSettingsManager::AddSetting(CBasisSetting* pSetting)
{
	if(!pSetting) return false;

	TSettingsCollection::iterator itSetting=m_SettingsCollection.find(pSetting->GetSettingName());
	if(itSetting==m_SettingsCollection.end()){
		m_SettingsCollection[pSetting->GetSettingName()]=pSetting;
		return true;
	}else
		return false;
}

void CSettingsManager::LoadSettings(void)
{
	//читаем файл в буфер
	std::vector<unsigned char> Buffer;
	ReadFromFile(&Buffer);
	
	ClearSettings();

	//наполняем настройки по буферу
	SSettingType SettingType;
	unsigned char* pucFrontPos=&Buffer[0];
	unsigned int uiReadedSize=0;
	unsigned int uiTotalReadedSize=0;
	while(uiTotalReadedSize<Buffer.size()){
		//читаем тип настройки
		memcpy((void*)&SettingType, pucFrontPos+uiTotalReadedSize, sizeof(SSettingType));
		uiTotalReadedSize+=sizeof(SSettingType);

		//создаем настройку
		CBasisSetting* pBasisSetting=m_SettingFactory.CreateSetting(SettingType);
		
		//читаем в нее данные
		pBasisSetting->FromBinary(pucFrontPos+uiTotalReadedSize, (unsigned int)Buffer.size()-uiTotalReadedSize, &uiReadedSize);
		uiTotalReadedSize+=uiReadedSize;

		//заносим настройку в коллекцию
		if(!AddSetting(pBasisSetting)){
			SAVE_LOG(L"Cann't add setting: " AND pBasisSetting->GetSettingName());
		}
	}

	m_bSettingsLoaded=true;
}

void CSettingsManager::SaveSettings(void)
{
	if(!m_bSettingsLoaded) return;

	//создаем буфер
	std::vector<unsigned char> Buffer;

	SSettingType SettingType;
	TSettingsCollection::iterator itSetting=m_SettingsCollection.begin();
	while(itSetting!=m_SettingsCollection.end()){
		//добавляем в буфер тип настройки
		SettingType=itSetting->second->GetSettingType();
		Buffer.resize(Buffer.size() + sizeof(SSettingType));
		memcpy((void*)&Buffer[Buffer.size()-sizeof(SSettingType)], (void*)&SettingType, sizeof(SSettingType));
		
		//добавляем в буфер настройку
		itSetting->second->ToBinary(&Buffer);

		itSetting++;
	}
	
	//пишем буфер в файл
	WriteToFile(&Buffer);
	
	m_bSettingsLoaded=false;
}

bool CSettingsManager::WriteToFile(std::vector<unsigned char>* pSettingsBuffer)
{
	if(!pSettingsBuffer)
		SS_THROW(L"pSettingsBuffer is NULL");

	std::wstring wsFilePath;
	MakeSettingFilePath(&wsFilePath);

	//открываем файл
	DWORD dwCreationDisposition=GENERIC_READ|GENERIC_WRITE; 
	DWORD dwDesiredAccess=OPEN_ALWAYS;
	HANDLE hFile=CreateFileW(wsFilePath.c_str(),
					dwCreationDisposition, 
					FILE_SHARE_READ | FILE_SHARE_WRITE, 
					NULL,
					dwDesiredAccess, 
					FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
					NULL);
	
	if(hFile==INVALID_HANDLE_VALUE)
		SS_THROW(L"Cann't open settings file");

	//очищаем файл
	LARGE_INTEGER FPos64={0};
	FPos64.LowPart=0;
	SetFilePointer(hFile, FPos64.LowPart, &FPos64.HighPart, FILE_BEGIN);
	SetEndOfFile(hFile);

	if(!pSettingsBuffer->empty()){
		//кодируем буфер
		EncodeDecodeBuffer(pSettingsBuffer);

		//пишем буфер в файл
		DWORD WrittenDataLen;
		WriteFile(hFile, &pSettingsBuffer->front(), (DWORD)pSettingsBuffer->size(), &WrittenDataLen, NULL);
	}

	//закрываем файл
	CloseHandle(hFile);

	return true;
}

bool CSettingsManager::ReadFromFile(std::vector<unsigned char>* pSettingsBuffer)
{
	if(!pSettingsBuffer)
		SS_THROW(L"pSettingsBuffer is NULL");

	std::wstring wsFilePath;
	MakeSettingFilePath(&wsFilePath);

	//открываем файл
	DWORD dwCreationDisposition=GENERIC_READ; 
	DWORD dwDesiredAccess=OPEN_ALWAYS;
	HANDLE hFile=CreateFileW(wsFilePath.c_str(),
					dwCreationDisposition, 
					FILE_SHARE_READ | FILE_SHARE_WRITE, 
					NULL,
					dwDesiredAccess, 
					FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
					NULL);

	if(hFile==INVALID_HANDLE_VALUE)
		SS_THROW(L"Cann't open settings file");

	//запоминаем время последнего изменения файла
	GetFileTime(hFile, 0, 0, &m_LastFileTime);

	//определяем размер файла
	LARGE_INTEGER FileSize64;
	FileSize64.LowPart=::GetFileSize(hFile, LPDWORD(&FileSize64.HighPart));

	//читаем содержимое в буфер
	DWORD ReaddenDataLen;
	pSettingsBuffer->resize(FileSize64.LowPart);
	ReadFile(hFile, &pSettingsBuffer->front(), (DWORD)pSettingsBuffer->size(), &ReaddenDataLen, NULL);
	
	//закрываем файл
	CloseHandle(hFile);
	
	//декодируем буфер
	EncodeDecodeBuffer(pSettingsBuffer);

	return true;
}

void CSettingsManager::EncodeDecodeBuffer(std::vector<unsigned char>* pSettingsBuffer)
{
	if(!pSettingsBuffer) return;

	//декодируем буфер
	srand(15);
	for(unsigned int k = 0; k<pSettingsBuffer->size(); k++)
	{
		(*pSettingsBuffer)[k] = ((*pSettingsBuffer)[k] ^ (rand() & 0xff));
	}
}

void CSettingsManager::ResetSettings(void)
{
	//загружаем
	if(!CheckUpToDate())
		SS_THROW(L"CheckUpToDate not succeeded");

	//очищаем
	ClearSettings();
	//пишем на диск
	SaveSettings();
}

bool CSettingsManager::CheckUpToDate(void)
{
	if(!m_bSettingsLoaded){
		LoadSettings();
	}

	//а вот теперь проверяем время файла
	std::wstring wsFilePath;
	MakeSettingFilePath(&wsFilePath);
	HANDLE hFile = CreateFile(wsFilePath.c_str(), 
					GENERIC_READ, 
					FILE_SHARE_READ, 
					NULL, 
					OPEN_EXISTING, 
					FILE_ATTRIBUTE_NORMAL, 
					NULL);

	if (hFile == INVALID_HANDLE_VALUE){
		SS_THROW(L"GSServer: cannot open setting file during checkuptodate");
	}

	//получаем время изменения файла
	FILETIME ftWrite;
	GetFileTime(hFile, 0, 0, &ftWrite);
	CloseHandle(hFile);

	if((ftWrite.dwHighDateTime == m_LastFileTime.dwHighDateTime) && (ftWrite.dwLowDateTime == m_LastFileTime.dwLowDateTime)){
		//если совпало, загружать настройки не надо
		return true;
	}
	
	//очищаем что было
	ClearSettings();
	//грузим настройки по новой
	LoadSettings ();
	return true;
}

//----------------------------------------------------------------//

}
}