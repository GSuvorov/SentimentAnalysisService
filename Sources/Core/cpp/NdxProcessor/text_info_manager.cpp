#include "StdAfx.h"
#include ".\text_info_manager.h"
#include "..\ASCInterface\defines.h"

USES_LOAD_MANAGER;

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Extracting
{

using namespace SS::Interface::Core::CommonContainers;
using namespace SS::Interface::Core::NdxSE;

//--------------------------------------------------------------------//

CTextInfoManager::CTextInfoManager(void) : m_pStorageManager(NULL)
{
}

CTextInfoManager::~CTextInfoManager(void)
{
}

void CTextInfoManager::Init(SS::Core::NdxSE::NdxProcessor::Storages::CStorageManager* pStorageManager)
{
	if(ISNULL(pStorageManager)) SS_THROW(L"pStorageManager is NULL");

	m_pStorageManager=pStorageManager;
}

void CTextInfoManager::GetTextInfo(unsigned int uiTextIndex, ITextFeature* pTextFeature)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	if(ISNULL(pTextFeature)) SS_THROW(L"pTextFeature is NULL");
	
	SS_TRY
		m_pStorageManager->Update();

		if(ISNULL(m_pStorageManager->GetTextsInfoStorage())) SS_THROW(L"GetTextsInfoStorage is NULL");
	
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}

		m_pStorageManager->GetTextsInfoStorage()->GetTextInfo(uiTextIndex, pTextFeature);

	SS_CATCH(L"");
}

void CTextInfoManager::GetTextInfo(unsigned int uiTextIndex, CNdxMetaField* pNdxMetaField)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	if(ISNULL(pNdxMetaField)) SS_THROW(L"pNdxMetaField is NULL");
	
	SS_TRY
		m_pStorageManager->Update();

		if(ISNULL(m_pStorageManager->GetTextsInfoStorage())) SS_THROW(L"GetTextsInfoStorage is NULL");
	
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}

		m_pStorageManager->GetTextsFieldsStorage()->GetTextInfo(uiTextIndex, pNdxMetaField);

		//ToConsole(L"//--------GetTextInfo");
		//ToConsole(L"TextIndex", uiTextIndex);
		//wstring wsValue;
		////CNdxMetaField NdxMetaField((*pNdxMetaField));
		////NdxMetaField.ToString(wsValue);
		//pNdxMetaField->ToString(wsValue);
		//ToConsole(wsValue.c_str());
		//ToConsole(L"//--------Eof-GetTextInfo");

	SS_CATCH(L"");
}

void CTextInfoManager::GetTextInfo(unsigned int uiTextIndex, TNdxMetaFieldCollection* pNdxMetaFieldCollection)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	if(ISNULL(pNdxMetaFieldCollection)) SS_THROW(L"pNdxMetaFieldCollection is NULL");
	
	SS_TRY
		m_pStorageManager->Update();

		if(ISNULL(m_pStorageManager->GetTextsInfoStorage())) SS_THROW(L"GetTextsInfoStorage is NULL");
	
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}

		m_pStorageManager->GetTextsFieldsStorage()->GetTextInfo(uiTextIndex, pNdxMetaFieldCollection);

	ToConsole(L"//--------GetTextInfo1");
	ToConsole(L"TextIndex", uiTextIndex);
	for(TNdxMetaFieldCollection::iterator itNdxMetaField=pNdxMetaFieldCollection->begin(); itNdxMetaField!=pNdxMetaFieldCollection->end(); itNdxMetaField++){
		wstring wsValue;
		if(itNdxMetaField->GetFieldType()!=SS::Interface::Core::DBMS::TYPE_WSTRING){
			itNdxMetaField->ToString(wsValue);
			ToConsole(wsValue.c_str());
		}else{
			if(itNdxMetaField->GetValue())
				ToConsole(((wstring*)itNdxMetaField->GetValue())->c_str());
		}
	}
	ToConsole(L"//--------Eof-GetTextInfo1");

	SS_CATCH(L"");
}

unsigned int CTextInfoManager::GetTextsNumber(void)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	
	SS_TRY
		m_pStorageManager->Update();

		if(ISNULL(m_pStorageManager->GetTextsInfoStorage())) SS_THROW(L"GetTextsInfoStorage is NULL");
	
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}

		return m_pStorageManager->GetTextsFieldsStorage()->GetTextsNumber();

	SS_CATCH(L"");
	return 0;
}

void CTextInfoManager::GetTextPath(unsigned int uiTextIndex, std::wstring* pPath)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	if(ISNULL(pPath)) SS_THROW(L"pPath is NULL");
	
	SS_TRY
		m_pStorageManager->Update();
	
		if(ISNULL(m_pStorageManager->GetTextsInfoStorage())) SS_THROW(L"GetTextsInfoStorage is NULL");
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}
		
		SS::Interface::Core::NdxSE::CNdxMetaField NdxMetaField(
			L"FILENAME", 
			SS::Interface::Core::DBMS::TYPE_WSTRING, 
			SS::Interface::Core::DBMS::MOD_COMMON);
		m_pStorageManager->GetTextsFieldsStorage()->GetTextInfo(uiTextIndex, &NdxMetaField);
		pPath->assign(((std::wstring*)NdxMetaField.GetValue())->c_str());

	SS_CATCH(L"");
}

unsigned int CTextInfoManager::GetTextSize(unsigned int uiTextIndex)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	
	SS_TRY
		m_pStorageManager->Update();
	
		if(ISNULL(m_pStorageManager->GetTextsInfoStorage())) SS_THROW(L"GetTextsInfoStorage is NULL");
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}

		SS::Interface::Core::NdxSE::CNdxMetaField NdxMetaField(
			L"FileSize", 
			SS::Interface::Core::DBMS::TYPE_UINT32, 
			SS::Interface::Core::DBMS::MOD_COMMON);
		m_pStorageManager->GetTextsFieldsStorage()->GetTextInfo(uiTextIndex, &NdxMetaField);
		unsigned int uiTextSize=0;
		memcpy((void*)&uiTextSize, NdxMetaField.GetValue(), sizeof(unsigned int));

		return uiTextSize;
	SS_CATCH(L"");
	return 0;
}

void CTextInfoManager::DeleteText(unsigned int uiTextIndex)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	
	SS_TRY
		m_pStorageManager->Update();
	
		if(ISNULL(m_pStorageManager->GetTextsInfoStorage())) SS_THROW(L"GetTextsInfoStorage is NULL");
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}
		
		//удаляем текст во всех хранилищах
		m_pStorageManager->GetTextsInfoStorage()->DeleteText(uiTextIndex);

		SS::Interface::Core::NdxSE::CNdxMetaField NdxMetaField(
			L"FilePath", 
			SS::Interface::Core::DBMS::TYPE_WSTRING, 
			SS::Interface::Core::DBMS::MOD_COMMON);
		m_pStorageManager->GetTextsFieldsStorage()->GetTextInfo(uiTextIndex, &NdxMetaField);
		
		m_pStorageManager->GetTextsPathsTreeStorage()->DeleteTextByPath((const wchar_t*)NdxMetaField.GetValue(), NULL);
		m_pStorageManager->GetTextsFieldsStorage()->DeleteText(uiTextIndex);

	SS_CATCH(L"");
}

unsigned int CTextInfoManager::IsTextDeleted(unsigned int uiTextIndex)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	
	SS_TRY
		m_pStorageManager->Update();
	
		if(ISNULL(m_pStorageManager->GetTextsInfoStorage())) SS_THROW(L"GetTextsInfoStorage is NULL");
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}

		return m_pStorageManager->GetTextsInfoStorage()->IsTextDeleted(uiTextIndex);

	SS_CATCH(L"");
	return 0;
}

void CTextInfoManager::GetHeadersStructure(unsigned int uiTextIndex, std::wstring* pwsHeaderStructure)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	if(ISNULL(pwsHeaderStructure)) SS_THROW(L"pwsHeaderStructure is NULL");
	
	SS_TRY
		m_pStorageManager->Update();
	
		if(ISNULL(m_pStorageManager->GetIndexStorage(L"HEADERS"))) SS_THROW(L"m_pStorageManager->GetIndexStorage is NULL");
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}

		//проверяем есть в результате куски заголовков
		Storages::CIndexStorageHeaders* pIndexStorageHeaders=
			static_cast<Storages::CIndexStorageHeaders*>(m_pStorageManager->GetIndexStorage(L"HEADERS"));

		Containers::CDocumentHeadersInfo* pDocumentHeadersInfo=
			pIndexStorageHeaders->FillHeadersInfo(uiTextIndex);

		if(pDocumentHeadersInfo){
			std::wstring wsValue;
			pDocumentHeadersInfo->ToString(wsValue);
			pwsHeaderStructure->assign(wsValue);
		}
	SS_CATCH(L"");
	return;
}

void CTextInfoManager::GetTablesStructure(unsigned int uiTextIndex, std::wstring* pwsTablesStructure)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	if(ISNULL(pwsTablesStructure)) SS_THROW(L"pwsTablesStructure is NULL");
	
	SS_TRY
		m_pStorageManager->Update();
	
		if(ISNULL(m_pStorageManager->GetIndexStorage(L"TABLES"))) SS_THROW(L"m_pStorageManager->GetIndexStorage is NULL");
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}

		//проверяем есть в результате куски заголовков
		Storages::CIndexStorageTables* pIndexStorageTables=
			static_cast<Storages::CIndexStorageTables*>(m_pStorageManager->GetIndexStorage(L"TABLES"));
		
		std::wstring wsValue;
		pIndexStorageTables->FillTablesStructure(uiTextIndex, &wsValue);
		pwsTablesStructure->assign(wsValue);

	SS_CATCH(L"");
	return;
}

//--------------------------------------------------------------------//

void CTextInfoManager::FillTextPathChunk(STextPathChunk* pTextPathChunk)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	if(ISNULL(pTextPathChunk)) SS_THROW(L"pTextPathChunk is NULL");
	
	SS_TRY
		m_pStorageManager->Update();
	
		if(ISNULL(m_pStorageManager->GetTextsPathsTreeStorage())) SS_THROW(L"GetTextsPathsTreeStorage is NULL");
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}

		m_pStorageManager->GetTextsPathsTreeStorage()->FillTextPathChunk(pTextPathChunk);

	SS_CATCH(L"");
}

void CTextInfoManager::GetTextPathChunkChildsOffsets(STextPathChunk* pTextPathChunk, std::vector<unsigned int>* pChildsOffsetsCollection)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	if(ISNULL(pTextPathChunk)) SS_THROW(L"pTextPathChunk is NULL");
	if(ISNULL(pChildsOffsetsCollection)) SS_THROW(L"pChildsOffsetsCollection is NULL");

	SS_TRY
		m_pStorageManager->Update();
	
		if(ISNULL(m_pStorageManager->GetTextsPathsTreeStorage())) SS_THROW(L"GetTextsPathsTreeStorage is NULL");
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}

		m_pStorageManager->GetTextsPathsTreeStorage()->GetChildsOffsets(pTextPathChunk, pChildsOffsetsCollection);

	SS_CATCH(L"");
}

void CTextInfoManager::GetTextPathChunkParentOffset(STextPathChunk* pTextPathChunk, unsigned int* pParentOffset)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	if(ISNULL(pTextPathChunk)) SS_THROW(L"pTextPathChunk is NULL");
	if(ISNULL(pParentOffset)) SS_THROW(L"pParentOffset is NULL");

	SS_TRY
		m_pStorageManager->Update();
	
		if(ISNULL(m_pStorageManager->GetTextsPathsTreeStorage())) SS_THROW(L"GetTextsPathsTreeStorage is NULL");
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}

		m_pStorageManager->GetTextsPathsTreeStorage()->GetParentOffset(pTextPathChunk, pParentOffset);

	SS_CATCH(L"");
}

void CTextInfoManager::DeleteTextPathChunk(STextPathChunk* pTextPathChunk)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	if(ISNULL(pTextPathChunk)) SS_THROW(L"pTextPathChunk is NULL");
	
	SS_TRY
		m_pStorageManager->Update();
	
		if(ISNULL(m_pStorageManager->GetTextsInfoStorage())) SS_THROW(L"GetTextsInfoStorage is NULL");
		if(ISNULL(m_pStorageManager->GetTextsPathsTreeStorage())) SS_THROW(L"GetTextsPathsTreeStorage is NULL");
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}

		vector<unsigned int> vDeletedTextID;
		m_pStorageManager->GetTextsPathsTreeStorage()->DeleteTextPathChunk(pTextPathChunk, &vDeletedTextID);

		//удаляем текст во всех хранилищах
		for(vector<unsigned int>::iterator itTextID=vDeletedTextID.begin(); itTextID!=vDeletedTextID.end(); itTextID++){
			m_pStorageManager->GetTextsInfoStorage()->DeleteText(*itTextID);
 			m_pStorageManager->GetTextsFieldsStorage()->DeleteText(*itTextID);
		}

	SS_CATCH(L"");
}

unsigned int CTextInfoManager::IsTextPathExist(const wchar_t* pwcszTextPath)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	if(ISNULL(pwcszTextPath)) SS_THROW(L"pwcszTextPath is NULL");
	
	SS_TRY
		m_pStorageManager->Update();
	
		if(ISNULL(m_pStorageManager->GetTextsPathsTreeStorage())) SS_THROW(L"GetTextsPathsTreeStorage is NULL");
		//if(!m_pStorageManager->IsExtractingMode()){
		//	///если режим извлечения не выставлен ничего не получится
		//	TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
		//	SS_THROW(L"Extracting not set, stop indexation before!!!");
		//}

		return m_pStorageManager->GetTextsPathsTreeStorage()->IsTextPathExist(pwcszTextPath);

	SS_CATCH(L"");
	return 0;
}

void CTextInfoManager::DeleteTextByPath(const wchar_t* pwcszTextPath)
{
	if(ISNULL(m_pStorageManager)) SS_THROW(L"m_pStorageManager is NULL");
	if(ISNULL(pwcszTextPath)) SS_THROW(L"pwcszTextPath is NULL");
	
	SS_TRY
		m_pStorageManager->Update();
	
		if(ISNULL(m_pStorageManager->GetTextsPathsTreeStorage())) SS_THROW(L"GetTextsPathsTreeStorage is NULL");
		if(ISNULL(m_pStorageManager->GetTextsInfoStorage())) SS_THROW(L"GetTextsInfoStorage is NULL");
		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}
		
		vector<unsigned int> vDeletedTextID;
		m_pStorageManager->GetTextsPathsTreeStorage()->DeleteTextByPath(pwcszTextPath, &vDeletedTextID);

		//удаляем текст во всех хранилищах
		for(vector<unsigned int>::iterator itTextID=vDeletedTextID.begin(); itTextID!=vDeletedTextID.end(); itTextID++){
			m_pStorageManager->GetTextsInfoStorage()->DeleteText(*itTextID);
			m_pStorageManager->GetTextsFieldsStorage()->DeleteText(*itTextID);
		}

	SS_CATCH(L"");
}

//--------------------------------------------------------------------//

}
}
}
}
}