#include "stdafx.h"
#include "DBMSManager.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{

CDBMSManager::CDBMSManager() : m_pNDXDBManager(NULL), m_pConstDBManager(NULL), m_wsITDataBasePath(L""), m_wsDataBasePath(L"")
{
	
}

void CDBMSManager::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	SS_TRY
	{
		SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);
		Init();
	}
	SS_CATCH(L"")
}

void CDBMSManager::SetITDataBasePath(const wchar_t * wszITDataBasePath)
{
	SS_TRY
	{
		m_wsITDataBasePath = wszITDataBasePath;

		Init();
	}
	SS_CATCH(L"")
}

void CDBMSManager::Init(void)
{
	SS_TRY
	{
		///*	получаем директорий базы данных	*/
		m_wsDataBasePath = 
			((SS::Interface::Core::CommonServices::IWorkRegister*) 
			GetLoadManager()->GetInterface(L"./core/LoadManager.dll",CLSID_WorkRegister,IID_WorkRegister))->GetParameter(L"PathDataBase");

		/*	получаем все необходимые пути	*/
		std::wstring wsDataBaseInit = m_wsDataBasePath + L"SS_DBMS.ini";
			
		std::wstring wsDataBaseRoot = m_wsDataBasePath.substr(0, m_wsDataBasePath.length()-1);

		/*	осуществляем инициализацию работы с базой данных	*/
		CreateAndInit(wsDataBaseInit, wsDataBaseRoot, &m_pConstDBManager);
	}
	SS_CATCH(L"")
}

CDBMSManager::~CDBMSManager()
{
	if(m_pConstDBManager)   
	{
		m_pConstDBManager->Release();
	}
	if(m_pNDXDBManager)    
	{
		m_pNDXDBManager->Release();
	}
}

void CDBMSManager::CreateAndInit(std::wstring sDataBaseInit, std::wstring sDataBaseRoot, SS::Interface::Core::DBMS::IDBManager** ppDBManager)
{
	SS_TRY
	{
		if(!*ppDBManager )
		{
			GetLoadManager()->Create(L"./core/linguistics/DBMS.dll", CLSID_DBMS, IID_DBMS, (void**)ppDBManager);	
			if((*ppDBManager)->Init(sDataBaseInit.c_str(), sDataBaseRoot.c_str()) != S_OK)
			{
				SAVE_LOG(SS_ERROR AND L"CDBMSManager::CreateAndInit, DBMS::IDBManager is not inited");
			};
		};
	}
	SS_CATCH(L"")

}

bool CDBMSManager::Open(const wchar_t* BaseType, SS::Interface::Core::DBMS::IDataBase** ptssConnection)
{
	SS_TRY
	{
		Init();

		if( (GetActiveStorage() == L"") && (wcscmp(BaseType, c_szNdxBase) == 0)	)
		{
			SS_THROW(L"Не задано активное хранилище, база - " AND BaseType);
		}

		if(!ptssConnection)
		{

			SAVE_LOG(SS_ERROR AND L"CDBMSManager::Open(const wchar_t* , DBMS::IDataBase**), !ppConnection");
			return false;
		};

		if(!m_pConstDBManager)
		{
			SS_THROW(L"DBMS::IDataBase*");
		};
							
		if(!wcscmp(c_szNdxBase,BaseType))
		{

			if(m_wsITDataBasePath == L"")
			{
				SS_THROW(L"Не задан путь к активному хранилищу");
			}

	 		std::wstring sDataBaseInit	= m_wsITDataBasePath + L"Ndx_dbms.ini";
			std::wstring sDataBaseRoot	= m_wsITDataBasePath + GetActiveStorage() + L"\\" ;

			if(!m_pNDXDBManager)
			{
				CreateAndInit(sDataBaseInit, sDataBaseRoot, &m_pNDXDBManager);
			}
			else
			{
				m_pNDXDBManager->Switch(sDataBaseInit.c_str(), sDataBaseRoot.c_str());
			};

			m_pNDXDBManager->MoveToDataBase(_T("DBMS"));	// Переход по имени		
			*ptssConnection = m_pNDXDBManager->OpenDataBase();	

		}else 
		{
			/*std::wstring sDataBaseName;

			if(!wcscmp(c_szMorphoRus,BaseType))
			{
				sDataBaseName = L"MorphoRussian";

			}else if(!wcscmp(c_szMorphoEng,BaseType))
			{
				sDataBaseName = L"MorphoEnglish";
			}else if(!wcscmp(c_szCoSeDiEng,BaseType))
			{
				sDataBaseName = L"CoSeDiEnglish";
			}else if(!wcscmp(c_szCoSeDiRus,BaseType))
			{
				sDataBaseName = L"CoSeDiRussian";
			}else if(!wcscmp(c_szSyntaxConstructionsEng,BaseType))
			{
				sDataBaseName = L"SyntaxConstructionsEnglish";
			}else if(!wcscmp(c_szSyntaxConstructionsRus,BaseType))
			{
				sDataBaseName = L"SyntaxConstructionsRussian";
			}else if(!wcscmp(c_szSemanticConstructionsEng,BaseType))
			{
				sDataBaseName = L"SemanticConstructionsEnglish";
			}else if(!wcscmp(c_szSemanticConstructionsRus,BaseType))
			{
				sDataBaseName = L"SemanticConstructionsRussian";
			}else if(!wcscmp(c_szAbbreviationEng,BaseType))
			{
				sDataBaseName = L"AbbreviationEnglish";
			}else if(!wcscmp(c_szAbbreviationRus,BaseType))
			{
				sDataBaseName = L"AbbreviationRussian";
			}else if(!wcscmp(c_szDownloadInfo,BaseType))
			{
				sDataBaseName = L"DownloadInfo";
			}
			else
				return false;*/

			if(m_pConstDBManager->MoveToDataBase(BaseType) != S_OK) // Переход по имени		
			{
				SS_THROW(L"вызов m_pConstDBManager->MoveToDataBase выполнен не верно. Неправильно задано имя базы. При задании имени базы регистр учитывается. Текущая база :" AND BaseType);
			}
			else
				*ptssConnection = m_pConstDBManager->OpenDataBase();	
		};

		return true;
	}
	SS_CATCH(L"")
}

void CDBMSManager::GetFileNamesFromDirectory(/*in*/std::pair<std::wstring, std::wstring>& DirectoryPaths, /*out*/std::vector< std::pair<std::wstring, std::wstring> >* vPathes)
{
	SS_TRY
	{

		std::pair<std::wstring, std::wstring> TablesPaths;

		WIN32_FIND_DATA FindFileData;
		HANDLE hFindFile;

		hFindFile = FindFirstFile((DirectoryPaths.first + _T("*")).c_str(), &FindFileData);
		if (INVALID_HANDLE_VALUE == hFindFile) 
		{
			return ;
		} 

		do
		{
			if(FILE_ATTRIBUTE_DIRECTORY != FindFileData.dwFileAttributes )
			{
				TablesPaths.first	= DirectoryPaths.first	+ FindFileData.cFileName;
				TablesPaths.second	= DirectoryPaths.second	+ FindFileData.cFileName;
	
				vPathes->push_back(TablesPaths);
			}
		}while(FindNextFile(hFindFile, &FindFileData));
	}
	SS_CATCH(L"")
}


void CDBMSManager::GetDatabaseFilePathes(std::vector< std::pair<std::wstring, std::wstring> >* vPathes, std::wstring& sBaseName)
{
	SS_TRY
	{
		///*	получаем директорий базы данных	*/
		m_wsDataBasePath = 
			((SS::Interface::Core::CommonServices::IWorkRegister*) 
			GetLoadManager()->GetInterface(L"./core/LoadManager.dll",CLSID_WorkRegister,IID_WorkRegister))->GetParameter(L"PathDataBase");

		/*	получаем все необходимые пути	*/
		std::wstring wsDataBaseInit = m_wsDataBasePath + L"SS_DBMS.ini";
			
		std::wstring wsDataBaseRoot = m_wsDataBasePath.substr(0, m_wsDataBasePath.length()-1);

		/*	осуществляем инициализацию работы с базой данных	*/
		CreateAndInit(wsDataBaseInit, wsDataBaseRoot, &m_pConstDBManager);


		/////////////////////////////////////////////////////////////////
		if(S_OK !=	m_pConstDBManager->MoveFirstDataBase())
		{
			SS_THROW(L"Ни одной статической базы данных не найдено");
			return ;
		}

		std::pair<std::wstring, std::wstring> DirectoryPaths;
		std::wstring wRelativelyPath;
		std::wstring wFileName;
		do
		{
			SS::Interface::Core::DBMS::IDataBase * pDataBase = m_pConstDBManager->OpenDataBase();
			if(pDataBase)
			{
				if( S_OK != pDataBase->MoveFirstDataTable())
				{
					std::wstring wMessage = L"В базе данных ";
					wMessage += pDataBase->GetName();
					wMessage += L" Не найдена ни одна таблица";
					SS_THROW((wchar_t*)wMessage.c_str());
				}

				

				wRelativelyPath = pDataBase->GetName();
				wRelativelyPath += L"\\";
				
				do
				{
					SS::Interface::Core::DBMS::IDataTable * pDataTable = pDataBase->OpenDataTable();					
					if(pDataTable)
					{
						DirectoryPaths.first	= m_wsDataBasePath + wRelativelyPath + pDataTable->GetName() + L"\\";
						DirectoryPaths.second	= wRelativelyPath + pDataTable->GetName() + L"\\";

						GetFileNamesFromDirectory(/*in*/DirectoryPaths,/*out*/vPathes);
					}
				}while(S_OK == pDataBase->MoveNextDataTable());
			}
		}while ((S_OK == m_pConstDBManager->MoveNextDataBase()));

	}
	SS_CATCH(L"")
}

bool CDBMSManager::Clear(const wchar_t* BaseType)
{
	SS_TRY
	{
		SS::Core::ResourceManagers::Types::THMDBMSConnections::iterator it_dbmsConnection	= m_hmdbmsConnections.find(BaseType);
		
		if(it_dbmsConnection != m_hmdbmsConnections.end())
		{
			Clear(it_dbmsConnection);
		}

		Open(BaseType);
	}
	SS_CATCH(L"")
}

void CDBMSManager::Clear(SS::Core::ResourceManagers::Types::THMDBMSConnections::iterator it_Connection)
{
	SS_TRY
	{
		if(!it_Connection->second.empty())			
		{
			if(*it_Connection->second.front())
			{
				(*it_Connection->second.front())->Clear();
			}
		};
	}
	SS_CATCH(L"Ошибка при закрытии базы данных " AND it_Connection->first.c_str())
}


void CDBMSManager::Close(SS::Core::ResourceManagers::Types::THMDBMSConnections::iterator it_Connection)
{
	SS_TRY
	{
		if(!it_Connection->second.empty())			
		{
			if(*it_Connection->second.front())
			{		
#ifdef _DEBUG
				std::wstring s = it_Connection->first;
#endif
				SS::Interface::Core::DBMS::IDataBase* pDataBase;
				pDataBase = (*it_Connection->second.front());

				if((pDataBase)&&(pDataBase->IsOpen()))
				{
					pDataBase->Close();
				}
			}
		};
	}
	SS_CATCH(L"Ошибка при закрытии базы данных " AND it_Connection->first.c_str())
}

bool CDBMSManager::Open(SS::Core::ResourceManagers::Types::THMDBMSConnections::iterator it_Connection)
{
	SS_TRY
	{
		if(!it_Connection->second.empty())			
		{
			if(!Open(it_Connection->first.c_str(), it_Connection->second.front()))
			{
				SAVE_LOG(SS_ERROR AND L"CDBMSManager::OpenExistingConnections(), !Open(BaseType,ppConnection)");
				return false;
			}
		}
		return true;
	}
	SS_CATCH(L"")
}

void CDBMSManager::Close()
{
	SS_TRY
	{
		SS::Core::ResourceManagers::Types::THMDBMSConnections::iterator it_dbmsConnection	= m_hmdbmsConnections.begin();
		while(it_dbmsConnection!=m_hmdbmsConnections.end())
		{
			Close(it_dbmsConnection);
			it_dbmsConnection++;
		};
	}
	SS_CATCH(L"")
}

void CDBMSManager::Close(const wchar_t* BaseType)
{

	SS_TRY
	{
		SS::Core::ResourceManagers::Types::THMDBMSConnections::iterator it_dbmsConnection	= m_hmdbmsConnections.find(BaseType);
		if(it_dbmsConnection!=m_hmdbmsConnections.end())
		{
			Close(it_dbmsConnection);
		}
	}
	SS_CATCH(L"")
}

bool CDBMSManager::Open()
{

	SS_TRY
	{
		SS::Core::ResourceManagers::Types::THMDBMSConnections::iterator it_dbmsConnection	= m_hmdbmsConnections.begin();
		//переоткрываем соединения
		while(it_dbmsConnection!=m_hmdbmsConnections.end())
		{
			Open(it_dbmsConnection);
			it_dbmsConnection++;
		}

		return true;
	}
	SS_CATCH(L"")
}

bool CDBMSManager::Open(const wchar_t* BaseType)
{
	SS_TRY
	{
		SS::Core::ResourceManagers::Types::THMDBMSConnections::iterator it_dbmsConnection	= m_hmdbmsConnections.find(BaseType);

		if(it_dbmsConnection != m_hmdbmsConnections.end())
		{
			Open(it_dbmsConnection);
		}

		return true;	
	}
	SS_CATCH(L"")
}


bool CDBMSManager::OpenStorage(const wchar_t* StorageName)
{
	SS_TRY
	{
	//	Close(c_szNdxBase);
		SetActiveStorage(StorageName);
		Open(c_szNdxBase);
		return true;
	}
	SS_CATCH(L"")
}

void CDBMSManager::CreateOpenAndRegister(const wchar_t* BaseType, SS::Interface::Core::DBMS::IDataBase** ptssConnection)
{
	SS_TRY
	{
		CBaseConnectionsManager<SS::Interface::Core::DBMS::IDataBase>::CreateOpenAndRegister(BaseType,ptssConnection,m_hmdbmsConnections);
	}
	SS_CATCH(L"")
}

void CDBMSManager::UnRegisterDeleteAndClose(const wchar_t* BaseType, SS::Interface::Core::DBMS::IDataBase** ptssConnection)
{
	SS_TRY
	{
		CBaseConnectionsManager<SS::Interface::Core::DBMS::IDataBase>::UnRegisterDeleteAndClose(BaseType,ptssConnection,m_hmdbmsConnections);
	}
	SS_CATCH(L"")
}

}
}
}