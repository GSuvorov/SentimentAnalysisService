#pragma once

#include "..\ASDInterface\INdxSearchEngine.h"
#include ".\storage_manager.h"

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

//--------------------------------------------------------------------//

///управляет извлечением п проиндексированным текстам
class CTextInfoManager:
	public SS::Interface::Core::NdxSE::INdxSearchEngineTextsInfo, 
	public SS::Interface::Core::NdxSE::INdxSearchEngineTextsPathsInfo 
{
public:
	CTextInfoManager(void);
	virtual ~CTextInfoManager(void);

	//--------------ITextsInfo
	///возвращает информацию по тексту
	void GetTextInfo(unsigned int uiTextIndex, 
		SS::Interface::Core::CommonContainers::ITextFeature* pTextFeature);
	///возвращает информацию по тексту
	void GetTextInfo(unsigned int uiTextIndex, SS::Interface::Core::NdxSE::CNdxMetaField* pNdxMetaField);
	///возвращает информацию по тексту
	void GetTextInfo(unsigned int uiTextIndex, SS::Interface::Core::NdxSE::TNdxMetaFieldCollection* pNdxMetaFieldCollection);
	///возвращает количество текстов в базе
	unsigned int GetTextsNumber(void);
	///возвращает путь текста по индексу
	void GetTextPath(unsigned int uiTextIndex, std::wstring* pPath);
	///возвращает размер текста по индексу
	unsigned int GetTextSize(unsigned int uiTextIndex);
	///удаляет текст из базы
	void DeleteText(unsigned int uiTextIndex);
	///проверяет удален ли текст из базы
	unsigned int IsTextDeleted(unsigned int uiTextIndex);
	///заполняет структуру заголовков по индексу текста
	void GetHeadersStructure(unsigned int uiTextIndex, std::wstring* pwsHeaderStructure);
	///заполняет структуру таблиц по индексу текста
	void GetTablesStructure(unsigned int uiTextIndex, std::wstring* pwsTablesStructure) ;
	//--------------Eof-ITextsInfo

	//--------------ITextsPathsInfo
	//заполняет в pTextPathChunk по смещению узла информацию по узлу
	void FillTextPathChunk(SS::Interface::Core::NdxSE::STextPathChunk* pTextPathChunk);
	//заполняет по pTextPathChunk смещения детей узла pChildsOffsetsCollection
	void GetTextPathChunkChildsOffsets(
		SS::Interface::Core::NdxSE::STextPathChunk* pTextPathChunk, 
		std::vector<unsigned int>* pChildsOffsetsCollection);
	///заполняет по pTextPathChunk смещения родителя узла pParentOffset
	void GetTextPathChunkParentOffset(
		SS::Interface::Core::NdxSE::STextPathChunk* pTextPathChunk, 
		unsigned int* pParentOffset);
	///удаляет узел вместе с детьми
	void DeleteTextPathChunk(SS::Interface::Core::NdxSE::STextPathChunk* pTextPathChunk);
	///проверяет есть ли путь в дереве путей текстов
	unsigned int IsTextPathExist(const wchar_t* pwcszTextPath);
	///удаляет текст по пути
	void DeleteTextByPath(const wchar_t* pwcszTextPath);
	//--------------Eof-ITextsPathsInfo

	///инициализирует менеджер процесса индексации
	void Init(SS::Core::NdxSE::NdxProcessor::Storages::CStorageManager* pStorageManager);

protected:
	///менеджер логических хранилищ индексной базы
	SS::Core::NdxSE::NdxProcessor::Storages::CStorageManager* m_pStorageManager;

};

//--------------------------------------------------------------------//

}
}
}
}
}