#include "stdafx.h"
#include "SQLManager.h"

#include "connection.h"

namespace SS
{
namespace Core
{
namespace ResourceManagers
{

CSQLManager::CSQLManager()
{
}

CSQLManager::~CSQLManager()
{
	Close();
}

bool CSQLManager::Open(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::IConnection** ptssConnection)
{

	SS_TRY
	{
		if(!ptssConnection)
		{

			SAVE_LOG(SS_ERROR AND L"CSQLManager::Open(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::IConnection** ppConnection), !ppConnection");
			return false;
		};


		std::wstring sPassword, sDataSource;

			SS::Interface::Core::CommonServices::IWorkRegister* pWorkRegister = 
				(SS::Interface::Core::CommonServices::IWorkRegister*) GetLoadManager()->GetInterface(L"./core/LoadManager.dll", CLSID_WorkRegister, IID_WorkRegister);
			sPassword=pWorkRegister->GetParameter(L"Password");
			sDataSource=pWorkRegister->GetParameter(L"DataSource");

		USES_CONVERSION;
		if(!*ptssConnection)
			*ptssConnection=new CConnection();

		if(!(*ptssConnection)->Open(L"sa", const_cast<wchar_t*>(sPassword.c_str()), const_cast<wchar_t*>(BaseType), const_cast<wchar_t*>(sDataSource.c_str())))
		{

			delete (*ptssConnection);
			*ptssConnection=NULL;
			return false;
		}
	}	
	SS_CATCH(L"");
	return true;

	//if(!wcscmp(c_szSearchBase,BaseType))
	//{

	//	//Открываем поисковую базу

	//	if(!(*ptssConnection)->Open(L"sa", const_cast<wchar_t*>(sPassword.c_str()), const_cast<wchar_t*>(GetActiveStorage().c_str()), const_cast<wchar_t*>(sDataSource.c_str())))
	//	{

	//		delete (*ptssConnection);
	//		*ptssConnection=NULL;
	//		return false;
	//	};

	//}else if(!wcscmp(c_szMorphoRus,BaseType))
	//{

	//	if(!(*ptssConnection)->Open(L"sa", const_cast<wchar_t*>(sPassword.c_str()), L"MorphoRussian", const_cast<wchar_t*>(sDataSource.c_str())))
	//	{

	//		delete (*ptssConnection);
	//		*ptssConnection=NULL;
	//		return false;
	//	};
	//}else if(!wcscmp(c_szMorphoEng,BaseType))
	//{
	//	if(!(*ptssConnection)->Open(L"sa", const_cast<wchar_t*>(sPassword.c_str()), L"MorphoEnglish", const_cast<wchar_t*>(sDataSource.c_str())))
	//	{
	//		delete (*ptssConnection);
	//		*ptssConnection=NULL;
	//		return false;
	//	};

	//}else if(!wcscmp(c_szSyntaxConstructionsEng,BaseType))
	//{
	//	if(!(*ptssConnection)->Open(L"sa", const_cast<wchar_t*>(sPassword.c_str()), L"SyntaxConstructionsEnglish", const_cast<wchar_t*>(sDataSource.c_str())))
	//	{
	//		delete (*ptssConnection);
	//		*ptssConnection=NULL;
	//		return false;
	//	};

	//}else if(!wcscmp(c_szSyntaxConstructionsRus,BaseType))
	//{
	//	if(!(*ptssConnection)->Open(L"sa", const_cast<wchar_t*>(sPassword.c_str()), L"SyntaxConstructionsRussian", const_cast<wchar_t*>(sDataSource.c_str())))
	//	{
	//		delete (*ptssConnection);
	//		*ptssConnection=NULL;
	//		return false;
	//	};

	//}else if(!wcscmp(c_szSemanticConstructionsEng,BaseType))
	//{
	//	if(!(*ptssConnection)->Open(L"sa", const_cast<wchar_t*>(sPassword.c_str()), L"SemanticConstructionsEnglish", const_cast<wchar_t*>(sDataSource.c_str())))
	//	{
	//		delete (*ptssConnection);
	//		*ptssConnection=NULL;
	//		return false;
	//	};

	//}else if(!wcscmp(c_szSemanticConstructionsRus,BaseType))
	//{
	//	if(!(*ptssConnection)->Open(L"sa", const_cast<wchar_t*>(sPassword.c_str()), L"SemanticConstructionsRussian", const_cast<wchar_t*>(sDataSource.c_str())))
	//	{
	//		delete (*ptssConnection);
	//		*ptssConnection=NULL;
	//		return false;
	//	};

	//}else if(!wcscmp(c_szCoSeDiEng,BaseType))
	//{
	//	if(!(*ptssConnection)->Open(L"sa", const_cast<wchar_t*>(sPassword.c_str()), L"CoSeDiEnglish", const_cast<wchar_t*>(sDataSource.c_str())))
	//	{
	//		delete (*ptssConnection);
	//		*ptssConnection=NULL;
	//		return false;
	//	};

	//}else if(!wcscmp(c_szCoSeDiRus,BaseType))
	//{
	//	if(!(*ptssConnection)->Open(L"sa", const_cast<wchar_t*>(sPassword.c_str()), L"CoSeDiRussian", const_cast<wchar_t*>(sDataSource.c_str())))
	//	{
	//		delete (*ptssConnection);
	//		*ptssConnection=NULL;
	//		return false;
	//	};

	//}else if(!wcscmp(c_szAbbreviationEng,BaseType))
	//{
	//	if(!(*ptssConnection)->Open(L"sa", const_cast<wchar_t*>(sPassword.c_str()), L"AbbreviationEnglish", const_cast<wchar_t*>(sDataSource.c_str())))
	//	{
	//		delete (*ptssConnection);
	//		*ptssConnection=NULL;
	//		return false;
	//	};

	//}else if(!wcscmp(c_szAbbreviationRus,BaseType))
	//{
	//	if(!(*ptssConnection)->Open(L"sa", const_cast<wchar_t*>(sPassword.c_str()), L"AbbreviationRussian", const_cast<wchar_t*>(sDataSource.c_str())))
	//	{
	//		delete (*ptssConnection);
	//		*ptssConnection=NULL;
	//		return false;
	//	};

	//}
	//else
	//{
	//	delete (*ptssConnection);
	//	*ptssConnection=NULL;
	//	return false;
	//};	
};

void CSQLManager::Close(SS::Core::ResourceManagers::Types::THMConnections::iterator it_Connection)
{

	if(!it_Connection->second.empty())			
	{

			//закрываем субсоединения
			SS::Core::ResourceManagers::Types::THMSubConnections::iterator it_SubConnections=m_hmSubConnections.find(it_Connection->first.c_str());

			if(it_SubConnections!=m_hmSubConnections.end())
			{

				SS::Core::ResourceManagers::Types::TLSubConnections::iterator it_SubConnection=it_SubConnections->second.begin();
				while(it_SubConnection!=it_SubConnections->second.end())
				{

					if(*it_SubConnection)
						(*it_SubConnection)->SubClose();
					it_SubConnection++;
				};
			};
		if(*it_Connection->second.front())
			(*it_Connection->second.front())->Close();
	};
}

bool CSQLManager::Open(SS::Core::ResourceManagers::Types::THMConnections::iterator it_Connection)
{


	if(!it_Connection->second.empty())			
	{

		if(!Open(it_Connection->first.c_str(), it_Connection->second.front()))
		{
			#ifdef PROXY_ERRORS_ON
				SAVE_LOG(SS_ERROR, "CSQLManager::OpenExistingConnections(), !Open(BaseType,ppConnection)", NULL);
			#endif

			return false;
		}
		else
		{

			//переоткрываем субсоединения
			SS::Core::ResourceManagers::Types::THMSubConnections::iterator it_SubConnections=m_hmSubConnections.find(it_Connection->first.c_str());

			if(it_SubConnections!=m_hmSubConnections.end())
			{

				SS::Core::ResourceManagers::Types::TLSubConnections::iterator it_SubConnection=it_SubConnections->second.begin();
				while(it_SubConnection!=it_SubConnections->second.end())
				{

					if(!(*it_SubConnection)->SubOpen(*it_Connection->second.front()))
					{
						SAVE_LOG(SS_ERROR AND L"CSQLManager::OpenExistingConnections(), !(*it_SubConnection)->Open");
						return false;
					};

					it_SubConnection++;
				};
			};
		};
	};

	return true;
};

void CSQLManager::Close()
{

	SS::Core::ResourceManagers::Types::THMConnections::iterator it_Connection=m_hmConnections.begin();
	while(it_Connection!=m_hmConnections.end())
	{

		Close(it_Connection);
		it_Connection++;
	};
}

void CSQLManager::Close(const wchar_t* BaseType)
{
	SS::Core::ResourceManagers::Types::THMConnections::iterator it_Connection=m_hmConnections.find(BaseType);
	if(it_Connection!=m_hmConnections.end())
		Close(it_Connection);
}

bool CSQLManager::Open()
{

	SS::Core::ResourceManagers::Types::THMConnections::iterator it_Connection=m_hmConnections.begin();

	//переоткрываем соединения
	while(it_Connection!=m_hmConnections.end())
	{

		Open(it_Connection);
		it_Connection++;
	};

	return true;
};

bool CSQLManager::Open(const wchar_t* BaseType)
{

	SS::Core::ResourceManagers::Types::THMConnections::iterator it_Connection=m_hmConnections.find(BaseType);
	if(it_Connection!=m_hmConnections.end())
		Open(it_Connection);

	return true;	
};

bool CSQLManager::OpenStorage(const wchar_t* StorageName)
{
	if(GetActiveStorage()!=StorageName)
	{
		Close();
		SetActiveStorage(StorageName);
		Open();
	};
	return true;
}

void CSQLManager::Register
	(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::ISubConnection* pSubConnection)
{

	SS::Core::ResourceManagers::Types::THMSubConnections::iterator it_SubConnections=m_hmSubConnections.find(BaseType);;

	if(it_SubConnections==m_hmSubConnections.end())
	{

		m_hmSubConnections.insert(SS::Core::ResourceManagers::Types::THMSubConnections::value_type(BaseType, SS::Core::ResourceManagers::Types::TLSubConnections(1, pSubConnection)));
		return;
	};

	it_SubConnections->second.push_back(pSubConnection);
}

void CSQLManager::UnRegister
	(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::ISubConnection* pSubConnection)
{

	SS::Core::ResourceManagers::Types::THMSubConnections::iterator it_SubConnections=m_hmSubConnections.find(BaseType);;

	if(it_SubConnections!=m_hmSubConnections.end())
	{

		SS::Core::ResourceManagers::Types::TLSubConnections::iterator it_curr=find(it_SubConnections->second.begin(), it_SubConnections->second.end(), pSubConnection);

		if(it_curr!=it_SubConnections->second.end())
			it_SubConnections->second.erase(it_curr);
	};
}

void CSQLManager::CreateOpenAndRegister
	(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::IConnection** ptssConnection)
{

	CBaseConnectionsManager<SS::Interface::Core::ResourceManagers::IConnection>::CreateOpenAndRegister(BaseType,ptssConnection,m_hmConnections);
}

void CSQLManager::UnRegisterDeleteAndClose(const wchar_t* BaseType, SS::Interface::Core::ResourceManagers::IConnection** ptssConnection)
{

	CBaseConnectionsManager<SS::Interface::Core::ResourceManagers::IConnection>::UnRegisterDeleteAndClose(BaseType,ptssConnection,m_hmConnections);
};

}
}
}