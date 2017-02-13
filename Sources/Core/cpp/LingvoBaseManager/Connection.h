#pragma once
#include "..\ASSInterface\ILingvoBaseManager.h"
#include <atldbcli.h>


namespace SS
{
namespace Core
{
namespace ResourceManagers
{

	/// управл€ет соединением с SQL-базой
	/** обеспечивает хранение и управление соединение с SQL-базой */
	class CConnection :
		public SS::Interface::Core::ResourceManagers::IConnection
	{

		/// true, если соединение открыто
		bool m_IsOpen;

		/// —оединение с базой
		ATL::CDataSource m_currentDB;
		/// —есси€ работы с базой
		ATL::CSession	m_currentSession;

	public:

		CConnection(void);
		virtual ~CConnection(void);

		/// ќткрывает соединение с sql-базой 
		/** 
			\param wszName »м€ пользовател€
			\param wszPassword ѕароль пользовател€
			\param wszLocation Ќазвание базы
			\param wszDataSource Ќазвание источника данных (»м€ SQL-сервера)
			\return bool true, если соединение прошло успешно
			
		ѕопытка открыть соединение с SQL - базой
		*/
		bool Open(wchar_t* wszName, wchar_t* wszPassword, wchar_t* wszLocation, wchar_t* wszDataSource);

		/// «акрывает соединение 
		/** 
			
		ќбеспечивает закрытие соединени€ с SQL-базой 
		*/
		void Close();

		/// ѕровер€ет, открыто ли соединение
		/** 
			\return bool если true, то соединение открыто
			
		ѕровер€ет, открыто ли соединение с SQL-базой 
		*/
		bool IsOpen();

		/// ¬озвращает сессию соединени€ 
		/** 
			\return ATL::CSession* сесси€ соединени€
		
		*/
		ATL::CSession* GetSession();

		/// ”ничтожает соединение 
		/** 
			\return ULONG смысловой нагрузки не несет
			
		просто delete this выполн€етс€ 
		*/
		ULONG Release(){delete this;return 0;};

		/// ѕереоткрывает сессию текущего соединени€ 
		/** 
			\return bool true, если переоткрытие прошло успешно
			
		ƒл€ переоткрыти€ используетс€ текущее соединение
		*/
		bool ReOpenSession();

		/// ¬озвращает число ссылок на сессию 
		/** 
			\return int число ссылок на сессию
			
		»спользуетс€ дл€ контрол€ (пока число ссылок не равно нулю, закрыть сессию не удастс€)
		*/
		int CountSessionRef();
	};  
}
}
}
