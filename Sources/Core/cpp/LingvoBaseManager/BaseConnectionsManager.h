#pragma once

#include "../ASSInterface/TDictionaryNames.h"

#include ".\consts.h"
#include "..\ASSInterface\TBaseNames.h"
using namespace SS::MANAGER::Constants;

namespace SS
{
namespace Core
{
namespace ResourceManagers
{

	/// –еализует работу с соединением (создание, регистрацию, дерегистрацию и удаление)
	template <class TConnection> class CBaseConnectionsManager 
	{
		std::wstring m_wsNameActiveStorage;
	public:

		void SetActiveStorage(const wchar_t* wszNameActiveStorage)
		{
			m_wsNameActiveStorage = wszNameActiveStorage;
		};

		std::wstring& GetActiveStorage()
		{
			return m_wsNameActiveStorage;
		};
	protected:

		/// ќткрывает заданное соединение определенного типа баз
		/** 
			\param BaseType \see SS::MANAGER::Constants
			\return bool - если true, то база открыта

			„исто абстрактна€ функци€, реализуетс€ к классах наследниках, вызываетс€ из этого класса
		*/
		virtual bool Open(const wchar_t* BaseType, TConnection** ptssConnection) = 0;

		/// ƒерегистраци€ ссылки на соединение
		/** 
			\param BaseType - тип базы, (морфологическа€, поискова€, сервисна€ и т.д.) \see SS::MANAGER::Constants
			\param ptssConnection - интерфейс по работе с базой
			
		≈сли число ссылок равно нулю, соединение закрываетс€ и уничтожаетс€ сущность, работающа€ с ней 
		*/
		template<class TConnections> void UnRegisterDeleteAndClose(const wchar_t* BaseType, TConnection** ptssConnection, TConnections& oConnections)
		{

			SS_TRY
			{
				TConnections::iterator it_Connection=oConnections.find(BaseType);

				if(!ptssConnection)
				{

					SAVE_LOG(SS_ERROR AND L"!ppConnection" AND BaseType AND __WFUNCTION__);
					return;
				};

				if(it_Connection!=oConnections.end())
				{

					if(it_Connection->second.empty())
					{

						SAVE_LOG(SS_ERROR AND L"it_Connections->second.empty()" AND BaseType AND __WFUNCTION__);
						return;
					};
					

					//находим и удал€ем ссылку
					TConnections::value_type::second_type::iterator it_CurrentConnection=find(it_Connection->second.begin(), it_Connection->second.end(), ptssConnection);

					if(it_CurrentConnection!=it_Connection->second.end())
						it_Connection->second.erase(it_CurrentConnection);


					if(it_Connection->second.empty())
					{
						//SAVE_LOG(SS_MESSAGE,"”далили коннект базы",BaseType,NULL);
						(*ptssConnection)->Close();
						//(*ptssConnection)->Release();
						
						*ptssConnection=NULL;
						oConnections.erase(it_Connection);
					};
				};
			}
			SS_CATCH(L"")
		};

		/// —оздание сущности, олицетвор€ющей собой базу определенного типа и возврат нужного интерфейса с регистрацией
		/** 
			\param BaseType - тип базы, (морфологическа€, поискова€, сервисна€ и т.д.) \see SS::MANAGER::Constants
			\param ptssConnection - интерфейс по работе с базой 
			
		—оздаем интерфейс класса (если класс еще не создан, в противном случае используем уже созданный),
		олицетвор€ющего собой базу определенного типа и записываем указатель на него в переданный указатель 
		*/
		template<class TConnections> void CreateOpenAndRegister(const wchar_t* BaseType, TConnection** ptssConnection, TConnections& oConnections)
		{
			SS_TRY
			{
				TConnections::iterator it_Connection=oConnections.find(BaseType);

				if(!ptssConnection)
				{

					SAVE_LOG(SS_ERROR AND L"!ppConnection" AND BaseType AND __WFUNCTION__);
					return;
				};

				if(it_Connection!=oConnections.end())
				{

					if(it_Connection->second.empty())
					{

						SAVE_LOG(SS_ERROR AND L"коннект есть, а переменных дл€ него нет" AND BaseType AND __WFUNCTION__);
						return;
					};
				

					//записываем указатель на внешнюю переменную с целью последующего тайного управлени€		
					TConnections::value_type::second_type::iterator it_CurrentConnection=find(it_Connection->second.begin(), it_Connection->second.end(), ptssConnection);

					if(it_CurrentConnection==it_Connection->second.end())
					{
						it_Connection->second.push_back(ptssConnection);
						*ptssConnection=*it_Connection->second.front();
					};


					if(!(*it_Connection->second.front())->IsOpen())
					{
						if(!Open(BaseType,it_Connection->second.front()))
						{
							SAVE_LOG(SS_ERROR AND L"существующее соединение не открываетс€" AND BaseType AND __WFUNCTION__);

							//удал€ем ссылку
							it_CurrentConnection=find(it_Connection->second.begin(), it_Connection->second.end(), ptssConnection);
							if(it_CurrentConnection!=it_Connection->second.end())
								it_Connection->second.erase(it_CurrentConnection);
						};
					};
				}
				else
				{

					*ptssConnection=NULL;
					if(Open(BaseType,ptssConnection))
						oConnections.insert(TConnections::value_type(BaseType,TConnections::value_type::second_type(1,ptssConnection)));
					else
					{

						SAVE_LOG(SS_ERROR AND L"соединение не открываетс€, Ѕаза-" AND BaseType AND __WFUNCTION__);
					};

				};
			}
			SS_CATCH(L"")
		}
	public:
	};
}
}
}