#pragma once

#include ".\node_containers.h"
#include "../ASDInterface/INdxDataStorage.h"
#include "../ASDInterface/TNdxSearchEngineTypes.h"
#include ".\const.h"
#include ".\basis_storage.h"
#include ".\texts_field_storage.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Storages
{

//--------------------------------------------------------------------//

///работа со строками
class CStringService
{
public:
	///сравнивает строки
	inline static bool StrEqual(const wchar_t* sz1, const wchar_t* sz2);
};

//--------------------------------------------------------------------//

///узел дерево путей текстов
class CTextPathNode 
	: public SS::Core::NdxSE::NdxProcessor::Containers::CVecTreeNode<SS::Interface::Core::NdxSE::STextPathChunk*>
{
public:
	CTextPathNode(const wchar_t* pMyPathChunk, unsigned int uiMyIndex = SS::Core::NdxSE::NdxProcessor::EMPTY_VALUE);
	virtual ~CTextPathNode(void);

	///добавляет ребенка данному узлу
	CTextPathNode* AddChild(const wchar_t* pMyPathChunk, unsigned int uiMyIndex = SS::Core::NdxSE::NdxProcessor::EMPTY_VALUE);
	///кладет узел на диск дереву
	void MoveDown(SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* pNdxFile);

protected:
	///добавляет ребенка данному узлу
	SS::Core::NdxSE::NdxProcessor::Containers::CVecTreeNode<SS::Interface::Core::NdxSE::STextPathChunk*>* 
		AddChild(SS::Core::NdxSE::NdxProcessor::Containers::CVecTreeNode<SS::Interface::Core::NdxSE::STextPathChunk*>* pChildNode){TO_CONSOLE(L"Proxed"); return NULL;};
};

//--------------------------------------------------------------------//

//структура хранения узлов:
//	сам узел:
//		флаг удаления - 1байт
//		смещение на родителя - 4байта
//		смещение на смещения детей - 4байта
//		идентификатор узла - 4байта
//		размер куска пути текста  - 4байта
//		кускок пути текста - размер куска пути текста
//	смещения детей:
//		количество детей - 4байта
//		смещения детей - 4байта*количество детей 

///дерево путей текстов
class CTextsPathsTreeStorage : public CBasisStorage
{
public:
	CTextsPathsTreeStorage(SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* pNdxCashedFile);
	virtual ~CTextsPathsTreeStorage(void);

	///осуществляет подготовку хранилища индексацию
	void PrepareIndexing(void){};
	///выполняет действия хранилища по окончании индексации
	void CompleteIndexing(void){};
	///выполняет действия хранилища по окончании индексации документа
	void CompleteDocumentIndexing(void){};
	
	///создает дерево путей по информации по текстам
	void BuildTree(CTextsFieldsStorage* pTextsInfo);
	///добавляет путь к дереву
	void AddTextPath(unsigned int uiTextIndex, wchar_t* pszTextPath);
	///рубит путь текста на куски
	void HackTextPathOnChunks(wchar_t* pszTextPath, vector<wstring>* pChunksCollection);
	///кладет дерево на диск дереву
	void MoveDown(void);
	///возвращает информацию по смещению узла
	void FillTextPathChunk(SS::Interface::Core::NdxSE::STextPathChunk* pTextPathChunk);
	///возвращает смещения детей узла
	void GetChildsOffsets(SS::Interface::Core::NdxSE::STextPathChunk* pTextPathChunk, vector<unsigned int>* pChildsOffsetsCollection);
	///возвращает смещение родителя узла
	void GetParentOffset(SS::Interface::Core::NdxSE::STextPathChunk* pTextPathChunk, unsigned int* pParentOffset);
	///удаляет узел вместе с детьми
	void DeleteTextPathChunk(SS::Interface::Core::NdxSE::STextPathChunk* pTextPathChunk, std::vector<unsigned int>* pDeletedTextsNumbers);
	///проверяет удален ли данный узел
	bool IsTextPathChunkDeleted(SS::Interface::Core::NdxSE::STextPathChunk* pTextPathChunk);
	///проверяет есть ли путь в дереве путей текстов
	bool IsTextPathExist(const wchar_t* pszTextPath);
	///удаляет текст по пути
	void DeleteTextByPath(const wchar_t* pszTextPath, std::vector<unsigned int>* pDeletedTextsNumbers);
		
protected:
	///рутовый узел дерева
	CTextPathNode m_RootPathNode;
	///физическое хранилище данных
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* m_pNdxCashedFile;
};

//--------------------------------------------------------------------//

}
}
}
}
}