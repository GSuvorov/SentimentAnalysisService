#pragma once

#include <atldbcli.h>

#include "..\ASSInterface\TLibBrokerTypes.h"
#include <sqloledb.h>


#pragma warning( disable : 4995 )

namespace SS
{
	namespace CoSeDi
	{
		namespace SQL
		{
			namespace SQL_ACCESS
			{
				
				class CAccessorTExistence 
				{
				protected:
					unsigned short	m_ushExistence;
					unsigned int	m_uiIDSource;
				public:	
					unsigned short	GetExistence()
					{
						return m_ushExistence;
					}
					void	SetExistence(unsigned int ushExistence)
					{
						m_ushExistence = ushExistence;
					}
					unsigned int	GetIDSource()
					{
						return m_uiIDSource;
					}
					void	SetIDSource(unsigned int uiIDSource)
					{
						m_uiIDSource = uiIDSource;
					}					
				};
		
				class CAccessSTExistence : public CAccessorTExistence
				{
				public:
					BEGIN_COLUMN_MAP(CAccessSTExistence)
						COLUMN_ENTRY(1, m_uiIDSource)
						COLUMN_ENTRY(2, m_ushExistence)						
					END_COLUMN_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessSTExistence, 
						_T("Select * from CoSeExistence"));	

				};
				class CAccessUTExistence : public CAccessorTExistence
				{
				public:	/*	параметры для запроса	*/
					BEGIN_PARAM_MAP(CAccessUTExistence)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)
						COLUMN_ENTRY(1, m_ushExistence)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)
						COLUMN_ENTRY(2, m_uiIDSource)						
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessUTExistence, 
						_T("UPDATE CoSeExistence SET IDExistence = ? WHERE IDRoot = ?"));	

				};



/////////////////////////////////////////////////////////////////////////////////////////
/*		Шаблоны для доступа к данным		*/
////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////


				
				
///////////////////////////////////////////////////////////////////////////////////////
				/*	таблица наличия основы в базе	*/
				typedef CAccessor<CAccessSTExistence> CAccessorSTExistence;
				typedef CCommand<CAccessorSTExistence> CCommandSTExistence;
				typedef CPreparedCommand<CAccessorSTExistence> CPreparedSTExistence;

				typedef CAccessor<CAccessUTExistence> CAccessorUTExistence;
				typedef CCommand<CAccessorUTExistence> CCommandUTExistence;
				typedef CPreparedCommand<CAccessorUTExistence> CPreparedUTExistence;

					
				/*typedef CAccessor<CAccess> CAccessor;
				typedef CCommand<CAccessor> CCommand;
				typedef CPreparedCommand<CAccessor> CPrepared;*/


			};

		};
	};
};

#pragma warning( default : 4995 )