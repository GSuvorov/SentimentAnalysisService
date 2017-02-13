#include "StdAfx.h"

#include ".\cashcosedi.h"

#include ".\dboCombinatory.h"
#include ".\dboSyntax.h"
#include ".\dboSemantic.h"


bool SS::CoSeDi::DBMS::CCashCoSeDi::Clear()
{	
	//SS_TRY
	//{
	//	for(int i = 0; i < ciMaxIDSource; i++)
	//		m_ucExistence[i] = 0;
	//	return true;
	//}
	//SS_CATCH(L"")

	return true;
}

bool SS::CoSeDi::DBMS::CCashCoSeDi::ReLoad()
{	
	//SS_TRY
	//{
	//	m_bIsLoad = false;		

	//	Load();
	//}
	//SS_CATCH(L"")

	//return m_bIsLoad;

	return true;
}

bool SS::CoSeDi::DBMS::CCashCoSeDi::ToBinary()
{
	//SS_TRY
	//{		
	//	return true;
	//}
	//SS_CATCH(L"")

	return true;
}

bool SS::CoSeDi::DBMS::CCashCoSeDi::Load()
{	
	//if(m_bIsLoad)
	//	return m_bIsLoad;

	//SS_TRY
	//{
	//	Clear();
	//	
	//	if((m_pDTable==NULL)||(!m_pDTable->IsOpen()))
	//		InitConnection(); 

	//	while(m_pDTable->Scan()==S_OK)   
	//	{
	//		if(m_SExistence.IDSource == 14535)
	//		{
	//			int i = 0;
	//		}
	//		m_ucExistence[m_SExistence.IDSource] = m_SExistence.IDExistence;
	//	}

	//	
	//	m_bIsLoad = true;
	//}
	//SS_CATCH(L"")

	//return m_bIsLoad;

	return true;
}

bool SS::CoSeDi::DBMS::CCashCoSeDi::InitConnection()
{
	//SS_TRY
	//{
	//	std::wstring wDataTableName = L"CoSeExistence";
	//	
	//	if(!SS::CoSeDi::DBMS::CConnect::InitConnection(wDataTableName))
	//		return false;
	//	
	//	TableRecord[0]	= &m_SExistence.IDSource;
	//	TableRecord[1]	= &m_SExistence.IDExistence;

	//	if(m_pDTable->ScanInit(0, TableRecord) != S_OK)
	//	{
	//		SS_THROW(L"ошибка инициализации ScanInit");
	//	}
	//}
	//SS_CATCH(L"")

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////


bool SS::CoSeDi::SQL::CCashCoSeDi::Clear()
{	
	//SS_TRY
	//{
	//	for(int i = 0; i < ciMaxIDSource; i++)
	//		m_ucExistence[i] = 0;
	//	return true;
	//}
	//SS_CATCH(L"")

	return true;
}

bool SS::CoSeDi::SQL::CCashCoSeDi::ReLoad()
{	
	//SS_TRY
	//{
	//	m_bIsLoad = false;		

	//	Load();
	//}
	//SS_CATCH(L"")

	//return m_bIsLoad;

	return true;
}


bool SS::CoSeDi::SQL::CCashCoSeDi::ToBinaryExistence()
{
	//SS_TRY
	//{
	//	std::wstring wDataTableName = L"CoSeExistence";		

	//	
	//	m_pDataBase->MoveToDataTable(wDataTableName.c_str());		// Переход по имени   
	//	
	//	SS::Interface::Core::DBMS::IDataTable * pDTable;
	//	pDTable = m_pDataBase->OpenDataTable();

	//	if(pDTable==NULL)
	//		return false; 

	//	pDTable->Clear();

	//	INT32        IDSource;
	//	INT32        IDExistence;			
	//	void* TableRecord[]= {&IDSource,    &IDExistence};

	//	if(pDTable->AddInit(TableRecord) != S_OK)   
	//		SS_THROW(L"Ошибка инициализации добавления слов в таблицу кеша новых слов");

	//	SS::CoSeDi::SQL::SQL_ACCESS::CPreparedSTExistence oPrepare;
	//
	//	
	//	oPrepare.Prepare(*m_pConnection->GetSession());

	//	if(oPrepare.Open() == S_OK)
	//	{
	//		if(oPrepare.MoveFirst() != DB_S_ENDOFROWSET)
	//		{					
	//			do
	//			{
	//				if(oPrepare.GetIDSource() == 14535)
	//				{
	//					int i = 0;
	//				}
	//				IDExistence	= oPrepare.GetExistence();
	//				IDSource	= oPrepare.GetIDSource();
	//				
	//				pDTable->Add();

	//			}while(oPrepare.MoveNext() != DB_S_ENDOFROWSET);					
	//		}
	//	}
	//	oPrepare.Close();

	//	pDTable->Close();

	//}
	//SS_CATCH(L"")

	return true;
}

bool SS::CoSeDi::SQL::CCashCoSeDi::ToBinarySemanticClassesTree()
{

	SS_TRY
	{
		std::wstring wDataTableName = L"SemanticClassesTree";		

		m_pDataBase->MoveToDataTable(wDataTableName.c_str());
		
		SS::Interface::Core::DBMS::IDataTable * pDTable;
		pDTable = m_pDataBase->OpenDataTable();

		if(pDTable==NULL)   {
			return false; 
		}

		pDTable->Clear();

		INT32        IDClass;
		INT32        IDParent;
		INT32        IdClassMaxPureVal = -1;
		std::wstring Name;
		std::wstring ShortName;
		UINT32       Color;
		
		void* TableRecord[]= {&IDClass, &IDParent, &Name, &ShortName, &Color};

		if(pDTable->AddInit(TableRecord) != S_OK)    { 
			SS_THROW(L"Ошибка инициализации добавления слов в таблицу SemanticClassesTree");
		}

		SS::CoSeDi::SQL::SQL_ACCESS::Semantic::CPreparedSFullTSemanticsTree oPrepare;
		
		oPrepare.Prepare(*m_pConnection->GetSession());

		if(oPrepare.Open() == S_OK)
		{
			if(oPrepare.MoveFirst() != DB_S_ENDOFROWSET)
			{
				do
				{
					IDClass  = oPrepare.GetIDClass();
					if(IdClassMaxPureVal < (IDClass & 0x00FFFFFF))   {
						IdClassMaxPureVal = (IDClass & 0x00FFFFFF);
					}
					IDParent  = oPrepare.GetIDParent();
					Name      = oPrepare.GetName();
					ShortName = oPrepare.GetShortName();
					Color     = oPrepare.GetColor();
					
					pDTable->Add();

				} while(oPrepare.MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oPrepare.Close();

		pDTable->SetUserValue(0,IdClassMaxPureVal+1);
		pDTable->Close();

	}
	SS_CATCH(L"")

		return true;
}

bool SS::CoSeDi::SQL::CCashCoSeDi::ToBinarySemanticClassesWords()
{

	SS_TRY
	{
		std::wstring wDataTableName = L"SemanticClassesWords";		

		m_pDataBase->MoveToDataTable(wDataTableName.c_str());
		
		SS::Interface::Core::DBMS::IDataTable * pDTable;
		pDTable = m_pDataBase->OpenDataTable();

		if(pDTable==NULL)   {
			return false; 
		}

		pDTable->Clear();

		INT32        IDSource;
		INT32        IDClass;
		
		void* TableRecord[]= {&IDSource, &IDClass };

		if(pDTable->AddInit(TableRecord) != S_OK)    { 
			SS_THROW(L"Ошибка инициализации добавления слов в таблицу SemanticClassesWords");
		}

		SS::CoSeDi::SQL::SQL_ACCESS::Semantic::CPreparedSFullTSemanticsWords oPrepare;
		
		oPrepare.Prepare(*m_pConnection->GetSession());

		if(oPrepare.Open() == S_OK)
		{
			if(oPrepare.MoveFirst() != DB_S_ENDOFROWSET)
			{
				do
				{
					IDSource = oPrepare.GetIDSource();
					IDClass  = oPrepare.GetIDClass();
					
					pDTable->Add();

				} while(oPrepare.MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oPrepare.Close();
		pDTable->Close();

	}
	SS_CATCH(L"")

		return true;
}

bool SS::CoSeDi::SQL::CCashCoSeDi::ToBinarySemantics()
{

	SS_TRY
	{
		std::wstring wDataTableName = L"Semantics";		

		m_pDataBase->MoveToDataTable(wDataTableName.c_str());		// Переход по имени   
		
		SS::Interface::Core::DBMS::IDataTable * pDTable;
		pDTable = m_pDataBase->OpenDataTable();

		if((pDTable==NULL)||(!pDTable->IsOpen()))
		{
			SS_THROW(L"таблица Semantics не открыта"); 
		}

		pDTable->Clear();

		using namespace SS::CoSeDi::DataTypes;
		SSemanticsAccess oSemanticsAccess;

		void* TableRecord[]= {
			&oSemanticsAccess.IDSource, 
			&oSemanticsAccess.DictionaryNumber, 
			&oSemanticsAccess.IDClass};

		if(pDTable->AddInit(TableRecord) != S_OK)  
		{
			SS_THROW(L"ошибка AddInit для Semantics"); 
		}
			

		using namespace SS::CoSeDi::SQL::SQL_ACCESS::Semantic;
		CPreparedSFullTSemantics oPrepare;
	
		
		oPrepare.Prepare(*m_pConnection->GetSession());

		if(oPrepare.Open() == S_OK)
		{
			if(oPrepare.MoveFirst() != DB_S_ENDOFROWSET)
			{					
				do
				{
					//oSemanticsAccess.IDClass			= oPrepare.GetClass();
					//oSemanticsAccess.DictionaryNumber	= (char)oPrepare.GetDictionaryNumber();
					//oSemanticsAccess.IDSource			= oPrepare.GetIDSource();

					UINT32 iUpperByte = oPrepare.GetDictionaryNumber() << 24;

					oSemanticsAccess.IDSource         = oPrepare.GetIDSource() | (iUpperByte & 0x7FFFFFFF);
					oSemanticsAccess.IDClass          = oPrepare.GetClass()    | (iUpperByte & 0x80000000) | (SS::Dictionary::DATA_TYPE::NAMES::ednSemantic << 24);
					oSemanticsAccess.DictionaryNumber = 0;

					pDTable->Add();

				}while(oPrepare.MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oPrepare.Close();

		pDTable->Close();
	}
	SS_CATCH(L"")

	return true;
}

bool SS::CoSeDi::SQL::CCashCoSeDi::ToBinarySemanticsExtra()
{

	SS_TRY
	{
		std::wstring wDataTableName = L"SemanticsExtra";		

		m_pDataBase->MoveToDataTable(wDataTableName.c_str());		// Переход по имени   
		
		SS::Interface::Core::DBMS::IDataTable * pDTable;
		pDTable = m_pDataBase->OpenDataTable();

		if((pDTable==NULL)||(!pDTable->IsOpen()))
		{
			SS_THROW(L"таблица SemanticsExtra не открыта"); 
		}

		pDTable->Clear();

		using namespace SS::CoSeDi::DataTypes;
		SSemanticsAccess oSemanticsAccess;

		void* TableRecord[]= {
			&oSemanticsAccess.IDSource, 
			&oSemanticsAccess.DictionaryNumber, 
			&oSemanticsAccess.IDClass};

		if(pDTable->AddInit(TableRecord) != S_OK)  
		{
			SS_THROW(L"ошибка AddInit для SemanticsExtra"); 
		}
			

		using namespace SS::CoSeDi::SQL::SQL_ACCESS::Semantic;
		CPreparedSFullTSemanticsExtra oPrepare;
	
		
		oPrepare.Prepare(*m_pConnection->GetSession());

		if(oPrepare.Open() == S_OK)
		{
			if(oPrepare.MoveFirst() != DB_S_ENDOFROWSET)
			{					
				do
				{
					//oSemanticsAccess.IDClass			= oPrepare.GetClass();
					//oSemanticsAccess.DictionaryNumber	= (char)oPrepare.GetDictionaryNumber();
					//oSemanticsAccess.IDSource			= oPrepare.GetIDSource();

					UINT32 iUpperByte = oPrepare.GetDictionaryNumber() << 24;

					oSemanticsAccess.IDSource         = oPrepare.GetIDSource() | (iUpperByte & 0x7FFFFFFF);
					oSemanticsAccess.IDClass          = oPrepare.GetClass()    | (iUpperByte & 0x80000000) | (SS::Dictionary::DATA_TYPE::NAMES::ednSemantic << 24);
					oSemanticsAccess.DictionaryNumber = 0;

					pDTable->Add();

				}while(oPrepare.MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oPrepare.Close();

		pDTable->Close();
	}
	SS_CATCH(L"")

	return true;
}


bool SS::CoSeDi::SQL::CCashCoSeDi::ToBinarySynonimsSemantics()
{

	SS_TRY
	{
		std::wstring wDataTableName = L"SemanticsSynonims";		

		m_pDataBase->MoveToDataTable(wDataTableName.c_str());		// Переход по имени   
		
		SS::Interface::Core::DBMS::IDataTable * pDTable;
		pDTable = m_pDataBase->OpenDataTable();

		if((pDTable==NULL)||(!pDTable->IsOpen()))
		{
			SS_THROW(L"таблица Semantics не открыта"); 
		}

		pDTable->Clear();

		using namespace SS::CoSeDi::DataTypes;
		SSemanticsAccess oSemanticsAccess;

		void* TableRecord[]= {
			&oSemanticsAccess.IDSource, 
			&oSemanticsAccess.DictionaryNumber, 
			&oSemanticsAccess.IDClass};

		if(pDTable->AddInit(TableRecord) != S_OK)  
		{
			SS_THROW(L"ошибка AddInit для Semantics"); 
		}
			

		using namespace SS::CoSeDi::SQL::SQL_ACCESS::Semantic;
		using namespace SS::Dictionary::DATA_TYPE::NAMES;
		CPreparedSFullTSemanticsSynonims oPrepare;
	
		
		oPrepare.Prepare(*m_pConnection->GetSession());

		if(oPrepare.Open() == S_OK)
		{
			if(oPrepare.MoveFirst() != DB_S_ENDOFROWSET)
			{					
				do
				{

					BOOL bSynDict;

					switch(oPrepare.GetDictionaryNumber())   {
						case ednAbbreviationsEng:
						case ednECombinatory:
						case ednENominalName:
						case ednEProperName:
						case ednSynonimsEng:
						case ednUnknownEng:
							bSynDict = ednSynonimsEng;
						break;
						default:
							bSynDict = ednSynonimsRus;
						break;
					}

					oSemanticsAccess.IDClass			   = oPrepare.GetClass()  | (bSynDict << 24);
					oSemanticsAccess.DictionaryNumber	= oPrepare.GetDictionaryNumber();
					oSemanticsAccess.IDSource			   = oPrepare.GetIDSource()  | (((UINT32)oPrepare.GetDictionaryNumber()) << 24);
					
					pDTable->Add();

				}while(oPrepare.MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oPrepare.Close();

		pDTable->Close();
	}
	SS_CATCH(L"")

	return true;
}

extern UINT32 _GetCrc32_20Bytes(UINT32 * paIDSourceArr);
extern UINT32 _GetCrc32_20Bytes_2(UINT32 * paIDSourceArr, UINT32 iMembersNum, UINT32 * piRealMembersNum = NULL);

bool SS::CoSeDi::SQL::CCashCoSeDi::ToBinaryCombinatory()
{

	SS_TRY
	{
        
		std::wstring wDataTableName = L"Combinatory";		

		m_pDataBase->MoveToDataTable(wDataTableName.c_str());		// Переход по имени   
		
		SS::Interface::Core::DBMS::IDataTable * pDTable;
		pDTable = m_pDataBase->OpenDataTable();

		if((pDTable==NULL)||(!pDTable->IsOpen()))
		{
			SS_THROW(L"ошибка при открытии таблицы Combinatory");
		}
		
		pDTable->Clear();

		SS::CoSeDi::DataTypes::SCombinationNewInfo oSCombinatoryAccess;
		
		void* TableRecord[]= 
		{
			&oSCombinatoryAccess.ID, 
			&oSCombinatoryAccess.HashKey, 
			&oSCombinatoryAccess.HashKey_2, 
			&oSCombinatoryAccess.CombiType, 
			&oSCombinatoryAccess.IDType,
			&oSCombinatoryAccess.NumOfMembers,								
			&oSCombinatoryAccess.WordIndexes[0].IDSource,
			&oSCombinatoryAccess.WordIndexes[0].IDForm,
			&oSCombinatoryAccess.WordIndexes[1].IDSource,
			&oSCombinatoryAccess.WordIndexes[1].IDForm,
			&oSCombinatoryAccess.WordIndexes[2].IDSource,
			&oSCombinatoryAccess.WordIndexes[2].IDForm,
			&oSCombinatoryAccess.WordIndexes[3].IDSource,
			&oSCombinatoryAccess.WordIndexes[3].IDForm,
			&oSCombinatoryAccess.WordIndexes[4].IDSource,
			&oSCombinatoryAccess.WordIndexes[4].IDForm
		};

		if(pDTable->AddInit(TableRecord) != S_OK)  
		{
			SS_THROW(L"ошибка AddInit для таблицы Combinatory");
		}
			

		using namespace	SS::CoSeDi::SQL::SQL_ACCESS::Combinatory;
		CPreparedSFullCombination oPrepare;
	
		int    i=0;												 
		int    iCurrID = -1;
		int    iMaxID  = -1;
//		int    iDictMask;
		int    iNumOfMembers=0;
		UINT32 aIDSourceArr[MAX_NUM_OF_COMBINATION_MEMBERS];
	
		oPrepare.Prepare(*m_pConnection->GetSession());

		if(oPrepare.Open() == S_OK)
		{
			if(oPrepare.MoveFirst() != DB_S_ENDOFROWSET)
			{	
				TIDSource oIDSource;
				do
				{
					if(iCurrID != oPrepare.GetID())   {
					
						if(iCurrID>=0)   {
							oSCombinatoryAccess.HashKey      = _GetCrc32_20Bytes(aIDSourceArr);
							oSCombinatoryAccess.HashKey_2    = _GetCrc32_20Bytes_2(aIDSourceArr,iNumOfMembers);
							oSCombinatoryAccess.NumOfMembers = iNumOfMembers;
							pDTable->Add();
						}

						i++;
						// Old

						// Next
						iNumOfMembers=0;
						memset(aIDSourceArr,0,sizeof(aIDSourceArr));
//						iDictMask = oPrepare.GetDictionaryNumber() << 24;
						oSCombinatoryAccess.ID        = oPrepare.GetID(); 
						oSCombinatoryAccess.CombiType = oPrepare.GetCombiType(); 
						oSCombinatoryAccess.IDType    = oPrepare.GetIDType(); 
						oSCombinatoryAccess.WordIndexes[0].IDSource = oSCombinatoryAccess.WordIndexes[0].IDForm =
						oSCombinatoryAccess.WordIndexes[1].IDSource = oSCombinatoryAccess.WordIndexes[1].IDForm =
						oSCombinatoryAccess.WordIndexes[2].IDSource = oSCombinatoryAccess.WordIndexes[2].IDForm =
						oSCombinatoryAccess.WordIndexes[3].IDSource = oSCombinatoryAccess.WordIndexes[3].IDForm =
						oSCombinatoryAccess.WordIndexes[4].IDSource = oSCombinatoryAccess.WordIndexes[4].IDForm = 0;

						iCurrID = oPrepare.GetID();
					}
					
					iNumOfMembers++;
					bool bDoFlag=TRUE;
					switch(oPrepare.GetLevel())   {
						case 1: aIDSourceArr[0] = oSCombinatoryAccess.WordIndexes[0].IDSource = (oPrepare.GetDictionaryNumber() << 24) | oPrepare.GetIDSource(); oSCombinatoryAccess.WordIndexes[0].IDForm=oPrepare.GetIDForm(); break;
						case 2: aIDSourceArr[1] = oSCombinatoryAccess.WordIndexes[1].IDSource = (oPrepare.GetDictionaryNumber() << 24) | oPrepare.GetIDSource(); oSCombinatoryAccess.WordIndexes[1].IDForm=oPrepare.GetIDForm(); break;
						case 3: aIDSourceArr[2] = oSCombinatoryAccess.WordIndexes[2].IDSource = (oPrepare.GetDictionaryNumber() << 24) | oPrepare.GetIDSource(); oSCombinatoryAccess.WordIndexes[2].IDForm=oPrepare.GetIDForm(); break;
						case 4: aIDSourceArr[3] = oSCombinatoryAccess.WordIndexes[3].IDSource = (oPrepare.GetDictionaryNumber() << 24) | oPrepare.GetIDSource(); oSCombinatoryAccess.WordIndexes[3].IDForm=oPrepare.GetIDForm(); break;
						case 5: aIDSourceArr[4] = oSCombinatoryAccess.WordIndexes[4].IDSource = (oPrepare.GetDictionaryNumber() << 24) | oPrepare.GetIDSource(); oSCombinatoryAccess.WordIndexes[4].IDForm=oPrepare.GetIDForm(); break;
						default: bDoFlag=FALSE; break;
					}

//					if(bDoFlag)   oListOfWords.push_back(sTmpStr);

					if(iMaxID<iCurrID)   {
						iMaxID = iCurrID;
					}

				} while(oPrepare.MoveNext() != DB_S_ENDOFROWSET);					

				if(iCurrID>=0 && iNumOfMembers>1)   {
					oSCombinatoryAccess.HashKey      = _GetCrc32_20Bytes(aIDSourceArr);
					oSCombinatoryAccess.HashKey_2    = _GetCrc32_20Bytes_2(aIDSourceArr,iNumOfMembers);
					oSCombinatoryAccess.NumOfMembers = iNumOfMembers;
					pDTable->Add();
				}
			}
		}
		oPrepare.Close();

		pDTable->SetUserValue(0,iMaxID+1);

		pDTable->Close();

	}
	SS_CATCH(L"")

	return true;
}

bool SS::CoSeDi::SQL::CCashCoSeDi::ToBinaryCombinatoryExtra()
{

	SS_TRY
	{
        
		std::wstring wDataTableName = L"CombinatoryExtra";		

		m_pDataBase->MoveToDataTable(wDataTableName.c_str());		// Переход по имени   
		
		SS::Interface::Core::DBMS::IDataTable * pDTable;
		pDTable = m_pDataBase->OpenDataTable();

		if((pDTable==NULL)||(!pDTable->IsOpen()))
		{
			SS_THROW(L"ошибка при открытии таблицы Combinatory");
		}
		
		pDTable->Clear();

		SS::CoSeDi::DataTypes::SCombinationNewInfo oSCombinatoryAccess;
		
		void* TableRecord[]= 
		{
			&oSCombinatoryAccess.ID, 
			&oSCombinatoryAccess.HashKey, 
			&oSCombinatoryAccess.HashKey_2, 
			&oSCombinatoryAccess.CombiType, 
			&oSCombinatoryAccess.IDType,
			&oSCombinatoryAccess.NumOfMembers,								
			&oSCombinatoryAccess.WordIndexes[0].IDSource,
			&oSCombinatoryAccess.WordIndexes[0].IDForm,
			&oSCombinatoryAccess.WordIndexes[1].IDSource,
			&oSCombinatoryAccess.WordIndexes[1].IDForm,
			&oSCombinatoryAccess.WordIndexes[2].IDSource,
			&oSCombinatoryAccess.WordIndexes[2].IDForm,
			&oSCombinatoryAccess.WordIndexes[3].IDSource,
			&oSCombinatoryAccess.WordIndexes[3].IDForm,
			&oSCombinatoryAccess.WordIndexes[4].IDSource,
			&oSCombinatoryAccess.WordIndexes[4].IDForm
		};

		if(pDTable->AddInit(TableRecord) != S_OK)  
		{
			SS_THROW(L"ошибка AddInit для таблицы Combinatory_New");
		}
			

		using namespace	SS::CoSeDi::SQL::SQL_ACCESS::Combinatory;
		CPreparedSFullCombinationExtra oPrepare;
	
		int    i=0;												 
		int    iCurrID = -1;
		int    iMaxID  = -1;
//		int    iDictMask;
		int    iNumOfMembers=0;
		UINT32 aIDSourceArr[MAX_NUM_OF_COMBINATION_MEMBERS];
	
		oPrepare.Prepare(*m_pConnection->GetSession());

		if(oPrepare.Open() == S_OK)
		{
			if(oPrepare.MoveFirst() != DB_S_ENDOFROWSET)
			{	
				TIDSource oIDSource;
				do
				{
					if(iCurrID != oPrepare.GetID())   {
					
						if(iCurrID>=0)   {
							oSCombinatoryAccess.HashKey      = _GetCrc32_20Bytes(aIDSourceArr);
							oSCombinatoryAccess.HashKey_2    = _GetCrc32_20Bytes_2(aIDSourceArr,iNumOfMembers);
							oSCombinatoryAccess.NumOfMembers = iNumOfMembers;
							pDTable->Add();
						}

						i++;
						// Old

						// Next
						iNumOfMembers=0;
						memset(aIDSourceArr,0,sizeof(aIDSourceArr));
//						iDictMask = oPrepare.GetDictionaryNumber() << 24;
						oSCombinatoryAccess.ID        = oPrepare.GetID(); 
						oSCombinatoryAccess.CombiType = oPrepare.GetCombiType(); 
						oSCombinatoryAccess.IDType    = oPrepare.GetIDType(); 
						oSCombinatoryAccess.WordIndexes[0].IDSource = oSCombinatoryAccess.WordIndexes[0].IDForm =
						oSCombinatoryAccess.WordIndexes[1].IDSource = oSCombinatoryAccess.WordIndexes[1].IDForm =
						oSCombinatoryAccess.WordIndexes[2].IDSource = oSCombinatoryAccess.WordIndexes[2].IDForm =
						oSCombinatoryAccess.WordIndexes[3].IDSource = oSCombinatoryAccess.WordIndexes[3].IDForm =
						oSCombinatoryAccess.WordIndexes[4].IDSource = oSCombinatoryAccess.WordIndexes[4].IDForm = 0;

						iCurrID = oPrepare.GetID();
					}
					
					iNumOfMembers++;
					bool bDoFlag=TRUE;
					switch(oPrepare.GetLevel())   {
						case 1: aIDSourceArr[0] = oSCombinatoryAccess.WordIndexes[0].IDSource = (oPrepare.GetDictionaryNumber() << 24) | oPrepare.GetIDSource(); oSCombinatoryAccess.WordIndexes[0].IDForm=oPrepare.GetIDForm(); break;
						case 2: aIDSourceArr[1] = oSCombinatoryAccess.WordIndexes[1].IDSource = (oPrepare.GetDictionaryNumber() << 24) | oPrepare.GetIDSource(); oSCombinatoryAccess.WordIndexes[1].IDForm=oPrepare.GetIDForm(); break;
						case 3: aIDSourceArr[2] = oSCombinatoryAccess.WordIndexes[2].IDSource = (oPrepare.GetDictionaryNumber() << 24) | oPrepare.GetIDSource(); oSCombinatoryAccess.WordIndexes[2].IDForm=oPrepare.GetIDForm(); break;
						case 4: aIDSourceArr[3] = oSCombinatoryAccess.WordIndexes[3].IDSource = (oPrepare.GetDictionaryNumber() << 24) | oPrepare.GetIDSource(); oSCombinatoryAccess.WordIndexes[3].IDForm=oPrepare.GetIDForm(); break;
						case 5: aIDSourceArr[4] = oSCombinatoryAccess.WordIndexes[4].IDSource = (oPrepare.GetDictionaryNumber() << 24) | oPrepare.GetIDSource(); oSCombinatoryAccess.WordIndexes[4].IDForm=oPrepare.GetIDForm(); break;
						default: bDoFlag=FALSE; break;
					}

//					if(bDoFlag)   oListOfWords.push_back(sTmpStr);

					if(iMaxID<iCurrID)   {
						iMaxID = iCurrID;
					}

				} while(oPrepare.MoveNext() != DB_S_ENDOFROWSET);					

				if(iCurrID>=0 && iNumOfMembers>1)   {
					oSCombinatoryAccess.HashKey      = _GetCrc32_20Bytes(aIDSourceArr);
					oSCombinatoryAccess.HashKey_2    = _GetCrc32_20Bytes_2(aIDSourceArr,iNumOfMembers);
					oSCombinatoryAccess.NumOfMembers = iNumOfMembers;
					pDTable->Add();
				}
			}
		}
		oPrepare.Close();

		pDTable->SetUserValue(0,iMaxID+1);

		pDTable->Close();

	}
	SS_CATCH(L"")

	return true;
}


bool SS::CoSeDi::SQL::CCashCoSeDi::ToBinaryCombiSynonyms()
{

	SS_TRY
	{
        
		std::wstring wDataTableName = L"CombiSynonyms";		

		m_pDataBase->MoveToDataTable(wDataTableName.c_str());		// Переход по имени   
		
		SS::Interface::Core::DBMS::IDataTable * pDTable;
		pDTable = m_pDataBase->OpenDataTable();

		if((pDTable==NULL)||(!pDTable->IsOpen()))
		{
			SS_THROW(L"Ошибка при открытии таблицы CombiSynonyms");
		}
		
		pDTable->Clear();

		using namespace SS::CoSeDi::DataTypes;
		SCombiSynonymsAccess oCombiSynonymsAccess;

		void* TableRecord[]= {
			&oCombiSynonymsAccess.IDSource, 
			&oCombiSynonymsAccess.DictionaryNumber, 
			&oCombiSynonymsAccess.SynonymClass
		};

		if(pDTable->AddInit(TableRecord) != S_OK)  
		{
			SS_THROW(L"Ошибка AddInit для CombiSynonyms"); 
		}

		UINT32 SynonymClass, DictionaryNumber, IDSource;

		void* TableRecord2[]= {
			&IDSource, 
			&DictionaryNumber, 
			&SynonymClass
		};

		if(pDTable->ScanInit(0,TableRecord2) != S_OK)  
		{
			SS_THROW(L"Ошибка ScanInit для CombiSynonyms"); 
		}
			

		using namespace	SS::CoSeDi::SQL::SQL_ACCESS::Combinatory;
		using namespace	SS::Dictionary::DATA_TYPE::NAMES;
		CPreparedSFullTCombiSynonyms oPrepare;
		

		int n=0,nUT=0;
		oPrepare.Prepare(*m_pConnection->GetSession());
		if(oPrepare.Open() == S_OK)
		{
			if(oPrepare.MoveFirst() != DB_S_ENDOFROWSET)
			{					
				do
				{
					BOOL bSynDict;

					switch(oPrepare.m_ushDictionaryNumber)   {
						case ednAbbreviationsEng:
						case ednECombinatory:
						case ednENominalName:
						case ednEProperName:
						case ednSynonimsEng:
						case ednUnknownEng:
							bSynDict = ednSynonimsEng;
						break;
						default:
							bSynDict = ednSynonimsRus;
						break;
					}

					oCombiSynonymsAccess.SynonymClass     = oPrepare.m_uiSynonymClass | (bSynDict << 24);
					oCombiSynonymsAccess.DictionaryNumber = oPrepare.m_ushDictionaryNumber;
					oCombiSynonymsAccess.IDSource         = oPrepare.m_uiIDSource | (oPrepare.m_ushDictionaryNumber << 24);

					
					pDTable->Add();
					if(pDTable->ScanOne(pDTable->GetRecordsNumber()-1)!=S_OK)   { // ScanLast
						SS_THROW(L"Ошибка ScanLast для CombiSynonyms"); 
					} else   {
						if(DictionaryNumber ==  oPrepare.m_ushDictionaryNumber   &&
							SynonymClass     == (oPrepare.m_uiSynonymClass  | (bSynDict << 24))  &&
							IDSource         == (oPrepare.m_uiIDSource | (oPrepare.m_ushDictionaryNumber << 24)))   {
							nUT++;
						}
					}
					n++;

				} while(oPrepare.MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oPrepare.Close();
		if(n!=nUT)   SS_THROW(L"Ошибка: различие данных SQL-DBMS для CombiSynonyms");

		pDTable->Close();
	}
	SS_CATCH(L"")

	return true;
}


bool SS::CoSeDi::SQL::CCashCoSeDi::ToBinarySyntax()
{

	SS_TRY
	{
		std::wstring wDataTableName = L"Syntax";		

		m_pDataBase->MoveToDataTable(wDataTableName.c_str());		// Переход по имени   
		
		SS::Interface::Core::DBMS::IDataTable * pDTable;
		pDTable = m_pDataBase->OpenDataTable();

		if((pDTable==NULL)||(!pDTable->IsOpen()))
		{
			SS_THROW(L"таблица Syntax не открыта");
		}

		pDTable->Clear();

		SS::CoSeDi::DataTypes::SSyntaxAccess oSSyntaxAccess;
		
		void* TableRecord[]= 
		{
			&oSSyntaxAccess.DictionaryNumber,
            &oSSyntaxAccess.IDSource, 
			&oSSyntaxAccess.SyntaxInfo
		};

		if(pDTable->AddInit(TableRecord) != S_OK)   
		{
			SS_THROW(L"ошибка инициализации добавления в DBMS функция AddInit");
		}

		using namespace SS::CoSeDi::SQL::SQL_ACCESS::Syntax;
		CPreparedSyntaxSALL oPrepare;
	
		
		oPrepare.Prepare(*m_pConnection->GetSession());

		if(oPrepare.Open() == S_OK)
		{
			if(oPrepare.MoveFirst() != DB_S_ENDOFROWSET)
			{
				TIDSource oIDSource;
				do
				{
					oSSyntaxAccess.DictionaryNumber =  (char)oPrepare.GetDictionaryNumber();
					oSSyntaxAccess.IDSource			= oPrepare.GetIDSource();
					oSSyntaxAccess.SyntaxInfo		= oPrepare.GetSyntaxInfo();
					
					pDTable->Add();

				}while(oPrepare.MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oPrepare.Close();

		pDTable->Close();
	}
	SS_CATCH(L"")

	return true;
}

bool SS::CoSeDi::SQL::CCashCoSeDi::ToBinarySemanticIDSourceTree()
{

	SS_TRY
	{
		std::wstring wDataTableName = L"SemanticIDSourceTree";		

		m_pDataBase->MoveToDataTable(wDataTableName.c_str());		// Переход по имени   
		
		SS::Interface::Core::DBMS::IDataTable * pDTable;
		pDTable = m_pDataBase->OpenDataTable();

		if((pDTable==NULL)||(!pDTable->IsOpen()))
		{
			SS_THROW(L"таблица SemanticIDSourceTree не открыта");
		}

		pDTable->Clear();

		SSemanticIDSourceTree oSemanticIDSourceTree;
		
		void* TableRecord[]= 
		{
				&oSemanticIDSourceTree.DictionaryNumberParent,
				&oSemanticIDSourceTree.IDSourceParent,
				&oSemanticIDSourceTree.DictionaryNumberChild,
				&oSemanticIDSourceTree.IDSourceChild								
		};

		if(pDTable->AddInit(TableRecord) != S_OK)   
		{
			SS_THROW(L"ошибка инициализации добавления в DBMS функция AddInit");
		}

		using namespace SS::CoSeDi::SQL::SQL_ACCESS::Semantic;
		CPreparedSAllSemanticIDSourceTree oPrepare;
		
		oPrepare.Prepare(*m_pConnection->GetSession());

		if(oPrepare.Open() == S_OK)
		{
			if(oPrepare.MoveFirst() != DB_S_ENDOFROWSET)
			{
				TIDSource oIDSource;
				do
				{
					oSemanticIDSourceTree.DictionaryNumberParent= oPrepare.GetDictionaryNumberParent();
					oSemanticIDSourceTree.IDSourceParent		= oPrepare.GetIDSourceParent();
					oSemanticIDSourceTree.DictionaryNumberChild	= oPrepare.GetDictionaryNumberChild();
					oSemanticIDSourceTree.IDSourceChild			= oPrepare.GetIDSourceChild();

					pDTable->Add();

				}while(oPrepare.MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oPrepare.Close();

		pDTable->Close();
	}
	SS_CATCH(L"")

	return true;
}

bool SS::CoSeDi::SQL::CCashCoSeDi::ToBinary()
{

	SS_TRY
	{
		if(!ToBinaryExistence())
			return false;

		if(!ToBinaryCombinatory())
			return false;

		if(!ToBinaryCombinatoryExtra())
			return false;

		if(!ToBinaryCombiSynonyms())
			return false;
		
		if(!ToBinarySynonimsSemantics())
			return false;

		if(!ToBinarySemantics())
			return false;

		if(!ToBinarySemanticsExtra())
			return false;
		
		if(!ToBinarySemanticClassesTree())
			return false;

		if(!ToBinarySemanticClassesWords())
			return false;

		if(!ToBinarySyntax())
			return false;

		if(!ToBinarySemanticIDSourceTree())
			return false;
	}
	SS_CATCH(L"")

	return true;
}



bool SS::CoSeDi::SQL::CCashCoSeDi::Load()
{	
	if(m_bIsLoad)
		return m_bIsLoad;

	SS_TRY
	{
		Clear();						
		
		SS::CoSeDi::SQL::SQL_ACCESS::CPreparedSTExistence* oT = new SS::CoSeDi::SQL::SQL_ACCESS::CPreparedSTExistence();	
		oT->Prepare(*m_pConnection->GetSession());

		if(oT->Open() == S_OK)
		{
			if(oT->MoveFirst() != DB_S_ENDOFROWSET)
			{					
				do
				{
					m_ucExistence[oT->GetIDSource()] = oT->GetExistence();
				}
				while(oT->MoveNext() != DB_S_ENDOFROWSET);					
			}
		}
		oT->Close();
		delete oT;
		oT = NULL;

		m_bIsLoad = true;
	}
	SS_CATCH(L"")

	return m_bIsLoad;
}


bool SS::CoSeDi::SQL::CCashCoSeDi::AddIDSource(unsigned int uiIDSource, SS::CoSeDi::DataTypes::EWorkRegion ewr)
{

	SS_TRY
	{

//		CBaseCashCoSeDi::AddIDSource(uiIDSource, ewr);
		
		///////////////////////////////////////////////////////////////////////
		/*	сохранение параметров в SQL	*/

		SS::CoSeDi::SQL::SQL_ACCESS::CPreparedUTExistence oT;					
		oT.Prepare(*m_pConnection->GetSession());

		oT.SetExistence(m_ucExistence[uiIDSource]);
		oT.SetIDSource(uiIDSource);			
		oT.Open();
		oT.Close();
	}
	SS_CATCH(L"")

	return true;
}
