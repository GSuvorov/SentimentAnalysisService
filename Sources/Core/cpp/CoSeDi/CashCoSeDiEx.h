#pragma once

#include "..\ASSInterface\ICoSeDi.h"
#include ".\CashCoSeDi.h"


namespace SS
{
	namespace CoSeDi
	{
		namespace SQL
		{
			class CCashCoSeDiEx : 
				public SS::Interface::Core::CoSeDi::ICoSeDiLoad,
				public SS::Core::CommonServices::CBaseCoreClass
			{
			public:
				CCashCoSeDiEx(void);
				~CCashCoSeDiEx(void);
			public: // function
				/// устанавливает указатель на актиный менеджер соединений
				/** 
					\param pLoadManager - указатель на актиный менеджер соединений
					\return void
				*/
				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
				{
					SS_TRY
					{
						SS::Core::CommonServices::CBaseCoreClass::SetLoadManager( pLoadManager);

						m_oCashCoSeDiRus.SetLoadManager( pLoadManager);
						m_oCashCoSeDiEng.SetLoadManager( pLoadManager);
					}
					SS_CATCH(L"")
				}
			public:	//	from ILoad
				/// очищает словарь
				bool Clear();
				///	начальная загрузка	
				bool Load();
				///	перезагрузка	
				bool ReLoad();
				///	преобразование к бинарному виду	
				bool ToBinary();
			public:
				SS::CoSeDi::SQL::CCashCoSeDi m_oCashCoSeDiRus;
				SS::CoSeDi::SQL::CCashCoSeDi m_oCashCoSeDiEng;
			};
		};
		namespace DBMS
		{
			class CCashCoSeDiEx : 
				public SS::Interface::Core::CoSeDi::ICoSeDiLoad,
				public SS::Core::CommonServices::CBaseCoreClass
			{
			public:
				CCashCoSeDiEx(void);
				~CCashCoSeDiEx(void);
			public: // function
				/// устанавливает указатель на актиный менеджер соединений
				/** 
					\param pLoadManager - указатель на актиный менеджер соединений
					\return void
				*/
				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
				{
					SS_TRY
					{
						SS::Core::CommonServices::CBaseCoreClass::SetLoadManager( pLoadManager);

						m_oCashCoSeDiRus.SetLoadManager( pLoadManager);
						m_oCashCoSeDiEng.SetLoadManager( pLoadManager);
					}
					SS_CATCH(L"")
				}
			public:	//	from ILoad
				/// очищает словарь
				bool Clear();
				///	начальная загрузка	
				bool Load();
				///	перезагрузка	
				bool ReLoad();
				///	преобразование к бинарному виду	
				bool ToBinary();
			public:
				SS::CoSeDi::DBMS::CCashCoSeDi m_oCashCoSeDiRus;
				SS::CoSeDi::DBMS::CCashCoSeDi m_oCashCoSeDiEng;
			};

		};
	};
};