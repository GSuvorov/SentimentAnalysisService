#include "StdAfx.h"
#include ".\search_engine.h"
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

using namespace SS::Core::NdxSE::NdxProcessor::Storages;
using namespace SS::Interface::Core::NdxSE;
//--------------------------------------------------------------------//

CNdxSearchEngine::CNdxSearchEngine(void)
:m_pStorageManager(new CStorageManager())
{
}

CNdxSearchEngine::~CNdxSearchEngine(void)
{
	if(m_pStorageManager) delete m_pStorageManager;
}

HRESULT CNdxSearchEngine::QueryInterface(REFIID pIID, void** pBase) 
{
	if(pIID==IID_Base){
		*pBase=(SS::Interface::IBase*)this;
	}else if(pIID==IID_NdxSearchEngine){
		*pBase=(SS::Interface::Core::NdxSE::INdxSearchEngine*)this;
	}else if(pIID==IID_TextsInfoExtracting){
		*pBase=(SS::Interface::Core::TextsInfo::ITextsInfoExtracting*)this;
	}else{
		*pBase=NULL;
	}
	
	return S_OK;
}

ULONG CNdxSearchEngine::Release(void)
{
	delete this;
	return 0;
}

void CNdxSearchEngine::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	if(ISNULL(pLoadManager)) return;

	SS::Interface::IBase::SetLoadManager(pLoadManager);

	//получаем менеджер индексной базы
	INdxBaseManager* pNdxBaseManager=(INdxBaseManager*)GetLoadManager()->
	GetInterface(L"./core/NdxBaseManager.dll", CLSID_NdxBaseManager, IID_NdxBaseManager);

	SS::Interface::Core::CommonServices::ISettingsServer* pSettingsServer=
	(SS::Interface::Core::CommonServices::ISettingsServer*)GetLoadManager()->
		GetInterface(L"./core/GSServer.dll", CLSID_SettingsServer , IID_SettingsServer);
	
	m_pStorageManager->Init(pNdxBaseManager);

	//устанавливаем у менеджеров указатель на менеджер хранилищ
	Indexing::CIndexingManager::Init(m_pStorageManager);	
	Extracting::CExtractingManager::Init(m_pStorageManager, pSettingsServer, (Extracting::CTextInfoManager*)this);	
	Extracting::CTextInfoManager::Init(m_pStorageManager);
}

void CNdxSearchEngine::GetTextInfo(unsigned int uiTextIndex, 
	SS::Interface::Core::CommonContainers::ITextFeature* pTextFeature)
{
	if(ISNULL(GetLoadManager())) SS_THROW(L"GetLoadManager() is NULL");

	Extracting::CTextInfoManager::GetTextInfo(uiTextIndex, pTextFeature);
}

void CNdxSearchEngine::FillTextContent(wstring* pwsIndexStorageName, 
		unsigned int uiHeadPosition, unsigned int uiTailPosition,
		SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock)
{
	if(ISNULL(GetLoadManager())) SS_THROW(L"GetLoadManager() is NULL");
	if(ISNULL(pwsIndexStorageName)) SS_THROW(L"pwsIndexStorageName is NULL");
	if(ISNULL(pTextBlock)) SS_THROW(L"pTextBlock is NULL");

	SS_TRY

		m_pStorageManager->Update();

		if(!m_pStorageManager->IsExtractingMode()){
			///если режим извлечения не выставлен ничего не получится
			TO_CONSOLE(L"Extracting not set, stop indexation before!!!");
			SS_THROW(L"Extracting not set, stop indexation before!!!");
		}
		
		//формируем интервал
		Containers::CInterval Interval(uiHeadPosition, uiTailPosition);
		//получаем индексное хранилище
		CIndexStorage* pIndexStorage=m_pStorageManager->GetIndexStorage(pwsIndexStorageName->c_str());

		if(ISNULL(pIndexStorage)) SS_THROW(L"pIndexStorage is NULL");
		//достаем сервисную информацию
		pIndexStorage->GetTextsBinaryStorage()->FillTextBlock(pTextBlock, &Interval, pwsIndexStorageName);

	SS_CATCH(L"");
}

//--------------------------------------------------------------------//

}
}
}
}