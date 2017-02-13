#pragma once

#include ".\storage_manager.h"
#include ".\indexing_manager.h"
#include ".\extracting_manager.h"
#include ".\text_info_manager.h"
#include "../ASCInterface/ITextsInfoExtracting.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{

//--------------------------------------------------------------------//

///поисквый движок SS осуществляет индексацию и поиск
class CNdxSearchEngine : public SS::Interface::Core::NdxSE::INdxSearchEngine,
	public SS::Interface::Core::TextsInfo::ITextsInfoExtracting,
	public Indexing::CIndexingManager,
	public Extracting::CExtractingManager, 
	public Extracting::CTextInfoManager
{
public:
	CNdxSearchEngine(void);
	virtual ~CNdxSearchEngine(void);

	//-----------IBase
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	///освобождение
	ULONG Release(void);
	///устанавливает указатель на лоад мэнеджер
	void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
	//-----------Eof-IBase

	//-----------INdxSearchEngine
	///возвращает интерфейс для выполнения поиска
	SS::Interface::Core::NdxSE::INdxSearchEngineFind* INdxSearchEngineFind(void){return (SS::Interface::Core::NdxSE::INdxSearchEngineFind*)this;};
	///возвращает интерфейс для выполнения индексации
	SS::Interface::Core::NdxSE::INdxSearchEngineIndexation* INdxSearchEngineIndexation(void){return (SS::Interface::Core::NdxSE::INdxSearchEngineIndexation*)this;};
	///возвращает интерфейс для получения информации по проиндексированным текстам
	SS::Interface::Core::NdxSE::INdxSearchEngineTextsInfo* INdxSearchEngineTextsInfo(void){return (SS::Interface::Core::NdxSE::INdxSearchEngineTextsInfo*)this;};
	///возвращает интерфейс для получения информации по путям проиндексированных текстов
	SS::Interface::Core::NdxSE::INdxSearchEngineTextsPathsInfo* INdxSearchEngineTextsPathsInfo(void){return (SS::Interface::Core::NdxSE::INdxSearchEngineTextsPathsInfo*)this;};
	//-----------Eof-INdxSearchEngine

	//-----------ITextsInfoExtracting
	///заполняет ITextFeature по номеру текста
	void GetTextInfo(unsigned int uiTextIndex, 
		SS::Interface::Core::CommonContainers::ITextFeature* pTextFeature);
	///извлекает бинарное содержимое текста для данного индексного хранилища по номеру текста и добавляет его в ITextBlock
	void FillTextContent(wstring* pwsIndexStorageName, 
			unsigned int uiHeadPosition,
			unsigned int uiTailPosition,
			SS::Interface::Core::CommonContainers::ITextBlock* pTextBlock);
	//-----------Eof-ITextsInfoExtracting

protected:
	///менеджер логических хранилищ индексной базы
	Storages::CStorageManager* m_pStorageManager;
};

//--------------------------------------------------------------------//

}
}
}
}