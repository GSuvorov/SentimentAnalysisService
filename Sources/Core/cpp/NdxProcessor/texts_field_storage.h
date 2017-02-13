#pragma once

#include ".\table_storage.h"
#include ".\coordinate_counter.h"
#include ".\res_containers.h"
#include "..\ASCInterface\TCoommonTypes.h"

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

///хранилище полей текстов
class CTextsFieldsStorage : public CTableStorage
{
public:
	CTextsFieldsStorage(SS::Interface::Core::NdxSE::NdxDataStorage::INdxTableStorage* pNdxTableStorage);
	virtual ~CTextsFieldsStorage(void); 

	///осуществляет подготовку хранилища индексацию
	void PrepareIndexing(void);
	///выполняет действия хранилища по окончании индексации
	void CompleteIndexing(void){};
	///выполняет действия хранилища по окончании индексации документа
	void CompleteDocumentIndexing(void){};

	///добавляет информацию по тексту
	void AddTextInfo(SS::Interface::Core::NdxSE::TNdxMetaFieldCollection* pNdxMetaFieldCollection);
	///возвращает информацию о тексте, тип информации указывается в CNdxMetaField
	void GetTextInfo(unsigned int uiTextIndex, SS::Interface::Core::NdxSE::CNdxMetaField* pNdxMetaField);
	///возвращает информацию о тексте, тип информации указывается в CNdxMetaField
	void GetTextInfo(unsigned int uiTextIndex, SS::Interface::Core::NdxSE::TNdxMetaFieldCollection* pNdxMetaFieldCollection);
	///возвращает количество текстов
	unsigned int GetTextsNumber(void);
	///удаляет текст
	void DeleteText(unsigned int uiTextIndex);
	///выполняет поиск текстов по фильтру
	void GetTexts(SS::Interface::Core::NdxSE::TNdxMetaFieldCollection* pNdxMetaFieldCollection, 
		SS::Core::NdxSE::NdxProcessor::Containers::CFindResultCollection* pFindResults, 
		SS::Core::Types::IndexationSearch::TSearchParams* pSearchParams);

protected:
	///счетчик координат
	SS::Core::NdxSE::NdxProcessor::Indexing::CCoordinateCounter m_CoordinateCounter;
};

//--------------------------------------------------------------------//

}
}
}
}
}
