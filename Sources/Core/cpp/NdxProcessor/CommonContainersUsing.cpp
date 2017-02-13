#include "StdAfx.h"

#ifdef _SS_UNITTESTS

#include ".\commoncontainersusing.h"
#include ".\console.h"
#include "..\ASCInterface\ICommonServices.h"

namespace SS
{
namespace UnitTests
{
namespace NdxSE
{
namespace NdxProcessor
{

using namespace SS::Interface::Core::CommonContainers;
using namespace SS::Core::NdxSE::NdxProcessor;
using namespace SS::Interface::Core::DBMS;
using namespace SS::Interface::Core::NdxSE;

//--------------------------------------------------------------------//

SS::Interface::Core::ResourceManagers::ILoadManager* CCommonContainersUsing::GetLoadManager(void)
{
	SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager=CUnitTestsUsings::GetLoadManager();

	if(!m_pIndexationResult)
		m_pIndexationResult=(IIndexationResult*)CUnitTestsUsings::GetLoadManager()->
		GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_IndexationResult, IID_IndexationResult);

	if(!m_pSearchResult) 
		m_pSearchResult=(ISearchResult*)CUnitTestsUsings::GetLoadManager()->
		GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_SearchResult, IID_SearchResult);

	if(!m_pQueryResult) 
		m_pQueryResult=(IQueryResult*)CUnitTestsUsings::GetLoadManager()->
		GetInterface(L"./core/linguistics/CommonContainers.dll", CLSID_QueryResult, IID_QueryResult);

	if(!m_pPrmQueryResult) 
		m_pPrmQueryResult=(IParametrizedQuery*)CUnitTestsUsings::GetLoadManager()->
		GetInterface(L"./core/NdxQuery.dll", CLSID_ParametrizedQuery, IID_ParametrizedQuery);

	if(!m_pISContainer) 
		m_pISContainer=(SS::Interface::Core::IISContainerUM*)CUnitTestsUsings::GetLoadManager()->
		GetInterface(L"./core/ISContainer.dll", CLSID_ISContainer, IID_ISContainer);
	
	return pLoadManager;
}

void CCommonContainersUsing::StartFillIndexationResult(void)
{
	//задаем стартовую точку, чтобы всегда была одинаковая последовательность
	srand(1);
	m_uiCurrTextNumber=0;
	m_uiCurrPosition=0;
}

void CCommonContainersUsing::InitMetaFieldCollection(void)
{
	SS::Interface::Core::CommonServices::ISettingsServer* pSettings=
	(SS::Interface::Core::CommonServices::ISettingsServer*)GetLoadManager()->
		GetInterface(L"./core/GSServer.dll", CLSID_SettingsServer , IID_SettingsServer);

	if(ISNULL(pSettings)) return;
	
	m_NdxMetaFieldCollection.clear();

	SS::Interface::Core::DBMS::TDataTableFieldDescription DataTableFieldDescription;
	SS::Interface::Core::CommonServices::EParameterType ParameterType=SS::Interface::Core::CommonServices::ptString;
	vector<wstring> vFieldsNames;
	pSettings->GetArray(L"AMIS::Fields", ParameterType, (void*)&vFieldsNames);
	wchar_t* wszField=NULL;
	wchar_t wszSeps[]=L" ";
	wchar_t *wszToken;
	for (unsigned int i = 0; i < vFieldsNames.size(); i++)
	{
		//каждое строка поля состоит из трех частей: имя поля, тип поля, модификатор поля
		//разделенные пробелом
		wszField=(wchar_t*)vFieldsNames[i].c_str();
		wszToken=wcstok(wszField, wszSeps);
		int k=0;
		while(wszToken!=NULL)
		{
			switch(k){
			case 0:
				//имя поля
				DataTableFieldDescription.szFieldName=wszToken;
				break;
			case 1:
				//тип поля
				DataTableFieldDescription.etFieldType=SS::Interface::Core::NdxSE::CNdxMetaField::StringToFieldDataTypes(wszToken);
				break;
			case 2:
				//модификатор поля
				DataTableFieldDescription.etFieldModificator=SS::Interface::Core::NdxSE::CNdxMetaField::StringToFieldModificatorTypes(wszToken);
				break;
			default:
				break;
			}

			wszToken=wcstok(NULL, wszSeps);
			k++;			
		}
		
		//добавляем поле в коллекцию
		m_NdxMetaFieldCollection.push_back(
			SS::Interface::Core::NdxSE::CNdxMetaField(DataTableFieldDescription.szFieldName, 
			(EFieldDataTypes)DataTableFieldDescription.etFieldType, 
			(EFieldModificatorTypes)DataTableFieldDescription.etFieldModificator));
	}

}

void CCommonContainersUsing::ViewMetaFieldCollection(void)
{
	wstring sFieldName;
	for (unsigned int i = 0; i < m_NdxMetaFieldCollection.size(); i++)
	{
		wstring szValue;
		sFieldName.assign(m_NdxMetaFieldCollection[i].GetFieldName());

		ToConsole(L"//------DataField");

		if(sFieldName==L"FILEDATE"){
			szValue.append(L"FILEDATE\n");
			FILETIME oFT;
			memcpy((void*)&oFT, (void*)m_NdxMetaFieldCollection[i].GetValue(), 8);
			wchar_t buf[100];
			wsprintf(buf, L"LowerDate: %u HighDate: %u", oFT.dwLowDateTime, oFT.dwHighDateTime);
			szValue.append(buf);
		}else{
			m_NdxMetaFieldCollection[i].ToString(szValue);
		}

		ToConsole(szValue.c_str());
		ToConsole(L"//------Eof-DataField");
	}
}

void CCommonContainersUsing::FillMetaFieldCollection(void)
{
	wstring sFieldName;
	for(unsigned int i=0; i<m_NdxMetaFieldCollection.size(); i++){
		sFieldName.assign(m_NdxMetaFieldCollection[i].GetFieldName());
		
		if(sFieldName==L"FILESIZE"){
			unsigned int uiSize=rand()&0x000000ff;
			m_NdxMetaFieldCollection[i].SetValue((void*)&uiSize, TYPE_UINT32);
		}else if(sFieldName==L"FILENAME"){
			wchar_t buf[100];
			wsprintf(buf, L"d:\\SS\\Texts\\text_%u.txt", m_uiCurrTextNumber);
			wstring wsValue(buf);
			m_NdxMetaFieldCollection[i].SetValue((void*)&wsValue, TYPE_WSTRING);
		}else if(sFieldName==L"FILEDATE"){
			char buf[10];
			memset(buf, 0, 10);
			FILETIME oFT;
			oFT.dwLowDateTime=rand()&0x0000000f;
			oFT.dwHighDateTime=0;
			memcpy((void*)buf, (void*)&oFT, 8);
			m_NdxMetaFieldCollection[i].SetValue((void*)&buf, TYPE_BYTE_10);
		}else if(sFieldName==L"FILEID"){
			m_NdxMetaFieldCollection[i].SetValue((void*)&m_uiCurrTextNumber, TYPE_UINT32);
		}else{
			m_NdxMetaFieldCollection[i].Disable();
		}

	}
}

void CCommonContainersUsing::InitDocumentStructure(void)
{
	m_DocumentStructure.clear();
	
	m_DocumentStructure.push_back(L"HEADERS|HEADERS|TEXT");
	m_DocumentStructure.push_back(L"TABLES");
	m_DocumentStructure.push_back(L"HEADERS|TEXT");
	m_DocumentStructure.push_back(L"HEADERS|TEXT");
	m_DocumentStructure.push_back(L"TABLES");
	m_DocumentStructure.push_back(L"TEXT");
}

void CCommonContainersUsing::FillIndexationResultByStructure(unsigned int uiBlockNumber)
{
	wchar_t seps[]   = L"|";
	wchar_t* wszToken;
	unsigned int uiTemp=0;

	m_pIndexationResult->Clear();

	if(uiBlockNumber>=m_DocumentStructure.size()){ 
		TO_CONSOLE(L"Invalid block number for document structure");
		return;
	}

	ITextBlock* pTextBlock=NULL;
	ITextBlockCut* pTextBlockCut=NULL;
	wszToken = wcstok((wchar_t*)m_DocumentStructure[uiBlockNumber].c_str(), seps);
	while(wszToken != NULL)
	{
		if(!wcscmp(wszToken, L"HEADERS")){
			if(!pTextBlock) pTextBlock=m_pIndexationResult->AddTextBlock();

			//заполняем тексткат заголовка
			pTextBlockCut=pTextBlock->AddBlockCut();
			pTextBlockCut->SetIndexTypeName(L"HEADERS");
			pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctHeader);
			pTextBlockCut->SetLevel(0);
			pTextBlockCut->m_IndexesType=SS::Core::Types::IndexationSearch::itNormal;
			FillTexBlockCut(pTextBlockCut, 10);
		
		}else if(!wcscmp(wszToken, L"TEXT")){
			if(!pTextBlock) pTextBlock=m_pIndexationResult->AddTextBlock();

			//заполняем тексткат текста
			pTextBlockCut=pTextBlock->AddBlockCut();
			pTextBlockCut->SetIndexTypeName(L"TEXT");
			pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctText);
			pTextBlockCut->m_IndexesType=SS::Core::Types::IndexationSearch::itNormal;
			FillTexBlockCut(pTextBlockCut, 500/*50000*/);

		}else if(!wcscmp(wszToken, L"TABLES")){
			//с таблицами не может быть других блоков
			FillIndexationResultTable();
			break;
		}
		wszToken = wcstok(NULL, seps);
	}
}

void CCommonContainersUsing::FillIndexationResultTable(void)
{
	if(ISNULL(m_pIndexationResult)) return;
	
	m_pIndexationResult->Clear();

 	ITextBlock* pTextBlock=NULL;
	ITextBlockCut* pTextBlockCut=NULL;

	for(unsigned int i=0; i<20; i++){
 		pTextBlock=m_pIndexationResult->AddTextBlock();
		for(unsigned int k=0; k<5; k++){
			//заполняем тексткаты таблицы
			pTextBlockCut=pTextBlock->AddBlockCut();
			pTextBlockCut->SetIndexTypeName(L"TABLES");
			pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctTable);
			pTextBlockCut->m_IndexesType=SS::Core::Types::IndexationSearch::itNormal;
			FillTexBlockCut(pTextBlockCut, 3);
		}
	}
}

void CCommonContainersUsing::FillIndexationResult(void)
{
	if(ISNULL(m_pIndexationResult)) return;
	
	m_pIndexationResult->Clear();
 	ITextBlock* pTextBlock=m_pIndexationResult->AddTextBlock();
	//pTextBlock->GetTextFeature()->SetTextNumber(m_uiCurrTextNumber);
	//wchar_t buf[100];
	//wsprintf(buf, L"d:\\SS\\Texts\\text_%u.txt", m_uiCurrTextNumber);
	//pTextBlock->GetTextFeature()->SetTextPath(buf);

	//unsigned char* pTextInfoBuffer=NULL;
	//unsigned int uiTextInfoBufferSize=0;
	////получаем буфер инфы по тексту
	//pTextBlock->GetTextFeature()->GetTextFeatureBuff((char**)&pTextInfoBuffer, &uiTextInfoBufferSize);
	//if(pTextInfoBuffer==NULL){
	//	ISNULL(pTextInfoBuffer);
	//	ToConsole(buf);
	//}	

	ITextBlockCut* pTextBlockCut=NULL;

	//заполняем тексткат заголовка
	pTextBlockCut=pTextBlock->AddBlockCut();
	pTextBlockCut->SetIndexTypeName(L"HEADERS");
	pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctHeader);
	pTextBlockCut->SetLevel(0);
	pTextBlockCut->m_IndexesType=SS::Core::Types::IndexationSearch::itNormal;
	FillTexBlockCut(pTextBlockCut, 10);

	//заполняем тексткат заголовка
	pTextBlockCut=pTextBlock->AddBlockCut();
	pTextBlockCut->SetIndexTypeName(L"HEADERS");
	pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctHeader);
	pTextBlockCut->SetLevel(1);
	pTextBlockCut->m_IndexesType=SS::Core::Types::IndexationSearch::itNormal;
	FillTexBlockCut(pTextBlockCut, 10);


	//заполняем тексткат текста
	pTextBlockCut=pTextBlock->AddBlockCut();
	pTextBlockCut->SetIndexTypeName(L"TEXT");
	pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctText);
	pTextBlockCut->m_IndexesType=SS::Core::Types::IndexationSearch::itNormal;
	FillTexBlockCut(pTextBlockCut, 100/*50000*/);

	//заполняем тексткат заголовка
	pTextBlockCut=pTextBlock->AddBlockCut();
	pTextBlockCut->SetIndexTypeName(L"HEADERS");
	pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctHeader);
	pTextBlockCut->SetLevel(1);
	pTextBlockCut->m_IndexesType=SS::Core::Types::IndexationSearch::itNormal;
	FillTexBlockCut(pTextBlockCut, 10);

	//заполняем тексткат текста
	pTextBlockCut=pTextBlock->AddBlockCut();
	pTextBlockCut->SetIndexTypeName(L"TEXT");
	pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctText);
	pTextBlockCut->m_IndexesType=SS::Core::Types::IndexationSearch::itNormal;
	FillTexBlockCut(pTextBlockCut, 100/*50000*/);

	//заполняем тексткат заголовка
	pTextBlockCut=pTextBlock->AddBlockCut();
	pTextBlockCut->SetIndexTypeName(L"HEADERS");
	pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctHeader);
	pTextBlockCut->SetLevel(1);
	pTextBlockCut->m_IndexesType=SS::Core::Types::IndexationSearch::itNormal;
	FillTexBlockCut(pTextBlockCut, 10);

	FillIndexationResultTable();
	pTextBlock=m_pIndexationResult->AddTextBlock();

	////заполняем тексткат текста
	//pTextBlockCut=pTextBlock->AddBlockCut();
	//pTextBlockCut->SetIndexTypeName(L"TEXT");
	//pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctText);
	//pTextBlockCut->m_IndexesType=SS::Core::Types::IndexationSearch::itNormal;
	//FillTexBlockCut(pTextBlockCut, 100/*50000*/);

	////заполняем тексткат заголовка
	//pTextBlockCut=pTextBlock->AddBlockCut();
	//pTextBlockCut->SetIndexTypeName(L"HEADERS");
	//pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctHeader);
	//pTextBlockCut->SetLevel(2);
	//pTextBlockCut->m_IndexesType=SS::Core::Types::IndexationSearch::itNormal;
	//FillTexBlockCut(pTextBlockCut, 10);

	////заполняем тексткат текста
	//pTextBlockCut=pTextBlock->AddBlockCut();
	//pTextBlockCut->SetIndexTypeName(L"TEXT");
	//pTextBlockCut->SetType(SS::Core::Types::IndexationSearch::tctText);
	//pTextBlockCut->m_IndexesType=SS::Core::Types::IndexationSearch::itNormal;
	//FillTexBlockCut(pTextBlockCut, 100/*50000*/);
}	

void CCommonContainersUsing::FillTexBlockCut(ITextBlockCut* pTextBlockCut, unsigned int uiIndexesCount)
{
	if(ISNULL(pTextBlockCut)) return;

	unsigned int uiIndex=0;
	unsigned int arIndexes[5]={1, 2, 3, 4, 5};
	//unsigned int arIndexes[7]={1, 1, 1, 1, 1, 1, 1};
	wchar_t buf[50];
	for(unsigned int i=0; i<uiIndexesCount; i++){
		IIndexAndPosition* pIndexAndPosition=pTextBlockCut->AddIndexAndPosition();	
		pIndexAndPosition->SetPosition(m_uiCurrPosition);
		pIndexAndPosition->SetFirstCharPosition(m_uiCurrPosition*10);

		//получаем случайное число ограничиваем его до 256
		//uiIndex=rand()&0x000000ff;
		//uiIndex=(rand()*rand())&0x0001ffff;
		//uiIndex=arIndexes[i];
		for(int i=0; i<12;i++) uiIndex+=rand()&0x00ffffff;
		uiIndex/=6;

		if(uiIndex==0) uiIndex=1;
		
		//ToConsole(L"uiIndex", uiIndex);

		pIndexAndPosition->GetIndexationIndex()->AddIndex(uiIndex);
		SS::Core::Types::IndexationSearch::TViewIndex ViewIndex;
		ViewIndex.m_DictionaryIndex=uiIndex;
		pIndexAndPosition->GetServiceIndex()->SetInfoBuff(reinterpret_cast<char*>(&ViewIndex), sizeof(ViewIndex));
		
		wsprintf(buf, L"w_%u", uiIndex);
		pIndexAndPosition->SetWord(buf);

		if(pTextBlockCut->m_IndexesType==SS::Core::Types::IndexationSearch::itNormal)
			m_uiCurrPosition++;
	}
}

void CCommonContainersUsing::FillQueryResult(void)
{
	if(ISNULL(m_pQueryResult)) return;
	
	m_pQueryResult->Clear();

	MakeQueryFromString(m_wszCurrentQuery);

//(((((100687477 <-0-0-0> | 100687476 <-0-0-0> | 100678712 <-0-0-0> | 100672785 <-0-0-0> | 100672421 <-0-0-0> )))& 
//((((83920533 <-1-0-0> | 83920532 <-1-0-0> | 83920531 <-1-0-0> | 83920530 <-1-0-0> )))))| 
//((((83920533 <-0-0-1> | 83920532 <-0-0-1> | 83920531 <-0-0-1> | 83920530 <-0-0-1> )))& 
//((((100687477 <-1-0-1> | 100687476 <-1-0-1> | 100678712 <-1-0-1> | 100672785 <-1-0-1> | 100672421 <-1-0-1> ))))))
//wstring wszValue(L"(((((100687477 <-0-0-0> | 100687476 <-0-0-0> | 100678712 <-0-0-0> | 100672785 <-0-0-0> | 100672421 <-0-0-0> )))& ((((83920533 <-1-0-0> | 83920532 <-1-0-0> | 83920531 <-1-0-0> | 83920530 <-1-0-0> )))))| ((((83920533 <-0-0-1> | 83920532 <-0-0-1> | 83920531 <-0-0-1> | 83920530 <-0-0-1> )))& ((((100687477 <-1-0-1> | 100687476 <-1-0-1> | 100678712 <-1-0-1> | 100672785 <-1-0-1> | 100672421 <-1-0-1> ))))))");

	//SQueryIndexAttributes IndexAttributes;
	//IQueryIndex* pQueryIndex=NULL;

	//IndexAttributes.m_TransformationID=0;
	//IndexAttributes.m_SequenceID=0;
	//IndexAttributes.m_IndexPosition=0;


	//pQueryIndex=m_pQueryResult->AddQueryIndex();
	//pQueryIndex->SetLeftBrackets(1);
	//pQueryIndex->SetOperator('&');
	//pQueryIndex->SetQueryIndex(41);
	//IndexAttributes.m_IndexPosition++;
	//pQueryIndex->SetAttributes(IndexAttributes);

	//pQueryIndex=m_pQueryResult->AddQueryIndex();
	//pQueryIndex->SetOperator('&');
	//pQueryIndex->SetQueryIndex(35);
	//IndexAttributes.m_IndexPosition++;
	//pQueryIndex->SetAttributes(IndexAttributes);

	//pQueryIndex=m_pQueryResult->AddQueryIndex();
	//pQueryIndex->SetQueryIndex(190);
	//IndexAttributes.m_IndexPosition++;
	//pQueryIndex->SetAttributes(IndexAttributes);
	//pQueryIndex->SetRightBrackets(1);

}

unsigned int CCommonContainersUsing::GetTogetherCharsNumber(wchar_t* wszToken, wchar_t wsChar)
{
	wchar_t* wszTemp;
	unsigned int uiTemp;

	//определяем количество нужных символов
	wszTemp=wcschr(wszToken, wsChar);
	uiTemp=0;
	while(wszTemp){
		if(*wszTemp==wsChar){
			uiTemp++;					
			wszTemp++;
		}else break;
	}

	return uiTemp;
}

void CCommonContainersUsing::MakeQueryIndexFromString(wstring& wszValue, char chOperator)
{
	wchar_t sepsForIndex[]   = L"( <";
	wchar_t sepsForIndexAttr[]   = L"<-> ";
	wchar_t wszFullIndexToken[100];
	wchar_t wszForIndexAttrToken[100];
	wchar_t* wszTempToken;
	wchar_t* wszIndexToken;
	wchar_t* wszIndexAttrToken;
	unsigned int uiTemp=0;

	wcscpy(wszFullIndexToken, wszValue.c_str());
	wcscpy(wszForIndexAttrToken, wszValue.c_str());
	
	IQueryIndex* pQueryIndex=NULL;
	pQueryIndex=m_pQueryResult->AddQueryIndex();


	//определяем количество открыващих скобок
	uiTemp=GetTogetherCharsNumber(wszFullIndexToken, '(');
	pQueryIndex->SetLeftBrackets(uiTemp);
	//определяем количество закрывающих скобок
	uiTemp=GetTogetherCharsNumber(wszFullIndexToken, ')');
	pQueryIndex->SetRightBrackets(uiTemp);
	
	//вытаскиваем индекс
	wszIndexToken = wcstok(wszFullIndexToken, sepsForIndex);
	pQueryIndex->SetQueryIndex(_wtoi(wszIndexToken));

	//вытаскиваем аттрибуты индекса
	SQueryIndexAttributes IndexAttributes;
	IndexAttributes.m_TransformationID=0;
	IndexAttributes.m_SequenceID=0;
	IndexAttributes.m_IndexPosition=0;
	wszTempToken=wcschr(wszForIndexAttrToken, sepsForIndexAttr[0]);
	wszIndexAttrToken = wcstok(wszTempToken, sepsForIndexAttr);
	unsigned int uiAttrInc=0;
	while(wszIndexAttrToken != NULL)
	{
		switch(uiAttrInc){
		case 0:
			IndexAttributes.m_IndexPosition=(char)_wtoi(wszIndexAttrToken);
			break;
		case 1:
			IndexAttributes.m_SequenceID=(char)_wtoi(wszIndexAttrToken);
			break;
		case 2:
			IndexAttributes.m_TransformationID=(char)_wtoi(wszIndexAttrToken);
			break;
		case 3:
			IndexAttributes.m_Obligatory=(char)_wtoi(wszIndexAttrToken);
			break;
		case 4:
			IndexAttributes.m_WordsBlockID=(char)_wtoi(wszIndexAttrToken);
			break;
		case 5:
			IndexAttributes.m_bAnswerIndex=(char)_wtoi(wszIndexAttrToken);
			break;
		default:
			break;
		}
		uiAttrInc++;
		wszIndexAttrToken = wcstok(NULL, sepsForIndexAttr);
	}
	pQueryIndex->SetAttributes(IndexAttributes);
	pQueryIndex->SetOperator(chOperator);
}

void CCommonContainersUsing::MakeQueryFromString(wstring& wszValue)
{
	vector<wstring> vQueryIndexStrings;
	vector<char> vQueryIndexOperator;
	wchar_t seps[]   = L"|&";
	
	//вытаскиваем операторы
	for(unsigned int i=0; i<wszValue.size(); i++){
		if(wszValue[i]==seps[0])
			vQueryIndexOperator.push_back('|');
		if(wszValue[i]==seps[1])
			vQueryIndexOperator.push_back('&');
	}
	vQueryIndexOperator.push_back(' ');

	//вытаскиваем строки индексов
	wchar_t* wszToken;
	wszToken = wcstok((wchar_t*)wszValue.c_str(), seps);
	while(wszToken != NULL)
	{
		vQueryIndexStrings.push_back(wstring(wszToken));
		wszToken = wcstok(NULL, seps);
	}
	
	if(vQueryIndexStrings.size()!=vQueryIndexOperator.size()){
		TO_CONSOLE(L"vQueryIndexStrings and vQueryIndexOperator sizes not equal");
		return;
	}

	//наполняем QueryResult
	for(unsigned int i=0; i<vQueryIndexStrings.size(); i++){
		MakeQueryIndexFromString(vQueryIndexStrings[i], vQueryIndexOperator[i]);
	}
}

void CCommonContainersUsing::QueryResultToString(wstring* pwszQueryResult)
{
	wstring szValue;

	if(ISNULL(GetQueryResult())) return;

	if(!GetQueryResult()->GetFirstQueryIndex()){ 
		szValue.append(L"---QueryResult empty!!!---");
		ToConsole(szValue.c_str());
		return;
	}

	wchar_t buf[200];
	IQueryIndex* pQueryIndex=GetQueryResult()->GetFirstQueryIndex();
	while(pQueryIndex){
		//левые скобки
		wstring wszLeftBracket(pQueryIndex->GetLeftBrackets(), '(');	
		szValue.append(wszLeftBracket);
		
		wsprintf(buf, L"%u <%u-%u-%u-%u-%u-%u> ",
			pQueryIndex->GetQueryIndex(), 
			(unsigned int)pQueryIndex->GetAttributes().m_IndexPosition,
			(unsigned int)pQueryIndex->GetAttributes().m_SequenceID,
			(unsigned int)pQueryIndex->GetAttributes().m_TransformationID,
			(unsigned int)pQueryIndex->GetAttributes().m_Obligatory,
			(unsigned int)pQueryIndex->GetAttributes().m_WordsBlockID,
			(unsigned int)pQueryIndex->GetAttributes().m_bAnswerIndex);	
		szValue.append(buf);
		
		//правые скобки
		wstring wszRightBracket(pQueryIndex->GetRightBrackets(), ')');	
		szValue.append(wszRightBracket);
		
		if(pQueryIndex->GetOperator()=='&' || pQueryIndex->GetOperator()=='|'){
			//логический оператор
			wstring wszOperator(1, pQueryIndex->GetOperator());
			szValue.append(wszOperator);
			szValue.append(L" ");
		}

		pQueryIndex=GetQueryResult()->GetNextQueryIndex();
	}

	pwszQueryResult->assign(szValue.c_str());
}

void CCommonContainersUsing::GetQueries(wstring& wsFilePath, vector<wstring>& Queries)
{
	FILE* pFileStream=_wfopen(wsFilePath.c_str(), L"r");
	if(ISNULL(pFileStream)) return;

	char szLine[10000];
	wchar_t wszLine[10000];
	while(!feof(pFileStream)){
		szLine[0]=0;
		wszLine[0]=0;

		if(!fgets(szLine, 10000, pFileStream))
			break;

		szLine[strlen(szLine)-1]=0;
		MultiByteToWideChar(CP_ACP, 0, szLine, -1, wszLine, 10000); 
		wszLine[strlen(szLine)]=0;
		Queries.push_back(wszLine);
	}

	fclose(pFileStream);
}

//--------------------------------------------------------------------//

}
}
}
}

#endif //_SS_UNITTESTS