#pragma once

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
				namespace Syntax
				{
					class CAccessDetectWordsGroups
					{
					protected:					
						wchar_t m_wszDetectWordsGroupName[50];
						wchar_t m_wszDetectWord[50];
					public:
						void SetDetectWordsGroupName( std::wstring& wsDetectWordsGroupName)
						{
							wcscpy(m_wszDetectWordsGroupName, wsDetectWordsGroupName.c_str());
						}
						void GetDetectWordsGroupName( std::wstring& wsDetectWordsGroupName)
						{
							wsDetectWordsGroupName = m_wszDetectWordsGroupName;
						}
						void SetDetectWord(const std::wstring& wsDetectWord)
						{
							wcscpy(m_wszDetectWord, wsDetectWord.c_str());
						}
						void GetDetectWord( std::wstring& wsDetectWord)
						{
							wsDetectWord = m_wszDetectWord;
						}
					};

					class CSDetectWordsGroupsByDetectWordsGroupName : public CAccessDetectWordsGroups
					{
					public:
						BEGIN_COLUMN_MAP(CSDetectWordsGroupsByDetectWordsGroupName)
							COLUMN_ENTRY(1, m_wszDetectWordsGroupName)										
							COLUMN_ENTRY(2, m_wszDetectWord)										
						END_COLUMN_MAP()
					public:
						BEGIN_PARAM_MAP(CSDetectWordsGroupsByDetectWordsGroupName)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(1, m_wszDetectWordsGroupName)						
						END_PARAM_MAP()		
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CSDetectWordsGroupsByDetectWordsGroupName,
							_T("SELECT DetectWordsGroupName, DetectWord FROM DetectWordsGroups WHERE DetectWordsGroupName = ?"));	

					};

					class CSDetectWordsGroups : public CAccessDetectWordsGroups
					{
					public:
						BEGIN_COLUMN_MAP(CSDetectWordsGroups)
							COLUMN_ENTRY(1, m_wszDetectWordsGroupName)										
							COLUMN_ENTRY(2, m_wszDetectWord)										
						END_COLUMN_MAP()
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CSDetectWordsGroups,
							_T("SELECT DetectWordsGroupName, DetectWord FROM DetectWordsGroups"));	

					};

					class CIDetectWordsGroups : public CAccessDetectWordsGroups
					{
					public:
						BEGIN_PARAM_MAP(CIDetectWordsGroups)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(1, m_wszDetectWordsGroupName)						
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(2, m_wszDetectWord)						
						END_PARAM_MAP()		
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CIDetectWordsGroups,
							_T("INSERT INTO DetectWordsGroups (DetectWordsGroupName, DetectWord) VALUES (?,?)"));	
					};

					class CDDetectWordsGroups : public CAccessDetectWordsGroups
					{
					public:
						BEGIN_PARAM_MAP(CDDetectWordsGroups)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(1, m_wszDetectWordsGroupName)						
						END_PARAM_MAP()		
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CDDetectWordsGroups,
							_T("DELETE DetectWordsGroups WHERE DetectWordsGroupName = ?"));	
					};

/////////////////////////////////////////////////////////////////////////////////////////
				/* Работа со словарем синтаксических конструкций  */

					/// класс для доступа к таблице SyntaxConstruction
					class CAccessorSyntaxConstruction 
					{
					public:
						CAccessorSyntaxConstruction() : m_lID(0), m_lDataSize(0), m_lLevel(0)
						{
							memset(m_Data   , 0, sizeof(char) * 600);
							memset(m_wszName, 0, sizeof(wchar_t) * 512);
							memset(m_wszWord, 0, sizeof(wchar_t) * 50);
						}
					public:
						/**	номер конструции	*/
						long m_lID;
						/**	название конструкции	*/
						wchar_t m_wszName[512];
						/**	литеральное представление искомого слова	*/
						wchar_t m_wszWord[50];
						/**	данные, хранимые в элементе конструкции	*/
						unsigned char m_Data[600];
						/**	размер, занимаемый данными на диске	*/
						long m_lDataSize;
						/**	номер записи в конструкции	*/
						long m_lLevel;
						
					public:	
						/**	возвращает номер записи в конструкции */
						long GetLevel()
						{
							return m_lLevel;
						}
						/**	устанавливает номер записи в конструкции	*/
						void SetLevel(long lLevel)
						{
							m_lLevel = lLevel;
						}

						/**	возвращает номер конструции	*/
						long GetID()
						{
							return m_lID;
						}
						/**	устанавливает номер конструции	*/
						void SetID(long lID)
						{
							m_lID = lID;
						}
						/**	возвращает название конструкции	*/
						wchar_t* GetName()
						{
							return m_wszName;
						}
						/**	устанавливает название конструкции	*/
						void SetName(const wchar_t* wszName)
						{
							wcscpy(m_wszName, wszName);
						}
						/**	возвращает название конструкции	*/
						wchar_t* GetWord()
						{
							return m_wszWord;
						}
						/**	устанавливает название конструкции	*/
						void SetWord(const wchar_t* wszWord)
						{
							wcscpy(m_wszWord, wszWord);
						}
						/**	возвращает данные, хранимые в элементе конструкции	*/
						void GetConstructionData(std::vector<unsigned char>& vecOfByte)
						{
							vecOfByte.resize(m_lDataSize);

							memcpy(&vecOfByte[0], m_Data, m_lDataSize);
							
						}
						/**	устанавливает данные, хранимые в элементе конструкции	*/
						void SetConstructionData(std::vector<unsigned char>& vecOfByte)
						{
							m_lDataSize = (long)vecOfByte.size();
							memcpy(m_Data, &vecOfByte[0], m_lDataSize);
						}
					protected:
						/**	возвращает размер данных	*/
						long GetDataSize()
						{
							return m_lDataSize;
						}
						/**	устанавливает размер данных	*/
						void SetDataSize(long lDataSize)
						{
							m_lDataSize = lDataSize;
						}
					};


					/**	удаление синтаксической конструкции	*/
					class CDSyntaxConstruction : public CAccessorSyntaxConstruction
					{
					public:
						BEGIN_PARAM_MAP(CDSyntaxConstruction)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(1, m_lID)	
						END_PARAM_MAP()		
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CDSyntaxConstruction,
							_T("DELETE SyntaxConstruction WHERE ID=?"));	
					};

					///Удаление всех записей из таблицы SyntaxConstruction
					class CDAllSyntaxConstruction : CAccessorSyntaxConstruction
					{
					public:
						DEFINE_COMMAND(CDSyntaxConstruction, _T("DELETE SyntaxConstruction"));
					};

					/*	добавление синтаксической конструкции	*/
					class CISyntaxConstruction : public CAccessorSyntaxConstruction
					{
					public:
						BEGIN_PARAM_MAP(CISyntaxConstruction)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(1, m_lID)	
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(2, m_wszName)	
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(3, m_wszWord)	
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(4, m_Data)	
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(5, m_lDataSize)						
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(6, m_lLevel)						
						END_PARAM_MAP()		
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CISyntaxConstruction,
							_T("INSERT SyntaxConstruction (ID, Name, Word, Data, DataSize, Level) VALUES(?,?,?,?,?,?)"));	
					};
					
					/**	получение всех конструкций	*/
					class CSAllSyntaxConstruction : public CAccessorSyntaxConstruction
					{
					public:
						BEGIN_COLUMN_MAP(CSAllSyntaxConstruction)
							COLUMN_ENTRY(1, m_lID)		
							COLUMN_ENTRY(2, m_wszName)	
							COLUMN_ENTRY(3, m_wszWord)	
							COLUMN_ENTRY(4, m_Data)	
							COLUMN_ENTRY(5, m_lDataSize)	
							COLUMN_ENTRY(6, m_lLevel)
						END_COLUMN_MAP()
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CSAllSyntaxConstruction,
//_T("SELECT ID, Name, Word, Data, DataSize, Level FROM SyntaxConstruction, ConstructionsInUse WHERE ID = IDConstruction and Rem = 1 ORDER BY ID, Level"));	
	_T( "select ID, Name, Word, Data, DataSize, Level \
			from ( select ID, Name, Word, Data, DataSize, Level from dbo.SyntaxConstruction \
				where ID not in \
					( select ID from dbo.ConstructionObjectsRem ) \
				or Level not in \
					( select Level from dbo.ConstructionObjectsRem ) ) as p, dbo.ConstructionsInUse \
		where ID = IDConstruction and Rem = 1 \
		order by ID, Level" ) );
					};

					/**	получение конструкций по ID	*/
					class CSByIDSyntaxConstruction : public CAccessorSyntaxConstruction
					{
					public:
						BEGIN_COLUMN_MAP(CSByIDSyntaxConstruction)
							COLUMN_ENTRY(1, m_lID)		
							COLUMN_ENTRY(2, m_wszName)	
							COLUMN_ENTRY(3, m_wszWord)	
							COLUMN_ENTRY(4, m_Data)	
							COLUMN_ENTRY(5, m_lDataSize)	
							COLUMN_ENTRY(6, m_lLevel)
						END_COLUMN_MAP()
					public:
						BEGIN_PARAM_MAP(CSByIDSyntaxConstruction)
							SET_PARAM_TYPE(DBPARAMIO_INPUT)						
							COLUMN_ENTRY(1, m_lID)							
						END_PARAM_MAP()		
				
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CISyntaxConstruction,
							_T("SELECT ID, Name, Word, Data, DataSize, Level FROM SyntaxConstruction WHERE ID=? ORDER BY Level"));	
					};

					/**	получение максимального значения ID	*/
					class CSMaxIDSyntaxConstruction : public CAccessorSyntaxConstruction
					{
					public:
						BEGIN_COLUMN_MAP(CSMaxIDSyntaxConstruction)
							COLUMN_ENTRY(1, m_lID)		
						END_COLUMN_MAP()
					public:	/*	параметры для запроса	*/
						/*	параметрический запрос	*/
						DEFINE_COMMAND(CSMaxIDSyntaxConstruction,
							_T("SELECT MAX(ID) FROM SyntaxConstruction HAVING count(*) > 0"));	
					};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					/*	группа слов для детектирования	*/

					typedef CAccessor<CSDetectWordsGroups> CAccessorSDetectWordsGroups;
					typedef CCommand<CAccessorSDetectWordsGroups> CCommandSDetectWordsGroups;
					typedef CPreparedCommand<CAccessorSDetectWordsGroups> CPreparedSDetectWordsGroups;

					typedef CAccessor<CSDetectWordsGroupsByDetectWordsGroupName> CAccessorSDetectWordsGroupsByDetectWordsGroupName;
					typedef CCommand<CAccessorSDetectWordsGroupsByDetectWordsGroupName> CCommandSDetectWordsGroupsByDetectWordsGroupName;
					typedef CPreparedCommand<CAccessorSDetectWordsGroupsByDetectWordsGroupName> CPreparedSDetectWordsGroupsByDetectWordsGroupName;

					typedef CAccessor<CIDetectWordsGroups> CAccessorIDetectWordsGroups;
					typedef CCommand<CAccessorIDetectWordsGroups> CCommandIDetectWordsGroups;
					typedef CPreparedCommand<CAccessorIDetectWordsGroups> CPreparedIDetectWordsGroups;

					typedef CAccessor<CDDetectWordsGroups> CAccessorDDetectWordsGroups;
					typedef CCommand<CAccessorDDetectWordsGroups> CCommandDDetectWordsGroups;
					typedef CPreparedCommand<CAccessorDDetectWordsGroups> CPreparedDDetectWordsGroups;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					/*	синтаксические конструкции	*/

					typedef CAccessor<CSMaxIDSyntaxConstruction> CAccessorSMaxIDSyntaxConstruction;
					typedef CCommand<CAccessorSMaxIDSyntaxConstruction> CCommandSMaxIDSyntaxConstruction;
					typedef CPreparedCommand<CAccessorSMaxIDSyntaxConstruction> CPreparedSMaxIDSyntaxConstruction;

					typedef CAccessor<CDSyntaxConstruction> CAccessorDSyntaxConstruction;
					typedef CCommand<CAccessorDSyntaxConstruction> CCommandDSyntaxConstruction;
					typedef CPreparedCommand<CAccessorDSyntaxConstruction> CPreparedDSyntaxConstruction;

					typedef CAccessor<CDAllSyntaxConstruction> CAccessorDAllSyntaxConstruction;
					typedef CCommand<CAccessorDAllSyntaxConstruction> CCommandDAllSyntaxConstruction;
					typedef CPreparedCommand<CAccessorDAllSyntaxConstruction> CPreparedDAllSyntaxConstruction;

					typedef CAccessor<CISyntaxConstruction> CAccessorISyntaxConstruction;
					typedef CCommand<CAccessorISyntaxConstruction> CCommandISyntaxConstruction;
					typedef CPreparedCommand<CAccessorISyntaxConstruction> CPreparedISyntaxConstruction;

					typedef CAccessor<CSAllSyntaxConstruction> CAccessorSAllSyntaxConstruction;
					typedef CCommand<CAccessorSAllSyntaxConstruction> CCommandSAllSyntaxConstruction;
					typedef CPreparedCommand<CAccessorSAllSyntaxConstruction> CPreparedSAllSyntaxConstruction;

					typedef CAccessor<CSByIDSyntaxConstruction> CAccessorSByIDSyntaxConstruction;
					typedef CCommand<CAccessorSByIDSyntaxConstruction> CCommandSByIDSyntaxConstruction;
					typedef CPreparedCommand<CAccessorSByIDSyntaxConstruction> CPreparedSByIDSyntaxConstruction;

				};				
			};
		};
	};
};

#pragma warning( default : 4995 )