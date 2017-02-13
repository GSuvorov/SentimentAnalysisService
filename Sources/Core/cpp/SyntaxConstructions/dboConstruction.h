#include <atldbcli.h>

#include "..\ASSInterface\TLibBrokerTypes.h"
#include <sqloledb.h>

#pragma warning( disable : 4995 )

namespace SS
{
	namespace SyntaxConstructions
	{
		namespace SQL
		{
			namespace SQL_ACCESS
			{
				namespace ConstructionGroups
				{
					///	класс для доступа к таблице групп конструкций
					class CConstructionGroups
					{
					protected:						
						///Индекс конструкции
						long m_lIDConstruction;
						///Индекс группы
						short m_shIDGroup;
					
					public:
						///Возвращает номер конструкции
						long GetIDConstruction()
						{
							return m_lIDConstruction;
						}
						///Устанавливает номер конструкции
						void SetIDConstruction(long lIDConstruction)
						{
							m_lIDConstruction = lIDConstruction;
						}
						///Возвращает номер группы конструкций
						short GetIDGroup()
						{
							return m_shIDGroup;
						}
						///Устанавливает номер группы конструкций
						void SetIDGroup(short shIDGroup)
						{
							m_shIDGroup = shIDGroup;
						}
					};

					/// класс для удаления конструкций из таблицы
					class CConstructionGroupsD : public CConstructionGroups
					{
					public:
						BEGIN_PARAM_MAP(CConstructionGroupsD)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(1, m_shIDGroup)							
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(2, m_lIDConstruction)							
						END_PARAM_MAP()		
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CConstructionGroupsD,
							_T("DELETE SyntaxConstructionGroups WHERE IDGroup = ? AND IDConstruction = ?"));	
					};

					/// класс для получения всех групп конструкций из таблицы
					class CConstructionGroupsSAll : public CConstructionGroups
					{
					public:
						BEGIN_COLUMN_MAP(CConstructionGroupsSAll)
							COLUMN_ENTRY(1, m_shIDGroup)		
							COLUMN_ENTRY(2, m_lIDConstruction)		
						END_COLUMN_MAP()
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CConstructionGroupsSAll,
							_T("SELECT IDGroup, SyntaxConstructionGroups.IDConstruction FROM SyntaxConstructionGroups, ConstructionsInUse where ConstructionsInUse.IDConstruction = SyntaxConstructionGroups.IDConstruction and Rem > 0 ORDER BY IDGroup, position"));	
					};

					/// класс для добавления группы и конструкции в таблицу
					class CConstructionGroupsI : public CConstructionGroups
					{
					public:
						BEGIN_PARAM_MAP(CConstructionGroupsI)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(1, m_shIDGroup)							
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(2, m_lIDConstruction)							
						END_PARAM_MAP()		
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CConstructionGroupsI,
							_T("INSERT INTO SyntaxConstructionGroups (IDGroup, IDConstruction) VALUES (?,?)"));	
					};

					///Класс для доступа к таблице SyntaxConstructionGroupsType
					class CConstructionGroupsType
					{
					protected:
						///Идентификатор группы
						int m_iIdGroup;
						///Режим разбора
						int m_iAnalyseMode;
						///Является ли группой разрешения омонимиии
						int m_iResolveAmbiguity;

					public:
						///Получение идентификатора
						int GetIdGroup()
						{
							return m_iIdGroup;
						}
						///Получить тип разбора
						int GetAnalyseMode()
						{
							return m_iAnalyseMode;
						}
						///Получение информации о принадлежности к разрешению омонимии
						int GetResolveAmbiguity()
						{
							return m_iResolveAmbiguity;
						}
					};
					
					///Запрос 
					class CConstructionGroupsTypeSAll : public CConstructionGroupsType
					{
					public:
						BEGIN_COLUMN_MAP(CConstructionGroupsTypeSAll)
							COLUMN_ENTRY(1, m_iIdGroup)							
							COLUMN_ENTRY(2, m_iAnalyseMode)							
							COLUMN_ENTRY(3, m_iResolveAmbiguity)	
						END_COLUMN_MAP()		
					
					public:
						///Запрос
						DEFINE_COMMAND(CConstructionGroupsTypeSAll,
							_T("SELECT SyntaxConstructionGroupsType.IDGroup, AnalyseMode, AmbiguityResolve FROM SyntaxConstructionGroupsType, AmbiguityGroups WHERE SyntaxConstructionGroupsType.IDGroup = AmbiguityGroups.IDGroup ORDER BY AnalyseMode, Position") );	
					};



/////////////////////////////////////////////////////////////////////////////////////////
/*		Шаблоны для доступа к данным		*/
////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
					
					typedef CAccessor<CConstructionGroupsD> CAccessorConstructionGroupsD;
					typedef CPreparedCommand<CAccessorConstructionGroupsD> CPreparedConstructionGroupsD;

					typedef CAccessor<CConstructionGroupsSAll> CAccessorConstructionGroupsSAll;
					typedef CPreparedCommand<CAccessorConstructionGroupsSAll> CPreparedConstructionGroupsSAll;

					typedef CAccessor<CConstructionGroupsI> CAccessorConstructionGroupsI;
					typedef CPreparedCommand<CAccessorConstructionGroupsI> CPreparedConstructionGroupsI;

					typedef CAccessor<CConstructionGroupsTypeSAll> CAccessorConstructionGroupsTypeSAll;
					typedef CPreparedCommand<CAccessorConstructionGroupsTypeSAll> CPreparedConstructionGroupsTypeSAll;

				};
			};
		};
	};
};

#pragma warning( default : 4995 )
