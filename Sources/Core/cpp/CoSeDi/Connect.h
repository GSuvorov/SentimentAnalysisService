#pragma once

#include "../ASCInterface/IDBMS.h"
#include "../ASSInterface/ILingvoBaseManager.h"
#include "../ASCInterface/IBaseManager.h"


namespace SS
{
	namespace CoSeDi
	{
		namespace SQL
		{
			/// класс отвечающий за соединение с SQL базой
			class CConnect : public SS::Core::CommonServices::CBaseCoreClass
			{

			public:
				///  онструктор
				/** 
					\param szBaseName - им€ базы
				*/
				CConnect(const wchar_t* szBaseName);
				/// деструктор
				virtual ~CConnect(void);
			protected:
				/// »нициализаци€ соединений с базами				
				bool InitConnection();
				/// «акрытие соединений с базами
				void CloseConnection(void);
			
			protected:
				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
				{
					SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);

					m_pLingvoBaseManager = 
						(SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)
						GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);

				};
	
				SS::Interface::Core::ResourceManagers::ILingvoBaseManager *m_pLingvoBaseManager;
				SS::Interface::Core::ResourceManagers::IConnection* m_pConnection;

			protected:
				/*! \var SS::Interface::Core::DBMS::IDataBase * m_pDataBase;
				*  \brief      ласс обеспечивающий соединение с DBMS базой
				*/
				SS::Interface::Core::DBMS::IDataBase * m_pDataBase;
				
				/*! \var char m_szBaseName[100]
				*  \brief     - содержит им€ базы данных
				*/
				wchar_t m_szBaseName[100];
			
			};

		};

		namespace DBMS
		{
			/// класс отвечающий за соединение с DBMS базой
			class CConnect : public SS::Core::CommonServices::CBaseCoreClass
			{
			public:
				///  онструктор
				/** 
					\param szBaseName - им€ базы
				*/
				CConnect(const wchar_t* szBaseName);
				/// деструктор
				virtual ~CConnect(void);
			protected:
				/// »нициализаци€ соединений с базами				
				bool InitConnection(std::wstring& wDataTableName);
				/// «акрытие соединений с базами			
				void CloseConnection(void);
			protected:
				/*! \var SS::Interface::Core::DBMS::IDataBase * m_pDataBase;
				*  \brief      ласс обеспечивающий соединение с DBMS базой
				*/
				SS::Interface::Core::DBMS::IDataBase * m_pDataBase;
				/*! \var char m_szBaseName[100]
				*  \brief     - содержит им€ базы данных
				*/
				wchar_t m_szBaseName[100];

				/*! \var SS::Interface::Core::DBMS::IDataTable * m_pDTable
				*  \brief     ”казатель дл€ работы с DBMS таблицами
				*/
				SS::Interface::Core::DBMS::IDataTable * m_pDTable;

			protected:

				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
				{
					SS::Core::CommonServices::CBaseCoreClass::SetLoadManager(pLoadManager);

					m_pLingvoBaseManager = 
						(SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)
						GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);

				};
	
				SS::Interface::Core::ResourceManagers::ILingvoBaseManager *m_pLingvoBaseManager;


			};

		};
	};
};
