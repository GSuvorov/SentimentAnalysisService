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
				namespace Combinatory
				{
					/// аксессор для доступа к комбинаторной части ТКС
					class CAccessorTCombinatory 
					{
					protected:	
						/*! \var unsigned int	m_uiID
						*  \brief     индекс комбинации
						*/
						unsigned int	m_uiID;
						/*! \var short			m_shIDType
						*  \brief     номер типа морфообразования
						*/
						short			m_shIDType;
						/*! \var short			m_shIDForm
						*  \brief     индекс словоформы
						*/
						short			m_shIDForm;
						/*! \var unsigned short	m_ushDictionaryNumber
						*  \brief     номер словаря
						*/
						unsigned short	m_ushDictionaryNumber;
						/*! \var unsigned int	m_uiIDSource
						*  \brief     индекс основы
						*/
						unsigned int	m_uiIDSource;
						/*! \var unsigned short	m_ushLevel
						*  \brief     номер слова в комбинации
						*/
						unsigned short	m_ushLevel;
						/*! \var short			m_shCombiType
						*  \brief     тип комбинации
						*/
						short			m_shCombiType;
					public:	
						/// возвращает индекс комбинации
						unsigned int	GetID()
						{
							return m_uiID;
						}
						/// устанавливает индекс комбинации
						void	SetID(unsigned int uiID)
						{
							m_uiID = uiID;
						}
						/// возвращает индекс основы
						unsigned int	GetIDSource()
						{
							return m_uiIDSource;
						}
						/// устанавливает индекс основы
						void	SetIDSource(unsigned int uiIDSource)
						{
							m_uiIDSource = uiIDSource;
						}	
						/// возвращает номер типа морфообразования
						short	GetIDType()
						{
							return m_shIDType;
						}
						/// возвращает номер словоформы
						short	GetIDForm()
						{
							return m_shIDForm;
						}
						/// возвращает тип комбинации
						SS::CoSeDi::DataTypes::ECombiType	GetCombiType()
						{
							return (SS::CoSeDi::DataTypes::ECombiType)m_shCombiType;
						}
						/// устанавливает номер типа морфообразования
						void	SetIDType(short shIDType)
						{
							m_shIDType = shIDType;
						}
						/// устанавливает номер словоформы
						void	SetIDForm(short shIDForm)
						{
							m_shIDForm = shIDForm;
						}
						/// устанавливает тип комбинации
						void	SetCombiType(SS::CoSeDi::DataTypes::ECombiType eCombiType)
						{
							m_shCombiType =(short)eCombiType;
						}
						/// возвращает индес словаря
						unsigned short	GetDictionaryNumber()
						{
							return m_ushDictionaryNumber;
						}
						/// устанавливает индес словаря
						void	SetDictionaryNumber(unsigned short ushDictionaryNumber)
						{
							m_ushDictionaryNumber = ushDictionaryNumber;
						}	
						/// возвращает номер слова в комбинации
						unsigned short	GetLevel()
						{
							return m_ushLevel;
						}
						/// устанавливает номер слова в комбинации
						void	SetLevel(unsigned short ushLevel)
						{
							m_ushLevel = ushLevel;
						}
						
					};					
					
					/// класс для вставки полей в базу
					class CAccessITCombinatory : public CAccessorTCombinatory
					{
					public:	/*	параметры для запроса	*/
						BEGIN_PARAM_MAP(CAccessITCombinatory)
							SET_PARAM_TYPE(DBPARAMIO_INPUT) // 1 //поле ID
							COLUMN_ENTRY(1, m_uiID) 
							SET_PARAM_TYPE(DBPARAMIO_INPUT) // 2 //поле IDType
							COLUMN_ENTRY(2, m_shIDType)
							SET_PARAM_TYPE(DBPARAMIO_INPUT) // 3 //поле IDForm
							COLUMN_ENTRY(3, m_shIDForm)
							SET_PARAM_TYPE(DBPARAMIO_INPUT) // 4 //поле DictionaryNumber
							COLUMN_ENTRY(4, m_ushDictionaryNumber)
							SET_PARAM_TYPE(DBPARAMIO_INPUT) // 5 //поле IDSource
							COLUMN_ENTRY(5, m_uiIDSource)
							SET_PARAM_TYPE(DBPARAMIO_INPUT) // 6 //поле Level
							COLUMN_ENTRY(6, m_ushLevel)
							SET_PARAM_TYPE(DBPARAMIO_INPUT) // 7 //поле CombiType
							COLUMN_ENTRY(7, m_shCombiType)
						END_PARAM_MAP()
					public:	/*	параметры для запроса	*/
					/*	параметрический запрос	*/
						DEFINE_COMMAND(CAccessITCombinatory, 
					_T("INSERT Combinatory (ID, IDType, IDForm, DictionaryNumber, IDSource, Level, CombiType) VALUES (?,?,?,?,?,?,?)"));	
					};

					/// класс для получение максимального ID 
					class CAccessSMaxTCombinatory : public CAccessorTCombinatory
					{
					public:
						BEGIN_COLUMN_MAP(CAccessSMaxTCombinatory)
							COLUMN_ENTRY(1, m_uiID)										
						END_COLUMN_MAP()
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CAccessSMaxTCombinatory, _T("SELECT MAX(ID) FROM Combinatory HAVING count(*) > 0"));	
					};

					/// класс для удаление записи из таблицы
					class CAccessDTCombinatory : public CAccessorTCombinatory
					{
					public:
						BEGIN_PARAM_MAP(CAccessDTCombinatory)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)
							COLUMN_ENTRY(1, m_uiID)										
						END_PARAM_MAP()
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CAccessDTCombinatory, _T("DELETE FROM Combinatory WHERE ID = ?"));	
					};

					/// класс для получения полной информации о комбинации
					class CAccessSTCombinatoryInfo : public CAccessorTCombinatory
					{
					public:
						BEGIN_COLUMN_MAP(CAccessSTCombinatoryInfo)
							COLUMN_ENTRY(1, m_uiID)										
							COLUMN_ENTRY(2, m_shIDType)										
							COLUMN_ENTRY(3, m_shIDForm)										
							COLUMN_ENTRY(4, m_ushDictionaryNumber)										
							COLUMN_ENTRY(5, m_uiIDSource)										
							COLUMN_ENTRY(6, m_ushLevel)										
							COLUMN_ENTRY(7, m_shCombiType)										
						END_COLUMN_MAP()
					public:
						BEGIN_PARAM_MAP(CAccessSTCombinatoryInfo)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)
							COLUMN_ENTRY(1, m_uiID)																
						END_PARAM_MAP()
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CAccessSTCombinatoryInfo,
							_T("SELECT * FROM Combinatory WHERE ID = ? ORDER BY Level"));	
					};

					/// класс для получения всех комбинаций для введенных индексов основ
					class CAccessSAllCombination : public CAccessorTCombinatory
					{
					public:
						BEGIN_COLUMN_MAP(CAccessSAllCombination)
							COLUMN_ENTRY(1, m_uiID)
						END_COLUMN_MAP()
					public:
						BEGIN_PARAM_MAP(CAccessSAllCombination)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)
							COLUMN_ENTRY(1, m_uiIDSource)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)
							COLUMN_ENTRY(2, m_ushDictionaryNumber)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)
							COLUMN_ENTRY(3, m_shIDForm)
						END_PARAM_MAP()
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CAccessSTCombinatoryInfo, 
							_T("SELECT ID FROM Combinatory WHERE IDSource = ? and DictionaryNumber = ? and (IDForm = ? or IDForm = -1)"));	
					};

					/// класс для получения всех комбинаций
					class CAccessSFullCombination : public CAccessorTCombinatory
					{
					public:
						BEGIN_COLUMN_MAP(CAccessSFullCombination)
							COLUMN_ENTRY(1, m_uiID)
							COLUMN_ENTRY(2, m_shIDType)
							COLUMN_ENTRY(3, m_shIDForm)
							COLUMN_ENTRY(4, m_ushDictionaryNumber)
							COLUMN_ENTRY(5, m_uiIDSource)
							COLUMN_ENTRY(6, m_ushLevel)
							COLUMN_ENTRY(7, m_shCombiType)						
						END_COLUMN_MAP()
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CAccessSFullCombination, 
							_T("SELECT * FROM Combinatory"));	
					};

					/// класс для получения всех комбинаций
					class CAccessSFullCombinationExtra : public CAccessorTCombinatory
					{
					public:
						BEGIN_COLUMN_MAP(CAccessSFullCombinationExtra)
							COLUMN_ENTRY(1, m_uiID)
							COLUMN_ENTRY(2, m_shIDType)
							COLUMN_ENTRY(3, m_shIDForm)
							COLUMN_ENTRY(4, m_ushDictionaryNumber)
							COLUMN_ENTRY(5, m_uiIDSource)
							COLUMN_ENTRY(6, m_ushLevel)
							COLUMN_ENTRY(7, m_shCombiType)						
						END_COLUMN_MAP()
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CAccessSFullCombinationExtra, 
							_T("SELECT * FROM CombinatoryExtra"));	
					};

					/// класс для получения записей из таблицы CombiSynonyms
					class CAccessSFullTCombiSynonyms
					{
					public:
						
						UINT32 m_uiSynonymClass;
						UINT16 m_ushDictionaryNumber;
						UINT32 m_uiIDSource;

						BEGIN_COLUMN_MAP(CAccessSFullTCombiSynonyms)
							COLUMN_ENTRY(1, m_uiSynonymClass)										
							COLUMN_ENTRY(2, m_ushDictionaryNumber)										
							COLUMN_ENTRY(3, m_uiIDSource)																										
						END_COLUMN_MAP()
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CAccessSFullTCombiSynonyms,
							_T("SELECT x.SemanticClass, x.DictionaryNumber, x.IDSource  FROM CombiSynonyms x ORDER BY x.SemanticClass,x.IDSource"));	
					};

///////////////////////////////////////////////////////////////////////////////////////
					/*	комибинаторный анализ	*/
					typedef CAccessor<CAccessSMaxTCombinatory> CAccessorSMaxTCombinatory;
					typedef CCommand<CAccessorSMaxTCombinatory> CCommandSMaxTCombinatory;
					typedef CPreparedCommand<CAccessorSMaxTCombinatory> CPreparedSMaxTCombinatory;

					typedef CAccessor<CAccessITCombinatory> CAccessorITCombinatory;
					typedef CCommand<CAccessorITCombinatory> CCommandITCombinatory;
					typedef CPreparedCommand<CAccessorITCombinatory> CPreparedITCombinatory;

					typedef CAccessor<CAccessDTCombinatory> CAccessorDTCombinatory;
					typedef CCommand<CAccessorDTCombinatory> CCommandDTCombinatory;
					typedef CPreparedCommand<CAccessorDTCombinatory> CPreparedDTCombinatory;

					typedef CAccessor<CAccessSTCombinatoryInfo> CAccessorSTCombinatoryInfo;
					typedef CCommand<CAccessorSTCombinatoryInfo> CCommandSTCombinatoryInfo;
					typedef CPreparedCommand<CAccessorSTCombinatoryInfo> CPreparedSTCombinatoryInfo;

					typedef CAccessor<CAccessSAllCombination> CAccessorSAllCombination;
					typedef CCommand<CAccessorSAllCombination> CCommandSAllCombination;
					typedef CPreparedCommand<CAccessorSAllCombination> CPreparedSAllCombination;

					/// выбор всей информации
					typedef CAccessor<CAccessSFullCombination> CAccessorSFullCombination;
					typedef CCommand<CAccessorSFullCombination> CCommandSFullCombination;
					typedef CPreparedCommand<CAccessorSFullCombination> CPreparedSFullCombination;

					/// выбор всей информации
					typedef CAccessor<CAccessSFullCombinationExtra> CAccessorSFullCombinationExtra;
					typedef CCommand<CAccessorSFullCombinationExtra> CCommandSFullCombinationExtra;
					typedef CPreparedCommand<CAccessorSFullCombinationExtra> CPreparedSFullCombinationExtra;

					/// выбор всей информации
					typedef CAccessor<CAccessSFullTCombiSynonyms> CAccessorSFullTCombiSynonyms;
					typedef CCommand<CAccessorSFullTCombiSynonyms> CCommandSFullTCombiSynonyms;
					typedef CPreparedCommand<CAccessorSFullTCombiSynonyms> CPreparedSFullTCombiSynonyms;

					
					/*typedef CAccessor<CAccess> CAccessor;
					typedef CCommand<CAccessor> CCommand;
					typedef CPreparedCommand<CAccessor> CPrepared;*/					
				};				
			};
		};
	};
};

#pragma warning( default : 4995 )