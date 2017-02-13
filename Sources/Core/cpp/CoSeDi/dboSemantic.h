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
				namespace Semantic
				{

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/*	семантическая иерархия слов	*/
					
					/// класс для доступа к таблице отражающий семантическую иерархию слов
					class CAccessSemanticIDSourceTree
					{
					protected:
						/*! \var int m_lIDSourceParent
						*  \brief     индекс основы родителя
						*/
						int m_lIDSourceParent;
						/*! \var int m_lIDSourceChild
						*  \brief     индекс основы ребенка
						*/
						int m_lIDSourceChild;
						/*! \var short m_shDictionaryNumberParent
						*  \brief     индекс словаря родителя
						*/
						short m_shDictionaryNumberParent;
						/*! \var short m_shDictionaryNumberChild
						*  \brief     индекс словаря ребенка
						*/
						short m_shDictionaryNumberChild;
					public:
						/// устанавливает индекс основы родителя
						void SetIDSourceParent(int lIDSourceParent)
						{
							m_lIDSourceParent = lIDSourceParent;
						}
						/// устанавливает индекс основы ребенка
						void SetIDSourceChild(int lIDSourceChild)
						{
							m_lIDSourceChild = lIDSourceChild;
						}
						/// устанавливает индекс словаря родителя
						void SetDictionaryNumberParent(short shDictionaryNumberParent)
						{
							m_shDictionaryNumberParent = shDictionaryNumberParent;
						}
						/// устанавливает индекс словаря ребенка
						void SetDictionaryNumberChild(short shDictionaryNumberChild)
						{
							m_shDictionaryNumberChild = shDictionaryNumberChild;
						}
						/// возвращает индекс основы родителя
						int GetIDSourceParent()
						{
							return m_lIDSourceParent;
						}
						/// возвращает индекс основы ребенка
						int GetIDSourceChild()
						{
							return m_lIDSourceChild;
						}
						/// возвращает индекс словаря родителя						
						short GetDictionaryNumberParent()
						{
							return m_shDictionaryNumberParent;
						}
						/// возвращает индекс словаря ребенка
						short GetDictionaryNumberChild()
						{
							return m_shDictionaryNumberChild;
						}
					};

					/// класс для добавления записей в таблицу семантических иерархий слов
					class CISemanticIDSourceTree : public CAccessSemanticIDSourceTree
					{
					public:
						BEGIN_PARAM_MAP(CISemanticIDSourceTree)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(1, m_shDictionaryNumberParent)							
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(2, m_lIDSourceParent)							
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(3, m_shDictionaryNumberChild)							
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(4, m_lIDSourceChild)							
						END_PARAM_MAP()		
					public:
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CISemanticIDSourceTree,
_T("INSERT SemanticIDSourceTree (DictionaryNumberParent, IDSourceParent, DictionaryNumberChild, IDSourceChild) VALUES(?,?,?,?)"));	
					
					};

					/// класс для удаления записей из таблицы семантических иерархий слов
					class CDSemanticIDSourceTree : public CAccessSemanticIDSourceTree
					{
					public:
						BEGIN_PARAM_MAP(CDSemanticIDSourceTree)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(1, m_shDictionaryNumberParent)							
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(2, m_lIDSourceParent)							
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(3, m_shDictionaryNumberChild)							
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(4, m_lIDSourceChild)							
						END_PARAM_MAP()		
					public:
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CDSemanticIDSourceTree,
_T("DELETE SemanticIDSourceTree WHERE DictionaryNumberParent = ? AND IDSourceParent = ? AND DictionaryNumberChild = ? AND IDSourceChild = ?"));	
					
					};

					/// класс для получения записей из таблицы семантических иерархий слов по индексу ребенка
					class CSSemanticIDSourceTree : public CAccessSemanticIDSourceTree
					{
				
					public:
						BEGIN_COLUMN_MAP(CSSemanticIDSourceTree)
							COLUMN_ENTRY(1, m_shDictionaryNumberParent)		
							COLUMN_ENTRY(2, m_lIDSourceParent)	
							COLUMN_ENTRY(3, m_shDictionaryNumberChild)	
							COLUMN_ENTRY(4, m_lIDSourceChild)								
						END_COLUMN_MAP()
					public:
						BEGIN_PARAM_MAP(CSSemanticIDSourceTree)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(1, m_shDictionaryNumberChild)							
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(2, m_lIDSourceChild)							
						END_PARAM_MAP()		
					public:
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CSSemanticIDSourceTree,
_T("SELECT DictionaryNumberParent, IDSourceParent, DictionaryNumberChild, IDSourceChild FROM SemanticIDSourceTree WHERE DictionaryNumberChild = ? AND IDSourceChild = ?"));	
					
					};

					/// класс для получения записей из таблицы семантических иерархий слов
					class CSAllSemanticIDSourceTree : public CAccessSemanticIDSourceTree
					{
				
					public:
						BEGIN_COLUMN_MAP(CSAllSemanticIDSourceTree)
							COLUMN_ENTRY(1, m_shDictionaryNumberParent)		
							COLUMN_ENTRY(2, m_lIDSourceParent)	
							COLUMN_ENTRY(3, m_shDictionaryNumberChild)	
							COLUMN_ENTRY(4, m_lIDSourceChild)								
						END_COLUMN_MAP()
					public:
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CSAllSemanticIDSourceTree,
		_T("SELECT DictionaryNumberParent, IDSourceParent, DictionaryNumberChild, IDSourceChild FROM SemanticIDSourceTree"));	
					
					};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
					/*	семантическая информация о каждом слове	*/
				
				/// класс описывающий поля таблицы Semantics и используемый для доступа ее полям
				class CAccessorTSemantics 
				{
				protected:	
					/*! \var unsigned short	m_ushDictionaryNumber
					*  \brief     номер словаря
					*/
					unsigned short	m_ushDictionaryNumber;
					/*! \var unsigned short	m_ushDictionaryNumber
					*  \brief     индекс основы слова
					*/
					unsigned int	m_uiIDSource;
					/*! \var unsigned short	m_ushDictionaryNumber
					*  \brief     индекс семантического класса слова
					*/
					int	m_iClass;									
				public:	
					/// возвращает индекс основы слова
					unsigned int	GetIDSource()
					{
						return m_uiIDSource;
					}
					/// устанавливает индекс основы слова
					void	SetIDSource(unsigned int uiIDSource)
					{
						m_uiIDSource = uiIDSource;
					}	
					/// возвращает индекс словаря слова
					unsigned short	GetDictionaryNumber()
					{
						return m_ushDictionaryNumber;
					}
					/// устанавливает индекс словаря слова
					void	SetDictionaryNumber(unsigned short ushDictionaryNumber)
					{
						m_ushDictionaryNumber = ushDictionaryNumber;
					}
					/// возвращает индекс семантического класса слова
					int	GetClass()
					{
						return m_iClass;
					}
					/// устанавливает индекс семантического класса слова
					void	SetClass(unsigned int iClass)
					{
						m_iClass = iClass;
					}					
				};

				/// класс для удаления записи из таблицы Semantics по индексу слова
				class CAccessDTSemantics : public CAccessorTSemantics
				{
				public:
					BEGIN_PARAM_MAP(CAccessDTSemantics)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)
						COLUMN_ENTRY(1, m_iClass)										
						SET_PARAM_TYPE(DBPARAMIO_INPUT)
						COLUMN_ENTRY(2, m_ushDictionaryNumber)										
						SET_PARAM_TYPE(DBPARAMIO_INPUT)
						COLUMN_ENTRY(3, m_uiIDSource)																						
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessDTSemantics, 
						_T("DELETE FROM Semantics WHERE SemanticClass = ? AND DictionaryNumber = ? AND IDSource = ?"));	
				};
				
				/// класс для добавления записи в таблицу Semantics 
				class CAccessITSemanticsAddWord : public CAccessorTSemantics
				{
				public:
					BEGIN_PARAM_MAP(CAccessITSemanticsAddWord)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(1, m_iClass)																
						SET_PARAM_TYPE(DBPARAMIO_INPUT)
						COLUMN_ENTRY(2, m_ushDictionaryNumber)																
						SET_PARAM_TYPE(DBPARAMIO_INPUT)
						COLUMN_ENTRY(3, m_uiIDSource)																						
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessITSemanticsAddWord,
						_T("INSERT Semantics (SemanticClass, DictionaryNumber, IDSource) VALUES (?,?,?)"));	
				};

				/// класс для получения записи из таблицы Semantics по индексу семантического класса
				class CAccessSTSemanticsGetByClass : public CAccessorTSemantics
				{
				public:
					BEGIN_COLUMN_MAP(CAccessSTSemanticsGetByClass)
						COLUMN_ENTRY(1, m_iClass)										
						COLUMN_ENTRY(2, m_ushDictionaryNumber)										
						COLUMN_ENTRY(3, m_uiIDSource)																
					END_COLUMN_MAP()
				public:
					BEGIN_PARAM_MAP(CAccessSTSemanticsGetByClass)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(1, m_iClass)																
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessSTSemanticsGetByClass, 
						_T("SELECT SemanticClass, DictionaryNumber, IDSource FROM Semantics WHERE SemanticClass = ?"));	
				};

				/// класс для получения записи из таблицы Semantics по индексу слова				
				class CAccessSTSemanticsGetByIDSource : public CAccessorTSemantics
				{
				public:
					BEGIN_COLUMN_MAP(CAccessSTSemanticsGetByIDSource)
						COLUMN_ENTRY(1, m_iClass)										
						COLUMN_ENTRY(2, m_ushDictionaryNumber)										
						COLUMN_ENTRY(3, m_uiIDSource)																
					END_COLUMN_MAP()
				public:
					BEGIN_PARAM_MAP(CAccessSTSemanticsGetByIDSource)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(1, m_ushDictionaryNumber)																
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(2, m_uiIDSource)																						
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessSTSemanticsGetByIDSource,
						_T("SELECT SemanticClass, DictionaryNumber, IDSource FROM Semantics WHERE DictionaryNumber = ? AND IDSource = ?"));	
				};

				/// класс для получения записей из таблицы Semantics 				
				class CAccessSFullTSemantics : public CAccessorTSemantics
				{
				public:
					BEGIN_COLUMN_MAP(CAccessSFullTSemantics)
						COLUMN_ENTRY(1, m_iClass)										
						COLUMN_ENTRY(2, m_ushDictionaryNumber)										
						COLUMN_ENTRY(3, m_uiIDSource)																										
					END_COLUMN_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessSFullTSemantics,
						_T("SELECT SemanticClass, DictionaryNumber, IDSource  FROM Semantics"));	
				};

				/// класс для получения записей из таблицы SemanticsExtra 				
				class CAccessSFullTSemanticsExtra : public CAccessorTSemantics
				{
				public:
					BEGIN_COLUMN_MAP(CAccessSFullTSemanticsExtra)
						COLUMN_ENTRY(1, m_iClass)										
						COLUMN_ENTRY(2, m_ushDictionaryNumber)										
						COLUMN_ENTRY(3, m_uiIDSource)																										
					END_COLUMN_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessSFullTSemanticsExtra,
						_T("SELECT SemanticClass, DictionaryNumber, IDSource  FROM SemanticsExtra"));	
				};

	/*************************************************/

				/// класс для удаления записи из таблицы Semantics по индексу слова
				class CAccessDTSemanticsSynonims : public CAccessorTSemantics
				{
				public:
					BEGIN_PARAM_MAP(CAccessDTSemanticsSynonims)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)
						COLUMN_ENTRY(1, m_iClass)										
						SET_PARAM_TYPE(DBPARAMIO_INPUT)
						COLUMN_ENTRY(2, m_ushDictionaryNumber)										
						SET_PARAM_TYPE(DBPARAMIO_INPUT)
						COLUMN_ENTRY(3, m_uiIDSource)																						
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessDTSemanticsSynonims, 
						_T("DELETE FROM SemanticsSynonims WHERE SemanticClass = ? AND DictionaryNumber = ? AND IDSource = ?"));	
				};
				
				/// класс для добавления записи в таблицу Semantics 
				class CAccessITSemanticsSynonimsAddWord : public CAccessorTSemantics
				{
				public:
					BEGIN_PARAM_MAP(CAccessITSemanticsSynonimsAddWord)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(1, m_iClass)																
						SET_PARAM_TYPE(DBPARAMIO_INPUT)
						COLUMN_ENTRY(2, m_ushDictionaryNumber)																
						SET_PARAM_TYPE(DBPARAMIO_INPUT)
						COLUMN_ENTRY(3, m_uiIDSource)																						
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessITSemanticsSynonimsAddWord,
						_T("INSERT SemanticsSynonims (SemanticClass, DictionaryNumber, IDSource) VALUES (?,?,?)"));	
				};

				/// класс для получения записи из таблицы Semantics по индексу семантического класса
				class CAccessSTSemanticsSynonimsGetByClass : public CAccessorTSemantics
				{
				public:
					BEGIN_COLUMN_MAP(CAccessSTSemanticsSynonimsGetByClass)
						COLUMN_ENTRY(1, m_iClass)										
						COLUMN_ENTRY(2, m_ushDictionaryNumber)										
						COLUMN_ENTRY(3, m_uiIDSource)																
					END_COLUMN_MAP()
				public:
					BEGIN_PARAM_MAP(CAccessSTSemanticsSynonimsGetByClass)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(1, m_iClass)																
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessSTSemanticsSynonimsGetByClass, 
						_T("SELECT SemanticClass, DictionaryNumber, IDSource FROM SemanticsSynonims WHERE SemanticClass = ?"));	
				};

				/// класс для получения записи из таблицы Semantics по индексу слова				
				class CAccessSTSemanticsSynonimsGetByIDSource : public CAccessorTSemantics
				{
				public:
					BEGIN_COLUMN_MAP(CAccessSTSemanticsSynonimsGetByIDSource)
						COLUMN_ENTRY(1, m_iClass)										
						COLUMN_ENTRY(2, m_ushDictionaryNumber)										
						COLUMN_ENTRY(3, m_uiIDSource)																
					END_COLUMN_MAP()
				public:
					BEGIN_PARAM_MAP(CAccessSTSemanticsSynonimsGetByIDSource)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(1, m_ushDictionaryNumber)																
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(2, m_uiIDSource)																						
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessSTSemanticsSynonimsGetByIDSource,
						_T("SELECT SemanticClass, DictionaryNumber, IDSource FROM SemanticsSynonims WHERE DictionaryNumber = ? AND IDSource = ?"));	
				};

				/// класс для получения записей из таблицы Semantics 				
				class CAccessSFullTSemanticsSynonims : public CAccessorTSemantics
				{
				public:
					BEGIN_COLUMN_MAP(CAccessSFullTSemanticsSynonims)
						COLUMN_ENTRY(1, m_iClass)										
						COLUMN_ENTRY(2, m_ushDictionaryNumber)										
						COLUMN_ENTRY(3, m_uiIDSource)																										
					END_COLUMN_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessSFullTSemanticsSynonims,
						_T("SELECT SemanticClass, DictionaryNumber, IDSource  FROM SemanticsSynonims"));	
				};

/////////////////////////////////////////////////////////////////////////////////////////
/* Работа с деревом семантических классов */

				/// класс описывающий поля таблицы	SemanticClassesTree и используемый для получения данных из нее			
				class CAccessorTSemanticsTree 
				{
				protected:	

					/*! \var unsigned int	m_ushIDClass
					*  \brief     индекс семантического класса
					*/
					unsigned int	m_ushIDClass;

					/*! \var unsigned int	m_ushIDParent
					*  \brief   индекс класса, являющегося родителем для текущего
					*/
					unsigned int	m_ushIDParent;

					/*! \var char				m_chName
					*  \brief     имя семантического класса
					*/
					wchar_t				m_chName[128];				

					/*! \var char				m_chShortName
					*  \brief     короткое имя семантического класса
					*/
					wchar_t				m_chShortName[128];				

					/*! \var unsigned int				m_uiColor
					*  \brief     цвет  семантического класса
					*/
					unsigned int 		m_uiColor;				

				public:	
					/// возвращает индекс класса
					unsigned int	GetIDClass()
					{
						return m_ushIDClass;
					}
					/// устанавливает индекс класса
					void	SetIDClass(unsigned int ushIDClass)
					{
						m_ushIDClass = ushIDClass;
					}	
					/// возвращает индекс класса, являющегося родителем для данного
					unsigned int	GetIDParent()
					{
						return m_ushIDParent;
					}
					/// устанавливает индекс класса, являющегося родителем для данного
					void	SetIDParent(unsigned int ushIDParent)
					{
						m_ushIDParent = ushIDParent;
					}	
					/// возвращает имя семантического класса
					wchar_t*	GetName()
					{
						return m_chName;
					}
					/// устанавливает имя семантического класса
					void	SetName(const wchar_t * chName)
					{
						wcscpy(m_chName, chName);	
						return;
					}
					/// возвращает короткое имя семантического класса
					wchar_t*	GetShortName()
					{
						return m_chShortName;
					}
					/// возвращает цвет семантического класса
					unsigned int	GetColor()
					{
						return m_uiColor;
					}
				};


				/// класс для поиска по имени семантического класса
				class CAccessTSemanticsTreeByName : public CAccessorTSemanticsTree
				{
				public:
					BEGIN_COLUMN_MAP(CAccessTSemanticsTreeByName)
						COLUMN_ENTRY(1, m_ushIDClass)										
						COLUMN_ENTRY(2, m_ushIDParent)										
						COLUMN_ENTRY(3, m_chName)										
					END_COLUMN_MAP()
				public:
					BEGIN_PARAM_MAP(CAccessTSemanticsTreeByName)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(1, m_chName)																						
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessTSemanticsTreeByName,
						_T("SELECT * FROM SemanticClassesTree WHERE Name = ?"));	
				};


				/// класс для поиска по индексу семантического класса
				class CAccessTSemanticsTreeByClass : public CAccessorTSemanticsTree
				{
				public:
					BEGIN_COLUMN_MAP(CAccessTSemanticsTreeByClass)
						COLUMN_ENTRY(1, m_ushIDClass)										
						COLUMN_ENTRY(2, m_ushIDParent)										
						COLUMN_ENTRY(3, m_chName)										
					END_COLUMN_MAP()
				public:
					BEGIN_PARAM_MAP(CAccessTSemanticsTreeByClass)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(1, m_ushIDClass)																						
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessTSemanticsTreeByClass,
						_T("SELECT * FROM SemanticClassesTree WHERE IDClass = ?"));	
				};

				/// класс для поиска семантического класса	по индексу родителя  
				class CAccessTSemanticsTreeByParent : public CAccessorTSemanticsTree
				{
				public:
					BEGIN_COLUMN_MAP(CAccessTSemanticsTreeByParent)
						COLUMN_ENTRY(1, m_ushIDClass)										
						COLUMN_ENTRY(2, m_ushIDParent)										
						COLUMN_ENTRY(3, m_chName)										
					END_COLUMN_MAP()
				public:
					BEGIN_PARAM_MAP(CAccessTSemanticsTreeByParent)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(1, m_ushIDParent)																						
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessTSemanticsTreeByClass,
						_T("SELECT * FROM SemanticClassesTree WHERE IDParent = ?"));	
				};

				/// класс для удаления записи из таблицы SemanticClassesTree по индексу семантическог класса
				class CAccessDTSemanticsTree : public CAccessorTSemanticsTree
				{
				public:
					BEGIN_PARAM_MAP(CAccessDTSemanticsTree)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(1, m_ushIDClass)																						
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessDTSemanticsTree,
						_T("DELETE FROM SemanticClassesTree WHERE IDClass = ?"));	
				};

				/// класс для добавления информации в таблицу SemanticClassesTree
				class CAccessITSemanticsTreeCreateClass : public CAccessorTSemanticsTree
				{
				public:
					BEGIN_PARAM_MAP(CAccessITSemanticsTreeCreateClass)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(1, m_ushIDClass)	
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(2, m_ushIDParent)	
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(3, m_chName)	
					END_PARAM_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessITSemanticsTreeCreateClass,
						_T("INSERT INTO SemanticClassesTree VALUES( ?, ?, ?)"));	
				};

				/// класс для получения из SemanticClassesTree максимального индекса семантического класса
				class CAccessSTSemanticsTreeGetMaxID : public CAccessorTSemanticsTree
				{
				public:
					BEGIN_COLUMN_MAP(CAccessSTSemanticsTreeGetMaxID)
						COLUMN_ENTRY(1, m_ushIDClass)																									
					END_COLUMN_MAP()				
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessSTSemanticsTreeGetMaxID,
						_T("SELECT MAX(IDClass) FROM SemanticClassesTree HAVING count(*) > 0"));	
				};

				/// класс для обновления информации в таблице SemanticClassesTree
				class CAccessUTSemanticsTreeName : public CAccessorTSemanticsTree
				{
				public:
					BEGIN_PARAM_MAP(CAccessUTSemanticsTreeName)
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(1, m_chName)	
						SET_PARAM_TYPE(DBPARAMIO_INPUT)						
						COLUMN_ENTRY(2, m_ushIDClass)						
					END_PARAM_MAP()		
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessUTSemanticsTreeName,
						_T("UPDATE SemanticClassesTree SET Name = ? WHERE IDClass = ?"));	
				};

				/// класс для получения полной информации из таблицы SemanticClassesTree
				class CAccessSFullTSemanticsTree : public CAccessorTSemanticsTree
				{
				public:
					BEGIN_COLUMN_MAP(CAccessSFullTSemanticsTree)
						COLUMN_ENTRY(1, m_ushIDClass)										
						COLUMN_ENTRY(2, m_ushIDParent)										
						COLUMN_ENTRY(3, m_chName)
						COLUMN_ENTRY(4, m_chShortName)
						COLUMN_ENTRY(5, m_uiColor)
					END_COLUMN_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessSFullTSemanticsTree,
						_T("SELECT * FROM SemanticClassesTree"));	
				};

/////////////////////////////////////////////////////////////////////////////////////////
/* Работа со словами семантических классов */

				/// класс описывающий поля таблицы	SemanticClassesWords и используемый для получения данных из нее			
				class CAccessorTSemanticsWords 
				{
				protected:	

					unsigned int	m_iIDSource;
					/*! \var unsigned int m_iIDClass
					*  \brief     индекс основы
					*/

					/*! \var unsigned int	m_ushIDClass
					*  \brief     индекс семантического класса
					*/
					unsigned int	m_iIDClass;
					
				public:	

					/// возвращает индекс класса, являющегося родителем для данного
					unsigned int	GetIDSource()
					{
						return m_iIDSource;
					}
					/// устанавливает индекс класса, являющегося родителем для данного
					void	SetIDSource(unsigned int iIDSource)
					{
						m_iIDSource = iIDSource;
					}	

					/// возвращает индекс класса
					unsigned int	GetIDClass()
					{
						return m_iIDClass;
					}
					/// устанавливает индекс класса
					void	SetIDClass(unsigned int iIDClass)
					{
						m_iIDClass = iIDClass;
					}	
				};



				/// класс для получения полной информации из таблицы SemanticClassesWords
				class CAccessSFullTSemanticsWords : public CAccessorTSemanticsWords
				{
				public:
					BEGIN_COLUMN_MAP(CAccessSFullTSemanticsWords)
						COLUMN_ENTRY(1, m_iIDSource)										
						COLUMN_ENTRY(2, m_iIDClass)										
					END_COLUMN_MAP()
				public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
					DEFINE_COMMAND(CAccessSFullTSemanticsWords,
						_T("SELECT * FROM SemanticClassesWords"));	
				};

/////////////////////////////////////////////////////////////////////////////////////////
/*		Шаблоны для доступа к данным		*/
////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////				
					
					/*	семантическа слов в отдельности	*/
					
					/// удаление из таблицы 
					typedef CAccessor<CAccessDTSemanticsSynonims> CAccessorDTSemanticsSynonims;
					typedef CCommand<CAccessorDTSemanticsSynonims> CCommandDTSemanticsSynonims;
					typedef CPreparedCommand<CAccessorDTSemanticsSynonims> CPreparedDTSemanticsSynonims;
					
					/// вставка в таблицу
					typedef CAccessor<CAccessITSemanticsSynonimsAddWord> CAccessorITSemanticsSynonimsAddWord;
					typedef CCommand<CAccessorITSemanticsSynonimsAddWord> CCommandITSemanticsSynonimsAddWord;
					typedef CPreparedCommand<CAccessorITSemanticsSynonimsAddWord> CPreparedITSemanticsSynonimsAddWord;

					/// выбор информации по классу
					typedef CAccessor<CAccessSTSemanticsSynonimsGetByClass> CAccessorSTSemanticsSynonimsGetByClass;
					typedef CCommand<CAccessorSTSemanticsSynonimsGetByClass> CCommandSTSemanticsSynonimsGetByClass;
					typedef CPreparedCommand<CAccessorSTSemanticsSynonimsGetByClass> CPreparedSTSemanticsSynonimsGetByClass;

					/// выбор информации по основе
					typedef CAccessor<CAccessSTSemanticsSynonimsGetByIDSource> CAccessorSTSemanticsSynonimsGetByIDSource;
					typedef CCommand<CAccessorSTSemanticsSynonimsGetByIDSource> CCommandSTSemanticsSynonimsGetByIDSource;
					typedef CPreparedCommand<CAccessorSTSemanticsSynonimsGetByIDSource> CPreparedSTSemanticsSynonimsGetByIDSource;

					/// выбор всей информации
					typedef CAccessor<CAccessSFullTSemanticsSynonims> CAccessorSFullTSemanticsSynonims;
					typedef CCommand<CAccessorSFullTSemanticsSynonims> CCommandSFullTSemanticsSynonims;
					typedef CPreparedCommand<CAccessorSFullTSemanticsSynonims> CPreparedSFullTSemanticsSynonims;

///////////////////////////////////////////////////////////////////////////////////////				
					
					/*	семантическа слов в отдельности	*/
					
					/// удаление из таблицы 
					typedef CAccessor<CAccessDTSemantics> CAccessorDTSemantics;
					typedef CCommand<CAccessorDTSemantics> CCommandDTSemantics;
					typedef CPreparedCommand<CAccessorDTSemantics> CPreparedDTSemantics;
					
					/// вставка в таблицу
					typedef CAccessor<CAccessITSemanticsAddWord> CAccessorITSemanticsAddWord;
					typedef CCommand<CAccessorITSemanticsAddWord> CCommandITSemanticsAddWord;
					typedef CPreparedCommand<CAccessorITSemanticsAddWord> CPreparedITSemanticsAddWord;

					/// выбор информации по классу
					typedef CAccessor<CAccessSTSemanticsGetByClass> CAccessorSTSemanticsGetByClass;
					typedef CCommand<CAccessorSTSemanticsGetByClass> CCommandSTSemanticsGetByClass;
					typedef CPreparedCommand<CAccessorSTSemanticsGetByClass> CPreparedSTSemanticsGetByClass;

					/// выбор информации по основе
					typedef CAccessor<CAccessSTSemanticsGetByIDSource> CAccessorSTSemanticsGetByIDSource;
					typedef CCommand<CAccessorSTSemanticsGetByIDSource> CCommandSTSemanticsGetByIDSource;
					typedef CPreparedCommand<CAccessorSTSemanticsGetByIDSource> CPreparedSTSemanticsGetByIDSource;

					/// выбор всей информации
					typedef CAccessor<CAccessSFullTSemantics> CAccessorSFullTSemantics;
					typedef CCommand<CAccessorSFullTSemantics> CCommandSFullTSemantics;
					typedef CPreparedCommand<CAccessorSFullTSemantics> CPreparedSFullTSemantics;

					/// выбор всей информации
					typedef CAccessor<CAccessSFullTSemanticsExtra> CAccessorSFullTSemanticsExtra;
					typedef CCommand<CAccessorSFullTSemanticsExtra> CCommandSFullTSemanticsExtra;
					typedef CPreparedCommand<CAccessorSFullTSemanticsExtra> CPreparedSFullTSemanticsExtra;

///////////////////////////////////////////////////////////////////////////////////////
				/*	семантические иерархии слов	*/

				
					typedef CAccessor<CSAllSemanticIDSourceTree> CAccessorSAllSemanticIDSourceTree;
					typedef CCommand<CAccessorSAllSemanticIDSourceTree> CCommandSAllSemanticIDSourceTree;
					typedef CPreparedCommand<CAccessorSAllSemanticIDSourceTree> CPreparedSAllSemanticIDSourceTree;

					typedef CAccessor<CSSemanticIDSourceTree> CAccessorCSSemanticIDSourceTree;
					typedef CCommand<CAccessorCSSemanticIDSourceTree> CCommandCSSemanticIDSourceTree;
					typedef CPreparedCommand<CAccessorCSSemanticIDSourceTree> CPreparedCSSemanticIDSourceTree;

					typedef CAccessor<CDSemanticIDSourceTree> CAccessorCDSemanticIDSourceTree;
					typedef CCommand<CAccessorCDSemanticIDSourceTree> CCommandCDSemanticIDSourceTree;
					typedef CPreparedCommand<CAccessorCDSemanticIDSourceTree> CPreparedCDSemanticIDSourceTree;

					typedef CAccessor<CISemanticIDSourceTree> CAccessorCISemanticIDSourceTree;
					typedef CCommand<CAccessorCISemanticIDSourceTree> CCommandCISemanticIDSourceTree;
					typedef CPreparedCommand<CAccessorCISemanticIDSourceTree> CPreparedCISemanticIDSourceTree;

///////////////////////////////////////////////////////////////////////////////////////

					/*	дерево	семантических классов */

					typedef CAccessor<CAccessUTSemanticsTreeName> CAccessorUTSemanticsTreeName;
					typedef CCommand<CAccessorUTSemanticsTreeName> CCommandUTSemanticsTreeName;
					typedef CPreparedCommand<CAccessorUTSemanticsTreeName> CPreparedUTSemanticsTreeName;

					typedef CAccessor<CAccessSTSemanticsTreeGetMaxID> CAccessorSTSemanticsTreeGetMaxID;
					typedef CCommand<CAccessorSTSemanticsTreeGetMaxID> CCommandSTSemanticsTreeGetMaxID;
					typedef CPreparedCommand<CAccessorSTSemanticsTreeGetMaxID> CPreparedSTSemanticsTreeGetMaxID;

					typedef CAccessor<CAccessTSemanticsTreeByName> CAccessorTSemanticsTreeByName;
					typedef CCommand<CAccessorTSemanticsTreeByName> CCommandTSemanticsTreeByName;
					typedef CPreparedCommand<CAccessorTSemanticsTreeByName> CPreparedTSemanticsTreeByName;

					typedef CAccessor<CAccessTSemanticsTreeByClass> CAccessorTSemanticsTreeByClass;
					typedef CCommand<CAccessorTSemanticsTreeByClass> CCommandTSemanticsTreeByClass;
					typedef CPreparedCommand<CAccessorTSemanticsTreeByClass> CPreparedTSemanticsTreeByClass;

					typedef CAccessor<CAccessTSemanticsTreeByParent> CAccessorTSemanticsTreeByParent;
					typedef CCommand<CAccessorTSemanticsTreeByParent> CCommandTSemanticsTreeByParent;
					typedef CPreparedCommand<CAccessorTSemanticsTreeByParent> CPreparedTSemanticsTreeByParent;

					typedef CAccessor<CAccessDTSemanticsTree> CAccessorDTSemanticsTree;
					typedef CCommand<CAccessorDTSemanticsTree> CCommandDTSemanticsTree;
					typedef CPreparedCommand<CAccessorDTSemanticsTree> CPreparedDTSemanticsTree;

					typedef CAccessor<CAccessITSemanticsTreeCreateClass> CAccessorITSemanticsTreeCreateClass;
					typedef CCommand<CAccessorITSemanticsTreeCreateClass> CCommandITSemanticsTreeCreateClass;
					typedef CPreparedCommand<CAccessorITSemanticsTreeCreateClass> CPreparedITSemanticsTreeCreateClass;		

					typedef CAccessor<CAccessSFullTSemanticsTree> CAccessorSFullTSemanticsTree;
					typedef CCommand<CAccessorSFullTSemanticsTree> CCommandSFullTSemanticsTree;
					typedef CPreparedCommand<CAccessorSFullTSemanticsTree> CPreparedSFullTSemanticsTree;

					typedef CAccessor<CAccessSFullTSemanticsWords> CAccessorSFullTSemanticsWords;
					typedef CCommand<CAccessorSFullTSemanticsWords> CCommandSFullTSemanticsWords;
					typedef CPreparedCommand<CAccessorSFullTSemanticsWords> CPreparedSFullTSemanticsWords;

				};
			};
		};
	};
};

#pragma warning( default : 4995 )