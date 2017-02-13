#pragma once

#include "../ASDInterface/INdxDataStorage.h"
#include "../ASCInterface/ICommonContainers.h"
#include ".\basis_storage.h"

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

///работает с информацией по текстам
class CTextsInfoStorage : public CBasisStorage
{
public:
	CTextsInfoStorage(SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* pNdxVectorStorage);
	virtual ~CTextsInfoStorage(void);
	
	///осуществляет подготовку хранилища индексацию
	void PrepareIndexing(void){};
	///выполняет действия хранилища по окончании индексации
	void CompleteIndexing(void){};
	///выполняет действия хранилища по окончании индексации документа
	void CompleteDocumentIndexing(void){};

	///добавляет информацию по тексту
	unsigned int AddTextInfo(SS::Interface::Core::CommonContainers::ITextFeature* pTextFeature);
	///возвращает информацию по тексту
	void GetTextInfo(unsigned int uiTextIndex, SS::Interface::Core::CommonContainers::ITextFeature* pTextFeature);
	///удаляет текст
	void DeleteText(unsigned int uiTextIndex);
	///проверяет удален ли текст
	bool IsTextDeleted(unsigned int uiTextIndex);
	///возвращает количество текстов
	unsigned int GetTextsNumber(void){return m_pNdxVectorStorage->GetIndexesNumber();};

protected:
	///физическое векторное хранилище
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* m_pNdxVectorStorage;
};

//--------------------------------------------------------------------//

}
}
}
}
}